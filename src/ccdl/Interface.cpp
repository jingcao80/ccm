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

#include "Interface.h"
#include "../util/StringBuilder.h"

#include <stdlib.h>

namespace ccm {
namespace ccdl {

Interface::~Interface()
{
    mNamespace = nullptr;

    for (int i = 0; i < mMethodIndex; i++) {
        Method* m = mMethods[i];
        delete m;
    }
    if (mMethods != nullptr) {
        free(mMethods);
        mMethods = nullptr;
    }
}

Interface& Interface::SetBaseInterface(
    /* [in] */ Interface* baseItf)
{
    if (baseItf == nullptr) return *this;

    mBaseInterface = baseItf;
    return *this;
}

Interface& Interface::SetAttribute(
    /* [in] */ const Attribute& attr)
{
    mUuid.Parse(attr.mUuid);
    mVersion = attr.mVersion;
    mDescription = attr.mDescription;
    return *this;
}

Interface& Interface::AddMethod(
    /* [in] */ Method* method)
{
    if (method == nullptr) return *this;

    if (mMethodIndex >= mMethodCapacity) {
        if (!EnlargeMethodArray()) return *this;
    }

    mMethods[mMethodIndex++] = method;
    return *this;
}

Interface& Interface::AddConstantDataMember(
    /* [in] */ ConstantDataMember* dataMember)
{
    if (dataMember == nullptr) return *this;

    if (mConstDataMemberIndex >= mConstDataMemberCapacity) {
        if (!EnlargeConstantDataMemberArray()) return *this;
    }

    mConstDataMembers[mConstDataMemberIndex++] = dataMember;
    return *this;
}

bool Interface::EnlargeMethodArray()
{
    int newSize = mMethodCapacity == 0 ? 20 : mMethodCapacity + 30;
    Method** newArray = (Method**)calloc(sizeof(Method*), newSize);
    if (newArray == nullptr) {
        return false;
    }

    if (mMethods != nullptr) {
        memcpy(newArray, mMethods, mMethodCapacity);
        free(mMethods);
    }
    mMethodCapacity = newSize;
    mMethods = newArray;
    return true;
}

bool Interface::EnlargeConstantDataMemberArray()
{
    int newSize = mConstDataMemberCapacity == 0 ? 10 : mConstDataMemberCapacity + 10;
    ConstantDataMember** newArray =
            (ConstantDataMember**)calloc(sizeof(ConstantDataMember*), newSize);
    if (newArray == nullptr) {
        return false;
    }

    if (mConstDataMembers != nullptr) {
        memcpy(newArray, mConstDataMembers, mConstDataMemberCapacity);
        free(mConstDataMembers);
    }
    mConstDataMemberCapacity = newSize;
    mConstDataMembers = newArray;
    return true;
}

String Interface::Dump(
    /* [in] */ const String& prefix)
{
    StringBuilder builder;

    builder.Append(prefix).Append("interface ").Append(mName).Append("[");
    builder.Append("uuid:").Append(mUuid.Dump());
    if (!mVersion.IsNullOrEmpty()) {
        builder.Append(", version:").Append(mVersion);
    }
    if (!mDescription.IsNullOrEmpty()) {
        builder.Append(", description:").Append(mDescription);
    }
    builder.Append("]\n");
    for (int i = 0; i < mConstDataMemberIndex; i++) {
        String dataMemberStr = mConstDataMembers[i]->Dump(String("    "));
        builder.Append(prefix).Append(dataMemberStr);
    }
    for (int i = 0; i < mMethodIndex; i++) {
        String methodStr = mMethods[i]->Dump(String("    "));
        builder.Append(prefix).Append(methodStr);
    }

    return builder.ToString();
}

}
}
