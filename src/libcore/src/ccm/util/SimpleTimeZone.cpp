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

#include "ccm/core/AutoLock.h"
#include "ccm/util/GregorianCalendar.h"
#include "ccm/util/SimpleTimeZone.h"
#include "ccm/util/calendar/CalendarSystem.h"
#include "ccm/util/calendar/CalendarUtils.h"
#include <ccmlogger.h>

using ccm::core::AutoLock;
using ccm::util::GregorianCalendar;
using ccm::util::calendar::CalendarSystem;
using ccm::util::calendar::CalendarUtils;

namespace ccm {
namespace util {

CCM_INTERFACE_IMPL_1(SimpleTimeZone, TimeZone, ISimpleTimeZone);

ECode SimpleTimeZone::Constructor(
    /* [in] */ Integer rawOffset,
    /* [in] */ const String& id)
{
    mRawOffset = rawOffset;
    FAIL_RETURN(SetID(id));
    mDstSavings = mMillisPerHour;
    return NOERROR;
}

ECode SimpleTimeZone::Constructor(
    /* [in] */ Integer rawOffset,
    /* [in] */ const String& id,
    /* [in] */ Integer startMonth,
    /* [in] */ Integer startDay,
    /* [in] */ Integer startDayOfWeek,
    /* [in] */ Integer startTime,
    /* [in] */ Integer endMonth,
    /* [in] */ Integer endDay,
    /* [in] */ Integer endDayOfWeek,
    /* [in] */ Integer endTime)
{
    return Constructor(rawOffset, id, startMonth, startDay,
            startDayOfWeek, startTime, WALL_TIME, endMonth,
            endDay, endDayOfWeek, endTime, WALL_TIME, mMillisPerHour);
}

ECode SimpleTimeZone::Constructor(
    /* [in] */ Integer rawOffset,
    /* [in] */ const String& id,
    /* [in] */ Integer startMonth,
    /* [in] */ Integer startDay,
    /* [in] */ Integer startDayOfWeek,
    /* [in] */ Integer startTime,
    /* [in] */ Integer endMonth,
    /* [in] */ Integer endDay,
    /* [in] */ Integer endDayOfWeek,
    /* [in] */ Integer endTime,
    /* [in] */ Integer dstSavings)
{
    return Constructor(rawOffset, id, startMonth, startDay,
            startDayOfWeek, startTime, WALL_TIME, endMonth,
            endDay, endDayOfWeek, endTime, WALL_TIME, dstSavings);
}

ECode SimpleTimeZone::Constructor(
    /* [in] */ Integer rawOffset,
    /* [in] */ const String& id,
    /* [in] */ Integer startMonth,
    /* [in] */ Integer startDay,
    /* [in] */ Integer startDayOfWeek,
    /* [in] */ Integer startTime,
    /* [in] */ Integer startTimeMode,
    /* [in] */ Integer endMonth,
    /* [in] */ Integer endDay,
    /* [in] */ Integer endDayOfWeek,
    /* [in] */ Integer endTime,
    /* [in] */ Integer endTimeMode,
    /* [in] */ Integer dstSavings)
{
    FAIL_RETURN(SetID(id));
    mRawOffset = rawOffset;
    mStartMonth = startMonth;
    mStartDay = startDay;
    mStartDayOfWeek = startDayOfWeek;
    mStartTime = startTime;
    mStartTimeMode = startTimeMode;
    mEndMonth = endMonth;
    mEndDay = endDay;
    mEndDayOfWeek = endDayOfWeek;
    mEndTime = endTime;
    mEndTimeMode = endTimeMode;
    mDstSavings = dstSavings;

    FAIL_RETURN(DecodeRules());
    if (dstSavings <= 0) {
        Logger::E("SimpleTimeZone", "Illegal daylight saving value: %d", dstSavings);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode SimpleTimeZone::SetStartYear(
    /* [in] */ Integer year)
{
    mStartYear = year;
    InvalidateCache();
    return NOERROR;
}

ECode SimpleTimeZone::SetStartRule(
    /* [in] */ Integer startMonth,
    /* [in] */ Integer startDay,
    /* [in] */ Integer startDayOfWeek,
    /* [in] */ Integer startTime)
{
    mStartMonth = startMonth;
    mStartDay = startDay;
    mStartDayOfWeek = startDayOfWeek;
    mStartTime = startTime;
    mStartTimeMode = WALL_TIME;
    FAIL_RETURN(DecodeStartRule());
    InvalidateCache();
    return NOERROR;
}

ECode SimpleTimeZone::SetStartRule(
    /* [in] */ Integer startMonth,
    /* [in] */ Integer startDay,
    /* [in] */ Integer startTime)
{
    return SetStartRule(startMonth, startDay, 0, startTime);
}

ECode SimpleTimeZone::SetStartRule(
    /* [in] */ Integer startMonth,
    /* [in] */ Integer startDay,
    /* [in] */ Integer startDayOfWeek,
    /* [in] */ Integer startTime,
    /* [in] */ Boolean after)
{
    if (after) {
        return SetStartRule(startMonth, startDay, -startDayOfWeek, startTime);
    }
    else {
        return SetStartRule(startMonth, -startDay, -startDayOfWeek, startTime);
    }
}

ECode SimpleTimeZone::SetEndRule(
    /* [in] */ Integer endMonth,
    /* [in] */ Integer endDay,
    /* [in] */ Integer endDayOfWeek,
    /* [in] */ Integer endTime)
{
    mEndMonth = endMonth;
    mEndDay = endDay;
    mEndDayOfWeek = endDayOfWeek;
    mEndTime = endTime;
    mEndTimeMode = WALL_TIME;
    FAIL_RETURN(DecodeEndRule());
    InvalidateCache();
    return NOERROR;
}

ECode SimpleTimeZone::SetEndRule(
    /* [in] */ Integer endMonth,
    /* [in] */ Integer endDay,
    /* [in] */ Integer endTime)
{
    return SetEndRule(endMonth, endDay, 0, endTime);
}

ECode SimpleTimeZone::SetEndRule(
    /* [in] */ Integer endMonth,
    /* [in] */ Integer endDay,
    /* [in] */ Integer endDayOfWeek,
    /* [in] */ Integer endTime,
    /* [in] */ Boolean after)
{
    if (after) {
        return SetEndRule(endMonth, endDay, -endDayOfWeek, endTime);
    }
    else {
        return SetEndRule(endMonth, -endDay, -endDayOfWeek, endTime);
    }
}

ECode SimpleTimeZone::GetOffset(
    /* [in] */ Long date,
    /* [out] */ Integer* offset)
{
    VALIDATE_NOT_NULL(offset);

    *offset = GetOffsets(date, Array<Integer>::Null());
    return NOERROR;
}

Integer SimpleTimeZone::GetOffsets(
    /* [in] */ Long date,
    /* [in] */ Array<Integer>&& offsets)
{
    Integer offset = mRawOffset;

    if (mUseDaylight) {
        {
            AutoLock lock(this);
            if (mCacheStart != 0) {
                if (date >= mCacheStart && date < mCacheEnd) {
                    offset += mDstSavings;
                    goto NEXT;
                }
            }
        }
        AutoPtr<ICalendarSystem> cal;
        if (date >= GregorianCalendar::DEFAULT_GREGORIAN_CUTOVER) {
            cal = ICalendarSystem::Probe(GetGcal());
        }
        else {
            AutoPtr<ICalendarSystem> cs;
            CalendarSystem::ForName(String("julian"), &cs);
            cal = cs;
        }
        AutoPtr<ICalendarDate> cdate;
        cal->NewCalendarDate(TimeZone::NO_TIMEZONE, &cdate);
        // Get the year in local time
        cal->GetCalendarDate(date + mRawOffset, cdate);
        Integer year;
        IBaseCalendarDate::Probe(cdate)->GetNormalizedYear(&year);
        if (year >= mStartYear) {
            // Clear time elements for the transition calculations
            cdate->SetTimeOfDay(0, 0, 0, 0);
            offset = GetOffset(cal, cdate, year, date);
        }
    }

NEXT:
    if (!offsets.IsNull()) {
        offsets[0] = mRawOffset;
        offsets[1] = offset - mRawOffset;
    }
    return offset;
}

ECode SimpleTimeZone::GetOffset(
    /* [in] */ Integer era,
    /* [in] */ Integer year,
    /* [in] */ Integer month,
    /* [in] */ Integer day,
    /* [in] */ Integer dayOfWeek,
    /* [in] */ Integer milliseconds,
    /* [out] */ Integer* offset)
{
    VALIDATE_NOT_NULL(offset);

    if (era != IGregorianCalendar::AD && era != IGregorianCalendar::BC) {
        Logger::E("SimpleTimeZone", "Illegal era %d", era);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer y = year;
    if (era == IGregorianCalendar::BC) {
        // adjust y with the GregorianCalendar-style year numbering.
        y = 1 - y;
    }

    // If the year isn't representable with the 64-bit long
    // integer in milliseconds, convert the year to an
    // equivalent year.
    if (y >= 292278994) {
            y = 2800 + y % 2800;
    }
    else if (y <= -292269054) {
        // y %= 28 also produces an equivalent year, but positive
        // year numbers would be convenient to use the UNIX cal
        // command.
        y = (Integer)CalendarUtils::Mod((Long)y, (Long)28);
    }

    // convert year to its 1-based month value
    Integer m = month + 1;

    // First, calculate time as a Gregorian date.
    AutoPtr<ICalendarSystem> cal = ICalendarSystem::Probe(GetGcal());
    AutoPtr<ICalendarDate> cdate;
    cal->NewCalendarDate(TimeZone::NO_TIMEZONE, &cdate);
    cdate->SetDate(y, m, day);
    Long time;
    cal->GetTime(cdate, &time); // normalize cdate
    time += milliseconds - mRawOffset; // UTC time

    // If the time value represents a time before the default
    // Gregorian cutover, recalculate time using the Julian
    // calendar system. For the Julian calendar system, the
    // normalized year numbering is ..., -2 (BCE 2), -1 (BCE 1),
    // 1, 2 ... which is different from the GregorianCalendar
    // style year numbering (..., -1, 0 (BCE 1), 1, 2, ...).
    if (time < GregorianCalendar::DEFAULT_GREGORIAN_CUTOVER) {
        AutoPtr<ICalendarSystem> cs;
        CalendarSystem::ForName(String("julian"), &cs);
        cal = cs;
        cdate = nullptr;
        cs->NewCalendarDate(TimeZone::NO_TIMEZONE, &cdate);
        IBaseCalendarDate::Probe(cdate)->SetNormalizedDate(y, m, day);
        cs->GetTime(cdate, &time);
        time = time + milliseconds - mRawOffset;
    }

    Integer yy, mm, dd;
    if ((IBaseCalendarDate::Probe(cdate)->GetNormalizedYear(&yy), yy != y)
            || (cdate->GetMonth(&mm), mm != m)
            || (cdate->GetDayOfMonth(&dd), dd != day)
            // The validation should be cdate.getDayOfWeek() ==
            // dayOfWeek. However, we don't check dayOfWeek for
            // compatibility.
            || (dayOfWeek < ICalendar::SUNDAY || dayOfWeek > ICalendar::SATURDAY)
            || (milliseconds < 0 || milliseconds >= (24 * 60 * 60 * 1000))) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (!mUseDaylight || year < mStartYear || era != GregorianCalendar::CE) {
        *offset = mRawOffset;
        return NOERROR;
    }

    *offset = GetOffset(cal, cdate, y, time);
    return NOERROR;
}

Integer SimpleTimeZone::GetOffset(
    /* [in] */ ICalendarSystem* cal,
    /* [in] */ ICalendarDate* cdate,
    /* [in] */ Integer year,
    /* [in] */ Long time)
{
    {
        AutoLock lock(this);

        if (mCacheStart != 0) {
            if (time >= mCacheStart && time < mCacheEnd) {
                return mRawOffset + mDstSavings;
            }
            if (year == mCacheYear) {
                return mRawOffset;
            }
        }
    }

    Long start = GetStart(cal, cdate, year);
    Long end = GetEnd(cal, cdate, year);
    Integer offset = mRawOffset;
    if (start <= end) {
        if (time >= start && time < end) {
            offset += mDstSavings;
        }
        {
            AutoLock lock(this);

            mCacheYear = year;
            mCacheStart = start;
            mCacheEnd = end;
        }
    }
    else {
        if (time < end) {
            // TODO: support Gregorian cutover. The previous year
            // may be in the other calendar system.
            start = GetStart(cal, cdate, year - 1);
            if (time >= start) {
                offset += mDstSavings;
            }
        }
        else if (time >= start) {
            // TODO: support Gregorian cutover. The next year
            // may be in the other calendar system.
            end = GetEnd(cal, cdate, year + 1);
            if (time < end) {
                offset += mDstSavings;
            }
        }
        if (start <= end) {
            {
                AutoLock lock(this);

                // The start and end transitions are in multiple years.
                mCacheYear = (Long) mStartYear - 1;
                mCacheStart = start;
                mCacheEnd = end;
            }
        }
    }
    return offset;
}

Long SimpleTimeZone::GetStart(
    /* [in] */ ICalendarSystem* cal,
    /* [in] */ ICalendarDate* cdate,
    /* [in] */ Integer year)
{
    Integer time = mStartTime;
    if (mStartTimeMode != UTC_TIME) {
        time -= mRawOffset;
    }
    return GetTransition(cal, cdate, mStartMode, year, mStartMonth,
            mStartDay, mStartDayOfWeek, time);
}

Long SimpleTimeZone::GetEnd(
    /* [in] */ ICalendarSystem* cal,
    /* [in] */ ICalendarDate* cdate,
    /* [in] */ Integer year)
{
    Integer time = mEndTime;
    if (mEndTimeMode != UTC_TIME) {
        time -= mRawOffset;
    }
    if (mEndTimeMode == WALL_TIME) {
        time -= mDstSavings;
    }
    return GetTransition(cal, cdate, mEndMode, year, mEndMonth,
            mEndDay, mEndDayOfWeek, time);
}

Long SimpleTimeZone::GetTransition(
    /* [in] */ ICalendarSystem* cal,
    /* [in] */ ICalendarDate* cdate,
    /* [in] */ Integer mode,
    /* [in] */ Integer year,
    /* [in] */ Integer month,
    /* [in] */ Integer dayOfMonth,
    /* [in] */ Integer dayOfWeek,
    /* [in] */ Integer timeOfDay)
{
    IBaseCalendarDate::Probe(cdate)->SetNormalizedYear(year);
    cdate->SetMonth(month + 1);
    switch (mode) {
        case DOM_MODE:
            cdate->SetDayOfMonth(dayOfMonth);
            break;

        case DOW_IN_MONTH_MODE:
        {
            cdate->SetDayOfMonth(1);
            if (dayOfMonth < 0){
                Integer dom;
                cal->GetMonthLength(cdate, &dom);
                cdate->SetDayOfMonth(dom);
            }
            AutoPtr<ICalendarDate> date;
            cal->GetNthDayOfWeek(dayOfMonth, dayOfWeek, cdate, &date);
            cdate = date;
            break;
        }

        case DOW_GE_DOM_MODE:
        {
            cdate->SetDayOfMonth(dayOfMonth);
            AutoPtr<ICalendarDate> date;
            cal->GetNthDayOfWeek(1, dayOfWeek, cdate, &date);
            cdate = date;
            break;
        }

        case DOW_LE_DOM_MODE:
        {
            cdate->SetDayOfMonth(dayOfMonth);
            AutoPtr<ICalendarDate> date;
            cal->GetNthDayOfWeek(-1, dayOfWeek, cdate, &date);
            cdate = date;
            break;
        }
    }
    Long time;
    cal->GetTime(cdate, &time);
    return time + timeOfDay;
}

ECode SimpleTimeZone::GetRawOffset(
    /* [out] */ Integer* offset)
{
    VALIDATE_NOT_NULL(offset);

    // The given date will be taken into account while
    // we have the historical time zone data in place.
    *offset = mRawOffset;
    return NOERROR;
}

ECode SimpleTimeZone::SetRawOffset(
    /* [in] */ Integer offsetMillis)
{
    mRawOffset = offsetMillis;
    return NOERROR;
}

ECode SimpleTimeZone::SetDSTSavings(
    /* [in] */ Integer millisSavedDuringDST)
{
    if (millisSavedDuringDST <= 0) {
        Logger::E("SimpleTimeZone", "Illegal daylight saving value: %d",
                millisSavedDuringDST);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mDstSavings = millisSavedDuringDST;
    return NOERROR;
}

ECode SimpleTimeZone::GetDSTSavings(
    /* [out] */ Integer* savingTime)
{
    VALIDATE_NOT_NULL(savingTime);

    *savingTime = mUseDaylight ? mDstSavings : 0;
    return NOERROR;
}

ECode SimpleTimeZone::UseDaylightTime(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mUseDaylight;
    return NOERROR;
}

ECode SimpleTimeZone::ObservesDaylightTime(
    /* [out] */ Boolean* result)
{
    return UseDaylightTime(result);
}

ECode SimpleTimeZone::InDaylightTime(
    /* [in] */ IDate* date,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Long time;
    date->GetTime(&time);
    Integer offset;
    GetOffset(time, &offset);
    *result = offset != mRawOffset;
    return NOERROR;
}

}
}
