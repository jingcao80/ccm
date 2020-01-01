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

#include "como/util/Calendar.h"
#include "como/util/CalendarFactory.h"

namespace como {
namespace util {

ECode CalendarFactory::GetInstance(
    /* [out] */ ICalendar** cal)
{
    VALIDATE_NOT_NULL(cal);

    AutoPtr<ICalendar> c = Calendar::GetInstance();
    c.MoveTo(cal);
    return NOERROR;
}

ECode CalendarFactory::GetInstance(
    /* [in] */ ITimeZone* zone,
    /* [out] */ ICalendar** cal)
{
    VALIDATE_NOT_NULL(cal);

    AutoPtr<ICalendar> c = Calendar::GetInstance(zone);
    c.MoveTo(cal);
    return NOERROR;
}

ECode CalendarFactory::GetInstance(
    /* [in] */ ILocale* aLocale,
    /* [out] */ ICalendar** cal)
{
    VALIDATE_NOT_NULL(cal);

    AutoPtr<ICalendar> c = Calendar::GetInstance(aLocale);
    c.MoveTo(cal);
    return NOERROR;
}

ECode CalendarFactory::GetInstance(
    /* [in] */ ITimeZone* zone,
    /* [in] */ ILocale* aLocale,
    /* [out] */ ICalendar** cal)
{
    VALIDATE_NOT_NULL(cal);

    AutoPtr<ICalendar> c = Calendar::GetInstance(zone, aLocale);
    c.MoveTo(cal);
    return NOERROR;
}

ECode CalendarFactory::GetJapaneseImperialInstance(
    /* [in] */ ITimeZone* zone,
    /* [in] */ ILocale* aLocale,
    /* [out] */ ICalendar** cal)
{
    VALIDATE_NOT_NULL(cal);

    AutoPtr<ICalendar> c = Calendar::GetJapaneseImperialInstance(zone, aLocale);
    c.MoveTo(cal);
    return NOERROR;
}

}
}
