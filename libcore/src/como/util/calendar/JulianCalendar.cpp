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

#include "como/core/CStringBuffer.h"
#include "como/core/System.h"
#include "como/util/calendar/CalendarUtils.h"
#include "como/util/calendar/CEra.h"
#include "como/util/calendar/JulianCalendar.h"
#include "como.core.ILong.h"
#include "como.core.IStringBuffer.h"
#include <comolog.h>

using como::core::CStringBuffer;
using como::core::ILong;
using como::core::IStringBuffer;
using como::core::IID_IStringBuffer;
using como::core::System;

namespace como {
namespace util {
namespace calendar {

COMO_INTERFACE_IMPL_1(JulianCalendar, BaseCalendar, IJulianCalendar);

ECode JulianCalendar::Constructor()
{
    BaseCalendar::Constructor();
    SetEras(GetEras());
    return NOERROR;
}

ECode JulianCalendar::GetName(
    /* [out] */ String& name)
{
    name = "julian";
    return NOERROR;
}

ECode JulianCalendar::GetCalendarDate(
    /* [out] */ AutoPtr<ICalendarDate>& date)
{
    NewCalendarDate(date);
    return GetCalendarDate(System::GetCurrentTimeMillis(), date);
}

ECode JulianCalendar::GetCalendarDate(
    /* [in] */ Long millis,
    /* [out] */ AutoPtr<ICalendarDate>& date)
{
    NewCalendarDate(date);
    return GetCalendarDate(millis, date);
}

ECode JulianCalendar::GetCalendarDate(
    /* [in] */ Long millis,
    /* [in] */ ICalendarDate* date)
{
    return BaseCalendar::GetCalendarDate(millis, date);
}

ECode JulianCalendar::GetCalendarDate(
    /* [in] */ Long millis,
    /* [in] */ ITimeZone* zone,
    /* [out] */ AutoPtr<ICalendarDate>& date)
{
    NewCalendarDate(zone, date);
    return GetCalendarDate(millis, date);
}

ECode JulianCalendar::NewCalendarDate(
    /* [out] */ AutoPtr<ICalendarDate>& date)
{
    AutoPtr<JulianCalendar::Date> d = new JulianCalendar::Date();
    ECode ec = d->Constructor();
    if (FAILED(ec)) return ec;
    date = d.Get();
    return NOERROR;
}

ECode JulianCalendar::NewCalendarDate(
    /* [in] */ ITimeZone* zone,
    /* [out] */ AutoPtr<ICalendarDate>& date)
{
    AutoPtr<JulianCalendar::Date> d = new JulianCalendar::Date();
    ECode ec = d->Constructor(zone);
    if (FAILED(ec)) return ec;
    date = d.Get();
    return NOERROR;
}

ECode JulianCalendar::GetFixedDate(
    /* [in] */ Integer year,
    /* [in] */ Integer month,
    /* [in] */ Integer dayOfMonth,
    /* [in] */ IBaseCalendarDate* date,
    /* [out] */ Long& fraction)
{
    Boolean isJan1 = month == JANUARY && dayOfMonth == 1;

    BaseCalendar::Date* cache = (BaseCalendar::Date*)IBaseCalendarDate::Probe(date);
    // Look up the one year cache
    if (cache != nullptr && cache->Hit(year)) {
        if (isJan1) {
            fraction = cache->GetCachedJan1();
            return NOERROR;
        }

        fraction = cache->GetCachedJan1() + GetDayOfYear(year, month, dayOfMonth) - 1;
        return NOERROR;
    }

    Long y = year;
    Long days = JULIAN_EPOCH - 1 + (365 * (y - 1)) + dayOfMonth;
    if (y > 0) {
        // CE years
        days += (y - 1) / 4;
    }
    else {
        // BCE years
        days += CalendarUtils::FloorDivide(y - 1, (Long)4);
    }
    if (month > 0) {
        days += ((367 * (Long)month) - 362) / 12;
    }
    else {
        days += CalendarUtils::FloorDivide((367 * (Long)month) - 362, (Long)12);
    }
    if (month > FEBRUARY) {
        days -= CalendarUtils::IsJulianLeapYear(year) ? 1 : 2;
    }

    // If it's January 1, update the cache.
    if (cache != nullptr && isJan1) {
        cache->SetCache(year, days, CalendarUtils::IsJulianLeapYear(year) ? 366 : 365);
    }

    fraction = days;
    return NOERROR;
}

ECode JulianCalendar::GetCalendarDateFromFixedDate(
    /* [in] */ ICalendarDate* date,
    /* [in] */ Long fixedDate)
{
    JulianCalendar::Date* jdate = (JulianCalendar::Date*)date;
    Long fd = 4 * (fixedDate - JULIAN_EPOCH) + 1464;
    Integer year;
    if (fd >= 0) {
        year = (Integer)(fd / 1461);
    }
    else {
        year = (Integer)CalendarUtils::FloorDivide(fd, (Long)1461);
    }
    Long fraction;
    GetFixedDate(year, JANUARY, 1, jdate, fraction);
    Integer priorDays = (Integer)(fixedDate - fraction);
    Boolean isLeap = CalendarUtils::IsJulianLeapYear(year);
    if (GetFixedDate(year, MARCH, 1, jdate, fraction), fixedDate >= fraction) {
        priorDays += isLeap ? 1 : 2;
    }
    Integer month = 12 * priorDays + 373;
    if (month > 0) {
        month /= 367;
    }
    else {
        month = CalendarUtils::FloorDivide(month, 367);
    }
    GetFixedDate(year, month, 1, jdate, fraction);
    Integer dayOfMonth = (Integer)(fixedDate - fraction) + 1;
    Integer dayOfWeek = GetDayOfWeekFromFixedDate(fixedDate);
    CHECK(dayOfWeek > 0);
    jdate->SetNormalizedYear(year);
    jdate->SetMonth(month);
    jdate->SetDayOfMonth(dayOfMonth);
    jdate->SetDayOfWeek(dayOfWeek);
    jdate->SetLeapYear(isLeap);
    jdate->SetNormalized(true);
    return NOERROR;
}

ECode JulianCalendar::GetYearFromFixedDate(
    /* [in] */ Long fixedDate,
    /* [out] */ Integer& year)
{
    Integer y = (Integer)CalendarUtils::FloorDivide(4 * (fixedDate - JULIAN_EPOCH) + 1464, (Long)1461);
    year = y;
    return NOERROR;
}

ECode JulianCalendar::GetDayOfWeek(
    /* [in] */ ICalendarDate* date,
    /* [out] */ Integer& days)
{
    Long fixedDate;
    GetFixedDate(date, fixedDate);
    days = GetDayOfWeekFromFixedDate(fixedDate);
    return NOERROR;
}

ECode JulianCalendar::IsLeapYear(
    /* [in] */ Integer jyear,
    /* [out] */ Boolean& leapYear)
{
    leapYear = CalendarUtils::IsJulianLeapYear(jyear);
    return NOERROR;
}

static const Array<IEra*> CreateEras()
{
    AutoPtr<IEra> bce, ce;
    CEra::New(String("BeforeCommonEra"), String("B.C.E."), ILong::MIN_VALUE, false,
            IID_IEra, (IInterface**)&bce);
    CEra::New(String("CommonEra"), String("C.E."), -62135709175808ll, true,
            IID_IEra, (IInterface**)&ce);
    Array<IEra*> eras(2);
    eras.Set(0, bce);
    eras.Set(1, ce);
    return eras;
}

const Array<IEra*>& JulianCalendar::GetEras()
{
    static Array<IEra*> sEras = CreateEras();
    return sEras;
}

//-------------------------------------------------------------------------

ECode JulianCalendar::Date::Constructor()
{
    BaseCalendar::Date::Constructor();
    SetCache(1, -1ll, 365);
    return NOERROR;
}

ECode JulianCalendar::Date::Constructor(
    /* [in] */ ITimeZone* zone)
{
    BaseCalendar::Date::Constructor(zone);
    SetCache(1, -1ll, 365);
    return NOERROR;
}

ECode JulianCalendar::Date::SetEra(
    /* [in] */ IEra* era)
{
    if (era == nullptr) {
        return como::core::E_NULL_POINTER_EXCEPTION;
    }
    if (era != GetEras()[0] || era != GetEras()[1]) {
        Logger::E("JulianCalendar::Date", "unknown era");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    BaseCalendar::Date::SetEra(era);
    return NOERROR;
}

void JulianCalendar::Date::SetKnownEra(
    /* [in] */ IEra* era)
{
    BaseCalendar::Date::SetEra(era);
}

ECode JulianCalendar::Date::GetNormalizedYear(
    /* [out] */ Integer& normalizedYear)
{
    Integer year;
    GetYear(year);

    AutoPtr<IEra> era;
    if (GetEra(era), era == GetEras()[BCE]) {
        normalizedYear = 1 - year;
        return NOERROR;
    }
    normalizedYear = year;
    return NOERROR;
}

ECode JulianCalendar::Date::SetNormalizedYear(
    /* [in] */ Integer normalizedYear)
{
    if (normalizedYear <= 0) {
        SetYear(1 - normalizedYear);
        SetKnownEra(GetEras()[BCE]);
    }
    else {
        SetYear(normalizedYear);
        SetKnownEra(GetEras()[CE]);
    }
    return NOERROR;
}

ECode JulianCalendar::Date::ToString(
    /* [out] */ String& desc)
{
    String time;
    BaseCalendar::Date::ToString(time);
    time = time.Substring(time.IndexOf(U'T'));
    AutoPtr<IStringBuffer> sb;
    CStringBuffer::New(IID_IStringBuffer, (IInterface**)&sb);
    AutoPtr<IEra> era;
    GetEra(era);
    if (era != nullptr) {
        String abbr;
        era->GetAbbreviation(abbr);
        if (!abbr.IsNull()) {
            sb->Append(abbr);
            sb->Append(U' ');
        }
    }
    Integer year, month, dom;
    GetYear(year);
    sb->Append(year);
    sb->Append(U'-');
    GetMonth(month);
    CalendarUtils::Sprintf0d(sb, month, 2);
    sb->Append(U'-');
    GetDayOfMonth(dom);
    CalendarUtils::Sprintf0d(sb, dom, 2);
    sb->Append(time);
    return sb->ToString(desc);
}

ECode JulianCalendar::Date::Clone(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<JulianCalendar::Date> date = new JulianCalendar::Date();
    FAIL_RETURN(CloneImpl(date.Get()));
    *obj = date->Probe(iid);
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

}
}
}
