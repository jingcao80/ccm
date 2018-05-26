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

#include "core/NativeMutex.h"
#include "core/NativeThread.h"
#include "core/NativeTimeUtils.h"
#include <ccmlogger.h>
#include <errno.h>
#include <linux/futex.h>

namespace ccm {
namespace core {

pid_t GetTid()
{
#if defined(__BIONIC__)
    return gettid();
#else
    return syscall(__NR_gettid);
#endif
}

static inline uint64_t SafeGetTid(
    /* [in] */ const NativeThread* self)
{
    if (self != nullptr) {
        return static_cast<uint64_t>(self->GetTid());
    }
    else {
        return static_cast<uint64_t>(GetTid());
    }
}

BaseMutex::BaseMutex(
    /* [in] */ const String& name,
    /* [in] */ LockLevel level)
    : mLevel(level)
    , mName(name)
{}

void BaseMutex::RegisterAsLocked(
    /* [in] */ NativeThread* self)
{
    if (UNLIKELY(self == nullptr)) {
        return;
    }
    // Don't record monitors as they are outside the scope of analysis. They may be inspected off of
    // the monitor list.
    if (mLevel != kMonitorLock) {
        self->SetHeldMutex(mLevel, this);
    }
}

void BaseMutex::RegisterAsUnlocked(
    /* [in] */ NativeThread* self)
{
    if (UNLIKELY(self == nullptr)) {
        return;
    }
    if (mLevel != kMonitorLock) {
        self->SetHeldMutex(mLevel, nullptr);
    }
}

void BaseMutex::CheckSafeToWait(
    /* [in] */ NativeThread* self)
{}

//----------------------------------------------------------------------------

NativeMutex::NativeMutex(
    /* [in] */ const String& name,
    /* [in] */ LockLevel level,
    /* [in] */ Boolean recursive)
    : BaseMutex(name, level)
    , mRecursive(recursive)
    , mRecursionCount(0)
{
    CHECK(mState.LoadRelaxed() == 0);
    CHECK(mNumContenders.LoadRelaxed() == 0);
    mExclusiveOwner = 0;
}

NativeMutex::~NativeMutex()
{
    if (mState.LoadRelaxed() != 0) {
        Logger::E("NativeMutex", "destroying mutex with owner: %llu", mExclusiveOwner);
    }
    else {
        if (mExclusiveOwner != 0) {
            Logger::E("NativeMutex", "unexpectedly found an owner on unlocked mutex %s",
                    mName.string());
        }
        if (mNumContenders.LoadSequentiallyConsistent() != 0) {
            Logger::E("NativeMutex", "unexpectedly found a contender on mutex %s",
                    mName.string());
        }
    }
}

void NativeMutex::ExclusiveLock(
    /* [in] */ NativeThread* self)
{
    CHECK(self == nullptr || self == NativeThread::Current());
    if (!mRecursive || !IsExclusiveHeld(self)) {
        Boolean done = false;
        do {
            int32_t curState = mState.LoadRelaxed();
            if (LIKELY(curState == 0)) {
                // Change state from 0 to 1 and impose load/store ordering appropriate for lock acquisition.
                done = mState.CompareExchangeWeakAcquire(0 /* cur_state */, 1 /* new state */);
            }
            else {
                mNumContenders++;
                if (futex(mState.Address(), FUTEX_WAIT, 1, nullptr, nullptr, 0) != 0) {
                    // EAGAIN and EINTR both indicate a spurious failure, try again from the beginning.
                    // We don't use TEMP_FAILURE_RETRY so we can intentionally retry to acquire the lock.
                    if ((errno != EAGAIN) && (errno != EINTR)) {
                        Logger::E("NativeMutex", "futex wait failed for %s", mName.string());
                    }
                }
                mNumContenders--;
            }
        } while (!done);

        mExclusiveOwner = SafeGetTid(self);
        RegisterAsLocked(self);
    }
    mRecursionCount++;
}

void NativeMutex::ExclusiveUnlock(
    /* [in] */ NativeThread* self)
{
    mRecursionCount--;
    if (!mRecursive || mRecursionCount == 0) {
        RegisterAsUnlocked(self);
        Boolean done = false;
        do {
            int32_t curState = mState.LoadRelaxed();
            if (LIKELY(curState == 1)) {
                // We're no longer the owner.
                mExclusiveOwner = 0;
                // Change state to 0 and impose load/store ordering appropriate for lock release.
                // Note, the relaxed loads below musn't reorder before the CompareExchange.
                // TODO: the ordering here is non-trivial as state is split across 3 fields, fix by placing
                // a status bit into the state on contention.
                done =  mState.CompareExchangeWeakSequentiallyConsistent(curState, 0 /* new state */);
                if (LIKELY(done)) {
                    // Wake a contender.
                    if (UNLIKELY(mNumContenders.LoadRelaxed() > 0)) {
                        futex(mState.Address(), FUTEX_WAKE, 1, nullptr, nullptr, 0);
                    }
                }
            }
            else {
                Logger::E("NativeMutex", "Unexpected state_ in unlock %d for %s", curState, mName.string());
            }
        } while(!done);
    }
}

Boolean NativeMutex::IsExclusiveHeld(
    /* [in] */ const NativeThread* self) const
{
    CHECK(self == nullptr || self == Thread::Current());
    Boolean result = (GetExclusiveOwnerTid() == SafeGetTid(self));
    return result;
}

//----------------------------------------------------------------------------

NativeReaderWriterMutex::NativeReaderWriterMutex(
    /* [in] */ const String& name,
    /* [in] */ LockLevel level)
    : BaseMutex(name, level)
    , mState(0)
    , mExclusiveOwner(0)
    , mNumPendingReaders(0)
    , mNumPendingWriters(0)
{  // NOLINT(whitespace/braces)
}

//----------------------------------------------------------------------------

NativeConditionVariable::NativeConditionVariable(
    /* [in] */ const String& name,
    /* [in] */ NativeMutex& mutex)
    : mName(name)
    , mGuard(mutex)
    , mNumWaiters(0)
{
    CHECK(mSequence.LoadRelaxed() == 0);
}

void NativeConditionVariable::Broadcast(
    /* [in] */ NativeThread* self)
{
    CHECK(self == nullptr || self == NativeThread::Current());
    // TODO: enable below, there's a race in thread creation that causes false failures currently.
    // guard_.AssertExclusiveHeld(self);
    CHECK(mGuard.GetExclusiveOwnerTid() == SafeGetTid(self));
    if (mNumWaiters > 0) {
        mSequence++;  // Indicate the broadcast occurred.
        Boolean done = false;
        do {
            int32_t curSequence = mSequence.LoadRelaxed();
            // Requeue waiters onto mutex. The waiter holds the contender count on the mutex high ensuring
            // mutex unlocks will awaken the requeued waiter thread.
            done = futex(mSequence.Address(), FUTEX_CMP_REQUEUE, 0,
                       reinterpret_cast<const timespec*>(std::numeric_limits<int32_t>::max()),
                       mGuard.mState.Address(), curSequence) != -1;
            if (!done) {
                if (errno != EAGAIN && errno != EINTR) {
                    Logger::E("NativeConditionVariable", "futex cmp requeue failed for %s",
                            mName.string());
                }
            }
        } while (!done);
    }
}

void NativeConditionVariable::Signal(
    /* [in] */ NativeThread* self)
{
    CHECK(self == nullptr || self == NativeThread::Current());
    mGuard.AssertExclusiveHeld(self);
    if (mNumWaiters > 0) {
        mSequence++; // Indicate a signal occurred.
        // Futex wake 1 waiter who will then come and in contend on mutex. It'd be nice to requeue them
        // to avoid this, however, requeueing can only move all waiters.
        int numWoken = futex(mSequence.Address(), FUTEX_WAKE, 1, nullptr, nullptr, 0);
        // Check something was woken or else we changed sequence_ before they had chance to wait.
        CHECK((numWoken == 0) || (numWoken == 1));
    }
}

void NativeConditionVariable::Wait(
    /* [in] */ NativeThread* self)
{
    mGuard.CheckSafeToWait(self);
    WaitHoldingLocks(self);
}

void NativeConditionVariable::WaitHoldingLocks(
    /* [in] */ NativeThread* self)
{
    CHECK(self == nullptr || self == Thread::Current());
    mGuard.AssertExclusiveHeld(self);
    unsigned int oldRecursionCount = mGuard.mRecursionCount;
    mNumWaiters++;
    // Ensure the Mutex is contended so that requeued threads are awoken.
    mGuard.mNumContenders++;
    mGuard.mRecursionCount = 1;
    int32_t curSequence = mSequence.LoadRelaxed();
    mGuard.ExclusiveUnlock(self);
    if (futex(mSequence.Address(), FUTEX_WAIT, curSequence, nullptr, nullptr, 0) != 0) {
        // Futex failed, check it is an expected error.
        // EAGAIN == EWOULDBLK, so we let the caller try again.
        // EINTR implies a signal was sent to this thread.
        if ((errno != EINTR) && (errno != EAGAIN)) {
            Logger::E("NativeConditionVariable", "futex wait failed for %s", mName.string());
        }
    }
    mGuard.ExclusiveLock(self);
    CHECK(mNumWaiters >= 0);
    mNumWaiters--;
    // We awoke and so no longer require awakes from the guard_'s unlock.
    CHECK(mGuard.mNumContenders.LoadRelaxed() >= 0);
    mGuard.mNumContenders--;
    mGuard.mRecursionCount = oldRecursionCount;
}

Boolean NativeConditionVariable::TimedWait(
    /* [in] */ NativeThread* self,
    /* [in] */ int64_t ms,
    /* [in] */ int32_t ns)
{
    CHECK(self == nullptr || self == Thread::Current());
    Boolean timedOut = false;
    mGuard.AssertExclusiveHeld(self);
    mGuard.CheckSafeToWait(self);
    unsigned int oldRecursionCount = mGuard.mRecursionCount;
    timespec rel_ts;
    InitTimeSpec(false, CLOCK_REALTIME, ms, ns, &rel_ts);
    mNumWaiters++;
    // Ensure the Mutex is contended so that requeued threads are awoken.
    mGuard.mNumContenders++;
    mGuard.mRecursionCount = 1;
    int32_t curSequence = mSequence.LoadRelaxed();
    mGuard.ExclusiveUnlock(self);
    if (futex(mSequence.Address(), FUTEX_WAIT, curSequence, &rel_ts, nullptr, 0) != 0) {
        if (errno == ETIMEDOUT) {
            // Timed out we're done.
            timedOut = true;
        }
        else if ((errno == EAGAIN) || (errno == EINTR)) {
            // A signal or ConditionVariable::Signal/Broadcast has come in.
        }
        else {
            Logger::E("NativeConditionVariable",
                    "timed futex wait failed for %s", mName.string());
        }
    }
    mGuard.ExclusiveLock(self);
    CHECK(mNumWaiters >= 0);
    mNumWaiters--;
    // We awoke and so no longer require awakes from the guard_'s unlock.
    CHECK(mGuard.mNumContenders.LoadRelaxed() >= 0);
    mGuard.mNumContenders--;
    mGuard.mRecursionCount = oldRecursionCount;
    return timedOut;
}

//----------------------------------------------------------------------------

NativeMutatorMutex* Locks::sMutatorLock = nullptr;
NativeMutex* Locks::sRuntimeShutdownLock = nullptr;
NativeMutex* Locks::sThreadListLock = nullptr;
NativeMutex* Locks::sAllocatedMonitorIdsLock = nullptr;
NativeMutex* Locks::sThreadSuspendCountLock = nullptr;

void Locks::Init()
{
    CHECK(sMutatorLock == nullptr);
    sMutatorLock = new NativeMutatorMutex(String("mutator lock"), kMutatorLock);

    CHECK(sRuntimeShutdownLock == nullptr);
    sRuntimeShutdownLock = new NativeMutex(String("runtime shutdown lock"), kRuntimeShutdownLock);

    CHECK(sThreadListLock == nullptr);
    sThreadListLock = new NativeMutex(String("thread list lock"), kThreadListLock);

    CHECK(sAllocatedMonitorIdsLock == nullptr);
    sAllocatedMonitorIdsLock = new NativeMutex(String("allocated monitor ids lock"), kMonitorPoolLock);

    CHECK(sThreadSuspendCountLock == nullptr);
    sThreadSuspendCountLock = new NativeMutex(String("thread suspend count lock"), kThreadSuspendCountLock);
}

}
}
