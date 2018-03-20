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

#include <stdlib.h>

namespace ccm {
namespace ccdl {

Namespace::~Namespace()
{
    for (int i = 0; i < mInnerNSIndex; i++) {
        Namespace* ns = mInnerNamespaces[i];
        delete ns;
    }
    if (mInnerNamespaces != nullptr) {
        delete mInnerNamespaces;
        mInnerNamespaces = nullptr;
    }
    mOuterNamespace = nullptr;
}

bool Namespace::AddInnerNamespace(
    /* [in] */ Namespace* innerNS)
{
    if (innerNS == nullptr) return true;

    if (mInnerNSIndex >= mInnerNSCapacity) {
        if (!EnlargeInnerNamespaces()) return false;
    }

    mInnerNamespaces[mInnerNSIndex++] = innerNS;
    innerNS->mOuterNamespace = this;
    return true;
}

Namespace* Namespace::FindInnerNamespace(
    /* [in] */ const String& nsString)
{
    for (int i = 0; i < mInnerNSIndex; i++) {
        Namespace* ns = mInnerNamespaces[i];
        if (ns->ToShortString().Equals(nsString)) {
            return ns;
        }
    }
    return nullptr;
}

bool Namespace::EnlargeInnerNamespaces()
{
    int newSize = mInnerNSCapacity == 0 ? 3 : mInnerNSCapacity + 3;
    Namespace** newArray = (Namespace**)calloc(sizeof(Namespace*), newSize);
    if (newArray == nullptr) return false;

    if (mInnerNamespaces != nullptr) {
        memcpy(newArray, mInnerNamespaces, mInnerNSCapacity);
        free(mInnerNamespaces);
    }
    mInnerNamespaces = newArray;
    mInnerNSCapacity = newSize;
    return true;
}

String Namespace::ToString()
{
    String nsString;
    Namespace* ns = this;
    while (ns != nullptr) {
        if (ns->mNSString.Equals("__global__")) return nsString;
        nsString = ns->mNSString + "::" + nsString;
        ns = ns->mOuterNamespace;
    }
    return nsString;
}

}
}