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

#include "como/util/calendar/BaseCalendar.h"
#include "como/util/calendar/CalendarUtils.h"
#include <comolog.h>

namespace como {
namespace util {
namespace calendar {

constexpr Integer BaseCalendar::FIXED_DATES[];
constexpr Integer BaseCalendar::DAYS_IN_MONTH[];
constexpr Integer BaseCalendar::ACCUMULATED_DAYS_IN_MONTH[];
constexpr Integer BaseCalendar::ACCUMULATED_DAYS_IN_MONTH_LEAP[];

COMO_INTERFACE_IMPL_1(BaseCalendar, AbstractCalendar, IBaseCalendar);

ECode BaseCalendar::Validate(
    /* [in] */ ICalendarDate* date,
    /* [out] */ Boolean& result)
{
    Date* bdate = (Date*)IBaseCalendarDate::Probe(date);
    Boolean normalized;
    if (bdate->IsNormalized(normalized), normalized) {
        result = true;
        return NOERROR;
    }
    Integer month;
    bdate->GetMonth(month);
    if (month < JANUARY || month > DECEMBER) {
        result = false;
        return NOERROR;
    }
    Integer d;
    if (bdate->GetDayOfMonth(d), d <= 0) {
        result = false;
        return NOERROR;
    }
    Integer normalizedYear;
    bdate->GetNormalizedYear(normalizedYear);
    if (d > GetMonthLength(normalizedYear, month)) {
        result = false;
        return NOERROR;
    }
    Integer bdow, dow;
    bdate->GetDayOfWeek(bdow);
    if (bdow != Date::FIELD_UNDEFINED &&
            (GetDayOfWeek(bdate, dow), bdow != dow)) {
        result = false;
        return NOERROR;
    }

    if (!ValidateTime(date)) {
        result = false;
        return NOERROR;
    }

    bdate->SetNormalized(true);
    result = true;
    return NOERROR;
}

ECode BaseCalendar::Normalize(
    /* [in] */ ICalendarDate* date,
    /* [out] */ Boolean* result)
{
    Boolean normalized;
    if (date->IsNormalized(normalized), normalized) {
        if (result != nullptr) *result = true;
        return NOERROR;
    }

    Date* bdate = (Date*)IBaseCalendarDate::Probe(date);
    AutoPtr<ITimeZone> zi;
    bdate->GetZone(zi);

    // If the date has a time zone, then we need to recalculate
    // the calendar fields. Let getTime() do it.
    if (zi != nullptr) {
        GetTime(date);
        if (result != nullptr) *result = true;
        return NOERROR;
    }

    Integer days = NormalizeTime(bdate);
    NormalizeMonth(bdate);
    Integer dom;
    bdate->GetDayOfMonth(dom);
    Long d = dom + days;
    Integer m, y;
    bdate->GetMonth(m);
    bdate->GetNormalizedYear(y);
    Integer ml = GetMonthLength(y, m);

    if (!(d > 0 && d <= ml)) {
        if (d <= 0 && d > -28) {
            ml = GetMonthLength(y, --m);
            d += ml;
            bdate->SetDayOfMonth((Integer)d);
            if (m == 0) {
                m = DECEMBER;
                bdate->SetNormalizedYear(y - 1);
            }
            bdate->SetMonth(m);
        }
        else if (d > ml && d < (ml + 28)) {
            d -= ml;
            ++m;
            bdate->SetDayOfMonth((Integer)d);
            if (m > DECEMBER) {
                bdate->SetNormalizedYear(y + 1);
                m = JANUARY;
            }
            bdate->SetMonth(m);
        }
        else {
            Long fixedDate;
            GetFixedDate(y, m, 1, bdate, fixedDate);
            fixedDate = d + fixedDate - 1ll;
            GetCalendarDateFromFixedDate(bdate, fixedDate);
        }
    }
    else {
        Integer dow;
        GetDayOfWeek(bdate, dow);
        bdate->SetDayOfWeek(dow);
    }
    bdate->GetNormalizedYear(y);
    Boolean isLeap;
    IsLeapYear(y, isLeap);
    date->SetLeapYear(isLeap);
    date->SetZoneOffset(0);
    date->SetDaylightSaving(0);
    date->SetNormalized(true);
    if (result != nullptr) *result = true;
    return NOERROR;
}

void BaseCalendar::NormalizeMonth(
    /* [in] */ ICalendarDate* date)
{
    Date* bdate = (Date*)IBaseCalendarDate::Probe(date);
    Integer year;
    bdate->GetNormalizedYear(year);
    Integer month;
    bdate->GetMonth(month);
    if (month <= 0) {
        Long xm = 1ll - month;
        year -= (Integer)((xm / 12) + 1);
        month = 13 - (Integer)(xm % 12);
        bdate->SetNormalizedYear(year);
        bdate->SetMonth(month);
    }
    else if (month > DECEMBER) {
        year += ((month - 1) / 12);
        month = (month - 1) % 12 + 1;
        bdate->SetNormalizedYear(year);
        bdate->SetMonth(month);
    }
}

ECode BaseCalendar::GetYearLength(
    /* [in] */ ICalendarDate* date,
    /* [out] */ Integer& days)
{
    Date* bdate = (Date*)IBaseCalendarDate::Probe(date);
    Integer year;
    bdate->GetNormalizedYear(year);
    Boolean isLeap;
    days = (IsLeapYear(year, isLeap), isLeap ? 366 : 365);
    return NOERROR;
}

ECode BaseCalendar::GetYearLengthInMonths(
    /* [in] */ ICalendarDate* date,
    /* [out] */ Integer& months)
{
    months = 12;
    return NOERROR;
}

ECode BaseCalendar::GetMonthLength(
    /* [in] */ ICalendarDate* date,
    /* [out] */ Integer& days)
{
    Date* bdate = (Date*)IBaseCalendarDate::Probe(date);
    Integer month;
    bdate->GetMonth(month);
    if (month < JANUARY || month > DECEMBER) {
        Logger::E("BaseCalendar", "Illegal month value: %d", month);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Integer year;
    bdate->GetNormalizedYear(year);
    days = GetMonthLength(year, month);
    return NOERROR;
}

Integer BaseCalendar::GetMonthLength(
    /* [in] */ Integer year,
    /* [in] */ Integer month)
{
    Integer days = DAYS_IN_MONTH[month];
    Boolean isLeap;
    if (month == FEBRUARY && (IsLeapYear(year, isLeap), isLeap)) {
        days++;
    }
    return days;
}

ECode BaseCalendar::GetDayOfYear(
    /* [in] */ ICalendarDate* date,
    /* [out] */ Long& days)
{
    Date* bdate = (Date*)IBaseCalendarDate::Probe(date);
    Integer y, m, dom;
    bdate->GetNormalizedYear(y);
    bdate->GetMonth(m);
    bdate->GetDayOfMonth(dom);
    days = GetDayOfYear(y, m, dom);
    return NOERROR;
}

Long BaseCalendar::GetDayOfYear(
    /* [in] */ Integer year,
    /* [in] */ Integer month,
    /* [in] */ Integer dayOfMonth)
{
    Boolean isLeap;
    return dayOfMonth + ((IsLeapYear(year, isLeap), isLeap) ?
            ACCUMULATED_DAYS_IN_MONTH_LEAP[month] : ACCUMULATED_DAYS_IN_MONTH[month]);
}

ECode BaseCalendar::GetFixedDate(
    /* [in] */ ICalendarDate* date,
    /* [out] */ Long& fraction)
{
    Boolean normalized;
    if (date->IsNormalized(normalized), !normalized) {
        NormalizeMonth(date);
    }
    Date* bdate = (Date*)IBaseCalendarDate::Probe(date);
    Integer y, m, dom;
    bdate->GetNormalizedYear(y);
    bdate->GetMonth(m);
    bdate->GetDayOfMonth(dom);
    return GetFixedDate(y, m, dom, bdate, fraction);
}

ECode BaseCalendar::GetFixedDate(
    /* [in] */ Integer year,
    /* [in] */ Integer month,
    /* [in] */ Integer dayOfMonth,
    /* [in] */ IBaseCalendarDate* date,
    /* [out] */ Long& fraction)
{
    Boolean isJan1 = month == JANUARY && dayOfMonth == 1;

    Date* cache = (Date*)IBaseCalendarDate::Probe(date);
    // Look up the one year cache
    if (cache != nullptr && cache->Hit(year)) {
        if (isJan1) {
            fraction = cache->GetCachedJan1();
            return NOERROR;
        }
        fraction = cache->GetCachedJan1() + GetDayOfYear(year, month, dayOfMonth) - 1;
        return NOERROR;
    }

    // Look up the pre-calculated fixed date table
    Integer n = year - BASE_YEAR;
    if (n >= 0 && n < ArrayLength(FIXED_DATES)) {
        Long jan1 = FIXED_DATES[n];
        if (cache != nullptr) {
            Boolean isLeap;
            IsLeapYear(year, isLeap);
            cache->SetCache(year, jan1, isLeap ? 366 : 365);
        }
        fraction = isJan1 ? jan1 : jan1 + GetDayOfYear(year, month, dayOfMonth) - 1;
        return NOERROR;
    }

    Long prevyear = (Long)year - 1;
    Long days = dayOfMonth;

    if (prevyear >= 0) {
        days += (365 * prevyear)
               + (prevyear / 4)
               - (prevyear / 100)
               + (prevyear / 400)
               + ((367 * month - 362) / 12);
    }
    else {
        days += (365 * prevyear)
               + CalendarUtils::FloorDivide(prevyear, (Long)4)
               - CalendarUtils::FloorDivide(prevyear, (Long)100)
               + CalendarUtils::FloorDivide(prevyear, (Long)400)
               + CalendarUtils::FloorDivide((367 * month - 362), 12);
    }

    if (month > FEBRUARY) {
        Boolean isLeap;
        days -= (IsLeapYear(year, isLeap), isLeap) ? 1 : 2;
    }

    // If it's January 1, update the cache.
    if (cache != nullptr && isJan1) {
        Boolean isLeap;
        cache->SetCache(year, days, (IsLeapYear(year, isLeap), isLeap) ? 366 : 365);
    }

    fraction = days;
    return NOERROR;
}

ECode BaseCalendar::GetCalendarDateFromFixedDate(
    /* [in] */ ICalendarDate* date,
    /* [in] */ Long fixedDate)
{
    Date* bdate = (Date*)IBaseCalendarDate::Probe(date);
    Integer year;
    Long jan1;
    Boolean isLeap;
    if (bdate->Hit(fixedDate)) {
        year = bdate->GetCachedYear();
        jan1 = bdate->GetCachedJan1();
        IsLeapYear(year, isLeap);
    }
    else {
        // Looking up FIXED_DATES[] here didn't improve performance
        // much. So we calculate year and jan1. getFixedDate()
        // will look up FIXED_DATES[] actually.
        year = GetGregorianYearFromFixedDate(fixedDate);
        GetFixedDate(year, JANUARY, 1, nullptr, jan1);
        IsLeapYear(year, isLeap);
        // Update the cache data
        bdate->SetCache(year, jan1, isLeap ? 366 : 365);
    }

    Integer priorDays = (Integer)(fixedDate - jan1);
    Long mar1 = jan1 + 31 + 28;
    if (isLeap) {
        ++mar1;
    }
    if (fixedDate >= mar1) {
        priorDays += isLeap ? 1 : 2;
    }
    Integer month = 12 * priorDays + 373;
    if (month > 0) {
        month /= 367;
    }
    else {
        month = CalendarUtils::FloorDivide(month, 367);
    }
    Long month1 = jan1 + ACCUMULATED_DAYS_IN_MONTH[month];
    if (isLeap && month >= MARCH) {
        ++month1;
    }
    Integer dayOfMonth = (Integer)(fixedDate - month1) + 1;
    Integer dayOfWeek = GetDayOfWeekFromFixedDate(fixedDate);
    CHECK(dayOfWeek > 0);
    bdate->SetNormalizedYear(year);
    bdate->SetMonth(month);
    bdate->SetDayOfMonth(dayOfMonth);
    bdate->SetDayOfWeek(dayOfWeek);
    bdate->SetLeapYear(isLeap);
    bdate->SetNormalized(true);
    return NOERROR;
}

ECode BaseCalendar::GetDayOfWeek(
    /* [in] */ ICalendarDate* date,
    /* [out] */ Integer& days)
{
    Long fixedDate;
    GetFixedDate(date, fixedDate);
    days = GetDayOfWeekFromFixedDate(fixedDate);
    return NOERROR;
}

Integer BaseCalendar::GetDayOfWeekFromFixedDate(
    /* [in] */ Long fixedDate)
{
    // The fixed day 1 (January 1, 1 Gregorian) is Monday.
    if (fixedDate >= 0) {
        return (Integer)(fixedDate % 7) + SUNDAY;
    }
    return (Integer)CalendarUtils::Mod(fixedDate, (Long)7) + SUNDAY;
}

ECode BaseCalendar::GetYearFromFixedDate(
    /* [in] */ Long fixedDate,
    /* [out] */ Integer& year)
{
    year = GetGregorianYearFromFixedDate(fixedDate);
    return NOERROR;
}

Integer BaseCalendar::GetGregorianYearFromFixedDate(
    /* [in] */ Long fixedDate)
{
    Long d0;
    Integer d1, d2, d3, d4;
    Integer n400, n100, n4, n1;
    Integer year;

    if (fixedDate > 0) {
        d0 = fixedDate - 1;
        n400 = (Integer)(d0 / 146097);
        d1 = (Integer)(d0 % 146097);
        n100 = d1 / 36524;
        d2 = d1 % 36524;
        n4 = d2 / 1461;
        d3 = d2 % 1461;
        n1 = d3 / 365;
        d4 = (d3 % 365) + 1;
    }
    else {
        d0 = fixedDate - 1;
        n400 = (Integer)CalendarUtils::FloorDivide(d0, 146097ll);
        d1 = (Integer)CalendarUtils::Mod(d0, 146097ll);
        n100 = CalendarUtils::FloorDivide(d1, 36524);
        d2 = CalendarUtils::Mod(d1, 36524);
        n4 = CalendarUtils::FloorDivide(d2, 1461);
        d3 = CalendarUtils::Mod(d2, 1461);
        n1 = CalendarUtils::FloorDivide(d3, 365);
        d4 = CalendarUtils::Mod(d3, 365) + 1;
    }
    year = 400 * n400 + 100 * n100 + 4 * n4 + n1;
    if (!(n100 == 4 || n1 == 4)) {
        ++year;
    }
    return year;
}

Boolean BaseCalendar::IsLeapYear(
    /* [in] */ ICalendarDate* date)
{
    Date* bdate = (Date*)IBaseCalendarDate::Probe(date);
    Integer year;
    bdate->GetNormalizedYear(year);
    Boolean leapYear;
    IsLeapYear(year, leapYear);
    return leapYear;
}

ECode BaseCalendar::IsLeapYear(
    /* [in] */ Integer normalizedYear,
    /* [out] */ Boolean& leapYear)
{
    leapYear = CalendarUtils::IsGregorianLeapYear(normalizedYear);
    return NOERROR;
}

//-------------------------------------------------------------------------

COMO_INTERFACE_IMPL_1(BaseCalendar::Date, CalendarDate, IBaseCalendarDate);

ECode BaseCalendar::Date::Constructor()
{
    return CalendarDate::Constructor();
}

ECode BaseCalendar::Date::Constructor(
    /* [in] */ ITimeZone* zone)
{
    return CalendarDate::Constructor(zone);
}

ECode BaseCalendar::Date::SetNormalizedDate(
    /* [in] */ Integer normalizedYear,
    /* [in] */ Integer month,
    /* [in] */ Integer dayOfMonth)
{
    SetNormalizedYear(normalizedYear);
    SetMonth(month);
    SetDayOfMonth(dayOfMonth);
    return NOERROR;
}

Boolean BaseCalendar::Date::Hit(
    /* [in] */ Integer year)
{
    return year == mCachedYear;
}

Boolean BaseCalendar::Date::Hit(
    /* [in] */ Long fixedDate)
{
    return (fixedDate >= mCachedFixedDateJan1 &&
            fixedDate < mCachedFixedDateNextJan1);
}

Integer BaseCalendar::Date::GetCachedYear()
{
    return mCachedYear;
}

Long BaseCalendar::Date::GetCachedJan1()
{
    return mCachedFixedDateJan1;
}

void BaseCalendar::Date::SetCache(
    /* [in] */ Integer year,
    /* [in] */ Long jan1,
    /* [in] */ Integer len)
{
    mCachedYear = year;
    mCachedFixedDateJan1 = jan1;
    mCachedFixedDateNextJan1 = jan1 + len;
}

ECode BaseCalendar::Date::CloneImpl(
    /* [out] */ IBaseCalendarDate* newObj)
{
    BaseCalendar::Date* date = (BaseCalendar::Date*)newObj;
    CalendarDate::CloneImpl(date);
    date->mCachedYear = mCachedYear;
    date->mCachedFixedDateJan1 = mCachedFixedDateJan1;
    date->mCachedFixedDateNextJan1 = mCachedFixedDateNextJan1;
    return NOERROR;
}

}
}
}
