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

#include "reflection/CMetaValue.h"

namespace como {

COMO_INTERFACE_IMPL_LIGHT_1(CMetaValue, LightRefBase, IMetaValue);

ECode CMetaValue::GetType(
    /* [out] */ AutoPtr<IMetaType>& type)
{
    type = mType;
    return NOERROR;
}

ECode CMetaValue::GetBooleanValue(
    /* [out] */ Boolean& value)
{
    value = mBooleanValue;
    return NOERROR;
}

ECode CMetaValue::GetCharValue(
    /* [out] */ Char& value)
{
    value = (Char)mIntegerValue;
    return NOERROR;
}

ECode CMetaValue::GetByteValue(
    /* [out] */ Byte& value)
{
    value = (Byte)mIntegerValue;
    return NOERROR;
}

ECode CMetaValue::GetShortValue(
    /* [out] */ Short& value)
{
    value = (Short)mIntegerValue;
    return NOERROR;
}

ECode CMetaValue::GetIntegerValue(
    /* [out] */ Integer& value)
{
    value = mIntegerValue;
    return NOERROR;
}

ECode CMetaValue::GetLongValue(
    /* [out] */ Long& value)
{
    value = mLongValue;
    return NOERROR;
}

ECode CMetaValue::GetFloatValue(
    /* [out] */ Float& value)
{
    value = mFloatValue;
    return NOERROR;
}

ECode CMetaValue::GetDoubleValue(
    /* [out] */ Double& value)
{
    value = mDoubleValue;
    return NOERROR;
}

ECode CMetaValue::GetStringValue(
    /* [out] */ String& value)
{
    value = mStringValue;
    return NOERROR;
}

ECode CMetaValue::GetRadix(
    /* [out] */ Integer& radix)
{
    radix = mRadix;
    return NOERROR;
}

} // namespace como
