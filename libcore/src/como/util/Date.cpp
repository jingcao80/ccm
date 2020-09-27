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
#include "como/core/Character.h"
#include "como/core/CStringBuilder.h"
#include "como/core/System.h"
#include "como/text/DateFormat.h"
#include "como/util/CDate.h"
#include "como/util/Date.h"
#include "como/util/CGregorianCalendar.h"
#include "como/util/Locale.h"
#include "como/util/TimeZone.h"
#include "como/util/calendar/CalendarSystem.h"
#include "como/util/calendar/CalendarUtils.h"
#include "como.core.IInteger.h"
#include "como.util.ITimeZone.h"
#include "como.util.calendar.ICalendarDate.h"
#include "como.util.calendar.ICalendarSystem.h"
#include "como.util.calendar.IEra.h"

using como::core::AutoLock;
using como::core::Character;
using como::core::CStringBuilder;
using como::core::IID_ICloneable;
using como::core::IID_IComparable;
using como::core::IID_IStringBuilder;
using como::core::IInteger;
using como::core::IStringBuilder;
using como::core::System;
using como::io::IID_ISerializable;
using como::text::DateFormat;
using como::text::IDateFormat;
using como::util::ITimeZone;
using como::util::calendar::CalendarSystem;
using como::util::calendar::CalendarUtils;
using como::util::calendar::ICalendarDate;
using como::util::calendar::ICalendarSystem;
using como::util::calendar::IEra;
using como::util::calendar::IID_IBaseCalendarDate;
using como::util::calendar::IID_ICalendarDate;

namespace como {
namespace util {

AutoPtr<IBaseCalendar> Date::sJcal;
Integer Date::sDefaultCenturyStart = 0;
constexpr Integer Date::sTtb[];

COMO_INTERFACE_IMPL_4(Date, SyncObject, IDate, ICloneable, IComparable, ISerializable);

SyncObject& Date::GetClassLock()
{
    static SyncObject sLock;
    return sLock;
}

AutoPtr<IBaseCalendar> Date::GetGcal()
{
    static AutoPtr<IBaseCalendar> sGcal = IBaseCalendar::Probe(
            CalendarSystem::GetGregorianCalendar());
    return sGcal;
}

static Array<String> CreateWtb()
{
    Array<String> strArr(32);
    strArr[0] = "am"; strArr[1] = "pm";
    strArr[2] = "monday"; strArr[3] = "tuesday"; strArr[4] = "wednesday";
    strArr[5] = "thursday"; strArr[6] = "friday"; strArr[7] = "saturday"; strArr[8] = "sunday";
    strArr[9] = "january"; strArr[10] = "february"; strArr[11] = "march"; strArr[12] = "april";
    strArr[13] = "may"; strArr[14] = "june"; strArr[15] = "july"; strArr[16] = "august";
    strArr[17] = "september"; strArr[18] = "october"; strArr[19] = "november"; strArr[20] = "december";
    strArr[21] = "gmt"; strArr[22] = "ut"; strArr[23] = "utc"; strArr[24] = "est";
    strArr[25] = "edt"; strArr[26] = "cst"; strArr[27] = "cdt";
    strArr[28] = "mst"; strArr[29] = "mdt"; strArr[30] = "pst"; strArr[31] = "pdt";

    return strArr;
}

Array<String>& Date::GetWtb()
{
    static Array<String> wtb = CreateWtb();
    return wtb;
}

ECode Date::Constructor()
{
    return Constructor(System::GetCurrentTimeMillis());
}

ECode Date::Constructor(
    /* [in] */ Long date)
{
    mFastTime = date;
    return NOERROR;
}

ECode Date::Constructor(
    /* [in] */ Integer year,
    /* [in] */ Integer month,
    /* [in] */ Integer date)
{
    return Constructor(year, month, date, 0, 0 ,0);
}

ECode Date::Constructor(
    /* [in] */ Integer year,
    /* [in] */ Integer month,
    /* [in] */ Integer date,
    /* [in] */ Integer hrs,
    /* [in] */ Integer min)
{
    return Constructor(year, month, date, hrs, min, 0);
}

ECode Date::Constructor(
    /* [in] */ Integer year,
    /* [in] */ Integer month,
    /* [in] */ Integer date,
    /* [in] */ Integer hrs,
    /* [in] */ Integer min,
    /* [in] */ Integer sec)
{
    Integer y = year + 1900;
    // month is 0-based. So we have to normalize month to support Long.MAX_VALUE.
    if (month >= 12) {
        y += month / 12;
        month %= 12;
    }
    else if (month < 0) {
        y += CalendarUtils::FloorDivide(month, 12);
        month = CalendarUtils::Mod(month, 12);
    }
    AutoPtr<IBaseCalendar> cal = GetCalendarSystem(y);
    AutoPtr<ICalendarDate> cdate;
    ICalendarSystem::Probe(cal)->NewCalendarDate(
            TimeZone::GetDefaultRef(), cdate);
    mCdate = IBaseCalendarDate::Probe(cdate);
    mCdate->SetNormalizedDate(y, month + 1, date);
    cdate->SetTimeOfDay(hrs, min, sec, 0);
    GetTimeImpl();
    mCdate = nullptr;
    return NOERROR;
}

ECode Date::Constructor(
    /* [in] */ const String& s)
{
    Long date;
    FAIL_RETURN(Parse(s, &date));
    return Constructor(date);
}

ECode Date::CloneImpl(
    /* [in] */ IDate* newObj)
{
    Date* dateObj = (Date*)newObj;

    dateObj->mFastTime = mFastTime;
    if (mCdate != nullptr) {
        AutoPtr<IBaseCalendarDate> cdate;
        ICloneable::Probe(mCdate)->Clone(IID_IBaseCalendarDate, (IInterface**)&cdate);
        dateObj->mCdate = cdate;
    }
    return NOERROR;
}

Long Date::UTC(
    /* [in] */ Integer year,
    /* [in] */ Integer month,
    /* [in] */ Integer date,
    /* [in] */ Integer hrs,
    /* [in] */ Integer min,
    /* [in] */ Integer sec)
{
    Integer y = year + 1900;
    // month is 0-based. So we have to normalize month to support Long.MAX_VALUE.
    if (month >= 12) {
        y += month / 12;
        month %= 12;
    }
    else if (month < 0) {
        y += CalendarUtils::FloorDivide(month, 12);
        month = CalendarUtils::Mod(month, 12);
    }
    Integer m = month + 1;
    AutoPtr<IBaseCalendar> cal = GetCalendarSystem(y);
    AutoPtr<ICalendarDate> cdate;
    ICalendarSystem::Probe(cal)->NewCalendarDate(nullptr, cdate);
    AutoPtr<IBaseCalendarDate> udate = IBaseCalendarDate::Probe(cdate);
    udate->SetNormalizedDate(y, m, date);
    cdate->SetTimeOfDay(hrs, min, sec, 0);

    // Use a Date instance to perform normalization. Its fastTime
    // is the UTC value after the normalization.
    AutoPtr<IDate> d;
    CDate::New(0, IID_IDate, (IInterface**)&d);
    From(d)->Normalize(udate);
    return From(d)->mFastTime;
}

ECode Date::Parse(
    /* [in] */ const String& s,
    /* [out] */ Long* date)
{
    Integer year = IInteger::MIN_VALUE;
    Integer mon = -1;
    Integer mday = -1;
    Integer hour = -1;
    Integer min = -1;
    Integer sec = -1;
    Integer millis = -1;
    Integer c = -1;
    Integer i = 0;
    Integer n = -1;
    Integer wst = -1;
    Integer tzoffset = -1;
    Integer prevc = 0;
syntax:
    {
        if (s.IsNull()) {
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        Integer limit = s.GetLength();
        while (i < limit) {
            c = s.GetChar(i);
            i++;
            if (c <= U' ' || c == U',') {
                continue;
            }
            if (c == U'(') { // skip comments
                Integer depth = 1;
                while (i < limit) {
                    c = s.GetChar(i);
                    i++;
                    if (c == U'(') depth++;
                    else if (c == U')') {
                        if (--depth <= 0) {
                            break;
                        }
                    }
                }
                continue;
            }
            if (U'0' <= c && c <= U'9') {
                n = c - U'0';
                while ((i < limit) && (U'0' <= (c = s.GetChar(i))) && (c <= U'9')) {
                    n = n * 10 + c - U'0';
                    i++;
                }
                if ((prevc == U'+') || (prevc == U'-' && year != IInteger::MIN_VALUE)) {
                    if (tzoffset != 0 && tzoffset != -1) {
                        return E_ILLEGAL_ARGUMENT_EXCEPTION;
                    }

                    // timezone offset
                    if (n < 24) {
                        n = n * 60; // EG. "GMT-3"

                        // Support for Timezones of the form GMT-3:30. We look for an ':" and
                        // parse the number following it as loosely as the original hours
                        // section (i.e, no range or validity checks).
                        Integer minutesPart = 0;
                        if (i < limit && (s.GetChar(i) == U':')) {
                            i++;
                            while (i < limit && U'0' <= (c = s.GetChar(i)) && c <= U'9') {
                                minutesPart = (minutesPart * 10) + (c - U'0');
                                i++;
                            }
                        }

                        n += minutesPart;
                    }
                    else {
                        n = (n % 100) + ((n / 100) * 60); // eg "GMT-0430"
                    }

                    if (prevc == U'+') { // plus means east of GMT
                        n = -n;
                    }

                    tzoffset = n;
                }
                else if (n >= 70) {
                    if (year != IInteger::MIN_VALUE) {
                        return E_ILLEGAL_ARGUMENT_EXCEPTION;
                    }
                    else if (c <= U' ' || c == U',' || c == U'/' || i >= limit) {
                        // year = n < 1900 ? n : n - 1900;
                        year = n;
                    }
                    else {
                        return E_ILLEGAL_ARGUMENT_EXCEPTION;
                    }
                }
                else if (c == U':') {
                    if (hour < 0) {
                        hour = (Byte)n;
                    }
                    else if (min < 0) {
                        min = (Byte)n;
                    }
                    else {
                        return E_ILLEGAL_ARGUMENT_EXCEPTION;
                    }
                }
                else if (c == U'/') {
                    if (mon < 0) {
                        mon = (Byte)(n - 1);
                    }
                    else if (mday < 0) {
                        mday = (Byte)n;
                    }
                    else {
                        return E_ILLEGAL_ARGUMENT_EXCEPTION;
                    }
                }
                else if (i < limit && c != U',' && c > U' ' && c != U'-') {
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
                }
                else if (hour >= 0 && min < 0) {
                    min = (Byte)n;
                }
                else if (min >= 0 && sec < 0) {
                    sec = (Byte)n;
                }
                else if (mday < 0) {
                    mday = (Byte)n;
                }
                // Handle two-digit years < 70 (70-99 handled above).
                else if (year == IInteger::MIN_VALUE && mon >= 0 && mday >= 0) {
                    year = n;
                }
                else {
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
                }
                prevc = 0;
            }
            else if (c == U'/' || c == U':' || c == U'+' || c == U'-') {
                prevc = c;
            }
            else {
                Integer st = i - 1;
                while (i < limit) {
                    c = s.GetChar(i);
                    if (!((U'A' <= c && c <= U'Z') || (U'a' <= c && c <= U'z'))) {
                        break;
                    }
                    i++;
                }
                if (i < st + 1) {
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
                }
                Integer k;
                for (k = GetWtb().GetLength(); --k >= 0;) {
                    if (GetWtb()[k].RegionMatchesIgnoreCase(0, s, st, i - st)) {
                        Integer action = sTtb[k];
                        if (action != 0) {
                            if (action == 1) {  // pm
                                if (hour > 12 || hour < 1) {
                                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
                                }
                                else if (hour < 12) {
                                    hour += 12;
                                }
                            }
                            else if (action == 14) {  // am
                                if (hour > 12 || hour < 1) {
                                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
                                }
                                else if (hour == 12) {
                                    hour = 0;
                                }
                            }
                            else if (action <= 13) {  // month!
                                if (mon < 0) {
                                    mon = (Byte)(action - 2);
                                }
                                else {
                                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
                                }
                            }
                            else {
                                tzoffset = action - 10000;
                            }
                        }
                        break;
                    }
                }
                if (k < 0) {
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
                }
                prevc = 0;
            }
        }
        if (year == IInteger::MIN_VALUE || mon < 0 || mday < 0) {
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        // Parse 2-digit years within the correct default century.
        if (year < 100) {
            {
                AutoLock lock(GetClassLock());
                if (sDefaultCenturyStart == 0) {
                    AutoPtr<ICalendarDate> cdate;
                    ICalendarSystem::Probe(GetGcal())->GetCalendarDate(cdate);
                    Integer y;
                    cdate->GetYear(y);
                    sDefaultCenturyStart = y - 80;
                }
            }
            year += (sDefaultCenturyStart / 100) * 100;
            if (year < sDefaultCenturyStart) year += 100;
        }
        if (sec < 0) sec = 0;
        if (min < 0) min = 0;
        if (hour < 0) hour = 0;
        AutoPtr<IBaseCalendar> cal = GetCalendarSystem(year);
        if (tzoffset == -1) {
            AutoPtr<ICalendarDate> ldate;
            ICalendarSystem::Probe(cal)->NewCalendarDate(
                    TimeZone::GetDefaultRef(), ldate);
            ldate->SetDate(year, mon + 1, mday);
            ldate->SetTimeOfDay(hour, min, sec, 0);
            return ICalendarSystem::Probe(cal)->GetTime(ldate, date);
        }
        AutoPtr<ICalendarDate> udate;
        ICalendarSystem::Probe(cal)->NewCalendarDate(nullptr, udate);
        udate->SetDate(year, mon + 1, mday);
        udate->SetTimeOfDay(hour, min, sec, 0);
        Long time;
        ICalendarSystem::Probe(cal)->GetTime(udate, &time);
        *date = time + tzoffset * (60 * 1000);
        return NOERROR;
    }
}

ECode Date::GetYear(
    /* [out] */ Integer& year)
{
    ICalendarDate::Probe(Normalize())->GetYear(year);
    year = year - 1900;
    return NOERROR;
}

ECode Date::SetYear(
    /* [in] */ Integer year)
{
    return GetCalendarDate()->SetNormalizedYear(year + 1900);
}

ECode Date::GetMonth(
    /* [out] */ Integer& month)
{
    ICalendarDate::Probe(Normalize())->GetMonth(month);
    month = month - 1; // adjust 1-based to 0-based
    return NOERROR;
}

ECode Date::SetMonth(
    /* [in] */ Integer month)
{
    Integer y = 0;
    if (month >= 12) {
        y = month / 12;
        month %= 12;
    }
    else if (month < 0) {
        y = CalendarUtils::FloorDivide(month, 12);
        month = CalendarUtils::Mod(month, 12);
    }
    AutoPtr<IBaseCalendarDate> d = GetCalendarDate();
    if (y != 0) {
        Integer ny;
        d->GetNormalizedYear(ny);
        d->SetNormalizedYear(ny + y);
    }
    return ICalendarDate::Probe(d)->SetMonth(month + 1); // adjust 0-based to 1-based month numbering
}

ECode Date::GetDate(
    /* [out] */ Integer& date)
{
    return ICalendarDate::Probe(Normalize())->GetDayOfMonth(date);
}

ECode Date::SetDate(
    /* [in] */ Integer date)
{
    return ICalendarDate::Probe(GetCalendarDate())->SetDayOfMonth(date);
}

ECode Date::GetDay(
    /* [out] */ Integer& day)
{
    ICalendarDate::Probe(Normalize())->GetDayOfWeek(day);
    day = day - IBaseCalendar::SUNDAY;
    return NOERROR;
}

ECode Date::GetHours(
    /* [out] */ Integer& hours)
{
    return ICalendarDate::Probe(Normalize())->GetHours(hours);
}

ECode Date::SetHours(
    /* [in] */ Integer hours)
{
    return ICalendarDate::Probe(GetCalendarDate())->SetHours(hours);
}

ECode Date::GetMinutes(
    /* [out] */ Integer& minutes)
{
    return ICalendarDate::Probe(Normalize())->GetMinutes(minutes);
}

ECode Date::SetMinutes(
    /* [in] */ Integer minutes)
{
    return ICalendarDate::Probe(GetCalendarDate())->SetMinutes(minutes);
}

ECode Date::GetSeconds(
    /* [out] */ Integer& seconds)
{
    return ICalendarDate::Probe(Normalize())->GetSeconds(seconds);
}

ECode Date::SetSeconds(
    /* [in] */ Integer seconds)
{
    return ICalendarDate::Probe(GetCalendarDate())->SetSeconds(seconds);
}

ECode Date::GetTime(
    /* [out] */ Long& time)
{
    time = GetTimeImpl();
    return NOERROR;
}

Long Date::GetTimeImpl()
{
    Boolean normalized;
    if (mCdate != nullptr &&
            (ICalendarDate::Probe(mCdate)->IsNormalized(normalized), !normalized)) {
        Normalize();
    }
    return mFastTime;
}

ECode Date::SetTime(
    /* [in] */ Long time)
{
    mFastTime = time;
    mCdate = nullptr;
    return NOERROR;
}

ECode Date::Before(
    /* [in] */ IDate* when,
    /* [out] */ Boolean& before)
{
    before = GetMillisOf(this) < GetMillisOf(when);
    return NOERROR;
}

ECode Date::After(
    /* [in] */ IDate* when,
    /* [out] */ Boolean& after)
{
    after = GetMillisOf(this) > GetMillisOf(when);
    return NOERROR;
}

ECode Date::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean& same)
{
    if (IDate::Probe(obj) == nullptr) {
        same = false;
        return NOERROR;
    }

    Long thisTime, anotherTime;
    GetTime(thisTime);
    IDate::Probe(obj)->GetTime(anotherTime);
    same = thisTime == anotherTime;
    return NOERROR;
}

Long Date::GetMillisOf(
    /* [in] */ IDate* date)
{
    Date* dateObj = From(date);
    Boolean normalized;
    if (dateObj->mCdate == nullptr ||
            (ICalendarDate::Probe(dateObj->mCdate)->IsNormalized(normalized),
                normalized)) {
        return dateObj->mFastTime;
    }
    AutoPtr<ICalendarDate> cdate;
    ICloneable::Probe(dateObj->mCdate)->Clone(IID_ICalendarDate, (IInterface**)&cdate);
    Long time;
    ICalendarSystem::Probe(GetGcal())->GetTime(cdate, &time);
    return time;
}

ECode Date::CompareTo(
    /* [in] */ IInterface* other,
    /* [out] */ Integer& result)
{
    if (IDate::Probe(other) == nullptr) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Long thisTime = GetMillisOf(this);
    Long anotherTime = GetMillisOf(IDate::Probe(other));
    result = (thisTime < anotherTime ? -1 : (thisTime == anotherTime ? 0 : 1));
    return NOERROR;
}

ECode Date::GetHashCode(
    /* [out] */ Integer& hash)
{
    Long ht;
    GetTime(ht);
    hash = (Integer)ht ^ (Integer)(ht >> 32);
    return NOERROR;
}

ECode Date::ToString(
    /* [out] */ String& desc)
{
    // "EEE MMM dd HH:mm:ss zzz yyyy";
    AutoPtr<ICalendarDate> date = ICalendarDate::Probe(Normalize());
    AutoPtr<IStringBuilder> sb;
    CStringBuilder::New(28, IID_IStringBuilder, (IInterface**)&sb);
    Integer index;
    date->GetDayOfWeek(index);
    if (index == IBaseCalendar::SUNDAY) {
        index = 8;
    }
    ConvertToAbbr(sb, GetWtb()[index]);
    sb->Append(U' '); // EEE
    Integer month, days, hours, minutes, seconds, year;
    date->GetMonth(month);
    date->GetDayOfMonth(days);
    date->GetHours(hours);
    date->GetMinutes(minutes);
    date->GetSeconds(seconds);
    date->GetYear(year);
    ConvertToAbbr(sb, GetWtb()[month - 1 + 2 + 7]);
    sb->Append(U' '); // MMM
    CalendarUtils::Sprintf0d(sb, days, 2);
    sb->Append(U' '); // dd
    CalendarUtils::Sprintf0d(sb, hours, 2);
    sb->Append(U':'); // HH
    CalendarUtils::Sprintf0d(sb, minutes, 2);
    sb->Append(U':'); // mm
    CalendarUtils::Sprintf0d(sb, seconds, 2);
    sb->Append(U' '); // ss
    AutoPtr<ITimeZone> zi;
    date->GetZone(zi);
    if (zi != nullptr) {
        Boolean daylight;
        date->IsDaylightTime(daylight);
        String name;
        zi->GetDisplayName(daylight, ITimeZone::SHORT, Locale::GetUS(), &name);
        sb->Append(name);
    }
    else {
        sb->Append(String("GMT"));
    }
    sb->Append(U' ');
    sb->Append(year); // yyyy
    return sb->ToString(desc);
}

void Date::ConvertToAbbr(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ const String& name)
{
    sb->Append(Character::ToUpperCase(name.GetChar(0)));
    sb->Append(name.GetChar(1));
    sb->Append(name.GetChar(2));
}

ECode Date::ToLocaleString(
    /* [out] */ String& str)
{
    AutoPtr<IDateFormat> formatter;
    DateFormat::GetDateTimeInstance(formatter);
    return formatter->Format(this, str);
}

ECode Date::ToGMTString(
    /* [out] */ String& str)
{
    Long t;
    GetTime(t);
    AutoPtr<ICalendarDate> date;
    ICalendarSystem::Probe(GetCalendarSystem(t))->GetCalendarDate(
            t, nullptr, date);
    AutoPtr<IStringBuilder> sb;
    CStringBuilder::New(32, IID_IStringBuilder, (IInterface**)&sb);
    Integer month, days, hours, minutes, seconds, year;
    date->GetDayOfMonth(days);
    date->GetMonth(month);
    date->GetYear(year);
    date->GetHours(hours);
    date->GetMinutes(minutes);
    date->GetSeconds(seconds);
    CalendarUtils::Sprintf0d(sb, days, 1);
    sb->Append(U' '); // d
    ConvertToAbbr(sb, GetWtb()[month - 1 + 2 + 7]);
    sb->Append(U' '); // MMM
    sb->Append(year);
    sb->Append(U' '); // yyyy
    CalendarUtils::Sprintf0d(sb, hours, 2);
    sb->Append(U':'); // HH
    CalendarUtils::Sprintf0d(sb, minutes, 2);
    sb->Append(U':'); // mm
    CalendarUtils::Sprintf0d(sb, seconds, 2); // ss
    sb->Append(String(" GMT")); // ' GMT'
    return sb->ToString(str);
}

ECode Date::GetTimezoneOffset(
    /* [out] */ Integer& tzOffset)
{
    Integer zoneOffset;
    if (mCdate == nullptr) {
        AutoPtr<ICalendar> cal;
        CGregorianCalendar::New(mFastTime, IID_ICalendar, (IInterface**)&cal);
        Integer zoff, doff;
        cal->Get(ICalendar::ZONE_OFFSET, zoff);
        cal->Get(ICalendar::DST_OFFSET, doff);
        zoneOffset = (zoff + doff);
    }
    else {
        Normalize();
        ICalendarDate::Probe(mCdate)->GetZoneOffset(zoneOffset);
    }
    tzOffset = -zoneOffset / 60000; // convert to minutes
    return NOERROR;
}

AutoPtr<IBaseCalendarDate> Date::GetCalendarDate()
{
    if (mCdate == nullptr) {
        AutoPtr<IBaseCalendar> cal = GetCalendarSystem(mFastTime);
        AutoPtr<ICalendarDate> cdate;
        ICalendarSystem::Probe(cal)->GetCalendarDate(mFastTime,
                TimeZone::GetDefaultRef(), cdate);
        mCdate = IBaseCalendarDate::Probe(cdate);
    }
    return mCdate;
}

AutoPtr<IBaseCalendarDate> Date::Normalize()
{
    if (mCdate == nullptr) {
        AutoPtr<IBaseCalendar> cal = GetCalendarSystem(mFastTime);
        AutoPtr<ICalendarDate> cdate;
        ICalendarSystem::Probe(cal)->GetCalendarDate(mFastTime,
                TimeZone::GetDefaultRef(), cdate);
        mCdate = IBaseCalendarDate::Probe(cdate);
        return mCdate;
    }

    AutoPtr<ICalendarDate> cdate = ICalendarDate::Probe(mCdate);

    // Normalize cdate with the TimeZone in cdate first. This is
    // required for the compatible behavior.
    Boolean normalized;
    if (cdate->IsNormalized(normalized), !normalized) {
        mCdate = Normalize(mCdate);
    }

    // If the default TimeZone has changed, then recalculate the
    // fields with the new TimeZone.
    AutoPtr<ITimeZone> tz = TimeZone::GetDefaultRef();
    AutoPtr<ITimeZone> zone;
    if (cdate->GetZone(zone), tz != zone) {
        cdate->SetZone(tz);
        AutoPtr<IBaseCalendar> cal = GetCalendarSystem(mCdate);
        ICalendarSystem::Probe(cal)->GetCalendarDate(mFastTime, cdate);
    }
    return mCdate;
}

AutoPtr<IBaseCalendarDate> Date::Normalize(
    /* [in] */ IBaseCalendarDate* date_)
{
    AutoPtr<IBaseCalendarDate> date = date_;

    Integer y, m, d, hh, mm, ss, ms;
    date->GetNormalizedYear(y);
    ICalendarDate* cdate = ICalendarDate::Probe(date);
    cdate->GetMonth(m);
    cdate->GetDayOfMonth(d);
    cdate->GetHours(hh);
    cdate->GetMinutes(mm);
    cdate->GetSeconds(ss);
    cdate->GetMillis(ms);
    AutoPtr<ITimeZone> tz;
    cdate->GetZone(tz);

    // If the specified year can't be handled using a long value
    // in milliseconds, GregorianCalendar is used for full
    // compatibility with underflow and overflow. This is required
    // by some JCK tests. The limits are based max year values -
    // years that can be represented by max values of d, hh, mm,
    // ss and ms. Also, let GregorianCalendar handle the default
    // cutover year so that we don't need to worry about the
    // transition here.
    if (y == 1582 || y > 280000000 || y < -280000000) {
        if (tz == nullptr) {
            TimeZone::GetTimeZone(String("GMT"), &tz);
        }
        AutoPtr<ICalendar> gc;
        CGregorianCalendar::New(tz, IID_ICalendar, (IInterface**)&gc);
        gc->Clear();
        gc->Set(ICalendar::MILLISECOND, ms);
        gc->Set(y, m - 1, d, hh, mm, ss);
        gc->GetTimeInMillis(mFastTime);
        AutoPtr<IBaseCalendar> cal = GetCalendarSystem(mFastTime);
        AutoPtr<ICalendarDate> cdate;
        ICalendarSystem::Probe(cal)->GetCalendarDate(mFastTime, tz, cdate);
        return IBaseCalendarDate::Probe(cdate);
    }

    AutoPtr<IBaseCalendar> cal = GetCalendarSystem(y);
    if (cal != GetCalendarSystem(date)) {
        AutoPtr<ICalendarDate> cdate;
        ICalendarSystem::Probe(cal)->NewCalendarDate(tz, cdate);
        date = IBaseCalendarDate::Probe(cdate);
        date->SetNormalizedDate(y, m, d);
        cdate->SetTimeOfDay(hh, mm, ss, ms);
    }
    // Perform the GregorianCalendar-style normalization.
    ICalendarSystem::Probe(cal)->GetTime(ICalendarDate::Probe(date), &mFastTime);

    // In case the normalized date requires the other calendar
    // system, we need to recalculate it using the other one.
    AutoPtr<IBaseCalendar> ncal = GetCalendarSystem(mFastTime);
    if (ncal != cal) {
        AutoPtr<ICalendarDate> cdate;
        ICalendarSystem::Probe(ncal)->NewCalendarDate(tz, cdate);
        date = IBaseCalendarDate::Probe(cdate);
        date->SetNormalizedDate(y, m, d);
        cdate->SetTimeOfDay(hh, mm, ss, ms);
        ICalendarSystem::Probe(ncal)->GetTime(cdate, &mFastTime);
    }
    return date;
}

AutoPtr<IBaseCalendar> Date::GetCalendarSystem(
    /* [in] */ Integer year)
{
    if (year >= 1582) {
        return GetGcal();
    }
    return GetJulianCalendar();
}

AutoPtr<IBaseCalendar> Date::GetCalendarSystem(
    /* [in] */ Long utc)
{
    Integer offset;
    if (utc >= 0
        || utc >= GregorianCalendar::DEFAULT_GREGORIAN_CUTOVER
                    - (TimeZone::GetDefaultRef()->GetOffset(utc, &offset), offset)) {
        return GetGcal();
    }
    return GetJulianCalendar();
}

AutoPtr<IBaseCalendar> Date::GetCalendarSystem(
    /* [in] */ IBaseCalendarDate* cdate)
{
    if (sJcal == nullptr) {
        return GetGcal();
    }
    AutoPtr<IEra> era;
    if (ICalendarDate::Probe(cdate)->GetEra(era), era != nullptr) {
        return sJcal;
    }
    return GetGcal();
}

AutoPtr<IBaseCalendar> Date::GetJulianCalendar()
{
    AutoLock lock(GetClassLock());

    if (sJcal == nullptr) {
        AutoPtr<ICalendarSystem> system;
        CalendarSystem::ForName("julian", system);
        sJcal = IBaseCalendar::Probe(system);
    }
    return sJcal;
}

}
}
