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

#ifndef __CCM_CORE_NATIVETHREAD_H__
#define __CCM_CORE_NATIVETHREAD_H__

#include "core/NativeMonitor.h"
#include "core/NativeMutex.h"
#include <ccmtypes.h>
#include <pthread.h>

namespace ccm {
namespace core {

class NativeThread
{
public:
    static NativeThread* Current();

    String ShortDump() const;

    uint32_t GetThreadId() const;

    pid_t GetTid() const;

    static void Startup();

    void Notify();

    NativeMutex* GetWaitMutex() const;

    NativeConditionVariable* GetWaitConditionVariable() const;

    NativeMonitor* GetWaitMonitor() const;

    NativeThread* GetWaitNext() const;

    void SetWaitNext(
        /* [in] */ NativeThread* next);

    void SetHeldMutex(
        /* [in] */ LockLevel level,
        /* [in] */ BaseMutex* mutex);

private:
    void NotifyLocked(
        /* [in] */ NativeThread* self);

    static void ThreadExitCallback(
        /* [in] */ void* arg);

private:
    static Boolean sIsStarted;

    static pthread_key_t sPthreadKeySelf;

    struct PACKED(4) tls_32bit_sized_values
    {
        explicit tls_32bit_sized_values()
            : mThinLockThreadId(0)
            , mTid(0)
            , mThreadExitCheckCount(0)
        {}

        // Thin lock thread id. This is a small integer used by the thin lock implementation.
        // This is not to be confused with the native thread's tid, nor is it the value returned
        // by java.lang.Thread.getId --- this is a distinct value, used only for locking. One
        // important difference between this id and the ids visible to managed code is that these
        // ones get reused (to ensure that they fit in the number of bits available).
        uint32_t mThinLockThreadId;

        // System thread id.
        uint32_t mTid;

        // How many times has our pthread key's destructor been called?
        uint32_t mThreadExitCheckCount;
    } mTls32;

    struct PACKED(sizeof(void*)) tls_ptr_sized_values
    {
        tls_ptr_sized_values()
            : mWaitNext(nullptr)
        {
            memset(&mHeldMutexes[0], 0, sizeof(mHeldMutexes));
        }

        // The next thread in the wait set this thread is part of or null if not waiting.
        NativeThread* mWaitNext;

        // Support for Mutex lock hierarchy bug detection.
        BaseMutex* mHeldMutexes[kLockLevelCount];
    } mTlsPtr;

    // Guards the 'interrupted_' and 'wait_monitor_' members.
    NativeMutex* mWaitMutex;

    // Condition variable waited upon during a wait.
    NativeConditionVariable* mWaitCond;
    // Pointer to the monitor lock we're currently waiting on or null if not waiting.
    NativeMonitor* mWaitMonitor;
};

inline uint32_t NativeThread::GetThreadId() const
{
    return mTls32.mThinLockThreadId;
}

inline pid_t NativeThread::GetTid() const
{
    return mTls32.mTid;
}

inline NativeMutex* NativeThread::GetWaitMutex() const
{
    return mWaitMutex;
}

inline NativeConditionVariable* NativeThread::GetWaitConditionVariable() const
{
    return mWaitCond;
}

inline NativeMonitor* NativeThread::GetWaitMonitor() const
{
    return mWaitMonitor;
}

inline NativeThread* NativeThread::GetWaitNext() const
{
    return mTlsPtr.mWaitNext;
}

inline void NativeThread::SetWaitNext(
    /* [in] */ NativeThread* next)
{
    mTlsPtr.mWaitNext = next;
}

}
}

#endif // __CCM_CORE_NATIVETHREAD_H__
