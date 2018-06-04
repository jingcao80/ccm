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

#include "libcore.h"
#include "core/NativeMonitor.h"
#include "core/NativeMonitorPool.h"
#include "core/NativeMutex.h"
#include "core/NativeObject.h"
#include "core/NativeThreadList.h"
#include <ccmlogger.h>
#include <inttypes.h>

namespace ccm {
namespace core {

NativeMonitor::NativeMonitor(
    /* [in] */ NativeThread* self,
    /* [in] */ NativeThread* owner,
    /* [in] */ NativeObject* obj,
    /* [in] */ MonitorId id)
    : mMonitorLock(String("a monitor lock"), kMonitorLock)
    , mMonitorContenders(String("monitor contenders"), mMonitorLock)
    , mOwner(owner)
    , mLockCount(0)
    , mObj(obj)
    , mWaitSet(nullptr)
    , mMonitorId(id)
    , mNextFree(nullptr)
{
    // We should only inflate a lock if the owner is ourselves or suspended. This avoids a race
    // with the owner unlocking the thin-lock.
    CHECK(owner == nullptr || owner == self || owner->IsSuspended());
    // The identity hash code is set for the life time of the monitor.
}

Boolean NativeMonitor::Install(
    /* [in] */ NativeThread* self)
{
    NativeMutex::AutoLock lock(self, mMonitorLock);
    CHECK(mOwner == nullptr || mOwner == self || mOwner->IsSuspended());
    // Propagate the lock state.
    NativeLockWord lw(GetObject()->GetLockWord(false));
    switch (lw.GetState()) {
        case NativeLockWord::kThinLocked: {
            CHECK(mOwner->GetThreadId() == lw.ThinLockOwner());
            mLockCount = lw.ThinLockCount();
            break;
        }
        case NativeLockWord::kFatLocked: {
            // The owner_ is suspended but another thread beat us to install a monitor.
            return false;
        }
        case NativeLockWord::kUnlocked: {
            Logger::E("NativeMonitor", "Inflating unlocked lock word");
            break;
        }
        default: {
            Logger::E("NativeMonitor", "Invalid monitor state %d", lw.GetState());
            return false;
        }
    }
    NativeLockWord fat(this);
    // Publish the updated lock word, which may race with other threads.
    return GetObject()->CasLockWordWeakRelease(lw, fat);
}

void NativeMonitor::AppendToWaitSet(
    /* [in] */ NativeThread* thread)
{
    CHECK(owner_ == Thread::Current());
    CHECK(thread != nullptr);
    CHECK(thread->GetWaitNext() == nullptr);
    if (mWaitSet == nullptr) {
        mWaitSet = thread;
        return;
    }

    // push_back.
    NativeThread* t = mWaitSet;
    while (t->GetWaitNext() != nullptr) {
        t = t->GetWaitNext();
    }
    t->SetWaitNext(thread);
}

void NativeMonitor::RemoveFromWaitSet(
    /* [in] */ NativeThread* thread)
{
    CHECK(owner_ == Thread::Current());
    CHECK(thread != nullptr);
    if (mWaitSet == nullptr) {
        return;
    }
    if (mWaitSet == thread) {
        mWaitSet = thread->GetWaitNext();
        thread->SetWaitNext(nullptr);
        return;
    }

    NativeThread* t = mWaitSet;
    while (t->GetWaitNext() != nullptr) {
        if (t->GetWaitNext() == thread) {
            t->SetWaitNext(thread->GetWaitNext());
            thread->SetWaitNext(nullptr);
            return;
        }
        t = t->GetWaitNext();
    }
}

ECode NativeMonitor::TryLockLocked(
    /* [in] */ NativeThread* self)
{
    if (mOwner == nullptr) {
        mOwner = self;
        CHECK(mLockCount == 0);
    }
    else if (mOwner == self) {
        mLockCount++;
    }
    else {
        return E_FAILED_EXCEPTION;
    }
    return NOERROR;
}

ECode NativeMonitor::TryLock(
    /* [in] */ NativeThread* self)
{
    NativeMutex::AutoLock lock(self, mMonitorLock);
    return TryLockLocked(self);
}

ECode NativeMonitor::Lock(
    /* [in] */ NativeThread* self)
{
    NativeMutex::AutoLock lock(self, mMonitorLock);
    while (true) {
        if (TryLockLocked(self)) {
            return NOERROR;
        }
        // Do this before releasing the lock so that we don't get deflated.
        size_t numWaiters = mNumWaiters;
        ++mNumWaiters;

        mMonitorLock.Unlock(self);  // Let go of locks in order.
        self->SetMonitorEnterObject(GetObject());
        {
            self->TransitionFromRunnableToSuspended(kBlocked);
            uint32_t originalOwnerThreadId = 0;
            {
                // Reacquire monitor_lock_ without mutator_lock_ for Wait.
                NativeMutex::AutoLock lock2(self, mMonitorLock);
                if (mOwner != nullptr) {  // Did the owner_ give the lock up?
                    originalOwnerThreadId = mOwner->GetThreadId();
                    mMonitorContenders.Wait(self);  // Still contended so wait.
                }
            }
            self->TransitionFromSuspendedToRunnable();
        }
        self->SetMonitorEnterObject(nullptr);
        mMonitorLock.Lock(self);  // Reacquire locks in order.
        --mNumWaiters;
    }
}

static String ThreadToString(
    /* [in] */ NativeThread* thread)
{
    if (thread == nullptr) {
        return String("nullptr");
    }
    return thread->ShortDump();
}

ECode NativeMonitor::FailedUnlock(
    /* [in] */ NativeObject* obj,
    /* [in] */ uint32_t expectedOwnerThreadId,
    /* [in] */ uint32_t foundOwnerThreadId,
    /* [in] */ NativeMonitor* monitor)
{
    // Acquire thread list lock so threads won't disappear from under us.
    String currentOwnerString;
    String expectedOwnerString;
    String foundOwnerString;
    uint32_t currentOwnerThreadId = 0;
    {
        NativeMutex::AutoLock lock(NativeThread::Current(), *Locks::sThreadListLock);
        NativeThreadList* const threadList = NativeRuntime::Current()->GetThreadList();
        NativeThread* expectedOwner = threadList->FindThreadByThreadId(expectedOwnerThreadId);
        NativeThread* foundOwner = threadList->FindThreadByThreadId(foundOwnerThreadId);

        // Re-read owner now that we hold lock.
        NativeThread* currentOwner = (monitor != nullptr) ? monitor->GetOwner() : nullptr;
        if (currentOwner != nullptr) {
            currentOwnerThreadId = currentOwner->GetThreadId();
        }
        // Get short descriptions of the threads involved.
        currentOwnerString = ThreadToString(currentOwner);
        expectedOwnerString = expectedOwner != nullptr ? ThreadToString(expectedOwner) : String("unnamed");
        foundOwnerString = foundOwner != nullptr ? ThreadToString(foundOwner) : String("unnamed");
    }

    if (currentOwnerThreadId == 0) {
        if (foundOwnerThreadId == 0) {
            Logger::E("NativeMonitor", "unlock of unowned monitor on object of type '%s'"
                    " on thread '%s'",
                    NativeObject::PrettyTypeOf(obj).string(),
                    expectedOwnerString.string());
            return E_ILLEGAL_MONITOR_STATE_EXCEPTION;
        }
        else {
            // Race: the original read found an owner but now there is none
            Logger::E("NativeMonitor", "unlock of monitor owned by '%s' on object of type '%s'"
                    " (where now the monitor appears unowned) on thread '%s'",
                    foundOwnerString.string(),
                    NativeObject::PrettyTypeOf(obj).string(),
                    expectedOwnerString.string());
            return E_ILLEGAL_MONITOR_STATE_EXCEPTION;
        }
    }
    else {
        if (foundOwnerThreadId == 0) {
            // Race: originally there was no owner, there is now
            Logger::E("NativeMonitor", "unlock of monitor owned by '%s' on object of type '%s'"
                    " (originally believed to be unowned) on thread '%s'",
                    currentOwnerString.string(),
                    NativeObject::PrettyTypeOf(obj).string(),
                    expectedOwnerString.string());
            return E_ILLEGAL_MONITOR_STATE_EXCEPTION;
        }
        else {
            if (foundOwnerThreadId != currentOwnerThreadId) {
                // Race: originally found and current owner have changed
                Logger::E("NativeMonitor", "unlock of monitor originally owned by '%s' (now"
                        " owned by '%s') on object of type '%s' on thread '%s'",
                        foundOwnerString.string(),
                        currentOwnerString.string(),
                        NativeObject::PrettyTypeOf(obj).string(),
                        expectedOwnerString.string());
                return E_ILLEGAL_MONITOR_STATE_EXCEPTION;
            }
            else {
                Logger::E("NativeMonitor", "unlock of monitor owned by '%s' on object of type '%s'"
                        " on thread '%s",
                        currentOwnerString.string(),
                        NativeObject::PrettyTypeOf(obj).string(),
                        expectedOwnerString.string());
                return E_ILLEGAL_MONITOR_STATE_EXCEPTION;
            }
        }
    }
}

ECode NativeMonitor::Unlock(
    /* [in] */ NativeThread* self)
{
    CHECK(self != nullptr);
    uint32_t ownerThreadId = 0;
    {
        NativeMutex::AutoLock lock(self, mMonitorLock);
        NativeThread* owner = mOwner;
        if (owner != nullptr) {
            ownerThreadId = owner->GetThreadId();
        }
        if (owner == self) {
            // We own the monitor, so nobody else can be in here.
            if (mLockCount == 0) {
                mOwner = nullptr;
                // Wake a contender.
                mMonitorContenders.Signal(self);
            }
            else {
                --mLockCount;
            }
            return NOERROR;
        }
    }
    // We don't own this, so we're not allowed to unlock it.
    return FailedUnlock(GetObject(), self->GetThreadId(), ownerThreadId, this);
}

ECode NativeMonitor::Wait(
    /* [in] */ NativeThread* self,
    /* [in] */ int64_t ms,
    /* [in] */ int32_t ns,
    /* [in] */ Boolean interruptShouldThrow,
    /* [in] */ NativeThreadState why)
{
    CHECK(self != nullptr);
    CHECK(why == kTimedWaiting || why == kWaiting || why == kSleeping);

    mMonitorLock.Lock(self);

    // Make sure that we hold the lock.
    if (mOwner != self) {
        mMonitorLock.Unlock(self);
        Logger::E("NativeMonitor", "object not locked by thread before wait()");
        return E_ILLEGAL_MONITOR_STATE_EXCEPTION;
    }

    // We need to turn a zero-length timed wait into a regular wait because
    // Object.wait(0, 0) is defined as Object.wait(0), which is defined as Object.wait().
    if (why == kTimedWaiting && (ms == 0 && ns == 0)) {
        why = kWaiting;
    }

    // Enforce the timeout range.
    if (ms < 0 || ns < 0 || ns > 999999) {
        mMonitorLock.Unlock(self);
        Logger::E("NativeMonitor", "timeout arguments out of range: ms=%" PRId64 " ns=%d", ms, ns);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    /*
    * Add ourselves to the set of threads waiting on this monitor, and
    * release our hold.  We need to let it go even if we're a few levels
    * deep in a recursive lock, and we need to restore that later.
    *
    * We append to the wait set ahead of clearing the count and owner
    * fields so the subroutine can check that the calling thread owns
    * the monitor.  Aside from that, the order of member updates is
    * not order sensitive as we hold the pthread mutex.
    */
    AppendToWaitSet(self);
    ++mNumWaiters;
    Integer prevLockCount = mLockCount;
    mLockCount = 0;
    mOwner = nullptr;

    Boolean wasInterrupted = false;
    {
        // Pseudo-atomically wait on self's wait_cond_ and release the monitor lock.
        NativeMutex::AutoLock lock(self, *self->GetWaitMutex());

        // Set wait_monitor_ to the monitor object we will be waiting on. When wait_monitor_ is
        // non-null a notifying or interrupting thread must signal the thread's wait_cond_ to wake it
        // up.
        CHECK(self->GetWaitMonitor() == nullptr);
        self->SetWaitMonitor(this);

        // Release the monitor lock.
        mMonitorContenders.Signal(self);
        mMonitorLock.Unlock(self);

        // Handle the case where the thread was interrupted before we called wait().
        if (self->IsInterruptedLocked()) {
            wasInterrupted = true;
        }
        else {
            // Wait for a notification or a timeout to occur.
            if (why == kWaiting) {
                self->GetWaitConditionVariable()->Wait(self);
            }
            else {
                CHECK(why == kTimedWaiting || why == kSleeping);
                self->GetWaitConditionVariable()->TimedWait(self, ms, ns);
            }
            wasInterrupted = self->IsInterruptedLocked();
        }
    }

    {
        // We reset the thread's wait_monitor_ field after transitioning back to runnable so
        // that a thread in a waiting/sleeping state has a non-null wait_monitor_ for debugging
        // and diagnostic purposes. (If you reset this earlier, stack dumps will claim that threads
        // are waiting on "null".)
        NativeMutex::AutoLock lock(self, *self->GetWaitMutex());
        CHECK(self->GetWaitMonitor() != nullptr);
        self->SetWaitMonitor(nullptr);
    }

    ECode ec = NOERROR;

    // Allocate the interrupted exception not holding the monitor lock since it may cause a GC.
    // If the GC requires acquiring the monitor for enqueuing cleared references, this would
    // cause a deadlock if the monitor is held.
    if (wasInterrupted && interruptShouldThrow) {
        /*
         * We were interrupted while waiting, or somebody interrupted an
         * un-interruptible thread earlier and we're bailing out immediately.
         *
         * The doc sayeth: "The interrupted status of the current thread is
         * cleared when this exception is thrown."
         */
        {
            NativeMutex::AutoLock lock(self, *self->GetWaitMutex());
            self->SetInterruptedLocked(false);
        }
        ec = E_INTERRUPTED_EXCEPTION;
    }

    // Re-acquire the monitor and lock.
    Lock(self);
    mMonitorLock.Lock(self);
    self->GetWaitMutex()->AssertNotHeld(self);

    /*
    * We remove our thread from wait set after restoring the count
    * and owner fields so the subroutine can check that the calling
    * thread owns the monitor. Aside from that, the order of member
    * updates is not order sensitive as we hold the pthread mutex.
    */
    mOwner = self;
    mLockCount = prevLockCount;
    --mNumWaiters;
    RemoveFromWaitSet(self);

    mMonitorLock.Unlock(self);

    return ec;
}

void NativeMonitor::Inflate(
    /* [in] */ NativeThread* self,
    /* [in] */ NativeThread* owner,
    /* [in] */ NativeObject* obj)
{
    CHECK(self != nullptr);
    CHECK(obj != nullptr);
    // Allocate and acquire a new monitor.
    NativeMonitor* m = NativeMonitorPool::CreateMonitor(self, owner, obj);
    CHECK(m != nullptr);
    if (m->Install(self)) {
        Logger::V("NativeMonitor", "monitor: thread%d created monitor %p for object %p",
                owner->GetThreadId(), m, obj);
        NativeRuntime::Current()->GetMonitorList()->Add(m);
        CHECK(obj->GetLockWord(true).GetState() == NativeLockWord::kFatLocked);
    }
    else {
        NativeMonitorPool::ReleaseMonitor(self, m);
    }
}

void NativeMonitor::InflateThinLocked(
    /* [in] */ NativeThread* self,
    /* [in] */ NativeObject* obj,
    /* [in] */ NativeLockWord lockWord)
{
    CHECK(lockWord.GetState() == NativeLockWord::kThinLocked);
    uint32_t ownerThreadId = lockWord.ThinLockOwner();
    if (ownerThreadId == self->GetThreadId()) {
        // We own the monitor, we can easily inflate it.
        Inflate(self, self, obj);
    }
    else {
        NativeThreadList* threadList = NativeRuntime::Current()->GetThreadList();
        // Suspend the owner, inflate. First change to blocked and give up mutator_lock_.
        self->SetMonitorEnterObject(obj);
        Boolean timedOut;
        self->TransitionFromRunnableToSuspended(kBlocked);
        NativeThread* owner = threadList->SuspendThreadByThreadId(ownerThreadId, false, &timedOut);
        self->TransitionFromSuspendedToRunnable();
        if (owner != nullptr) {
            // We succeeded in suspending the thread, check the lock's status didn't change.
            lockWord = obj->GetLockWord(true);
            if (lockWord.GetState() == NativeLockWord::kThinLocked &&
                    lockWord.ThinLockOwner() == ownerThreadId) {
                // Go ahead and inflate the lock.
                Inflate(self, owner, obj);
            }
            threadList->Resume(owner, false);
        }
        self->SetMonitorEnterObject(nullptr);
    }
}

ECode NativeMonitor::MonitorEnter(
    /* [in] */ NativeThread* self,
    /* [in] */ NativeObject* obj,
    /* [in] */ Boolean trylock)
{
    CHECK(self != nullptr);
    CHECK(obj != nullptr);
    uint32_t threadId = self->GetThreadId();
    size_t contentionCount = 0;
    while (true) {
        // We initially read the lockword with ordinary relaxed semantics. When stronger
        // semantics are needed, we address it below. Since GetLockWord bottoms out to a relaxed load,
        // we can fix it later, in an infrequently executed case, with a fence.
        NativeLockWord lockWord = obj->GetLockWord(false);
        switch (lockWord.GetState()) {
            case NativeLockWord::kUnlocked: {
                // No ordering required for preceding lockword read, since we retest.
                NativeLockWord thinLocked(NativeLockWord::FromThinLockId(threadId, 0));
                if (obj->CasLockWordWeakAcquire(lockWord, thinLocked)) {
                  return NOERROR;  // Success!
                }
                continue;  // Go again.
            }
            case NativeLockWord::kThinLocked: {
                uint32_t ownerThreadId = lockWord.ThinLockOwner();
                if (ownerThreadId == threadId) {
                    // No ordering required for initial lockword read.
                    // We own the lock, increase the recursion count.
                    uint32_t newCount = lockWord.ThinLockCount() + 1;
                    if (LIKELY(newCount <= NativeLockWord::kThinLockMaxCount)) {
                        NativeLockWord thinLocked(NativeLockWord::FromThinLockId(
                                threadId, newCount));
                        // Only this thread pays attention to the count. Thus there is no need for stronger
                        // than relaxed memory ordering.
                        obj->SetLockWord(thinLocked, false /* volatile */);
                        return NOERROR;
                    }
                    else {
                        // We'd overflow the recursion count, so inflate the monitor.
                        InflateThinLocked(self, obj, lockWord);
                    }
                }
                else {
                    if (trylock) {
                        return E_FAILED_EXCEPTION;
                    }
                    // Contention.
                    contentionCount++;
                    if (contentionCount <= NativeRuntime::Current()->
                            GetMaxSpinsBeforeThinLockInflation()) {
                        // TODO: Consider switching the thread state to kBlocked when we are yielding.
                        // Use sched_yield instead of NanoSleep since NanoSleep can wait much longer than the
                        // parameter you pass in. This can cause thread suspension to take excessively long
                        // and make long pauses. See b/16307460.
                        // TODO: We should literally spin first, without sched_yield. Sched_yield either does
                        // nothing (at significant expense), or guarantees that we wait at least microseconds.
                        // If the owner is running, I would expect the median lock hold time to be hundreds
                        // of nanoseconds or less.
                        sched_yield();
                    }
                    else {
                        contentionCount = 0;
                        // No ordering required for initial lockword read. Install rereads it anyway.
                        InflateThinLocked(self, obj, lockWord);
                    }
                }
                continue; // Start from the beginning.
            }
            case NativeLockWord::kFatLocked: {
                // We should have done an acquire read of the lockword initially, to ensure
                // visibility of the monitor data structure. Use an explicit fence instead.
                QuasiAtomic::ThreadFenceAcquire();
                NativeMonitor* mon = lockWord.FatLockMonitor();
                if (trylock) {
                    return mon->TryLock(self);
                }
                else {
                    return mon->Lock(self); // Success!
                }
            }
            default: {
                Logger::E("NativeMonitor", "Invalid monitor state %d", lockWord.GetState());
                return E_ILLEGAL_MONITOR_STATE_EXCEPTION;
            }
        }
    }
}

ECode NativeMonitor::MonitorExit(
    /* [in] */ NativeThread* self,
    /* [in] */ NativeObject* obj)
{
    CHECK(self != nullptr);
    CHECK(obj != nullptr);
    self->AssertThreadSuspensionIsAllowable();
    while (true) {
        NativeLockWord lockWord = obj->GetLockWord(true);
        switch (lockWord.GetState()) {
            case NativeLockWord::kUnlocked:
                return FailedUnlock(obj, self->GetThreadId(), 0, nullptr);
            case NativeLockWord::kThinLocked: {
                uint32_t threadId = self->GetThreadId();
                uint32_t ownerThreadId = lockWord.ThinLockOwner();
                if (ownerThreadId != threadId) {
                    return FailedUnlock(obj, threadId, ownerThreadId, nullptr);
                }
                else {
                    // We own the lock, decrease the recursion count.
                    NativeLockWord newLw = NativeLockWord::Default();
                    if (lockWord.ThinLockCount() != 0) {
                        uint32_t newCount = lockWord.ThinLockCount() - 1;
                        newLw = NativeLockWord::FromThinLockId(threadId, newCount);
                    }
                    else {
                        newLw = NativeLockWord::FromDefault();
                    }
                    // TODO: This really only needs memory_order_release, but we currently have
                    // no way to specify that. In fact there seem to be no legitimate uses of SetLockWord
                    // with a final argument of true. This slows down x86 and ARMv7, but probably not v8.
                    obj->SetLockWord(newLw, true);
                    // Success!
                    return NOERROR;
                }
            }
            case NativeLockWord::kFatLocked: {
                NativeMonitor* mon = lockWord.FatLockMonitor();
                return mon->Unlock(self);
            }
            default: {
                Logger::E("NativeMonitor", "Invalid monitor state %d", lockWord.GetState());
                return E_ILLEGAL_MONITOR_STATE_EXCEPTION;
            }
        }
    }
}

ECode NativeMonitor::Notify(
    /* [in] */ NativeThread* self)
{
    CHECK(self != nullptr);
    NativeMutex::AutoLock lock(self, mMonitorLock);
    if (mOwner != self) {
        Logger::E("NativeLockWord", "object not locked by thread before Notify()");
        return E_ILLEGAL_MONITOR_STATE_EXCEPTION;
    }
    while (mWaitSet != nullptr) {
        NativeThread* thread = mWaitSet;
        mWaitSet = thread->GetWaitNext();
        thread->SetWaitNext(nullptr);

        NativeMutex::AutoLock waitLock(self, *thread->GetWaitMutex());
        if (thread->GetWaitMonitor() != nullptr) {
            thread->GetWaitConditionVariable()->Signal(self);
            return NOERROR;
        }
    }
}

ECode NativeMonitor::NotifyAll(
    /* [in] */ NativeThread* self)
{
    CHECK(self != nullptr);
    NativeMutex::AutoLock lock(self, mMonitorLock);
    if (mOwner != self) {
        Logger::E("NativeLockWord", "object not locked by thread before NotifyAll()");
        return E_ILLEGAL_MONITOR_STATE_EXCEPTION;
    }
    while (mWaitSet != nullptr) {
        NativeThread* thread = mWaitSet;
        mWaitSet = thread->GetWaitNext();
        thread->SetWaitNext(nullptr);
        thread->Notify();
    }
    return NOERROR;
}

ECode NativeMonitor::Wait(
    /* [in] */ NativeThread* self,
    /* [in] */ NativeObject* obj,
    /* [in] */ int64_t ms,
    /* [in] */ int32_t ns,
    /* [in] */ Boolean interruptShouldThrow,
    /* [in] */ NativeThreadState why)
{
    CHECK(self != nullptr);
    CHECK(obj != nullptr);
    NativeLockWord lockWord = obj->GetLockWord(true);
    while (lockWord.GetState() != NativeLockWord::kFatLocked) {
        switch (lockWord.GetState()) {
            case NativeLockWord::kUnlocked:
                Logger::E("NativeMonitor", "object not locked by thread before wait()");
                return E_ILLEGAL_MONITOR_STATE_EXCEPTION;
            case NativeLockWord::kThinLocked: {
                uint32_t threadId = self->GetThreadId();
                uint32_t ownerThreadId = lockWord.ThinLockOwner();
                if (ownerThreadId != threadId) {
                    Logger::E("NativeMonitor", "object not locked by thread before wait()");
                    return E_ILLEGAL_MONITOR_STATE_EXCEPTION;
                }
                else {
                    // We own the lock, inflate to enqueue ourself on the Monitor. May fail spuriously so
                    // re-load.
                    Inflate(self, self, obj);
                    lockWord = obj->GetLockWord(true);
                }
                break;
            }
            case NativeLockWord::kFatLocked:
            default: {
                Logger::E("NativeMonitor", "Invalid monitor state %d", lockWord.GetState());
                return E_ILLEGAL_MONITOR_STATE_EXCEPTION;
            }
        }
    }
    NativeMonitor* mon = lockWord.FatLockMonitor();
    return mon->Wait(self, ms, ns, interruptShouldThrow, why);
}

ECode NativeMonitor::DoNotify(
    /* [in] */ NativeThread* self,
    /* [in] */ NativeObject* obj,
    /* [in] */ Boolean notifyAll)
{
    CHECK(self != nullptr);
    CHECK(obj != nullptr);
    NativeLockWord lockWord = obj->GetLockWord(true);
    switch (lockWord.GetState()) {
        case NativeLockWord::kUnlocked: {
            Logger::E("NativeLockWord", "object not locked by thread before Notify()");
            return E_ILLEGAL_MONITOR_STATE_EXCEPTION;
        }
        case NativeLockWord::kThinLocked: {
            uint32_t threadId = self->GetThreadId();
            uint32_t ownerThreadId = lockWord.ThinLockOwner();
            if (ownerThreadId != threadId) {
                Logger::E("NativeLockWord", "object not locked by thread before Notify()");
                return E_ILLEGAL_MONITOR_STATE_EXCEPTION;
            }
            else {
                // We own the lock but there's no Monitor and therefore no waiters.
                return NOERROR;
            }
        }
        case NativeLockWord::kFatLocked: {
            NativeMonitor* mon = lockWord.FatLockMonitor();
            if (notifyAll) {
                return mon->NotifyAll(self);
            }
            else {
                return mon->Notify(self);
            }
        }
        default: {
            Logger::E("NativeLockWord", "Invalid monitor state %d", lockWord.GetState());
            return NOERROR;
        }
    }
}

uint32_t NativeMonitor::GetLockOwnerThreadId(
    /* [in] */ NativeObject* obj)
{
    CHECK(obj != nullptr);
    NativeLockWord lockWord = obj->GetLockWord(true);
    switch (lockWord.GetState()) {
        case NativeLockWord::kUnlocked:
            return NativeThreadList::kInvalidThreadId;
        case NativeLockWord::kThinLocked:
            return lockWord.ThinLockOwner();
        case NativeLockWord::kFatLocked: {
            NativeMonitor* mon = lockWord.FatLockMonitor();
            return mon->GetOwnerThreadId();
        }
        default: {
            return NativeThreadList::kInvalidThreadId;
        }
    }
}

uint32_t NativeMonitor::GetOwnerThreadId()
{
    NativeMutex::AutoLock lock(NativeThread::Current(), mMonitorLock);
    NativeThread* owner = mOwner;
    if (owner != nullptr) {
        return owner->GetThreadId();
    }
    else {
        return NativeThreadList::kInvalidThreadId;
    }
}

//------------------------------------------------------------------------------------------------------

NativeMonitorList::NativeMonitorList()
    : mAllowNewMonitors(true)
    , mMonitorListLock(String("MonitorList lock"), kMonitorListLock)
    , mMonitorAddCondition(String("MonitorList disallow condition"), mMonitorListLock)
{}

NativeMonitorList::~NativeMonitorList()
{
    NativeThread* self = NativeThread::Current();
    NativeMutex::AutoLock lock(self, mMonitorListLock);
    // Release all monitors to the pool.
    // TODO: Is it an invariant that *all* open monitors are in the list? Then we could
    // clear faster in the pool.
    NativeMonitorPool::ReleaseMonitors(self, &mList);
}

void NativeMonitorList::Add(
    /* [in] */ NativeMonitor* m)
{
    NativeThread* self = NativeThread::Current();
    NativeMutex::AutoLock lock(self, mMonitorListLock);
    // CMS needs this to block for concurrent reference processing because an object allocated during
    // the GC won't be marked and concurrent reference processing would incorrectly clear the JNI weak
    // ref. But CC (kUseReadBarrier == true) doesn't because of the to-space invariant.
    while (UNLIKELY(!mAllowNewMonitors)) {
        mMonitorAddCondition.WaitHoldingLocks(self);
    }
    mList.push_front(m);
}

}
}
