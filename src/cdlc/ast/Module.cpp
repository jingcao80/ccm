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

#include "ast/Module.h"
#include "util/Properties.h"
#include "util/StringBuilder.h"
#include <algorithm>

namespace cdlc {

void Module::SetAttributes(
    /* [in] */ const Attributes& attrs)
{
    mUuid = UUID::Parse(attrs.mUuid);
    mVersion = attrs.mVersion;
    mDescription = attrs.mDescription;
    mUri = attrs.mUri;
}

AutoPtr<Namespace> Module::ParseNamespace(
    /* [in] */ const String& nsString)
{
    if (nsString.IsEmpty()) {
        return nullptr;
    }

    String nss = nsString + "::";
    AutoPtr<Namespace> parentNs, targetNs;
    int index;
    while (!nss.IsEmpty() && ((index = nss.IndexOf("::")) != -1)) {
        String targetNss = nss.Substring(0, index);
        if (parentNs == nullptr) {
            targetNs = FindNamespace(targetNss);
            if (targetNs == nullptr) {
                targetNs = new Namespace(targetNss, this);
                AddNamespace(targetNs);
            }
            parentNs = targetNs;
        }
        else {
            targetNs = parentNs->FindNamespace(targetNss);
            if (targetNs == nullptr) {
                targetNs = new Namespace(targetNss, this);
                parentNs->AddNamespace(targetNs);
            }
            parentNs = targetNs;
        }
        nss = nss.Substring(index + 2);
    }
    return targetNs;
}

AutoPtr<Constant> Module::GetConstant(
    /* [in] */ int i)
{
    if (i >= 0 && i < mConstants.size()) {
        return mConstants[i];
    }
    return nullptr;
}

int Module::IndexOf(
    /* [in] */ Constant* constant)
{
    auto it = std::find(mConstants.begin(), mConstants.end(), constant);
    if (it != mConstants.end()) {
        return std::distance(mConstants.begin(), it);
    }
    return -1;
}

AutoPtr<EnumerationType> Module::FindEnumeration(
    /* [in] */ const String& fullName)
{
    AutoPtr<Type> type = FindType(fullName);
    if (type != nullptr && type->IsEnumerationType()) {
        return (EnumerationType*)type.Get();
    }
    return nullptr;
}

AutoPtr<EnumerationType> Module::GetEnumeration(
        /* [in] */ int i)
{
    if (i >= 0 && i < mEnumerations.size()) {
        return mEnumerations[i];
    }
    return nullptr;
}

int Module::IndexOf(
    /* [in] */ EnumerationType* enumeration)
{
    auto it = std::find(mEnumerations.begin(), mEnumerations.end(), enumeration);
    if (it != mEnumerations.end()) {
        return std::distance(mEnumerations.begin(), it);
    }
    return -1;
}

AutoPtr<InterfaceType> Module::GetInterface(
    /* [in] */ int i)
{
    if (i >= 0 && i < mInterfaces.size()) {
        return mInterfaces[i];
    }
    return nullptr;
}

int Module::IndexOf(
    /* [in] */ InterfaceType* interface)
{
    auto it = std::find(mInterfaces.begin(), mInterfaces.end(), interface);
    if (it != mInterfaces.end()) {
        return std::distance(mInterfaces.begin(), it);
    }
    return -1;
}

AutoPtr<CoclassType> Module::GetCoclass(
    /* [in] */ int i)
{
    if (i >= 0 && i < mKlasses.size()) {
        return mKlasses[i];
    }
    return nullptr;
}

int Module::IndexOf(
    /* [in] */ CoclassType* klass)
{
    auto it = std::find(mKlasses.begin(), mKlasses.end(), klass);
    if (it != mKlasses.end()) {
        return std::distance(mKlasses.begin(), it);
    }
    return -1;
}

void Module::AddType(
    /* [in] */ Type* type)
{
    mAllTypeMap[type->ToString()] = type;
    type->SetModule(this);
}

void Module::AddTemporaryType(
    /* [in] */ Type* type)
{
    if (type != nullptr) {
        mAllTypeMap[type->ToString()] = type;
        type->SetModule(this);
    }
}

AutoPtr<Type> Module::FindType(
    /* [in] */ const String& fullName)
{
    if (fullName.IsEmpty()) {
        return nullptr;
    }

    auto it = mAllTypeMap.find(fullName);
    if (it != mAllTypeMap.end()) {
        return it->second;
    }

    return nullptr;
}

int Module::IndexOf(
    /* [in] */ Type* type)
{
    auto it = mAllTypeMap.begin();
    for (int i = 0; it != mAllTypeMap.end(); i++, it++) {
        if (it->second == type) {
            return i;
        }
    }
    return -1;
}

String Module::ToString()
{
    StringBuilder builder;

    builder.Append("Module[");
    builder.AppendFormat("name:%s, ", mName.string());
    builder.AppendFormat("uuid:%s, ", mUuid->ToString().string());
    builder.AppendFormat("uri:%s", mUri.string());
    builder.Append("]\n");
    return builder.ToString();
}

String Module::Dump(
    /* [in] */ const String& prefix)
{
    StringBuilder builder;

    builder.Append(prefix).Append("Module[");
    builder.AppendFormat("name:%s, ", mName.string());
    builder.AppendFormat("uuid:%s, ", mUuid->ToString().string());
    builder.AppendFormat("uri:%s", mUri.string());
    if (!mDescription.IsEmpty()) {
        builder.AppendFormat(", description:%s", mDescription.string());
    }
    builder.Append("]\n");

    for (AutoPtr<Constant> constant : mConstants) {
        String constantInfo = constant->Dump(prefix + Properties::INDENT);
        builder.AppendFormat("%s\n", constantInfo.string());
    }
    for (AutoPtr<EnumerationType> enumeration : mEnumerations) {
        if (enumeration->IsForwardDeclared()) {
            continue;
        }
        String enumerationInfo = enumeration->Dump(prefix + Properties::INDENT);
        builder.AppendFormat("%s\n", enumerationInfo.string());
    }
    for (AutoPtr<InterfaceType> interface : mInterfaces) {
        if (interface->IsForwardDeclared()) {
            continue;
        }
        String interfaceInfo = interface->Dump(prefix + Properties::INDENT);
        builder.AppendFormat("%s\n", interfaceInfo.string());
    }
    for (AutoPtr<CoclassType> klass : mKlasses) {
        String klassInfo = klass->Dump(prefix + Properties::INDENT);
        builder.AppendFormat("%s\n", klassInfo.string());
    }

    return builder.ToString();
}

}
