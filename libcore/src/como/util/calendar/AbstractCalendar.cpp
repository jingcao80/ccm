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

#include "como/core/System.h"
#include "como/util/calendar/AbstractCalendar.h"
#include "como/util/calendar/CalendarUtils.h"
#include <ccmlogger.h>

using como::core::System;

namespace como {
namespace util {
namespace calendar {

ECode AbstractCalendar::Constructor()
{
    return NOERROR;
}

ECode AbstractCalendar::GetEra(
    /* [in] */ const String& eraName,
    /* [out] */ IEra** era)
{
    VALIDATE_NOT_NULL(era);

    if (!mEras.IsNull()) {
        for (Integer i = 0; i < mEras.GetLength(); i++) {
            String name;
            if (mEras[i]->GetName(&name), name.Equals(eraName)) {
                *era = mEras[i];
                REFCOUNT_ADD(*era);
                return NOERROR;
            }
        }
    }
    *era = nullptr;
    return NOERROR;
}

ECode AbstractCalendar::GetEras(
    /* [out, callee] */ Array<IEra*>* eras)
{
    VALIDATE_NOT_NULL(eras);

    if (!mEras.IsNull()) {
        *eras = Array<IEra*>(mEras.GetLength());
        eras->Copy(mEras, mEras.GetLength());
        return NOERROR;
    }
    else {
        *eras = Array<IEra*>::Null();
        return NOERROR;
    }
}

ECode AbstractCalendar::SetEra(
    /* [in] */ ICalendarDate* date,
    /* [in] */ const String& eraName)
{
    if (mEras.IsNull()) {
        return NOERROR;
    }
    for (Integer i = 0; i < mEras.GetLength(); i++) {
        IEra* e = mEras[i];
        String name;
        if (e != nullptr && (e->GetName(&name), name.Equals(eraName))) {
            date->SetEra(e);
            return NOERROR;
        }
    }
    Logger::E("AbstractCalendar", "unknown era name: %s", eraName.string());
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

void AbstractCalendar::SetEras(
    /* [in] */ const Array<IEra*>& eras)
{
    mEras = eras;
}

ECode AbstractCalendar::GetCalendarDate(
    /* [out] */ ICalendarDate** date)
{
    VALIDATE_NOT_NULL(date);

    FAIL_RETURN(NewCalendarDate(date));
    return GetCalendarDate(System::GetCurrentTimeMillis(), *date);
}

ECode AbstractCalendar::GetCalendarDate(
    /* [in] */ Long millis,
    /* [out] */ ICalendarDate** date)
{
    VALIDATE_NOT_NULL(date);

    FAIL_RETURN(NewCalendarDate(date));
    return GetCalendarDate(millis, *date);
}

ECode AbstractCalendar::GetCalendarDate(
    /* [in] */ Long millis,
    /* [in] */ ITimeZone* zone,
    /* [out] */ ICalendarDate** date)
{
    VALIDATE_NOT_NULL(date);

    FAIL_RETURN(NewCalendarDate(zone, date));
    return GetCalendarDate(millis, *date);
}

ECode AbstractCalendar::GetCalendarDate(
    /* [in] */ Long millis,
    /* [in] */ ICalendarDate* date)
{
    Integer ms = 0;             // time of day
    Integer zoneOffset = 0;
    Integer saving = 0;
    Long days = 0;              // fixed date

    // adjust to local time if `date' has time zone.
    AutoPtr<ITimeZone> zi;
    date->GetZone(&zi);
    if (zi != nullptr) {
        Array<Integer> offsets(2);
        zi->GetOffset(millis, &zoneOffset);
        zi->GetRawOffset(&offsets[0]);
        offsets[1] = zoneOffset - offsets[0];

        // We need to calculate the given millis and time zone
        // offset separately for como::util::GregorianCalendar
        // compatibility. (i.e., millis + zoneOffset could cause
        // overflow or underflow, which must be avoided.) Usually
        // days should be 0 and ms is in the range of -13:00 to
        // +14:00. However, we need to deal with extreme cases.
        days = zoneOffset / DAY_IN_MILLIS;
        ms = zoneOffset % DAY_IN_MILLIS;
        saving = offsets[1];
    }
    date->SetZoneOffset(zoneOffset);
    date->SetDaylightSaving(saving);

    days += millis / DAY_IN_MILLIS;
    ms += (Integer) (millis % DAY_IN_MILLIS);
    if (ms >= DAY_IN_MILLIS) {
        // at most ms is (DAY_IN_MILLIS - 1) * 2.
        ms -= DAY_IN_MILLIS;
        ++days;
    }
    else {
        // at most ms is (1 - DAY_IN_MILLIS) * 2. Adding one
        // DAY_IN_MILLIS results in still negative.
        while (ms < 0) {
            ms += DAY_IN_MILLIS;
            --days;
        }
    }

    // convert to fixed date (offset from Jan. 1, 1 (Gregorian))
    days += EPOCH_OFFSET;

    // calculate date fields from the fixed date
    GetCalendarDateFromFixedDate(date, days);

    // calculate time fields from the time of day
    SetTimeOfDay(date, ms);
    date->SetLeapYear(IsLeapYear(date));
    date->SetNormalized(true);
    return NOERROR;
}

ECode AbstractCalendar::GetTime(
    /* [in] */ ICalendarDate* date,
    /* [out] */ Long* time)
{
    Long gd;
    GetFixedDate(date, &gd);
    Long ms = (gd - EPOCH_OFFSET) * DAY_IN_MILLIS + GetTimeOfDay(date);
    Integer zoneOffset = 0;
    AutoPtr<ITimeZone> zi;
    date->GetZone(&zi);
    if (zi != nullptr) {
        Boolean result;
        if (date->IsNormalized(&result), result) {
            date->GetZoneOffset(&zoneOffset);
            if (time != nullptr) *time = ms - zoneOffset;
            return NOERROR;
        }
        // adjust time zone and daylight saving
        Array<Integer> offsets(2);
        if (date->IsStandardTime(&result), result) {
            // 1) 2:30am during starting-DST transition is
            //    intrepreted as 2:30am ST
            // 2) 5:00pm during DST is still interpreted as 5:00pm ST
            // 3) 1:30am during ending-DST transition is interpreted
            //    as 1:30am ST (after transition)
            zi->GetRawOffset(&zoneOffset);
            zi->GetOffset(ms - zoneOffset, &zoneOffset);
        }
        else {
            // 1) 2:30am during starting-DST transition is
            //    intrepreted as 3:30am DT
            // 2) 5:00pm during DST is intrepreted as 5:00pm DT
            // 3) 1:30am during ending-DST transition is interpreted
            //    as 1:30am DT/0:30am ST (before transition)
            zi->GetRawOffset(&zoneOffset);
            zi->GetOffset(ms - zoneOffset, &zoneOffset);
        }
    }
    ms -= zoneOffset;
    GetCalendarDate(ms, date);
    if (time != nullptr) *time = ms;
    return NOERROR;
}

Long AbstractCalendar::GetTimeOfDay(
    /* [in] */ ICalendarDate* date)
{
    Long fraction;
    date->GetTimeOfDay(&fraction);
    if (fraction != ICalendarDate::TIME_UNDEFINED) {
        return fraction;
    }
    fraction = GetTimeOfDayValue(date);
    date->SetTimeOfDay(fraction);
    return fraction;
}

Long AbstractCalendar::GetTimeOfDayValue(
    /* [in] */ ICalendarDate* date)
{
    Long fraction;
    Integer v;
    date->GetHours(&v);
    fraction = v * 60;
    date->GetMinutes(&v);
    fraction = (fraction + v) * 60;
    date->GetSeconds(&v);
    fraction = (fraction + v) * 1000;
    date->GetMillis(&v);
    fraction += v;
    return fraction;
}

ECode AbstractCalendar::SetTimeOfDay(
    /* [in] */ ICalendarDate* date,
    /* [in] */ Integer timeOfDay)
{
    if (timeOfDay < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Boolean normalizedState;
    date->IsNormalized(&normalizedState);
    Integer time = timeOfDay;
    Integer hours = time / HOUR_IN_MILLIS;
    time %= HOUR_IN_MILLIS;
    Integer minutes = time / MINUTE_IN_MILLIS;
    time %= MINUTE_IN_MILLIS;
    Integer seconds = time / SECOND_IN_MILLIS;
    time %= SECOND_IN_MILLIS;
    date->SetHours(hours);
    date->SetMinutes(minutes);
    date->SetSeconds(seconds);
    date->SetMillis(time);
    date->SetTimeOfDay(timeOfDay);
    if (hours < 24 && normalizedState) {
        // If this time of day setting doesn't affect the date,
        // then restore the normalized state.
        date->SetNormalized(normalizedState);
    }
    return NOERROR;
}

ECode AbstractCalendar::GetWeekLength(
    /* [out] */ Integer* weeks)
{
    VALIDATE_NOT_NULL(weeks);

    *weeks = 7;
    return NOERROR;
}

ECode AbstractCalendar::GetNthDayOfWeek(
    /* [in] */ Integer nth,
    /* [in] */ Integer dayOfWeek,
    /* [in] */ ICalendarDate* inDate,
    /* [out] */ ICalendarDate** outDate)
{
    VALIDATE_NOT_NULL(outDate);

    AutoPtr<ICalendarDate> ndate;
    ICloneable::Probe(inDate)->Clone(IID_ICalendarDate, (IInterface**)&ndate);
    Normalize(ndate);
    Long fd;
    GetFixedDate(ndate, &fd);
    Long nfd;
    if (nth > 0) {
        nfd = 7 * nth + GetDayOfWeekDateBefore(fd, dayOfWeek);
    }
    else {
        nfd = 7 * nth + GetDayOfWeekDateAfter(fd, dayOfWeek);
    }
    GetCalendarDateFromFixedDate(ndate, nfd);
    ndate.MoveTo(outDate);
    return NOERROR;
}

Long AbstractCalendar::GetDayOfWeekDateBefore(
    /* [in] */ Long fixedDate,
    /* [in] */ Integer dayOfWeek)
{
    return GetDayOfWeekDateOnOrBefore(fixedDate - 1, dayOfWeek);
}

Long AbstractCalendar::GetDayOfWeekDateAfter(
    /* [in] */ Long fixedDate,
    /* [in] */ Integer dayOfWeek)
{
    return GetDayOfWeekDateOnOrBefore(fixedDate + 7, dayOfWeek);
}

Long AbstractCalendar::GetDayOfWeekDateOnOrBefore(
    /* [in] */ Long fixedDate,
    /* [in] */ Integer dayOfWeek)
{
    Long fd = fixedDate - (dayOfWeek - 1);
    if (fd >= 0) {
        return fixedDate - (fd % 7);
    }
    return fixedDate - CalendarUtils::Mod(fd, (Long)7);
}

Boolean AbstractCalendar::ValidateTime(
    /* [in] */ ICalendarDate* date)
{
    Integer t;
    date->GetHours(&t);
    if (t < 0 || t >= 24) {
        return false;
    }
    date->GetMinutes(&t);
    if (t < 0 || t >= 60) {
        return false;
    }
    date->GetSeconds(&t);
    // TODO: Leap second support.
    if (t < 0 || t >= 60) {
        return false;
    }
    date->GetMillis(&t);
    if (t < 0 || t >= 1000) {
        return false;
    }
    return true;
}

Integer AbstractCalendar::NormalizeTime(
    /* [in] */ ICalendarDate* date)
{
    Long fraction = GetTimeOfDay(date);
    Long days = 0;

    if (fraction >= DAY_IN_MILLIS) {
        days = fraction / DAY_IN_MILLIS;
        fraction %= DAY_IN_MILLIS;
    }
    else if (fraction < 0) {
        days = CalendarUtils::FloorDivide(fraction, (Long)DAY_IN_MILLIS);
        if (days != 0) {
            fraction -= DAY_IN_MILLIS * days; // mod(fraction, DAY_IN_MILLIS)
        }
    }
    if (days != 0) {
        date->SetTimeOfDay(fraction);
    }
    date->SetMillis((Integer)(fraction % 1000));
    fraction /= 1000;
    date->SetSeconds((Integer)(fraction % 60));
    fraction /= 60;
    date->SetMinutes((Integer)(fraction % 60));
    date->SetHours((Integer)(fraction / 60));
    return days;
}

}
}
}
