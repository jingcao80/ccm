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
#include "ccm/core/Math.h"
#include "ccm/core/System.h"
#include "ccm/util/JapaneseImperialCalendar.h"
#include "ccm/util/TimeZone.h"
#include "ccm/util/calendar/CalendarSystem.h"
#include "ccm/util/calendar/CEra.h"
#include "ccm.core.IInteger.h"
#include "ccm.core.ILong.h"
#include "ccm.util.calendar.IBaseCalendar.h"
#include "ccm.util.calendar.ICalendarDate.h"

using ccm::core::AutoLock;
using ccm::core::IInteger;
using ccm::core::ILong;
using ccm::core::Math;
using ccm::core::System;
using ccm::util::calendar::CalendarSystem;
using ccm::util::calendar::CEra;
using ccm::util::calendar::IBaseCalendar;
using ccm::util::calendar::ICalendarDate;
using ccm::util::calendar::ICalendarSystem;
using ccm::util::calendar::IID_IEra;

namespace ccm {
namespace util {

CCM_INTERFACE_IMPL_1(JapaneseImperialCalendar, Calendar, IJapaneseImperialCalendar);

Array<IEra*> JapaneseImperialCalendar::sEras;
Array<Long> JapaneseImperialCalendar::sSinceFixedDates;
constexpr Integer JapaneseImperialCalendar::MIN_VALUES[];
constexpr Integer JapaneseImperialCalendar::LEAST_MAX_VALUES[];
constexpr Integer JapaneseImperialCalendar::MAX_VALUES[];
Boolean JapaneseImperialCalendar::sInitialized = false;
SyncObject JapaneseImperialCalendar::sInitLock;

inline static AutoPtr<ICalendarSystem> GetLocalGregorianCalendar()
{
    AutoPtr<ICalendarSystem> cs;
    CalendarSystem::ForName(String("japanese"), &cs);
    return cs;
}

AutoPtr<ICalendarSystem> JapaneseImperialCalendar::GetJcal()
{
    static AutoPtr<ICalendarSystem> sJcal = GetLocalGregorianCalendar();
    return sJcal;
}

AutoPtr<ICalendarSystem> JapaneseImperialCalendar::GetGcal()
{
    static AutoPtr<ICalendarSystem> sGcal = ICalendarSystem::Probe(
            CalendarSystem::GetGregorianCalendar());
    return sGcal;
}

inline static AutoPtr<IEra> CreateEra(
    /* [in] */ const String& name,
    /* [in] */ const String& abbr,
    /* [in] */ Long since,
    /* [in] */ Boolean localTime)
{
    AutoPtr<IEra> era;
    CEra::New(name, abbr, since, localTime, IID_IEra, (IInterface**)&era);
    return era;
}

AutoPtr<IEra> JapaneseImperialCalendar::GetBEFORE_MEIJI_ERA()
{
    static AutoPtr<IEra> BEFORE_MEIJI_ERA = CreateEra(
            String("BeforeMeiji"), String("BM"), ILong::MIN_VALUE, false);
    return BEFORE_MEIJI_ERA;
}

ECode JapaneseImperialCalendar::StaticInitialize()
{
    AutoLock lock(sInitLock);

    if (sInitialized) return NOERROR;

    Array<IEra*> es;
    GetJcal()->GetEras(&es);
    Integer length = es.GetLength() + 1;
    sEras = Array<IEra*>(length);
    sSinceFixedDates = Array<Long>(length);

    // eras[BEFORE_MEIJI] and sinceFixedDate[BEFORE_MEIJI] are the
    // same as Gregorian.
    Integer index = BEFORE_MEIJI;
    AutoPtr<ICalendarDate> date;
    GetBEFORE_MEIJI_ERA()->GetSinceDate(&date);
    IBaseCalendar::Probe(GetGcal())->GetFixedDate(date, &sSinceFixedDates[index]);
    sEras.Set(index++, GetBEFORE_MEIJI_ERA());
    for (Integer i = 0; i < es.GetLength(); i++) {
        IEra* e = es[i];
        AutoPtr<ICalendarDate> d;
        e->GetSinceDate(&d);
        IBaseCalendar::Probe(GetGcal())->GetFixedDate(d, &sSinceFixedDates[index]);
        sEras.Set(index++, e);
    }

    *const_cast<Integer*>(&LEAST_MAX_VALUES[ERA]) = sEras.GetLength() - 1;
    *const_cast<Integer*>(&MAX_VALUES[ERA]) = sEras.GetLength() - 1;

    // Calculate the least maximum year and least day of Year
    // values. The following code assumes that there's at most one
    // era transition in a Gregorian year.
    Integer year = IInteger::MAX_VALUE;
    Integer dayOfYear = IInteger::MAX_VALUE;
    date = nullptr;
    GetGcal()->NewCalendarDate(TimeZone::NO_TIMEZONE, &date);
    for (Integer i = 0; i < sEras.GetLength(); i++) {
        Long fd = sSinceFixedDates[i];
        AutoPtr<ICalendarDate> transitionDate;
        sEras[i]->GetSinceDate(&transitionDate);
        Integer year;
        transitionDate->GetYear(&year);
        date->SetDate(year, IBaseCalendar::JANUARY, 1);
        Long fdd;
        IBaseCalendar::Probe(GetGcal())->GetFixedDate(date, &fdd);
        if (fd != fdd) {
            dayOfYear = Math::Min((Integer)(fd - fdd) + 1, dayOfYear);
        }
        date->SetDate(year, IBaseCalendar::DECEMBER, 31);
        IBaseCalendar::Probe(GetGcal())->GetFixedDate(date, &fdd);
        if (fd != fdd) {
            dayOfYear = Math::Min((Integer)(fdd - fd) + 1, dayOfYear);
        }
        AutoPtr<LocalGregorianCalendar::Date> lgd = GetCalendarDate(fd - 1);
        Integer y;
        lgd->GetYear(&y);
        // Unless the first year starts from January 1, the actual
        // max value could be one year short. For example, if it's
        // Showa 63 January 8, 63 is the actual max value since
        // Showa 64 January 8 doesn't exist.
        Integer m, d;
        if (!((lgd->GetMonth(&m), m) == IBaseCalendar::JANUARY && (lgd->GetDayOfMonth(&d), d) == 1)) {
            y--;
        }
        year = Math::Min(y, year);
    }

    *const_cast<Integer*>(&LEAST_MAX_VALUES[YEAR]) = year; // Max year could be smaller than this value.
    *const_cast<Integer*>(&LEAST_MAX_VALUES[DAY_OF_YEAR]) = dayOfYear;

    sInitialized = true;

    return NOERROR;
}

ECode JapaneseImperialCalendar::Constructor(
    /* [in] */ ITimeZone* zone,
    /* [in] */ ILocale* locale)
{
    StaticInitialize();
    Calendar::Constructor(zone, locale);
    AutoPtr<ICalendarDate> date;
    GetJcal()->NewCalendarDate(zone, &date);
    mJdate = (LocalGregorianCalendar::Date*)date.Get();
    SetTimeInMillis(System::GetCurrentTimeMillis());
    return NOERROR;
}

ECode JapaneseImperialCalendar::Constructor(
    /* [in] */ ITimeZone* zone,
    /* [in] */ ILocale* locale,
    /* [in] */ Boolean flag)
{
    StaticInitialize();
    Calendar::Constructor(zone, locale);
    AutoPtr<ICalendarDate> date;
    GetJcal()->NewCalendarDate(zone, &date);
    mJdate = (LocalGregorianCalendar::Date*)date.Get();
    return NOERROR;
}

ECode JapaneseImperialCalendar::GetCalendarType(
    /* [out] */ String* type)
{
    VALIDATE_NOT_NULL(type);

    *type = "japanese";
    return NOERROR;
}

ECode JapaneseImperialCalendar::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* same)
{
    VALIDATE_NOT_NULL(same);

    if (IJapaneseImperialCalendar::Probe(obj) == nullptr) {
        *same = false;
        return NOERROR;
    }
    return Calendar::Equals(obj, same);
}

ECode JapaneseImperialCalendar::GetHashCode(
    /* [out] */ Integer* hash)
{
    VALIDATE_NOT_NULL(hash);

    Integer chash, jhash;
    Calendar::GetHashCode(&chash);
    mJdate->GetHashCode(&jhash);
    *hash = chash + jhash;
    return NOERROR;
}

}
}
