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
class Pool;

class Type : public Node
{
public:
    Type();

    virtual ~Type();

    inline String GetName();

    inline void SetName(
        /* [in] */ const String& name);

    inline Namespace* GetNamespace();

    virtual void SetNamespace(
        /* [in] */ Namespace* ns);

    inline Pool* GetPool();

    inline void SetPool(
        /* [in] */ Pool* pool);

    inline bool IsExternal();

    inline void SetExternal(
        /* [in] */ bool external);

    inline bool IsSpecialized();

    inline void SetSpecialized(
        /* [in] */ bool specialized);

    inline Type* GetSourceType();

    inline void SetSourceType(
        /* [in] */ Type* source);

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

    virtual bool IsCoclassIDType();

    virtual bool IsComponentIDType();

    virtual bool IsInterfaceIDType();

    virtual bool IsArrayType();

    virtual bool IsCoclassType();

    virtual bool IsEnumerationType();

    virtual bool IsInterfaceType();

    virtual bool IsPointerType();

    virtual String Signature() = 0;

    virtual void Specialize();

    String ToString() override;

    virtual String ToShortString();

    String Dump(
        /* [in] */ const String& prefix) override;

protected:
    String mName;
    Namespace* mNamespace;
    Pool* mPool;
    bool mExternal;
    bool mSpecialized;
    Type* mSourceType;
};

String Type::GetName()
{
    return mName;
}

void Type::SetName(
    /* [in] */ const String& name)
{
    mName = name;
}

Namespace* Type::GetNamespace()
{
    return mNamespace;
}

Pool* Type::GetPool()
{
    return mPool;
}

void Type::SetPool(
    /* [in] */ Pool* pool)
{
    mPool = pool;
}

bool Type::IsExternal()
{
    return mExternal;
}

void Type::SetExternal(
    /* [in] */ bool external)
{
    mExternal = external;
}

bool Type::IsSpecialized()
{
    return mSpecialized;
}

void Type::SetSpecialized(
    /* [in] */ bool specialized)
{
    mSpecialized = specialized;
}

Type* Type::GetSourceType()
{
    return mSourceType;
}

void Type::SetSourceType(
    /* [in] */ Type* source)
{
    mSourceType = source;
}

}
}

#endif // __CCDL_AST_TYPE_H__
