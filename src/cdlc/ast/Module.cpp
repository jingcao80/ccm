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

namespace cdlc {

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
    }
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

String Module::ToString()
{
    return "Module";
}

String Module::Dump(
    /* [in] */ const String& prefix)
{
    return prefix + ToString();
}

}
