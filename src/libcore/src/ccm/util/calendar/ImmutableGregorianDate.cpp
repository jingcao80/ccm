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

#include "ccm/util/calendar/ImmutableGregorianDate.h"

namespace ccm {
namespace util {
namespace calendar {

static const InterfaceID IID_ImmutableGregorianDate =
        {{0x5501a9ff,0x1afb,0x4023,0x907e,{0xf,0xc,0x7,0xb,0xf,0xf,0x2,0xb,0x8,0x6,0xe,0xd}}, &CID_libcore};

IInterface* ImmutableGregorianDate::Probe(
    /* [in] */ const InterfaceID& iid)
{
    if (iid == IID_ImmutableGregorianDate) {
        return (IBaseCalendarDate*)this;
    }
    return BaseCalendar::Date::Probe(iid);
}

ECode ImmutableGregorianDate::Constructor(
    /* [in] */ IBaseCalendarDate* date)
{
    if (date == nullptr) {
        return ccm::core::E_NULL_POINTER_EXCEPTION;
    }
    mDate = (BaseCalendar::Date*)date;
    return NOERROR;
}

ECode ImmutableGregorianDate::GetEra(
    /* [out] */ IEra** era)
{
    return mDate->GetEra(era);
}

ECode ImmutableGregorianDate::SetEra(
    /* [in] */ IEra* era)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ImmutableGregorianDate::GetYear(
    /* [out] */ Integer* year)
{
    return mDate->GetYear(year);
}

ECode ImmutableGregorianDate::SetYear(
    /* [in] */ Integer year)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ImmutableGregorianDate::AddYear(
    /* [in] */ Integer n)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ImmutableGregorianDate::IsLeapYear(
    /* [out] */ Boolean* leapYear)
{
    return mDate->IsLeapYear(leapYear);
}

ECode ImmutableGregorianDate::SetLeapYear(
    /* [in] */ Boolean leapYear)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ImmutableGregorianDate::GetMonth(
    /* [out] */ Integer* month)
{
    return mDate->GetMonth(month);
}

ECode ImmutableGregorianDate::SetMonth(
    /* [in] */ Integer month)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ImmutableGregorianDate::AddMonth(
    /* [in] */ Integer n)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ImmutableGregorianDate::GetDayOfMonth(
    /* [out] */ Integer* date)
{
    return mDate->GetDayOfMonth(date);
}

ECode ImmutableGregorianDate::SetDayOfMonth(
    /* [in] */ Integer date)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ImmutableGregorianDate::AddDayOfMonth(
    /* [in] */ Integer n)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ImmutableGregorianDate::GetDayOfWeek(
    /* [out] */ Integer* date)
{
    return mDate->GetDayOfWeek(date);
}

ECode ImmutableGregorianDate::GetHours(
    /* [out] */ Integer* hours)
{
    return mDate->GetHours(hours);
}

ECode ImmutableGregorianDate::SetHours(
    /* [in] */ Integer hours)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ImmutableGregorianDate::AddHours(
    /* [in] */ Integer n)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ImmutableGregorianDate::GetMinutes(
    /* [out] */ Integer* minutes)
{
    return mDate->GetMinutes(minutes);
}

ECode ImmutableGregorianDate::SetMinutes(
    /* [in] */ Integer minutes)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ImmutableGregorianDate::AddMinutes(
    /* [in] */ Integer n)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ImmutableGregorianDate::GetSeconds(
    /* [out] */ Integer* seconds)
{
    return mDate->GetSeconds(seconds);
}

ECode ImmutableGregorianDate::SetSeconds(
    /* [in] */ Integer seconds)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ImmutableGregorianDate::AddSeconds(
    /* [in] */ Integer n)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ImmutableGregorianDate::GetMillis(
    /* [out] */ Integer* millis)
{
    return mDate->GetMillis(millis);
}

ECode ImmutableGregorianDate::SetMillis(
    /* [in] */ Integer millis)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ImmutableGregorianDate::AddMillis(
    /* [in] */ Integer n)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ImmutableGregorianDate::GetTimeOfDay(
    /* [out] */ Long* date)
{
    return mDate->GetTimeOfDay(date);
}

ECode ImmutableGregorianDate::SetDate(
    /* [in] */ Integer year,
    /* [in] */ Integer month,
    /* [in] */ Integer dayOfMonth)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ImmutableGregorianDate::AddDate(
    /* [in] */ Integer year,
    /* [in] */ Integer month,
    /* [in] */ Integer dayOfMonth)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ImmutableGregorianDate::SetTimeOfDay(
    /* [in] */ Integer hours,
    /* [in] */ Integer minutes,
    /* [in] */ Integer seconds,
    /* [in] */ Integer millis)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ImmutableGregorianDate::AddTimeOfDay(
    /* [in] */ Integer hours,
    /* [in] */ Integer minutes,
    /* [in] */ Integer seconds,
    /* [in] */ Integer millis)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ImmutableGregorianDate::SetTimeOfDay(
    /* [in] */ Long fraction)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ImmutableGregorianDate::IsNormalized(
    /* [out] */ Boolean* normalized)
{
    return mDate->IsNormalized(normalized);
}

ECode ImmutableGregorianDate::IsStandardTime(
    /* [out] */ Boolean* standardTime)
{
    return mDate->IsStandardTime(standardTime);
}

ECode ImmutableGregorianDate::SetStandardTime(
    /* [in] */ Boolean standardTime)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ImmutableGregorianDate::IsDaylightTime(
    /* [out] */ Boolean* daylightTime)
{
    return mDate->IsDaylightTime(daylightTime);
}

ECode ImmutableGregorianDate::SetLocale(
    /* [in] */ ILocale* loc)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ImmutableGregorianDate::GetZone(
    /* [out] */ ITimeZone** zone)
{
    return mDate->GetZone(zone);
}

ECode ImmutableGregorianDate::SetZone(
    /* [in] */ ITimeZone* zoneinfo)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ImmutableGregorianDate::IsSameDate(
    /* [in] */ ICalendarDate* date,
    /* [out] */ Boolean* same)
{
    return mDate->IsSameDate(date, same);
}

ECode ImmutableGregorianDate::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* same)
{
    VALIDATE_NOT_NULL(same);

    if (IInterface::Equals((IBaseCalendarDate*)this, obj)) {
        *same = true;
        return NOERROR;
    }
    if (obj->Probe(IID_ImmutableGregorianDate) == nullptr) {
        *same = false;
        return NOERROR;
    }
    BaseCalendar::Date* thatDate = ((ImmutableGregorianDate*)
            IBaseCalendarDate::Probe(obj))->mDate;
    *same = Object::Equals((IBaseCalendarDate*)mDate, (IBaseCalendarDate*)thatDate);
    return NOERROR;
}

ECode ImmutableGregorianDate::GetHashCode(
    /* [out] */ Integer* hash)
{
    return mDate->GetHashCode(hash);
}

ECode ImmutableGregorianDate::Clone(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<ImmutableGregorianDate> date = new ImmutableGregorianDate();
    date->mDate = mDate;
    BaseCalendar::Date::CloneImpl(date.Get());
    *obj = date->Probe(iid);
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

ECode ImmutableGregorianDate::ToString(
    /* [out] */ String* desc)
{
    return mDate->ToString(desc);
}

ECode ImmutableGregorianDate::SetDayOfWeek(
    /* [in] */ Integer dayOfWeek)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ImmutableGregorianDate::SetNormalized(
    /* [in] */ Boolean normalized)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ImmutableGregorianDate::GetZoneOffset(
    /* [out] */ Integer* offset)
{
    return mDate->GetZoneOffset(offset);
}

ECode ImmutableGregorianDate::SetZoneOffset(
    /* [in] */ Integer offset)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ImmutableGregorianDate::GetDaylightSaving(
        /* [out] */ Integer* ds)
{
    return mDate->GetDaylightSaving(ds);
}

ECode ImmutableGregorianDate::SetDaylightSaving(
    /* [in] */ Integer daylightSaving)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ImmutableGregorianDate::GetNormalizedYear(
    /* [out] */ Integer* normalizedYear)
{
    return mDate->GetNormalizedYear(normalizedYear);
}

ECode ImmutableGregorianDate::SetNormalizedYear(
    /* [in] */ Integer normalizedYear)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

}
}
}
