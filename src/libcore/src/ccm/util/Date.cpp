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
#include "ccm/core/System.h"
#include "ccm/util/CDate.h"
#include "ccm/util/Date.h"
#include "ccm/util/TimeZone.h"
#include "ccm/util/calendar/CalendarSystem.h"
#include "ccm/util/calendar/CalendarUtils.h"
#include "ccm.core.IInteger.h"
#include "ccm.util.calendar.ICalendarDate.h"
#include "ccm.util.calendar.ICalendarSystem.h"

using ccm::core::AutoLock;
using ccm::core::IID_ICloneable;
using ccm::core::IID_IComparable;
using ccm::core::IInteger;
using ccm::core::System;
using ccm::io::IID_ISerializable;
using ccm::util::calendar::CalendarSystem;
using ccm::util::calendar::CalendarUtils;
using ccm::util::calendar::ICalendarDate;
using ccm::util::calendar::ICalendarSystem;

namespace ccm {
namespace util {

Integer Date::sDefaultCenturyStart = 0;
constexpr Integer Date::sTtb[];

CCM_INTERFACE_IMPL_4(Date, SyncObject, IDate, ICloneable, IComparable, ISerializable);

SyncObject& Date::GetLock()
{
    static SyncObject sLock;
    return sLock;
}

AutoPtr<IBaseCalendar> Date::GetGcal()
{
    static AutoPtr<IBaseCalendar> gcal = IBaseCalendar::Probe(
            CalendarSystem::GetGregorianCalendar());
    return gcal;
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
            TimeZone::GetDefaultRef(), (ICalendarDate**)&cdate);
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
        AutoPtr<IInterface> cdate;
        ICloneable::Probe(mCdate)->Clone((IInterface**)&cdate);
        dateObj->mCdate = IBaseCalendarDate::Probe(cdate);
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
    ICalendarSystem::Probe(cal)->NewCalendarDate(nullptr, (ICalendarDate**)&cdate);
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
            if (c <= ' ' || c == ',') {
                continue;
            }
            if (c == '(') { // skip comments
                Integer depth = 1;
                while (i < limit) {
                    c = s.GetChar(i);
                    i++;
                    if (c == '(') depth++;
                    else if (c == ')') {
                        if (--depth <= 0) {
                            break;
                        }
                    }
                }
                continue;
            }
            if ('0' <= c && c <= '9') {
                n = c - '0';
                while ((i < limit) && ('0' <= (c = s.GetChar(i))) && (c <= '9')) {
                    n = n * 10 + c - '0';
                    i++;
                }
                if ((prevc == '+') || (prevc == '-' && year != IInteger::MIN_VALUE)) {
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
                        if (i < limit && (s.GetChar(i) == ':')) {
                            i++;
                            while (i < limit && '0' <= (c = s.GetChar(i)) && c <= '9') {
                                minutesPart = (minutesPart * 10) + (c - '0');
                                i++;
                            }
                        }

                        n += minutesPart;
                    }
                    else {
                        n = (n % 100) + ((n / 100) * 60); // eg "GMT-0430"
                    }

                    if (prevc == '+') { // plus means east of GMT
                        n = -n;
                    }

                    tzoffset = n;
                }
                else if (n >= 70) {
                    if (year != IInteger::MIN_VALUE) {
                        return E_ILLEGAL_ARGUMENT_EXCEPTION;
                    }
                    else if (c <= ' ' || c == ',' || c == '/' || i >= limit) {
                        // year = n < 1900 ? n : n - 1900;
                        year = n;
                    }
                    else {
                        return E_ILLEGAL_ARGUMENT_EXCEPTION;
                    }
                }
                else if (c == ':') {
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
                else if (c == '/') {
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
                else if (i < limit && c != ',' && c > ' ' && c != '-') {
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
            else if (c == '/' || c == ':' || c == '+' || c == '-') {
                prevc = c;
            }
            else {
                Integer st = i - 1;
                while (i < limit) {
                    c = s.GetChar(i);
                    if (!(('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z'))) {
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
                AutoLock lock(GetLock());
                if (sDefaultCenturyStart == 0) {
                    AutoPtr<ICalendarDate> cdate;
                    ICalendarSystem::Probe(GetGcal())->GetCalendarDate((ICalendarDate**)&cdate);
                    Integer y;
                    cdate->GetYear(&y);
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
                    TimeZone::GetDefaultRef(), (ICalendarDate**)&ldate);
            ldate->SetDate(year, mon + 1, mday);
            ldate->SetTimeOfDay(hour, min, sec, 0);
            return ICalendarSystem::Probe(cal)->GetTime(ldate, date);
        }
        AutoPtr<ICalendarDate> udate;
        ICalendarSystem::Probe(cal)->NewCalendarDate(nullptr, (ICalendarDate**)&udate);
        udate->SetDate(year, mon + 1, mday);
        udate->SetTimeOfDay(hour, min, sec, 0);
        Long time;
        ICalendarSystem::Probe(cal)->GetTime(udate, &time);
        *date = time + tzoffset * (60 * 1000);
        return NOERROR;
    }
}

ECode Date::After(
    /* [in] */ IDate* when,
    /* [out] */ Boolean* after)
{
    return NOERROR;
}

ECode Date::Before(
    /* [in] */ IDate* when,
    /* [out] */ Boolean* before)
{
    return NOERROR;
}

ECode Date::GetDate(
    /* [out] */ Integer* date)
{
    return NOERROR;
}

ECode Date::GetDay(
    /* [out] */ Integer* day)
{
    return NOERROR;
}

ECode Date::GetHours(
    /* [out] */ Integer* hours)
{
    return NOERROR;
}

ECode Date::GetMinutes(
    /* [out] */ Integer* minutes)
{
    return NOERROR;
}

ECode Date::GetMonth(
    /* [out] */ Integer* month)
{
    return NOERROR;
}

ECode Date::GetSeconds(
    /* [out] */ Integer* seconds)
{
    return NOERROR;
}

ECode Date::GetTime(
    /* [out] */ Long* time)
{
    return NOERROR;
}

ECode Date::GetTimezoneOffset(
    /* [out] */ Integer* tzOffset)
{
    return NOERROR;
}

ECode Date::GetYear(
    /* [out] */ Integer* year)
{
    return NOERROR;
}

ECode Date::SetDate(
    /* [in] */ Integer date)
{
    return NOERROR;
}

ECode Date::SetHours(
    /* [in] */ Integer hours)
{
    return NOERROR;
}

ECode Date::SetMinutes(
    /* [in] */ Integer minutes)
{
    return NOERROR;
}

ECode Date::SetMonth(
    /* [in] */ Integer month)
{
    return NOERROR;
}

ECode Date::SetSeconds(
    /* [in] */ Integer seconds)
{
    return NOERROR;
}

ECode Date::SetTime(
    /* [in] */ Long time)
{
    return NOERROR;
}

ECode Date::SetYear(
    /* [in] */ Integer year)
{
    return NOERROR;
}

ECode Date::ToGMTString(
    /* [out] */ String* str)
{
    return NOERROR;
}

ECode Date::ToLocaleString(
    /* [out] */ String* str)
{
    return NOERROR;
}

ECode Date::CompareTo(
    /* [in] */ IInterface* other,
    /* [out] */ Integer* result)
{
    return NOERROR;
}

Long Date::GetTimeImpl()
{
    return 0;
}

AutoPtr<IBaseCalendar> Date::GetCalendarSystem(
    /* [in] */ Integer year)
{
    return nullptr;
}

}
}
