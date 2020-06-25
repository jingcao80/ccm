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

#include "como/core/System.h"
#include "como/util/calendar/Gregorian.h"

using como::core::System;

namespace como {
namespace util {
namespace calendar {

COMO_INTERFACE_IMPL_1(Gregorian, BaseCalendar, IGregorian);

ECode Gregorian::Constructor()
{
    return BaseCalendar::Constructor();
}

ECode Gregorian::GetName(
    /* [out] */ String& name)
{
    name = "gregorian";
    return NOERROR;
}

ECode Gregorian::GetCalendarDate(
    /* [out] */ AutoPtr<ICalendarDate>& date)
{
    NewCalendarDate(date);
    return GetCalendarDate(System::GetCurrentTimeMillis(), date);
}

ECode Gregorian::GetCalendarDate(
    /* [in] */ Long millis,
    /* [out] */ AutoPtr<ICalendarDate>& date)
{
    NewCalendarDate(date);
    return GetCalendarDate(millis, date);
}

ECode Gregorian::GetCalendarDate(
    /* [in] */ Long millis,
    /* [in] */ ICalendarDate* date)
{
    return BaseCalendar::GetCalendarDate(millis, date);
}

ECode Gregorian::GetCalendarDate(
    /* [in] */ Long millis,
    /* [in] */ ITimeZone* zone,
    /* [out] */ AutoPtr<ICalendarDate>& date)
{
    NewCalendarDate(zone, date);
    return GetCalendarDate(millis, date);
}

ECode Gregorian::NewCalendarDate(
    /* [out] */ AutoPtr<ICalendarDate>& date)
{
    AutoPtr<Gregorian::Date> d = new Gregorian::Date();
    ECode ec = d->Constructor();
    if (FAILED(ec)) return ec;
    date = d.Get();
    return NOERROR;
}

ECode Gregorian::NewCalendarDate(
    /* [in] */ ITimeZone* zone,
    /* [out] */ AutoPtr<ICalendarDate>& date)
{
    AutoPtr<Gregorian::Date> d = new Gregorian::Date();
    ECode ec = d->Constructor(zone);
    if (FAILED(ec)) return ec;
    date = d.Get();
    return NOERROR;
}

//-------------------------------------------------------------------------

ECode Gregorian::Date::Constructor()
{
    return BaseCalendar::Date::Constructor();
}

ECode Gregorian::Date::Constructor(
    /* [in] */ ITimeZone* zone)
{
    return BaseCalendar::Date::Constructor(zone);
}

ECode Gregorian::Date::GetNormalizedYear(
    /* [out] */ Integer& normalizedYear)
{
    return GetYear(normalizedYear);
}

ECode Gregorian::Date::SetNormalizedYear(
    /* [in] */ Integer normalizedYear)
{
    return SetYear(normalizedYear);
}

ECode Gregorian::Date::Clone(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<Gregorian::Date> date = new Gregorian::Date();
    FAIL_RETURN(CloneImpl(date.Get()));
    *obj = date->Probe(iid);
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

}
}
}
