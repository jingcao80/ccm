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

#ifndef __COMO_CORE_THREAD_H__
#define __COMO_CORE_THREAD_H__

#include "como/core/Runnable.h"
#include "como/core/ThreadLocal.h"
#include "como/core/volatile.h"
#include "como.core.IStackTraceElement.h"
#include "como.core.IThread.h"
#include "como.core.IThreadGroup.h"
#include "como.io.IInterruptible.h"
#include <ccmautoptr.h>

using como::io::IInterruptible;

namespace como {
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
    COMO_INTERFACE_DECL();

    ECode Constructor();

    ECode Constructor(
        /* [in] */ IThreadGroup* group,
        /* [in] */ const String& name,
        /* [in] */ Integer priority,
        /* [in] */ Boolean daemon);

    ECode Constructor(
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

public:
    /**
     * The argument supplied to the current call to
     * como::util::concurrent::locks::LockSupport::Park.
     * Set by como::util::concurrent::locks::LockSupport::SetBlocker
     * Accessed using como::util::concurrent::locks::LockSupport::GetBlocker
     */
    VOLATILE AutoPtr<IInterface> mParkBlocker;

    // The following three initially uninitialized fields are exclusively
    // managed by class como::util::concurrent::ThreadLocalRandom. These
    // fields are used to build the high-performance PRNGs in the
    // concurrent code, and we can not risk accidental false sharing.
    // Hence, the fields are isolated with @Contended.

    /** The current seed for a ThreadLocalRandom */
    Long mThreadLocalRandomSeed;

    /** Probe hash value; nonzero if threadLocalRandomSeed initialized */
    Integer mThreadLocalRandomProbe;

    /** Secondary seed isolated from public ThreadLocalRandom sequence */
    Integer mThreadLocalRandomSecondarySeed;

private:
    /**
     * The synchronization object responsible for this thread's join/sleep/park operations.
     */
    SyncObject mLock;

    VOLATILE HANDLE mNative = 0;

    Boolean mStarted = false;

    VOLATILE String mName;

    Integer mPriority;

    /* Whether or not the thread is a daemon thread. */
    Boolean mDaemon = false;

    /* What will be run. */
    AutoPtr<IRunnable> mTarget;

    /* The group of this thread */
    IThreadGroup* mGroup;

    /* The context ClassLoader for this thread */
    AutoPtr<IClassLoader> mContextClassLoader;

    AutoPtr<ThreadLocal::ThreadLocalMap> mThreadLocals;

    Long mStackSize;

    Long mTid;

    VOLATILE Integer mThreadStatus = 0;

    VOLATILE AutoPtr<IInterruptible> mBlocker;

    SyncObject mBlockerLock;

    static constexpr Integer NANOS_PER_MILLI = 1000000;

    Integer mParkState = ParkState::UNPARKED;

    friend class NativeThread;
    friend class Runtime;
    friend class ThreadLocal;
};

inline Thread* Thread::From(
    /* [in] */ IThread* t)
{
    return (Thread*)t;
}

}
}

#endif // __COMO_CORE_THREAD_H__
