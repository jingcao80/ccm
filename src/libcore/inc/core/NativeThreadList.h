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

#ifndef __CCM_CORE_NATIVETHREADLIST_H__
#define __CCM_CORE_NATIVETHREADLIST_H__

#include "core/NativeThread.h"
#include "core/NativeTimeUtils.h"
#include <bitset>
#include <list>

namespace ccm {
namespace core {

class NativeThread;
class Thread;

class NativeThreadList
{
public:
    explicit NativeThreadList(
        /* [in] */ uint64_t threadSuspendTimeoutNs);

    void Resume(
        /* [in] */ NativeThread* thread,
        /* [in] */ Boolean forDebugger = false);

    // Suspend a thread using a peer, typically used by the debugger. Returns the thread on success,
    // else null. The peer is used to identify the thread to avoid races with the thread terminating.
    // If the thread should be suspended then value of requestSuspension should be true otherwise
    // the routine will wait for a previous suspend request. If the suspension times out then *timeout
    // is set to true.
    NativeThread* SuspendThreadByPeer(
        /* [in] */ Thread* peer,
        /* [in] */ Boolean requestSuspension,
        /* [in] */ Boolean debugSuspension,
        /* [out] */ Boolean* timedOut);

    // Suspend a thread using its thread id, typically used by lock/monitor inflation. Returns the
    // thread on success else null. The thread id is used to identify the thread to avoid races with
    // the thread terminating. Note that as thread ids are recycled this may not suspend the expected
    // thread, that may be terminating. If the suspension times out then *timeout is set to true.
    NativeThread* SuspendThreadByThreadId(
        /* [in] */ uint32_t threadId,
        /* [in] */ Boolean debugSuspension,
        /* [out] */ Boolean* timedOut);

    // Find an existing thread (or self) by its thread id (not tid).
    NativeThread* FindThreadByThreadId(
        /* [in] */ uint32_t threadId);

    void Register(
        /* [in] */ NativeThread* self);

    void Unregister(
        /* [in] */ NativeThread* self);

private:
    uint32_t AllocThreadId(
        /* [in] */ NativeThread* self);

    void ReleaseThreadId(
        /* [in] */ NativeThread* self,
        /* [in] */ uint32_t id);

    Boolean Contains(
        /* [in] */ NativeThread* thread);

public:
    static constexpr uint32_t kMaxThreadId = 0xFFFF;
    static constexpr uint32_t kMainThreadId = 1;
    static constexpr uint64_t kDefaultThreadSuspendTimeout = MsToNs(10000);

private:
    friend class NativeThread;

    std::bitset<kMaxThreadId> mAllocatedIds;

    // The actual list of all threads.
    std::list<NativeThread*> mList;

    // Ongoing suspend all requests, used to ensure threads added to list_ respect SuspendAll.
    int mSuspendAllCount;
    int mDebugSuspendAllCount;

    // Number of threads unregistering, ~ThreadList blocks until this hits 0.
    int mUnregisteringCount;

    // Thread suspension timeout in nanoseconds.
    const uint64_t mThreadSuspendTimeoutNs;
};

}
}

#endif // __CCM_CORE_NATIVETHREADLIST_H__
