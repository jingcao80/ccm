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

#include "reflection/CMetaConstant.h"
#include "reflection/CMetaType.h"
#include "reflection/CMetaValue.h"
#include "reflection/reflection.h"
#include "util/comolog.h"

namespace como {

const char* CMetaConstant::TAG = "CMetaConstant";

COMO_INTERFACE_IMPL_LIGHT_1(CMetaConstant, LightRefBase, IMetaConstant)

CMetaConstant::CMetaConstant(
    /* [in] */ MetaComponent* mc,
    /* [in] */ MetaConstant* mk)
    : mMetadata(mk)
    , mName(mk->mName)
    , mNamespace(mk->mNamespace)
{
    mType = new CMetaType(mc, mc->mTypes[mk->mTypeIndex]);
    mValue = BuildValue(mType);
}

ECode CMetaConstant::GetName(
    /* [out] */ String& name)
{
    name = mName;
    return NOERROR;
}

ECode CMetaConstant::GetNamespace(
    /* [in] */ String& ns)
{
    ns = mNamespace.Equals(NAMESPACE_GLOBAL) ? "" : mNamespace;
    return NOERROR;
}

ECode CMetaConstant::GetType(
    /* [out] */ AutoPtr<IMetaType>& type)
{
    type = mType;
    return NOERROR;
}

ECode CMetaConstant::GetValue(
    /* [out] */ AutoPtr<IMetaValue>& value)
{
    value = mValue;
    return NOERROR;
}

AutoPtr<IMetaValue> CMetaConstant::BuildValue(
    /* [in] */ IMetaType* type)
{
    AutoPtr<CMetaValue> value = new CMetaValue();
    value->mType = type;
    TypeKind kind;
    type->GetTypeKind(kind);
    switch(kind) {
        case TypeKind::Boolean:
            value->mBooleanValue = mMetadata->mValue.mBooleanValue;
            break;
        case TypeKind::Char:
        case TypeKind::Byte:
        case TypeKind::Short:
        case TypeKind::Integer:
        case TypeKind::Enum:
            value->mIntegerValue = mMetadata->mValue.mIntegralValue;
            break;
        case TypeKind::Long:
            value->mLongValue = mMetadata->mValue.mIntegralValue;
            break;
        case TypeKind::Float:
            value->mFloatValue = mMetadata->mValue.mFloatingPointValue;
            break;
        case TypeKind::Double:
            value->mDoubleValue = mMetadata->mValue.mFloatingPointValue;
            break;
        case TypeKind::String:
            value->mStringValue = mMetadata->mValue.mStringValue;
            break;
        default:
            Logger::E(TAG, "The type of the constant is not supported.");
            break;
    }
    return value;
}

} // namespace como
