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

#include "ccm/core/CStringBuilder.h"
#include "ccm/core/StackTraceElement.h"
#include <ccmautoptr.h>

using ccm::io::IID_ISerializable;

namespace ccm {
namespace core {

CCM_INTERFACE_IMPL_2(StackTraceElement, SyncObject, IStackTraceElement, ISerializable);

ECode StackTraceElement::Constructor(
    /* [in] */ const String& no,
    /* [in] */ const String& pc,
    /* [in] */ const String& soname,
    /* [in] */ const String& symbol)
{
    mNo = no;
    mPC = pc;
    mSoname = soname;
    mSymbol = symbol;
    return NOERROR;
}

ECode StackTraceElement::ToString(
    /* [out] */ String* desc)
{
    VALIDATE_NOT_NULL(desc);

    AutoPtr<IStringBuilder> sb;
    CStringBuilder::New(IID_IStringBuilder, (IInterface**)&sb);
    sb->Append(mNo);
    sb->Append(String("  "));
    sb->Append(mPC);
    sb->Append(U' ');
    sb->Append(mSoname);
    if (!mSymbol.IsNullOrEmpty()) {
        sb->Append(U' ');
        sb->Append(mSymbol);
    }
    return sb->ToString(desc);
}

}
}
