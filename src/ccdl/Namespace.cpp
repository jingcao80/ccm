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

namespace ccm {
namespace ccdl {

bool Namespace::AddInnerNamespace(
    /* [in] */ Namespace* innerNS)
{
    if (innerNS == nullptr) return true;

    bool ret = mInnerNamespaces.Add(innerNS);
    if (ret) innerNS->mOuterNamespace = this;
    return ret;
}

Namespace* Namespace::FindInnerNamespace(
    /* [in] */ const String& nsString)
{
    for (int i = 0; i < mInnerNamespaces.GetSize(); i++) {
        Namespace* ns = mInnerNamespaces.Get(i);
        if (ns->mName.Equals(nsString)) {
            return ns;
        }
    }
    return nullptr;
}

String Namespace::ToString()
{
    String nsString;
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