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

#include "ast/Namespace.h"
#include "ast/CoclassType.h"
#include "ast/Constant.h"
#include "ast/EnumerationType.h"
#include "ast/InterfaceType.h"
#include "ast/Module.h"

namespace cdlc {

const String Namespace::GLOBAL_NAME("__global__");

Namespace::Namespace(
    /* [in] */ const String& name,
    /* [in] */ Module* module)
    : mName(name)
    , mModule(module)
{}

Namespace::Namespace(
    /* [in] */ InterfaceType* interfaceWrapped,
    /* [in] */ Module* module)
    : mName(interfaceWrapped->GetName())
    , mIsWrapper(true)
    , mInterfaceWrapped(interfaceWrapped)
    , mModule(module)
{}

void Namespace::AddNamespace(
    /* [in] */ Namespace* ns)
{
    if (ns != nullptr) {
        mChildren.push_back(ns);
        ns->SetParent(this);
    }
}

AutoPtr<Namespace> Namespace::FindNamespace(
    /* [in] */ const String& nsString)
{
    int index = nsString.IndexOf("::");
    String childNsString = index != -1
            ? nsString.Substring(0, index)
            : nsString;
    for (AutoPtr<Namespace> child : mChildren) {
        if (child->ToShortString().Equals(childNsString)) {
            if (index != -1) {
                return child->FindNamespace(nsString.Substring(index + 2));
            }
            else {
                return child;
            }
        }
    }
    return nullptr;
}

void Namespace::AddConstant(
    /* [in] */ Constant* constant)
{
    if (constant != nullptr) {
        mConstants.push_back(constant);
        constant->SetNamespace(this);
        mModule->AddConstant(constant);
    }
}

void Namespace::AddEnumerationType(
    /* [in] */ EnumerationType* enumeration)
{
    if (enumeration != nullptr) {
        mTypes.push_back(enumeration);
        enumeration->SetNamespace(this);
        mModule->AddEnumerationType(enumeration);
    }
}

void Namespace::AddInterfaceType(
    /* [in] */ InterfaceType* interface)
{
    if (interface != nullptr) {
        mTypes.push_back(interface);
        interface->SetNamespace(this);
        mModule->AddInterfaceType(interface);
    }
}

void Namespace::AddCoclassType(
    /* [in] */ CoclassType* klass)
{
    if (klass != nullptr) {
        mTypes.push_back(klass);
        klass->SetNamespace(this);
        mModule->AddCoclassType(klass);
    }
}

void Namespace::AddType(
    /* [in] */ Type* type)
{
    if (type != nullptr) {
        mTypes.push_back(type);
        type->SetNamespace(this);
        mModule->AddType(type);
    }
}

String Namespace::ToString()
{
    String nsString = mName;
    Namespace* parent = mParent;
    while (parent != nullptr && !parent->IsGlobal()) {
        nsString = parent->mName + "::" + nsString;
        parent = parent->mParent;
    }
    return nsString;
}

String Namespace::Dump(
    /* [in] */ const String& prefix)
{
    return prefix + ToString();
}

}
