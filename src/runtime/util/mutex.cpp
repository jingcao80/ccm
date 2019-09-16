//=========================================================================
// Copyright (C) 2018 The C++ Component Model(COMO) Open Source Project
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

/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mutex.h"
#include "ccmlogger.h"
#include <limits>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <linux/futex.h>
#include <sys/syscall.h>

#include <stdio.h>

namespace ccm {

pid_t GetTid()
{
#if defined(__BIONIC__)
    return gettid();
#else
    return syscall(__NR_gettid);
#endif
}

#ifndef SYS_futex
#define SYS_futex __NR_futex
#endif

static inline int futex(volatile int *uaddr, int op, int val, const struct timespec *timeout,
                        volatile int *uaddr2, int val3)
{
    return syscall(SYS_futex, uaddr, op, val, timeout, uaddr2, val3);
}

static void InitTimeSpec(
    /* [in] */ bool absolute,
    /* [in] */ int clock,
    /* [in] */ int64_t ms,
    /* [in] */ int32_t ns,
    /* [in] */ timespec* ts)
{
    if (absolute) {
        clock_gettime(clock, ts);
    }
    else {
        ts->tv_sec = 0;
        ts->tv_nsec = 0;
    }

    int64_t end_sec = ts->tv_sec + ms / 1000;
    constexpr int32_t int32_max = std::numeric_limits<int32_t>::max();
    if (UNLIKELY(end_sec >= int32_max)) {
        // Either ms was intended to denote an infinite timeout, or we have a
        // problem. The former generally uses the largest possible millisecond
        // or nanosecond value.  Log only in the latter case.
        constexpr int64_t int64_max = std::numeric_limits<int64_t>::max();
        if (ms != int64_max && ms != int64_max / (1000 * 1000)) {
            Logger::V("NativeTimeUtils", "Note: end time exceeds INT32_MAX: %d", end_sec);
        }
        end_sec = int32_max - 1;  // Allow for increment below.
    }
    ts->tv_sec = end_sec;
    ts->tv_nsec = (ts->tv_nsec + (ms % 1000) * 1000000) + ns;

    // Catch rollover.
    if (ts->tv_nsec >= 1000000000L) {
        ts->tv_sec++;
        ts->tv_nsec -= 1000000000L;
    }
}

void Mutex::Lock()
{
    if (!mRecursive || mExclusiveOwner != GetTid()) {
        Boolean done = false;
        do {
            int32_t curState = mState.load(std::memory_order_relaxed);
            if (LIKELY(curState == 0)) {
                // Change state from 0 to 1 and impose load/store ordering appropriate for lock acquisition.
                int32_t expected = 0, desired = 1;
                done = mState.compare_exchange_weak(expected, desired, std::memory_order_acquire);
            }
            else {
                mNumContenders++;
                if (futex((int32_t*)&mState, FUTEX_WAIT, 1, nullptr, nullptr, 0) != 0) {
                    // EAGAIN and EINTR both indicate a spurious failure, try again from the beginning.
                    // We don't use TEMP_FAILURE_RETRY so we can intentionally retry to acquire the lock.
                    if ((errno != EAGAIN) && (errno != EINTR)) {
                        Logger::E("Mutex", "futex wait failed");
                    }
                }
                mNumContenders--;
            }
        } while (!done);

        mExclusiveOwner = GetTid();
    }
    mRecursionCount++;
}

void Mutex::Unlock()
{
    mRecursionCount--;
    if (!mRecursive || mRecursionCount == 0) {
        Boolean done = false;
        do {
            int32_t curState = mState.load(std::memory_order_relaxed);
            if (LIKELY(curState == 1)) {
                // We're no longer the owner.
                mExclusiveOwner = 0;
                // Change state to 0 and impose load/store ordering appropriate for lock release.
                // Note, the relaxed loads below musn't reorder before the CompareExchange.
                int32_t desired = 0;
                done =  mState.compare_exchange_weak(curState, desired, std::memory_order_seq_cst);
                if (LIKELY(done)) {
                    if (UNLIKELY(mNumContenders.load(std::memory_order_relaxed) > 0)) {
                        futex((int32_t*)&mState, FUTEX_WAKE, 1, nullptr, nullptr, 0);
                    }
                }
            }
            else {
                Logger::E("Mutex", "Unexpected state_ in unlock %d", curState);
            }
        } while (!done);
    }
}

//---------------------------------------------------------------------------------------------------

void Condition::Wait()
{
    unsigned int oldRecursionCount = mGuard.mRecursionCount;
    mNumWaiters++;
    // Ensure the Mutex is contended so that requeued threads are awoken.
    mGuard.mNumContenders++;
    mGuard.mRecursionCount = 1;
    int32_t curSequence = mSequence.load(std::memory_order_relaxed);
    mGuard.Unlock();
    if (futex((int32_t*)&mSequence, FUTEX_WAIT, curSequence, nullptr, nullptr, 0) != 0) {
        // Futex failed, check it is an expected error.
        // EAGAIN == EWOULDBLK, so we let the caller try again.
        // EINTR implies a signal was sent to this thread.
        if ((errno != EINTR) && (errno != EAGAIN)) {
            Logger::E("Condition", "futex wait failed");
        }
    }
    mGuard.Lock();
    CHECK(mNumWaiters >= 0);
    mNumWaiters--;
    CHECK(mGuard.mNumContenders.load(std::memory_order_relaxed) >= 0);
    mGuard.mNumContenders--;
    mGuard.mRecursionCount = oldRecursionCount;
}

Boolean Condition::TimedWait(
    /* [in] */ int64_t ms,
    /* [in] */ int32_t ns)
{
    Boolean timedOut = false;
    unsigned int oldRecursionCount = mGuard.mRecursionCount;
    timespec rel_ts;
    InitTimeSpec(false, CLOCK_REALTIME, ms, ns, &rel_ts);
    mNumWaiters++;
    // Ensure the Mutex is contended so that requeued threads are awoken.
    mGuard.mNumContenders++;
    mGuard.mRecursionCount = 1;
    int32_t curSequence = mSequence.load(std::memory_order_relaxed);
    mGuard.Unlock();
    if (futex((int32_t*)&mSequence, FUTEX_WAIT, curSequence, &rel_ts, nullptr, 0) != 0) {
        if (errno == ETIMEDOUT) {
            // Timed out we're done.
            timedOut = true;
        }
        else if ((errno == EAGAIN) || (errno == EINTR)) {
            // A signal or ConditionVariable::Signal/Broadcast has come in.
        }
        else {
            Logger::E("Condition",
                    "timed futex wait failed");
        }
    }
    mGuard.Lock();
    CHECK(mNumWaiters >= 0);
    mNumWaiters--;
    // We awoke and so no longer require awakes from the guard_'s unlock.
    CHECK(mGuard.mNumContenders.load(std::memory_order_relaxed) >= 0);
    mGuard.mNumContenders--;
    mGuard.mRecursionCount = oldRecursionCount;
    return timedOut;
}

void Condition::Signal()
{
    if (mNumWaiters > 0) {
        mSequence++; // Indicate a signal occurred.
        // Futex wake 1 waiter who will then come and in contend on mutex. It'd be nice to requeue them
        // to avoid this, however, requeueing can only move all waiters.
        int numWoken = futex((int32_t*)&mSequence, FUTEX_WAKE, 1, nullptr, nullptr, 0);
        // Check something was woken or else we changed sequence_ before they had chance to wait.
        CHECK((numWoken == 0) || (numWoken == 1));
    }
}

void Condition::SignalAll()
{
    if (mNumWaiters > 0) {
        mSequence++;  // Indicate the broadcast occurred.
        Boolean done = false;
        do {
            int32_t curSequence = mSequence.load(std::memory_order_relaxed);
            // Requeue waiters onto mutex. The waiter holds the contender count on the mutex high ensuring
            // mutex unlocks will awaken the requeued waiter thread.
            done = futex((int32_t*)&mSequence, FUTEX_CMP_REQUEUE, 0,
                       reinterpret_cast<const timespec*>(std::numeric_limits<int32_t>::max()),
                       (int32_t*)&mGuard.mState, curSequence) != -1;
            if (!done) {
                if (errno != EAGAIN && errno != EINTR) {
                    Logger::E("Condition", "futex cmp requeue failed");
                }
            }
        } while (!done);
    }
}

}
