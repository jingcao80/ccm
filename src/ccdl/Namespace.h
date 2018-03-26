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

#include "ASTNode.h"
#include "../util/ArrayList.h"
#include "../util/String.h"

namespace ccm {
namespace ccdl {

class Namespace : public ASTNode
{
public:
    Namespace(
        /* [in] */ const String& nsStr)
        : mName(nsStr)
        , mOuterNamespace(nullptr)
        , mInnerNamespaces(5, false)
    {}

    ~Namespace()
    { mOuterNamespace = nullptr; }

    inline String GetName()
    { return mName; }

    inline Namespace& SetOuterNamespace(
        /* [in] */ Namespace* outerNS)
    { mOuterNamespace = outerNS; return *this; }

    bool AddInnerNamespace(
        /* [in] */ Namespace* innerNS);

    inline int GetInnerNamespaceNumber()
    { return mInnerNamespaces.GetSize(); }

    inline Namespace* GetInnerNamespace(
        /* [in] */ int index)
    { return mInnerNamespaces.Get(index); }

    Namespace* FindInnerNamespace(
        /* [in] */ const String& nsString);

    inline Namespace* GetOuterNamespace()
    { return mOuterNamespace; }

    inline int GetCoclassNumber()
    { return 0; }

    inline int GetEnumerationNumber()
    { return 0; }

    inline int GetInterfaceNumber()
    { return 0; }

    String ToString() override;

    inline String ToShortString()
    { return mName; }

private:
    String mName;
    Namespace* mOuterNamespace;
    ArrayList<Namespace*> mInnerNamespaces;
};

}
}

#endif // __CCM_CCDL_NAMESPACE_H__
