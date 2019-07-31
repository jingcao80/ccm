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
#include "Type.h"

namespace ccdl {
namespace ast {

Type::Type()
    : mNamespace(nullptr)
    , mIsPredecl(false)
    , mPool(nullptr)
    , mExternal(false)
    , mSpecialized(true)
    , mSourceType(nullptr)
    , mTempTypes(5)
{}

Type::~Type()
{
    mNamespace = nullptr;
    mPool = nullptr;
    mSourceType = nullptr;
}

void Type::SetNamespace(
    /* [in] */ Namespace* ns)
{
    mNamespace = ns;
}

void Type::SetDeclared()
{
    if (mIsPredecl) {
        mIsPredecl = false;
    }
}

bool Type::IsPrimitiveType()
{
    return false;
}

bool Type::IsBooleanType()
{
    return false;
}

bool Type::IsStringType()
{
    return false;
}

bool Type::IsHANDLEType()
{
    return false;
}

bool Type::IsECodeType()
{
    return false;
}

bool Type::IsNumericType()
{
    return false;
}

bool Type::IsIntegralType()
{
    return false;
}

bool Type::IsByteType()
{
    return false;
}

bool Type::IsShortType()
{
    return false;
}

bool Type::IsIntegerType()
{
    return false;
}

bool Type::IsLongType()
{
    return false;
}

bool Type::IsCharType()
{
    return false;
}

bool Type::IsFloatingPointType()
{
    return false;
}

bool Type::IsFloatType()
{
    return false;
}

bool Type::IsDoubleType()
{
    return false;
}

bool Type::IsCoclassIDType()
{
    return false;
}

bool Type::IsComponentIDType()
{
    return false;
}

bool Type::IsInterfaceIDType()
{
    return false;
}

bool Type::IsArrayType()
{
    return false;
}

bool Type::IsCoclassType()
{
    return false;
}

bool Type::IsEnumerationType()
{
    return false;
}

bool Type::IsInterfaceType()
{
    return false;
}

bool Type::IsTripleType()
{
    return false;
}

bool Type::IsPointerType()
{
    return false;
}

bool Type::IsReferenceType()
{
    return false;
}

void Type::Specialize()
{}

bool Type::AddTemporaryType(
    /* [in] */ Type* type)
{
    if (type == nullptr) return true;

    return mTempTypes.Add(type);
}

Type* Type::FindTemporaryType(
    /* [in] */ const String& typeStr)
{
    if (typeStr.IsNullOrEmpty()) return nullptr;

    for (int i = 0; i < mTempTypes.GetSize(); i++) {
        Type* type = mTempTypes.Get(i);
        if (type->ToString().Equals(typeStr)) {
            return type;
        }
    }
    return nullptr;
}

String Type::ToString()
{
    return mNamespace == nullptr ? mName : mNamespace->ToString() + mName;
}

String Type::ToShortString()
{
    return mName;
}

String Type::Dump(
    /* [in] */ const String& prefix)
{
    return ToString();
}

String Type::Kind()
{
    if (IsInterfaceType()) {
        return String("Interface");
    }
    else if (IsEnumerationType()) {
        return String("Enumeration");
    }
    return String("Type");
}

}
}
