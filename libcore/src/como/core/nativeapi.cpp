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

#include "como/core/nativeapi.h"
#include "como/core/NativeObject.h"
#include "como/core/NativeThread.h"
#include <ccmlogger.h>

namespace como {
namespace core {

size_t GetStackOverflowReservedBytes(
    /* [in] */ InstructionSet isa)
{
    switch (isa) {
        case kArm64:
            return kArm64StackOverflowReservedBytes;

        case kX86_64:
            return kX86_64StackOverflowReservedBytes;

        case kNone:
            Logger::E("nativeapi", "kNone has no stack overflow size");
            return 0;

        default:
            Logger::E("nativeapi", "Unknown instruction set%d", isa);
            return 0;
    }
}

void SetThreadName(
    /* [in] */ const String& name)
{
    const char* threadName = name.string();
    int hasAt = 0;
    int hasDot = 0;
    const char* s = threadName;
    while (*s) {
        if (*s == '.') {
            hasDot = 1;
        }
        else if (*s == '@') {
            hasAt = 1;
        }
        s++;
    }
    int len = s - threadName;
    if (len < 15 || hasAt || !hasDot) {
        s = threadName;
    }
    else {
        s = threadName + len - 15;
    }
    // pthread_setname_np fails rather than truncating long strings.
    char buf[16];       // MAX_TASK_COMM_LEN=16 is hard-coded in the kernel.
    strncpy(buf, s, sizeof(buf)-1);
    buf[sizeof(buf)-1] = '\0';
    errno = pthread_setname_np(pthread_self(), buf);
    if (errno != 0) {
        Logger::W("nativeapi", "Unable to set the name of current thread to '%s'",
                buf);
    }
}

void DumpNativeStack(
    /* [in] */ String* os,
    /* [in] */ pid_t tid,
    /* [in] */ BacktraceMap* map,
    /* [in] */ const char* prefix,
    /* [in] */ void* ucontext)
{}

HANDLE CreateNativeObject(
    /* [in] */ HANDLE comoObject)
{
    NativeObject* obj = new NativeObject(comoObject);
    if (obj == nullptr) {
        Logger::E("nativeapi", "Out of memory.");
    }
    return reinterpret_cast<HANDLE>(obj);
}

void DestroyNativeObject(
    /* [in] */ HANDLE handle)
{
    NativeObject* obj = reinterpret_cast<NativeObject*>(handle);
    if (obj != nullptr) {
        delete obj;
    }
}

ECode NativeObjectLock(
    /* [in] */ HANDLE handle)
{
    NativeThread* self = NativeThread::Current();
    return reinterpret_cast<NativeObject*>(handle)->MonitorEnter(self);
}

ECode NativeObjectUnlock(
    /* [in] */ HANDLE handle)
{
    NativeThread* self = NativeThread::Current();
    return reinterpret_cast<NativeObject*>(handle)->MonitorExit(self);
}

ECode NativeObjectNotify(
    /* [in] */ HANDLE handle)
{
    NativeThread* self = NativeThread::Current();
    return reinterpret_cast<NativeObject*>(handle)->Notify(self);
}

ECode NativeObjectNotifyAll(
    /* [in] */ HANDLE handle)
{
    NativeThread* self = NativeThread::Current();
    return reinterpret_cast<NativeObject*>(handle)->NotifyAll(self);
}

ECode NativeObjectWait(
    /* [in] */ HANDLE handle)
{
    NativeThread* self = NativeThread::Current();
    return reinterpret_cast<NativeObject*>(handle)->Wait(self);
}

ECode NativeObjectWait(
    /* [in] */ HANDLE handle,
    /* [in] */ Long ms,
    /* [in] */ Integer ns)
{
    NativeThread* self = NativeThread::Current();
    return reinterpret_cast<NativeObject*>(handle)->Wait(self, ms, ns);
}

Boolean MaybeCauseIcuException(
    /* [in] */ const char* function,
    /* [in] */ UErrorCode error)
{
    if (U_SUCCESS(error)) {
        return false;
    }
    const char* exceptionClass = "java/lang/RuntimeException";
    if (error == U_ILLEGAL_ARGUMENT_ERROR) {
        Logger::E("IcuException", "E_ILLEGAL_ARGUMENT_EXCEPTION");
    }
    else if (error == U_INDEX_OUTOFBOUNDS_ERROR || error == U_BUFFER_OVERFLOW_ERROR) {
        Logger::E("IcuException", "E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION");
    }
    else if (error == U_UNSUPPORTED_ERROR) {
        Logger::E("IcuException", "E_UNSUPPORTED_OPERATION_EXCEPTION");
    }
    else if (error == U_FORMAT_INEXACT_ERROR) {
        Logger::E("IcuException", "E_ARITHMETIC_EXCEPTION");
    }
    else {
        Logger::E("IcuException", "E_RUNTIME_EXCEPTION");
    }
    return true;
}

}
}