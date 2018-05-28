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

#ifndef __CCM_CORE_THREAD_H__
#define __CCM_CORE_THREAD_H__

#include "core/Runnable.h"
#include "ccm.core.IThread.h"
#include "ccm.core.IThreadGroup.h"

namespace ccm {
namespace core {

class COM_PUBLIC Thread
    : public Runnable
    , public IThread
{
public:
    CCM_INTERFACE_DECL();

    ECode constructor();

    ECode Run() override;

    ECode CheckAccess() override;

    ECode CountStackFrames(
        /* [out] */ Integer* frameNum) override;

    ECode Destroy() override;

    ECode GetContextClassLoader(
        /* [out] */ IClassLoader** loader) override;

    ECode GetId(
        /* [out] */ Long* id) override;

    ECode GetName(
        /* [out] */ String* name) override;

    ECode GetPriority(
        /* [out] */ Integer* priority) override;

    ECode GetStackTrace(
        /* [out, callee] */ Array<IStackTraceElement*>** trace) override;

    ECode GetState(
        /* [out] */ ThreadState* state) override;

    ECode GetThreadGroup(
        /* [out] */ IThreadGroup** tg) override;

    ECode GetUncaughtExceptionHandler(
        /* [out] */ IUncaughtExceptionHandler** handler) override;

    ECode DispatchUncaughtException(
        /* [in] */ ECode ec) override;

    ECode Interrupt() override;

    ECode IsAlive(
        /* [out] */ Boolean* alive) override;

    ECode IsDaemon(
        /* [out] */ Boolean* daemon) override;

    ECode IsInterrupted(
        /* [out] */ Boolean* interrupted) override;

    ECode Join() override;

    ECode Join(
        /* [in] */ Long millis) override;

    ECode Join(
        /* [in] */ Long millis,
        /* [in] */ Integer nanos) override;

    ECode ParkFor(
        /* [in] */ Long nanos) override;

    ECode ParkUntil(
        /* [in] */ Long time) override;

    ECode Resume() override;

    ECode SetContextClassLoader(
        /* [in] */ IClassLoader* cl) override;

    ECode SetDaemon(
        /* [in] */ Boolean on) override;

    ECode SetName(
        /* [in] */ const String& name) override;

    ECode SetPriority(
        /* [in] */ Integer newPriority) override;

    ECode SetUncaughtExceptionHandler(
        /* [in] */ IUncaughtExceptionHandler* handler) override;

    ECode Start() override;

    ECode Stop() override;

    ECode Suspend() override;

    ECode Unpark() override;

private:
    ECode NativeCreate(
        /* [in] */ Thread* t,
        /* [in] */ Long stackSize,
        /* [in] */ Boolean daemon);

private:
    friend class NativeThread;

    HANDLE mNative = 0;

    /**
     * The synchronization object responsible for this thread's join/sleep/park operations.
     */
    SyncObject* mLock;

    String mName;

    Integer mPriority;

    /* The group of this thread */
    IThreadGroup* mGroup;
};

}
}

#endif // __CCM_CORE_THREAD_H__
