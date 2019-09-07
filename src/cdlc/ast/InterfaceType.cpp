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
#include "util/Properties.h"
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

String InterfaceType::ToString()
{
    return mNamespace->ToString() + "::" + mName;
}

String InterfaceType::Dump(
    /* [in] */ const String& prefix)
{
    StringBuilder builder;

    builder.Append(prefix).Append("Interface[");
    builder.AppendFormat("name:%s, ", mName.string());
    builder.AppendFormat("namespace:%s, ", mNamespace->ToString().string());
    builder.AppendFormat("base:%s, ", mBaseInterface->ToString().string());
    if (mOuterInterface != nullptr) {
        builder.AppendFormat("outer:%s, ", mOuterInterface->ToString().string());
    }
    builder.AppendFormat("uuid:%s, ", mUuid->ToString().string());
    builder.AppendFormat("version:%s", mVersion.string());
    if (!mDescription.IsEmpty()) {
        builder.AppendFormat(", description:%s", mDescription.string());
    }
    builder.Append("]\n");

    for (AutoPtr<InterfaceType> interface : mNestedInterfaces) {
        String interfaceInfo = String::Format("Interface[name:%s]\n", interface->GetName().string());
        builder.Append(prefix + Properties::INDENT).Append(interfaceInfo);
    }
    for (AutoPtr<Constant> constant : mConstants) {
        String constantInfo = constant->Dump(prefix + Properties::INDENT);
        builder.AppendFormat("%s\n", constantInfo.string());
    }
    for (AutoPtr<Method> method : mMethods) {
        String methodInfo = method->Dump(prefix + Properties::INDENT);
        builder.AppendFormat("%s\n", methodInfo.string());
    }

    return builder.ToString();
}

}
