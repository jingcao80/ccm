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

#include "ccm/core/NativeRuntime.h"
#include "ccm/core/NativeScopedThreadStateChange.h"
#include "ccm/core/NativeThread.h"

namespace ccm {
namespace core {

ScopedThreadStateChange::ScopedThreadStateChange(
    /* [in] */ NativeThread* self,
    /* [in] */ NativeThreadState newThreadState)
    : mSelf(self)
    , mThreadState(newThreadState)
    , mExpectedHasNoThread(false)
{
    if (UNLIKELY(mSelf == nullptr)) {
        // Value chosen arbitrarily and won't be used in the destructor since thread_ == null.
        mOldThreadState = kTerminated;
        NativeRuntime* runtime = NativeRuntime::Current();
        CHECK(runtime == nullptr || !runtime->IsStarted() || runtime->IsShuttingDown(mSelf));
    }
    else {
        CHECK(self == Thread::Current());
        // Read state without locks, ok as state is effectively thread local and we're not interested
        // in the suspend count (this will be handled in the runnable transitions).
        mOldThreadState = self->GetState();
        if (mOldThreadState != newThreadState) {
            if (newThreadState == kRunnable) {
                mSelf->TransitionFromSuspendedToRunnable();
            }
            else if (mOldThreadState == kRunnable) {
                mSelf->TransitionFromRunnableToSuspended(newThreadState);
            }
            else {
                // A suspended transition to another effectively suspended transition, ok to use Unsafe.
                mSelf->SetState(newThreadState);
            }
        }
    }
}

ScopedThreadStateChange::~ScopedThreadStateChange()
{
    if (UNLIKELY(mSelf == nullptr)) {
        if (!mExpectedHasNoThread) {
           NativeRuntime* runtime = NativeRuntime::Current();
           Boolean shuttingDown = (runtime == nullptr) || runtime->IsShuttingDown(nullptr);
           CHECK(shuttingDown);
        }
    }
    else {
        if (mOldThreadState != mThreadState) {
            if (mOldThreadState == kRunnable) {
                mSelf->TransitionFromSuspendedToRunnable();
            }
            else if (mThreadState == kRunnable) {
                mSelf->TransitionFromRunnableToSuspended(mOldThreadState);
            }
            else {
                // A suspended transition to another effectively suspended transition, ok to use Unsafe.
                mSelf->SetState(mOldThreadState);
            }
        }
    }
}


}
}
