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
#include "ccm/util/CDate.h"
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

            }

        case DAY_OF_MONTH:
            {

            }

        case DAY_OF_YEAR:
            {

            }

        case DAY_OF_WEEK:
            {

            }

        case DAY_OF_WEEK_IN_MONTH:
            {

            }
    }

    Set(field, GetRolledValue(InternalGet(field), amount, min, max));
}

}
}

