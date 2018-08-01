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

#ifndef __CCM_CORE_STACKTRACK_H__
#define __CCM_CORE_STACKTRACK_H__

#include "ccm/core/SyncObject.h"
#include "ccm.core.IStackTrace.h"
#include "ccm.core.IStackTraceElement.h"
#include "ccm.io.IPrintStream.h"
#include "ccm.io.IPrintWriter.h"
#include "ccm.io.ISerializable.h"
#include <ccmautoptr.h>

using ccm::io::IPrintStream;
using ccm::io::IPrintWriter;
using ccm::io::ISerializable;

namespace ccm {
namespace core {

class StackTrace
    : public SyncObject
    , public IStackTrace
    , public ISerializable
{
public:
    CCM_INTERFACE_DECL();

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
    Array<IStackTraceElement*> GetOurStackTrace();

    AutoPtr<IStackTraceElement> ParseElement(
        /* [in] */ const String& info);

private:
    static constexpr Integer MAX_FRAME_DEPTH = 64;

    String mDetailMessage;
    Array<IStackTraceElement*> mStackTrace;
    Array<uintptr_t> mFrames = Array<uintptr_t>(MAX_FRAME_DEPTH);
    Integer mFrameCount = 0;
};

}
}

#endif // __CCM_CORE_STACKTRACK_H__
