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

#include "core/AutoLock.h"
#include "core/NativeObject.h"
#include "core/NativeRuntime.h"
#include "core/NativeThread.h"
#include "core/NativeThreadList.h"
#include "core/SyncObject.h"
#include "core/Thread.h"

namespace ccm {
namespace core {

CCM_INTERFACE_IMPL_1(Thread, Runnable, IThread);

Integer Thread::GetNextThreadNum()
{
    static Integer sThreadInitNumber = 0;
    AutoLock lock(GetStaticLock());
    return sThreadInitNumber++;
}

Long Thread::GetNextThreadID()
{
    static Long sThreadSeqNumber = 0;
    AutoLock lock(GetStaticLock());
    return ++sThreadSeqNumber;
}

ECode Thread::GetCurrentThread(
    /* [out] */ IThread** t)
{
    VALIDATE_NOT_NULL(t);

    NativeThread* self = NativeThread::Current();
    Thread* tPeer = self->GetPeerThread();
    *t = (IThread*)tPeer;
    REFCOUNT_ADD(*t);
    return NOERROR;
}

ECode Thread::Init(
    /* [in] */ IThreadGroup* g,
    /* [in] */ IRunnable* target,
    /* [in] */ const String& name,
    /* [in] */ Long stackSize)
{
    AutoPtr<IThread> parent;
    GetCurrentThread((IThread**)&parent);
    AutoPtr<IThreadGroup> gg = g;
    if (gg == nullptr) {
        parent->GetThreadGroup((IThreadGroup**)&gg);
    }

    gg->AddUnstarted();
    mGroup = g;

    mTarget = target;
    parent->GetPriority(&mPriority);
    parent->IsDaemon(&mDaemon);
    SetName(name);

    Init2(parent);

    mStackSize = stackSize;
    mTid = GetNextThreadID();
    return NOERROR;
}

void Thread::Init2(
    /* [in] */ IThread* parent)
{
    parent->GetContextClassLoader((IClassLoader**)&mContextClassLoader);
}

ECode Thread::GetPriority(
    /* [out] */ Integer* priority)
{
    VALIDATE_NOT_NULL(priority);

    *priority = mPriority;
    return NOERROR;
}

ECode Thread::SetName(
    /* [in] */ const String& name)
{
    FAIL_RETURN(CheckAccess());
    if (mName.IsNullOrEmpty()) {
        return E_NULL_POINTER_EXCEPTION;
    }

    {
        AutoLock lock(this);
        mName = name;
        Boolean alive;
        if (IsAlive(&alive), alive) {
            NativeSetName(mName);
        }
    }

    return NOERROR;
}

ECode Thread::GetThreadGroup(
    /* [out] */ IThreadGroup** tg)
{
    VALIDATE_NOT_NULL(tg);

    ThreadState ts;
    if (GetState(&ts), ts == ThreadState::TERMINATED) {
        *tg = nullptr;
        return NOERROR;
    }
    *tg = mGroup;
    REFCOUNT_ADD(*tg);
    return NOERROR;
}

ECode Thread::CheckAccess()
{
    return NOERROR;
}

void Thread::NativeSetName(
    /* [in] */ const String& newName)
{
    NativeThread* self = NativeThread::Current();
    if (this == self->GetPeerThread()) {
        self->SetThreadName(newName);
        return;
    }
    // Suspend thread to avoid it from killing itself while we set its name. We don't just hold the
    // thread list lock to avoid this, as setting the thread name causes mutator to lock/unlock
    // in the DDMS send code.
    NativeThreadList* threadList = NativeRuntime::Current()->GetThreadList();
    Boolean timedOut;
    // Take suspend thread lock to avoid races with threads trying to suspend this one.
    NativeThread* thread = threadList->SuspendThreadByPeer(this, true, false, &timedOut);
    if (thread != nullptr) {
        {
            ScopedObjectAccess soa(self);
            thread->SetThreadName(newName);
        }
        threadList->Resume(thread, false);
    }
    else if (timedOut) {
        Logger::E("Thread", "Trying to set thread name to '%s' failed as the thread "
                "failed to suspend within a generous timeout.", newName.string());
    }
}

ECode Thread::constructor()
{
    String name = String::Format("Thread-%d", GetNextThreadNum());
    return Init(nullptr, nullptr, name, 0);
}

ECode Thread::constructor(
    /* [in] */ IThreadGroup* group,
    /* [in] */ const String& name,
    /* [in] */ Integer priority,
    /* [in] */ Boolean daemon)
{
    mGroup = group;
    mGroup->AddUnstarted();
    // Must be tolerant of threads without a name.
    String threadName = name;
    if (threadName.IsNull()) {
        threadName = String::Format("Thread-%d", GetNextThreadNum());
    }

    mName = name;

    mPriority = priority;
    mDaemon = daemon;
    AutoPtr<IThread> parent;
    GetCurrentThread((IThread**)&parent);
    Init2(parent);
    mTid = GetNextThreadID();
    return NOERROR;
}

ECode Thread::Run()
{
    return NOERROR;
}

ECode Thread::CountStackFrames(
    /* [out] */ Integer* frameNum)
{
    return NOERROR;
}

ECode Thread::Destroy()
{
    return NOERROR;
}

ECode Thread::GetContextClassLoader(
    /* [out] */ IClassLoader** loader)
{
    return NOERROR;
}

ECode Thread::GetId(
    /* [out] */ Long* id)
{
    return NOERROR;
}

ECode Thread::GetName(
    /* [out] */ String* name)
{
    return NOERROR;
}

ECode Thread::GetStackTrace(
    /* [out, callee] */ Array<IStackTraceElement*>** trace)
{
    return NOERROR;
}

ECode Thread::GetState(
    /* [out] */ ThreadState* state)
{
    return NOERROR;
}

ECode Thread::GetUncaughtExceptionHandler(
    /* [out] */ IUncaughtExceptionHandler** handler)
{
    return NOERROR;
}

ECode Thread::DispatchUncaughtException(
    /* [in] */ ECode ec)
{
    return NOERROR;
}

ECode Thread::Interrupt()
{
    return NOERROR;
}

ECode Thread::IsAlive(
    /* [out] */ Boolean* alive)
{
    return NOERROR;
}

ECode Thread::IsDaemon(
    /* [out] */ Boolean* daemon)
{
    VALIDATE_NOT_NULL(daemon);

    *daemon = mDaemon;
    return NOERROR;
}

ECode Thread::IsInterrupted(
    /* [out] */ Boolean* interrupted)
{
    return NOERROR;
}

ECode Thread::Join()
{
    return NOERROR;
}

ECode Thread::Join(
    /* [in] */ Long millis)
{
    return NOERROR;
}

ECode Thread::Join(
    /* [in] */ Long millis,
    /* [in] */ Integer nanos)
{
    return NOERROR;
}

ECode Thread::ParkFor(
    /* [in] */ Long nanos)
{
    return NOERROR;
}

ECode Thread::ParkUntil(
    /* [in] */ Long time)
{
    return NOERROR;
}

ECode Thread::Resume()
{
    return NOERROR;
}

ECode Thread::SetContextClassLoader(
    /* [in] */ IClassLoader* cl)
{
    return NOERROR;
}

ECode Thread::SetDaemon(
    /* [in] */ Boolean on)
{
    return NOERROR;
}

ECode Thread::SetPriority(
    /* [in] */ Integer newPriority)
{
    return NOERROR;
}

ECode Thread::SetUncaughtExceptionHandler(
    /* [in] */ IUncaughtExceptionHandler* handler)
{
    return NOERROR;
}

ECode Thread::Start()
{
    return NOERROR;
}

ECode Thread::Stop()
{
    return NOERROR;
}

ECode Thread::Suspend()
{
    return NOERROR;
}

ECode Thread::Unpark()
{
    return NOERROR;
}

ECode Thread::NativeCreate(
    /* [in] */ Thread* t,
    /* [in] */ Long stackSize,
    /* [in] */ Boolean daemon)
{
    return NativeThread::CreateNativeThread(t, stackSize, daemon);
}

SyncObject* Thread::GetStaticLock()
{
    static SyncObject* sLock = new SyncObject();
    return sLock;
}

}
}
