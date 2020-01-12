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

#ifndef __COMO_CORE_STACKTRACK_H__
#define __COMO_CORE_STACKTRACK_H__

#include "como/core/SyncObject.h"
#include "como/core/volatile.h"
#include "como.core.IStackTrace.h"
#include "como.core.IStackTraceElement.h"
#include "como.io.IPrintStream.h"
#include "como.io.IPrintWriter.h"
#include "como.io.ISerializable.h"
#include <comosp.h>
#include <comoref.h>

using como::io::IPrintStream;
using como::io::IPrintWriter;
using como::io::ISerializable;

namespace como {
namespace core {

class StackTrace
    : public SyncObject
    , public IStackTrace
    , public ISerializable
{
private:
    class PrintStreamOrWriter
        : public LightRefBase
    {
    public:
        virtual AutoPtr<ISynchronize> Lock() = 0;

        virtual ECode Println(
            /* [in] */ IInterface* o) = 0;
    };

    class WrappedPrintStream
        : public PrintStreamOrWriter
    {
    public:
        WrappedPrintStream(
            /* [in] */ IPrintStream* printStream)
            : mPrintStream(printStream)
        {}

        AutoPtr<ISynchronize> Lock() override
        {
            return ISynchronize::Probe(mPrintStream);
        }

        ECode Println(
            /* [in] */ IInterface* o) override
        {
            return mPrintStream->Println(o);
        }

    private:
        AutoPtr<IPrintStream> mPrintStream;
    };

    class WrappedPrintWriter
        : public PrintStreamOrWriter
    {
    public:
        WrappedPrintWriter(
            /* [in] */ IPrintWriter* printWriter)
            : mPrintWriter(printWriter)
        {}

        AutoPtr<ISynchronize> Lock() override
        {
            return ISynchronize::Probe(mPrintWriter);
        }

        ECode Println(
            /* [in] */ IInterface* o) override
        {
            return mPrintWriter->Println(o);
        }

    private:
        AutoPtr<IPrintWriter> mPrintWriter;
    };

public:
    COMO_INTERFACE_DECL();

    ECode Constructor();

    ECode Constructor(
        /* [in] */ const String& message);

    ECode PrintStackTrace() override;

    ECode PrintStackTrace(
        /* [in] */ IPrintStream* s) override;

    ECode PrintStackTrace(
        /* [in] */ IPrintWriter* s) override;

    ECode FillInStackTrace() override;

    ECode GetStackTrace(
        /* [out, callee] */ Array<IStackTraceElement*>* stack) override;

private:
    ECode PrintStackTrace(
        /* [in] */ PrintStreamOrWriter* s);

    Array<IStackTraceElement*> GetOurStackTrace();

    AutoPtr<IStackTraceElement> ParseElement(
        /* [in] */ const String& info);

private:
    static constexpr Integer MAX_FRAME_DEPTH = 64;

    String mDetailMessage;
    VOLATILE Array<IStackTraceElement*> mStackTrace;
    Array<uintptr_t> mFrames = Array<uintptr_t>(MAX_FRAME_DEPTH);
    Integer mFrameCount = 0;
};

}
}

#endif // __COMO_CORE_STACKTRACK_H__
