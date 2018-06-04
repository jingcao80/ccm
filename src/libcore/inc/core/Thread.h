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
#include "ccm.core.IStackTraceElement.h"
#include "ccm.core.IThread.h"
#include "ccm.core.IThreadGroup.h"
#include "ccm.io.IInterruptible.h"
#include <ccmautoptr.h>

using ccm::io::IInterruptible;

namespace ccm {
namespace core {

class COM_PUBLIC Thread
    : public Runnable
    , public IThread
{
private:
    /** Park states */
    class ParkState
    {
    public:
        /** park state indicating unparked */
        static constexpr Integer UNPARKED = 1;

        /** park state indicating preemptively unparked */
        static constexpr Integer PREEMPTIVELY_UNPARKED = 2;

        /** park state indicating parked */
        static constexpr Integer PARKED = 3;
    };

public:
    CCM_INTERFACE_DECL();

    ECode constructor();

    ECode constructor(
        /* [in] */ IThreadGroup* group,
        /* [in] */ const String& name,
        /* [in] */ Integer priority,
        /* [in] */ Boolean daemon);

    ECode constructor(
        /* [in] */ HANDLE peer);

    ECode BlockedOn(
        /* [in] */ IInterruptible* b);

    static ECode GetCurrentThread(
        /* [out] */ IThread** t);

    static void Yield();

    static ECode Sleep(
        /* [in] */ Long millis);

    static ECode Sleep(
        /* [in] */ Long millis,
        /* [in] */ Integer nanos);

    ECode Start() override;

    ECode Run() override;

    ECode Stop() override;

    ECode Interrupt() override;

    static Boolean Interrupted();

    ECode IsInterrupted(
        /* [out] */ Boolean* interrupted) override;

    ECode Destroy() override;

    ECode IsAlive(
        /* [out] */ Boolean* alive) override;

    ECode Suspend() override;

    ECode Resume() override;

    ECode SetPriority(
        /* [in] */ Integer newPriority) override;

    ECode GetPriority(
        /* [out] */ Integer* priority) override;

    ECode SetName(
        /* [in] */ const String& name) override;

    ECode GetName(
        /* [out] */ String* name) override;

    ECode GetThreadGroup(
        /* [out] */ IThreadGroup** tg) override;

    static ECode ActiveCount(
        /* [out] */ Integer* count);

    static ECode Enumerate(
        /* [out] */ Array<IThread*>& tarray,
        /* [out] */ Integer* count);

    ECode CountStackFrames(
        /* [out] */ Integer* frameNum) override;

    ECode Join(
        /* [in] */ Long millis) override;

    ECode Join(
        /* [in] */ Long millis,
        /* [in] */ Integer nanos) override;

    ECode Join() override;

    static void DumpStack();

    ECode SetDaemon(
        /* [in] */ Boolean on) override;

    ECode IsDaemon(
        /* [out] */ Boolean* daemon) override;

    ECode CheckAccess() override;

    ECode ToString(
        /* [out] */ String* desc) override;

    ECode GetContextClassLoader(
        /* [out] */ IClassLoader** loader) override;

    ECode SetContextClassLoader(
        /* [in] */ IClassLoader* cl) override;

    static Boolean HoldsLock(
        /* [in] */ IInterface* obj);

    ECode GetStackTrace(
        /* [out, callee] */ Array<IStackTraceElement*>* trace) override;

    ECode GetId(
        /* [out] */ Long* id) override;

    ECode GetState(
        /* [out] */ ThreadState* state) override;

    ECode Unpark() override;

    ECode ParkFor(
        /* [in] */ Long nanos) override;

    ECode ParkUntil(
        /* [in] */ Long time) override;

    static Thread* From(
        /* [in] */ IThread* t);

private:
    static Integer GetNextThreadNum();

    static Long GetNextThreadID();

    static ECode Sleep(
        /* [in] */ SyncObject* lock,
        /* [in] */ Long millis,
        /* [in] */ Integer nanos);

    ECode Init(
        /* [in] */ IThreadGroup* g,
        /* [in] */ IRunnable* target,
        /* [in] */ const String& name,
        /* [in] */ Long stackSize);

    void Init2(
        /* [in] */ IThread* parent);

    ECode NativeCreate(
        /* [in] */ Thread* t,
        /* [in] */ Long stackSize,
        /* [in] */ Boolean daemon);

    void Exit();

    Boolean NativeHoldsLock(
        /* [in] */ IInterface* obj);

    static Array<IStackTraceElement*> Get_EMPTY_STACK_TRACE();

    void NativeSetName(
        /* [in] */ const String& newName);

    void NativeSetPriority(
        /* [in] */ Integer newPriority);

    ThreadState NativeGetStatus(
        /* [in] */ Boolean hasBeenStarted);

    ECode NativeInterrupt();

    static SyncObject* GetStaticLock();

private:
    friend class NativeThread;

    /**
     * The synchronization object responsible for this thread's join/sleep/park operations.
     */
    SyncObject mLock;

    HANDLE mNative = 0;

    Boolean mStarted = false;

    String mName;

    Integer mPriority;

    /* Whether or not the thread is a daemon thread. */
    Boolean mDaemon = false;

    /* What will be run. */
    AutoPtr<IRunnable> mTarget;

    /* The group of this thread */
    IThreadGroup* mGroup;

    /* The context ClassLoader for this thread */
    AutoPtr<IClassLoader> mContextClassLoader;

    Long mStackSize;

    Long mTid;

    Integer mThreadStatus = 0;

    AutoPtr<IInterruptible> mBlocker;

    SyncObject mBlockerLock;

    static constexpr Integer NANOS_PER_MILLI = 1000000;

    Integer mParkState = ParkState::UNPARKED;
};

inline Thread* Thread::From(
    /* [in] */ IThread* t)
{
    return (Thread*)t;
}

}
}

#endif // __CCM_CORE_THREAD_H__
