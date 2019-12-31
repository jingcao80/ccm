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

#include "ccm/core/AutoLock.h"
#include "ccm/core/CStackTraceElement.h"
#include "ccm/core/NativeBacktrace.h"
#include "ccm/core/StackTrace.h"
#include "ccm.core.IStackTraceElement.h"

using ccm::io::IID_ISerializable;

namespace ccm {
namespace core {

CCM_INTERFACE_IMPL_2(StackTrace, SyncObject, IStackTrace, ISerializable);

ECode StackTrace::Constructor()
{
    FillInStackTrace();
    return NOERROR;
}

ECode StackTrace::Constructor(
    /* [in] */ const String& message)
{
    FillInStackTrace();
    mDetailMessage = message;
    return NOERROR;
}

ECode StackTrace::PrintStackTrace()
{
    return NOERROR;
}

ECode StackTrace::PrintStackTrace(
    /* [in] */ IPrintStream* s)
{
    AutoPtr<PrintStreamOrWriter> ps = new WrappedPrintStream(s);
    return PrintStackTrace(ps);
}

ECode StackTrace::PrintStackTrace(
    /* [in] */ PrintStreamOrWriter* s)
{
    AutoLock lock(s->Lock());

    Array<IStackTraceElement*> trace = GetOurStackTrace();
    for (Integer i = 0; i < trace.GetLength(); i++) {
        IStackTraceElement* traceElement = trace[i];
        s->Println(traceElement);
    }
    return NOERROR;
}

ECode StackTrace::PrintStackTrace(
    /* [in] */ IPrintWriter* s)
{
    AutoPtr<PrintStreamOrWriter> ps = new WrappedPrintWriter(s);
    return PrintStackTrace(ps);
}

ECode StackTrace::FillInStackTrace()
{
    mFrameCount = GetBacktrace(mFrames.GetPayload(), mFrames.GetLength());
    return NOERROR;
}

ECode StackTrace::GetStackTrace(
    /* [out, callee] */ Array<IStackTraceElement*>* stack)
{
    VALIDATE_NOT_NULL(stack);

    *stack = GetOurStackTrace().Clone();
    return NOERROR;
}

Array<IStackTraceElement*> StackTrace::GetOurStackTrace()
{
    if (mFrameCount > 0) {
        String backtrace = DumpBacktrace(mFrames.GetPayload(), mFrameCount);
        VOLATILE_SET(mStackTrace, Array<IStackTraceElement*>(mFrameCount));
        Integer fromIdx = 0;
        Integer lrIdx = backtrace.IndexOf(U'\n', fromIdx);
        Integer count = 0;
        while (lrIdx != -1 && count < mFrameCount) {
            String line = backtrace.Substring(fromIdx, lrIdx).Trim();
            mStackTrace.Set(count++, ParseElement(line));
            fromIdx = lrIdx + 1;
            lrIdx = backtrace.IndexOf(U'\n', fromIdx);
        }
    }
    return mStackTrace;
}

AutoPtr<IStackTraceElement> StackTrace::ParseElement(
    /* [in] */ const String& info)
{
    if (info.IsNullOrEmpty()) return nullptr;

    Integer fromIdx = 0;
    Integer idx = info.IndexOf("  ", fromIdx);
    if (idx == -1) return nullptr;
    String no = info.Substring(0, idx);
    fromIdx = idx + 5;
    idx = info.IndexOf(" ", fromIdx);
    if (idx == -1) return nullptr;
    String pc = info.Substring(fromIdx, idx);
    fromIdx = idx + 2;
    idx = info.IndexOf(" ", fromIdx);
    String soname = idx != -1 ? info.Substring(fromIdx, idx) : info.Substring(fromIdx);
    if (soname.IsNullOrEmpty()) return nullptr;
    String symbol;
    if (idx != -1) {
        symbol = info.Substring(idx + 1);
    }
    AutoPtr<IStackTraceElement> element;
    CStackTraceElement::New(no, pc, soname, symbol, IID_IStackTraceElement, (IInterface**)&element);
    return element;
}

}
}
