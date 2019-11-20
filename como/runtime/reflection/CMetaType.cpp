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

#include "reflection/CMetaType.h"

namespace como {

const char* CMetaType::TAG = "CMetaType";

COMO_INTERFACE_IMPL_LIGHT_1(CMetaType, LightRefBase, IMetaType)

CMetaType::CMetaType(
    /* [in] */ MetaComponent* mc,
    /* [in] */ MetaType* mt)
    : mMetadata(mt)
    , mKind(mt->mKind)
    , mMode(TypeModification::NAKED)
{
    int N = mt->mProperties & TYPE_NUMBER_MASK;
    if (N == 1) {
        mMode = ((mt->mProperties >> 2) & TYPE_POINTER)
                ? TypeModification::POINTER : TypeModification::REFERENCE;
    }
    else if (N == 2) {
        if ((mt->mProperties >> 2) & TYPE_POINTER) {
            mMode = ((mt->mProperties >> 4) & TYPE_POINTER)
                ? TypeModification::POINTER_POINTER : TypeModification::POINTER_REFERENCE;
        }
        else {
            mMode = ((mt->mProperties >> 4) & TYPE_POINTER)
                ? TypeModification::REFERENCE_POINTER : TypeModification::REFERENCE_REFERENCE;
        }
    }
    else if (N > 2) {
        Logger::E(TAG, "The pointer number or reference number is large then two.");
    }
    mName = BuildName(mc, mt);
    if (mKind == TypeKind::Array && mt->mIndex != 0) {
        mElementType = new CMetaType(mc, mc->mTypes[mt->mIndex]);
    }
}

ECode CMetaType::GetName(
    /* [out] */ String& name)
{
    name = mName;
    return NOERROR;
}

ECode CMetaType::GetTypeKind(
    /* [out] */ TypeKind& kind)
{
    kind = mKind;
    return NOERROR;
}

ECode CMetaType::GetElementType(
    /* [out] */ AutoPtr<IMetaType>& elemType)
{
    elemType = mElementType;
    return NOERROR;
}

ECode CMetaType::GetTypeModification(
    /* [out] */ TypeModification& mode)
{
    mode = mMode;
    return NOERROR;
}

String CMetaType::BuildName(
    /* [in] */ MetaComponent* mc,
    /* [in] */ MetaType* mt)
{
    String typeStr;

    switch(mt->mKind) {
        case TypeKind::Unknown:
            return "Unknown";
        case TypeKind::Char:
            typeStr = "Char";
            break;
        case TypeKind::Byte:
            typeStr = "Byte";
            break;
        case TypeKind::Short:
            typeStr = "Short";
            break;
        case TypeKind::Integer:
            typeStr = "Integer";
            break;
        case TypeKind::Long:
            typeStr = "Long";
            break;
        case TypeKind::Float:
            typeStr = "Float";
            break;
        case TypeKind::Double:
            typeStr = "Double";
            break;
        case TypeKind::Boolean:
            typeStr = "Boolean";
            break;
        case TypeKind::String:
            typeStr = mMode == TypeModification::NAKED ?
                    "const String&" : "String";
            break;
        case TypeKind::CoclassID:
            typeStr = mMode == TypeModification::NAKED ?
                    "const CoclassID&" : "CoclassID";
            break;
        case TypeKind::ComponentID:
            typeStr = mMode == TypeModification::NAKED ?
                    "const ComponentID&" : "ComponentID";
            break;
        case TypeKind::InterfaceID:
            typeStr = mMode == TypeModification::NAKED ?
                    "const InterfaceID&" : "InterfaceID";
            break;
        case TypeKind::HANDLE:
            typeStr = "HANDLE";
            break;
        case TypeKind::ECode:
            typeStr = "ECode";
            break;
        case TypeKind::Enum:
            typeStr = mc->mEnumerations[mt->mIndex]->mName;
            break;
        case TypeKind::Array: {
            MetaType* elem = mc->mTypes[mt->mIndex];
            typeStr = String::Format("Array<%s>", BuildName(mc, elem).string());
            break;
        }
        case TypeKind::Interface:
            typeStr = mc->mInterfaces[mt->mIndex]->mName;
            break;
        case TypeKind::Triple:
            typeStr = mMode == TypeModification::NAKED ?
                    "const Triple&" : "Triple";
            break;
        case TypeKind::TypeKind:
            typeStr = "TypeKind";
            break;
        default:
            Logger::E(TAG, "The type is not supported in BuildName.");
            break;
    }

    switch (mMode) {
        case TypeModification::POINTER:
            typeStr += "*";
            break;
        case TypeModification::REFERENCE:
            typeStr += "&";
            break;
        case TypeModification::POINTER_POINTER:
            typeStr += "**";
            break;
        case TypeModification::POINTER_REFERENCE:
            typeStr += "*&";
            break;
        case TypeModification::REFERENCE_REFERENCE:
            typeStr += "&&";
            break;
        case TypeModification::REFERENCE_POINTER:
            typeStr += "&*";
            break;
        default:
            break;
    }

    return typeStr;
}

} // namespace como
