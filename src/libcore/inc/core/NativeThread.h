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
#include "core/NativeThreadState.h"
#include <ccmtypes.h>
#include <pthread.h>

namespace ccm {
namespace core {

enum ThreadFlag
{
    kSuspendRequest = 1,    // If set implies that suspend_count_ > 0 and the Thread should enter the
                            // safepoint handler.
    kCheckpointRequest = 2,  // Request that the thread do some checkpoint work and then continue.
    kEmptyCheckpointRequest = 4,  // Request that the thread do empty checkpoint and then continue.
    kActiveSuspendBarrier = 8,  // Register that at least 1 suspend barrier needs to be passed.
};

class NativeThread
{
public:
    static NativeThread* Current();

    String ShortDump() const;

    ThreadState GetState() const;

    ThreadState SetState(
        /* [in] */ ThreadState newState);

    int GetSuspendCount() const;

    Boolean IsSuspended() const;

    // If delta > 0 and (this != self or suspend_barrier is not null), this function may temporarily
    // release thread_suspend_count_lock_ internally.
    Boolean ModifySuspendCount(
        /* [in] */ NativeThread* self,
        /* [in] */ Integer delta,
        /* [in] */ AtomicInteger* suspendBarrier,
        /* [in] */ Boolean forDebugger);

    // Transition from non-runnable to runnable state acquiring share on mutator_lock_.
    ThreadState TransitionFromSuspendedToRunnable();

    // Transition from runnable into a state where mutator privileges are denied. Releases share of
    // mutator lock.
    void TransitionFromRunnableToSuspended(
        /* [in] */ ThreadState newState);

    uint32_t GetThreadId() const;

    pid_t GetTid() const;

    static void Startup();

    void SetMonitorEnterObject(
        /* [in] */ NativeObject* obj);

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

    Boolean ReadFlag(
        /* [in] */ ThreadFlag flag) const;

    void AtomicClearFlag(
        /* [in] */ ThreadFlag flag);

    // Trigger a suspend check by making the suspend_trigger_ TLS value an invalid pointer.
    // The next time a suspend check is done, it will load from the value at this address
    // and trigger a SIGSEGV.
    void TriggerSuspend();

    void TransitionToSuspendedAndRunCheckpoints(
        /* [in] */ ThreadState newState);

    void PassActiveSuspendBarriers();

    Boolean ModifySuspendCountInternal(
        /* [in] */ NativeThread* self,
        /* [in] */ Integer delta,
        /* [in] */ AtomicInteger* suspendBarrier,
        /* [in] */ Boolean forDebugger);

    Boolean PassActiveSuspendBarriers(
        /* [in] */ NativeThread* self);

    // 32 bits of atomically changed state and flags. Keeping as 32 bits allows and atomic CAS to
    // change from being Suspended to Runnable without a suspend request occurring.
    union PACKED(4) StateAndFlags
    {
        StateAndFlags() {}
        struct PACKED(4)
        {
            // Bitfield of flag values. Must be changed atomically so that flag values aren't lost. See
            // ThreadFlags for bit field meanings.
            volatile uint16_t mFlags;
            // Holds the ThreadState. May be changed non-atomically between Suspended (ie not Runnable)
            // transitions. Changing to Runnable requires that the suspend_request be part of the atomic
            // operation. If a thread is suspended and a suspend_request is present, a thread may not
            // change to Runnable as a GC or other operation is in progress.
            volatile uint16_t mState;
        } mAsStruct;
        AtomicInteger mAsAtomicInt;
        volatile int32_t mAsInt;
    };

    static void ThreadExitCallback(
        /* [in] */ void* arg);

private:
    friend class NativeThreadList;

    // Maximum number of suspend barriers.
    static constexpr uint32_t kMaxSuspendBarriers = 3;

    static Boolean sIsStarted;

    static pthread_key_t sPthreadKeySelf;

    // Used to notify threads that they should attempt to resume, they will suspend again if
    // their suspend count is > 0.
    static NativeConditionVariable* sResumeCond;

    struct PACKED(4) tls_32bit_sized_values
    {
        explicit tls_32bit_sized_values()
            : mSuspendCount(0)
            , mDebugSuspendCount(0)
            , mThinLockThreadId(0)
            , mTid(0)
            , mThreadExitCheckCount(0)
        {}

        union StateAndFlags mStateAndFlags;

        // A non-zero value is used to tell the current thread to enter a safe point
        // at the next poll.
        int mSuspendCount;

        // How much of 'suspend_count_' is by request of the debugger, used to set things right
        // when the debugger detaches. Must be <= suspend_count_.
        int mDebugSuspendCount;

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
            : mSuspendTrigger(nullptr)
            , mWaitNext(nullptr)
            , mMonitorEnterObject(nullptr)
        {
            memset(&mHeldMutexes[0], 0, sizeof(mHeldMutexes));
        }

        // In certain modes, setting this to 0 will trigger a SEGV and thus a suspend check.  It is
        // normally set to the address of itself.
        uintptr_t* mSuspendTrigger;

        // The next thread in the wait set this thread is part of or null if not waiting.
        NativeThread* mWaitNext;

        // If we're blocked in MonitorEnter, this is the object we're trying to lock.
        NativeObject* mMonitorEnterObject;

        // Pending barriers that require passing or NULL if non-pending. Installation guarding by
        // Locks::thread_suspend_count_lock_.
        // They work effectively as art::Barrier, but implemented directly using AtomicInteger and futex
        // to avoid additional cost of a mutex and a condition variable, as used in art::Barrier.
        AtomicInteger* mActiveSuspendBarriers[kMaxSuspendBarriers];

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

inline ThreadState NativeThread::GetState() const
{
    CHECK(mTls32.mStateAndFlags.mAsStruct.mState >= kTerminated);
    CHECK(mTls32.mStateAndFlags.mAsStruct.mState <= kSuspended);
    return static_cast<ThreadState>(mTls32.mStateAndFlags.mAsStruct.mState);
}

inline int NativeThread::GetSuspendCount() const
{
    return mTls32.mSuspendCount;
}

inline uint32_t NativeThread::GetThreadId() const
{
    return mTls32.mThinLockThreadId;
}

inline pid_t NativeThread::GetTid() const
{
    return mTls32.mTid;
}

inline void NativeThread::SetMonitorEnterObject(
    /* [in] */ NativeObject* obj)
{
    mTlsPtr.mMonitorEnterObject = obj;
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

inline Boolean NativeThread::ReadFlag(
    /* [in] */ ThreadFlag flag) const
{
    return (mTls32.mStateAndFlags.mAsStruct.mFlags & flag) != 0;
}

inline void NativeThread::AtomicClearFlag(
    /* [in] */ ThreadFlag flag)
{
    mTls32.mStateAndFlags.mAsAtomicInt.FetchAndAndSequentiallyConsistent(-1 ^ flag);
}

inline void NativeThread::TriggerSuspend()
{
    mTlsPtr.mSuspendTrigger = nullptr;
}

}
}

#endif // __CCM_CORE_NATIVETHREAD_H__
