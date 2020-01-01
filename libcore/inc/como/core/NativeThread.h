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

#ifndef __COMO_CORE_NATIVETHREAD_H__
#define __COMO_CORE_NATIVETHREAD_H__

#include "como/core/nativeapi.h"
#include "como/core/NativeMonitor.h"
#include "como/core/NativeMutex.h"
#include "como/core/NativeScopedThreadStateChange.h"
#include "como/core/NativeThreadState.h"
#include <ccmtypes.h>
#include <pthread.h>

namespace como {
namespace core {

class IThreadGroup;
class NativeThreadList;
class Thread;

// Thread priorities. These must match the Thread.MIN_PRIORITY,
// Thread.NORM_PRIORITY, and Thread.MAX_PRIORITY constants.
enum NativeThreadPriority
{
    kMinThreadPriority = 1,
    kNormThreadPriority = 5,
    kMaxThreadPriority = 10,
};

enum NativeThreadFlag
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
    // Creates a new native thread corresponding to the given managed peer.
    // Used to implement Thread.start.
    static ECode CreateNativeThread(
        /* [in] */ Thread* peer,
        /* [in] */ size_t stackSize,
        /* [in] */ Boolean daemon);

    // Attaches the calling native thread to the runtime, returning the new native peer.
    static NativeThread* Attach(
        /* [in] */ const String& threadName,
        /* [in] */ Boolean asDaemon,
        /* [in] */ IThreadGroup* threadGroup,
        /* [in] */ Boolean createPeer);

    static NativeThread* Current();

    static NativeThread* FromManagedThread(
        /* [in] */ Thread* peer);

    String ShortDump() const;

    NativeThreadState GetState() const;

    NativeThreadState SetState(
        /* [in] */ NativeThreadState newState);

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
    NativeThreadState TransitionFromSuspendedToRunnable();

    // Transition from runnable into a state where mutator privileges are denied. Releases share of
    // mutator lock.
    void TransitionFromRunnableToSuspended(
        /* [in] */ NativeThreadState newState);

    void AssertThreadSuspensionIsAllowable(
        /* [in] */ Boolean checkLocks = true) const;

    Boolean HoldsLock(
        /* [in] */ NativeObject* object);

    /*
    * Changes the priority of this thread to match that of the java.lang.Thread object.
    *
    * We map a priority value from 1-10 to Linux "nice" values, where lower
    * numbers indicate higher priority.
    */
    void SetNativePriority(
        /* [in] */ int newPriority);

    /*
    * Returns the thread priority for the current thread by querying the system.
    *
    * Returns a value from 1 to 10 (compatible with como::core::Thread values).
    */
    static Integer GetNativePriority();

    uint32_t GetThreadId() const;

    pid_t GetTid() const;

    // Returns the java.lang.Thread's name, or null if this Thread* doesn't have a peer.
    String GetThreadName();

    // Sets 'name' to the como::core::Thread's name. This requires no transition to managed code,
    // allocation, or locking.
    void GetThreadName(
        /* [in] */ String* name);

    // Sets the thread's name.
    void SetThreadName(
        /* [in] */ const String& name);

    NativeObject* GetPeer() const;

    Thread* GetPeerThread() const;

    static void Startup();

    static void FinishStartup();

    static void Shutdown();

    void SetMonitorEnterObject(
        /* [in] */ NativeObject* obj);

    Boolean Interrupted();

    Boolean IsInterrupted();

    Boolean IsInterruptedLocked();

    void Interrupt(
        /* [in] */ NativeThread* self);

    void SetInterruptedLocked(
        /* [in] */ Boolean i);

    void Notify();

    NativeMutex* GetWaitMutex() const;

    NativeConditionVariable* GetWaitConditionVariable() const;

    NativeMonitor* GetWaitMonitor() const;

    void SetWaitMonitor(
        /* [in] */ NativeMonitor* mon);

    NativeThread* GetWaitNext() const;

    void SetWaitNext(
        /* [in] */ NativeThread* next);

    uint8_t* GetStackEnd() const;

    // Set the stack end to that to be used during regular execution
    void ResetDefaultStackEnd();

    void SetHeldMutex(
        /* [in] */ LockLevel level,
        /* [in] */ BaseMutex* mutex);

private:
    explicit NativeThread(
        /* [in] */ Boolean daemon);

    template <typename PeerAction>
    static NativeThread* Attach(
        /* [in] */ const String& threadName,
        /* [in] */ Boolean asDaemon,
        /* [in] */ PeerAction p);

    ECode CreatePeer(
        /* [in] */ const String& name,
        /* [in] */ Boolean asDaemon,
        /* [in] */ IThreadGroup* threadGroup);

    void Destroy();

    void NotifyLocked(
        /* [in] */ NativeThread* self);

    Boolean ReadFlag(
        /* [in] */ NativeThreadFlag flag) const;

    void AtomicClearFlag(
        /* [in] */ NativeThreadFlag flag);

    // Remove the suspend trigger for this thread by making the suspend_trigger_ TLS value
    // equal to a valid pointer.
    void RemoveSuspendTrigger();

    // Trigger a suspend check by making the suspend_trigger_ TLS value an invalid pointer.
    // The next time a suspend check is done, it will load from the value at this address
    // and trigger a SIGSEGV.
    void TriggerSuspend();

    Boolean ProtectStack(
        /* [in] */ Boolean fatalOnError = true);

    Boolean UnprotectStack();

    static void* CreateCallback(
        /* [in] */ void* arg);

    void RemoveFromThreadGroup(
        /* [in] */ ScopedObjectAccess& soa);

    void SetUpAlternateSignalStack();

    // Initialize a thread.
    Boolean Init(
        /* [in] */ NativeThreadList* threadList);

    void InitCpu();

    void InitTid();

    Boolean InitStackHwm();

    void TransitionToSuspendedAndRunCheckpoints(
        /* [in] */ NativeThreadState newState);

    void PassActiveSuspendBarriers();

    Boolean ModifySuspendCountInternal(
        /* [in] */ NativeThread* self,
        /* [in] */ Integer delta,
        /* [in] */ AtomicInteger* suspendBarrier,
        /* [in] */ Boolean forDebugger);

    Boolean PassActiveSuspendBarriers(
        /* [in] */ NativeThread* self);

    // Install the protected region for implicit stack checks.
    void InstallImplicitProtection();

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
            // Holds the NativeThreadState. May be changed non-atomically between Suspended (ie not Runnable)
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

public:
    static const size_t kStackOverflowImplicitCheckSize;

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
        explicit tls_32bit_sized_values(Boolean daemon)
            : mSuspendCount(0)
            , mDebugSuspendCount(0)
            , mThinLockThreadId(0)
            , mTid(0)
            , mDaemon(daemon)
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

        // Is the thread a daemon?
        const Boolean mDaemon;

        // How many times has our pthread key's destructor been called?
        uint32_t mThreadExitCheckCount;
    } mTls32;

    struct PACKED(sizeof(void*)) tls_ptr_sized_values
    {
        tls_ptr_sized_values()
            : mStackEnd(nullptr)
            , mSuspendTrigger(nullptr)
            , mSelf(nullptr)
            , mOPeer(nullptr)
            , mPeer(0)
            , mStackBegin(nullptr)
            , mStackSize(0)
            , mWaitNext(nullptr)
            , mMonitorEnterObject(nullptr)
            , mName(nullptr)
            , mPthreadSelf(0)
        {
            memset(&mHeldMutexes[0], 0, sizeof(mHeldMutexes));
        }

        // The end of this thread's stack. This is the lowest safely-addressable address on the stack.
        // We leave extra space so there's room for the code that throws StackOverflowError.
        uint8_t* mStackEnd;

        // In certain modes, setting this to 0 will trigger a SEGV and thus a suspend check.  It is
        // normally set to the address of itself.
        uintptr_t* mSuspendTrigger;

        // Initialized to "this". On certain architectures (such as x86) reading off of Thread::Current
        // is easy but getting the address of Thread::Current is hard. This field can be read off of
        // Thread::Current to give the address.
        NativeThread* mSelf;

        NativeObject* mOPeer;
        HANDLE mPeer;

        // The "lowest addressable byte" of the stack.
        uint8_t* mStackBegin;

        // Size of the stack.
        size_t mStackSize;

        // The next thread in the wait set this thread is part of or null if not waiting.
        NativeThread* mWaitNext;

        // If we're blocked in MonitorEnter, this is the object we're trying to lock.
        NativeObject* mMonitorEnterObject;

        // A cached copy of the java.lang.Thread's name.
        String* mName;

        // A cached pthread_t for the pthread underlying this Thread*.
        pthread_t mPthreadSelf;

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

    // Thread "interrupted" status; stays raised until queried or thrown.
    Boolean mInterrupted;

    static constexpr Boolean DEBUG = false;
};

inline NativeThreadState NativeThread::GetState() const
{
    CHECK(mTls32.mStateAndFlags.mAsStruct.mState >= kTerminated);
    CHECK(mTls32.mStateAndFlags.mAsStruct.mState <= kSuspended);
    return static_cast<NativeThreadState>(mTls32.mStateAndFlags.mAsStruct.mState);
}

inline void NativeThread::AssertThreadSuspensionIsAllowable(
    /* [in] */ Boolean checkLocks) const
{}

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

inline NativeObject* NativeThread::GetPeer() const
{
    CHECK(Current() == this);
    CHECK(mTlsPtr.mPeer == nullptr);
    return mTlsPtr.mOPeer;
}

inline void NativeThread::SetMonitorEnterObject(
    /* [in] */ NativeObject* obj)
{
    mTlsPtr.mMonitorEnterObject = obj;
}

inline Boolean NativeThread::IsInterruptedLocked()
{
    return mInterrupted;
}

inline void NativeThread::SetInterruptedLocked(
    /* [in] */ Boolean i)
{
    mInterrupted = i;
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

inline void NativeThread::SetWaitMonitor(
    /* [in] */ NativeMonitor* mon)
{
    mWaitMonitor = mon;
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

inline uint8_t* NativeThread::GetStackEnd() const
{
    return mTlsPtr.mStackEnd;
}

inline void NativeThread::ResetDefaultStackEnd()
{
    // Our stacks grow down, so we want stack_end_ to be near there, but reserving enough room
    // to throw a StackOverflowError.
    mTlsPtr.mStackEnd = mTlsPtr.mStackBegin + GetStackOverflowReservedBytes(kRuntimeISA);
}

inline Boolean NativeThread::ReadFlag(
    /* [in] */ NativeThreadFlag flag) const
{
    return (mTls32.mStateAndFlags.mAsStruct.mFlags & flag) != 0;
}

inline void NativeThread::AtomicClearFlag(
    /* [in] */ NativeThreadFlag flag)
{
    mTls32.mStateAndFlags.mAsAtomicInt.FetchAndAndSequentiallyConsistent(-1 ^ flag);
}

inline void NativeThread::RemoveSuspendTrigger()
{
    mTlsPtr.mSuspendTrigger = reinterpret_cast<uintptr_t*>(&mTlsPtr.mSuspendTrigger);
}

inline void NativeThread::TriggerSuspend()
{
    mTlsPtr.mSuspendTrigger = nullptr;
}

class NativeThreadLifecycleCallback
{
public:
    virtual ~NativeThreadLifecycleCallback() {}

    virtual void ThreadStart(
        /* [in] */ NativeThread* self) = 0;

    virtual void ThreadDeath(
        /* [in] */ NativeThread* self) = 0;
};

}
}

#endif // __COMO_CORE_NATIVETHREAD_H__
