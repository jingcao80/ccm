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
#include "como/core/StringUtils.h"
#include "como/core/System.h"
#include "como/util/CArrayList.h"
#include "como/util/CStringTokenizer.h"
#include "como/util/calendar/CalendarUtils.h"
#include "como/util/calendar/CEra.h"
#include "como/util/calendar/LocalGregorianCalendar.h"
#include <comolog.h>

using como::core::CStringBuffer;
using como::core::IStringBuffer;
using como::core::IID_IStringBuffer;
using como::core::StringUtils;
using como::core::System;

namespace como {
namespace util {
namespace calendar {

COMO_INTERFACE_IMPL_1(LocalGregorianCalendar, BaseCalendar, ILocalGregorianCalendar);

ECode LocalGregorianCalendar::GetLocalGregorianCalendar(
    /* [in] */ const String& name,
    /* [out] */ AutoPtr<ILocalGregorianCalendar>& calendar)
{
    AutoPtr<IProperties> calendarProps;
    ECode ec = GetCalendarProperties(calendarProps);
    if (FAILED(ec)) {
        return E_RUNTIME_EXCEPTION;
    }

    // Parse calendar.*.eras
    String props;
    calendarProps->GetProperty(String("calendar.") + name + ".eras", &props);
    if (props.IsNull()) {
        calendar = nullptr;
        return NOERROR;
    }
    AutoPtr<IList> eras;
    CArrayList::New(IID_IList, (IInterface**)&eras);
    AutoPtr<IStringTokenizer> eraTokens;
    CStringTokenizer::New(props, String(";"), IID_IStringTokenizer, (IInterface**)&eraTokens);
    Boolean hasMore;
    while (eraTokens->HasMoreTokens(hasMore), hasMore) {
        String items;
        eraTokens->NextToken(&items);
        items.Trim();
        AutoPtr<IStringTokenizer> itemTokens;
        CStringTokenizer::New(items, String(","), IID_IStringTokenizer, (IInterface**)&itemTokens);
        String eraName;
        Boolean localTime = true;
        Long since = 0;
        String abbr;

        Boolean itemHasMore;
        while (itemTokens->HasMoreTokens(itemHasMore), itemHasMore) {
            String item;
            itemTokens->NextToken(&item);
            Integer index = item.IndexOf(U'=');
            // it must be in the key=value form.
            if (index == -1) {
                calendar = nullptr;
                return NOERROR;
            }
            String key = item.Substring(0, index);
            String value = item.Substring(index + 1);
            if (String("name").Equals(key)) {
                eraName = value;
            }
            else if (String("since").Equals(key)) {
                if (value.EndsWith("u")) {
                    localTime = false;
                    StringUtils::ParseLong(value.Substring(0, value.GetLength() - 1), since);
                }
                else {
                    StringUtils::ParseLong(value, since);
                }
            }
            else if (String("abbr").Equals(key)) {
                abbr = value;
            }
            else {
                Logger::E("LocalGregorianCalendar", "Unknown key word: %s", key.string());
                return E_RUNTIME_EXCEPTION;
            }
        }
        AutoPtr<IEra> era;
        CEra::New(eraName, abbr, since, localTime, IID_IEra, (IInterface**)&era);
        eras->Add(era);
    }
    Boolean empty;
    if (eras->IsEmpty(empty), empty) {
        Logger::E("LocalGregorianCalendar", "No eras for %s", name.string());
        return E_RUNTIME_EXCEPTION;
    }
    Array<IEra*> eraArray;
    eras->ToArray(IID_IEra, (Array<IInterface*>*)&eraArray);

    AutoPtr<LocalGregorianCalendar> lgcal = new LocalGregorianCalendar();
    FAIL_RETURN(lgcal->Constructor(name, eraArray));
    calendar = lgcal.Get();
    return NOERROR;
}

ECode LocalGregorianCalendar::Constructor(
    /* [in] */ const String& name,
    /* [in] */ const Array<IEra*>& eras)
{
    mName = name;
    mEras = eras;
    SetEras(eras);
    return NOERROR;
}

ECode LocalGregorianCalendar::GetName(
    /* [out] */ String& name)
{
    name = mName;
    return NOERROR;
}

ECode LocalGregorianCalendar::GetCalendarDate(
    /* [out] */ AutoPtr<ICalendarDate>& date)
{
    NewCalendarDate(date);
    return GetCalendarDate(System::GetCurrentTimeMillis(), date);
}

ECode LocalGregorianCalendar::GetCalendarDate(
    /* [in] */ Long millis,
    /* [out] */ AutoPtr<ICalendarDate>& date)
{
    NewCalendarDate(date);
    return GetCalendarDate(millis, date);
}

ECode LocalGregorianCalendar::GetCalendarDate(
    /* [in] */ Long millis,
    /* [in] */ ITimeZone* zone,
    /* [out] */ AutoPtr<ICalendarDate>& date)
{
    NewCalendarDate(zone, date);
    return GetCalendarDate(millis, date);
}

ECode LocalGregorianCalendar::GetCalendarDate(
    /* [in] */ Long millis,
    /* [in] */ ICalendarDate* date)
{
    BaseCalendar::GetCalendarDate(millis, date);
    LocalGregorianCalendar::Date* ldate = (LocalGregorianCalendar::Date*)date;
    Integer zoneOffset;
    ldate->GetZoneOffset(zoneOffset);
    AdjustYear(ldate, millis, zoneOffset);
    return NOERROR;
}

void LocalGregorianCalendar::AdjustYear(
    /* [in] */ Date* ldate,
    /* [in] */ Long millis,
    /* [in] */ Integer zoneOffset)
{
    Integer i;
    for (i = mEras.GetLength() - 1; i >= 0; --i) {
        IEra* era = mEras[i];
        Long since;
        era->GetSince(nullptr, since);
        Boolean local;
        if (era->IsLocalTime(local), local) {
            since -= zoneOffset;
        }
        if (millis >= since) {
            ldate->SetLocalEra(era);
            Integer normYear;
            ldate->GetNormalizedYear(normYear);
            AutoPtr<ICalendarDate> sinceDate;
            era->GetSinceDate(sinceDate);
            Integer year;
            sinceDate->GetYear(year);
            Integer y = normYear - year + 1;
            ldate->SetLocalYear(y);
            break;
        }
    }
    if (i < 0) {
        ldate->SetLocalEra(nullptr);
        Integer year;
        ldate->GetNormalizedYear(year);
        ldate->SetLocalYear(year);
    }
    ldate->SetNormalized(true);
}

ECode LocalGregorianCalendar::NewCalendarDate(
    /* [out] */ AutoPtr<ICalendarDate>& date)
{
    AutoPtr<LocalGregorianCalendar::Date> d = new LocalGregorianCalendar::Date();
    ECode ec = d->Constructor();
    if (FAILED(ec)) return ec;
    date = d.Get();
    return NOERROR;
}

ECode LocalGregorianCalendar::NewCalendarDate(
    /* [in] */ ITimeZone* zone,
    /* [out] */ AutoPtr<ICalendarDate>& date)
{
    AutoPtr<LocalGregorianCalendar::Date> d = new LocalGregorianCalendar::Date();
    ECode ec = d->Constructor(zone);
    if (FAILED(ec)) return ec;
    date = d.Get();
    return NOERROR;
}

ECode LocalGregorianCalendar::Validate(
    /* [in] */ ICalendarDate* date,
    /* [out] */ Boolean& result)
{
    LocalGregorianCalendar::Date* ldate = (LocalGregorianCalendar::Date*)date;
    AutoPtr<IEra> era;
    ldate->GetEra(era);
    if (era != nullptr) {
        if (!ValidateEra(era)) {
            result = false;
            return NOERROR;
        }
        AutoPtr<ICalendarDate> sinceDate;
        era->GetSinceDate(sinceDate);
        Integer sinceYear, year;
        sinceDate->GetYear(sinceYear);
        ldate->GetYear(year);
        ldate->SetNormalizedYear(sinceYear + year - 1);
        AutoPtr<ITimeZone> zone;
        date->GetZone(zone);
        AutoPtr<ICalendarDate> tmp;
        NewCalendarDate(zone, tmp);
        tmp->SetEra(era);
        Integer y, m, dom;
        date->GetYear(y);
        date->GetMonth(m);
        date->GetDayOfMonth(dom);
        tmp->SetDate(y, m, dom);
        Normalize(tmp);
        AutoPtr<IEra> tmpEra;
        if (tmp->GetEra(tmpEra), tmpEra != era) {
            result = false;
            return NOERROR;
        }
    }
    else {
        AutoPtr<ICalendarDate> sinceDate;
        mEras[0]->GetSinceDate(sinceDate);
        Integer sinceYear, year;
        if (date->GetYear(year), sinceDate->GetYear(sinceYear), year >= sinceYear) {
            result = false;
            return NOERROR;
        }
        ldate->SetNormalizedYear(year);
    }
    return BaseCalendar::Validate(date, result);
}

Boolean LocalGregorianCalendar::ValidateEra(
    /* [in] */ IEra* era)
{
    // Validate the era
    for (Integer i = 0; i < mEras.GetLength(); i++) {
        if (era == mEras[i]) {
            return true;
        }
    }
    return false;
}

ECode LocalGregorianCalendar::Normalize(
    /* [in] */ ICalendarDate* date,
    /* [out] */ Boolean* result)
{
    Boolean normalized;
    if (date->IsNormalized(normalized), normalized) {
        if (result != nullptr) *result = true;
        return NOERROR;
    }

    NormalizeYear(date);
    LocalGregorianCalendar::Date* ldate = (LocalGregorianCalendar::Date*)date;

    BaseCalendar::Normalize(date);

    Boolean hasMillis = false;
    Long millis = 0;
    Integer year;
    ldate->GetNormalizedYear(year);
    Integer i;
    AutoPtr<IEra> era;
    for (i = mEras.GetLength() - 1; i >= 0; --i) {
        era = mEras[i];
        Boolean local;
        if (era->IsLocalTime(local), local) {
            AutoPtr<ICalendarDate> sinceDate;
            era->GetSinceDate(sinceDate);
            Integer sinceYear;
            sinceDate->GetYear(sinceYear);
            if (year > sinceYear) {
                break;
            }
            if (year == sinceYear) {
                Integer month, sinceMonth;
                ldate->GetMonth(month);
                sinceDate->GetMonth(sinceMonth);
                if (month > sinceMonth) {
                    break;
                }
                if (month == sinceMonth) {
                    Integer day, sinceDay;
                    ldate->GetDayOfMonth(day);
                    sinceDate->GetDayOfMonth(sinceDay);
                    if (day > sinceDay) {
                        break;
                    }
                    if (day == sinceDay) {
                        Long timeOfDay, sinceTimeOfDay;
                        ldate->GetTimeOfDay(timeOfDay);
                        sinceDate->GetTimeOfDay(sinceTimeOfDay);
                        if (timeOfDay >= sinceTimeOfDay) {
                            break;
                        }
                        --i;
                        break;
                    }
                }
            }
        }
        else {
            if (!hasMillis) {
                BaseCalendar::GetTime(date, &millis);
                hasMillis = true;
            }

            AutoPtr<ITimeZone> zone;
            date->GetZone(zone);
            Long since;
            era->GetSince(zone, since);
            if (millis >= since) {
                break;
            }
        }
    }
    if (i >= 0) {
        ldate->SetLocalEra(era);
        AutoPtr<ICalendarDate> sinceDate;
        era->GetSinceDate(sinceDate);
        Integer sinceYear, year;
        sinceDate->GetYear(sinceYear);
        ldate->GetNormalizedYear(year);
        ldate->SetLocalYear(year - sinceYear + 1);
    }
    else {
        // Set Gregorian year with no era
        ldate->SetEra(nullptr);
        ldate->SetLocalYear(year);
        ldate->SetNormalizedYear(year);
    }
    ldate->SetNormalized(true);
    if (result != nullptr) *result = true;
    return NOERROR;
}

void LocalGregorianCalendar::NormalizeMonth(
    /* [in] */ ICalendarDate* date)
{
    NormalizeYear(date);
    BaseCalendar::NormalizeMonth(date);
}

void LocalGregorianCalendar::NormalizeYear(
    /* [in] */ ICalendarDate* date)
{
    LocalGregorianCalendar::Date* ldate = (LocalGregorianCalendar::Date*)date;
    // Set the supposed-to-be-correct Gregorian year first
    // e.g., Showa 90 becomes 2015 (1926 + 90 - 1).
    AutoPtr<IEra> era;
    ldate->GetEra(era);
    if (era == nullptr || !ValidateEra(era)) {
        Integer year;
        ldate->GetYear(year);
        ldate->SetNormalizedYear(year);
    }
    else {
        AutoPtr<ICalendarDate> sinceDate;
        era->GetSinceDate(sinceDate);
        Integer sinceYear, year;
        sinceDate->GetYear(sinceYear);
        ldate->GetYear(year);
        ldate->SetNormalizedYear(sinceYear + year - 1);
    }
}

ECode LocalGregorianCalendar::IsLeapYear(
    /* [in] */ Integer gregorianYear,
    /* [out] */ Boolean& leapYear)
{
    leapYear = CalendarUtils::IsGregorianLeapYear(gregorianYear);
    return NOERROR;
}

ECode LocalGregorianCalendar::IsLeapYear(
    /* [in] */ IEra* era,
    /* [in] */ Integer year,
    /* [out] */ Boolean& leapYear)
{
    if (era == nullptr) {
        return IsLeapYear(year, leapYear);
    }
    AutoPtr<ICalendarDate> sinceDate;
    era->GetSinceDate(sinceDate);
    Integer sinceYear;
    sinceDate->GetYear(sinceYear);
    Integer gyear = sinceYear + year - 1;
    return IsLeapYear(gyear, leapYear);
}

ECode LocalGregorianCalendar::GetCalendarDateFromFixedDate(
    /* [in] */ ICalendarDate* date,
    /* [in] */ Long fixedDate)
{
    BaseCalendar::GetCalendarDateFromFixedDate(date, fixedDate);
    LocalGregorianCalendar::Date* ldate = (LocalGregorianCalendar::Date*)date;
    AdjustYear(ldate, (fixedDate - EPOCH_OFFSET) * DAY_IN_MILLIS, 0);
    return NOERROR;
}

//-------------------------------------------------------------------------

ECode LocalGregorianCalendar::Date::Constructor()
{
    return BaseCalendar::Date::Constructor();
}

ECode LocalGregorianCalendar::Date::Constructor(
    /* [in] */ ITimeZone* zone)
{
    return BaseCalendar::Date::Constructor(zone);
}

ECode LocalGregorianCalendar::Date::SetEra(
    /* [in] */ IEra* era)
{
    AutoPtr<IEra> thisEra;
    if (GetEra(thisEra), thisEra != era) {
        BaseCalendar::Date::SetEra(era);
        mGregorianYear = FIELD_UNDEFINED;
    }
    return NOERROR;
}

ECode LocalGregorianCalendar::Date::AddYear(
    /* [in] */ Integer n)
{
    BaseCalendar::Date::AddYear(n);
    mGregorianYear += n;
    return NOERROR;
}

ECode LocalGregorianCalendar::Date::SetYear(
    /* [in] */ Integer year)
{
    Integer thisYear;
    if (GetYear(thisYear), thisYear != year) {
        BaseCalendar::Date::SetYear(year);
        mGregorianYear = FIELD_UNDEFINED;
    }
    return NOERROR;
}

ECode LocalGregorianCalendar::Date::GetNormalizedYear(
    /* [out] */ Integer& normalizedYear)
{
    normalizedYear = mGregorianYear;
    return NOERROR;
}

ECode LocalGregorianCalendar::Date::SetNormalizedYear(
    /* [in] */ Integer normalizedYear)
{
    mGregorianYear = normalizedYear;
    return NOERROR;
}

void LocalGregorianCalendar::Date::SetLocalEra(
    /* [in] */ IEra* era)
{
    BaseCalendar::Date::SetEra(era);
}

void LocalGregorianCalendar::Date::SetLocalYear(
    /* [in] */ Integer year)
{
    BaseCalendar::Date::SetYear(year);
}

ECode LocalGregorianCalendar::Date::ToString(
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
        }
    }
    Integer year, month, dom;
    GetYear(year);
    sb->Append(year);
    sb->Append(U'.');
    GetMonth(month);
    CalendarUtils::Sprintf0d(sb, month, 2);
    sb->Append(U'.');
    GetDayOfMonth(dom);
    CalendarUtils::Sprintf0d(sb, dom, 2);
    sb->Append(time);
    return sb->ToString(desc);
}

ECode LocalGregorianCalendar::Date::Clone(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<LocalGregorianCalendar::Date> date = new LocalGregorianCalendar::Date();
    FAIL_RETURN(CloneImpl(date.Get()));
    date->mGregorianYear = mGregorianYear;
    *obj = date->Probe(iid);
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

}
}
}
