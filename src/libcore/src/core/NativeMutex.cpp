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

//----------------------------------------------------------------------------

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

}
}
