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

#ifndef __CCDL_AST_TYPE_H__
#define __CCDL_AST_TYPE_H__

#include "Node.h"
#include "../util/String.h"

namespace ccdl {
namespace ast {

class Namespace;

class Type : public Node
{
public:
    Type()
        : mNamespace(nullptr)
    {}

    virtual ~Type();

    inline String GetName()
    { return mName; }

    inline void SetName(
        /* [in] */ const String& name)
    { mName = name; }

    inline Namespace* GetNamespace()
    { return mNamespace; }

    virtual void SetNamespace(
        /* [in] */ Namespace* ns);

    virtual bool IsPrimitiveType();

    virtual bool IsBooleanType();

    virtual bool IsStringType();

    virtual bool IsHANDLEType();

    virtual bool IsNumericType();

    virtual bool IsIntegralType();

    virtual bool IsByteType();

    virtual bool IsShortType();

    virtual bool IsIntegerType();

    virtual bool IsLongType();

    virtual bool IsCharType();

    virtual bool IsFloatingPointType();

    virtual bool IsFloatType();

    virtual bool IsDoubleType();

    virtual bool IsArray();

    virtual bool IsCoclass();

    virtual bool IsEnumeration();

    virtual bool IsInterface();

    virtual bool IsPointerType();

    virtual String Signature() = 0;

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

#endif // __CCDL_AST_TYPE_H__
