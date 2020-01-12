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

#ifndef __COMO_CORE_NATIVERUNTIME_H__
#define __COMO_CORE_NATIVERUNTIME_H__

#include "como/core/NativeMutex.h"
#include "como/core/NativeRuntimeCallbacks.h"
#include "como.core.IThreadGroup.h"
#include <comosp.h>
#include <comotypes.h>
#include <memory>

namespace como {
namespace core {

class NativeMonitorList;
class NativeMonitorPool;
class NativeThread;
class NativeThreadList;

class NativeRuntime
{
public:
    ~NativeRuntime();

    // Starts a runtime, which may cause threads to be started and code to run.
    Boolean Start();

    static Boolean Create();

    Boolean IsShuttingDown(
        /* [in] */ NativeThread* self);

    Boolean IsShuttingDownLocked();

    void StartThreadBirth();

    void EndThreadBirth();

    static NativeRuntime* Current();

    // Returns the "main" ThreadGroup, used when attaching user threads.
    AutoPtr<IThreadGroup> GetMainThreadGroup() const;

    size_t GetDefaultStackSize() const;

    size_t GetMaxSpinsBeforeThinLockInflation() const;

    NativeMonitorList* GetMonitorList() const;

    NativeMonitorPool* GetMonitorPool() const;

    NativeThreadList* GetThreadList() const;

    Boolean ExplicitStackOverflowChecks() const;

    NativeRuntimeCallbacks* GetRuntimeCallbacks();

    void InitThreadGroups(
        /* [in] */ NativeThread* self);

private:
    NativeRuntime();

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

    Boolean mStarted;

    // New flag added which tells us if the runtime has finished starting. If
    // this flag is set then the Daemon threads are created and the class loader
    // is created. This flag is needed for knowing if its safe to request CMS.
    Boolean mFinishedStarting;

    AutoPtr<IThreadGroup> mMainThreadGroup;

    AutoPtr<IThreadGroup> mSystemThreadGroup;

    Boolean mImplicitSoChecks;         // StackOverflow checks are implicit.

    // Whether or not the sig chain (and implicitly the fault handler) should be
    // disabled.
    Boolean mNoSigChain;

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

#endif // __COMO_CORE_NATIVERUNTIME_H__
