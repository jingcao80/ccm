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

#include "como/core/AutoLock.h"
#include "como/core/CoreUtils.h"
#include "como/core/Math.h"
#include "como/core/System.h"
#include "como/util/CDate.h"
#include "como/util/CLocale.h"
#include "como/util/GregorianCalendar.h"
#include "como/util/TimeZone.h"
#include "como/util/calendar/BaseCalendar.h"
#include "como/util/calendar/CalendarSystem.h"
#include "como/util/calendar/CalendarUtils.h"
#include "como/util/calendar/Gregorian.h"
#include "como.core.ILong.h"
#include "como.util.calendar.ICalendarDate.h"
#include "como.util.calendar.ICalendarSystem.h"
#include "libcore.util.IZoneInfo.h"
#include <comolog.h>

using como::core::AutoLock;
using como::core::CoreUtils;
using como::core::ILong;
using como::core::Math;
using como::core::System;
using como::util::calendar::BaseCalendar;
using como::util::calendar::CalendarSystem;
using como::util::calendar::CalendarUtils;
using como::util::calendar::Gregorian;
using como::util::calendar::ICalendarDate;
using como::util::calendar::ICalendarSystem;
using como::util::calendar::IID_IBaseCalendarDate;
using como::util::calendar::IID_ICalendarDate;
using libcore::util::IZoneInfo;

namespace como {
namespace util {

COMO_INTERFACE_IMPL_1(GregorianCalendar, Calendar, IGregorianCalendar);

constexpr Integer GregorianCalendar::MONTH_LENGTH[];
constexpr Integer GregorianCalendar::LEAP_MONTH_LENGTH[];
constexpr Integer GregorianCalendar::MIN_VALUES[];
constexpr Integer GregorianCalendar::LEAST_MAX_VALUES[];
constexpr Integer GregorianCalendar::MAX_VALUES[];
AutoPtr<IJulianCalendar> GregorianCalendar::sJcal;
SyncObject GregorianCalendar::sJcalLock;
Array<IEra*> GregorianCalendar::sJeras;

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
    /* [out] */ Boolean& same)
{
    IGregorianCalendar* gregory = IGregorianCalendar::Probe(obj);
    if (gregory == nullptr) {
        same = false;
        return NOERROR;
    }
    Calendar::Equals(obj, same);
    if (!same) {
        return NOERROR;
    }
    same = mGregorianCutover == ((GregorianCalendar*)gregory)->mGregorianCutover;
    return NOERROR;
}

ECode GregorianCalendar::GetHashCode(
    /* [out] */ Integer& hash)
{
    Calendar::GetHashCode(hash);
    hash = hash ^ (Integer)mGregorianCutoverDate;
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
    else if (field == MONTH) {
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
    else if (field == ERA) {
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
    return NOERROR;
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
                Integer monthLen; // actual month length
                if (isCutoverYear) {
                    month1 = GetFixedDateMonth1(mCdate, fd);
                    monthLen = ActualMonthLength();
                }
                else {
                    month1 = fd - InternalGet(DAY_OF_MONTH) + 1;
                    ICalendarSystem::Probe(mCalsys)->GetMonthLength(
                            ICalendarDate::Probe(mCdate), &monthLen);
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
                else if (nfd >= (month1 + monthLen)) {
                    nfd = month1 + monthLen - 1;
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
                    Integer monthLen;
                    ICalendarSystem::Probe(mCalsys)->GetMonthLength(
                            ICalendarDate::Probe(mCdate), &monthLen);
                    Integer lastDays = monthLen % 7;
                    max = monthLen / 7;
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
                Integer monthLen = ActualMonthLength();
                Integer lastDays = monthLen % 7;
                max = monthLen / 7;
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
    return NOERROR;
}

ECode GregorianCalendar::GetMinimum(
    /* [in] */ Integer field,
    /* [out] */ Integer* value)
{
    VALIDATE_NOT_NULL(value);

    if (field < 0 || field >= FIELD_COUNT) {
        return como::core::E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    *value = MIN_VALUES[field];
    return NOERROR;
}

ECode GregorianCalendar::GetMaximum(
    /* [in] */ Integer field,
    /* [out] */ Integer* value)
{
    VALIDATE_NOT_NULL(value);

    if (field < 0 || field >= FIELD_COUNT) {
        return como::core::E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

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

    if (field < 0 || field >= FIELD_COUNT) {
        return como::core::E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

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

    if (field < 0 || field >= FIELD_COUNT) {
        return como::core::E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

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

    if (field < 0 || field >= FIELD_COUNT) {
        return como::core::E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

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

    if (field < 0 || field >= FIELD_COUNT) {
        return como::core::E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

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
                Integer monthLen = gc->ActualMonthLength();
                Long monthEnd = gc->GetFixedDateMonth1(gc->mCdate, fd) + monthLen - 1;
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
                    Integer monthLen;
                    ICalendarSystem::Probe(cal)->GetMonthLength(d, &monthLen);
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
                    monthLen -= nDaysFirstWeek + 7 * 3;
                    if (monthLen > 0) {
                        value++;
                        if (monthLen > 7) {
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
            return como::core::E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
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
    Calendar::CloneImpl(other);

    other->mGregorianCutover = mGregorianCutover;
    other->mGregorianCutoverDate = mGregorianCutoverDate;
    other->mGregorianCutoverYear = mGregorianCutoverYear;
    other->mGregorianCutoverYearJulian = mGregorianCutoverYearJulian;
    other->mCalsys = mCalsys;
    other->mZoneOffsets = mZoneOffsets;
    other->mOriginalFields = mOriginalFields;
    other->mCachedFixedDate = mCachedFixedDate;

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

ECode GregorianCalendar::ComputeFields()
{
    Integer mask;
    if (IsPartiallyNormalized()) {
        // Determine which calendar fields need to be computed.
        mask = GetSetStateFields();
        Integer fieldMask = ~mask & ALL_FIELDS;
        // We have to call computTime in case calsys == null in
        // order to set calsys and cdate. (6263644)
        if (fieldMask != 0 || mCalsys == nullptr) {
            mask |= ComputeFields(fieldMask,
                    mask & (ZONE_OFFSET_MASK | DST_OFFSET_MASK));
            CHECK(mask == ALL_FIELDS);
        }
    }
    else {
        mask = ALL_FIELDS;
        ComputeFields(mask, 0);
    }
    // After computing all the fields, set the field state to `COMPUTED'.
    SetFieldsComputed(mask);
    return NOERROR;
}

Integer GregorianCalendar::ComputeFields(
    /* [in] */ Integer fieldMask,
    /* [in] */ Integer tzMask)
{
    Integer zoneOffset = 0;
    AutoPtr<ITimeZone> tz = GetZone();
    if (mZoneOffsets.IsNull()) {
        mZoneOffsets = Array<Integer>(2);
    }
    if (tzMask != (ZONE_OFFSET_MASK | DST_OFFSET_MASK)) {
        if (IZoneInfo::Probe(tz) != nullptr) {
            IZoneInfo* zoneInfo = IZoneInfo::Probe(tz);
            zoneInfo->GetOffsetsByUtcTime(mTime, mZoneOffsets, &zoneOffset);
        }
        else {
            tz->GetOffset(mTime, &zoneOffset);
            tz->GetRawOffset(&mZoneOffsets[0]);
            mZoneOffsets[1] = zoneOffset - mZoneOffsets[0];
        }
    }
    if (tzMask != 0) {
        if (IsFieldSet(tzMask, ZONE_OFFSET)) {
            mZoneOffsets[0] = InternalGet(ZONE_OFFSET);
        }
        if (IsFieldSet(tzMask, DST_OFFSET)) {
            mZoneOffsets[1] = InternalGet(DST_OFFSET);
        }
        zoneOffset = mZoneOffsets[0] + mZoneOffsets[1];
    }

    // By computing time and zoneOffset separately, we can take
    // the wider range of time+zoneOffset than the previous
    // implementation.
    Long fixedDate = zoneOffset / ONE_DAY;
    Integer timeOfDay = zoneOffset % (Integer)ONE_DAY;
    fixedDate += mTime / ONE_DAY;
    timeOfDay += (Integer) (mTime % ONE_DAY);
    if (timeOfDay >= ONE_DAY) {
        timeOfDay -= ONE_DAY;
        ++fixedDate;
    }
    else {
        while (timeOfDay < 0) {
            timeOfDay += ONE_DAY;
            --fixedDate;
        }
    }
    fixedDate += EPOCH_OFFSET;

    Integer era = CE;
    Integer year;
    if (fixedDate >= mGregorianCutoverDate) {
        BLOCK_CHECK() {
            Boolean normalized;
            if (mCachedFixedDate != ILong::MIN_VALUE &&
                    (ICalendarDate::Probe(mGdate)->IsNormalized(&normalized), !normalized)) {
                Logger::E("GregorianCalendar", "cache control: not normalized");
            }
            Integer normYear, month, dayOfMonth;
            Long date;
            if (mCachedFixedDate != ILong::MIN_VALUE &&
                    (mGdate->GetNormalizedYear(&normYear),
                     ICalendarDate::Probe(mGdate)->GetMonth(&month),
                     ICalendarDate::Probe(mGdate)->GetDayOfMonth(&dayOfMonth),
                     IBaseCalendar::Probe(GetGcal())->GetFixedDate(normYear, month, dayOfMonth, mGdate, &date),
                     date == mCachedFixedDate)) {
                Logger::E("GregorianCalendar", "cache control: inconsictency, cachedFixedDate=%lld"
                        ", computed=%lld, date=%s", mCachedFixedDate, date, Object::ToString(mGdate).string());
            }
        }

        // See if we can use gdate to avoid date calculation.
        if (fixedDate != mCachedFixedDate) {
            IBaseCalendar::Probe(GetGcal())->GetCalendarDateFromFixedDate(
                    ICalendarDate::Probe(mGdate), fixedDate);
            mCachedFixedDate = fixedDate;
        }

        ICalendarDate::Probe(mGdate)->GetYear(&year);
        if (year <= 0) {
            year = 1 - year;
            era = BCE;
        }
        mCalsys = IBaseCalendar::Probe(GetGcal());
        mCdate = mGdate;
        BLOCK_CHECK() {
            Integer dayOfWeek;
            ICalendarDate::Probe(mCdate)->GetDayOfWeek(&dayOfWeek);
            if (dayOfWeek <= 0) {
                Logger::E("GregorianCalendar", "dow=%d, date=%s", dayOfWeek, Object::ToString(mCdate).string());
            }
        }
    }
    else {
        // Handle Julian calendar dates.
        mCalsys = GetJulianCalendarSystem();
        AutoPtr<ICalendarDate> date;
        ICalendarSystem::Probe(sJcal)->NewCalendarDate(GetZone(), &date);
        mCdate = IBaseCalendarDate::Probe(date);
        IBaseCalendar::Probe(sJcal)->GetCalendarDateFromFixedDate(date, fixedDate);
        AutoPtr<IEra> e;
        date->GetEra(&e);
        if (e == sJeras[0]) {
            era = BCE;
        }
        date->GetYear(&year);
    }

    // Always set the ERA and YEAR values.
    InternalSet(ERA, era);
    InternalSet(YEAR, year);
    Integer mask = fieldMask | (ERA_MASK | YEAR_MASK);

    Integer month, dayOfMonth;
    ICalendarDate::Probe(mCdate)->GetMonth(&month);
    month = month - 1; // 0-based;
    ICalendarDate::Probe(mCdate)->GetDayOfMonth(&dayOfMonth);

    // Set the basic date fields.
    if ((fieldMask & (MONTH_MASK | DAY_OF_MONTH_MASK | DAY_OF_WEEK_MASK))
            != 0) {
        Integer dayOfWeek;
        ICalendarDate::Probe(mCdate)->GetDayOfWeek(&dayOfWeek);
        InternalSet(MONTH, month);
        InternalSet(DAY_OF_MONTH, dayOfMonth);
        InternalSet(DAY_OF_WEEK, dayOfWeek);
        mask |= MONTH_MASK | DAY_OF_MONTH_MASK | DAY_OF_WEEK_MASK;
    }

    if ((fieldMask & (HOUR_OF_DAY_MASK | AM_PM_MASK | HOUR_MASK
            | MINUTE_MASK | SECOND_MASK | MILLISECOND_MASK)) != 0) {
        if (timeOfDay != 0) {
            Integer hours = timeOfDay / ONE_HOUR;
            InternalSet(HOUR_OF_DAY, hours);
            InternalSet(AM_PM, hours / 12); // Assume AM == 0
            InternalSet(HOUR, hours % 12);
            Integer r = timeOfDay % ONE_HOUR;
            InternalSet(MINUTE, r / ONE_MINUTE);
            r %= ONE_MINUTE;
            InternalSet(SECOND, r / ONE_SECOND);
            InternalSet(MILLISECOND, r % ONE_SECOND);
        }
        else {
            InternalSet(HOUR_OF_DAY, 0);
            InternalSet(AM_PM, AM);
            InternalSet(HOUR, 0);
            InternalSet(MINUTE, 0);
            InternalSet(SECOND, 0);
            InternalSet(MILLISECOND, 0);
        }
        mask |= (HOUR_OF_DAY_MASK | AM_PM_MASK | HOUR_MASK
                | MINUTE_MASK | SECOND_MASK | MILLISECOND_MASK);
    }

    if ((fieldMask & (ZONE_OFFSET_MASK | DST_OFFSET_MASK)) != 0) {
        InternalSet(ZONE_OFFSET, mZoneOffsets[0]);
        InternalSet(DST_OFFSET, mZoneOffsets[1]);
        mask |= (ZONE_OFFSET_MASK | DST_OFFSET_MASK);
    }

    if ((fieldMask & (DAY_OF_YEAR_MASK | WEEK_OF_YEAR_MASK | WEEK_OF_MONTH_MASK | DAY_OF_WEEK_IN_MONTH_MASK)) != 0) {
        Integer normalizedYear;
        mCdate->GetNormalizedYear(&normalizedYear);
        Long fixedDateJan1;
        mCalsys->GetFixedDate(normalizedYear, 1, 1, mCdate, &fixedDateJan1);
        Integer dayOfYear = (Integer)(fixedDate - fixedDateJan1) + 1;
        Long fixedDateMonth1 = fixedDate - dayOfMonth + 1;
        Integer cutoverGap = 0;
        Integer cutoverYear = (mCalsys == IBaseCalendar::Probe(GetGcal())) ? mGregorianCutoverYear : mGregorianCutoverYearJulian;
        Integer relativeDayOfMonth = dayOfMonth - 1;

        // If we are in the cutover year, we need some special handling.
        if (normalizedYear == cutoverYear) {
            // Need to take care of the "missing" days.
            if (mGregorianCutoverYearJulian <= mGregorianCutoverYear) {
                // We need to find out where we are. The cutover
                // gap could even be more than one year.  (One
                // year difference in ~48667 years.)
                fixedDateJan1 = GetFixedDateJan1(mCdate, fixedDate);
                if (fixedDate >= mGregorianCutoverDate) {
                    fixedDateMonth1 = GetFixedDateMonth1(mCdate, fixedDate);
                }
            }
            Integer realDayOfYear = (Integer)(fixedDate - fixedDateJan1) + 1;
            cutoverGap = dayOfYear - realDayOfYear;
            dayOfYear = realDayOfYear;
            relativeDayOfMonth = (Integer)(fixedDate - fixedDateMonth1);
        }
        InternalSet(DAY_OF_YEAR, dayOfYear);
        InternalSet(DAY_OF_WEEK_IN_MONTH, relativeDayOfMonth / 7 + 1);

        Integer weekOfYear = GetWeekNumber(fixedDateJan1, fixedDate);

        // The spec is to calculate WEEK_OF_YEAR in the
        // ISO8601-style. This creates problems, though.
        if (weekOfYear == 0) {
            // If the date belongs to the last week of the
            // previous year, use the week number of "12/31" of
            // the "previous" year. Again, if the previous year is
            // the Gregorian cutover year, we need to take care of
            // it.  Usually the previous day of January 1 is
            // December 31, which is not always true in
            // GregorianCalendar.
            Long fixedDec31 = fixedDateJan1 - 1;
            Long prevJan1  = fixedDateJan1 - 365;
            if (normalizedYear > (cutoverYear + 1)) {
                if (CalendarUtils::IsGregorianLeapYear(normalizedYear - 1)) {
                    --prevJan1;
                }
            }
            else if (normalizedYear <= mGregorianCutoverYearJulian) {
                if (CalendarUtils::IsJulianLeapYear(normalizedYear - 1)) {
                    --prevJan1;
                }
            }
            else {
                AutoPtr<IBaseCalendar> calForJan1 = mCalsys;
                //int prevYear = normalizedYear - 1;
                Integer prevYear;
                GetCalendarDate(fixedDec31)->GetNormalizedYear(&prevYear);
                if (prevYear == mGregorianCutoverYear) {
                    calForJan1 = GetCutoverCalendarSystem();
                    if (calForJan1 == IBaseCalendar::Probe(sJcal)) {
                        calForJan1->GetFixedDate(prevYear,
                                IBaseCalendar::JANUARY, 1, nullptr, &prevJan1);
                    }
                    else {
                        prevJan1 = mGregorianCutoverDate;
                        calForJan1 = IBaseCalendar::Probe(GetGcal());
                    }
                }
                else if (prevYear <= mGregorianCutoverYearJulian) {
                    calForJan1 = GetJulianCalendarSystem();
                    calForJan1->GetFixedDate(prevYear,
                            IBaseCalendar::JANUARY, 1, nullptr, &prevJan1);
                }
            }
            weekOfYear = GetWeekNumber(prevJan1, fixedDec31);
        }
        else {
            if (normalizedYear > mGregorianCutoverYear ||
                    normalizedYear < (mGregorianCutoverYearJulian - 1)) {
                // Regular years
                if (weekOfYear >= 52) {
                    Long nextJan1 = fixedDateJan1 + 365;
                    Boolean leapYear;
                    if (ICalendarDate::Probe(mCdate)->IsLeapYear(&leapYear), leapYear) {
                        nextJan1++;
                    }
                    Integer firstDayOfWeek;
                    GetFirstDayOfWeek(&firstDayOfWeek);
                    Long nextJan1st = BaseCalendar::GetDayOfWeekDateOnOrBefore(nextJan1 + 6,
                            firstDayOfWeek);
                    Integer ndays = (Integer)(nextJan1st - nextJan1);
                    Integer minimalDays;
                    if ((GetMinimalDaysInFirstWeek(&minimalDays), ndays >= minimalDays) && fixedDate >= (nextJan1st - 7)) {
                        // The first days forms a week in which the date is included.
                        weekOfYear = 1;
                    }
                }
            }
            else {
                AutoPtr<IBaseCalendar> calForJan1 = mCalsys;
                Integer nextYear = normalizedYear + 1;
                if (nextYear == (mGregorianCutoverYearJulian + 1) &&
                    nextYear < mGregorianCutoverYear) {
                    // In case the gap is more than one year.
                    nextYear = mGregorianCutoverYear;
                }
                if (nextYear == mGregorianCutoverYear) {
                    calForJan1 = GetCutoverCalendarSystem();
                }

                Long nextJan1;
                if (nextYear > mGregorianCutoverYear
                    || mGregorianCutoverYearJulian == mGregorianCutoverYear
                    || nextYear == mGregorianCutoverYearJulian) {
                    calForJan1->GetFixedDate(nextYear,
                            IBaseCalendar::JANUARY, 1, nullptr, &nextJan1);
                }
                else {
                    nextJan1 = mGregorianCutoverDate;
                    calForJan1 = IBaseCalendar::Probe(GetGcal());
                }

                Integer firstDayOfWeek;
                GetFirstDayOfWeek(&firstDayOfWeek);
                Long nextJan1st = BaseCalendar::GetDayOfWeekDateOnOrBefore(
                        nextJan1 + 6, firstDayOfWeek);
                Integer ndays = (Integer)(nextJan1st - nextJan1);
                Integer minimalDays;
                if ((GetMinimalDaysInFirstWeek(&minimalDays), ndays >= minimalDays) && fixedDate >= (nextJan1st - 7)) {
                    // The first days forms a week in which the date is included.
                    weekOfYear = 1;
                }
            }
        }
        InternalSet(WEEK_OF_YEAR, weekOfYear);
        InternalSet(WEEK_OF_MONTH, GetWeekNumber(fixedDateMonth1, fixedDate));
        mask |= (DAY_OF_YEAR_MASK | WEEK_OF_YEAR_MASK | WEEK_OF_MONTH_MASK | DAY_OF_WEEK_IN_MONTH_MASK);
    }
    return mask;
}

Integer GregorianCalendar::GetWeekNumber(
    /* [in] */ Long fixedDay1,
    /* [in] */ Long fixedDate)
{
    // We can always use `gcal' since Julian and Gregorian are the
    // same thing for this calculation.
    Integer firstDayOfWeek;
    GetFirstDayOfWeek(&firstDayOfWeek);
    Long fixedDay1st = Gregorian::GetDayOfWeekDateOnOrBefore(fixedDay1 + 6, firstDayOfWeek);
    Integer ndays = (Integer)(fixedDay1st - fixedDay1);
    CHECK(ndays <= 7);
    Integer minimalDays;
    if (GetMinimalDaysInFirstWeek(&minimalDays), ndays >= minimalDays) {
        fixedDay1st -= 7;
    }
    Integer normalizedDayOfPeriod = (Integer)(fixedDate - fixedDay1st);
    if (normalizedDayOfPeriod >= 0) {
        return normalizedDayOfPeriod / 7 + 1;
    }
    return CalendarUtils::FloorDivide(normalizedDayOfPeriod, 7) + 1;
}

ECode GregorianCalendar::ComputeTime()
{
    // In non-lenient mode, perform brief checking of calendar
    // fields which have been set externally. Through this
    // checking, the field values are stored in originalFields[]
    // to see if any of them are normalized later.
    Boolean lenient;
    if (IsLenient(&lenient), !lenient) {
        if (mOriginalFields.IsNull()) {
            mOriginalFields = Array<Integer>(FIELD_COUNT);
        }
        for (Integer field = 0; field < FIELD_COUNT; field++) {
            Integer value = InternalGet(field);
            if (IsExternallySet(field)) {
                // Quick validation for any out of range values
                Integer min, max;
                if (value < (GetMinimum(field, &min), min) ||
                        value > (GetMaximum(field, &max), max)) {
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
                }
            }
            mOriginalFields[field] = value;
        }
    }

    // Let the super class determine which calendar fields to be
    // used to calculate the time.
    Integer fieldMask = SelectFields();

    // The year defaults to the epoch start. We don't check
    // fieldMask for YEAR because YEAR is a mandatory field to
    // determine the date.
    Boolean set;
    Integer year = (IsSet(YEAR, &set), set) ? InternalGet(YEAR) : EPOCH_YEAR;

    Integer era = InternalGetEra();
    if (era == BCE) {
        year = 1 - year;
    }
    else if (era != CE) {
        // Even in lenient mode we disallow ERA values other than CE & BCE.
        // (The same normalization rule as add()/roll() could be
        // applied here in lenient mode. But this checking is kept
        // unchanged for compatibility as of 1.5.)
        Logger::E("GregorianCalendar", "Invalid era");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // If year is 0 or negative, we need to set the ERA value later.
    if (year <= 0 && (IsSet(ERA, &set), !set)) {
        fieldMask |= ERA_MASK;
        SetFieldsComputed(ERA_MASK);
    }

    // Calculate the time of day. We rely on the convention that
    // an UNSET field has 0.
    Long timeOfDay = 0;
    if (IsFieldSet(fieldMask, HOUR_OF_DAY)) {
        timeOfDay += (Long)InternalGet(HOUR_OF_DAY);
    }
    else {
        timeOfDay += InternalGet(HOUR);
        // The default value of AM_PM is 0 which designates AM.
        if (IsFieldSet(fieldMask, AM_PM)) {
            timeOfDay += 12 * InternalGet(AM_PM);
        }
    }
    timeOfDay *= 60;
    timeOfDay += InternalGet(MINUTE);
    timeOfDay *= 60;
    timeOfDay += InternalGet(SECOND);
    timeOfDay *= 1000;
    timeOfDay += InternalGet(MILLISECOND);

    // Convert the time of day to the number of days and the
    // millisecond offset from midnight.
    Long fixedDate = timeOfDay / ONE_DAY;
    timeOfDay %= ONE_DAY;
    while (timeOfDay < 0) {
        timeOfDay += ONE_DAY;
        --fixedDate;
    }

    // Calculate the fixed date since January 1, 1 (Gregorian).
    {
        Long gfd, jfd;
        if (year > mGregorianCutoverYear && year > mGregorianCutoverYearJulian) {
            gfd = fixedDate + GetFixedDate(IBaseCalendar::Probe(GetGcal()), year, fieldMask);
            if (gfd >= mGregorianCutoverDate) {
                fixedDate = gfd;
                goto OUT_OF_CALCULATE_FIXED_DATE;
            }
            jfd = fixedDate + GetFixedDate(GetJulianCalendarSystem(), year, fieldMask);
        }
        else if (year < mGregorianCutoverYear && year < mGregorianCutoverYearJulian) {
            jfd = fixedDate + GetFixedDate(GetJulianCalendarSystem(), year, fieldMask);
            if (jfd < mGregorianCutoverDate) {
                fixedDate = jfd;
                goto OUT_OF_CALCULATE_FIXED_DATE;
            }
            gfd = jfd;
        }
        else {
            jfd = fixedDate + GetFixedDate(GetJulianCalendarSystem(), year, fieldMask);
            gfd = fixedDate + GetFixedDate(IBaseCalendar::Probe(GetGcal()), year, fieldMask);
        }

        // Now we have to determine which calendar date it is.

        // If the date is relative from the beginning of the year
        // in the Julian calendar, then use jfd;
        if (IsFieldSet(fieldMask, DAY_OF_YEAR) || IsFieldSet(fieldMask, WEEK_OF_YEAR)) {
            if (mGregorianCutoverYear == mGregorianCutoverYearJulian) {
                fixedDate = jfd;
                goto OUT_OF_CALCULATE_FIXED_DATE;
            }
            else if (year == mGregorianCutoverYear) {
                fixedDate = gfd;
                goto OUT_OF_CALCULATE_FIXED_DATE;
            }
        }

        if (gfd >= mGregorianCutoverDate) {
            if (jfd >= mGregorianCutoverDate) {
                fixedDate = gfd;
            }
            else {
                // The date is in an "overlapping" period. No way
                // to disambiguate it. Determine it using the
                // previous date calculation.
                if (mCalsys == IBaseCalendar::Probe(GetGcal()) || mCalsys == nullptr) {
                    fixedDate = gfd;
                }
                else {
                    fixedDate = jfd;
                }
            }
        }
        else {
            if (jfd < mGregorianCutoverDate) {
                fixedDate = jfd;
            }
            else {
                // The date is in a "missing" period.
                if (IsLenient(&lenient), !lenient) {
                    Logger::E("GregorianCalendar", "the specified date doesn't exist");
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
                }
                // Take the Julian date for compatibility, which
                // will produce a Gregorian date.
                fixedDate = jfd;
            }
        }
    }

OUT_OF_CALCULATE_FIXED_DATE:
    // millis represents local wall-clock time in milliseconds.
    Long millis = (fixedDate - EPOCH_OFFSET) * ONE_DAY + timeOfDay;

    // Compute the time zone offset and DST offset.  There are two potential
    // ambiguities here.  We'll assume a 2:00 am (wall time) switchover time
    // for discussion purposes here.
    // 1. The transition into DST.  Here, a designated time of 2:00 am - 2:59 am
    //    can be in standard or in DST depending.  However, 2:00 am is an invalid
    //    representation (the representation jumps from 1:59:59 am Std to 3:00:00 am DST).
    //    We assume standard time.
    // 2. The transition out of DST.  Here, a designated time of 1:00 am - 1:59 am
    //    can be in standard or DST.  Both are valid representations (the rep
    //    jumps from 1:59:59 DST to 1:00:00 Std).
    //    Again, we assume standard time.
    // We use the TimeZone object, unless the user has explicitly set the ZONE_OFFSET
    // or DST_OFFSET fields; then we use those fields.
    AutoPtr<ITimeZone> zone = GetZone();
    // BEGIN Android-changed: time zone related calculation via helper methods
    Integer tzMask = fieldMask & (ZONE_OFFSET_MASK | DST_OFFSET_MASK);

    millis = AdjustForZoneAndDaylightSavingsTime(tzMask, millis, zone);
    // END Android-changed: time zone related calculation via helper methods

    // Set this calendar's time in milliseconds
    mTime = millis;

    Integer mask = ComputeFields(fieldMask | GetSetStateFields(), tzMask);

    if (IsLenient(&lenient), !lenient) {
        for (Integer field = 0; field < FIELD_COUNT; field++) {
            if (!IsExternallySet(field)) {
                continue;
            }
            if (mOriginalFields[field] != InternalGet(field)) {
                String s = String::Format("%d -> %d", mOriginalFields[field], InternalGet(field));
                // Restore the original field values
                mFields.Copy(mOriginalFields, 0, mFields.GetLength());
                Logger::E("GregorianCalendar", "%s: %s", GetFieldName(field).string(), s.string());
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }
    }
    SetFieldsNormalized(mask);
    return NOERROR;
}

Long GregorianCalendar::AdjustForZoneAndDaylightSavingsTime(
    /* [in] */ Integer tzMask,
    /* [in] */ Long utcTimeInMillis,
    /* [in] */ ITimeZone* zone)
{
    // The following don't actually need to be initialized because they are always set before
    // they are used but the compiler cannot detect that.
    Integer zoneOffset = 0;
    Integer dstOffset = 0;

    // If either of the ZONE_OFFSET or DST_OFFSET fields are not set then get the information
    // from the TimeZone.
    if (tzMask != (ZONE_OFFSET_MASK | DST_OFFSET_MASK)) {
        if (mZoneOffsets.IsNull()) {
            mZoneOffsets = Array<Integer>(2);
        }
        Integer gmtOffset;
        if (IsFieldSet(tzMask, ZONE_OFFSET)) {
            gmtOffset = InternalGet(ZONE_OFFSET);
        }
        else {
            zone->GetRawOffset(&gmtOffset);
        }

        // Calculate the standard time (no DST) in the supplied zone. This is a ballpark figure
        // and not used in the final calculation as the offset used here may not be the same as
        // the actual offset the time zone requires be used for this time. This is to handle
        // situations like Honolulu, where its raw offset changed from GMT-10:30 to GMT-10:00
        // in 1947. The TimeZone always uses a raw offset of -10:00 but will return -10:30
        // for dates before the change over.
        Long standardTimeInZone = utcTimeInMillis - gmtOffset;

        // Retrieve the correct zone and DST offsets from the time zone.
        if (IZoneInfo::Probe(zone) != nullptr) {
            // Android-changed: libcore ZoneInfo uses different method to get offsets.
            IZoneInfo* zoneInfo = IZoneInfo::Probe(zone);
            zoneInfo->GetOffsetsByUtcTime(standardTimeInZone, mZoneOffsets);
        }
        else {
            TimeZone::From(zone)->GetOffsets(standardTimeInZone, mZoneOffsets);
        }
        zoneOffset = mZoneOffsets[0];
        dstOffset = mZoneOffsets[1];

        // If necessary adjust the DST offset to handle an invalid wall clock sensibly.
        dstOffset = AdjustDstOffsetForInvalidWallClock(standardTimeInZone, zone, dstOffset);
    }

    // If either ZONE_OFFSET of DST_OFFSET fields are set then get the information from the
    // fields, potentially overriding information from the TimeZone.
    if (tzMask != 0) {
        if (IsFieldSet(tzMask, ZONE_OFFSET)) {
            zoneOffset = InternalGet(ZONE_OFFSET);
        }
        if (IsFieldSet(tzMask, DST_OFFSET)) {
            dstOffset = InternalGet(DST_OFFSET);
        }
    }

    // Adjust the time zone offset values to get the UTC time.
    Long standardTimeInZone = utcTimeInMillis - zoneOffset;
    return standardTimeInZone - dstOffset;
}

Integer GregorianCalendar::AdjustDstOffsetForInvalidWallClock(
    /* [in] */ Long standardTimeInZone,
    /* [in] */ ITimeZone* zone,
    /* [in] */ Integer dstOffset)
{
    if (dstOffset != 0) {
        // If applying the DST offset produces a time that is outside DST then it must be
        // an invalid wall clock so clear the DST offset to avoid that happening.
        AutoPtr<IDate> date;
        CDate::New(standardTimeInZone - dstOffset, IID_IDate, (IInterface**)&date);
        Boolean daylight;
        if (zone->InDaylightTime(date, &daylight), !daylight) {
            dstOffset = 0;
        }
    }
    return dstOffset;
}

Long GregorianCalendar::GetFixedDate(
    /* [in] */ IBaseCalendar* cal,
    /* [in] */ Integer year,
    /* [in] */ Integer fieldMask)
{
    Integer month = JANUARY;
    if (IsFieldSet(fieldMask, MONTH)) {
        // No need to check if MONTH has been set (no isSet(MONTH)
        // call) since its unset value happens to be JANUARY (0).
        month = InternalGet(MONTH);

        // If the month is out of range, adjust it into range
        if (month > DECEMBER) {
            year += month / 12;
            month %= 12;
        }
        else if (month < JANUARY) {
            Array<Integer> rem(1);
            year += CalendarUtils::FloorDivide(month, 12, rem);
            month = rem[0];
        }
    }

    // Get the fixed date since Jan 1, 1 (Gregorian). We are on
    // the first day of either `month' or January in 'year'.
    Long fixedDate;
    cal->GetFixedDate(year, month + 1, 1,
            cal == IBaseCalendar::Probe(GetGcal()) ? mGdate : nullptr, &fixedDate);
    if (IsFieldSet(fieldMask, MONTH)) {
        // Month-based calculations
        if (IsFieldSet(fieldMask, DAY_OF_MONTH)) {
            // We are on the first day of the month. Just add the
            // offset if DAY_OF_MONTH is set. If the isSet call
            // returns false, that means DAY_OF_MONTH has been
            // selected just because of the selected
            // combination. We don't need to add any since the
            // default value is the 1st.
            Boolean set;
            if (IsSet(DAY_OF_MONTH, &set), set) {
                // To avoid underflow with DAY_OF_MONTH-1, add
                // DAY_OF_MONTH, then subtract 1.
                fixedDate += InternalGet(DAY_OF_MONTH);
                fixedDate--;
            }
        }
        else {
            if (IsFieldSet(fieldMask, WEEK_OF_MONTH)) {
                Integer dateValue;
                GetFirstDayOfWeek(&dateValue);
                Long firstDayOfWeek = BaseCalendar::GetDayOfWeekDateOnOrBefore(fixedDate + 6,
                        dateValue);
                // If we have enough days in the first week, then
                // move to the previous week.
                if ((firstDayOfWeek - fixedDate) >= (GetMinimalDaysInFirstWeek(&dateValue), dateValue)) {
                    firstDayOfWeek -= 7;
                }
                if (IsFieldSet(fieldMask, DAY_OF_WEEK)) {
                    firstDayOfWeek = BaseCalendar::GetDayOfWeekDateOnOrBefore(firstDayOfWeek + 6,
                            InternalGet(DAY_OF_WEEK));
                }
                // In lenient mode, we treat days of the previous
                // months as a part of the specified
                // WEEK_OF_MONTH. See 4633646.
                fixedDate = firstDayOfWeek + 7 * (InternalGet(WEEK_OF_MONTH) - 1);
            }
            else {
                Integer dayOfWeek;
                if (IsFieldSet(fieldMask, DAY_OF_WEEK)) {
                    dayOfWeek = InternalGet(DAY_OF_WEEK);
                }
                else {
                    GetFirstDayOfWeek(&dayOfWeek);
                }
                // We are basing this on the day-of-week-in-month.  The only
                // trickiness occurs if the day-of-week-in-month is
                // negative.
                Integer dowim;
                if (IsFieldSet(fieldMask, DAY_OF_WEEK_IN_MONTH)) {
                    dowim = InternalGet(DAY_OF_WEEK_IN_MONTH);
                }
                else {
                    dowim = 1;
                }
                if (dowim >= 0) {
                    fixedDate = BaseCalendar::GetDayOfWeekDateOnOrBefore(fixedDate + (7 * dowim) - 1,
                            dayOfWeek);
                }
                else {
                    // Go to the first day of the next week of
                    // the specified week boundary.
                    Integer lastDate = MonthLength(month, year) + (7 * (dowim + 1));
                    // Then, get the day of week date on or before the last date.
                    fixedDate = BaseCalendar::GetDayOfWeekDateOnOrBefore(fixedDate + lastDate - 1,
                            dayOfWeek);
                }
            }
        }
    }
    else {
        if (year == mGregorianCutoverYear && cal == IBaseCalendar::Probe(GetGcal())
            && fixedDate < mGregorianCutoverDate
            && mGregorianCutoverYear != mGregorianCutoverYearJulian) {
            // January 1 of the year doesn't exist.  Use
            // gregorianCutoverDate as the first day of the
            // year.
            fixedDate = mGregorianCutoverDate;
        }
        // We are on the first day of the year.
        if (IsFieldSet(fieldMask, DAY_OF_YEAR)) {
            // Add the offset, then subtract 1. (Make sure to avoid underflow.)
            fixedDate += InternalGet(DAY_OF_YEAR);
            fixedDate--;
        }
        else {
            Integer dateValue;
            GetFirstDayOfWeek(&dateValue);
            Long firstDayOfWeek = BaseCalendar::GetDayOfWeekDateOnOrBefore(fixedDate + 6,
                    dateValue);
            // If we have enough days in the first week, then move
            // to the previous week.
            if ((firstDayOfWeek - fixedDate) >= (GetMinimalDaysInFirstWeek(&dateValue), dateValue)) {
                firstDayOfWeek -= 7;
            }
            if (IsFieldSet(fieldMask, DAY_OF_WEEK)) {
                Integer dayOfWeek = InternalGet(DAY_OF_WEEK);
                if (dayOfWeek != (GetFirstDayOfWeek(&dateValue), dateValue)) {
                    firstDayOfWeek = BaseCalendar::GetDayOfWeekDateOnOrBefore(firstDayOfWeek + 6,
                            dayOfWeek);
                }
            }
            fixedDate = firstDayOfWeek + 7 * ((Long)InternalGet(WEEK_OF_YEAR) - 1);
        }
    }

    return fixedDate;
}

AutoPtr<GregorianCalendar> GregorianCalendar::GetNormalizedCalendar()
{
    AutoPtr<GregorianCalendar> gc;
    if (IsFullyNormalized()) {
        gc = this;
    }
    else {
        // Create a clone and normalize the calendar fields
        AutoPtr<IGregorianCalendar> clone;
        Clone(IID_IGregorianCalendar, (IInterface**)&clone);
        gc = (GregorianCalendar*)clone.Get();
        gc->SetLenient(true);
        gc->Complete();
    }
    return gc;
}

AutoPtr<IBaseCalendar> GregorianCalendar::GetJulianCalendarSystem()
{
    AutoLock lock(sJcalLock);
    if (sJcal == nullptr) {
        AutoPtr<ICalendarSystem> cs;
        CalendarSystem::ForName(String("julian"), &cs);
        sJcal = IJulianCalendar::Probe(cs);
        cs->GetEras(&sJeras);
    }
    return IBaseCalendar::Probe(sJcal);
}

AutoPtr<IBaseCalendar> GregorianCalendar::GetCutoverCalendarSystem()
{
    if (mGregorianCutoverYearJulian < mGregorianCutoverYear) {
        return IBaseCalendar::Probe(GetGcal());
    }
    return GetJulianCalendarSystem();
}

Boolean GregorianCalendar::IsCutoverYear(
    /* [in] */ Integer normalizedYear)
{
    int cutoverYear = (mCalsys == IBaseCalendar::Probe(GetGcal())) ? mGregorianCutoverYear : mGregorianCutoverYearJulian;
    return normalizedYear == cutoverYear;
}

Long GregorianCalendar::GetFixedDateJan1(
    /* [in] */ IBaseCalendarDate* date,
    /* [in] */ Long fixedDate)
{
    BLOCK_CHECK() {
        Integer normYear;
        date->GetNormalizedYear(&normYear);
        CHECK(normYear == mGregorianCutoverYear ||
                normYear == mGregorianCutoverYearJulian);
    }
    if (mGregorianCutoverYear != mGregorianCutoverYearJulian) {
        if (fixedDate >= mGregorianCutoverDate) {
            // Dates before the cutover date don't exist
            // in the same (Gregorian) year. So, no
            // January 1 exists in the year. Use the
            // cutover date as the first day of the year.
            return mGregorianCutoverDate;
        }
    }
    // January 1 of the normalized year should exist.
    AutoPtr<IBaseCalendar> juliancal = GetJulianCalendarSystem();
    Integer normYear;
    date->GetNormalizedYear(&normYear);
    Long dateValue;
    juliancal->GetFixedDate(normYear, IBaseCalendar::JANUARY, 1, nullptr, &dateValue);
    return dateValue;
}

Long GregorianCalendar::GetFixedDateMonth1(
    /* [in] */ IBaseCalendarDate* date,
    /* [in] */ Long fixedDate)
{
    BLOCK_CHECK() {
        Integer normYear;
        date->GetNormalizedYear(&normYear);
        CHECK(normYear == mGregorianCutoverYear ||
                normYear == mGregorianCutoverYearJulian);
    }
    AutoPtr<ICalendarDate> gCutover = ICalendarDate::Probe(GetGregorianCutoverDate());
    Integer month, dayOfMonth;
    if ((gCutover->GetMonth(&month), month == IBaseCalendar::JANUARY)
        && (gCutover->GetDayOfMonth(&dayOfMonth), dayOfMonth == 1)) {
        // The cutover happened on January 1.
        ICalendarDate::Probe(date)->GetDayOfMonth(&dayOfMonth);
        return fixedDate - dayOfMonth + 1;
    }

    Long fixedDateMonth1;
    // The cutover happened sometime during the year.
    Integer month1;
    if (ICalendarDate::Probe(date)->GetMonth(&month1), month1 == month) {
        // The cutover happened in the month.
        AutoPtr<IBaseCalendarDate> jLastDate = GetLastJulianDate();
        Integer month2;
        if (mGregorianCutoverYear == mGregorianCutoverYearJulian
            && (ICalendarDate::Probe(jLastDate)->GetMonth(&month2), month == month2)) {
            // The "gap" fits in the same month.
            Integer normYear;
            date->GetNormalizedYear(&normYear);
            IBaseCalendar::Probe(sJcal)->GetFixedDate(normYear, month1, 1, nullptr, &fixedDateMonth1);
        }
        else {
            // Use the cutover date as the first day of the month.
            fixedDateMonth1 = mGregorianCutoverDate;
        }
    }
    else {
        // The cutover happened before the month.
        ICalendarDate::Probe(date)->GetDayOfMonth(&dayOfMonth);
        fixedDateMonth1 = fixedDate - dayOfMonth + 1;
    }

    return fixedDateMonth1;
}

AutoPtr<IBaseCalendarDate> GregorianCalendar::GetCalendarDate(
    /* [in] */ Long fd)
{
    AutoPtr<IBaseCalendar> cal = (fd >= mGregorianCutoverDate) ?
            IBaseCalendar::Probe(GetGcal()) : GetJulianCalendarSystem().Get();
    AutoPtr<ICalendarDate> d;
    ICalendarSystem::Probe(cal)->NewCalendarDate(TimeZone::NO_TIMEZONE, &d);
    cal->GetCalendarDateFromFixedDate(d, fd);
    return IBaseCalendarDate::Probe(d);
}

AutoPtr<IBaseCalendarDate> GregorianCalendar::GetGregorianCutoverDate()
{
    return GetCalendarDate(mGregorianCutoverDate);
}

AutoPtr<IBaseCalendarDate> GregorianCalendar::GetLastJulianDate()
{
    return GetCalendarDate(mGregorianCutoverDate - 1);
}

Integer GregorianCalendar::MonthLength(
    /* [in] */ Integer month,
    /* [in] */ Integer year)
{
    Boolean leap;
    return (IsLeapYear(year, &leap), leap) ? LEAP_MONTH_LENGTH[month] : MONTH_LENGTH[month];
}

Integer GregorianCalendar::MonthLength(
    /* [in] */ Integer month)
{
    Integer year = InternalGet(YEAR);
    if (InternalGetEra() == BCE) {
        year = 1 - year;
    }
    return MonthLength(month, year);
}

Integer GregorianCalendar::ActualMonthLength()
{
    Integer year;
    mCdate->GetNormalizedYear(&year);
    if (year != mGregorianCutoverYear && year != mGregorianCutoverYearJulian) {
        Integer monthLen;
        ICalendarSystem::Probe(mCalsys)->GetMonthLength(
                ICalendarDate::Probe(mCdate), &monthLen);
        return monthLen;
    }
    AutoPtr<ICalendarDate> date;
    ICloneable::Probe(mCdate)->Clone(IID_ICalendarDate, (IInterface**)&date);
    Long fd;
    mCalsys->GetFixedDate(date, &fd);
    Long month1 = GetFixedDateMonth1(IBaseCalendarDate::Probe(date), fd);
    Integer monthLen;
    ICalendarSystem::Probe(mCalsys)->GetMonthLength(date, &monthLen);
    Long next1 = month1 + monthLen;
    if (next1 < mGregorianCutoverDate) {
        return (Integer)(next1 - month1);
    }
    if (mCdate != mGdate) {
        date = nullptr;
        ICalendarSystem::Probe(GetGcal())->NewCalendarDate(TimeZone::NO_TIMEZONE, &date);
    }
    IBaseCalendar::Probe(GetGcal())->GetCalendarDateFromFixedDate(date, next1);
    next1 = GetFixedDateMonth1(IBaseCalendarDate::Probe(date), next1);
    return (Integer)(next1 - month1);
}

void GregorianCalendar::PinDayOfMonth()
{
    Integer year = InternalGet(YEAR);
    Integer monthLen;
    if (year > mGregorianCutoverYear || year < mGregorianCutoverYearJulian) {
        monthLen = MonthLength(InternalGet(MONTH));
    }
    else {
        AutoPtr<GregorianCalendar> gc = GetNormalizedCalendar();
        gc->GetActualMaximum(DAY_OF_MONTH, &monthLen);
    }
    Integer dom = InternalGet(DAY_OF_MONTH);
    if (dom > monthLen) {
        Set(DAY_OF_MONTH, monthLen);
    }
}

Long GregorianCalendar::GetCurrentFixedDate()
{
    Long date;
    return (mCalsys == IBaseCalendar::Probe(GetGcal())) ?
            mCachedFixedDate : (mCalsys->GetFixedDate(ICalendarDate::Probe(mCdate), &date), date);
}

Integer GregorianCalendar::GetRolledValue(
    /* [in] */ Integer value,
    /* [in] */ Integer amount,
    /* [in] */ Integer min,
    /* [in] */ Integer max)
{
    CHECK(value >= min && value <= max);
    Integer range = max - min + 1;
    amount %= range;
    Integer n = value + amount;
    if (n > max) {
        n -= range;
    }
    else if (n < min) {
        n += range;
    }
    CHECK(n >= min && n <= max);
    return n;
}

Integer GregorianCalendar::InternalGetEra()
{
    Boolean set;
    return (IsSet(ERA, &set), set) ? InternalGet(ERA) : CE;
}

}
}

