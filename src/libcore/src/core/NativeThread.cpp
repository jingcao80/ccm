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
#include "core/NativeTimeUtils.h"
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

NativeThreadState NativeThread::SetState(
    /* [in] */ NativeThreadState newState)
{
    // Should only be used to change between suspended states.
    // Cannot use this code to change into or from Runnable as changing to Runnable should
    // fail if old_state_and_flags.suspend_request is true and changing from Runnable might
    // miss passing an active suspend barrier.
    CHECK(newState != kRunnable);
    union StateAndFlags oldStateAndFlags;
    oldStateAndFlags.mAsInt = mTls32.mStateAndFlags.mAsInt;
    CHECK(oldStateAndFlags.mAsStruct.mState != kRunnable);
    mTls32.mStateAndFlags.mAsStruct.mState = newState;
    return static_cast<NativeThreadState>(oldStateAndFlags.mAsStruct.mState);
}

Boolean NativeThread::IsSuspended() const
{
    union StateAndFlags stateAndFlags;
    stateAndFlags.mAsInt = mTls32.mStateAndFlags.mAsInt;
    return stateAndFlags.mAsStruct.mState != kRunnable &&
            (stateAndFlags.mAsStruct.mFlags & kSuspendRequest) != 0;
}

Boolean NativeThread::ModifySuspendCount(
    /* [in] */ NativeThread* self,
    /* [in] */ Integer delta,
    /* [in] */ AtomicInteger* suspendBarrier,
    /* [in] */ Boolean forDebugger)
{
    if (delta > 0 && suspendBarrier != nullptr) {
        // When delta > 0 (requesting a suspend), ModifySuspendCountInternal() may fail either if
        // active_suspend_barriers is full or we are in the middle of a thread flip. Retry in a loop.
        while (true) {
            if (LIKELY(ModifySuspendCountInternal(self, delta, suspendBarrier, forDebugger))) {
                return true;
            }
            else {
                // Failure means the list of active_suspend_barriers is full or we are in the middle of a
                // thread flip, we should release the thread_suspend_count_lock_ (to avoid deadlock) and
                // wait till the target thread has executed or Thread::PassActiveSuspendBarriers() or the
                // flip function. Note that we could not simply wait for the thread to change to a suspended
                // state, because it might need to run checkpoint function before the state change or
                // resumes from the resume_cond_, which also needs thread_suspend_count_lock_.
                //
                // The list of active_suspend_barriers is very unlikely to be full since more than
                // kMaxSuspendBarriers threads need to execute SuspendAllInternal() simultaneously, and
                // target thread stays in kRunnable in the mean time.
                Locks::sThreadSuspendCountLock->ExclusiveUnlock(self);
                NanoSleep(100000);
                Locks::sThreadSuspendCountLock->ExclusiveLock(self);
            }
        }
    }
    else {
        return ModifySuspendCountInternal(self, delta, suspendBarrier, forDebugger);
    }
}

void NativeThread::TransitionToSuspendedAndRunCheckpoints(
    /* [in] */ NativeThreadState newState)
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

// Attempt to rectify locks so that we dump thread list with required locks before exiting.
static void UnsafeLogFatalForSuspendCount(
    /* [in] */ NativeThread* self,
    /* [in] */ NativeThread* thread)
{}

Boolean NativeThread::ModifySuspendCountInternal(
    /* [in] */ NativeThread* self,
    /* [in] */ Integer delta,
    /* [in] */ AtomicInteger* suspendBarrier,
    /* [in] */ Boolean forDebugger)
{
    if (UNLIKELY(delta < 0 && mTls32.mSuspendCount <= 0)) {
        UnsafeLogFatalForSuspendCount(self, this);
        return false;
    }

    uint16_t flags = kSuspendRequest;
    if (delta > 0 && suspendBarrier != nullptr) {
        uint32_t availableBarrier = kMaxSuspendBarriers;
        for (uint32_t i = 0; i < kMaxSuspendBarriers; ++i) {
            if (mTlsPtr.mActiveSuspendBarriers[i] == nullptr) {
                availableBarrier = i;
                break;
            }
        }
        if (availableBarrier == kMaxSuspendBarriers) {
            // No barrier spaces available, we can't add another.
            return false;
        }
        mTlsPtr.mActiveSuspendBarriers[availableBarrier] = suspendBarrier;
        flags |= kActiveSuspendBarrier;
    }

    mTls32.mSuspendCount += delta;
    if (forDebugger) {
        mTls32.mDebugSuspendCount += delta;
    }

    if (mTls32.mSuspendCount == 0) {
        AtomicClearFlag(kSuspendRequest);
    }
    else {
        // Two bits might be set simultaneously.
        mTls32.mStateAndFlags.mAsAtomicInt.FetchAndOrSequentiallyConsistent(flags);
        TriggerSuspend();
    }
    return true;
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
    /* [in] */ NativeThreadState newState)
{
    CHECK(this == Current());
    // Change to non-runnable state, thereby appearing suspended to the system.
    TransitionToSuspendedAndRunCheckpoints(newState);
    // Mark the release of the share of the mutator_lock_.
    Locks::sMutatorLock->TransitionFromRunnableToSuspended(this);
    // Once suspended - check the active suspend barrier flag
    PassActiveSuspendBarriers();
}

NativeThreadState NativeThread::TransitionFromSuspendedToRunnable()
{
    union StateAndFlags oldStateAndFlags;
    oldStateAndFlags.mAsInt = mTls32.mStateAndFlags.mAsInt;
    int16_t oldState = oldStateAndFlags.mAsStruct.mState;
    CHECK(static_cast<NativeThreadState>(oldState) != kRunnable);
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
    return static_cast<NativeThreadState>(oldState);
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
