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

#ifndef __CCM_CORE_NATIVEMUTEX_H__
#define __CCM_CORE_NATIVEMUTEX_H__

#include "core/NativeAtomic.h"
#include <ccmtypes.h>
#include <unistd.h>
#include <linux/futex.h>
#include <sys/syscall.h>

#ifndef SYS_futex
#define SYS_futex __NR_futex
#endif

namespace ccm {
namespace core {

// LockLevel is used to impose a lock hierarchy [1] where acquisition of a Mutex at a higher or
// equal level to a lock a thread holds is invalid. The lock hierarchy achieves a cycle free
// partial ordering and thereby cause deadlock situations to fail checks.
//
// [1] http://www.drdobbs.com/parallel/use-lock-hierarchies-to-avoid-deadlock/204801163
enum LockLevel {
    kLoggingLock = 0,
    kThreadSuspendCountLock,
    kDefaultMutexLevel,
    kMonitorPoolLock,
    kMonitorLock,
    kThreadListLock,
    kRuntimeShutdownLock,
    kMutatorLock,

    kLockLevelCount  // Must come last.
};

static inline int futex(volatile int *uaddr, int op, int val, const struct timespec *timeout,
                        volatile int *uaddr2, int val3)
{
    return syscall(SYS_futex, uaddr, op, val, timeout, uaddr2, val3);
}

class NativeThread;

class BaseMutex
{
public:
    BaseMutex(
        /* [in] */ const String& name,
        /* [in] */ LockLevel level);

protected:
    void RegisterAsLocked(
        /* [in] */ NativeThread* self);

    void RegisterAsUnlocked(
        /* [in] */ NativeThread* self);

    void CheckSafeToWait(
        /* [in] */ NativeThread* self);

protected:
    friend class NativeConditionVariable;

    const LockLevel mLevel;  // Support for lock hierarchy.
    const String mName;
};

class NativeMutex
    : public BaseMutex
{
public:
    class AutoLock
    {
    public:
        inline explicit AutoLock(
            /* [in] */ NativeThread* self,
            /* [in] */ NativeMutex& mutex);

        inline ~AutoLock();

    private:
        NativeThread* mSelf;
        NativeMutex& mLock;
    };

public:
    explicit NativeMutex(
        /* [in] */ const String& name,
        /* [in] */ LockLevel level = kDefaultMutexLevel,
        /* [in] */ Boolean recursive = false);

    ~NativeMutex();

    void ExclusiveLock(
        /* [in] */ NativeThread* self);

    void Lock(
        /* [in] */ NativeThread* self);

    void ExclusiveUnlock(
        /* [in] */ NativeThread* self);

    void Unlock(
        /* [in] */ NativeThread* self);

    // Is the current thread the exclusive holder of the Mutex.
    Boolean IsExclusiveHeld(
        /* [in] */ const NativeThread* self) const;

    // Assert that the Mutex is exclusively held by the current thread.
    void AssertExclusiveHeld(
        /* [in] */ const NativeThread* self);

    // Id associated with exclusive owner. No memory ordering semantics if called from a thread other
    // than the owner.
    uint64_t GetExclusiveOwnerTid() const;

private:
    friend class NativeConditionVariable;

    // 0 is unheld, 1 is held.
    AtomicInteger mState;
    // Exclusive owner.
    volatile uint64_t mExclusiveOwner;
    // Number of waiting contenders.
    AtomicInteger mNumContenders;
    const Boolean mRecursive;  // Can the lock be recursively held?
    unsigned int mRecursionCount;
};

inline void NativeMutex::Lock(
    /* [in] */ NativeThread* self)
{
    ExclusiveLock(self);
}

inline void NativeMutex::Unlock(
    /* [in] */ NativeThread* self)
{
    ExclusiveUnlock(self);
}

inline void NativeMutex::AssertExclusiveHeld(
    /* [in] */ const NativeThread* self)
{
    CHECK(IsExclusiveHeld(self));
}

inline uint64_t NativeMutex::GetExclusiveOwnerTid() const
{
    return mExclusiveOwner;
}

NativeMutex::AutoLock::AutoLock(
    /* [in] */ NativeThread* self,
    /* [in] */ NativeMutex& mutex)
    : mSelf(self)
    , mLock(mutex)
{
    mLock.ExclusiveLock(mSelf);
}

NativeMutex::AutoLock::~AutoLock()
{
    mLock.ExclusiveUnlock(mSelf);
}

// A ReaderWriterMutex is used to achieve mutual exclusion between threads, similar to a Mutex.
// Unlike a Mutex a ReaderWriterMutex can be used to gain exclusive (writer) or shared (reader)
// access to what it guards. A flaw in relation to a Mutex is that it cannot be used with a
// condition variable. A ReaderWriterMutex can be in one of three states:
// - Free - not owned by any thread,
// - Exclusive - owned by a single thread,
// - Shared(n) - shared amongst n threads.
//
// The effect of locking and unlocking operations on the state is:
//
// State     | ExclusiveLock | ExclusiveUnlock | SharedLock       | SharedUnlock
// ----------------------------------------------------------------------------
// Free      | Exclusive     | error           | SharedLock(1)    | error
// Exclusive | Block         | Free            | Block            | error
// Shared(n) | Block         | error           | SharedLock(n+1)* | Shared(n-1) or Free
// * for large values of n the SharedLock may block.
class NativeReaderWriterMutex
    : public BaseMutex
{
public:
    explicit NativeReaderWriterMutex(
        /* [in] */ const String& name,
        /* [in] */ LockLevel level = kDefaultMutexLevel);

    // Assert the current thread has shared access to the ReaderWriterMutex.
    void AssertSharedHeld(
        /* [in] */ const NativeThread* self);

    // Assert the current thread doesn't hold this ReaderWriterMutex either in shared or exclusive
    // mode.
    void AssertNotHeld(
        /* [in] */ const NativeThread* self);

private:
    // -1 implies held exclusive, +ve shared held by state_ many owners.
    AtomicInteger mState;
    // Exclusive owner. Modification guarded by this mutex.
    volatile uint64_t mExclusiveOwner;
    // Number of contenders waiting for a reader share.
    AtomicInteger mNumPendingReaders;
    // Number of contenders waiting to be the writer.
    AtomicInteger mNumPendingWriters;
};

inline void NativeReaderWriterMutex::AssertSharedHeld(
    /* [in] */ const NativeThread* self)
{}

inline void NativeReaderWriterMutex::AssertNotHeld(
    /* [in] */ const NativeThread* self)
{}


// MutatorMutex is a special kind of ReaderWriterMutex created specifically for the
// Locks::mutator_lock_ mutex. The behaviour is identical to the ReaderWriterMutex except that
// thread state changes also play a part in lock ownership. The mutator_lock_ will not be truly
// held by any mutator threads. However, a thread in the kRunnable state is considered to have
// shared ownership of the mutator lock and therefore transitions in and out of the kRunnable
// state have associated implications on lock ownership. Extra methods to handle the state
// transitions have been added to the interface but are only accessible to the methods dealing
// with state transitions. The thread state and flags attributes are used to ensure thread state
// transitions are consistent with the permitted behaviour of the mutex.
//
// *) The most important consequence of this behaviour is that all threads must be in one of the
// suspended states before exclusive ownership of the mutator mutex is sought.
//
class NativeMutatorMutex
    : public NativeReaderWriterMutex
{
public:
    explicit NativeMutatorMutex(
        /* [in] */ const String& name,
        /* [in] */ LockLevel level = kDefaultMutexLevel);

private:
    friend class NativeThread;
    void TransitionFromRunnableToSuspended(
        /* [in] */ NativeThread* self);

    void TransitionFromSuspendedToRunnable(
        /* [in] */ NativeThread* self);
};

inline NativeMutatorMutex::NativeMutatorMutex(
    /* [in] */ const String& name,
    /* [in] */ LockLevel level)
    : NativeReaderWriterMutex(name, level)
{}

inline void NativeMutatorMutex::TransitionFromRunnableToSuspended(
    /* [in] */ NativeThread* self)
{
    AssertSharedHeld(self);
    RegisterAsUnlocked(self);
}

inline void NativeMutatorMutex::TransitionFromSuspendedToRunnable(
    /* [in] */ NativeThread* self)
{
    RegisterAsLocked(self);
    AssertSharedHeld(self);
}

class NativeConditionVariable
{
public:
    NativeConditionVariable(
        /* [in] */ const String& name,
        /* [in] */ NativeMutex& mutex);

    void Broadcast(
        /* [in] */ NativeThread* self);

    void Signal(
        /* [in] */ NativeThread* self);

    // TODO: No thread safety analysis on Wait and TimedWait as they call mutex operations via their
    //       pointer copy, thereby defeating annotalysis.
    void Wait(
        /* [in] */ NativeThread* self);

    // Variant of Wait that should be used with caution. Doesn't validate that no mutexes are held
    // when waiting.
    // TODO: remove this.
    void WaitHoldingLocks(
        /* [in] */ NativeThread* self);

private:
    const String mName;
    // The Mutex being used by waiters. It is an error to mix condition variables between different
    // Mutexes.
    NativeMutex& mGuard;
    // A counter that is modified by signals and broadcasts. This ensures that when a waiter gives up
    // their Mutex and another thread takes it and signals, the waiting thread observes that sequence_
    // changed and doesn't enter the wait. Modified while holding guard_, but is read by futex wait
    // without guard_ held.
    AtomicInteger mSequence;
    // Number of threads that have come into to wait, not the length of the waiters on the futex as
    // waiters may have been requeued onto guard_. Guarded by guard_.
    volatile int32_t mNumWaiters;
};

// Global mutexes corresponding to the levels above.
class Locks
{
public:
    static void Init();

    // A barrier is used to synchronize the GC/Debugger thread with mutator threads. When GC/Debugger
    // thread wants to suspend all mutator threads, it needs to wait for all mutator threads to pass
    // a barrier. Threads that are already suspended will get their barrier passed by the GC/Debugger
    // thread; threads in the runnable state will pass the barrier when they transit to the suspended
    // state. GC/Debugger thread will be woken up when all mutator threads are suspended.
    //
    // Thread suspension:
    // mutator thread                                | GC/Debugger
    //   .. running ..                               |   .. running ..
    //   .. running ..                               | Request thread suspension by:
    //   .. running ..                               |   - acquiring thread_suspend_count_lock_
    //   .. running ..                               |   - incrementing Thread::suspend_count_ on
    //   .. running ..                               |     all mutator threads
    //   .. running ..                               |   - releasing thread_suspend_count_lock_
    //   .. running ..                               | Block wait for all threads to pass a barrier
    // Poll Thread::suspend_count_ and enter full    |   .. blocked ..
    // suspend code.                                 |   .. blocked ..
    // Change state to kSuspended (pass the barrier) | Wake up when all threads pass the barrier
    // x: Acquire thread_suspend_count_lock_         |   .. running ..
    // while Thread::suspend_count_ > 0              |   .. running ..
    //   - wait on Thread::resume_cond_              |   .. running ..
    //     (releases thread_suspend_count_lock_)     |   .. running ..
    //   .. waiting ..                               | Request thread resumption by:
    //   .. waiting ..                               |   - acquiring thread_suspend_count_lock_
    //   .. waiting ..                               |   - decrementing Thread::suspend_count_ on
    //   .. waiting ..                               |     all mutator threads
    //   .. waiting ..                               |   - notifying on Thread::resume_cond_
    //    - re-acquire thread_suspend_count_lock_    |   - releasing thread_suspend_count_lock_
    // Release thread_suspend_count_lock_            |  .. running ..
    // Change to kRunnable                           |  .. running ..
    //  - this uses a CAS operation to ensure the    |  .. running ..
    //    suspend request flag isn't raised as the   |  .. running ..
    //    state is changed                           |  .. running ..
    //  - if the CAS operation fails then goto x     |  .. running ..
    //  .. running ..                                |  .. running ..
    static NativeMutatorMutex* sMutatorLock;

    // Guards shutdown of the runtime.
    static NativeMutex* sRuntimeShutdownLock;

    // The thread_list_lock_ guards ThreadList::list_. It is also commonly held to stop threads
    // attaching and detaching.
    static NativeMutex* sThreadListLock;

    static NativeMutex* sAllocatedMonitorIdsLock;

    // Allow mutual exclusion when manipulating Thread::suspend_count_.
    // TODO: Does the trade-off of a per-thread lock make sense?
    static NativeMutex* sThreadSuspendCountLock;
};

}
}

#endif // __CCM_CORE_NATIVEMUTEX_H__
