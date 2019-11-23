//=========================================================================
// Copyright (C) 2018 The C++ Component Model(COMO) Open Source Project
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

#include "ast/Type.h"
#include "ast/Module.h"
#include "ast/Namespace.h"

namespace cdlc {

AutoPtr<Module> Type::GetModule()
{
    return mModule;
}

bool Type::IsECodeType()
{
    return false;
}

bool Type::IsBooleanType()
{
    return false;
}

bool Type::IsCharType()
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

bool Type::IsFloatType()
{
    return false;
}

bool Type::IsDoubleType()
{
    return false;
}

bool Type::IsIntegralType()
{
    return false;
}

bool Type::IsFloatingPointType()
{
    return false;
}

bool Type::IsNumericType()
{
    return IsIntegralType() || IsFloatingPointType();
}

bool Type::IsHANDLEType()
{
    return false;
}

bool Type::IsStringType()
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

bool Type::IsCoclassType()
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

bool Type::IsTripleType()
{
    return false;
}

bool Type::IsArrayType()
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

bool Type::IsBuildinType()
{
    return false;
}

String Type::ToString()
{
    if (mNamespace != nullptr) {
        return mNamespace->ToString() + "::" + mName;
    }
    else {
        return mName;
    }
}

String Type::Dump(
    /* [in] */ const String& prefix)
{
    return prefix + ToString();
}

void Type::CloneBase(
    /* [in] */ Type* clone,
    /* [in] */ Module* module)
{
    clone->mName = mName;
    clone->mExternalModuleName = mExternalModuleName.IsEmpty()
            ? mModule->GetName()
            : mExternalModuleName;
    clone->mDeepCopied = true;
    if (mNamespace != nullptr) {
        AutoPtr<Namespace> ns = module->ParseNamespace(mNamespace->ToString());
        ns->AddType(clone);
    }
}

AutoPtr<Type> Type::Choose(
    /* [in] */ Type* type1,
    /* [in] */ Type* type2)
{
    if (type1->IsDoubleType()) {
        return type1;
    }
    else if (type1->IsFloatType()) {
        return type2->IsDoubleType() ? type2 : type1;
    }
    else if (type1->IsLongType()) {
        return type2->IsFloatingPointType() ? type2 : type1;
    }
    return type2;
}

}
