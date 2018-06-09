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
#include "core/NativeMonitor.h"
#include "core/NativeMutex.h"
#include "core/NativeObject.h"
#include "core/NativeRuntime.h"
#include "core/NativeThread.h"
#include "core/NativeThreadList.h"
#include "core/SyncObject.h"
#include "core/System.h"
#include "core/Thread.h"
#include "ccm.core.ILong.h"
#include <unwind.h>

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

ECode Thread::BlockedOn(
    /* [in] */ IInterruptible* b)
{
    AutoLock lock(mBlockerLock);

    mBlocker = b;
    return NOERROR;
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

void Thread::Yield()
{
    sched_yield();
}

ECode Thread::Sleep(
    /* [in] */ Long millis)
{
    return Sleep(millis, 0);
}

ECode Thread::Sleep(
    /* [in] */ SyncObject* lock,
    /* [in] */ Long millis,
    /* [in] */ Integer nanos)
{
    return NativeMonitor::Wait(
            NativeThread::Current(),
            reinterpret_cast<NativeObject*>(lock->mNativeObject),
            millis, nanos, true, kSleeping);
}

ECode Thread::Sleep(
    /* [in] */ Long millis,
    /* [in] */ Integer nanos)
{
    if (millis < 0) {
        Logger::E("Thread", "millis < 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (nanos < 0 || nanos > 999999) {
        Logger::E("Thread", "nanosecond timeout value out of range");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (millis == 0 && nanos == 0) {
        if (Interrupted()) {
            return E_INTERRUPTED_EXCEPTION;
        }
        return NOERROR;
    }

    Long start = System::GetNanoTime();
    Long duration = (millis * NANOS_PER_MILLI) + nanos;

    AutoPtr<IThread> t;
    GetCurrentThread((IThread**)&t);
    SyncObject* lock = &From(t)->mLock;

    {
        AutoLock l(lock);

        while (true) {
            FAIL_RETURN(Sleep(lock, millis, nanos));

            Long now = System::GetNanoTime();
            Long elapsed = now - start;

            if (elapsed >= duration) {
                break;
            }

            duration -= elapsed;
            start = now;
            millis = duration / NANOS_PER_MILLI;
            nanos = (Integer)(duration % NANOS_PER_MILLI);
        }
    }

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

ECode Thread::constructor(
    /* [in] */ HANDLE peer)
{
    return NOERROR;
}

ECode Thread::Start()
{
    AutoLock lock(this);

    if (mThreadStatus != 0 || mStarted) {
        return E_ILLEGAL_THREAD_STATE_EXCEPTION;
    }

    mGroup->Add(this);

    mStarted = false;
    ECode ec = NativeCreate(this, mStackSize, mDaemon);
    if (SUCCEEDED(ec)) {
        mStarted = true;
    }

    if (!mStarted) {
        mGroup->ThreadStartFailed(this);
    }

    return NOERROR;
}

ECode Thread::NativeCreate(
    /* [in] */ Thread* t,
    /* [in] */ Long stackSize,
    /* [in] */ Boolean daemon)
{
    return NativeThread::CreateNativeThread(t, stackSize, daemon);
}

ECode Thread::Run()
{
    if (mTarget != nullptr) {
        return mTarget->Run();
    }
    return NOERROR;
}

void Thread::Exit()
{
    if (mGroup != nullptr) {
        mGroup->ThreadTerminated(this);
        mGroup = nullptr;
    }
    mTarget = nullptr;
    mBlocker = nullptr;
}

ECode Thread::Stop()
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Thread::Interrupt()
{
    AutoPtr<IThread> t;
    GetCurrentThread((IThread**)&t);
    if ((IThread*)this != t.Get()) {
        FAIL_RETURN(CheckAccess());
    }

    {
        AutoLock lock(mBlockerLock);

        AutoPtr<IInterruptible> b = mBlocker;
        if (b != nullptr) {
            FAIL_RETURN(NativeInterrupt());
            return b->Interrupt(this);
        }
    }
    return NativeInterrupt();
}

Boolean Thread::Interrupted()
{
    return NativeThread::Current()->Interrupted() ?
            true : false;
}

ECode Thread::IsInterrupted(
    /* [out] */ Boolean* interrupted)
{
    VALIDATE_NOT_NULL(interrupted);

    NativeThread* self = NativeThread::Current();
    NativeMutex::AutoLock lock(self, *Locks::sThreadListLock);
    NativeThread* thread =  NativeThread::FromManagedThread(this);
    return (thread != nullptr) ? thread->IsInterrupted() : false;
}

ECode Thread::Destroy()
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Thread::IsAlive(
    /* [out] */ Boolean* alive)
{
    VALIDATE_NOT_NULL(alive);

    *alive = mNative != 0;
    return NOERROR;
}

ECode Thread::Suspend()
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Thread::Resume()
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Thread::SetPriority(
    /* [in] */ Integer newPriority)
{
    FAIL_RETURN(CheckAccess());
    if (newPriority > MAX_PRIORITY || newPriority < MIN_PRIORITY) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IThreadGroup> g;
    GetThreadGroup((IThreadGroup**)&g);
    if (g != nullptr) {
        Integer maxPriority;
        g->GetMaxPriority(&maxPriority);
        if (newPriority > maxPriority) {
            newPriority = maxPriority;
        }

        {
            AutoLock lock(this);
            mPriority = newPriority;
            Boolean alive;
            if (IsAlive(&alive), alive) {
                NativeSetPriority(newPriority);
            }
        }
    }
    return NOERROR;
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
    if (name.IsNullOrEmpty()) {
        Logger::E("Thread", "name == null");
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

ECode Thread::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    *name = mName;
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

ECode Thread::ActiveCount(
    /* [out] */ Integer* count)
{
    VALIDATE_NOT_NULL(count);

    AutoPtr<IThread> t;
    GetCurrentThread((IThread**)&t);
    AutoPtr<IThreadGroup> g;
    t->GetThreadGroup((IThreadGroup**)&g);
    return g->ActiveCount(count);
}

ECode Thread::Enumerate(
    /* [out] */ Array<IThread*>& tarray,
    /* [out] */ Integer* count)
{
    VALIDATE_NOT_NULL(count);

    AutoPtr<IThread> t;
    GetCurrentThread((IThread**)&t);
    AutoPtr<IThreadGroup> g;
    t->GetThreadGroup((IThreadGroup**)&g);
    return g->Enumerate(tarray, count);
}

ECode Thread::CountStackFrames(
    /* [out] */ Integer* frameNum)
{
    VALIDATE_NOT_NULL(frameNum);

    Array<IStackTraceElement*> frames;
    GetStackTrace(&frames);
    *frameNum = frames.GetLength();
    return NOERROR;
}

ECode Thread::Join(
    /* [in] */ Long millis)
{
    if (millis < 0) {
        Logger::E("Thread", "timeout value is negative");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoLock lock(mLock);
    Long base = System::GetCurrentTimeMillis();
    Long now = 0;

    if (millis == 0) {
        Boolean alive;
        while (IsAlive(&alive), alive) {
            FAIL_RETURN(mLock.Wait(0));
        }
    }
    else {
        Boolean alive;
        while (IsAlive(&alive), alive) {
            Long delay = millis - now;
            if (delay <= 0) {
                break;
            }
            FAIL_RETURN(mLock.Wait(delay));
            now = System::GetCurrentTimeMillis() - base;
        }
    }
    return NOERROR;
}

ECode Thread::Join(
    /* [in] */ Long millis,
    /* [in] */ Integer nanos)
{
    if (millis < 0) {
        Logger::E("Thread", "timeout value is negative");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (nanos < 0 || nanos > 999999) {
        Logger::E("Thread", "nanosecond timeout value out of range");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (nanos >= 500000 || (nanos != 0 && millis == 0)) {
        millis++;
    }

    return Join(millis);
}

ECode Thread::Join()
{
    return Join(0);
}

void Thread::DumpStack()
{
    AutoPtr<IThread> t;
    GetCurrentThread((IThread**)&t);
    Array<IStackTraceElement*> frames;
    t->GetStackTrace(&frames);
    for (Integer i = 0; i < frames.GetLength(); i++) {
        Logger::D("Thread", "Frame[%d] %s", i,
                Object::ToString(frames[i]).string());
    }
}

ECode Thread::SetDaemon(
    /* [in] */ Boolean on)
{
    FAIL_RETURN(CheckAccess());
    Boolean alive;
    if (IsAlive(&alive), alive) {
        return E_ILLEGAL_THREAD_STATE_EXCEPTION;
    }
    mDaemon = on;
    return NOERROR;
}

ECode Thread::IsDaemon(
    /* [out] */ Boolean* daemon)
{
    VALIDATE_NOT_NULL(daemon);

    *daemon = mDaemon;
    return NOERROR;
}

ECode Thread::CheckAccess()
{
    return NOERROR;
}

ECode Thread::ToString(
    /* [out] */ String* desc)
{
    VALIDATE_NOT_NULL(desc);

    AutoPtr<IThreadGroup> g;
    GetThreadGroup((IThreadGroup**)&g);
    if (g != nullptr) {
        String name;
        GetName(&name);
        Integer prio;
        GetPriority(&prio);
        String gName;
        g->GetName(&gName);
        *desc = String::Format("Thread[%s,%d,%s]",
                name.string(), prio, gName.string());
        return NOERROR;
    }
    else {
        String name;
        GetName(&name);
        Integer prio;
        GetPriority(&prio);
        *desc = String::Format("Thread[%s,%d,]",
                name.string(), prio);
        return NOERROR;
    }
}

ECode Thread::GetContextClassLoader(
    /* [out] */ IClassLoader** loader)
{
    VALIDATE_NOT_NULL(loader);

    *loader = mContextClassLoader;
    REFCOUNT_ADD(*loader);
    return NOERROR;
}

ECode Thread::SetContextClassLoader(
    /* [in] */ IClassLoader* cl)
{
    mContextClassLoader = cl;
    return NOERROR;
}

Boolean Thread::HoldsLock(
    /* [in] */ IInterface* obj)
{
    AutoPtr<IThread> t;
    GetCurrentThread((IThread**)&t);
    return From(t)->NativeHoldsLock(obj);
}

Boolean Thread::NativeHoldsLock(
    /* [in] */ IInterface* obj)
{
    SyncObject* so = SyncObject::From(obj);
    if (so == nullptr) {
        Logger::E("Thread", "%p is not an object.");
        return false;
    }
    NativeThread* self = NativeThread::Current();
    NativeMutex::AutoLock lock(self, *Locks::sThreadListLock);
    NativeThread* thread = NativeThread::FromManagedThread(this);
    return thread->HoldsLock(reinterpret_cast<NativeObject*>(so->mNativeObject));
}

Array<IStackTraceElement*> Thread::Get_EMPTY_STACK_TRACE()
{
    static Array<IStackTraceElement*> EMPTY_STACK_TRACE(0);
    return EMPTY_STACK_TRACE;
}

static _Unwind_Reason_Code calculate_frames(_Unwind_Context* context, void* arg)
{
    Integer* frameNum = static_cast<Integer*>(arg);

    uintptr_t ip = _Unwind_GetIP(context);

    if (ip == 0) {
        return _URC_END_OF_STACK;
    }
    *frameNum++;
    return _URC_CONTINUE_UNWIND;
}

static _Unwind_Reason_Code trace_frames(_Unwind_Context* context, void* arg)
{
    Array<IStackTraceElement*>* frames = static_cast<Array<IStackTraceElement*>*>(arg);

    uintptr_t ip = _Unwind_GetIP(context);

    if (ip == 0) {
        return _URC_END_OF_STACK;
    }

    // TODO:
    return _URC_CONTINUE_UNWIND;
}

ECode Thread::GetStackTrace(
    /* [out, callee] */ Array<IStackTraceElement*>* trace)
{
    VALIDATE_NOT_NULL(trace);

    Integer frameNum = 0;
    _Unwind_Backtrace(calculate_frames, static_cast<void*>(&frameNum));
    Array<IStackTraceElement*> frames(frameNum);
    _Unwind_Backtrace(trace_frames, static_cast<void*>(&frames));
    *trace = frames;
    return NOERROR;
}

ECode Thread::GetId(
    /* [out] */ Long* id)
{
    VALIDATE_NOT_NULL(id);

    *id = mTid;
    return NOERROR;
}

ECode Thread::GetState(
    /* [out] */ ThreadState* state)
{
    VALIDATE_NOT_NULL(state);

    *state = NativeGetStatus(mStarted);
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

void Thread::NativeSetPriority(
    /* [in] */ Integer newPriority)
{
    NativeThread* self = NativeThread::Current();
    NativeMutex::AutoLock lock(self, *Locks::sThreadListLock);
    NativeThread* thread = NativeThread::FromManagedThread(this);
    if (thread != nullptr) {
        thread->SetNativePriority(newPriority);
    }
}

ThreadState Thread::NativeGetStatus(
    /* [in] */ Boolean hasBeenStarted)
{
    NativeThread* self = NativeThread::Current();
    ScopedObjectAccess soa(self);
    NativeThreadState threadState = hasBeenStarted ?
            kTerminated : kStarting;
    NativeMutex::AutoLock lock(soa.Self(), *Locks::sThreadListLock);
    NativeThread* thread = NativeThread::FromManagedThread(this);
    if (thread != nullptr) {
        threadState = thread->GetState();
    }
    switch (threadState) {
        case kTerminated:                       return ThreadState::TERMINATED;
        case kRunnable:                         return ThreadState::RUNNABLE;
        case kTimedWaiting:                     return ThreadState::TIMED_WAITING;
        case kSleeping:                         return ThreadState::TIMED_WAITING;
        case kBlocked:                          return ThreadState::BLOCKED;
        case kWaiting:                          return ThreadState::WAITING;
        case kStarting:                         return ThreadState::NEW;
        case kNative:                           return ThreadState::RUNNABLE;
        case kWaitingForSignalCatcherOutput:    return ThreadState::WAITING;
        case kWaitingInMainSignalCatcherLoop:   return ThreadState::WAITING;
        default:
            Logger::E("Thread", "Unexpected thread state: %d", threadState);
            return (ThreadState)-1;
    }
}

ECode Thread::NativeInterrupt()
{
    NativeThread* self = NativeThread::Current();
    NativeMutex::AutoLock lock(self, *Locks::sThreadListLock);
    NativeThread* thread = NativeThread::FromManagedThread(this);
    if (thread != nullptr) {
        thread->Interrupt(self);
    }
    return NOERROR;
}

ECode Thread::Unpark()
{
    AutoLock lock(mLock);
    switch (mParkState) {
        case ParkState::PREEMPTIVELY_UNPARKED: {
            break;
        }
        case ParkState::UNPARKED: {
            mParkState = ParkState::PREEMPTIVELY_UNPARKED;
            break;
        }
        default: {
            mParkState = ParkState::UNPARKED;
            mLock.NotifyAll();
            break;
        }
    }
    return NOERROR;
}

ECode Thread::ParkFor(
    /* [in] */ Long nanos)
{
    AutoLock lock(mLock);
    switch (mParkState) {
        case ParkState::PREEMPTIVELY_UNPARKED: {
            mParkState = ParkState::UNPARKED;
            break;
        }
        case ParkState::UNPARKED: {
            Long millis = nanos / NANOS_PER_MILLI;
            nanos %= NANOS_PER_MILLI;

            mParkState = ParkState::PARKED;
            ECode ec = mLock.Wait(millis, (Integer)nanos);
            if (ec == E_INTERRUPTED_EXCEPTION) {
                Interrupt();
            }
            /*
             * Note: If parkState manages to become
             * PREEMPTIVELY_UNPARKED before hitting this
             * code, it should left in that state.
             */
            if (mParkState == ParkState::PARKED) {
                mParkState = ParkState::UNPARKED;
            }
            break;
        }
        default: {
            Logger::E("Thread", "Attempt to repark.");
            return E_ASSERTION_ERROR;
        }
    }
    return NOERROR;
}

ECode Thread::ParkUntil(
    /* [in] */ Long time)
{
    AutoLock lock(mLock);
    Long currentTime = System::GetCurrentTimeMillis();
    if (time < currentTime) {
        mParkState = ParkState::UNPARKED;
        return NOERROR;
    }
    else {
        Long delayMillis = time - currentTime;
        // Long.MAX_VALUE / NANOS_PER_MILLI (0x8637BD05SF6) is the largest
        // long value that won't overflow to negative value when
        // multiplyed by NANOS_PER_MILLI (10^6).
        Long maxValue = (ILong::MAX_VALUE / NANOS_PER_MILLI);
        if (delayMillis > maxValue) {
            delayMillis = maxValue;
        }
        return ParkFor(delayMillis * NANOS_PER_MILLI);
    }
}

SyncObject* Thread::GetStaticLock()
{
    static SyncObject* sLock = new SyncObject();
    return sLock;
}

}
}
