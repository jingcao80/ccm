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

#ifndef __COMO_CMETAVALUE_H__
#define __COMO_CMETAVALUE_H__

#include "comotypes.h"
#include "metadata/Component.h"
#include "util/comoref.h"

namespace como {

class CMetaValue
    : public LightRefBase
    , public IMetaValue
{
public:
    COMO_INTERFACE_DECL();

    ECode GetType(
        /* [out] */ AutoPtr<IMetaType>& type) override;

    ECode GetBooleanValue(
        /* [out] */ Boolean& value) override;

    ECode GetCharValue(
        /* [out] */ Char& value) override;

    ECode GetByteValue(
        /* [out] */ Byte& value) override;

    ECode GetShortValue(
        /* [out] */ Short& value) override;

    ECode GetIntegerValue(
        /* [out] */ Integer& value) override;

    ECode GetLongValue(
        /* [out] */ Long& value) override;

    ECode GetFloatValue(
        /* [out] */ Float& value) override;

    ECode GetDoubleValue(
        /* [out] */ Double& value) override;

    ECode GetStringValue(
        /* [out] */ String& value) override;

    ECode GetRadix(
        /* [out] */ Integer& radix) override;

public:
    AutoPtr<IMetaType> mType;
    Boolean mBooleanValue;
    Integer mIntegerValue;
    Long mLongValue;
    Float mFloatValue;
    Double mDoubleValue;
    String mStringValue;
    Integer mRadix;
};

} // namespace como

#endif // __COMO_CMETAVALUE_H__
