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

#include "Namespace.h"

namespace ccdl {
namespace ast {

Namespace::Namespace(
    /* [in] */ const String& nsStr)
    : mName(nsStr)
    , mOuterNamespace(nullptr)
    , mNamespaces(5, false)
    , mCoclasses(20, false)
    , mConstants(10, false)
    , mEnumerations(10, false)
    , mInterfaces(20, false)
    , mResolved(true)
{}

Namespace::~Namespace()
{
    mOuterNamespace = nullptr;
}

bool Namespace::AddNamespace(
    /* [in] */ Namespace* innerNS)
{
    if (innerNS == nullptr) return true;

    bool ret = mNamespaces.Add(innerNS);
    if (ret) innerNS->mOuterNamespace = this;
    return ret;
}

Namespace* Namespace::FindNamespace(
    /* [in] */ const String& nsString)
{
    for (int i = 0; i < mNamespaces.GetSize(); i++) {
        Namespace* ns = mNamespaces.Get(i);
        if (ns->mName.Equals(nsString)) {
            return ns;
        }
    }
    return nullptr;
}

bool Namespace::AddCoclass(
    /* [in] */ Coclass* klass)
{
    if (klass == nullptr) return true;

    return mCoclasses.Add(klass);
}

bool Namespace::AddConstant(
    /* [in] */ Constant* constant)
{
    if (constant == nullptr) return true;

    return mConstants.Add(constant);
}

bool Namespace::AddEnumeration(
    /* [in] */ Enumeration* enumn)
{
    if (enumn == nullptr) return true;

    return mEnumerations.Add(enumn);
}

int Namespace::GetExternalEnumerationNumber()
{
    int number = 0;
    for (int i = 0; i < mEnumerations.GetSize(); i++) {
        Enumeration* enumn = mEnumerations.Get(i);
        if (enumn->IsExternal()) number++;
    }
    return number;
}

bool Namespace::UpdateEnumeration(
    /* [in] */ Enumeration* enumn)
{
    if (enumn == nullptr) return true;

    mEnumerations.Remove(enumn);
    return mEnumerations.Add(enumn);
}

bool Namespace::AddInterface(
    /* [in] */ Interface* itf)
{
    if (itf == nullptr) return true;

    return mInterfaces.Add(itf);
}

int Namespace::GetExternalInterfaceNumber()
{
    int number = 0;
    for (int i = 0; i < mInterfaces.GetSize(); i++) {
        Interface* itf = mInterfaces.Get(i);
        if (itf->IsExternal()) number++;
    }
    return number;
}

bool Namespace::UpdateInterface(
    /* [in] */ Interface* itf)
{
    if (itf == nullptr) return true;

    mInterfaces.Remove(itf);
    return mInterfaces.Add(itf);
}

String Namespace::ToString()
{
    String nsString("");
    Namespace* ns = this;
    while (ns != nullptr) {
        if (ns->mName.Equals("__global__")) return nsString;
        nsString = ns->mName + "::" + nsString;
        ns = ns->mOuterNamespace;
    }
    return nsString;
}

}
}