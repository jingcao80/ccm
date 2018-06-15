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

#ifndef __CCM_CORE_NATIVEMONITOR_H__
#define __CCM_CORE_NATIVEMONITOR_H__

#include "ccm/core/NativeLockWord.h"
#include "ccm/core/NativeMutex.h"
#include "ccm/core/NativeThreadState.h"
#include <ccmtypes.h>
#include <list>
#include <memory>

namespace ccm {
namespace core {

typedef uint32_t MonitorId;

class NativeThread;
class NativeObject;

class NativeMonitor
{
public:
    // Return the thread id of the lock owner or 0 when there is no owner.
    static uint32_t GetLockOwnerThreadId(
        /* [in] */ NativeObject* obj);

    static ECode MonitorEnter(
        /* [in] */ NativeThread* self,
        /* [in] */ NativeObject* obj,
        /* [in] */ Boolean trylock);

    static ECode MonitorExit(
        /* [in] */ NativeThread* self,
        /* [in] */ NativeObject* obj);

    static ECode Notify(
        /* [in] */ NativeThread* self,
        /* [in] */ NativeObject* obj);

    static ECode NotifyAll(
        /* [in] */ NativeThread* self,
        /* [in] */ NativeObject* obj);

    // Object.wait().  Also called for class init.
    // NO_THREAD_SAFETY_ANALYSIS for mon->Wait.
    static ECode Wait(
        /* [in] */ NativeThread* self,
        /* [in] */ NativeObject* obj,
        /* [in] */ int64_t ms,
        /* [in] */ int32_t ns,
        /* [in] */ Boolean interruptShouldThrow,
        /* [in] */ NativeThreadState why);

    NativeObject* GetObject();

    NativeThread* GetOwner() const;

    MonitorId GetMonitorId() const;

    // Inflate the lock on obj. May fail to inflate for spurious reasons, always re-check.
    static void InflateThinLocked(
        /* [in] */ NativeThread* self,
        /* [in] */ NativeObject* obj,
        /* [in] */ NativeLockWord lockWord);

private:
    NativeMonitor(
        /* [in] */ NativeThread* self,
        /* [in] */ NativeThread* owner,
        /* [in] */ NativeObject* obj,
        /* [in] */ MonitorId id);

    // Install the monitor into its object, may fail if another thread installs a different monitor
    // first.
    Boolean Install(
        /* [in] */ NativeThread* self);

    // Links a thread into a monitor's wait set.  The monitor lock must be held by the caller of this
    // routine.
    void AppendToWaitSet(
        /* [in] */ NativeThread* thread);

    // Unlinks a thread from a monitor's wait set.  The monitor lock must be held by the caller of
    // this routine.
    void RemoveFromWaitSet(
        /* [in] */ NativeThread* thread);

    // Changes the shape of a monitor from thin to fat, preserving the internal lock state. The
    // calling thread must own the lock or the owner must be suspended. There's a race with other
    // threads inflating the lock, installing hash codes and spurious failures. The caller should
    // re-read the lock word following the call.
    static void Inflate(
        /* [in] */ NativeThread* self,
        /* [in] */ NativeThread* owner,
        /* [in] */ NativeObject* obj);

    static ECode FailedUnlock(
        /* [in] */ NativeObject* obj,
        /* [in] */ uint32_t expectedOwnerThreadId,
        /* [in] */ uint32_t foundOwnerThreadId,
        /* [in] */ NativeMonitor* mon);

    // Try to lock without blocking, returns true if we acquired the lock.
    ECode TryLock(
        /* [in] */ NativeThread* self);

    // Variant for already holding the monitor lock.
    ECode TryLockLocked(
        /* [in] */ NativeThread* self);

    ECode Lock(
        /* [in] */ NativeThread* self);

    ECode Unlock(
        /* [in] */ NativeThread* self);

    static ECode DoNotify(
        /* [in] */ NativeThread* self,
        /* [in] */ NativeObject* obj,
        /* [in] */ Boolean notifyAll);

    ECode Notify(
        /* [in] */ NativeThread* self);

    ECode NotifyAll(
        /* [in] */ NativeThread* self);

    // Wait on a monitor until timeout, interrupt, or notification.  Used for Object.wait() and
    // (somewhat indirectly) Thread.sleep() and Thread.join().
    //
    // If another thread calls Thread.interrupt(), we throw InterruptedException and return
    // immediately if one of the following are true:
    //  - blocked in wait(), wait(long), or wait(long, int) methods of Object
    //  - blocked in join(), join(long), or join(long, int) methods of Thread
    //  - blocked in sleep(long), or sleep(long, int) methods of Thread
    // Otherwise, we set the "interrupted" flag.
    //
    // Checks to make sure that "ns" is in the range 0-999999 (i.e. fractions of a millisecond) and
    // throws the appropriate exception if it isn't.
    //
    // The spec allows "spurious wakeups", and recommends that all code using Object.wait() do so in
    // a loop.  This appears to derive from concerns about pthread_cond_wait() on multiprocessor
    // systems.  Some commentary on the web casts doubt on whether these can/should occur.
    //
    // Since we're allowed to wake up "early", we clamp extremely long durations to return at the end
    // of the 32-bit time epoch.
    ECode Wait(
        /* [in] */ NativeThread* self,
        /* [in] */ int64_t msec,
        /* [in] */ int32_t nsec,
        /* [in] */ Boolean interruptShouldThrow,
        /* [in] */ NativeThreadState why);

    uint32_t GetOwnerThreadId();

public:
    // The default number of spins that are done before thread suspension is used to forcibly inflate
    // a lock word.
    constexpr static size_t kDefaultMaxSpinsBeforeThinLockInflation = 50;

private:
    friend class NativeMonitorPool;

    NativeMutex mMonitorLock;

    NativeConditionVariable mMonitorContenders;

    // Number of people waiting on the condition.
    size_t mNumWaiters;

    // Which thread currently owns the lock?
    NativeThread* volatile mOwner;

    // Owner's recursive lock depth.
    Integer mLockCount;

    NativeObject* mObj;

    // Threads currently waiting on this monitor.
    NativeThread* mWaitSet;

    // The denser encoded version of this monitor as stored in the lock word.
    MonitorId mMonitorId;

    // Free list for monitor pool.
    NativeMonitor* mNextFree;
};

inline ECode NativeMonitor::Notify(
    /* [in] */ NativeThread* self,
    /* [in] */ NativeObject* obj)
{
    return DoNotify(self, obj, false);
}

inline ECode NativeMonitor::NotifyAll(
    /* [in] */ NativeThread* self,
    /* [in] */ NativeObject* obj)
{
    return DoNotify(self, obj, true);
}

inline NativeObject* NativeMonitor::GetObject()
{
    return mObj;
}

inline NativeThread* NativeMonitor::GetOwner() const
{
    return mOwner;
}

inline MonitorId NativeMonitor::GetMonitorId() const
{
    return mMonitorId;
}

class NativeMonitorList
{
public:
    NativeMonitorList();

    ~NativeMonitorList();

    void Add(
        /* [in] */ NativeMonitor* m);

public:
    typedef std::list<NativeMonitor*, std::allocator<NativeMonitor*>> Monitors;

private:
    // During sweeping we may free an object and on a separate thread have an object created using
    // the newly freed memory. That object may then have its lock-word inflated and a monitor created.
    // If we allow new monitor registration during sweeping this monitor may be incorrectly freed as
    // the object wasn't marked when sweeping began.
    Boolean mAllowNewMonitors;
    NativeMutex mMonitorListLock;
    NativeConditionVariable mMonitorAddCondition;
    Monitors mList;
};

}
}

#endif // __CCM_CORE_NATIVEMONITOR_H__
