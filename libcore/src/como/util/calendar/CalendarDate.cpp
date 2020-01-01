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

#include "como/core/CStringBuilder.h"
#include "como/util/TimeZone.h"
#include "como/util/calendar/CalendarDate.h"
#include "como/util/calendar/CalendarUtils.h"
#include "como.core.IStringBuilder.h"

using como::core::CStringBuilder;
using como::core::IStringBuilder;
using como::core::IID_ICloneable;
using como::core::IID_IStringBuilder;

namespace como {
namespace util {
namespace calendar {

COMO_INTERFACE_IMPL_2(CalendarDate, SyncObject, ICalendarDate, ICloneable);

ECode CalendarDate::Constructor()
{
    AutoPtr<ITimeZone> zone = TimeZone::GetDefault();
    return Constructor(zone);
}

ECode CalendarDate::Constructor(
    /* [in] */ ITimeZone* zone)
{
    mZoneinfo = zone;
    return NOERROR;
}

ECode CalendarDate::GetEra(
    /* [out] */ IEra** era)
{
    VALIDATE_NOT_NULL(era);

    *era = mEra;
    REFCOUNT_ADD(*era);
    return NOERROR;
}

ECode CalendarDate::SetEra(
    /* [in] */ IEra* era)
{
    if (mEra == era) {
        return NOERROR;
    }
    mEra = era;
    mNormalized = false;
    return NOERROR;
}

ECode CalendarDate::GetYear(
    /* [out] */ Integer* year)
{
    VALIDATE_NOT_NULL(year);

    *year = mYear;
    return NOERROR;
}

ECode CalendarDate::SetYear(
    /* [in] */ Integer year)
{
    if (mYear != year) {
        mYear = year;
        mNormalized = false;
    }
    return NOERROR;
}

ECode CalendarDate::AddYear(
    /* [in] */ Integer n)
{
    if (n != 0) {
        mYear += n;
        mNormalized = false;
    }
    return NOERROR;
}

ECode CalendarDate::IsLeapYear(
    /* [out] */ Boolean* leapYear)
{
    VALIDATE_NOT_NULL(leapYear);

    *leapYear = mLeapYear;
    return NOERROR;
}

ECode CalendarDate::SetLeapYear(
    /* [in] */ Boolean leapYear)
{
    mLeapYear = leapYear;
    return NOERROR;
}

ECode CalendarDate::GetMonth(
    /* [out] */ Integer* month)
{
    VALIDATE_NOT_NULL(month);

    *month = mMonth;
    return NOERROR;
}

ECode CalendarDate::SetMonth(
    /* [in] */ Integer month)
{
    if (mMonth != month) {
        mMonth = month;
        mNormalized = false;
    }
    return NOERROR;
}

ECode CalendarDate::AddMonth(
    /* [in] */ Integer n)
{
    if (n != 0) {
        mMonth += n;
        mNormalized = false;
    }
    return NOERROR;
}

ECode CalendarDate::GetDayOfMonth(
    /* [out] */ Integer* date)
{
    VALIDATE_NOT_NULL(date);

    *date = mDayOfMonth;
    return NOERROR;
}

ECode CalendarDate::SetDayOfMonth(
    /* [in] */ Integer date)
{
    if (mDayOfMonth != date) {
        mDayOfMonth = date;
        mNormalized = false;
    }
    return NOERROR;
}

ECode CalendarDate::AddDayOfMonth(
    /* [in] */ Integer n)
{
    if (n != 0) {
        mDayOfMonth += n;
        mNormalized = false;
    }
    return NOERROR;
}

ECode CalendarDate::GetDayOfWeek(
    /* [out] */ Integer* date)
{
    VALIDATE_NOT_NULL(date);

    Boolean normalize;
    if (IsNormalized(&normalize), !normalize) {
        mDayOfWeek = FIELD_UNDEFINED;
    }
    *date = mDayOfWeek;
    return NOERROR;
}

ECode CalendarDate::GetHours(
    /* [out] */ Integer* hours)
{
    VALIDATE_NOT_NULL(hours);

    *hours = mHours;
    return NOERROR;
}

ECode CalendarDate::SetHours(
    /* [in] */ Integer hours)
{
    if (mHours != hours) {
        mHours = hours;
        mNormalized = false;
    }
    return NOERROR;
}

ECode CalendarDate::AddHours(
    /* [in] */ Integer n)
{
    if (n != 0) {
        mHours += n;
        mNormalized = false;
    }
    return NOERROR;
}

ECode CalendarDate::GetMinutes(
    /* [out] */ Integer* minutes)
{
    VALIDATE_NOT_NULL(minutes);

    *minutes = mMinutes;
    return NOERROR;
}

ECode CalendarDate::SetMinutes(
    /* [in] */ Integer minutes)
{
    if (mMinutes != minutes) {
        mMinutes = minutes;
        mNormalized = false;
    }
    return NOERROR;
}

ECode CalendarDate::AddMinutes(
    /* [in] */ Integer n)
{
    if (n != 0) {
        mMinutes += n;
        mNormalized = false;
    }
    return NOERROR;
}

ECode CalendarDate::GetSeconds(
    /* [out] */ Integer* seconds)
{
    VALIDATE_NOT_NULL(seconds);

    *seconds = mSeconds;
    return NOERROR;
}

ECode CalendarDate::SetSeconds(
    /* [in] */ Integer seconds)
{
    if (mSeconds != seconds) {
        mSeconds = seconds;
        mNormalized = false;
    }
    return NOERROR;
}

ECode CalendarDate::AddSeconds(
    /* [in] */ Integer n)
{
    if (n != 0) {
        mSeconds += n;
        mNormalized = false;
    }
    return NOERROR;
}

ECode CalendarDate::GetMillis(
    /* [out] */ Integer* millis)
{
    VALIDATE_NOT_NULL(millis);

    *millis = mMillis;
    return NOERROR;
}

ECode CalendarDate::SetMillis(
    /* [in] */ Integer millis)
{
    if (mMillis != millis) {
        mMillis = millis;
        mNormalized = false;
    }
    return NOERROR;
}

ECode CalendarDate::AddMillis(
    /* [in] */ Integer n)
{
    if (n != 0) {
        mMillis += n;
        mNormalized = false;
    }
    return NOERROR;
}

ECode CalendarDate::GetTimeOfDay(
    /* [out] */ Long* date)
{
    VALIDATE_NOT_NULL(date);

    Boolean normalize;
    if (IsNormalized(&normalize), !normalize) {
        mFraction = TIME_UNDEFINED;
    }
    *date = mFraction;
    return NOERROR;
}

ECode CalendarDate::SetDate(
    /* [in] */ Integer year,
    /* [in] */ Integer month,
    /* [in] */ Integer dayOfMonth)
{
    SetYear(year);
    SetMonth(month);
    SetDayOfMonth(dayOfMonth);
    return NOERROR;
}

ECode CalendarDate::AddDate(
    /* [in] */ Integer year,
    /* [in] */ Integer month,
    /* [in] */ Integer dayOfMonth)
{
    AddYear(year);
    AddMonth(month);
    AddDayOfMonth(dayOfMonth);
    return NOERROR;
}

ECode CalendarDate::SetTimeOfDay(
    /* [in] */ Integer hours,
    /* [in] */ Integer minutes,
    /* [in] */ Integer seconds,
    /* [in] */ Integer millis)
{
    SetHours(hours);
    SetMinutes(minutes);
    SetSeconds(seconds);
    SetMillis(millis);
    return NOERROR;
}

ECode CalendarDate::AddTimeOfDay(
    /* [in] */ Integer hours,
    /* [in] */ Integer minutes,
    /* [in] */ Integer seconds,
    /* [in] */ Integer millis)
{
    AddHours(hours);
    AddMinutes(minutes);
    AddSeconds(seconds);
    AddMillis(millis);
    return NOERROR;
}

ECode CalendarDate::SetTimeOfDay(
    /* [in] */ Long fraction)
{
    mFraction = fraction;
    return NOERROR;
}

ECode CalendarDate::IsNormalized(
    /* [out] */ Boolean* normalized)
{
    VALIDATE_NOT_NULL(normalized);

    *normalized = mNormalized;
    return NOERROR;
}

ECode CalendarDate::IsStandardTime(
    /* [out] */ Boolean* standardTime)
{
    VALIDATE_NOT_NULL(standardTime);

    *standardTime = mForceStandardTime;
    return NOERROR;
}

ECode CalendarDate::SetStandardTime(
    /* [in] */ Boolean standardTime)
{
    mForceStandardTime = standardTime;
    return NOERROR;
}

ECode CalendarDate::IsDaylightTime(
    /* [out] */ Boolean* daylightTime)
{
    VALIDATE_NOT_NULL(daylightTime);

    Boolean standardTime;
    if (IsStandardTime(&standardTime), standardTime) {
        *daylightTime = false;
        return NOERROR;
    }
    *daylightTime = mDaylightSaving != 0;
    return NOERROR;
}

ECode CalendarDate::SetLocale(
    /* [in] */ ILocale* loc)
{
    mLocale = loc;
    return NOERROR;
}

ECode CalendarDate::GetZone(
    /* [out] */ ITimeZone** zone)
{
    VALIDATE_NOT_NULL(zone);

    *zone = mZoneinfo;
    REFCOUNT_ADD(*zone);
    return NOERROR;
}

ECode CalendarDate::SetZone(
    /* [in] */ ITimeZone* zoneinfo)
{
    mZoneinfo = zoneinfo;
    return NOERROR;
}

ECode CalendarDate::IsSameDate(
    /* [in] */ ICalendarDate* date,
    /* [out] */ Boolean* same)
{
    VALIDATE_NOT_NULL(same);

    Integer thisDate, othDate;
    AutoPtr<IEra> thisEra, othEra;
    *same = (GetDayOfWeek(&thisDate), date->GetDayOfWeek(&othDate), thisDate == othDate) &&
            (GetMonth(&thisDate), date->GetMonth(&othDate), thisDate == othDate) &&
            (GetYear(&thisDate), date->GetYear(&othDate), thisDate == othDate) &&
            (GetEra(&thisEra), date->GetEra(&othEra), thisEra == othEra);
    return NOERROR;
}

ECode CalendarDate::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* same)
{
    VALIDATE_NOT_NULL(same);

    if (ICalendarDate::Probe(obj) == nullptr) {
        *same = false;
        return NOERROR;
    }
    CalendarDate* that = (CalendarDate*)ICalendarDate::Probe(obj);
    Boolean thisNorm, othNorm;
    if (IsNormalized(&thisNorm), that->IsNormalized(&othNorm), thisNorm != othNorm) {
        *same = false;
        return NOERROR;
    }
    Boolean hasZone = mZoneinfo != nullptr;
    Boolean thatHasZone = that->mZoneinfo != nullptr;
    if (hasZone != thatHasZone) {
        *same = false;
        return NOERROR;
    }
    if (hasZone && !Object::Equals(mZoneinfo, that->mZoneinfo)) {
        *same = false;
        return NOERROR;
    }
    AutoPtr<IEra> thisEra, othEra;
    *same = (GetEra(&thisEra), that->GetEra(&othEra), thisEra == othEra) &&
            mYear == that->mYear &&
            mMonth == that->mMonth &&
            mDayOfMonth == that->mDayOfMonth &&
            mHours == that->mHours &&
            mMinutes == that->mMinutes &&
            mSeconds == that->mSeconds &&
            mMillis == that->mMillis &&
            mZoneOffset == that->mZoneOffset;
    return NOERROR;
}

ECode CalendarDate::GetHashCode(
    /* [out] */ Integer* hash)
{
    VALIDATE_NOT_NULL(hash);

    // a pseudo (local standard) time stamp value in milliseconds
    // from the Epoch, assuming Gregorian calendar fields.
    Long h = ((((((Long)mYear - 1970) * 12) + (mMonth - 1)) * 30) + mDayOfMonth) * 24;
    h = ((((((h + mHours) * 60) + mMinutes) * 60) + mSeconds) * 1000) + mMillis;
    h -= mZoneOffset;
    Boolean norm;
    Integer normalized = (IsNormalized(&norm), norm) ? 1 : 0;
    Integer era = 0;
    AutoPtr<IEra> e;
    GetEra(&e);
    if (e != nullptr) {
        era = Object::GetHashCode(e);
    }
    Integer zone = mZoneinfo != nullptr ? Object::GetHashCode(mZoneinfo) : 0;
    *hash = (Integer) h * (Integer)(h >> 32) ^ era ^ normalized ^ zone;
    return NOERROR;
}

ECode CalendarDate::CloneImpl(
    /* [out] */ ICalendarDate* newObj)
{
    CalendarDate* that = (CalendarDate*)newObj;
    that->mEra = mEra;
    that->mYear = mYear;
    that->mMonth = mMonth;
    that->mDayOfMonth = mDayOfMonth;
    that->mDayOfWeek = mDayOfWeek;
    that->mLeapYear = mLeapYear;
    that->mHours = mHours;
    that->mMinutes = mMinutes;
    that->mSeconds = mSeconds;
    that->mMillis = mMillis;
    that->mFraction = mFraction;
    that->mNormalized = mNormalized;
    that->mZoneinfo = mZoneinfo;
    that->mZoneOffset = mZoneOffset;
    that->mDaylightSaving = mDaylightSaving;
    that->mForceStandardTime = mForceStandardTime;
    that->mLocale = mLocale;
    return NOERROR;
}

ECode CalendarDate::ToString(
    /* [out] */ String* desc)
{
    VALIDATE_NOT_NULL(desc);

    AutoPtr<IStringBuilder> sb;
    CStringBuilder::New(IID_IStringBuilder, (IInterface**)&sb);
    CalendarUtils::Sprintf0d(sb, mYear, 4);
    sb->Append(U'-');
    CalendarUtils::Sprintf0d(sb, mMonth, 2);
    sb->Append(U'-');
    CalendarUtils::Sprintf0d(sb, mDayOfMonth, 2);
    sb->Append(U'T');
    CalendarUtils::Sprintf0d(sb, mHours, 2);
    sb->Append(U':');
    CalendarUtils::Sprintf0d(sb, mMinutes, 2);
    sb->Append(U':');
    CalendarUtils::Sprintf0d(sb, mSeconds, 2);
    sb->Append(U'.');
    CalendarUtils::Sprintf0d(sb, mMillis, 3);
    if (mZoneOffset == 0) {
        sb->Append(U'Z');
    }
    else if (mZoneOffset != FIELD_UNDEFINED) {
        Integer offset;
        Char sign;
        if (mZoneOffset > 0) {
            offset = mZoneOffset;
            sign = U'+';
        }
        else {
            offset = -mZoneOffset;
            sign = U'-';
        }
        offset /= 60000;
        sb->Append(sign);
        CalendarUtils::Sprintf0d(sb, offset / 60, 2);
        CalendarUtils::Sprintf0d(sb, offset % 60, 2);
    }
    else {
        sb->Append(String(" local time"));
    }
    return sb->ToString(desc);
}

ECode CalendarDate::SetDayOfWeek(
    /* [in] */ Integer dayOfWeek)
{
    mDayOfWeek = dayOfWeek;
    return NOERROR;
}

ECode CalendarDate::SetNormalized(
    /* [in] */ Boolean normalized)
{
    mNormalized = normalized;
    return NOERROR;
}

ECode CalendarDate::GetZoneOffset(
    /* [out] */ Integer* offset)
{
    VALIDATE_NOT_NULL(offset);

    *offset = mZoneOffset;
    return NOERROR;
}

ECode CalendarDate::SetZoneOffset(
    /* [in] */ Integer offset)
{
    mZoneOffset = offset;
    return NOERROR;
}

ECode CalendarDate::GetDaylightSaving(
    /* [out] */ Integer* ds)
{
    VALIDATE_NOT_NULL(ds);

    *ds = mDaylightSaving;
    return NOERROR;
}

ECode CalendarDate::SetDaylightSaving(
    /* [in] */ Integer daylightSaving)
{
    mDaylightSaving = daylightSaving;
    return NOERROR;
}

}
}
}
