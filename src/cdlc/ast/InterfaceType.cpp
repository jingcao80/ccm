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

#include "ast/InterfaceType.h"
#include "ast/Namespace.h"
#include "util/StringBuilder.h"

namespace cdlc {

void InterfaceType::SetAttributes(
    /* [in] */ const Attributes& attrs)
{
    mUuid = UUID::Parse(attrs.mUuid);
    mVersion = attrs.mVersion;
    mDescription = attrs.mDescription;
}

AutoPtr<Constant> InterfaceType::FindConstant(
    /* [in] */ const String& name)
{
    for (AutoPtr<Constant> constant : mConstants) {
        if (constant->GetName().Equals(name)) {
            return constant;
        }
    }
    return nullptr;
}

AutoPtr<Method> InterfaceType::FindMethod(
    /* [in] */ const String& name,
    /* [in] */ const String& signature)
{
    for (AutoPtr<Method> method : mMethods) {
        if (method->GetName().Equals(name) &&
                method->GetSignature().Equals(signature)) {
            return method;
        }
    }
    return nullptr;
}

bool InterfaceType::IsInterfaceType()
{
    return true;
}

String InterfaceType::GetSignature()
{
    StringBuilder builder;

    builder.Append("L");
    if (!mNamespace->IsGlobal()) {
        builder.Append(mNamespace->ToString().Replace("::", "/"));
    }
    builder.Append("/");
    builder.Append(mName);
    builder.Append(";");
    return builder.ToString();
}

String InterfaceType::Dump(
    /* [in] */ const String& prefix)
{
    return prefix + ToString();
}

}
