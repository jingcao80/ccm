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
    if (mMethods != nullptr) free(mMethods);
    mMethods = nullptr;
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

    if (mMethodIndex > mMethodCapacity) {
        if (!EnlargeMethodArray()) return *this;
    }

    mMethods[mMethodIndex++] = method;
    return *this;
}

bool Interface::EnlargeMethodArray()
{
    int newSize = mMethodCapacity == 0? 20 : mMethodCapacity + 30;
    Method** newArray = (Method**)calloc(sizeof(Method*), newSize);
    if (newArray == nullptr) {
        return false;
    }

    memcpy(newArray, mMethods, mMethodCapacity);
    mMethodCapacity = newSize;
    free(mMethods);
    mMethods = newArray;
    return true;
}

}
}