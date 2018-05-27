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

#ifndef __CCM_CORE_NATIVERUNTIME_H__
#define __CCM_CORE_NATIVERUNTIME_H__

#include "core/NativeMutex.h"
#include "core/NativeRuntimeCallbacks.h"
#include <ccmtypes.h>
#include <memory>

namespace ccm {
namespace core {

class NativeMonitorList;
class NativeMonitorPool;
class NativeThread;
class NativeThreadList;

class NativeRuntime
{
public:
    static Boolean Create();

    Boolean IsShuttingDown(
        /* [in] */ NativeThread* self);

    Boolean IsShuttingDownLocked();

    void StartThreadBirth();

    void EndThreadBirth();

    static NativeRuntime* Current();

    size_t GetDefaultStackSize() const;

    size_t GetMaxSpinsBeforeThinLockInflation() const;

    NativeMonitorList* GetMonitorList() const;

    NativeMonitorPool* GetMonitorPool() const;

    NativeThreadList* GetThreadList() const;

    Boolean ExplicitStackOverflowChecks() const;

    NativeRuntimeCallbacks* GetRuntimeCallbacks();

private:
    Boolean Init();

private:
    static NativeRuntime* sInstance;

    // The default stack size for managed threads created by the runtime.
    size_t mDefaultStackSize;

    // The number of spins that are done before thread suspension is used to forcibly inflate.
    size_t mMaxSpinsBeforeThinLockInflation;

    NativeMonitorList* mMonitorList;

    NativeMonitorPool* mMonitorPool;

    NativeThreadList* mThreadList;

    // A non-zero value indicates that a thread has been created but not yet initialized. Guarded by
    // the shutdown lock so that threads aren't born while we're shutting down.
    size_t mThreadsBeingBorn;

    // Waited upon until no threads are being born.
    std::unique_ptr<NativeConditionVariable> mShutdownCond;

    // Set when runtime shutdown is past the point that new threads may attach.
    Boolean mShuttingDown;

    // The runtime is starting to shutdown but is blocked waiting on mShutdownCond.
    Boolean mShuttingDownStarted;

    Boolean mImplicitSoChecks;         // StackOverflow checks are implicit.

    std::unique_ptr<NativeRuntimeCallbacks> mCallbacks;
};

inline Boolean NativeRuntime::IsShuttingDownLocked()
{
    return mShuttingDown;
}

inline void NativeRuntime::StartThreadBirth()
{
    mThreadsBeingBorn++;
}

inline NativeRuntime* NativeRuntime::Current()
{
    return sInstance;
}

inline size_t NativeRuntime::GetDefaultStackSize() const
{
    return mDefaultStackSize;
}

inline size_t NativeRuntime::GetMaxSpinsBeforeThinLockInflation() const
{
    return mMaxSpinsBeforeThinLockInflation;
}

inline NativeMonitorList* NativeRuntime::GetMonitorList() const
{
    return mMonitorList;
}

inline NativeMonitorPool* NativeRuntime::GetMonitorPool() const
{
    return mMonitorPool;
}

inline NativeThreadList* NativeRuntime::GetThreadList() const
{
    return mThreadList;
}

inline Boolean NativeRuntime::ExplicitStackOverflowChecks() const
{
    return !mImplicitSoChecks;
}

}
}

#endif // __CCM_CORE_NATIVERUNTIME_H__
