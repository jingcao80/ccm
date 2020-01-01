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

#include "como/core/CThreadGroup.h"
#include "como/core/NativeMonitor.h"
#include "como/core/NativeMonitorPool.h"
#include "como/core/NativeMutex.h"
#include "como/core/NativeRuntime.h"
#include "como/core/NativeThread.h"
#include "como/core/NativeThreadList.h"

namespace como {
namespace core {

NativeRuntime* NativeRuntime::sInstance = nullptr;

NativeRuntime::NativeRuntime()
    : mDefaultStackSize(0)
    , mMaxSpinsBeforeThinLockInflation(NativeMonitor::kDefaultMaxSpinsBeforeThinLockInflation)
    , mMonitorList(nullptr)
    , mMonitorPool(nullptr)
    , mThreadList(nullptr)
    , mThreadsBeingBorn(0)
    , mShutdownCond(new NativeConditionVariable(String("Runtime shutdown"), *Locks::sRuntimeShutdownLock))
    , mShuttingDown(false)
    , mShuttingDownStarted(false)
    , mStarted(false)
    , mFinishedStarting(false)
    , mImplicitSoChecks(false)
    , mNoSigChain(false)
{
    mCallbacks.reset(new NativeRuntimeCallbacks());
}

NativeRuntime::~NativeRuntime()
{
    CHECK(sInstance == nullptr || sInstance == this);
    sInstance = nullptr;
}

Boolean NativeRuntime::Create()
{
    Locks::Init();

    if (sInstance != nullptr) {
        return false;
    }
    sInstance = new NativeRuntime();
    if (!sInstance->Init()) {
        sInstance = nullptr;
        return false;
    }
    return true;
}

Boolean NativeRuntime::Start()
{
    Logger::V("NativeRuntime", "Start entering");

    CHECK(!mNoSigChain);

    // Restore main thread state to kNative as expected by native code.
    NativeThread* self = NativeThread::Current();

    self->TransitionFromRunnableToSuspended(kNative);

    mStarted = true;

    // Initialize well known thread group values that may be accessed threads while attaching.
    InitThreadGroups(self);

    NativeThread::FinishStartup();

    Logger::V("NativeRuntime", "Start exiting");
    mFinishedStarting = true;

    return true;
}

void NativeRuntime::EndThreadBirth()
{
    CHECK(mThreadsBeingBorn > 0);
    mThreadsBeingBorn--;
    if (mShuttingDownStarted && mThreadsBeingBorn == 0) {
        mShutdownCond->Broadcast(NativeThread::Current());
    }
}

Boolean NativeRuntime::IsShuttingDown(
    /* [in] */ NativeThread* self)
{
    NativeMutex::AutoLock lock(self, *Locks::sRuntimeShutdownLock);
    return IsShuttingDownLocked();
}

Boolean NativeRuntime::Init()
{
    mDefaultStackSize = 1 * MB;

    mMaxSpinsBeforeThinLockInflation = NativeMonitor::kDefaultMaxSpinsBeforeThinLockInflation;

    mMonitorList = new NativeMonitorList;
    mMonitorPool = NativeMonitorPool::Create();
    mThreadList = new NativeThreadList(NativeThreadList::kDefaultThreadSuspendTimeout);

    // Change the implicit checks flags based on runtime architecture.
    switch (kRuntimeISA) {
        case kArm64:
        case kX86_64:
            // Installing stack protection does not play well with valgrind.
            mImplicitSoChecks = true;
            break;
        default:
            // Keep the defaults.
            break;
    }

    NativeThread::Startup();

    NativeThread* self = NativeThread::Attach(String("main"), false, nullptr, false);
    CHECK(self->GetThreadId() == NativeThreadList::kMainThreadId);
    CHECK(self != nullptr);

    return true;
}

void NativeRuntime::InitThreadGroups(
    /* [in] */ NativeThread* self)
{
    mMainThreadGroup = CThreadGroup::GetMainThreadGroup();
    CHECK(mMainThreadGroup != nullptr);
    mSystemThreadGroup = CThreadGroup::GetSystemThreadGroup();
    CHECK(mSystemThreadGroup);
}

AutoPtr<IThreadGroup> NativeRuntime::GetMainThreadGroup() const
{
    CHECK(mMainThreadGroup != nullptr);
    return mMainThreadGroup;
}

NativeRuntimeCallbacks* NativeRuntime::GetRuntimeCallbacks()
{
    return mCallbacks.get();
}

}
}
