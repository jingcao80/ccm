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

#ifndef __CCM_CCDL_TYPE_H__
#define __CCM_CCDL_TYPE_H__

#include "ASTNode.h"
#include "Namespace.h"
#include "../util/String.h"

namespace ccm {
namespace ccdl {

class Type : public ASTNode
{
public:
    Type()
        : mNamespace(nullptr)
    {}

    virtual ~Type();

    inline String GetName()
    { return mName; }

    inline Type& SetName(
        /* [in] */ const String& name)
    { mName = name; return *this; }

    inline Type& SetNamespace(
        /* [in] */ Namespace* ns)
    { mNamespace = ns; return *this; }

    virtual bool IsPrimitiveType();

    virtual bool IsNumericType();

    virtual bool IsIntegralType();

    virtual bool IsFloatingPointType();

    virtual bool IsCoclass();

    virtual bool IsEnumeration();

    virtual bool IsInterface();

    String ToString() override;

    virtual String ToShortString();

    String Dump(
        /* [in] */ const String& prefix) override;

protected:
    String mName;
    Namespace* mNamespace;
};

}
}

#endif // __CCM_CCDL_TYPE_H__
