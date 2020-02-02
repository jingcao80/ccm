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

#include "como/core/CInteger.h"

using como::io::IID_ISerializable;

namespace como {
namespace core {

COMO_INTERFACE_IMPL_4(CInteger, SyncObject, IInteger, INumber, ISerializable, IComparable);

COMO_OBJECT_IMPL(CInteger);

ECode CInteger::Constructor(
    /* [in] */ Integer value)
{
    mValue = value;
    return NOERROR;
}

ECode CInteger::ByteValue(
    /* [out] */ Byte& value)
{
    value = (Byte)mValue;
    return NOERROR;
}

ECode CInteger::ShortValue(
    /* [out] */ Short& value)
{
    value = (Short)mValue;
    return NOERROR;
}

ECode CInteger::IntegerValue(
    /* [out] */ Integer& value)
{
    value = mValue;
    return NOERROR;
}

ECode CInteger::LongValue(
    /* [out] */ Long& value)
{
    value = (Long)mValue;
    return NOERROR;
}

ECode CInteger::FloatValue(
    /* [out] */ Float& value)
{
    value = (Float)mValue;
    return NOERROR;
}

ECode CInteger::DoubleValue(
    /* [out] */ Double& value)
{
    value = (Double)mValue;
    return NOERROR;
}

ECode CInteger::GetValue(
    /* [out] */ Integer& value)
{
    value = mValue;
    return NOERROR;
}

ECode CInteger::CompareTo(
    /* [in] */ IInterface* other,
    /* [out] */ Integer& result)
{
    IInteger* i = IInteger::Probe(other);
    if (i == nullptr) {
        result = -1;
        return NOERROR;
    }

    Integer iv;
    i->GetValue(iv);
    result = mValue == iv ? 0 : (mValue > iv ? 1 : -1);
    return NOERROR;
}

ECode CInteger::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean& result)
{
    IInteger* i = IInteger::Probe(other);
    if (i == nullptr) {
        result = false;
        return NOERROR;
    }

    Integer iv;
    i->GetValue(iv);
    result = mValue == iv;
    return NOERROR;
}

ECode CInteger::GetHashCode(
    /* [out] */ Integer& hash)
{
    hash = mValue;
    return NOERROR;
}

ECode CInteger::ToString(
    /* [out] */ String& str)
{
    str = String::Format("%d", mValue);
    return NOERROR;
}

}
}
