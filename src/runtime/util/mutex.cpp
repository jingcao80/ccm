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

#include "Mutex.h"
#include "ccmlogger.h"
#include <errno.h>
#include <unistd.h>
#include <linux/futex.h>
#include <sys/syscall.h>

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
                        Logger::E("NativeMutex", "futex wait failed");
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
                Logger::E("NativeMutex", "Unexpected state_ in unlock %d", curState);
            }
        } while (!done);
    }
}

}
