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

#ifndef __CCM_CCDL_NAMESPACE_H__
#define __CCM_CCDL_NAMESPACE_H__

#include "ASTElement.h"
#include "../util/String.h"

namespace ccm {
namespace ccdl {

class Namespace : public ASTElement
{
public:
    Namespace(
        /* [in] */ const String& nsStr)
        : mNSString(nsStr)
        , mOuterNamespace(nullptr)
        , mInnerNSCapacity(0)
        , mInnerNSIndex(0)
        , mInnerNamespaces(nullptr)
    {}

    ~Namespace();

    inline Namespace& SetOuterNamespace(
        /* [in] */ Namespace* outerNS) { mOuterNamespace = outerNS; return *this; }
    bool AddInnerNamespace(
        /* [in] */ Namespace* innerNS);
    Namespace* FindInnerNamespace(
        /* [in] */ const String& nsString);

    inline int GetInnerNamespaceNumber() { return mInnerNSIndex; }
    inline Namespace** GetInnerNamespaces() { return mInnerNamespaces; }

    String ToString();
    inline String ToShortString() { return mNSString; }

private:
    bool EnlargeInnerNamespaces();

private:
    String mNSString;
    Namespace* mOuterNamespace;
    int mInnerNSCapacity;
    int mInnerNSIndex;
    Namespace** mInnerNamespaces;
};

}
}

#endif // __CCM_CCDL_NAMESPACE_H__
