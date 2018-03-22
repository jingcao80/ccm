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
    : mEnumerations(10)
    , mInterfaces(20)
    , mNamespaces(5)
    , mTempTypes(20)
    , mTypes(6000)
{
    mByteType = new ByteType();
    mShortType = new ShortType();
    mIntegerType = new IntegerType();
    mLongType = new LongType();
    mCharType = new CharType();
    mFloatType = new FloatType();
    mDoubleType = new DoubleType();
    mBooleanType = new BooleanType();
    mStringType = new StringType();
    mHANDLEType = new HANDLEType();

    mTypes.Put(String("Byte"), mByteType);
    mTypes.Put(String("Short"), mShortType);
    mTypes.Put(String("Integer"), mIntegerType);
    mTypes.Put(String("Long"), mLongType);
    mTypes.Put(String("Char"), mCharType);
    mTypes.Put(String("Float"), mFloatType);
    mTypes.Put(String("Double"), mDoubleType);
    mTypes.Put(String("Boolean"), mBooleanType);
    mTypes.Put(String("String"), mStringType);
    mTypes.Put(String("HANDLE"), mHANDLEType);
}

Pool::~Pool()
{
    delete mByteType;
    delete mShortType;
    delete mIntegerType;
    delete mLongType;
    delete mCharType;
    delete mFloatType;
    delete mDoubleType;
    delete mBooleanType;
    delete mStringType;
    delete mHANDLEType;
}

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
            nsp = FindNamespace(ns);
            if (nsp == nullptr) {
                nsp = new Namespace(ns);
                AddNamespace(nsp);
            }
            currNsp = headNsp = nsp;
        }
        else {
            nsp = currNsp->FindInnerNamespace(ns);
            if (nsp == nullptr) {
                nsp = new Namespace(ns);
                AddNamespace(nsp);
                currNsp->AddInnerNamespace(nsp);
            }
            currNsp = nsp;
        }
        nss = nss.Substring(cIndex + 2);
    }
    if (currNsp == nullptr) {
        nsp = FindNamespace(nss);
        if (nsp == nullptr) {
            nsp = new Namespace(nss);
            AddNamespace(nsp);
        }
        headNsp = nsp;
    }
    else {
        nsp = currNsp->FindInnerNamespace(nss);
        if (nsp == nullptr) {
            nsp = new Namespace(nss);
            AddNamespace(nsp);
            currNsp->AddInnerNamespace(nsp);
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

Type* Pool::FindType(
    /* [in] */ const String& typeName)
{
    if (typeName.IsNullOrEmpty()) return nullptr;
    return mTypes.Get(typeName);
}

String Pool::Dump(
    /* [in] */ const String& prefix)
{
    StringBuilder buider;

    for (int i = 0; i < mEnumerations.GetSize(); i++) {
        String enumStr = mEnumerations.Get(i)->Dump(String("    "));
        buider.Append(enumStr).Append("\n");
    }
    for (int i = 0; i < mInterfaces.GetSize(); i++) {
        if (!mInterfaces.Get(i)->IsDefined()) continue;
        String itfStr = mInterfaces.Get(i)->Dump(String("    "));
        buider.Append(itfStr).Append("\n");
    }

    return buider.ToString();
}

}
}
