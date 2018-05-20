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
    kMonitorLock,

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

protected:
    void RegisterAsLocked(
        /* [in] */ NativeThread* self);

    void RegisterAsUnlocked(
        /* [in] */ NativeThread* self);

protected:
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
    void ExclusiveLock(
        /* [in] */ NativeThread* self);

    void ExclusiveUnlock(
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
    friend class ConditionVariable;

    // 0 is unheld, 1 is held.
    AtomicInteger mState;
    // Exclusive owner.
    volatile uint64_t mExclusiveOwner;
    // Number of waiting contenders.
    AtomicInteger mNumContenders;
    const Boolean mRecursive;  // Can the lock be recursively held?
    unsigned int mRecursionCount;
};

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

class NativeConditionVariable
{
public:
    void Signal(
        /* [in] */ NativeThread* self);

private:
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

}
}

#endif // __CCM_CORE_NATIVEMUTEX_H__
