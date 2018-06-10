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

#include "core/CInteger.h"

using ccm::io::IID_ISerializable;

namespace ccm {
namespace core {

CCM_INTERFACE_IMPL_4(CInteger, SyncObject, IInteger, INumber, ISerializable, IComparable);

CCM_OBJECT_IMPL(CInteger);

ECode CInteger::constructor(
    /* [in] */ Integer value)
{
    mValue = value;
    return NOERROR;
}

ECode CInteger::ByteValue(
    /* [out] */ Byte* value)
{
    VALIDATE_NOT_NULL(value);

    *value = (Byte)mValue;
    return NOERROR;
}

ECode CInteger::ShortValue(
    /* [out] */ Short* value)
{
    VALIDATE_NOT_NULL(value);

    *value = (Short)mValue;
    return NOERROR;
}

ECode CInteger::IntValue(
    /* [out] */ Integer* value)
{
    VALIDATE_NOT_NULL(value);

    *value = mValue;
    return NOERROR;
}

ECode CInteger::LongValue(
    /* [out] */ Long* value)
{
    VALIDATE_NOT_NULL(value);

    *value = (Long)mValue;
    return NOERROR;
}

ECode CInteger::FloatValue(
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);

    *value = (Float)mValue;
    return NOERROR;
}

ECode CInteger::DoubleValue(
    /* [out] */ Double* value)
{
    VALIDATE_NOT_NULL(value);

    *value = (Double)mValue;
    return NOERROR;
}

ECode CInteger::GetValue(
    /* [out] */ Integer* value)
{
    VALIDATE_NOT_NULL(value);

    *value = mValue;
    return NOERROR;
}

ECode CInteger::CompareTo(
    /* [in] */ IInterface* other,
    /* [out] */ Integer* result)
{
    VALIDATE_NOT_NULL(result);

    IInteger* i = IInteger::Probe(other);
    if (i == nullptr) {
        *result = -1;
        return NOERROR;
    }

    Integer iv;
    i->GetValue(&iv);
    *result = mValue == iv ? 0 : (mValue > iv ? 1 : -1);
    return NOERROR;
}

ECode CInteger::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    IInteger* i = IInteger::Probe(other);
    if (i == nullptr) {
        *result = false;
        return NOERROR;
    }

    Integer iv;
    i->GetValue(&iv);
    *result = mValue == iv;
    return NOERROR;
}

ECode CInteger::GetHashCode(
    /* [out] */ Integer* hash)
{
    VALIDATE_NOT_NULL(hash);

    *hash = mValue;
    return NOERROR;
}

ECode CInteger::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    *str = String::Format("%d", mValue);
    return NOERROR;
}

}
}
