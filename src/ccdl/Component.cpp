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

#include "Component.h"
#include "../util/Logger.h"
#include "../util/StringBuilder.h"

#include <stdlib.h>
#include <string.h>

namespace ccm {
namespace ccdl {

Component::Component(
    /* [in] */ const String& cdlFile)
    : mCdlFile(cdlFile)
    , mEnumCapacity(0)
    , mEnumIndex(0)
    , mEnumerations(nullptr)
    , mItfCapacity(0)
    , mItfIndex(0)
    , mInterfaces(nullptr)
    , mNSCapacity(0)
    , mNSIndex(0)
    , mNamespaces(nullptr)
    , mTempTypeCapacity(0)
    , mTempTypeIndex(0)
    , mTempTypes(nullptr)
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

Component::~Component()
{
    for (int i = 0; i < mEnumIndex; i++) {
        Enumeration* enumeration = mEnumerations[i];
        delete enumeration;
    }
    if (mEnumerations != nullptr) free(mEnumerations);
    mEnumerations = nullptr;

    for (int i = 0; i < mItfIndex; i++) {
        Interface* itf = mInterfaces[i];
        delete itf;
    }
    if (mInterfaces != nullptr) free(mInterfaces);
    mInterfaces = nullptr;

    for (int i = 0; i < mNSIndex; i++) {
        Namespace* ns = mNamespaces[i];
        delete ns;
    }
    if (mNamespaces != nullptr) free(mNamespaces);
    mNamespaces = nullptr;

    for (int i = 0; i < mTempTypeIndex; i++) {
        Type* ty = mTempTypes[i];
        delete ty;
    }
    if (mTempTypes != nullptr) free(mTempTypes);
    mTempTypes = nullptr;

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

bool Component::AddEnumeration(
    /* [in] */ Enumeration* enumeration)
{
    if (enumeration == nullptr) return true;

    if (mEnumIndex >= mEnumCapacity) {
        if (!EnlargeEnumerationArray()) return false;
    }

    mEnumerations[mEnumIndex++] = enumeration;
    mTypes.Put(enumeration->ToString(), enumeration);
    return true;
}

bool Component::AddInterface(
    /* [in] */ Interface* interface)
{
    if (interface == nullptr) return true;

    if (mItfIndex >= mItfCapacity) {
        if (!EnlargeInterfaceArray()) return false;
    }

    mInterfaces[mItfIndex++] = interface;
    mTypes.Put(interface->ToString(), interface);
    return true;
}

Interface* Component::FindInterface(
    /* [in] */ const String& itfFullName)
{
    Type* type = FindType(itfFullName);
    if (type == nullptr || !type->IsInterface()) {
        return nullptr;
    }
    return (Interface*)type;
}

bool Component::AddNamespace(
    /* [in] */ Namespace* ns)
{
    if (ns == nullptr) return true;

    if (mNSIndex >= mNSCapacity) {
        if (!EnlargeNamespaceArray()) return false;
    }

    mNamespaces[mNSIndex++] = ns;
    return true;
}

Namespace* Component::ParseNamespace(
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
            currNsp->AddInnerNamespace(nsp);
        }
    }
    return headNsp;
}

Namespace* Component::FindNamespace(
    /* [in] */ const String& nsString)
{
    for (int i = 0; i < mNSIndex; i++) {
        Namespace* ns = mNamespaces[i];
        if (ns->ToShortString().Equals(nsString)) {
            return ns;
        }
    }
    return nullptr;
}

bool Component::AddTemporaryType(
    /* [in] */ Type* type)
{
    if (type == nullptr) return true;

    if (mTempTypeIndex >= mTempTypeCapacity) {
        if (!EnlargeTempTypeArray()) return false;
    }

    mTempTypes[mTempTypeIndex++] = type;
    mTypes.Put(type->ToString(), type);
    return true;
}

Type* Component::FindType(
    /* [in] */ const String& typeName)
{
    if (typeName.IsNullOrEmpty()) return nullptr;
    return mTypes.Get(typeName);
}

bool Component::EnlargeEnumerationArray()
{
    int newSize = mEnumCapacity == 0? 10 : mEnumCapacity + 10;
    Enumeration** newArray = (Enumeration**)calloc(sizeof(Enumeration*), newSize);
    if (newArray == nullptr) {
        return false;
    }

    if (mEnumerations != nullptr) {
        memcpy(newArray, mEnumerations, sizeof(Enumeration*) * mEnumCapacity);
        free(mEnumerations);
    }
    mEnumCapacity = newSize;
    mEnumerations = newArray;
    return true;
}

bool Component::EnlargeInterfaceArray()
{
    int newSize = mItfCapacity == 0? 10 : mItfCapacity * 2;
    Interface** newArray = (Interface**)calloc(sizeof(Interface*), newSize);
    if (newArray == nullptr) {
        return false;
    }

    if (mInterfaces != nullptr) {
        memcpy(newArray, mInterfaces, sizeof(Interface*) * mItfCapacity);
        free(mInterfaces);
    }
    mItfCapacity = newSize;
    mInterfaces = newArray;
    return true;
}

bool Component::EnlargeNamespaceArray()
{
    int newSize = mNSCapacity == 0? 5 : mNSCapacity + 5;
    Namespace** newArray = (Namespace**)calloc(sizeof(Namespace*), newSize);
    if (newArray == nullptr) {
        return false;
    }

    if (mNamespaces != nullptr) {
        memcpy(newArray, mNamespaces, sizeof(Namespace*) * mNSCapacity);
        free(mNamespaces);
    }
    mNSCapacity = newSize;
    mNamespaces = newArray;
    return true;
}

bool Component::EnlargeTempTypeArray()
{
    int newSize = mTempTypeCapacity == 0? 10 : mTempTypeCapacity + 10;
    Type** newArray = (Type**)calloc(sizeof(Type*), newSize);
    if (newArray == nullptr) {
        return false;
    }

    if (mTempTypes != nullptr) {
        memcpy(newArray, mTempTypes, sizeof(Type*) * mTempTypeCapacity);
        free(mTempTypes);
    }
    mTempTypeCapacity = newSize;
    mTempTypes = newArray;
    return true;
}

String Component::Dump()
{
    StringBuilder buider;

    buider.Append("Component[").Append(mCdlFile).Append("]\n");
    for (int i = 0; i < mEnumIndex; i++) {
        String enumStr = mEnumerations[i]->Dump(String("    "));
        buider.Append(enumStr).Append("\n");
    }
    for (int i = 0; i < mItfIndex; i++) {
        if (!mInterfaces[i]->IsDefined()) continue;
        String itfStr = mInterfaces[i]->Dump(String("    "));
        buider.Append(itfStr).Append("\n");
    }

    return buider.ToString();
}

}
}
