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

#include "CMetaType.h"

namespace ccm {

COMO_INTERFACE_IMPL_LIGHT_1(CMetaType, LightRefBase, IMetaType)

CMetaType::CMetaType()
    : mMetadata(nullptr)
    , mKind(CcmTypeKind::Unknown)
    , mPointerNumber(0)
{}

CMetaType::CMetaType(
    /* [in] */ MetaComponent* mc,
    /* [in] */ MetaType* mt)
    : mMetadata(mt)
    , mKind(mt->mKind)
    , mPointerNumber(mt->mPointerNumber)
{
    mName = BuildName(mc, mt);
    if (mt->mNestedTypeIndex != -1) {
        mElementType = new CMetaType(mc,
                mc->mTypes[mt->mNestedTypeIndex]);
    }
}

CMetaType::~CMetaType()
{
    mMetadata = nullptr;
}

ECode CMetaType::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    *name = mName;
    return NOERROR;
}

ECode CMetaType::GetTypeKind(
    /* [out] */ Integer* kind)
{
    VALIDATE_NOT_NULL(kind);

    *kind = (Integer)mKind;
    return NOERROR;
}

ECode CMetaType::GetElementType(
    /* [out] */ IMetaType** elemType)
{
    VALIDATE_NOT_NULL(elemType);

    *elemType = mElementType;
    REFCOUNT_ADD(*elemType);
    return NOERROR;
}

ECode CMetaType::GetPointerNumber(
    /* [out] */ Integer* number)
{
    VALIDATE_NOT_NULL(number);

    *number = mPointerNumber;
    return NOERROR;
}

String CMetaType::BuildName(
    /* [in] */ MetaComponent* mc,
    /* [in] */ MetaType* mt)
{
    String typeStr;

    switch(mt->mKind) {
        case CcmTypeKind::Unknown:
            return String("Unknown");
        case CcmTypeKind::Char:
            typeStr = "Char";
            break;
        case CcmTypeKind::Byte:
            typeStr = "Byte";
            break;
        case CcmTypeKind::Short:
            typeStr = "Short";
            break;
        case CcmTypeKind::Integer:
            typeStr = "Integer";
            break;
        case CcmTypeKind::Long:
            typeStr = "Long";
            break;
        case CcmTypeKind::Float:
            typeStr = "Float";
            break;
        case CcmTypeKind::Double:
            typeStr = "Double";
            break;
        case CcmTypeKind::Boolean:
            typeStr = "Boolean";
            break;
        case CcmTypeKind::String:
            typeStr = mt->mPointerNumber == 0 ?
                    "const String&" : "String";
            break;
        case CcmTypeKind::CoclassID:
            typeStr = mt->mPointerNumber == 0 ?
                    "const CoclassID&" : "CoclassID";
            break;
        case CcmTypeKind::ComponentID:
            typeStr = mt->mPointerNumber == 0 ?
                    "const ComponentID&" : "ComponentID";
            break;
        case CcmTypeKind::InterfaceID:
            typeStr = mt->mPointerNumber == 0 ?
                    "const InterfaceID&" : "InterfaceID";
            break;
        case CcmTypeKind::HANDLE:
            typeStr = "HANDLE";
            break;
        case CcmTypeKind::ECode:
            typeStr = "ECode";
            break;
        case CcmTypeKind::Enum:
            typeStr = mc->mEnumerations[mt->mIndex]->mName;
            break;
        case CcmTypeKind::Array: {
            MetaType* elem = mc->mTypes[mt->mNestedTypeIndex];
            typeStr = String::Format("Array<%s>",
                    BuildName(mc, elem).string());
            break;
        }
        case CcmTypeKind::Interface:
            typeStr = mc->mInterfaces[mt->mIndex]->mName;
            break;
        case CcmTypeKind::Triple:
            typeStr = mt->mPointerNumber == 0 ?
                    "const Triple&" : "Triple";
            break;
    }

    for (Integer i = 0; i < mt->mPointerNumber; i++) {
        typeStr += "*";
    }

    return typeStr;
}

}
