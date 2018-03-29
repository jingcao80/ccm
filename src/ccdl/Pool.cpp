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

#include "Pool.h"
#include "../util/StringBuilder.h"

namespace ccm {
namespace ccdl {

Pool::Pool()
    : mCoclasses(20)
    , mEnumerations(10)
    , mInterfaces(20)
    , mNamespaces(5)
    , mTempTypes(20)
    , mTypes(6000)
{}

bool Pool::AddEnumeration(
    /* [in] */ Enumeration* enumeration)
{
    if (enumeration == nullptr) return true;

    if (!mEnumerations.Add(enumeration)) return false;
    mTypes.Put(enumeration->ToString(), enumeration);
    return true;
}

Enumeration* Pool::FindEnumeration(
    /* [in] */ const String& enumFullName)
{
    Type* type = FindType(enumFullName);
    if (type == nullptr || !type->IsEnumeration()) {
        return nullptr;
    }
    return (Enumeration*)type;
}

bool Pool::AddInterface(
    /* [in] */ Interface* interface)
{
    if (interface == nullptr) return true;

    if (!mInterfaces.Add(interface)) return false;
    mTypes.Put(interface->ToString(), interface);
    return true;
}

Interface* Pool::FindInterface(
    /* [in] */ const String& itfFullName)
{
    Type* type = FindType(itfFullName);
    if (type == nullptr || !type->IsInterface()) {
        return nullptr;
    }
    return (Interface*)type;
}

bool Pool::AddCoclass(
    /* [in] */ Coclass* klass)
{
    if (klass == nullptr) return true;

    if (!mCoclasses.Add(klass)) return false;
    mTypes.Put(klass->ToString(), klass);
    return true;
}

Coclass* Pool::FindClass(
    /* [in] */ const String& klassName)
{
    Type* type = FindType(klassName);
    if (type == nullptr || !type->IsCoclass()) {
        return nullptr;
    }
    return (Coclass*)type;
}

bool Pool::AddNamespace(
    /* [in] */ Namespace* ns)
{
    if (ns == nullptr) return true;

    return mNamespaces.Add(ns);
}

Namespace* Pool::FindNamespace(
    /* [in] */ const String& nsString)
{
    for (int i = 0; i < mNamespaces.GetSize(); i++) {
        Namespace* ns = mNamespaces.Get(i);
        if (ns->ToShortString().Equals(nsString)) {
            return ns;
        }
    }
    return nullptr;
}

Namespace* Pool::ParseNamespace(
    /* [in] */ const String& nsString)
{
    String nss = nsString;
    Namespace* nsp = nullptr, *headNsp = nullptr, *currNsp = nullptr;
    int cIndex;
    while ((cIndex = nss.IndexOf("::")) != -1) {
        String ns = nss.Substring(0, cIndex - 1);
        if (currNsp == nullptr) {
            currNsp = FindNamespace(String("__global__"));
            nsp = currNsp->FindNamespace(nss);
            if (nsp == nullptr) {
                nsp = new Namespace(ns);
                AddNamespace(nsp);
                currNsp->AddNamespace(nsp);
            }
            currNsp = headNsp = nsp;
        }
        else {
            nsp = currNsp->FindNamespace(ns);
            if (nsp == nullptr) {
                nsp = new Namespace(ns);
                AddNamespace(nsp);
                currNsp->AddNamespace(nsp);
            }
            currNsp = nsp;
        }
        nss = nss.Substring(cIndex + 2);
    }
    if (currNsp == nullptr) {
        currNsp = FindNamespace(String("__global__"));
        nsp = currNsp->FindNamespace(nss);
        if (nsp == nullptr) {
            nsp = new Namespace(nss);
            AddNamespace(nsp);
            currNsp->AddNamespace(nsp);
        }
        headNsp = nsp;
    }
    else {
        nsp = currNsp->FindNamespace(nss);
        if (nsp == nullptr) {
            nsp = new Namespace(nss);
            AddNamespace(nsp);
            currNsp->AddNamespace(nsp);
        }
    }
    return headNsp;
}

bool Pool::AddTemporaryType(
    /* [in] */ Type* type)
{
    if (type == nullptr) return true;

    if (!mTempTypes.Add(type)) return false;
    mTypes.Put(type->ToString(), type);
    return true;
}

Type* Pool::GetType(
    /* [in] */ int index)
{
    std::shared_ptr< ArrayList<HashMap<Type*>::Pair*> > types =
            GetTypes();
    return types->Get(index)->mValue;
}

Type* Pool::FindType(
    /* [in] */ const String& typeName)
{
    if (typeName.IsNullOrEmpty()) return nullptr;
    return mTypes.Get(typeName);
}

int Pool::IndexOf(
    /* [in] */ Type* type)
{
    std::shared_ptr< ArrayList<HashMap<Type*>::Pair*> > types =
            GetTypes();
    for (int i = 0; i < types->GetSize(); i++) {
        HashMap<Type*>::Pair* p = types->Get(i);
        if (p->mValue == type) return i;
    }
    return -1;
}

String Pool::Dump(
    /* [in] */ const String& prefix)
{
    StringBuilder builder;

    for (int i = 0; i < mEnumerations.GetSize(); i++) {
        String enumStr = mEnumerations.Get(i)->Dump(String("  "));
        builder.Append(enumStr).Append("\n");
    }
    for (int i = 0; i < mInterfaces.GetSize(); i++) {
        if (!mInterfaces.Get(i)->IsDeclared()) continue;
        String itfStr = mInterfaces.Get(i)->Dump(String("  "));
        builder.Append(itfStr).Append("\n");
    }
    for (int i = 0; i < mCoclasses.GetSize(); i++) {
        String coclassStr = mCoclasses.Get(i)->Dump(String("  "));
        builder.Append(coclassStr).Append("\n");
    }

    return builder.ToString();
}

}
}