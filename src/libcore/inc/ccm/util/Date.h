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

#ifndef __CCM_UTIL_DATE_H__
#define __CCM_UTIL_DATE_H__

#include "ccm/core/SyncObject.h"
#include "ccm.core.ICloneable.h"
#include "ccm.core.IComparable.h"
#include "ccm.io.ISerializable.h"
#include "ccm.util.IDate.h"

using ccm::core::ICloneable;
using ccm::core::IComparable;
using ccm::core::SyncObject;
using ccm::io::ISerializable;

namespace ccm {
namespace util {

class Date
    : public SyncObject
    , public IDate
    , public ICloneable
    , public IComparable
    , public ISerializable
{
public:
    CCM_INTERFACE_DECL();

    ECode Constructor();

    ECode Constructor(
        /* [in] */ Long date);

    ECode Constructor(
        /* [in] */ Integer year,
        /* [in] */ Integer month,
        /* [in] */ Integer date);

    ECode Constructor(
        /* [in] */ Integer year,
        /* [in] */ Integer month,
        /* [in] */ Integer date,
        /* [in] */ Integer hrs,
        /* [in] */ Integer min);

    ECode Constructor(
        /* [in] */ Integer year,
        /* [in] */ Integer month,
        /* [in] */ Integer date,
        /* [in] */ Integer hrs,
        /* [in] */ Integer min,
        /* [in] */ Integer sec);

    ECode Constructor(
        /* [in] */ const String& s);

    ECode After(
        /* [in] */ IDate* when,
        /* [out] */ Boolean* after) override;

    ECode Before(
        /* [in] */ IDate* when,
        /* [out] */ Boolean* before) override;

    ECode GetDate(
        /* [out] */ Integer* date) override;

    ECode GetDay(
        /* [out] */ Integer* day) override;

    ECode GetHours(
        /* [out] */ Integer* hours) override;

    ECode GetMinutes(
        /* [out] */ Integer* minutes) override;

    ECode GetMonth(
        /* [out] */ Integer* month) override;

    ECode GetSeconds(
        /* [out] */ Integer* seconds) override;

    ECode GetTime(
        /* [out] */ Long* time) override;

    ECode GetTimezoneOffset(
        /* [out] */ Integer* tzOffset) override;

    ECode GetYear(
        /* [out] */ Integer* year) override;

    ECode SetDate(
        /* [in] */ Integer date) override;

    ECode SetHours(
        /* [in] */ Integer hours) override;

    ECode SetMinutes(
        /* [in] */ Integer minutes) override;

    ECode SetMonth(
        /* [in] */ Integer month) override;

    ECode SetSeconds(
        /* [in] */ Integer seconds) override;

    ECode SetTime(
        /* [in] */ Long time) override;

    ECode SetYear(
        /* [in] */ Integer year) override;

    ECode ToGMTString(
        /* [out] */ String* str) override;

    ECode ToLocaleString(
        /* [out] */ String* str) override;

    ECode CompareTo(
        /* [in] */ IInterface* other,
        /* [out] */ Integer* result) override;

protected:
    ECode CloneImpl(
        /* [in] */ IDate* newObj);

private:
    Long mFastTime;
};

}
}

#endif // __CCM_UTIL_DATE_H__
