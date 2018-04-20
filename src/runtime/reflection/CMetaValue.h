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

#ifndef __CCM_CMETAVALUE_H__
#define __CCM_CMETAVALUE_H__

#include "ccmautoptr.h"
#include "ccmrefbase.h"
#include "Component.h"

namespace ccm {

class CMetaValue
    : public LightRefBase
    , public IMetaValue
{
public:
    CCM_INTERFACE_DECL();

    ECode GetType(
        /* [out] */ IMetaType** type);

    ECode GetBooleanValue(
        /* [out] */ Boolean* value);

    ECode GetCharValue(
        /* [out] */ Char* value);

    ECode GetByteValue(
        /* [out] */ Byte* value);

    ECode GetShortValue(
        /* [out] */ Short* value);

    ECode GetIntegerValue(
        /* [out] */ Integer* value);

    ECode GetLongValue(
        /* [out] */ Long* value);

    ECode GetFloatValue(
        /* [out] */ Float* value);

    ECode GetDoubleValue(
        /* [out] */ Double* value);

    ECode GetStringValue(
        /* [out] */ String* value);

    ECode GetRadix(
        /* [out] */ Integer* radix);

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

}

#endif // __CCM_CMETAVALUE_H__
