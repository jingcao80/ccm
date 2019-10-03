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

#include "CMetaConstant.h"
#include "CMetaType.h"
#include "CMetaValue.h"

namespace ccm {

COMO_INTERFACE_IMPL_LIGHT_1(CMetaConstant, LightRefBase, IMetaConstant)

CMetaConstant::CMetaConstant(
    /* [in] */ MetaComponent* mc,
    /* [in] */ MetaConstant* mk)
    : mMetadata(mk)
    , mName(mk->mName)
{
    mType = new CMetaType(mc, mc->mTypes[mk->mTypeIndex]);
    mValue = BuildValue(mType);
}

CMetaConstant::~CMetaConstant()
{}

ECode CMetaConstant::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    *name = mName;
    return NOERROR;
}

ECode CMetaConstant::GetType(
    /* [out] */ IMetaType** type)
{
    VALIDATE_NOT_NULL(type);

    *type = mType;
    REFCOUNT_ADD(*type);
    return NOERROR;
}

ECode CMetaConstant::GetValue(
    /* [out] */ IMetaValue** value)
{
    VALIDATE_NOT_NULL(value);

    *value = mValue;
    REFCOUNT_ADD(*value);
    return NOERROR;
}

AutoPtr<IMetaValue> CMetaConstant::BuildValue(
    /* [in] */ IMetaType* type)
{
    CMetaValue* mvObj = new CMetaValue();
    mvObj->mType = type;
    CcmTypeKind kind;
    type->GetTypeKind((Integer*)&kind);
    switch(kind) {
        case CcmTypeKind::Boolean:
            mvObj->mBooleanValue = mMetadata->mValue.mBoolean;
            break;
        case CcmTypeKind::Char:
        case CcmTypeKind::Byte:
        case CcmTypeKind::Short:
        case CcmTypeKind::Integer:
            mvObj->mIntegerValue = mMetadata->mValue.mInteger;
            break;
        case CcmTypeKind::Long:
            mvObj->mLongValue = mMetadata->mValue.mLong;
            break;
        case CcmTypeKind::Float:
            mvObj->mFloatValue = mMetadata->mValue.mFloat;
            break;
        case CcmTypeKind::Double:
            mvObj->mDoubleValue = mMetadata->mValue.mDouble;
            break;
        case CcmTypeKind::String:
            mvObj->mStringValue = mMetadata->mValue.mString;
            break;
    }
    return mvObj;
}

}
