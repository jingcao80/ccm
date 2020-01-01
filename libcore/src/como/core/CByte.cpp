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

#include "como/core/CByte.h"

using como::io::IID_ISerializable;

namespace como {
namespace core {

COMO_INTERFACE_IMPL_4(CByte, SyncObject, IByte, INumber, ISerializable, IComparable);

COMO_OBJECT_IMPL(CByte);

ECode CByte::Constructor(
    /* [in] */ Byte value)
{
    mValue = value;
    return NOERROR;
}

ECode CByte::ByteValue(
    /* [out] */ Byte* value)
{
    VALIDATE_NOT_NULL(value);

    *value = mValue;
    return NOERROR;
}

ECode CByte::ShortValue(
    /* [out] */ Short* value)
{
    VALIDATE_NOT_NULL(value);

    *value = (Short)mValue;
    return NOERROR;
}

ECode CByte::IntegerValue(
    /* [out] */ Integer* value)
{
    VALIDATE_NOT_NULL(value);

    *value = (Integer)mValue;
    return NOERROR;
}

ECode CByte::LongValue(
    /* [out] */ Long* value)
{
    VALIDATE_NOT_NULL(value);

    *value = (Long)mValue;
    return NOERROR;
}

ECode CByte::FloatValue(
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);

    *value = (Float)mValue;
    return NOERROR;
}

ECode CByte::DoubleValue(
    /* [out] */ Double* value)
{
    VALIDATE_NOT_NULL(value);

    *value = (Double)mValue;
    return NOERROR;
}

ECode CByte::GetValue(
    /* [out] */ Byte* value)
{
    VALIDATE_NOT_NULL(value);

    *value = mValue;
    return NOERROR;
}

ECode CByte::CompareTo(
    /* [in] */ IInterface* other,
    /* [out] */ Integer* result)
{
    VALIDATE_NOT_NULL(result);

    IByte* b = IByte::Probe(other);
    if (b == nullptr) {
        *result = -1;
        return NOERROR;
    }

    Byte ov;
    b->GetValue(&ov);
    *result = mValue == ov ? 0 : (mValue > ov ? 1 : -1);
    return NOERROR;
}

ECode CByte::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    IByte* b = IByte::Probe(other);
    if (b == nullptr) {
        *result = false;
        return NOERROR;
    }

    Byte ov;
    b->GetValue(&ov);
    *result = mValue == ov;
    return NOERROR;
}

ECode CByte::GetHashCode(
    /* [out] */ Integer* hash)
{
    VALIDATE_NOT_NULL(hash);

    *hash = (Integer)mValue;
    return NOERROR;
}

ECode CByte::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    *str = String::Format("%d", mValue);
    return NOERROR;
}

}
}
