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

#include "como/core/ArrayHolder.h"

namespace como {
namespace core {

ArrayHolder::~ArrayHolder()
{
    mArray.FreeData();
}

COMO_INTERFACE_IMPL_1(ArrayHolder, Object, IArrayHolder)

ECode ArrayHolder::Constructor(
    /* [in] */ const Triple& array)
{
    mArray = array;
    return NOERROR;
}

ECode ArrayHolder::GetArray(
    /* [out] */ Triple* array)
{
    VALIDATE_NOT_NULL(array);

    *array = mArray;
    return NOERROR;
}

ECode ArrayHolder::GetArrayType(
    /* [out] */ Integer* type)
{
    VALIDATE_NOT_NULL(type);

    switch(mArray.mType) {
        case CcmTypeKind::Char:
            *type = TYPE_CHAR_ARRAY;
            break;
        case CcmTypeKind::Byte:
            *type = TYPE_BYTE_ARRAY;
            break;
        case CcmTypeKind::Short:
            *type = TYPE_SHORT_ARRAY;
            break;
        case CcmTypeKind::Integer:
            *type = TYPE_INTEGER_ARRAY;
            break;
        case CcmTypeKind::Long:
            *type = TYPE_LONG_ARRAY;
            break;
        case CcmTypeKind::Float:
            *type = TYPE_FLOAT_ARRAY;
            break;
        case CcmTypeKind::Double:
            *type = TYPE_DOUBLE_ARRAY;
            break;
        case CcmTypeKind::Boolean:
            *type = TYPE_BOOLEAN_ARRAY;
            break;
        case CcmTypeKind::String:
            *type = TYPE_STRING_ARRAY;
            break;
        case CcmTypeKind::Interface:
            *type = TYPE_INTERFACE_ARRAY;
            break;
        default:
            *type = TYPE_UNKNOWN_ARRAY;
            break;
    }
    return NOERROR;
}

}
}
