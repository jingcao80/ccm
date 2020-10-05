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
#include "como/core/CStringBuilder.h"
#include "como/util/GregorianCalendar.h"
#include "como/util/SimpleTimeZone.h"
#include "como/util/calendar/CalendarSystem.h"
#include "como/util/calendar/CalendarUtils.h"
#include <comolog.h>

using como::core::AutoLock;
using como::core::CStringBuilder;
using como::core::IStringBuilder;
using como::core::IID_IStringBuilder;
using como::util::GregorianCalendar;
using como::util::calendar::CalendarSystem;
using como::util::calendar::CalendarUtils;

namespace como {
namespace util {

COMO_INTERFACE_IMPL_1(SimpleTimeZone, TimeZone, ISimpleTimeZone);

constexpr Byte SimpleTimeZone::sStaticMonthLength[];
constexpr Byte SimpleTimeZone::sStaticLeapMonthLength[];

const AutoPtr<IGregorian> SimpleTimeZone::GetGcal()
{
    static const AutoPtr<IGregorian> sGcal = CalendarSystem::GetGregorianCalendar();
    return sGcal;
}

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
    /* [out] */ Integer& offset)
{
    offset = GetOffsets(date, Array<Integer>::Null());
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
            CalendarSystem::ForName("julian", cs);
            cal = cs;
        }
        AutoPtr<ICalendarDate> cdate;
        cal->NewCalendarDate(TimeZone::NO_TIMEZONE, cdate);
        // Get the year in local time
        cal->GetCalendarDate(date + mRawOffset, cdate);
        Integer year;
        IBaseCalendarDate::Probe(cdate)->GetNormalizedYear(year);
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
    /* [out] */ Integer& offset)
{
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
    cal->NewCalendarDate(TimeZone::NO_TIMEZONE, cdate);
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
        CalendarSystem::ForName("julian", cs);
        cal = cs;
        cs->NewCalendarDate(TimeZone::NO_TIMEZONE, cdate);
        IBaseCalendarDate::Probe(cdate)->SetNormalizedDate(y, m, day);
        cs->GetTime(cdate, &time);
        time = time + milliseconds - mRawOffset;
    }

    Integer yy, mm, dd;
    if ((IBaseCalendarDate::Probe(cdate)->GetNormalizedYear(yy), yy != y)
            || (cdate->GetMonth(mm), mm != m)
            || (cdate->GetDayOfMonth(dd), dd != day)
            // The validation should be cdate.getDayOfWeek() ==
            // dayOfWeek. However, we don't check dayOfWeek for
            // compatibility.
            || (dayOfWeek < ICalendar::SUNDAY || dayOfWeek > ICalendar::SATURDAY)
            || (milliseconds < 0 || milliseconds >= (24 * 60 * 60 * 1000))) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (!mUseDaylight || year < mStartYear || era != GregorianCalendar::CE) {
        offset = mRawOffset;
        return NOERROR;
    }

    offset = GetOffset(cal, cdate, y, time);
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
    /* [in] */ ICalendarDate* _cdate,
    /* [in] */ Integer mode,
    /* [in] */ Integer year,
    /* [in] */ Integer month,
    /* [in] */ Integer dayOfMonth,
    /* [in] */ Integer dayOfWeek,
    /* [in] */ Integer timeOfDay)
{
    AutoPtr<ICalendarDate> cdate = _cdate;
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
                cal->GetMonthLength(cdate, dom);
                cdate->SetDayOfMonth(dom);
            }
            AutoPtr<ICalendarDate> date;
            cal->GetNthDayOfWeek(dayOfMonth, dayOfWeek, cdate, date);
            cdate = date;
            break;
        }

        case DOW_GE_DOM_MODE:
        {
            cdate->SetDayOfMonth(dayOfMonth);
            AutoPtr<ICalendarDate> date;
            cal->GetNthDayOfWeek(1, dayOfWeek, cdate, date);
            cdate = date;
            break;
        }

        case DOW_LE_DOM_MODE:
        {
            cdate->SetDayOfMonth(dayOfMonth);
            AutoPtr<ICalendarDate> date;
            cal->GetNthDayOfWeek(-1, dayOfWeek, cdate, date);
            cdate = date;
            break;
        }
    }
    Long time;
    cal->GetTime(cdate, &time);
    return time + timeOfDay;
}

ECode SimpleTimeZone::GetRawOffset(
    /* [out] */ Integer& offset)
{
    // The given date will be taken into account while
    // we have the historical time zone data in place.
    offset = mRawOffset;
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
    /* [out] */ Integer& savingTime)
{
    savingTime = mUseDaylight ? mDstSavings : 0;
    return NOERROR;
}

ECode SimpleTimeZone::UseDaylightTime(
    /* [out] */ Boolean& result)
{
    result = mUseDaylight;
    return NOERROR;
}

ECode SimpleTimeZone::ObservesDaylightTime(
    /* [out] */ Boolean& result)
{
    return UseDaylightTime(result);
}

ECode SimpleTimeZone::InDaylightTime(
    /* [in] */ IDate* date,
    /* [out] */ Boolean& result)
{
    Long time;
    date->GetTime(time);
    Integer offset;
    GetOffset(time, offset);
    result = offset != mRawOffset;
    return NOERROR;
}

ECode SimpleTimeZone::CloneImpl(
    /* [in] */ ISimpleTimeZone* newObj)
{
    VALIDATE_NOT_NULL(newObj);

    SimpleTimeZone* clone = (SimpleTimeZone*)newObj;
    TimeZone::CloneImpl(clone);

    clone->mStartMonth = mStartMonth;
    clone->mStartDay = mStartDay;
    clone->mStartDayOfWeek = mStartDayOfWeek;
    clone->mStartTime = mStartTime;
    clone->mStartTimeMode = mStartTimeMode;
    clone->mEndMonth = mEndMonth;
    clone->mEndDay = mEndDay;
    clone->mEndDayOfWeek = mEndDayOfWeek;
    clone->mEndTime = mEndTime;
    clone->mEndTimeMode = mEndTimeMode;
    clone->mStartYear = mStartYear;
    clone->mRawOffset = mRawOffset;
    clone->mUseDaylight = mUseDaylight;
    clone->mStartMode = mStartMode;
    clone->mEndMode = mEndMode;
    clone->mDstSavings = mDstSavings;
    clone->mCacheYear = mCacheYear;
    clone->mCacheStart = mCacheStart;
    clone->mCacheEnd = mCacheEnd;
    return NOERROR;
}

ECode SimpleTimeZone::GetHashCode(
    /* [out] */ Integer& hash)
{
    AutoLock lock(this);

    hash = mStartMonth ^ mStartDay ^ mStartDayOfWeek ^ mStartTime ^
            mEndMonth ^ mEndDay ^ mEndDayOfWeek ^ mEndTime ^ mRawOffset;
    return NOERROR;
}

ECode SimpleTimeZone::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean& same)
{
    SimpleTimeZone* that = (SimpleTimeZone*)ISimpleTimeZone::Probe(obj);

    if (that == nullptr) {
        same = false;
        return NOERROR;
    }

    if (this == that) {
        same = true;
        return NOERROR;
    }

    String thisID, thatID;
    GetID(thisID);
    that->GetID(thatID);

    if (!thisID.Equals(thatID)) {
        same = false;
        return NOERROR;
    }

    return HasSameRules(that, same);
}

ECode SimpleTimeZone::HasSameRules(
    /* [in] */ ITimeZone* other,
    /* [out] */ Boolean& result)
{
    SimpleTimeZone* that = (SimpleTimeZone*)other;

    if (that == nullptr) {
        result = false;
        return NOERROR;
    }

    if (this == that) {
        result = true;
        return NOERROR;
    }
    result = mRawOffset == that->mRawOffset &&
            mUseDaylight == that->mUseDaylight &&
            (!mUseDaylight
             // Only check rules if using DST
             || (mDstSavings == that->mDstSavings &&
                 mStartMode == that->mStartMode &&
                 mStartMonth == that->mStartMonth &&
                 mStartDay == that->mStartDay &&
                 mStartDayOfWeek == that->mStartDayOfWeek &&
                 mStartTime == that->mStartTime &&
                 mStartTimeMode == that->mStartTimeMode &&
                 mEndMode == that->mEndMode &&
                 mEndMonth == that->mEndMonth &&
                 mEndDay == that->mEndDay &&
                 mEndDayOfWeek == that->mEndDayOfWeek &&
                 mEndTime == that->mEndTime &&
                 mEndTimeMode == that->mEndTimeMode &&
                 mStartYear == that->mStartYear));
    return NOERROR;
}

ECode SimpleTimeZone::ToString(
    /* [out] */ String& desc)
{
    AutoPtr<IStringBuilder> sb;
    CStringBuilder::New(IID_IStringBuilder, (IInterface**)&sb);
    sb->Append(GetCoclassName((ISimpleTimeZone*)this));
    sb->Append("[id=");
    String id;
    GetID(id);
    sb->Append(id);
    sb->Append(",offset=");
    sb->Append(mRawOffset);
    sb->Append(",dstSavings=");
    sb->Append(mDstSavings);
    sb->Append(",useDaylight=");
    sb->Append(mUseDaylight);
    sb->Append(",startYear=");
    sb->Append(mStartYear);
    sb->Append(",startMode=");
    sb->Append(mStartMode);
    sb->Append(",startMonth=");
    sb->Append(mStartMonth);
    sb->Append(",startDay=");
    sb->Append(mStartDay);
    sb->Append(",startDayOfWeek=");
    sb->Append(mStartDayOfWeek);
    sb->Append(",startTime=");
    sb->Append(mStartTime);
    sb->Append(",startTimeMode=");
    sb->Append(mStartTimeMode);
    sb->Append(",endMode=");
    sb->Append(mEndMode);
    sb->Append(",endMonth=");
    sb->Append(mEndMonth);
    sb->Append(",endDay=");
    sb->Append(mEndDay);
    sb->Append(",endDayOfWeek=");
    sb->Append(mEndDayOfWeek);
    sb->Append(",endTime=");
    sb->Append(mEndTime);
    sb->Append(",endTimeMode=");
    sb->Append(mEndTimeMode);
    sb->Append(U']');
    return sb->ToString(desc);
}

void SimpleTimeZone::InvalidateCache()
{
    mCacheYear = mStartYear - 1;
    mCacheStart = mCacheEnd = 0;
}

ECode SimpleTimeZone::DecodeRules()
{
    FAIL_RETURN(DecodeStartRule());
    return DecodeEndRule();
}

ECode SimpleTimeZone::DecodeStartRule()
{
    mUseDaylight = (mStartDay != 0) && (mEndDay != 0);
    if (mStartDay != 0) {
        if (mStartMonth < ICalendar::JANUARY || mStartMonth > ICalendar::DECEMBER) {
            Logger::E("SimpleTimeZone", "Illegal start month %d", mStartMonth);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        if (mStartTime < 0 || mStartTime > mMillisPerDay) {
            Logger::E("SimpleTimeZone", "Illegal start time %d", mStartTime);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        if (mStartDayOfWeek == 0) {
            mStartMode = DOM_MODE;
        }
        else {
            if (mStartDayOfWeek > 0) {
                mStartMode = DOW_IN_MONTH_MODE;
            }
            else {
                mStartDayOfWeek = -mStartDayOfWeek;
                if (mStartDay > 0) {
                    mStartMode = DOW_GE_DOM_MODE;
                }
                else {
                    mStartDay = -mStartDay;
                    mStartMode = DOW_LE_DOM_MODE;
                }
            }
            if (mStartDayOfWeek > ICalendar::SATURDAY) {
                Logger::E("SimpleTimeZone", "Illegal start day of week %d", mStartDayOfWeek);
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }
        if (mStartMode == DOW_IN_MONTH_MODE) {
            if (mStartDay < -5 || mStartDay > 5) {
                Logger::E("SimpleTimeZone", "Illegal start day of week in month %d", mStartDay);
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }
        else if (mStartDay < 1 || mStartDay > sStaticMonthLength[mStartMonth]) {
            Logger::E("SimpleTimeZone", "Illegal start day %d", mStartDay);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode SimpleTimeZone::DecodeEndRule()
{
    mUseDaylight = (mStartDay != 0) && (mEndDay != 0);
    if (mEndDay != 0) {
        if (mEndMonth < ICalendar::JANUARY || mEndMonth > ICalendar::DECEMBER) {
            Logger::E("SimpleTimeZone", "Illegal end month %d", mEndMonth);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        if (mEndTime < 0 || mEndTime > mMillisPerDay) {
            Logger::E("SimpleTimeZone", "Illegal end time %d", mEndTime);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        if (mEndDayOfWeek == 0) {
            mEndMode = DOM_MODE;
        }
        else {
            if (mEndDayOfWeek > 0) {
                mEndMode = DOW_IN_MONTH_MODE;
            }
            else {
                mEndDayOfWeek = -mEndDayOfWeek;
                if (mEndDay > 0) {
                    mEndMode = DOW_GE_DOM_MODE;
                }
                else {
                    mEndDay = -mEndDay;
                    mEndMode = DOW_LE_DOM_MODE;
                }
            }
            if (mEndDayOfWeek > ICalendar::SATURDAY) {
                Logger::E("SimpleTimeZone", "Illegal end day of week %d", mEndDayOfWeek);
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }
        if (mEndMode == DOW_IN_MONTH_MODE) {
            if (mEndDay < -5 || mEndDay > 5) {
                Logger::E("SimpleTimeZone", "Illegal end day of week in month %d", mEndDay);
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }
        else if (mEndDay < 1 || mEndDay > sStaticMonthLength[mEndMonth]) {
            Logger::E("SimpleTimeZone", "Illegal end day %d", mEndDay);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    return NOERROR;
}

}
}
