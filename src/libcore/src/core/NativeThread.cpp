//=========================================================================
// Copyright (C) 2018 The C++ Component Model(CCM) Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "core/NativeThread.h"
#include <ccmlogger.h>

namespace ccm {
namespace core {

Boolean NativeThread::sIsStarted = false;
pthread_key_t NativeThread::sPthreadKeySelf;
NativeConditionVariable* NativeThread::sResumeCond = nullptr;

NativeThread* NativeThread::Current()
{
    if (!sIsStarted) {
        return nullptr;
    }
    else {
        void* thread = pthread_getspecific(NativeThread::sPthreadKeySelf);
        return reinterpret_cast<NativeThread*>(thread);
    }
}

String NativeThread::ShortDump() const
{
    return String();
}

void NativeThread::TransitionToSuspendedAndRunCheckpoints(
    /* [in] */ ThreadState newState)
{
    CHECK(newState != kRunnable);
    CHECK(GetState() == kRunnable);
    union StateAndFlags oldStateAndFlags;
    union StateAndFlags newStateAndFlags;
    while (true) {
        oldStateAndFlags.mAsInt = mTls32.mStateAndFlags.mAsInt;
        newStateAndFlags.mAsStruct.mFlags = oldStateAndFlags.mAsStruct.mFlags;
        newStateAndFlags.mAsStruct.mState = newState;
        // CAS the value with a memory ordering.
        Boolean done = mTls32.mStateAndFlags.mAsAtomicInt.CompareExchangeWeakRelease(
                oldStateAndFlags.mAsInt, newStateAndFlags.mAsInt);
        if (LIKELY(done)) {
            break;
        }
    }
}

void NativeThread::PassActiveSuspendBarriers()
{
    while (true) {
        uint16_t currentFlags = mTls32.mStateAndFlags.mAsStruct.mFlags;
        if (LIKELY((currentFlags & kActiveSuspendBarrier) == 0)) {
            break;
        }
        else {
            PassActiveSuspendBarriers(this);
        }
    }
}

Boolean NativeThread::PassActiveSuspendBarriers(
    /* [in] */ NativeThread* self)
{
    // Grab the suspend_count lock and copy the current set of
    // barriers. Then clear the list and the flag. The ModifySuspendCount
    // function requires the lock so we prevent a race between setting
    // the kActiveSuspendBarrier flag and clearing it.
    AtomicInteger* passBarriers[kMaxSuspendBarriers];
    {
        NativeMutex::AutoLock lock(self, *Locks::sThreadSuspendCountLock);
        if (!ReadFlag(kActiveSuspendBarrier)) {
            // quick exit test: the barriers have already been claimed - this is
            // possible as there may be a race to claim and it doesn't matter
            // who wins.
            // All of the callers of this function (except the SuspendAllInternal)
            // will first test the kActiveSuspendBarrier flag without lock. Here
            // double-check whether the barrier has been passed with the
            // suspend_count lock.
            return false;
        }

        for (uint32_t i = 0; i < kMaxSuspendBarriers; ++i) {
            passBarriers[i] = mTlsPtr.mActiveSuspendBarriers[i];
            mTlsPtr.mActiveSuspendBarriers[i] = nullptr;
        }
        AtomicClearFlag(kActiveSuspendBarrier);
    }

    uint32_t barrierCount = 0;
    for (uint32_t i = 0; i < kMaxSuspendBarriers; i++) {
        AtomicInteger* pendingThreads = passBarriers[i];
        if (pendingThreads != nullptr) {
            Boolean done = false;
            do {
                int32_t curVal = pendingThreads->LoadRelaxed();
                CHECK(curVal > 0);
                // Reduce value by 1.
                done = pendingThreads->CompareExchangeWeakRelaxed(curVal, curVal - 1);
                if (done && (curVal - 1) == 0) {  // Weak CAS may fail spuriously.
                    futex(pendingThreads->Address(), FUTEX_WAKE, -1, nullptr, nullptr, 0);
                }
            } while (!done);
            ++barrierCount;
        }
    }
    CHECK(barrierCount > 0);
    return true;
}

void NativeThread::TransitionFromRunnableToSuspended(
    /* [in] */ ThreadState newState)
{
    CHECK(this == Current());
    // Change to non-runnable state, thereby appearing suspended to the system.
    TransitionToSuspendedAndRunCheckpoints(newState);
    // Mark the release of the share of the mutator_lock_.
    Locks::sMutatorLock->TransitionFromRunnableToSuspended(this);
    // Once suspended - check the active suspend barrier flag
    PassActiveSuspendBarriers();
}

ThreadState NativeThread::TransitionFromSuspendedToRunnable()
{
    union StateAndFlags oldStateAndFlags;
    oldStateAndFlags.mAsInt = mTls32.mStateAndFlags.mAsInt;
    int16_t oldState = oldStateAndFlags.mAsStruct.mState;
    CHECK(static_cast<ThreadState>(oldState) != kRunnable);
    do {
        Locks::sMutatorLock->AssertNotHeld(this);  // Otherwise we starve GC..
        oldStateAndFlags.mAsInt = mTls32.mStateAndFlags.mAsInt;
        CHECK(oldStateAndFlags.mAsStruct.mState == oldState);
        if (LIKELY(oldStateAndFlags.mAsStruct.mFlags == 0)) {
            // Optimize for the return from native code case - this is the fast path.
            // Atomically change from suspended to runnable if no suspend request pending.
            union StateAndFlags newStateAndFlags;
            newStateAndFlags.mAsInt = oldStateAndFlags.mAsInt;
            newStateAndFlags.mAsStruct.mState = kRunnable;
            // CAS the value with a memory barrier.
            if (LIKELY(mTls32.mStateAndFlags.mAsAtomicInt.CompareExchangeWeakAcquire(
                    oldStateAndFlags.mAsInt, newStateAndFlags.mAsInt))) {
                // Mark the acquisition of a share of the mutator_lock_.
                Locks::sMutatorLock->TransitionFromSuspendedToRunnable(this);
                break;
            }
        }
        else if ((oldStateAndFlags.mAsStruct.mFlags & kActiveSuspendBarrier) != 0) {
            PassActiveSuspendBarriers(this);
        }
        else if ((oldStateAndFlags.mAsStruct.mFlags & kSuspendRequest) != 0) {
            // Wait while our suspend count is non-zero.

            // We pass null to the MutexLock as we may be in a situation where the
            // runtime is shutting down. Guarding ourselves from that situation
            // requires to take the shutdown lock, which is undesirable here.
            NativeThread* threadToPass = nullptr;
            NativeMutex::AutoLock lock(threadToPass, *Locks::sThreadSuspendCountLock);
            oldStateAndFlags.mAsInt = mTls32.mStateAndFlags.mAsInt;
            CHECK(oldStateAndFlags.mAsStruct.mState == oldState);
            while ((oldStateAndFlags.mAsStruct.mFlags & kSuspendRequest) != 0) {
                // Re-check when Thread::resume_cond_ is notified.
                NativeThread::sResumeCond->Wait(threadToPass);
                oldStateAndFlags.mAsInt = mTls32.mStateAndFlags.mAsInt;
                CHECK(oldStateAndFlags.mAsStruct.mState == oldState);
            }
            CHECK(GetSuspendCount() == 0);
        }
    } while (true);
    return static_cast<ThreadState>(oldState);
}

void NativeThread::ThreadExitCallback(
    /* [in] */ void* arg)
{
    NativeThread* self = reinterpret_cast<NativeThread*>(arg);
    if (self->mTls32.mThreadExitCheckCount == 0) {
        Logger::W("NativeThread", "Native thread exiting without having called DetachCurrentThread (maybe it's "
                "going to use a pthread_key_create destructor?): %s", self->ShortDump().string());
        CHECK(sIsStarted);
        pthread_setspecific(NativeThread::sPthreadKeySelf, self);
        self->mTls32.mThreadExitCheckCount = 1;
    }
    else {
        Logger::E("NativeThread", "Native thread exited without calling DetachCurrentThread: %s",
                self->ShortDump().string());
    }
}

void NativeThread::Startup()
{
    CHECK(!sIsStarted);
    sIsStarted = true;

    pthread_key_create(&sPthreadKeySelf, NativeThread::ThreadExitCallback);

    if (pthread_getspecific(sPthreadKeySelf) != nullptr) {
        Logger::E("NativeThread", "Newly created pthread TLS slot is not nullptr");
    }
}

void NativeThread::Notify()
{
    NativeThread* self = Current();
    NativeMutex::AutoLock lock(self, *mWaitMutex);
    NotifyLocked(self);
}

void NativeThread::NotifyLocked(
    /* [in] */ NativeThread* self)
{
    if (mWaitMonitor != nullptr) {
        mWaitCond->Signal(self);
    }
}

void NativeThread::SetHeldMutex(
    /* [in] */ LockLevel level,
    /* [in] */ BaseMutex* mutex)
{
    mTlsPtr.mHeldMutexes[level] = mutex;
}

}
}
