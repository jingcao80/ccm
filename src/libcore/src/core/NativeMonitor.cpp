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
#include "core/NativeLockWord.h"
#include "core/NativeMonitor.h"
#include "core/NativeMutex.h"
#include "core/NativeObject.h"
#include <ccmlogger.h>

namespace ccm {
namespace core {

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

ECode NativeMonitor::DoNotify(
    /* [in] */ NativeThread* self,
    /* [in] */ NativeObject* obj,
    /* [in] */ Boolean notifyAll)
{
    CHECK(self != nullptr);
    CHECK(obj != nullptr);
    NativeLockWord lockWord = obj->GetLockWord(true);
    switch (lockWord.GetState()) {
        case NativeLockWord::kHashCode:
            // Fall-through.
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

}
}
