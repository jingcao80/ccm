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

#include "ccm/core/System.h"
#include "ccm/util/Date.h"

using ccm::core::IID_ICloneable;
using ccm::core::IID_IComparable;
using ccm::core::System;
using ccm::io::IID_ISerializable;

namespace ccm {
namespace util {

CCM_INTERFACE_IMPL_4(Date, SyncObject, IDate, ICloneable, IComparable, ISerializable);

ECode Date::Constructor()
{
    return Constructor(System::GetCurrentTimeMillis());
}

ECode Date::Constructor(
    /* [in] */ Long date)
{
    mFastTime = date;
    return NOERROR;
}

ECode Date::Constructor(
    /* [in] */ Integer year,
    /* [in] */ Integer month,
    /* [in] */ Integer date)
{
    return Constructor(year, month, date, 0, 0 ,0);
}

ECode Date::Constructor(
    /* [in] */ Integer year,
    /* [in] */ Integer month,
    /* [in] */ Integer date,
    /* [in] */ Integer hrs,
    /* [in] */ Integer min)
{
    return Constructor(year, month, date, hrs, min, 0);
}

ECode Date::Constructor(
    /* [in] */ Integer year,
    /* [in] */ Integer month,
    /* [in] */ Integer date,
    /* [in] */ Integer hrs,
    /* [in] */ Integer min,
    /* [in] */ Integer sec)
{
    return NOERROR;
}

ECode Date::Constructor(
    /* [in] */ const String& s)
{
    return NOERROR;
}

ECode Date::After(
    /* [in] */ IDate* when,
    /* [out] */ Boolean* after)
{
    return NOERROR;
}

ECode Date::Before(
    /* [in] */ IDate* when,
    /* [out] */ Boolean* before)
{
    return NOERROR;
}

ECode Date::GetDate(
    /* [out] */ Integer* date)
{
    return NOERROR;
}

ECode Date::GetDay(
    /* [out] */ Integer* day)
{
    return NOERROR;
}

ECode Date::GetHours(
    /* [out] */ Integer* hours)
{
    return NOERROR;
}

ECode Date::GetMinutes(
    /* [out] */ Integer* minutes)
{
    return NOERROR;
}

ECode Date::GetMonth(
    /* [out] */ Integer* month)
{
    return NOERROR;
}

ECode Date::GetSeconds(
    /* [out] */ Integer* seconds)
{
    return NOERROR;
}

ECode Date::GetTime(
    /* [out] */ Long* time)
{
    return NOERROR;
}

ECode Date::GetTimezoneOffset(
    /* [out] */ Integer* tzOffset)
{
    return NOERROR;
}

ECode Date::GetYear(
    /* [out] */ Integer* year)
{
    return NOERROR;
}

ECode Date::SetDate(
    /* [in] */ Integer date)
{
    return NOERROR;
}

ECode Date::SetHours(
    /* [in] */ Integer hours)
{
    return NOERROR;
}

ECode Date::SetMinutes(
    /* [in] */ Integer minutes)
{
    return NOERROR;
}

ECode Date::SetMonth(
    /* [in] */ Integer month)
{
    return NOERROR;
}

ECode Date::SetSeconds(
    /* [in] */ Integer seconds)
{
    return NOERROR;
}

ECode Date::SetTime(
    /* [in] */ Long time)
{
    return NOERROR;
}

ECode Date::SetYear(
    /* [in] */ Integer year)
{
    return NOERROR;
}

ECode Date::ToGMTString(
    /* [out] */ String* str)
{
    return NOERROR;
}

ECode Date::ToLocaleString(
    /* [out] */ String* str)
{
    return NOERROR;
}

ECode Date::CompareTo(
    /* [in] */ IInterface* other,
    /* [out] */ Integer* result)
{
    return NOERROR;
}

ECode Date::CloneImpl(
    /* [in] */ IDate* newObj)
{
    return NOERROR;
}

}
}
