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
#include <list>

namespace ccm {
namespace core {

class NativeThread;

class NativeThreadList
{
public:
    void Resume(
        /* [in] */ NativeThread* thread,
        /* [in] */ Boolean forDebugger = false);

    // Suspend a thread using its thread id, typically used by lock/monitor inflation. Returns the
    // thread on success else null. The thread id is used to identify the thread to avoid races with
    // the thread terminating. Note that as thread ids are recycled this may not suspend the expected
    // thread, that may be terminating. If the suspension times out then *timeout is set to true.
    NativeThread* SuspendThreadByThreadId(
        /* [in] */ uint32_t threadId,
        /* [in] */ Boolean debugSuspension,
        /* [in] */ Boolean* timedOut);

private:
    Boolean Contains(
        /* [in] */ NativeThread* thread);

private:
    // The actual list of all threads.
    std::list<NativeThread*> mList;

    // Thread suspension timeout in nanoseconds.
    const uint64_t mThreadSuspendTimeoutNs;
};

}
}

#endif // __CCM_CORE_NATIVETHREADLIST_H__
