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
    , mNSCapacity(0)
    , mNSIndex(0)
    , mNamespaces(nullptr)
{}

Component::~Component()
{
    for (int i = 0; i < mEnumIndex; i++) {
        Enumeration* enumeration = mEnumerations[i];
        delete enumeration;
    }
    if (mEnumerations != nullptr) free(mEnumerations);

    for (int i = 0; i < mNSIndex; i++) {
        Namespace* ns = mNamespaces[i];
        delete ns;
    }
    if (mNamespaces != nullptr) free(mNamespaces);
}

bool Component::AddEnumeration(
    /* [in] */ Enumeration* enumeration)
{
    if (enumeration == nullptr) return true;

    if (mEnumIndex >= mEnumCapacity) {
        if (!EnlargeEnumerationArray()) return false;
    }

    mEnumerations[mEnumIndex++] = enumeration;
    return true;
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

bool Component::EnlargeEnumerationArray()
{
    int newSize = mEnumCapacity == 0? 10 : mEnumCapacity + 10;
    Enumeration** newArray = (Enumeration**)calloc(sizeof(Enumeration*), newSize);
    if (newArray == nullptr) {
        return false;
    }

    memcpy(newArray, mEnumerations, sizeof(Enumeration*) * mEnumCapacity);
    mEnumCapacity = newSize;
    free(mEnumerations);
    mEnumerations = newArray;
    return true;
}

bool Component::EnlargeNamespaceArray()
{
    int newSize = mNSCapacity == 0? 5 : mNSCapacity + 5;
    Namespace** newArray = (Namespace**)calloc(sizeof(Namespace*), newSize);
    if (newArray == nullptr) {
        return false;
    }

    memcpy(newArray, mNamespaces, sizeof(Namespace*) * mNSCapacity);
    mNSCapacity = newSize;
    free(mNamespaces);
    mNamespaces = newArray;
    return true;
}

}
}
