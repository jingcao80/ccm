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

#include "core/NativeThread.h"
#include "core/Thread.h"

namespace ccm {
namespace core {

CCM_INTERFACE_IMPL_1(Thread, Runnable, IThread);

ECode Thread::constructor()
{
    return NOERROR;
}

ECode Thread::Run()
{
    return NOERROR;
}

ECode Thread::CheckAccess()
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

ECode Thread::GetPriority(
    /* [out] */ Integer* priority)
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

ECode Thread::GetThreadGroup(
    /* [out] */ IThreadGroup** tg)
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

ECode Thread::SetName(
    /* [in] */ const String& name)
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

}
}
