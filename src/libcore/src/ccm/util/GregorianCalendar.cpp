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

#include "ccm/core/CoreUtils.h"
#include "ccm/core/Math.h"
#include "ccm/core/System.h"
#include "ccm/util/CDate.h"
#include "ccm/util/CLocale.h"
#include "ccm/util/GregorianCalendar.h"
#include "ccm/util/TimeZone.h"
#include "ccm/util/calendar/BaseCalendar.h"
#include "ccm/util/calendar/CalendarSystem.h"
#include "ccm/util/calendar/CalendarUtils.h"
#include "ccm.core.ILong.h"
#include "ccm.util.calendar.ICalendarDate.h"
#include "ccm.util.calendar.ICalendarSystem.h"
#include <ccmlogger.h>

using ccm::core::CoreUtils;
using ccm::core::ILong;
using ccm::core::Math;
using ccm::core::System;
using ccm::util::calendar::BaseCalendar;
using ccm::util::calendar::CalendarSystem;
using ccm::util::calendar::CalendarUtils;
using ccm::util::calendar::ICalendarDate;
using ccm::util::calendar::ICalendarSystem;
using ccm::util::calendar::IID_IBaseCalendarDate;
using ccm::util::calendar::IID_ICalendarDate;

namespace ccm {
namespace util {

CCM_INTERFACE_IMPL_1(GregorianCalendar, Calendar, IGregorianCalendar);

constexpr Integer GregorianCalendar::MIN_VALUES[];
constexpr Integer GregorianCalendar::LEAST_MAX_VALUES[];
constexpr Integer GregorianCalendar::MAX_VALUES[];

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

ECode GregorianCalendar::GetGregorianChange(
    /* [out] */ IDate** date)
{
    VALIDATE_NOT_NULL(date);

    return CDate::New(mGregorianCutover, IID_IDate, (IInterface**)date);
}

ECode GregorianCalendar::IsLeapYear(
    /* [in] */ Integer year,
    /* [out] */ Boolean* leap)
{
    VALIDATE_NOT_NULL(leap);

    if ((year & 3) != 0) {
        *leap = false;
        return NOERROR;
    }

    if (year > mGregorianCutoverYear) {
        *leap = (year % 100 != 0) || (year % 400 == 0); // Gregorian
        return NOERROR;
    }
    if (year < mGregorianCutoverYearJulian) {
        *leap = true; // Julian
        return NOERROR;
    }
    Boolean gregorian;
    // If the given year is the Gregorian cutover year, we need to
    // determine which calendar system to be applied to February in the year.
    if (mGregorianCutoverYear == mGregorianCutoverYearJulian) {
        AutoPtr<IBaseCalendarDate> d = GetCalendarDate(mGregorianCutoverDate); // Gregorian
        Integer month;
        ICalendarDate::Probe(d)->GetMonth(&month);
        gregorian = month < IBaseCalendar::MARCH;
    }
    else {
        gregorian = year == mGregorianCutoverYear;
    }
    *leap = gregorian ? (year % 100 != 0) || (year % 400 == 0) : true;
    return NOERROR;
}

ECode GregorianCalendar::GetCalendarType(
    /* [out] */ String* type)
{
    VALIDATE_NOT_NULL(type);

    *type = "gregory";
    return NOERROR;
}

ECode GregorianCalendar::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* same)
{
    VALIDATE_NOT_NULL(same);

    IGregorianCalendar* gregory = IGregorianCalendar::Probe(obj);
    if (gregory == nullptr) {
        *same = false;
        return NOERROR;
    }
    Calendar::Equals(obj, same);
    if (!*same) return NOERROR;
    *same = mGregorianCutover = ((GregorianCalendar*)gregory)->mGregorianCutover;
    return NOERROR;
}

ECode GregorianCalendar::GetHashCode(
    /* [out] */ Integer* hash)
{
    VALIDATE_NOT_NULL(hash);

    Calendar::GetHashCode(hash);
    *hash = *hash ^ (Integer)mGregorianCutoverDate;
    return NOERROR;
}

ECode GregorianCalendar::Add(
    /* [in] */ Integer field,
    /* [in] */ Integer amount)
{
    // If amount == 0, do nothing even the given field is out of
    // range. This is tested by JCK.
    if (amount == 0) {
        return NOERROR;   // Do nothing!
    }

    if (field < 0 || field >= ZONE_OFFSET) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // Sync the time and calendar fields.
    Complete();

    if (field == YEAR) {
        Integer year = InternalGet(YEAR);
        if (InternalGetEra() == CE) {
            year += amount;
            if (year > 0) {
                Set(YEAR, year);
            }
            else { // year <= 0
                Set(YEAR, 1 - year);
                // if year == 0, you get 1 BCE.
                Set(ERA, BCE);
            }
        }
        else { // era == BCE
            year -= amount;
            if (year > 0) {
                Set(YEAR, year);
            }
            else { // year <= 0
                Set(YEAR, 1 - year);
                // if year == 0, you get 1 CE.
                Set(ERA, CE);
            }
        }
        PinDayOfMonth();
    }
    else if (field = MONTH) {
        Integer month = InternalGet(MONTH) + amount;
        Integer year = InternalGet(YEAR);
        Integer y_amount;

        if (month >= 0) {
            y_amount = month / 12;
        }
        else {
            y_amount = (month + 1) / 12 - 1;
        }
        if (y_amount != 0) {
            if (InternalGetEra() == CE) {
                year += y_amount;
                if (year > 0) {
                    Set(YEAR, year);
                }
                else { // year <= 0
                    Set(YEAR, 1 - year);
                    // if year == 0, you get 1 BCE
                    Set(ERA, BCE);
                }
            }
            else { // era == BCE
                year -= y_amount;
                if (year > 0) {
                    Set(YEAR, year);
                }
                else { // year <= 0
                    Set(YEAR, 1 - year);
                    // if year == 0, you get 1 CE
                    Set(ERA, CE);
                }
            }
        }

        if (month >= 0) {
            Set(MONTH,  month % 12);
        }
        else {
            // month < 0
            month %= 12;
            if (month < 0) {
                month += 12;
            }
            Set(MONTH, JANUARY + month);
        }
        PinDayOfMonth();
    }
    else if (field = ERA) {
        Integer era = InternalGet(ERA) + amount;
        if (era < 0) {
            era = 0;
        }
        if (era > 1) {
            era = 1;
        }
        Set(ERA, era);
    }
    else {
        Long delta = amount;
        Long timeOfDay = 0;
        switch (field) {
            // Handle the time fields here. Convert the given
            // amount to milliseconds and call setTimeInMillis.
            case HOUR:
            case HOUR_OF_DAY:
                delta *= 60 * 60 * 1000;        // hours to minutes
                break;

            case MINUTE:
                delta *= 60 * 1000;             // minutes to seconds
                break;

            case SECOND:
                delta *= 1000;                  // seconds to milliseconds
                break;

            case MILLISECOND:
                break;

            // Handle week, day and AM_PM fields which involves
            // time zone offset change adjustment. Convert the
            // given amount to the number of days.
            case WEEK_OF_YEAR:
            case WEEK_OF_MONTH:
            case DAY_OF_WEEK_IN_MONTH:
                delta *= 7;
                break;

            case DAY_OF_MONTH: // synonym of DATE
            case DAY_OF_YEAR:
            case DAY_OF_WEEK:
                break;

            case AM_PM:
                // Convert the amount to the number of days (delta)
                // and +12 or -12 hours (timeOfDay).
                delta = amount / 2;
                timeOfDay = 12 * (amount % 2);
                break;
        }

        // The time fields don't require time zone offset change
        // adjustment.
        if (field >= HOUR) {
            SetTimeInMillis(mTime + delta);
            return NOERROR;
        }

        // The rest of the fields (week, day or AM_PM fields)
        // require time zone offset (both GMT and DST) change
        // adjustment.

        // Translate the current time to the fixed date and time
        // of the day.
        Long fd = GetCurrentFixedDate();
        timeOfDay += InternalGet(HOUR_OF_DAY);
        timeOfDay *= 60;
        timeOfDay += InternalGet(MINUTE);
        timeOfDay *= 60;
        timeOfDay += InternalGet(SECOND);
        timeOfDay *= 1000;
        timeOfDay += InternalGet(MILLISECOND);
        if (timeOfDay >= ONE_DAY) {
            fd++;
            timeOfDay -= ONE_DAY;
        }
        else if (timeOfDay < 0) {
            fd--;
            timeOfDay += ONE_DAY;
        }

        fd += delta; // fd is the expected fixed date after the calculation
        // Calculate the time in the UTC time zone.
        Long utcTime = (fd - EPOCH_OFFSET) * ONE_DAY + timeOfDay;

        // Neither of the time zone related fields are relevant because they have not been
        // set since the call to complete() above.
        Integer tzMask = 0;

        // Adjust the time to account for zone and daylight savings time offset.
        Long millis = AdjustForZoneAndDaylightSavingsTime(tzMask, utcTime, GetZone());

        // Update the time and recompute the fields.
        SetTimeInMillis(millis);
    }
}

ECode GregorianCalendar::Roll(
    /* [in] */ Integer field,
    /* [in] */ Boolean up)
{
    return Roll(field, up ? +1 : -1);
}

ECode GregorianCalendar::Roll(
    /* [in] */ Integer field,
    /* [in] */ Integer amount)
{
    // If amount == 0, do nothing even the given field is out of
    // range.
    if (amount == 0) {
        return NOERROR;
    }

    if (field < 0 || field >= ZONE_OFFSET) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // Sync the time and calendar fields.
    Complete();

    Integer min, max;
    GetMinimum(field, &min);
    GetMaximum(field, &max);

    switch(field) {
        case AM_PM:
        case ERA:
        case YEAR:
        case MINUTE:
        case SECOND:
        case MILLISECOND:
            // These fields are handled simply, since they have fixed minima
            // and maxima.  The field DAY_OF_MONTH is almost as simple.  Other
            // fields are complicated, since the range within they must roll
            // varies depending on the date.
            break;

        case HOUR:
        case HOUR_OF_DAY:
            {
                Integer unit = max + 1; // 12 or 24 hours
                Integer h = InternalGet(field);
                Integer nh = (h + amount) % unit;
                if (nh < 0) {
                    nh += unit;
                }
                mTime += ONE_HOUR * (nh - h);

                // The day might have changed, which could happen if
                // the daylight saving time transition brings it to
                // the next day, although it's very unlikely. But we
                // have to make sure not to change the larger fields.
                AutoPtr<ICalendarDate> d;
                ICalendarSystem::Probe(mCalsys)->GetCalendarDate(
                        mTime, GetZone(), &d);
                Integer days;
                if (d->GetDayOfMonth(&days), days != InternalGet(DAY_OF_MONTH)) {
                    d->SetDate(InternalGet(YEAR),
                            InternalGet(MONTH) + 1,
                            InternalGet(DAY_OF_MONTH));
                    if (field == HOUR) {
                        CHECK(InternalGet(AM_PM) == PM);
                        d->AddHours(+12); // restore PM
                    }
                    ICalendarSystem::Probe(mCalsys)->GetTime(d, &mTime);
                }
                Integer hourOfDay;
                d->GetHours(&hourOfDay);
                InternalSet(field, hourOfDay % unit);
                if (field == HOUR) {
                    InternalSet(HOUR_OF_DAY, hourOfDay);
                }
                else {
                    InternalSet(AM_PM, hourOfDay / 12);
                    InternalSet(HOUR, hourOfDay % 12);
                }

                // Time zone offset and/or daylight saving might have changed.
                Integer zoneOffset, saving;
                d->GetZoneOffset(&zoneOffset);
                d->GetDaylightSaving(&saving);
                InternalSet(ZONE_OFFSET, zoneOffset - saving);
                InternalSet(DST_OFFSET, saving);
                return NOERROR;
            }

        case MONTH:
            // Rolling the month involves both pinning the final value to [0, 11]
            // and adjusting the DAY_OF_MONTH if necessary.  We only adjust the
            // DAY_OF_MONTH if, after updating the MONTH field, it is illegal.
            // E.g., <jan31>.roll(MONTH, 1) -> <feb28> or <feb29>.
            {
                Integer normYear;
                mCdate->GetNormalizedYear(&normYear);
                if (!IsCutoverYear(normYear)) {
                    Integer mon = (InternalGet(MONTH) + amount) % 12;
                    if (mon < 0) {
                        mon += 12;
                    }
                    Set(MONTH, mon);

                    // Keep the day of month in the range.  We don't want to spill over
                    // into the next month; e.g., we don't want jan31 + 1 mo -> feb31 ->
                    // mar3.
                    Integer monthLen = MonthLength(mon);
                    if (InternalGet(DAY_OF_MONTH) > monthLen) {
                        Set(DAY_OF_MONTH, monthLen);
                    }
                }
                else {
                    // We need to take care of different lengths in
                    // year and month due to the cutover.
                    Integer yearLength;
                    GetActualMaximum(MONTH, &yearLength);
                    yearLength += 1;
                    Integer mon = (InternalGet(MONTH) + amount) % yearLength;
                    if (mon < 0) {
                        mon += yearLength;
                    }
                    Set(MONTH, mon);
                    Integer monthLen;
                    GetActualMaximum(DAY_OF_MONTH, &monthLen);
                    if (InternalGet(DAY_OF_MONTH) > monthLen) {
                        Set(DAY_OF_MONTH, monthLen);
                    }
                }
                return NOERROR;
            }

        case WEEK_OF_YEAR:
            {
                Integer y;
                mCdate->GetNormalizedYear(&y);
                GetActualMaximum(WEEK_OF_YEAR, &max);
                Set(DAY_OF_WEEK, InternalGet(DAY_OF_WEEK));
                Integer woy = InternalGet(WEEK_OF_YEAR);
                Integer value = woy + amount;
                if (!IsCutoverYear(y)) {
                    Integer weekYear;
                    GetWeekYear(&weekYear);
                    if (weekYear == y) {
                        // If the new value is in between min and max
                        // (exclusive), then we can use the value.
                        if (value > min && value < max) {
                            Set(WEEK_OF_YEAR, value);
                            return NOERROR;
                        }
                        Long fd = GetCurrentFixedDate();
                        // Make sure that the min week has the current DAY_OF_WEEK
                        // in the calendar year
                        Long day1 = fd - (7 * (woy - min));
                        Integer year;
                        if (mCalsys->GetYearFromFixedDate(day1, &year), year != y) {
                            min++;
                        }

                        // Make sure the same thing for the max week
                        fd += 7 * (max - InternalGet(WEEK_OF_YEAR));
                        if (mCalsys->GetYearFromFixedDate(fd, &year), year != y) {
                            max--;
                        }
                    }
                    else {
                        // When WEEK_OF_YEAR and YEAR are out of sync,
                        // adjust woy and amount to stay in the calendar year.
                        if (weekYear > y) {
                            if (amount < 0) {
                                amount++;
                            }
                            woy = max;
                        }
                        else {
                            if (amount > 0) {
                                amount -= woy - max;
                            }
                            woy = min;
                        }
                    }
                    Set(field, GetRolledValue(woy, amount, min, max));
                    return NOERROR;
                }

                // Handle cutover here.
                Long fd = GetCurrentFixedDate();
                AutoPtr<IBaseCalendar> cal;
                if (mGregorianCutoverYear == mGregorianCutoverYearJulian) {
                    cal = GetCutoverCalendarSystem();
                }
                else if (y == mGregorianCutoverYear) {
                    cal = IBaseCalendar::Probe(GetGcal());
                }
                else {
                    cal = GetJulianCalendarSystem();
                }
                Long day1 = fd - (7 * (woy - min));
                // Make sure that the min week has the current DAY_OF_WEEK
                Integer year;
                if (cal->GetYearFromFixedDate(day1, &year), year != y) {
                    min++;
                }

                // Make sure the same thing for the max week
                fd += 7 * (max - woy);
                cal = (fd >= mGregorianCutoverDate) ? IBaseCalendar::Probe(GetGcal()) : GetJulianCalendarSystem().Get();
                if (cal->GetYearFromFixedDate(fd, &year), year != y) {
                    max--;
                }
                // value: the new WEEK_OF_YEAR which must be converted
                // to month and day of month.
                value = GetRolledValue(woy, amount, min, max) - 1;
                AutoPtr<IBaseCalendarDate> d = GetCalendarDate(day1 + value * 7);
                Integer month, days;
                ICalendarDate::Probe(d)->GetMonth(&month);
                ICalendarDate::Probe(d)->GetDayOfMonth(&days);
                Set(MONTH, month - 1);
                Set(DAY_OF_MONTH, days);
                return NOERROR;
            }

        case WEEK_OF_MONTH:
            {
                Integer normYear;
                mCdate->GetNormalizedYear(&normYear);
                Boolean isCutoverYear = IsCutoverYear(normYear);
                // dow: relative day of week from first day of week
                Integer firstDay;
                GetFirstDayOfWeek(&firstDay);
                Integer dow = InternalGet(DAY_OF_WEEK) - firstDay;
                if (dow < 0) {
                    dow += 7;
                }

                Long fd = GetCurrentFixedDate();
                Long month1;     // fixed date of the first day (usually 1) of the month
                Integer monthLength; // actual month length
                if (isCutoverYear) {
                    month1 = GetFixedDateMonth1(mCdate, fd);
                    monthLength = ActualMonthLength();
                }
                else {
                    month1 = fd - InternalGet(DAY_OF_MONTH) + 1;
                    ICalendarSystem::Probe(mCalsys)->GetMonthLength(
                            ICalendarDate::Probe(mCdate), &monthLength);
                }

                // the first day of week of the month.
                Long monthDay1st = BaseCalendar::GetDayOfWeekDateOnOrBefore(
                        month1 + 6, firstDay);

                // if the week has enough days to form a week, the
                // week starts from the previous month.
                Integer minDays;
                GetMinimalDaysInFirstWeek(&minDays);
                if ((Integer)(monthDay1st - month1) >= minDays) {
                    monthDay1st -= 7;
                }
                GetActualMaximum(field, &max);

                // value: the new WEEK_OF_MONTH value
                Integer value = GetRolledValue(InternalGet(field), amount, 1, max) - 1;

                // nfd: fixed date of the rolled date
                Long nfd = monthDay1st + value * 7 + dow;

                // Unlike WEEK_OF_YEAR, we need to change day of week if the
                // nfd is out of the month.
                if (nfd < month1) {
                    nfd = month1;
                }
                else if (nfd >= (month1 + monthLength)) {
                    nfd = month1 + monthLength - 1;
                }
                Integer dayOfMonth;
                if (isCutoverYear) {
                    // If we are in the cutover year, convert nfd to
                    // its calendar date and use dayOfMonth.
                    AutoPtr<IBaseCalendarDate> d = GetCalendarDate(nfd);
                    ICalendarDate::Probe(d)->GetDayOfMonth(&dayOfMonth);
                }
                else {
                    dayOfMonth = (Integer)(nfd - month1) + 1;
                }
                Set(DAY_OF_MONTH, dayOfMonth);
                return NOERROR;
            }

        case DAY_OF_MONTH:
            {
                Integer normYear;
                mCdate->GetNormalizedYear(&normYear);
                if (!IsCutoverYear(normYear)) {
                    ICalendarSystem::Probe(mCalsys)->GetMonthLength(
                            ICalendarDate::Probe(mCdate), &max);
                    break;
                }

                // Cutover year handling
                Long fd = GetCurrentFixedDate();
                Long month1 = GetFixedDateMonth1(mCdate, fd);
                // It may not be a regular month. Convert the date and range to
                // the relative values, perform the roll, and
                // convert the result back to the rolled date.
                Integer value = GetRolledValue((Integer)(fd - month1), amount, 0, ActualMonthLength() - 1);
                AutoPtr<IBaseCalendarDate> d = GetCalendarDate(month1 + value);
                BLOCK_CHECK() {
                    Integer month;
                    ICalendarDate::Probe(d)->GetMonth(&month);
                    CHECK(month - 1 == InternalGet(MONTH));
                }
                Integer dayOfMonth;
                ICalendarDate::Probe(d)->GetDayOfMonth(&dayOfMonth);
                Set(DAY_OF_MONTH, dayOfMonth);
                return NOERROR;
            }

        case DAY_OF_YEAR:
            {
                GetActualMaximum(field, &max);
                Integer normYear;
                mCdate->GetNormalizedYear(&normYear);
                if (!IsCutoverYear(normYear)) {
                    break;
                }

                // Handle cutover here.
                Long fd = GetCurrentFixedDate();
                Long jan1 = fd - InternalGet(DAY_OF_YEAR) + 1;
                Integer value = GetRolledValue((Integer)(fd - jan1) + 1, amount, min, max);
                AutoPtr<IBaseCalendarDate> d = GetCalendarDate(jan1 + value - 1);
                Integer month, dayOfMonth;
                ICalendarDate::Probe(d)->GetMonth(&month);
                ICalendarDate::Probe(d)->GetDayOfMonth(&dayOfMonth);
                Set(MONTH, month - 1);
                Set(DAY_OF_MONTH, dayOfMonth);
                return NOERROR;
            }

        case DAY_OF_WEEK:
            {
                Integer normYear;
                mCdate->GetNormalizedYear(&normYear);
                if (!IsCutoverYear(normYear)) {
                    // If the week of year is in the same year, we can
                    // just change DAY_OF_WEEK.
                    Integer weekOfYear = InternalGet(WEEK_OF_YEAR);
                    if (weekOfYear > 1 && weekOfYear < 52) {
                        Set(WEEK_OF_YEAR, weekOfYear); // update stamp[WEEK_OF_YEAR]
                        max = SATURDAY;
                        break;
                    }
                }

                // We need to handle it in a different way around year
                // boundaries and in the cutover year. Note that
                // changing era and year values violates the roll
                // rule: not changing larger calendar fields...
                amount %= 7;
                if (amount == 0) {
                    return NOERROR;
                }
                Long fd = GetCurrentFixedDate();
                Integer firstDay;
                GetFirstDayOfWeek(&firstDay);
                Long dowFirst = BaseCalendar::GetDayOfWeekDateOnOrBefore(
                        fd, firstDay);
                fd += amount;
                if (fd < dowFirst) {
                    fd += 7;
                }
                else if (fd >= dowFirst + 7) {
                    fd -= 7;
                }
                AutoPtr<IBaseCalendarDate> d = GetCalendarDate(fd);
                Integer year, month, dayOfMonth;
                d->GetNormalizedYear(&normYear);
                ICalendarDate* date = ICalendarDate::Probe(d);
                date->GetYear(&year);
                date->GetMonth(&month);
                date->GetDayOfMonth(&dayOfMonth);
                Set(ERA, (normYear <= 0 ? BCE : CE));
                Set(year, month - 1, dayOfMonth);
                return NOERROR;
            }

        case DAY_OF_WEEK_IN_MONTH:
            {
                min = 1; // after normalized, min should be 1.
                Integer normYear;
                mCdate->GetNormalizedYear(&normYear);
                if (!IsCutoverYear(normYear)) {
                    Integer dom = InternalGet(DAY_OF_MONTH);
                    Integer monthLength;
                    ICalendarSystem::Probe(mCalsys)->GetMonthLength(
                            ICalendarDate::Probe(mCdate), &monthLength);
                    Integer lastDays = monthLength % 7;
                    max = monthLength / 7;
                    Integer x = (dom - 1) % 7;
                    if (x < lastDays) {
                        max++;
                    }
                    Set(DAY_OF_WEEK, InternalGet(DAY_OF_WEEK));
                    break;
                }

                // Cutover year handling
                Long fd = GetCurrentFixedDate();
                Long month1 = GetFixedDateMonth1(mCdate, fd);
                Integer monthLength = ActualMonthLength();
                Integer lastDays = monthLength % 7;
                max = monthLength / 7;
                Integer x = (Integer)(fd - month1) % 7;
                if (x < lastDays) {
                    max++;
                }
                Integer value = GetRolledValue(InternalGet(field), amount, min, max) - 1;
                fd = month1 + value * 7 + x;
                AutoPtr<IBaseCalendar> cal = (fd >= mGregorianCutoverDate) ?
                        IBaseCalendar::Probe(GetGcal()) : GetJulianCalendarSystem().Get();
                AutoPtr<ICalendarDate> date;
                ICalendarSystem::Probe(cal)->NewCalendarDate(TimeZone::NO_TIMEZONE, &date);
                cal->GetCalendarDateFromFixedDate(date, fd);
                Integer dayOfMonth;
                date->GetDayOfMonth(&dayOfMonth);
                Set(DAY_OF_MONTH, dayOfMonth);
                return NOERROR;
            }
    }

    Set(field, GetRolledValue(InternalGet(field), amount, min, max));
}

ECode GregorianCalendar::GetMinimum(
    /* [in] */ Integer field,
    /* [out] */ Integer* value)
{
    VALIDATE_NOT_NULL(value);

    *value = MIN_VALUES[field];
    return NOERROR;
}

ECode GregorianCalendar::GetMaximum(
    /* [in] */ Integer field,
    /* [out] */ Integer* value)
{
    VALIDATE_NOT_NULL(value);

    switch (field) {
        case MONTH:
        case DAY_OF_MONTH:
        case DAY_OF_YEAR:
        case WEEK_OF_YEAR:
        case WEEK_OF_MONTH:
        case DAY_OF_WEEK_IN_MONTH:
        case YEAR:
            {
                // On or after Gregorian 200-3-1, Julian and Gregorian
                // calendar dates are the same or Gregorian dates are
                // larger (i.e., there is a "gap") after 300-3-1.
                if (mGregorianCutoverYear > 200) {
                    break;
                }
                // There might be "overlapping" dates.
                AutoPtr<IGregorianCalendar> clone;
                Clone(IID_IGregorianCalendar, (IInterface**)&clone);
                GregorianCalendar* gc = (GregorianCalendar*)clone.Get();
                gc->SetLenient(true);
                gc->SetTimeInMillis(mGregorianCutover);
                Integer v1, v2;
                gc->GetActualMaximum(field, &v1);
                gc->SetTimeInMillis(mGregorianCutover - 1);
                gc->GetActualMaximum(field, &v2);
                *value = Math::Max(MAX_VALUES[field], Math::Max(v1, v2));
                return NOERROR;
            }
    }
    *value = MAX_VALUES[field];
    return NOERROR;
}

ECode GregorianCalendar::GetGreatestMinimum(
    /* [in] */ Integer field,
    /* [out] */ Integer* value)
{
    VALIDATE_NOT_NULL(value);

    if (field == DAY_OF_MONTH) {
        AutoPtr<IBaseCalendarDate> d = GetGregorianCutoverDate();
        Long mon1 = GetFixedDateMonth1(d, mGregorianCutoverDate);
        d = GetCalendarDate(mon1);
        Integer dayOfMonth;
        ICalendarDate::Probe(d)->GetDayOfMonth(&dayOfMonth);
        *value = Math::Max(MIN_VALUES[field], dayOfMonth);
        return NOERROR;
    }
    *value = MIN_VALUES[field];
    return NOERROR;
}

ECode GregorianCalendar::GetLeastMaximum(
    /* [in] */ Integer field,
    /* [out] */ Integer* value)
{
    VALIDATE_NOT_NULL(value);

    switch (field) {
        case MONTH:
        case DAY_OF_MONTH:
        case DAY_OF_YEAR:
        case WEEK_OF_YEAR:
        case WEEK_OF_MONTH:
        case DAY_OF_WEEK_IN_MONTH:
        case YEAR:
            {
                AutoPtr<IGregorianCalendar> clone;
                Clone(IID_IGregorianCalendar, (IInterface**)&clone);
                GregorianCalendar* gc = (GregorianCalendar*)clone.Get();
                gc->SetLenient(true);
                gc->SetTimeInMillis(mGregorianCutover);
                Integer v1, v2;
                gc->GetActualMaximum(field, &v1);
                gc->SetTimeInMillis(mGregorianCutover - 1);
                gc->GetActualMaximum(field, &v2);
                *value = Math::Min(LEAST_MAX_VALUES[field], Math::Min(v1, v2));
                return NOERROR;
            }
    }
    *value = LEAST_MAX_VALUES[field];
    return NOERROR;
}

ECode GregorianCalendar::GetActualMinimum(
    /* [in] */ Integer field,
    /* [out] */ Integer* value)
{
    VALIDATE_NOT_NULL(value);

    if (field == DAY_OF_MONTH) {
        AutoPtr<GregorianCalendar> gc = GetNormalizedCalendar();
        Integer year;
        gc->mCdate->GetNormalizedYear(&year);
        if (year == mGregorianCutoverYear || year == mGregorianCutoverYearJulian) {
            Long date;
            gc->mCalsys->GetFixedDate(ICalendarDate::Probe(gc->mCdate), &date);
            Long month1 = GetFixedDateMonth1(gc->mCdate, date);
            AutoPtr<IBaseCalendarDate> d = GetCalendarDate(month1);
            return ICalendarDate::Probe(d)->GetDayOfMonth(value);
        }
    }
    return GetMinimum(field, value);
}

ECode GregorianCalendar::GetActualMaximum(
    /* [in] */ Integer field,
    /* [out] */ Integer* retValue)
{
    VALIDATE_NOT_NULL(retValue);

    Integer fieldsForFixedMax = ERA_MASK | DAY_OF_WEEK_MASK | HOUR_MASK | AM_PM_MASK |
            HOUR_OF_DAY_MASK | MINUTE_MASK | SECOND_MASK | MILLISECOND_MASK |
            ZONE_OFFSET_MASK | DST_OFFSET_MASK;
    if ((fieldsForFixedMax & (1 << field)) != 0) {
        return GetMaximum(field, retValue);
    }

    AutoPtr<GregorianCalendar> gc = GetNormalizedCalendar();
    AutoPtr<IBaseCalendarDate> date = gc->mCdate;
    AutoPtr<IBaseCalendar> cal = gc->mCalsys;
    Integer normalizedYear;
    date->GetNormalizedYear(&normalizedYear);

    Integer value = -1;
    switch (field) {
        case MONTH:
            {
                if (!gc->IsCutoverYear(normalizedYear)) {
                    value = DECEMBER;
                    break;
                }

                // January 1 of the next year may or may not exist.
                Long nextJan1;
                do {
                    IBaseCalendar::Probe(GetGcal())->GetFixedDate(
                            ++normalizedYear, IBaseCalendar::JANUARY, 1, nullptr, &nextJan1);
                } while (nextJan1 < mGregorianCutoverDate);
                AutoPtr<ICalendarDate> d;
                ICloneable::Probe(date)->Clone(IID_ICalendarDate, (IInterface**)&d);
                cal->GetCalendarDateFromFixedDate(d, nextJan1 - 1);
                Integer month;
                d->GetMonth(&month);
                value = month - 1;
            }
            break;

        case DAY_OF_MONTH:
            {
                ICalendarSystem::Probe(cal)->GetMonthLength(ICalendarDate::Probe(date), &value);
                Integer dayOfMonth;
                if (!gc->IsCutoverYear(normalizedYear) ||
                        (ICalendarDate::Probe(date)->GetDayOfMonth(&dayOfMonth), dayOfMonth == value)) {
                    break;
                }

                // Handle cutover year.
                Long fd = gc->GetCurrentFixedDate();
                if (fd >= mGregorianCutoverDate) {
                    break;
                }
                Integer monthLength = gc->ActualMonthLength();
                Long monthEnd = gc->GetFixedDateMonth1(gc->mCdate, fd) + monthLength - 1;
                // Convert the fixed date to its calendar date.
                AutoPtr<IBaseCalendarDate> d = gc->GetCalendarDate(monthEnd);
                ICalendarDate::Probe(d)->GetDayOfMonth(&value);
            }
            break;

        case DAY_OF_YEAR:
            {
                if (!gc->IsCutoverYear(normalizedYear)) {
                    ICalendarSystem::Probe(cal)->GetYearLength(ICalendarDate::Probe(date), &value);
                    break;
                }

                // Handle cutover year.
                Long jan1;
                if (mGregorianCutoverYear == mGregorianCutoverYearJulian) {
                    AutoPtr<IBaseCalendar> cocal = gc->GetCutoverCalendarSystem();
                    cocal->GetFixedDate(normalizedYear, 1, 1, nullptr, &jan1);
                }
                else if (normalizedYear == mGregorianCutoverYearJulian) {
                    cal->GetFixedDate(normalizedYear, 1, 1, nullptr, &jan1);
                }
                else {
                    jan1 = mGregorianCutoverDate;
                }
                // January 1 of the next year may or may not exist.
                Long nextJan1;
                IBaseCalendar::Probe(GetGcal())->GetFixedDate(++normalizedYear, 1, 1, nullptr, &nextJan1);
                if (nextJan1 < mGregorianCutoverDate) {
                    nextJan1 = mGregorianCutoverDate;
                }
                BLOCK_CHECK() {
                    Integer normYear, month, dayOfMonth;
                    date->GetNormalizedYear(&normYear);
                    ICalendarDate::Probe(date)->GetMonth(&month);
                    ICalendarDate::Probe(date)->GetDayOfMonth(&dayOfMonth);
                    Long d;
                    cal->GetFixedDate(normYear, month, dayOfMonth, date, &d);
                    CHECK(jan1 < d);
                    CHECK(nextJan1 >= d);
                }
                value = (Integer)(nextJan1 - jan1);
            }
            break;

        case WEEK_OF_YEAR:
            {
                if (!gc->IsCutoverYear(normalizedYear)) {
                    // Get the day of week of January 1 of the year
                    AutoPtr<ICalendarDate> d;
                    ICalendarSystem::Probe(cal)->NewCalendarDate(TimeZone::NO_TIMEZONE, &d);
                    Integer year;
                    ICalendarDate::Probe(date)->GetYear(&year);
                    d->SetDate(year, IBaseCalendar::JANUARY, 1);
                    Integer dayOfWeek;
                    cal->GetDayOfWeek(d, &dayOfWeek);
                    // Normalize the day of week with the firstDayOfWeek value
                    Integer firstDayOfWeek;
                    GetFirstDayOfWeek(&firstDayOfWeek);
                    dayOfWeek -= firstDayOfWeek;
                    if (dayOfWeek < 0) {
                        dayOfWeek += 7;
                    }
                    value = 52;
                    Integer minDays;
                    GetMinimalDaysInFirstWeek(&minDays);
                    Integer magic = dayOfWeek + minDays - 1;
                    Boolean leap;
                    if ((magic == 6) ||
                        ((ICalendarDate::Probe(date)->IsLeapYear(&leap), leap) && (magic == 5 || magic == 12))) {
                        value++;
                    }
                    break;
                }

                if (gc == this) {
                    AutoPtr<IGregorianCalendar> clone;
                    gc->Clone(IID_IGregorianCalendar, (IInterface**)&clone);
                    gc = (GregorianCalendar*)clone.Get();
                }
                Integer maxDayOfYear;
                GetActualMaximum(DAY_OF_YEAR, &maxDayOfYear);
                gc->Set(DAY_OF_YEAR, maxDayOfYear);
                gc->Get(WEEK_OF_YEAR, &value);
                Integer weeks;
                gc->GetWeekYear(&weeks);
                if (InternalGet(YEAR) != weeks) {
                    gc->Set(DAY_OF_YEAR, maxDayOfYear - 7);
                    gc->Get(WEEK_OF_YEAR, &value);
                }
            }
            break;

        case WEEK_OF_MONTH:
            {
                if (!gc->IsCutoverYear(normalizedYear)) {
                    AutoPtr<ICalendarDate> d;
                    ICalendarSystem::Probe(cal)->NewCalendarDate(nullptr, &d);
                    Integer year, month;
                    ICalendarDate::Probe(date)->GetYear(&year);
                    ICalendarDate::Probe(date)->GetMonth(&month);
                    d->SetDate(year, month, 1);
                    Integer dayOfWeek;
                    cal->GetDayOfWeek(d, &dayOfWeek);
                    Integer monthLength;
                    ICalendarSystem::Probe(cal)->GetMonthLength(d, &monthLength);
                    Integer firstDayOfWeek;
                    GetFirstDayOfWeek(&firstDayOfWeek);
                    dayOfWeek -= firstDayOfWeek;
                    if (dayOfWeek < 0) {
                        dayOfWeek += 7;
                    }
                    Integer nDaysFirstWeek = 7 - dayOfWeek; // # of days in the first week
                    value = 3;
                    Integer minDays;
                    GetMinimalDaysInFirstWeek(&minDays);
                    if (nDaysFirstWeek >= minDays) {
                        value++;
                    }
                    monthLength -= nDaysFirstWeek + 7 * 3;
                    if (monthLength > 0) {
                        value++;
                        if (monthLength > 7) {
                            value++;
                        }
                    }
                    break;
                }

                // Cutover year handling
                if (gc == this) {
                    AutoPtr<IGregorianCalendar> clone;
                    gc->Clone(IID_IGregorianCalendar, (IInterface**)&clone);
                    gc = (GregorianCalendar*)clone.Get();
                }
                Integer y = gc->InternalGet(YEAR);
                Integer m = gc->InternalGet(MONTH);
                Integer year, month;
                do {
                    gc->Get(WEEK_OF_MONTH, &value);
                    gc->Add(WEEK_OF_MONTH, +1);
                } while ((gc->Get(YEAR, &year), year == y) && (gc->Get(MONTH, &month), month == m));
            }
            break;

        case DAY_OF_WEEK_IN_MONTH:
            {
                // may be in the Gregorian cutover month
                Integer ndays, dow1;
                Integer dow;
                ICalendarDate::Probe(date)->GetDayOfWeek(&dow);
                if (!gc->IsCutoverYear(normalizedYear)) {
                    AutoPtr<ICalendarDate> d;
                    ICloneable::Probe(date)->Clone(IID_ICalendarDate, (IInterface**)&d);
                    ICalendarSystem::Probe(cal)->GetMonthLength(d, &ndays);
                    d->SetDayOfMonth(1);
                    ICalendarSystem::Probe(cal)->Normalize(d);
                    d->GetDayOfWeek(&dow1);
                }
                else {
                    // Let a cloned GregorianCalendar take care of the cutover cases.
                    if (gc == this) {
                        AutoPtr<IGregorianCalendar> clone;
                        gc->Clone(IID_IGregorianCalendar, (IInterface**)&clone);
                        gc = (GregorianCalendar*)clone.Get();
                    }
                    ndays = gc->ActualMonthLength();
                    Integer dayOfMonth;
                    gc->GetActualMinimum(DAY_OF_MONTH, &dayOfMonth);
                    gc->Set(DAY_OF_MONTH, dayOfMonth);
                    gc->Get(DAY_OF_WEEK, &dow1);
                }
                Integer x = dow - dow1;
                if (x < 0) {
                    x += 7;
                }
                ndays -= x;
                value = (ndays + 6) / 7;
            }
            break;

        case YEAR:
            /* The year computation is no different, in principle, from the
             * others, however, the range of possible maxima is large.  In
             * addition, the way we know we've exceeded the range is different.
             * For these reasons, we use the special case code below to handle
             * this field.
             *
             * The actual maxima for YEAR depend on the type of calendar:
             *
             *     Gregorian = May 17, 292275056 BCE - Aug 17, 292278994 CE
             *     Julian    = Dec  2, 292269055 BCE - Jan  3, 292272993 CE
             *     Hybrid    = Dec  2, 292269055 BCE - Aug 17, 292278994 CE
             *
             * We know we've exceeded the maximum when either the month, date,
             * time, or era changes in response to setting the year.  We don't
             * check for month, date, and time here because the year and era are
             * sufficient to detect an invalid year setting.  NOTE: If code is
             * added to check the month and date in the future for some reason,
             * Feb 29 must be allowed to shift to Mar 1 when setting the year.
             */
            {
                if (gc == this) {
                    AutoPtr<IGregorianCalendar> clone;
                    gc->Clone(IID_IGregorianCalendar, (IInterface**)&clone);
                    gc = (GregorianCalendar*)clone.Get();
                }

                // Calculate the millisecond offset from the beginning
                // of the year of this calendar and adjust the max
                // year value if we are beyond the limit in the max
                // year.
                Long current = gc->GetYearOffsetInMillis();

                if (gc->InternalGetEra() == CE) {
                    gc->SetTimeInMillis(ILong::MAX_VALUE);
                    gc->Get(YEAR, &value);
                    Long maxEnd = gc->GetYearOffsetInMillis();
                    if (current > maxEnd) {
                        value--;
                    }
                }
                else {
                    Long time;
                    gc->GetTimeInMillis(&time);
                    AutoPtr<ICalendarSystem> mincal = time >= mGregorianCutover ?
                        ICalendarSystem::Probe(GetGcal()) : ICalendarSystem::Probe(GetJulianCalendarSystem());
                    AutoPtr<ICalendarDate> d;
                    mincal->GetCalendarDate(ILong::MIN_VALUE, GetZone(), &d);
                    Long dayOfYear;
                    cal->GetDayOfYear(d, &dayOfYear);
                    Integer hours;
                    d->GetHours(&hours);
                    Long maxEnd = (dayOfYear - 1) * 24 + hours;
                    Integer minutes, seconds, millis;
                    d->GetMinutes(&minutes);
                    d->GetSeconds(&seconds);
                    d->GetMillis(&millis);
                    maxEnd *= 60;
                    maxEnd += minutes;
                    maxEnd *= 60;
                    maxEnd += seconds;
                    maxEnd *= 1000;
                    maxEnd += millis;
                    d->GetYear(&value);
                    if (value <= 0) {
                        CHECK(mincal == gcal);
                        value = 1 - value;
                    }
                    if (current < maxEnd) {
                        value--;
                    }
                }
            }
            break;

        default:
            return ccm::core::E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    *retValue = value;
    return NOERROR;
}

Long GregorianCalendar::GetYearOffsetInMillis()
{
    Long t = (InternalGet(DAY_OF_YEAR) - 1) * 24;
        t += InternalGet(HOUR_OF_DAY);
        t *= 60;
        t += InternalGet(MINUTE);
        t *= 60;
        t += InternalGet(SECOND);
        t *= 1000;
    return t + InternalGet(MILLISECOND) -
            (InternalGet(ZONE_OFFSET) + InternalGet(DST_OFFSET));
}

ECode GregorianCalendar::CloneImpl(
    /* [in] */ IGregorianCalendar* newObj)
{
    GregorianCalendar* other = (GregorianCalendar*)newObj;

    other->mGdate = (IBaseCalendarDate*)CoreUtils::Clone(mGdate, IID_IBaseCalendarDate).Get();
    if (mCdate != nullptr) {
        if (mCdate != mGdate) {
            other->mCdate = (IBaseCalendarDate*)CoreUtils::Clone(mCdate, IID_IBaseCalendarDate).Get();
        }
        else {
            other->mCdate = other->mGdate;
        }
    }
    return NOERROR;
}

ECode GregorianCalendar::GetTimeZone(
    /* [out] */ ITimeZone** zone)
{
    VALIDATE_NOT_NULL(zone);

    Calendar::GetTimeZone(zone);
    // To share the zone by CalendarDates
    ICalendarDate::Probe(mGdate)->SetZone(*zone);
    if (mCdate != nullptr && mCdate != mGdate) {
        ICalendarDate::Probe(mCdate)->SetZone(*zone);
    }
    return NOERROR;
}

ECode GregorianCalendar::SetTimeZone(
    /* [in] */ ITimeZone* zone)
{
    Calendar::SetTimeZone(zone);
    // To share the zone by CalendarDates
    ICalendarDate::Probe(mGdate)->SetZone(zone);
    if (mCdate != nullptr && mCdate != mGdate) {
        ICalendarDate::Probe(mCdate)->SetZone(zone);
    }
    return NOERROR;
}

ECode GregorianCalendar::IsWeekDateSupported(
    /* [out] */ Boolean* supported)
{
    VALIDATE_NOT_NULL(supported);

    *supported = true;
    return NOERROR;
}

ECode GregorianCalendar::GetWeekYear(
    /* [out] */ Integer* weekYear)
{
    VALIDATE_NOT_NULL(weekYear);

    Integer year;
    Get(YEAR, &year); // implicitly calls complete()
    if (InternalGetEra() == BCE) {
        year = 1 - year;
    }

    // Fast path for the Gregorian calendar years that are never
    // affected by the Julian-Gregorian transition
    if (year > mGregorianCutoverYear + 1) {
        Integer weekOfYear = InternalGet(WEEK_OF_YEAR);
        if (InternalGet(MONTH) == JANUARY) {
            if (weekOfYear >= 52) {
                --year;
            }
        }
        else {
            if (weekOfYear == 1) {
                ++year;
            }
        }
        *weekYear = year;
        return NOERROR;
    }

    // General (slow) path
    Integer dayOfYear = InternalGet(DAY_OF_YEAR);
    Integer maxDayOfYear;
    GetActualMaximum(DAY_OF_YEAR, &maxDayOfYear);
    Integer minimalDays;
    GetMinimalDaysInFirstWeek(&minimalDays);

    // Quickly check the possibility of year adjustments before
    // cloning this GregorianCalendar.
    if (dayOfYear > minimalDays && dayOfYear < (maxDayOfYear - 6)) {
        *weekYear = year;
        return NOERROR;
    }

    // Create a clone to work on the calculation
    AutoPtr<IGregorianCalendar> clone;
    Clone(IID_IGregorianCalendar, (IInterface**)&clone);
    GregorianCalendar* cal = (GregorianCalendar*)clone.Get();
    cal->SetLenient(true);
    // Use GMT so that intermediate date calculations won't
    // affect the time of day fields.
    AutoPtr<ITimeZone> zone;
    TimeZone::GetTimeZone(String("GMT"), &zone);
    cal->SetTimeZone(zone);
    // Go to the first day of the year, which is usually January 1.
    cal->Set(DAY_OF_YEAR, 1);
    cal->Complete();

    // Get the first day of the first day-of-week in the year.
    Integer firstDayOfWeek, dayOfWeek;
    GetFirstDayOfWeek(&firstDayOfWeek);
    cal->Get(DAY_OF_WEEK, &dayOfWeek);
    Integer delta = firstDayOfWeek - dayOfWeek;
    if (delta != 0) {
        if (delta < 0) {
            delta += 7;
        }
        cal->Add(DAY_OF_YEAR, delta);
    }
    Integer minDayOfYear;
    cal->Get(DAY_OF_YEAR, &minDayOfYear);
    if (dayOfYear < minDayOfYear) {
        if (minDayOfYear <= minimalDays) {
            --year;
        }
    }
    else {
        cal->Set(YEAR, year + 1);
        cal->Set(DAY_OF_YEAR, 1);
        cal->Complete();
        GetFirstDayOfWeek(&firstDayOfWeek);
        cal->Get(DAY_OF_WEEK, &dayOfWeek);
        Integer del = firstDayOfWeek - dayOfWeek;
        if (del != 0) {
            if (del < 0) {
                del += 7;
            }
            cal->Add(DAY_OF_YEAR, del);
        }
        cal->Get(DAY_OF_YEAR, &minDayOfYear);
        minDayOfYear = minDayOfYear - 1;
        if (minDayOfYear == 0) {
            minDayOfYear = 7;
        }
        if (minDayOfYear >= minimalDays) {
            Integer days = maxDayOfYear - dayOfYear + 1;
            if (days <= (7 - minDayOfYear)) {
                ++year;
            }
        }
    }
    *weekYear = year;
    return NOERROR;
}

ECode GregorianCalendar::SetWeekDate(
    /* [in] */ Integer weekYear,
    /* [in] */ Integer weekOfYear,
    /* [in] */ Integer dayOfWeek)
{
    if (dayOfWeek < SUNDAY || dayOfWeek > SATURDAY) {
        Logger::E("GregorianCalendar", "invalid dayOfWeek: %d", dayOfWeek);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // To avoid changing the time of day fields by date
    // calculations, use a clone with the GMT time zone.
    AutoPtr<IGregorianCalendar> clone;
    Clone(IID_IGregorianCalendar, (IInterface**)&clone);
    GregorianCalendar* gc = (GregorianCalendar*)clone.Get();
    gc->SetLenient(true);
    Integer era;
    gc->Get(ERA, &era);
    gc->Clear();
    AutoPtr<ITimeZone> zone;
    TimeZone::GetTimeZone(String("GMT"), &zone);
    gc->SetTimeZone(zone);
    gc->Set(ERA, era);
    gc->Set(YEAR, weekYear);
    gc->Set(WEEK_OF_YEAR, 1);
    Integer firstDayOfWeek;
    GetFirstDayOfWeek(&firstDayOfWeek);
    gc->Set(DAY_OF_WEEK, firstDayOfWeek);
    Integer days = dayOfWeek - firstDayOfWeek;
    if (days < 0) {
        days += 7;
    }
    days += 7 * (weekOfYear - 1);
    if (days != 0) {
        gc->Add(DAY_OF_YEAR, days);
    }
    else {
        gc->Complete();
    }

    Boolean lenient;
    Integer gcWeekYear;
    if ((IsLenient(&lenient), !lenient) &&
        ((gc->GetWeekYear(&gcWeekYear), gcWeekYear != weekYear)
         || gc->InternalGet(WEEK_OF_YEAR) != weekOfYear
         || gc->InternalGet(DAY_OF_WEEK) != dayOfWeek)) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Set(ERA, gc->InternalGet(ERA));
    Set(YEAR, gc->InternalGet(YEAR));
    Set(MONTH, gc->InternalGet(MONTH));
    Set(DAY_OF_MONTH, gc->InternalGet(DAY_OF_MONTH));

    // to avoid throwing an IllegalArgumentException in
    // non-lenient, set WEEK_OF_YEAR internally
    InternalSet(WEEK_OF_YEAR, weekOfYear);
    Complete();
    return NOERROR;
}

ECode GregorianCalendar::GetWeeksInWeekYear(
    /* [out] */ Integer* weeks)
{
    VALIDATE_NOT_NULL(weeks);

    AutoPtr<GregorianCalendar> gc = GetNormalizedCalendar();
    Integer weekYear;
    gc->GetWeekYear(&weekYear);
    if (weekYear == gc->InternalGet(YEAR)) {
        return gc->GetActualMaximum(WEEK_OF_YEAR, weeks);
    }

    // Use the 2nd week for calculating the max of WEEK_OF_YEAR
    if (gc == this) {
        AutoPtr<IGregorianCalendar> clone;
        Clone(IID_IGregorianCalendar, (IInterface**)&clone);
        gc = (GregorianCalendar*)clone.Get();
    }
    gc->SetWeekDate(weekYear, 2, InternalGet(DAY_OF_WEEK));
    return gc->GetActualMaximum(WEEK_OF_YEAR, weeks);
}

}
}

