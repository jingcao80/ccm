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
#include "core/NativeScopedThreadStateChange.h"
#include "core/NativeThreadList.h"
#include "core/NativeTimeUtils.h"
#include <ccmlogger.h>
#include <algorithm>

namespace ccm {
namespace core {

// Use 0 since we want to yield to prevent blocking for an unpredictable amount of time.
static constexpr useconds_t kThreadSuspendInitialSleepUs = 0;
static constexpr useconds_t kThreadSuspendMaxYieldUs = 3000;
static constexpr useconds_t kThreadSuspendMaxSleepUs = 5000;

Boolean NativeThreadList::Contains(
    /* [in] */ NativeThread* thread)
{
    return find(mList.begin(), mList.end(), thread) != mList.end();
}

// Unlike suspending all threads where we can wait to acquire the mutator_lock_, suspending an
// individual thread requires polling. delay_us is the requested sleep wait. If delay_us is 0 then
// we use sched_yield instead of calling usleep.
static void ThreadSuspendSleep(
    /* [in] */ useconds_t delayUs)
{
    if (delayUs == 0) {
        sched_yield();
    }
    else {
        usleep(delayUs);
    }
}

void NativeThreadList::Resume(
    /* [in] */ NativeThread* thread,
    /* [in] */ Boolean forDebugger)
{
    NativeThread* self = NativeThread::Current();
    CHECK(thread != self);
    Logger::V("NativeThreadList", "Resume(%p) starting...",
            reinterpret_cast<void*>(thread));

    {
        // To check Contains.
        NativeMutex::AutoLock lock(self, *Locks::sThreadListLock);
        // To check IsSuspended.
        NativeMutex::AutoLock lock2(self, *Locks::sThreadSuspendCountLock);
        CHECK(thread->IsSuspended());
        if (!Contains(thread)) {
            // We only expect threads within the thread-list to have been suspended otherwise we can't
            // stop such threads from delete-ing themselves.
            Logger::E("NativeThreadList", "Resume(%p) thread not within thread list",
                    reinterpret_cast<void*>(thread));
            return;
        }
        Boolean updated = thread->ModifySuspendCount(self, -1, nullptr, forDebugger);
        CHECK(updated);
    }

    {
        Logger::V("NativeThreadList", "Resume(%p) waking others",
                reinterpret_cast<void*>(thread));
        NativeMutex::AutoLock lock(self, *Locks::sThreadSuspendCountLock);
        NativeThread::sResumeCond->Broadcast(self);
    }

    Logger::V("NativeThreadList", "Resume(%p) complete", reinterpret_cast<void*>(thread));
}

NativeThread* NativeThreadList::SuspendThreadByThreadId(
    /* [in] */ uint32_t threadId,
    /* [in] */ Boolean debugSuspension,
    /* [in] */ Boolean* timedOut)
{
    const uint64_t startTime = NanoTime();
    useconds_t sleepUs = kThreadSuspendInitialSleepUs;
    *timedOut = false;
    NativeThread* suspendedThread = nullptr;
    NativeThread* const self = NativeThread::Current();
    CHECK(threadId != kInvalidThreadId);
    Logger::V("NativeThreadList", "SuspendThreadByThreadId starting");
    while (true) {
        {
            // Note: this will transition to runnable and potentially suspend. We ensure only one thread
            // is requesting another suspend, to avoid deadlock, by requiring this function be called
            // holding Locks::thread_list_suspend_thread_lock_. Its important this thread suspend rather
            // than request thread suspension, to avoid potential cycles in threads requesting each other
            // suspend.
            ScopedObjectAccess soa(self);
            NativeMutex::AutoLock lock(self, *Locks::sThreadListLock);
            NativeThread* thread = nullptr;
            for (const auto& it : mList) {
                if (it->GetThreadId() == threadId) {
                    thread = it;
                    break;
                }
            }
            if (thread == nullptr) {
                CHECK(suspendedThread == nullptr);
                // There's a race in inflating a lock and the owner giving up ownership and then dying.
                Logger::W("NativeThreadList", "No such thread id for suspend: %d", threadId);
                return nullptr;
            }
            Logger::V("NativeThreadList", "SuspendThreadByThreadId found thread: %s",
                    thread->ShortDump().string());
            CHECK(Contains(thread));
            {
                NativeMutex::AutoLock lock2(self, *Locks::sThreadSuspendCountLock);
                if (suspendedThread == nullptr) {
                    if (self->GetSuspendCount() > 0) {
                        // We hold the suspend count lock but another thread is trying to suspend us. Its not
                        // safe to try to suspend another thread in case we get a cycle. Start the loop again
                        // which will allow this thread to be suspended.
                     continue;
                    }
                    Boolean updated = thread->ModifySuspendCount(self, +1, nullptr, debugSuspension);
                    CHECK(updated);
                    suspendedThread = thread;
                }
                else {
                    CHECK(suspendedThread == thread);
                    // If the caller isn't requesting suspension, a suspension should have already occurred.
                    CHECK(thread->GetSuspendCount() > 0);
                }
                // IsSuspended on the current thread will fail as the current thread is changed into
                // Runnable above. As the suspend count is now raised if this is the current thread
                // it will self suspend on transition to Runnable, making it hard to work with. It's simpler
                // to just explicitly handle the current thread in the callers to this code.
                CHECK(thread != self);
                Logger::V("NativeThreadList", "Attempt to suspend the current thread for the debugger");
                // If thread is suspended (perhaps it was already not Runnable but didn't have a suspend
                // count, or else we've waited and it has self suspended) or is the current thread, we're
                // done.
                if (thread->IsSuspended()) {
                    Logger::V("NativeThreadList", "SuspendThreadByThreadId thread suspended: %s",
                            thread->ShortDump().string());
                    return thread;
                }
                const uint64_t totalDelay = NanoTime() - startTime;
                if (totalDelay >= mThreadSuspendTimeoutNs) {
                    Logger::W("NativeThreadList", "Thread suspension timed out: %d", threadId);
                    if (suspendedThread != nullptr) {
                        Boolean updated = thread->ModifySuspendCount(soa.Self(), -1, nullptr, debugSuspension);
                        CHECK(updated);
                    }
                    *timedOut = true;
                    return nullptr;
                }
                else if (sleepUs == 0 &&
                    totalDelay > static_cast<uint64_t>(kThreadSuspendMaxYieldUs) * 1000) {
                    // We have spun for kThreadSuspendMaxYieldUs time, switch to sleeps to prevent
                    // excessive CPU usage.
                    sleepUs = kThreadSuspendMaxYieldUs / 2;
                }
            }
            // Release locks and come out of runnable state.
        }
        Logger::V("NativeThreadList", "SuspendThreadByThreadId waiting to allow thread chance to suspend");
        ThreadSuspendSleep(sleepUs);
        sleepUs = std::min(sleepUs * 2, kThreadSuspendMaxSleepUs);
    }
}

NativeThread* NativeThreadList::FindThreadByThreadId(
    /* [in] */ uint32_t threadId)
{
    for (const auto& thread : mList) {
        if (thread->GetThreadId() == threadId) {
            return thread;
        }
    }
    return nullptr;
}

}
}
