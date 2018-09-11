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
#include "ccm/util/CLocale.h"
#include "ccm/util/GregorianCalendar.h"
#include "ccm/util/TimeZone.h"
#include "ccm/util/calendar/CalendarSystem.h"
#include "ccm/util/calendar/CalendarUtils.h"
#include "ccm.core.ILong.h"
#include "ccm.util.calendar.ICalendarDate.h"
#include "ccm.util.calendar.ICalendarSystem.h"

using ccm::core::ILong;
using ccm::core::System;
using ccm::util::calendar::CalendarSystem;
using ccm::util::calendar::CalendarUtils;
using ccm::util::calendar::ICalendarDate;
using ccm::util::calendar::ICalendarSystem;

namespace ccm {
namespace util {

CCM_INTERFACE_IMPL_1(GregorianCalendar, Calendar, IGregorianCalendar);

AutoPtr<IGregorian> GregorianCalendar::GetGcal()
{
    static AutoPtr<IGregorian> gcal =
            CalendarSystem::GetGregorianCalendar();
    return gcal;
}

ECode GregorianCalendar::Constructor()
{
    Constructor(TimeZone::GetDefaultRef(),
            CLocale::GetDefault(CLocale::Category::GetFORMAT()));
    SetZoneShared(true);
    return NOERROR;
}

ECode GregorianCalendar::Constructor(
    /* [in] */ ITimeZone* zone)
{
    return Constructor(zone, CLocale::GetDefault(CLocale::Category::GetFORMAT()));
}

ECode GregorianCalendar::Constructor(
    /* [in] */ ILocale* locale)
{
    Constructor(TimeZone::GetDefaultRef(), locale);
    SetZoneShared(true);
    return NOERROR;
}

ECode GregorianCalendar::Constructor(
    /* [in] */ ITimeZone* zone,
    /* [in] */ ILocale* locale)
{
    Calendar::Constructor(zone, locale);
    AutoPtr<ICalendarDate> date;
    ICalendarSystem::Probe(GetGcal())->NewCalendarDate(&date);
    mGdate = IBaseCalendarDate::Probe(date);
    SetTimeInMillis(System::GetCurrentTimeMillis());
    return NOERROR;
}

ECode GregorianCalendar::Constructor(
    /* [in] */ Integer year,
    /* [in] */ Integer month,
    /* [in] */ Integer dayOfMonth)
{
    return Constructor(year, month, dayOfMonth, 0, 0, 0, 0);
}

ECode GregorianCalendar::Constructor(
    /* [in] */ Integer year,
    /* [in] */ Integer month,
    /* [in] */ Integer dayOfMonth,
    /* [in] */ Integer hourOfDay,
    /* [in] */ Integer minute)
{
    return Constructor(year, month, dayOfMonth, hourOfDay, minute, 0, 0);
}

ECode GregorianCalendar::Constructor(
    /* [in] */ Integer year,
    /* [in] */ Integer month,
    /* [in] */ Integer dayOfMonth,
    /* [in] */ Integer hourOfDay,
    /* [in] */ Integer minute,
    /* [in] */ Integer second)
{
    return Constructor(year, month, dayOfMonth, hourOfDay, minute, second, 0);
}

ECode GregorianCalendar::Constructor(
    /* [in] */ Integer year,
    /* [in] */ Integer month,
    /* [in] */ Integer dayOfMonth,
    /* [in] */ Integer hourOfDay,
    /* [in] */ Integer minute,
    /* [in] */ Integer second,
    /* [in] */ Integer millis)
{
    Calendar::Constructor();
    AutoPtr<ICalendarDate> date;
    ICalendarSystem::Probe(GetGcal())->NewCalendarDate(GetZone(), &date);
    mGdate = IBaseCalendarDate::Probe(date);
    Set(YEAR, year);
    Set(MONTH, month);
    Set(DAY_OF_MONTH, dayOfMonth);

    // Set AM_PM and HOUR here to set their stamp values before
    // setting HOUR_OF_DAY (6178071).
    if (hourOfDay >= 12 && hourOfDay <= 23) {
        // If hourOfDay is a valid PM hour, set the correct PM values
        // so that it won't throw an exception in case it's set to
        // non-lenient later.
        InternalSet(AM_PM, PM);
        InternalSet(HOUR, hourOfDay - 12);
    }
    else {
        // The default value for AM_PM is AM.
        // We don't care any out of range value here for leniency.
        InternalSet(HOUR, hourOfDay);
    }
    // The stamp values of AM_PM and HOUR must be COMPUTED. (6440854)
    SetFieldsComputed(HOUR_MASK | AM_PM_MASK);

    Set(HOUR_OF_DAY, hourOfDay);
    Set(MINUTE, minute);
    Set(SECOND, second);
    // should be changed to set() when this constructor is made
    // public.
    InternalSet(MILLISECOND, millis);
    return NOERROR;
}

ECode GregorianCalendar::Constructor(
    /* [in] */ ITimeZone* zone,
    /* [in] */ ILocale* locale,
    /* [in] */ Boolean flag)
{
    Constructor(zone, locale);
    AutoPtr<ICalendarDate> date;
    ICalendarSystem::Probe(GetGcal())->NewCalendarDate(GetZone(), &date);
    mGdate = IBaseCalendarDate::Probe(date);
    return NOERROR;
}

ECode GregorianCalendar::Constructor(
    /* [in] */ Long milliseconds)
{
    Constructor();
    SetTimeInMillis(milliseconds);
    return NOERROR;
}

ECode GregorianCalendar::SetGregorianChange(
    /* [in] */ IDate* date)
{
    Long cutoverTime;
    date->GetTime(&cutoverTime);
    if (cutoverTime == mGregorianCutover) {
        return NOERROR;
    }
    // Before changing the cutover date, make sure to have the
    // time of this calendar.
    Complete();
    SetGregorianChange(cutoverTime);
    return NOERROR;
}

void GregorianCalendar::SetGregorianChange(
    /* [in] */ Long cutoverTime)
{
    mGregorianCutover = cutoverTime;
    mGregorianCutoverDate = CalendarUtils::FloorDivide(cutoverTime, ONE_DAY)
            + EPOCH_OFFSET;

    // To provide the "pure" Julian calendar as advertised.
    // Strictly speaking, the last millisecond should be a
    // Gregorian date. However, the API doc specifies that setting
    // the cutover date to ILong::MAX_VALUE will make this calendar
    // a pure Julian calendar. (See 4167995)
    if (cutoverTime == ILong::MAX_VALUE) {
        mGregorianCutoverDate++;
    }

    AutoPtr<IBaseCalendarDate> d = GetGregorianCutoverDate();

    // Set the cutover year (in the Gregorian year numbering)
    ICalendarDate::Probe(d)->GetYear(&mGregorianCutoverYear);

    AutoPtr<IBaseCalendar> julianCal = GetJulianCalendarSystem();
    AutoPtr<ICalendarDate> date;
    ICalendarSystem::Probe(julianCal)->NewCalendarDate(TimeZone::NO_TIMEZONE, &date);
    d = IBaseCalendarDate::Probe(date);
    julianCal->GetCalendarDateFromFixedDate(date, mGregorianCutoverDate - 1);
    d->GetNormalizedYear(&mGregorianCutoverYearJulian);

    if (mTime < mGregorianCutover) {
        // The field values are no longer valid under the new
        // cutover date.
        SetUnnormalized();
    }
}

}
}

