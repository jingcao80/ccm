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

#include "coredef.h"
#include "ccm/core/AutoLock.h"
#include "ccm/core/CoreUtils.h"
#include "ccm/core/Math.h"
#include "ccm/core/System.h"
#include "ccm/util/CGregorianCalendar.h"
#include "ccm/util/CHashSet.h"
#include "ccm/util/JapaneseImperialCalendar.h"
#include "ccm/util/TimeZone.h"
#include "ccm/util/calendar/CalendarSystem.h"
#include "ccm/util/calendar/CalendarUtils.h"
#include "ccm/util/calendar/CEra.h"
#include "ccm/util/locale/provider/CalendarDataUtility.h"
#include "ccm.core.IInteger.h"
#include "ccm.util.calendar.IBaseCalendar.h"
#include <ccmlogger.h>

using ccm::core::AutoLock;
using ccm::core::CoreUtils;
using ccm::core::IInteger;
using ccm::core::Math;
using ccm::core::System;
using ccm::util::CHashSet;
using ccm::util::calendar::CalendarSystem;
using ccm::util::calendar::CalendarUtils;
using ccm::util::calendar::CEra;
using ccm::util::calendar::IBaseCalendar;
using ccm::util::calendar::ICalendarSystem;
using ccm::util::calendar::IID_ICalendarDate;
using ccm::util::calendar::IID_IEra;
using ccm::util::locale::provider::CalendarDataUtility;

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

ECode JapaneseImperialCalendar::Add(
    /* [in] */ Integer field,
    /* [in] */ Integer amount)
{
    // If amount == 0, do nothing even the given field is out of
    // range.
    if (amount == 0) {
        return NOERROR;   // Do nothing!
    }

    if (field < 0 || field >= ZONE_OFFSET) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // Sync the time and calendar fields.
    Complete();

    if (field == YEAR) {
        AutoPtr<ICalendarDate> date;
        mJdate->Clone(IID_ICalendarDate, (IInterface**)&date);
        LocalGregorianCalendar::Date* d = (LocalGregorianCalendar::Date*)date.Get();
        d->AddYear(amount);
        PinDayOfMonth(d);
        Integer y, m, day;
        d->GetYear(&y);
        d->GetMonth(&m);
        d->GetDayOfMonth(&day);
        Set(ERA, GetEraIndex(d));
        Set(YEAR, y);
        Set(MONTH, m - 1);
        Set(DAY_OF_MONTH, day);
    }
    else if (field == MONTH) {
        AutoPtr<ICalendarDate> date;
        mJdate->Clone(IID_ICalendarDate, (IInterface**)&date);
        LocalGregorianCalendar::Date* d = (LocalGregorianCalendar::Date*)date.Get();
        d->AddMonth(amount);
        PinDayOfMonth(d);
        Integer y, m, day;
        d->GetYear(&y);
        d->GetMonth(&m);
        d->GetDayOfMonth(&day);
        Set(ERA, GetEraIndex(d));
        Set(YEAR, y);
        Set(MONTH, m - 1);
        Set(DAY_OF_MONTH, day);
    }
    else if (field == ERA) {
        Integer era = InternalGet(ERA) + amount;
        if (era < 0) {
            era = 0;
        }
        else if (era > sEras.GetLength() - 1) {
            era = sEras.GetLength() - 1;
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
                delta *= 60 * 60 * 1000;        // hours to milliseconds
                break;

            case MINUTE:
                delta *= 60 * 1000;             // minutes to milliseconds
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
        Long fd = mCachedFixedDate;
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
        Integer zoneOffset = InternalGet(ZONE_OFFSET) + InternalGet(DST_OFFSET);
        SetTimeInMillis((fd - EPOCH_OFFSET) * ONE_DAY + timeOfDay - zoneOffset);
        zoneOffset -= InternalGet(ZONE_OFFSET) + InternalGet(DST_OFFSET);
        // If the time zone offset has changed, then adjust the difference.
        if (zoneOffset != 0) {
            SetTimeInMillis(mTime + zoneOffset);
            Long fd2 = mCachedFixedDate;
            // If the adjustment has changed the date, then take
            // the previous one.
            if (fd2 != fd) {
                SetTimeInMillis(mTime - zoneOffset);
            }
        }
    }
    return NOERROR;
}

ECode JapaneseImperialCalendar::Roll(
    /* [in] */ Integer field,
    /* [in] */ Boolean up)
{
    return Roll(field, up ? 1 : -1);
}

ECode JapaneseImperialCalendar::Roll(
    /* [in] */ Integer field,
    /* [in] */ Integer amount)
{
    // If amount == 0, do nothing even the given field is out of
    // range. This is tested by JCK.
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

    switch (field) {
        case ERA:
        case AM_PM:
        case MINUTE:
        case SECOND:
        case MILLISECOND:
            // These fields are handled simply, since they have fixed
            // minima and maxima. Other fields are complicated, since
            // the range within they must roll varies depending on the
            // date, a time zone and the era transitions.
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
            GetJcal()->GetCalendarDate(mTime, GetZone(), &d);
            Integer dayOfMonth;
            if (InternalGet(DAY_OF_MONTH) != (d->GetDayOfMonth(&dayOfMonth), dayOfMonth)) {
                AutoPtr<IEra> era;
                mJdate->GetEra(&era);
                d->SetEra(era);
                d->SetDate(InternalGet(YEAR),
                           InternalGet(MONTH) + 1,
                           InternalGet(DAY_OF_MONTH));
                if (field == HOUR) {
                    CHECK(InternalGet(AM_PM) == PM);
                    d->AddHours(+12); // restore PM
                }
                GetJcal()->GetTime(d, &mTime);
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

        case YEAR:
            GetActualMinimum(field, &min);
            GetActualMaximum(field, &max);
            break;

        case MONTH:
        // Rolling the month involves both pinning the final value to [0, 11]
        // and adjusting the DAY_OF_MONTH if necessary.  We only adjust the
        // DAY_OF_MONTH if, after updating the MONTH field, it is illegal.
        // E.g., <jan31>.roll(MONTH, 1) -> <feb28> or <feb29>.
        {
            Integer normYear;
            mJdate->GetNormalizedYear(&normYear);
            if (!IsTransitionYear(normYear)) {
                Integer year;
                mJdate->GetYear(&year);
                Integer maxYear, minYear;
                if (year == (GetMaximum(YEAR, &maxYear), maxYear)) {
                    AutoPtr<ICalendarDate> jd, d;
                    GetJcal()->GetCalendarDate(mTime, GetZone(), &jd);
                    GetJcal()->GetCalendarDate(ILong::MAX_VALUE, GetZone(), &d);
                    Integer m;
                    max = (d->GetMonth(&m), m) - 1;
                    Integer n = GetRolledValue(InternalGet(field), amount, min, max);
                    if (n == max) {
                        // To avoid overflow, use an equivalent year.
                        jd->AddYear(-400);
                        jd->SetMonth(n + 1);
                        Integer jdDayOfMonth, dDayOfMonth;
                        if ((jd->GetDayOfMonth(&jdDayOfMonth), jdDayOfMonth) >
                                (d->GetDayOfMonth(&dDayOfMonth), dDayOfMonth)) {
                            jd->SetDayOfMonth(dDayOfMonth);
                            GetJcal()->Normalize(jd);
                        }
                        Long jdTimeOfDay, dTimeOfDay;
                        if ((jd->GetDayOfMonth(&jdDayOfMonth), jdDayOfMonth) ==
                                (d->GetDayOfMonth(&dDayOfMonth), dDayOfMonth)
                                && (jd->GetTimeOfDay(&jdTimeOfDay), jdTimeOfDay) >
                                (d->GetTimeOfDay(&dTimeOfDay), dTimeOfDay)) {
                            jd->SetMonth(n + 1);
                            jd->SetDayOfMonth(dDayOfMonth - 1);
                            GetJcal()->Normalize(jd);
                            // Month may have changed by the normalization.
                            n = (jd->GetMonth(&m), m) - 1;
                        }
                        Set(DAY_OF_MONTH, (jd->GetDayOfMonth(&jdDayOfMonth), jdDayOfMonth));
                    }
                    Set(MONTH, n);
                }
                else if (year == (GetMinimum(YEAR, &minYear), minYear)) {
                    AutoPtr<ICalendarDate> jd, d;
                    GetJcal()->GetCalendarDate(mTime, GetZone(), &jd);
                    GetJcal()->GetCalendarDate(ILong::MIN_VALUE, GetZone(), &d);
                    Integer m;
                    min = (d->GetMonth(&m), m) - 1;
                    Integer n = GetRolledValue(InternalGet(field), amount, min, max);
                    if (n == min) {
                        // To avoid underflow, use an equivalent year.
                        jd->AddYear(+400);
                        jd->SetMonth(n + 1);
                        Integer jdDayOfMonth, dDayOfMonth;
                        if ((jd->GetDayOfMonth(&jdDayOfMonth), jdDayOfMonth) <
                                (d->GetDayOfMonth(&dDayOfMonth), dDayOfMonth)) {
                            jd->SetDayOfMonth(dDayOfMonth);
                            GetJcal()->Normalize(jd);
                        }
                        Long jdTimeOfDay, dTimeOfDay;
                        if ((jd->GetDayOfMonth(&jdDayOfMonth), jdDayOfMonth) ==
                                (d->GetDayOfMonth(&dDayOfMonth), dDayOfMonth)
                                && (jd->GetTimeOfDay(&jdTimeOfDay), jdTimeOfDay) <
                                (d->GetTimeOfDay(&dTimeOfDay), dTimeOfDay)) {
                            jd->SetMonth(n + 1);
                            jd->SetDayOfMonth(dDayOfMonth - 1);
                            GetJcal()->Normalize(jd);
                            // Month may have changed by the normalization.
                            n = (jd->GetMonth(&m), m) - 1;
                        }
                        Set(DAY_OF_MONTH, (jd->GetDayOfMonth(&jdDayOfMonth), jdDayOfMonth));
                    }
                    Set(MONTH, n);
                }
                else {
                    Integer mon = (InternalGet(MONTH) + amount) % 12;
                    if (mon < 0) {
                        mon += 12;
                    }
                    Set(MONTH, mon);

                    // Keep the day of month in the range.  We
                    // don't want to spill over into the next
                    // month; e.g., we don't want jan31 + 1 mo ->
                    // feb31 -> mar3.
                    Integer monthLen = MonthLength(mon);
                    if (InternalGet(DAY_OF_MONTH) > monthLen) {
                        Set(DAY_OF_MONTH, monthLen);
                    }
                }
            }
            else {
                Integer eraIndex = GetEraIndex(mJdate);
                AutoPtr<ICalendarDate> transition;
                Integer year, month;
                if ((mJdate->GetYear(&year), year) == 1) {
                    sEras[eraIndex]->GetSinceDate(&transition);
                    min = (transition->GetMonth(&month), month) - 1;
                }
                else {
                    if (eraIndex < sEras.GetLength() - 1) {
                        sEras[eraIndex + 1]->GetSinceDate(&transition);
                        Integer normYear;
                        if ((transition->GetYear(&year), year) ==
                                (mJdate->GetNormalizedYear(&normYear), normYear)) {
                            max = (transition->GetMonth(&month), month) - 1;
                            Integer dayOfMonth;
                            if ((transition->GetDayOfMonth(&dayOfMonth), dayOfMonth) == 1) {
                                max--;
                            }
                        }
                    }
                }

                if (min == max) {
                    // The year has only one month. No need to
                    // process further. (Showa Gan-nen (year 1)
                    // and the last year have only one month.)
                    return NOERROR;
                }
                Integer n = GetRolledValue(InternalGet(field), amount, min, max);
                Set(MONTH, n);
                if (n == min) {
                    Integer jdDayOfMonth, tDayOfMonth;
                    if (!((transition->GetMonth(&month), month) == IBaseCalendar::JANUARY
                          && (transition->GetDayOfMonth(&tDayOfMonth), tDayOfMonth) == 1)) {
                        if ((mJdate->GetDayOfMonth(&jdDayOfMonth), jdDayOfMonth) < tDayOfMonth) {
                            Set(DAY_OF_MONTH, tDayOfMonth);
                        }
                    }
                }
                else if (n == max && ((transition->GetMonth(&month), month) - 1 == n)) {
                    Integer tDayOfMonth, jdDayOfMonth;
                    transition->GetDayOfMonth(&tDayOfMonth);
                    if ((mJdate->GetDayOfMonth(&jdDayOfMonth), jdDayOfMonth) >= tDayOfMonth) {
                        Set(DAY_OF_MONTH, tDayOfMonth - 1);
                    }
                }
            }
            return NOERROR;
        }

        case WEEK_OF_YEAR:
        {
            Integer y;
            mJdate->GetNormalizedYear(&y);
            GetActualMaximum(WEEK_OF_YEAR, &max);
            Set(DAY_OF_WEEK, InternalGet(DAY_OF_WEEK)); // update stamp[field]
            Integer woy = InternalGet(WEEK_OF_YEAR);
            Integer value = woy + amount;
            Integer normYear;
            mJdate->GetNormalizedYear(&normYear);
            if (!IsTransitionYear(normYear)) {
                Integer year, maxYear, minYear;
                mJdate->GetYear(&year);
                if (year == (GetMaximum(YEAR, &maxYear), maxYear)) {
                    GetActualMaximum(WEEK_OF_YEAR, &max);
                }
                else if (year == (GetMinimum(YEAR, &minYear), minYear)) {
                    GetActualMinimum(WEEK_OF_YEAR, &min);
                    GetActualMaximum(WEEK_OF_YEAR, &max);
                    if (value > min && value < max) {
                        Set(WEEK_OF_YEAR, value);
                        return NOERROR;
                    }

                }
                // If the new value is in between min and max
                // (exclusive), then we can use the value.
                if (value > min && value < max) {
                    Set(WEEK_OF_YEAR, value);
                    return NOERROR;
                }
                Long fd = mCachedFixedDate;
                // Make sure that the min week has the current DAY_OF_WEEK
                Long day1 = fd - (7 * (woy - min));
                if (year != (GetMinimum(YEAR, &minYear), minYear)) {
                    Integer yy;
                    if ((IBaseCalendar::Probe(GetGcal())->GetYearFromFixedDate(day1, &yy), yy) != y) {
                        min++;
                    }
                }
                else {
                    AutoPtr<ICalendarDate> d;
                    GetJcal()->GetCalendarDate(ILong::MIN_VALUE, GetZone(), &d);
                    Long date;
                    if (day1 < (IBaseCalendar::Probe(GetJcal())->GetFixedDate(d, &date), date)) {
                        min++;
                    }
                }

                // Make sure the same thing for the max week
                fd += 7 * (max - InternalGet(WEEK_OF_YEAR));
                Integer yy;
                if ((IBaseCalendar::Probe(GetGcal())->GetYearFromFixedDate(fd, &yy), yy) != y) {
                    max--;
                }
                break;
            }

            // Handle transition here.
            Long fd = mCachedFixedDate;
            Long day1 = fd - (7 * (woy - min));
            // Make sure that the min week has the current DAY_OF_WEEK
            AutoPtr<LocalGregorianCalendar::Date> d = GetCalendarDate(day1);
            AutoPtr<IEra> dEra, jdEra;
            Integer dy, jdy;
            if (!((d->GetEra(&dEra), dEra) == (mJdate->GetEra(&jdEra), jdEra) &&
                    (d->GetYear(&dy), dy) == (mJdate->GetYear(&jdy), jdy))) {
                min++;
            }

            // Make sure the same thing for the max week
            fd += 7 * (max - woy);
            IBaseCalendar::Probe(GetJcal())->GetCalendarDateFromFixedDate(d, fd);
            dEra = jdEra = nullptr;
            if (!((d->GetEra(&dEra), dEra) == (mJdate->GetEra(&jdEra), jdEra) &&
                    (d->GetYear(&dy), dy) == (mJdate->GetYear(&jdy), jdy))) {
                max--;
            }
            // value: the new WEEK_OF_YEAR which must be converted
            // to month and day of month.
            value = GetRolledValue(woy, amount, min, max) - 1;
            d = GetCalendarDate(day1 + value * 7);
            Integer mon, dayOfMonth;
            d->GetMonth(&mon);
            d->GetDayOfMonth(&dayOfMonth);
            Set(MONTH, mon - 1);
            Set(DAY_OF_MONTH, dayOfMonth);
            return NOERROR;
        }

        case WEEK_OF_MONTH:
        {
            Integer normYear;
            mJdate->GetNormalizedYear(&normYear);
            Boolean isTransitionYear = IsTransitionYear(normYear);
            // dow: relative day of week from the first day of week
            Integer dayOfWeek;
            Integer dow = InternalGet(DAY_OF_WEEK) - (GetFirstDayOfWeek(&dayOfWeek), dayOfWeek);
            if (dow < 0) {
                dow += 7;
            }

            Long fd = mCachedFixedDate;
            Long month1;     // fixed date of the first day (usually 1) of the month
            Integer monthLength; // actual month length
            if (isTransitionYear) {
                month1 = GetFixedDateMonth1(mJdate, fd);
                monthLength = ActualMonthLength();
            }
            else {
                month1 = fd - InternalGet(DAY_OF_MONTH) + 1;
                GetJcal()->GetMonthLength(mJdate, &monthLength);
            }

            // the first day of week of the month.
            Long monthDay1st = LocalGregorianCalendar::GetDayOfWeekDateOnOrBefore(month1 + 6,
                    (GetFirstDayOfWeek(&dayOfWeek), dayOfWeek));
            // if the week has enough days to form a week, the
            // week starts from the previous month.
            Integer minDays;
            if ((Integer)(monthDay1st - month1) >= (GetMinimalDaysInFirstWeek(&minDays), minDays)) {
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
            else if (nfd >= (month1 + monthLength)) {
                nfd = month1 + monthLength - 1;
            }
            Set(DAY_OF_MONTH, (Integer)(nfd - month1) + 1);
            return NOERROR;
        }

        case DAY_OF_MONTH:
        {
            Integer normYear;
            mJdate->GetNormalizedYear(&normYear);
            if (!IsTransitionYear(normYear)) {
                GetJcal()->GetMonthLength(mJdate, &max);
                break;
            }

            // TODO: Need to change the spec to be usable DAY_OF_MONTH rolling...

            // Transition handling. We can't change year and era
            // values here due to the Calendar roll spec!
            Long month1 = GetFixedDateMonth1(mJdate, mCachedFixedDate);

            // It may not be a regular month. Convert the date and range to
            // the relative values, perform the roll, and
            // convert the result back to the rolled date.
            Integer value = GetRolledValue((Integer)(mCachedFixedDate - month1), amount,
                                       0, ActualMonthLength() - 1);
            AutoPtr<LocalGregorianCalendar::Date> d = GetCalendarDate(month1 + value);
            BLOCK_CHECK() {
                Integer y, m;
                CHECK(GetEraIndex(d) == InternalGetEra());
                CHECK((d->GetYear(&y), y) == InternalGet(YEAR));
                CHECK((d->GetMonth(&m), m) -1 == InternalGet(MONTH));
            }
            Integer dayOfMonth;
            d->GetDayOfMonth(&dayOfMonth);
            Set(DAY_OF_MONTH, dayOfMonth);
            return NOERROR;
        }

        case DAY_OF_YEAR:
        {
            GetActualMaximum(field, &max);
            Integer normYear;
            mJdate->GetNormalizedYear(&normYear);
            if (!IsTransitionYear(normYear)) {
                break;
            }

            // Handle transition. We can't change year and era values
            // here due to the Calendar roll spec.
            Integer value = GetRolledValue(InternalGet(DAY_OF_YEAR), amount, min, max);
            Long jan0 = mCachedFixedDate - InternalGet(DAY_OF_YEAR);
            AutoPtr<LocalGregorianCalendar::Date> d = GetCalendarDate(jan0 + value);
            BLOCK_CHECK() {
                Integer y;
                CHECK(GetEraIndex(d) == InternalGetEra());
                CHECK((d->GetYear(&y), y) == InternalGet(YEAR));
            }
            Integer month, dayOfMonth;
            d->GetMonth(&month);
            d->GetDayOfMonth(&dayOfMonth);
            Set(MONTH, month - 1);
            Set(DAY_OF_MONTH, dayOfMonth);
            return NOERROR;
        }

        case DAY_OF_WEEK:
        {
            Integer normYear;
            mJdate->GetNormalizedYear(&normYear);
            if (!IsTransitionYear(normYear) && !IsTransitionYear(normYear - 1)) {
                // If the week of year is in the same year, we can
                // just change DAY_OF_WEEK.
                Integer weekOfYear = InternalGet(WEEK_OF_YEAR);
                if (weekOfYear > 1 && weekOfYear < 52) {
                    Set(WEEK_OF_YEAR, InternalGet(WEEK_OF_YEAR));
                    max = SATURDAY;
                    break;
                }
            }

            // We need to handle it in a different way around year
            // boundaries and in the transition year. Note that
            // changing era and year values violates the roll
            // rule: not changing larger calendar fields...
            amount %= 7;
            if (amount == 0) {
                return NOERROR;
            }
            Long fd = mCachedFixedDate;
            Integer dayOfWeek;
            GetFirstDayOfWeek(&dayOfWeek);
            Long dowFirst = LocalGregorianCalendar::GetDayOfWeekDateOnOrBefore(fd, dayOfWeek);
            fd += amount;
            if (fd < dowFirst) {
                fd += 7;
            }
            else if (fd >= dowFirst + 7) {
                fd -= 7;
            }
            AutoPtr<LocalGregorianCalendar::Date> d = GetCalendarDate(fd);
            Set(ERA, GetEraIndex(d));
            Integer y, m, dom;
            d->GetYear(&y);
            d->GetMonth(&m);
            d->GetDayOfMonth(&dom);
            Set(y, m - 1, dom);
            return NOERROR;
        }

        case DAY_OF_WEEK_IN_MONTH:
        {
            min = 1; // after having normalized, min should be 1.
            Integer normYear;
            mJdate->GetNormalizedYear(&normYear);
            if (!IsTransitionYear(normYear)) {
                Integer dom = InternalGet(DAY_OF_MONTH);
                Integer monthLength;
                GetJcal()->GetMonthLength(mJdate, &monthLength);
                Integer lastDays = monthLength % 7;
                max = monthLength / 7;
                Integer x = (dom - 1) % 7;
                if (x < lastDays) {
                    max++;
                }
                Set(DAY_OF_WEEK, InternalGet(DAY_OF_WEEK));
                break;
            }

            // Transition year handling.
            Long fd = mCachedFixedDate;
            Long month1 = GetFixedDateMonth1(mJdate, fd);
            Integer monthLength = ActualMonthLength();
            Integer lastDays = monthLength % 7;
            max = monthLength / 7;
            Integer x = (Integer)(fd - month1) % 7;
            if (x < lastDays) {
                max++;
            }
            Integer value = GetRolledValue(InternalGet(field), amount, min, max) - 1;
            fd = month1 + value * 7 + x;
            AutoPtr<LocalGregorianCalendar::Date> d = GetCalendarDate(fd);
            Integer dom;
            d->GetDayOfMonth(&dom);
            Set(DAY_OF_MONTH, dom);
            return NOERROR;
        }
    }

    Set(field, GetRolledValue(InternalGet(field), amount, min, max));
    return NOERROR;
}

ECode JapaneseImperialCalendar::GetDisplayName(
    /* [in] */ Integer field,
    /* [in] */ Integer style,
    /* [in] */ ILocale* locale,
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    Boolean result;
    FAIL_RETURN(CheckDisplayNameParams(field, style, SHORT, NARROW_FORMAT, locale,
            ERA_MASK | YEAR_MASK | MONTH_MASK | DAY_OF_WEEK_MASK | AM_PM_MASK, &result))
    if (!result) {
        *name = nullptr;
        return NOERROR;
    }

    Integer fieldValue;
    Get(field, &fieldValue);

    // "GanNen" is supported only in the LONG style.
    Integer eraValue;
    if (field == YEAR
            && (GetBaseStyle(style) != LONG || fieldValue != 1 || (Get(ERA, &eraValue), eraValue) == 0)) {
        *name = nullptr;
        return NOERROR;
    }

    String type;
    GetCalendarType(&type);
    CalendarDataUtility::RetrieveFieldValueName(type, field,
            fieldValue, style, locale, name);
    // If the ERA value is null, then
    // try to get its name or abbreviation from the Era instance.
    if (name->IsNull() && field == ERA && fieldValue < sEras.GetLength()) {
        IEra* era = sEras[fieldValue];
        if (style == SHORT) {
            era->GetAbbreviation(name);
        }
        else {
            era->GetName(name);
        }
    }
    return NOERROR;
}

ECode JapaneseImperialCalendar::GetDisplayNames(
    /* [in] */ Integer field,
    /* [in] */ Integer style,
    /* [in] */ ILocale* locale,
    /* [out] */ IMap** names)
{
    VALIDATE_NOT_NULL(names);

    Boolean result;
    FAIL_RETURN(CheckDisplayNameParams(field, style, ALL_STYLES, NARROW_FORMAT, locale,
            ERA_MASK | YEAR_MASK | MONTH_MASK | DAY_OF_WEEK_MASK | AM_PM_MASK, &result));
    if (!result) {
        *names = nullptr;
        return NOERROR;
    }

    String type;
    GetCalendarType(&type);
    CalendarDataUtility::RetrieveFieldValueNames(type, field, style, locale, names);
    // If strings[] has fewer than eras[], get more names from eras[].
    if (*names != nullptr) {
        if (field == ERA) {
            Integer size;
            (*names)->GetSize(&size);
            if (style == ALL_STYLES) {
                AutoPtr<ISet> values;
                CHashSet::New(IID_ISet, (IInterface**)&values);
                // count unique era values
                AutoPtr<ISet> keySet;
                (*names)->GetKeySet(&keySet);
                FOR_EACH(IInterface*, key, , keySet) {
                    AutoPtr<IInterface> value;
                    (*names)->Get(key, &value);
                    values->Add(value);
                } END_FOR_EACH();
                values->GetSize(&size);
            }
            if (size < sEras.GetLength()) {
                Integer baseStyle = GetBaseStyle(style);
                for (Integer i = size; i < sEras.GetLength(); i++) {
                    IEra* era = sEras[i];
                    if (baseStyle == ALL_STYLES || baseStyle == SHORT
                            || baseStyle == NARROW_FORMAT) {
                        String name;
                        era->GetAbbreviation(&name);
                        (*names)->Put(CoreUtils::Box(name), CoreUtils::Box(i));
                    }
                    if (baseStyle == ALL_STYLES || baseStyle == LONG) {
                        String name;
                        era->GetName(&name);
                        (*names)->Put(CoreUtils::Box(name), CoreUtils::Box(i));
                    }
                }
            }
        }
    }
    return NOERROR;
}

ECode JapaneseImperialCalendar::GetMinimum(
    /* [in] */ Integer field,
    /* [out] */ Integer* value)
{
    VALIDATE_NOT_NULL(value);

    if (field < 0 || field >= FIELD_COUNT) {
        return ccm::core::E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    *value = MIN_VALUES[field];
    return NOERROR;
}

ECode JapaneseImperialCalendar::GetMaximum(
    /* [in] */ Integer field,
    /* [out] */ Integer* value)
{
    VALIDATE_NOT_NULL(value);

    if (field < 0 || field >= FIELD_COUNT) {
        return ccm::core::E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    switch (field) {
        case YEAR:
        {
            // The value should depend on the time zone of this calendar.
            AutoPtr<ICalendarDate> d;
            GetJcal()->GetCalendarDate(ILong::MAX_VALUE, GetZone(), &d);
            Integer y;
            d->GetYear(&y);
            *value = Math::Max(LEAST_MAX_VALUES[YEAR], y);
            return NOERROR;
        }
    }
    *value = MAX_VALUES[field];
    return NOERROR;
}

ECode JapaneseImperialCalendar::GetGreatestMinimum(
    /* [in] */ Integer field,
    /* [out] */ Integer* value)
{
    VALIDATE_NOT_NULL(value);

    if (field < 0 || field >= FIELD_COUNT) {
        return ccm::core::E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    *value = field == YEAR ? 1 : MIN_VALUES[field];
    return NOERROR;
}

ECode JapaneseImperialCalendar::GetLeastMaximum(
    /* [in] */ Integer field,
    /* [out] */ Integer* value)
{
    VALIDATE_NOT_NULL(value);

    if (field < 0 || field >= FIELD_COUNT) {
        return ccm::core::E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    switch (field) {
        case YEAR:
        {
            Integer y;
            GetMaximum(YEAR, &y);
            *value = Math::Min(LEAST_MAX_VALUES[YEAR], y);
            return NOERROR;
        }
    }
    *value = LEAST_MAX_VALUES[field];
    return NOERROR;
}

ECode JapaneseImperialCalendar::GetActualMinimum(
    /* [in] */ Integer field,
    /* [out] */ Integer* value)
{
    VALIDATE_NOT_NULL(value);

    if (field < 0 || field >= FIELD_COUNT) {
        return ccm::core::E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    if (!IsFieldSet(YEAR_MASK | MONTH_MASK | WEEK_OF_YEAR_MASK, field)) {
        return GetMinimum(field, value);
    }

    *value = 0;
    AutoPtr<JapaneseImperialCalendar> jc = GetNormalizedCalendar();
    // Get a local date which includes time of day and time zone,
    // which are missing in jc.jdate.
    Long millis;
    jc->GetTimeInMillis(&millis);
    AutoPtr<ICalendarDate> jd;
    GetJcal()->GetCalendarDate(millis, GetZone(), &jd);
    Integer eraIndex = GetEraIndex((LocalGregorianCalendar::Date*)jd.Get());
    switch (field) {
        case YEAR:
        {
            if (eraIndex > BEFORE_MEIJI) {
                *value = 1;
                Long since;
                sEras[eraIndex]->GetSince(GetZone(), &since);
                AutoPtr<ICalendarDate> d;
                GetJcal()->GetCalendarDate(since, GetZone(), &d);
                // Use the same year in jd to take care of leap
                // years. i.e., both jd and d must agree on leap
                // or common years.
                Integer y;
                d->GetYear(&y);
                jd->SetYear(y);
                GetJcal()->Normalize(jd);
                BLOCK_CHECK() {
                    Boolean jdLeap, dLeap;
                    jd->IsLeapYear(&jdLeap);
                    d->IsLeapYear(&dLeap);
                    CHECK(jdLeap == dLeap);
                }
                if (GetYearOffsetInMillis(jd) < GetYearOffsetInMillis(d)) {
                    (*value)++;
                }
            }
            else {
                GetMinimum(field, value);
                AutoPtr<ICalendarDate> d;
                GetJcal()->GetCalendarDate(ILong::MIN_VALUE, GetZone(), &d);
                // Use an equvalent year of d.getYear() if
                // possible. Otherwise, ignore the leap year and
                // common year difference.
                Integer y;
                d->GetYear(&y);
                if (y > 400) {
                    y -= 400;
                }
                jd->SetYear(y);
                GetJcal()->Normalize(jd);
                if (GetYearOffsetInMillis(jd) < GetYearOffsetInMillis(d)) {
                    (*value)++;
                }
            }
            break;
        }

        case MONTH:
        {
            // In Before Meiji and Meiji, January is the first month.
            Integer y;
            if (eraIndex > MEIJI && (jd->GetYear(&y), y) == 1) {
                Long since;
                sEras[eraIndex]->GetSince(GetZone(), &since);
                AutoPtr<ICalendarDate> d;
                GetJcal()->GetCalendarDate(since, GetZone(), &d);
                Integer m, jdDom, dDom;
                *value = (d->GetMonth(&m), m) - 1;
                if ((jd->GetDayOfMonth(&jdDom), jdDom) <
                        (d->GetDayOfMonth(&dDom), dDom)) {
                    (*value)++;
                }
            }
            break;
        }

        case WEEK_OF_YEAR:
        {
            *value = 1;
            AutoPtr<ICalendarDate> d;
            GetJcal()->GetCalendarDate(ILong::MIN_VALUE, GetZone(), &d);
            // shift 400 years to avoid underflow
            d->AddYear(+400);
            GetJcal()->Normalize(d);
            AutoPtr<IEra> era;
            d->GetEra(&era);
            jd->SetEra(era);
            Integer y;
            d->GetYear(&y);
            jd->SetYear(y);
            GetJcal()->Normalize(jd);

            Long jan1, fd;
            IBaseCalendar* bc = IBaseCalendar::Probe(GetJcal());
            bc->GetFixedDate(d, &jan1);
            bc->GetFixedDate(jd, &fd);
            Integer woy = GetWeekNumber(jan1, fd);
            Long day1 = fd - (7 * (woy - 1));
            Long jdTod, dTod;
            if ((day1 < jan1) ||
                    (day1 == jan1 &&
                    (jd->GetTimeOfDay(&jdTod), jdTod) <
                    (d->GetTimeOfDay(&dTod), dTod))) {
                (*value)++;
            }
            break;
        }
    }
    return NOERROR;
}

ECode JapaneseImperialCalendar::GetActualMaximum(
    /* [in] */ Integer field,
    /* [out] */ Integer* value)
{
    VALIDATE_NOT_NULL(value);

    if (field < 0 || field >= FIELD_COUNT) {
        return ccm::core::E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    constexpr Integer fieldsForFixedMax = ERA_MASK | DAY_OF_WEEK_MASK |
            HOUR_MASK | AM_PM_MASK | HOUR_OF_DAY_MASK |MINUTE_MASK |
            SECOND_MASK | MILLISECOND_MASK | ZONE_OFFSET_MASK | DST_OFFSET_MASK;
    if ((fieldsForFixedMax & (1 << field)) != 0) {
        return GetMaximum(field, value);
    }

    AutoPtr<JapaneseImperialCalendar> jc = GetNormalizedCalendar();
    AutoPtr<LocalGregorianCalendar::Date> date = jc->mJdate;
    Integer normalizedYear;
    date->GetNormalizedYear(&normalizedYear);

    *value = -1;
    switch (field) {
        case MONTH:
        {
            *value = DECEMBER;
            if (IsTransitionYear(normalizedYear)) {
                // TODO: there may be multiple transitions in a year.
                Integer eraIndex = GetEraIndex(date);
                Integer y;
                if (date->GetYear(&y), y != 1) {
                    eraIndex++;
                    CHECK(eraIndex < sEras.GetLength());
                }
                Long transition = sSinceFixedDates[eraIndex];
                Long fd = jc->mCachedFixedDate;
                if (fd < transition) {
                    AutoPtr<ICalendarDate> ldate;
                    date->Clone(IID_ICalendarDate, (IInterface**)&ldate);
                    IBaseCalendar::Probe(GetJcal())->GetCalendarDateFromFixedDate(ldate, transition - 1);
                    Integer m;
                    *value = (ldate->GetMonth(&m), m) - 1;
                }
            }
            else {
                AutoPtr<ICalendarDate> d;
                GetJcal()->GetCalendarDate(ILong::MAX_VALUE, GetZone(), &d);
                AutoPtr<IEra> daEra, dEra;
                Integer daY, dY;
                if ((date->GetEra(&daEra), daEra) == (d->GetEra(&dEra), dEra) &&
                        (date->GetYear(&daY), daY) == (d->GetYear(&dY), dY)) {
                    Integer m;
                    *value = (d->GetMonth(&m), m) - 1;
                }
            }
            break;
        }

        case DAY_OF_MONTH:
            GetJcal()->GetMonthLength(date, value);
            break;

        case DAY_OF_YEAR:
        {
            if (IsTransitionYear(normalizedYear)) {
                // Handle transition year.
                // TODO: there may be multiple transitions in a year.
                Integer eraIndex = GetEraIndex(date);
                Integer y;
                if (date->GetYear(&y), y != 1) {
                    eraIndex++;
                    CHECK(eraIndex < sEras.GetLength());
                }
                Long transition = sSinceFixedDates[eraIndex];
                Long fd = jc->mCachedFixedDate;
                AutoPtr<ICalendarDate> d;
                GetGcal()->NewCalendarDate(TimeZone::NO_TIMEZONE, &d);
                d->SetDate(normalizedYear, IBaseCalendar::JANUARY, 1);
                if (fd < transition) {
                    Long date;
                    IBaseCalendar::Probe(GetGcal())->GetFixedDate(d, &date);
                    *value = (Integer)(transition - date);
                }
                else {
                    d->AddYear(+1);
                    Long date;
                    IBaseCalendar::Probe(GetGcal())->GetFixedDate(d, &date);
                    *value = (Integer)(date - transition);
                }
            }
            else {
                AutoPtr<ICalendarDate> d;
                GetJcal()->GetCalendarDate(ILong::MAX_VALUE, GetZone(), &d);
                AutoPtr<IEra> daEra, dEra;
                Integer daY, dY;
                if ((date->GetEra(&daEra), daEra) == (d->GetEra(&dEra), dEra) &&
                        (date->GetYear(&daY), daY) == (d->GetYear(&dY), dY)) {
                    Long fd;
                    IBaseCalendar::Probe(GetGcal())->GetFixedDate(d, &fd);
                    Long jan1 = GetFixedDateJan1((LocalGregorianCalendar::Date*)d.Get(), fd);
                    *value = (Integer)(fd - jan1) + 1;
                }
                else if ((date->GetYear(&daY), daY) == (GetMinimum(YEAR, &dY), dY)) {
                    AutoPtr<ICalendarDate> d1;
                    GetJcal()->GetCalendarDate(ILong::MIN_VALUE, GetZone(), &d1);
                    Long fd1;
                    IBaseCalendar::Probe(GetGcal())->GetFixedDate(d1, &fd1);
                    d1->AddYear(1);
                    d1->SetMonth(IBaseCalendar::JANUARY);
                    d1->SetDayOfMonth(1);
                    GetJcal()->Normalize(d1);
                    Long fd2;
                    IBaseCalendar::Probe(GetGcal())->GetFixedDate(d1, &fd2);
                    *value = (Integer)(fd2 - fd1);
                }
                else {
                    GetJcal()->GetYearLength(date, value);
                }
            }
            break;
        }

        case WEEK_OF_YEAR:
        {
            if (!IsTransitionYear(normalizedYear)) {
                AutoPtr<ICalendarDate> jd;
                GetJcal()->GetCalendarDate(ILong::MAX_VALUE, GetZone(), &jd);
                AutoPtr<IEra> daEra, jdEra;
                Integer daY, jdY;
                if ((date->GetEra(&daEra), daEra) == (jd->GetEra(&jdEra), jdEra) &&
                        (date->GetYear(&daY), daY) == (jd->GetYear(&jdY), jdY)) {
                    Long fd;
                    IBaseCalendar::Probe(GetJcal())->GetFixedDate(jd, &fd);
                    Long jan1 = GetFixedDateJan1((LocalGregorianCalendar::Date*)jd.Get(), fd);
                    *value = GetWeekNumber(jan1, fd);
                }
                else if ((date->GetEra(&daEra), daEra) == nullptr &&
                        (date->GetYear(&daY), daY) == (GetMinimum(YEAR, &jdY), jdY)) {
                    AutoPtr<ICalendarDate> d;
                    GetJcal()->GetCalendarDate(ILong::MIN_VALUE, GetZone(), &d);
                    // shift 400 years to avoid underflow
                    d->AddYear(+400);
                    GetJcal()->Normalize(d);
                    AutoPtr<IEra> dEra;
                    d->GetEra(&dEra);
                    jd->SetEra(dEra);
                    Integer y;
                    d->GetYear(&y);
                    jd->SetDate(y + 1, IBaseCalendar::JANUARY, 1);
                    GetJcal()->Normalize(jd);
                    Long jan1, nextJan1;
                    IBaseCalendar::Probe(GetJcal())->GetFixedDate(d, &jan1);
                    IBaseCalendar::Probe(GetJcal())->GetFixedDate(jd, &nextJan1);
                    Integer dayOfWeek;
                    GetFirstDayOfWeek(&dayOfWeek);
                    Long nextJan1st = LocalGregorianCalendar::GetDayOfWeekDateOnOrBefore(nextJan1 + 6,
                            dayOfWeek);
                    Integer ndays = (Integer)(nextJan1st - nextJan1);
                    Integer minDays;
                    GetMinimalDaysInFirstWeek(&minDays);
                    if (ndays >= minDays) {
                        nextJan1st -= 7;
                    }
                    *value = GetWeekNumber(jan1, nextJan1st);
                }
                else {
                    // Get the day of week of January 1 of the year
                    AutoPtr<ICalendarDate> d;
                    GetGcal()->NewCalendarDate(TimeZone::NO_TIMEZONE, &d);
                    d->SetDate(normalizedYear, IBaseCalendar::JANUARY, 1);
                    Integer dayOfWeek, dow;
                    IBaseCalendar::Probe(GetGcal())->GetDayOfWeek(d, &dayOfWeek);
                    // Normalize the day of week with the firstDayOfWeek value
                    dayOfWeek -= (GetFirstDayOfWeek(&dow), dow);
                    if (dayOfWeek < 0) {
                        dayOfWeek += 7;
                    }
                    *value = 52;
                    Integer minDays;
                    GetMinimalDaysInFirstWeek(&minDays);
                    Integer magic = dayOfWeek + minDays - 1;
                    Boolean leap;
                    if ((magic == 6) ||
                            ((date->IsLeapYear(&leap), leap) && (magic == 5 || magic == 12))) {
                        (*value)++;
                    }
                }
                break;
            }

            if (jc == this) {
                AutoPtr<IJapaneseImperialCalendar> cal;
                jc->Clone(IID_IJapaneseImperialCalendar, (IInterface**)&cal);
                jc = (JapaneseImperialCalendar*)cal.Get();
            }
            Integer max;
            GetActualMaximum(DAY_OF_YEAR, &max);
            jc->Set(DAY_OF_YEAR, max);
            jc->Get(WEEK_OF_YEAR, value);
            if (*value == 1 && max > 7) {
                jc->Add(WEEK_OF_YEAR, -1);
                jc->Get(WEEK_OF_YEAR, value);
            }
            break;
        }

        case WEEK_OF_MONTH:
        {
            AutoPtr<ICalendarDate> jd;
            GetJcal()->GetCalendarDate(ILong::MAX_VALUE, GetZone(), &jd);
            AutoPtr<IEra> daEra, jdEra;
            Integer daY, jdY;
            if ((date->GetEra(&daEra), daEra) == (jd->GetEra(&jdEra), jdEra) &&
                    (date->GetYear(&daY), daY) == (jd->GetYear(&jdY), jdY)) {
                AutoPtr<ICalendarDate> d;
                GetGcal()->NewCalendarDate(TimeZone::NO_TIMEZONE, &d);
                Integer m;
                date->GetMonth(&m);
                d->SetDate(normalizedYear, m, 1);
                Integer dayOfWeek, monthLength, dow;
                IBaseCalendar::Probe(GetGcal())->GetDayOfWeek(d, &dayOfWeek);
                GetGcal()->GetMonthLength(d, &monthLength);
                dayOfWeek -= (GetFirstDayOfWeek(&dow), dow);
                if (dayOfWeek < 0) {
                    dayOfWeek += 7;
                }
                Integer nDaysFirstWeek = 7 - dayOfWeek; // # of days in the first week
                *value = 3;
                Integer minDays;
                GetMinimalDaysInFirstWeek(&minDays);
                if (nDaysFirstWeek >= minDays) {
                    (*value)++;
                }
                monthLength -= nDaysFirstWeek + 7 * 3;
                if (monthLength > 0) {
                    (*value)++;
                    if (monthLength > 7) {
                        (*value)++;
                    }
                }
            }
            else {
                Long fd;
                IBaseCalendar::Probe(GetJcal())->GetFixedDate(jd, &fd);
                Integer dom;
                Long month1 = fd - (jd->GetDayOfMonth(&dom), dom) + 1;
                *value = GetWeekNumber(month1, fd);
            }
            break;
        }

        case DAY_OF_WEEK_IN_MONTH:
        {
            Integer ndays, dow1, dow;
            date->GetDayOfWeek(&dow);
            AutoPtr<ICalendarDate> d;
            date->Clone(IID_ICalendarDate, (IInterface**)&d);
            GetJcal()->GetMonthLength(d, &ndays);
            d->SetDayOfMonth(1);
            GetJcal()->Normalize(d);
            d->GetDayOfWeek(&dow1);
            Integer x = dow - dow1;
            if (x < 0) {
                x += 7;
            }
            ndays -= x;
            *value = (ndays + 6) / 7;
            break;
        }

        case YEAR:
        {
            Long millis;
            jc->GetTimeInMillis(&millis);
            AutoPtr<ICalendarDate> jd, d;
            GetJcal()->GetCalendarDate(millis, GetZone(), &jd);
            Integer eraIndex = GetEraIndex(date);
            if (eraIndex == sEras.GetLength() - 1) {
                GetJcal()->GetCalendarDate(ILong::MAX_VALUE, GetZone(), &d);
                d->GetYear(value);
                // Use an equivalent year for the
                // getYearOffsetInMillis call to avoid overflow.
                if (*value > 400) {
                    jd->SetYear(*value - 400);
                }
            }
            else {
                Long time;
                sEras[eraIndex + 1]->GetSince(GetZone(), &time);
                GetJcal()->GetCalendarDate(time - 1, GetZone(), &d);
                d->GetYear(value);
                // Use the same year as d.getYear() to be
                // consistent with leap and common years.
                jd->SetYear(*value);
            }
            GetJcal()->Normalize(jd);
            if (GetYearOffsetInMillis(jd) > GetYearOffsetInMillis(d)) {
                (*value)--;
            }
            break;
        }

        default:
            return ccm::core::E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    return NOERROR;
}

Long JapaneseImperialCalendar::GetYearOffsetInMillis(
    /* [in] */ ICalendarDate* date)
{
    Long doy;
    IBaseCalendar::Probe(GetJcal())->GetDayOfYear(date, &doy);
    Long t = (doy - 1) * ONE_DAY;
    Long tod;
    date->GetTimeOfDay(&tod);
    Integer zo;
    date->GetZoneOffset(&zo);
    return t + tod - zo;
}

ECode JapaneseImperialCalendar::Clone(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<JapaneseImperialCalendar> other = new JapaneseImperialCalendar();
    Calendar::CloneImpl(other.Get());
    AutoPtr<ICalendarDate> date;
    mJdate->Clone(IID_ICalendarDate, (IInterface**)&date);
    other->mJdate = (LocalGregorianCalendar::Date*)date.Get();
    *obj = (IJapaneseImperialCalendar*)other.Get();
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

ECode JapaneseImperialCalendar::GetTimeZone(
    /* [out] */ ITimeZone** zone)
{
    VALIDATE_NOT_NULL(zone);

    Calendar::GetTimeZone(zone);
    mJdate->SetZone(*zone);
    return NOERROR;
}

ECode JapaneseImperialCalendar::SetTimeZone(
    /* [in] */ ITimeZone* zone)
{
    Calendar::SetTimeZone(zone);
    mJdate->SetZone(zone);
    return NOERROR;
}

ECode JapaneseImperialCalendar::ComputeFields()
{
    Integer mask = 0;
    if (IsPartiallyNormalized()) {
        // Determine which calendar fields need to be computed.
        mask = GetSetStateFields();
        Integer fieldMask = ~mask & ALL_FIELDS;
        if (fieldMask != 0 || mCachedFixedDate == ILong::MIN_VALUE) {
            mask |= ComputeFields(fieldMask,
                    mask & (ZONE_OFFSET_MASK | DST_OFFSET_MASK));
            CHECK(mask == ALL_FIELDS);
        }
    }
    else {
        // Specify all fields
        mask = ALL_FIELDS;
        ComputeFields(mask, 0);
    }
    // After computing all the fields, set the field state to `COMPUTED'.
    SetFieldsComputed(mask);
    return NOERROR;
}

Integer JapaneseImperialCalendar::ComputeFields(
    /* [in] */ Integer fieldMask,
    /* [in] */ Integer tzMask)
{
    Integer zoneOffset = 0;
    AutoPtr<ITimeZone> tz = GetZone();
    if (mZoneOffsets.IsNull()) {
        mZoneOffsets = Array<Integer>(2);
    }
    if (tzMask != (ZONE_OFFSET_MASK | DST_OFFSET_MASK)) {
        // Android-changed: remove ZoneInfo support.
        tz->GetOffset(mTime, &zoneOffset);
        tz->GetRawOffset(&mZoneOffsets[0]);
        mZoneOffsets[1] = zoneOffset - mZoneOffsets[0];
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

    // See if we can use jdate to avoid date calculation.
    if (fixedDate != mCachedFixedDate || fixedDate < 0) {
        IBaseCalendar::Probe(GetJcal())->GetCalendarDateFromFixedDate(mJdate, fixedDate);
        mCachedFixedDate = fixedDate;
    }
    Integer era = GetEraIndex(mJdate);
    Integer year;
    mJdate->GetYear(&year);

    // Always set the ERA and YEAR values.
    InternalSet(ERA, era);
    InternalSet(YEAR, year);
    Integer mask = fieldMask | (ERA_MASK | YEAR_MASK);

    Integer month, dayOfMonth;
    mJdate->GetMonth(&month);
    month -= 1; // 0-based
    mJdate->GetDayOfMonth(&dayOfMonth);

    // Set the basic date fields.
    if ((fieldMask & (MONTH_MASK | DAY_OF_MONTH_MASK | DAY_OF_WEEK_MASK))
            != 0) {
        Integer dayOfWeek;
        mJdate->GetDayOfWeek(&dayOfWeek);
        InternalSet(MONTH, month);
        InternalSet(DAY_OF_MONTH, dayOfMonth);
        InternalSet(DAY_OF_WEEK, dayOfWeek);
        mask |= MONTH_MASK | DAY_OF_MONTH_MASK | DAY_OF_WEEK_MASK;
    }

    if ((fieldMask & (HOUR_OF_DAY_MASK | AM_PM_MASK | HOUR_MASK |
            MINUTE_MASK | SECOND_MASK | MILLISECOND_MASK)) != 0) {
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
        mask |= (HOUR_OF_DAY_MASK | AM_PM_MASK | HOUR_MASK |
                MINUTE_MASK | SECOND_MASK | MILLISECOND_MASK);
    }

    if ((fieldMask & (ZONE_OFFSET_MASK | DST_OFFSET_MASK)) != 0) {
        InternalSet(ZONE_OFFSET, mZoneOffsets[0]);
        InternalSet(DST_OFFSET, mZoneOffsets[1]);
        mask |= (ZONE_OFFSET_MASK | DST_OFFSET_MASK);
    }

    if ((fieldMask & (DAY_OF_YEAR_MASK | WEEK_OF_YEAR_MASK |
            WEEK_OF_MONTH_MASK | DAY_OF_WEEK_IN_MONTH_MASK)) != 0) {
        Integer normalizedYear;
        mJdate->GetNormalizedYear(&normalizedYear);
        // If it's a year of an era transition, we need to handle
        // irregular year boundaries.
        Boolean transitionYear = IsTransitionYear(normalizedYear);
        Integer dayOfYear;
        Long fixedDateJan1;
        if (transitionYear) {
            fixedDateJan1 = GetFixedDateJan1(mJdate, fixedDate);
            dayOfYear = (Integer)(fixedDate - fixedDateJan1) + 1;
        }
        else if (normalizedYear == MIN_VALUES[YEAR]) {
            AutoPtr<ICalendarDate> dx;
            GetJcal()->GetCalendarDate(ILong::MIN_VALUE, GetZone(), &dx);
            IBaseCalendar::Probe(GetJcal())->GetFixedDate(dx, &fixedDateJan1);
            dayOfYear = (Integer)(fixedDate - fixedDateJan1) + 1;
        }
        else {
            Long doy;
            IBaseCalendar::Probe(GetJcal())->GetDayOfYear(mJdate, &doy);
            dayOfYear = doy;
            fixedDateJan1 = fixedDate - dayOfYear + 1;
        }
        Long fixedDateMonth1 = transitionYear ?
                GetFixedDateMonth1(mJdate, fixedDate) : fixedDate - dayOfMonth + 1;

        InternalSet(DAY_OF_YEAR, dayOfYear);
        InternalSet(DAY_OF_WEEK_IN_MONTH, (dayOfMonth - 1) / 7 + 1);

        Integer weekOfYear = GetWeekNumber(fixedDateJan1, fixedDate);

        // The spec is to calculate WEEK_OF_YEAR in the
        // ISO8601-style. This creates problems, though.
        if (weekOfYear == 0) {
            // If the date belongs to the last week of the
            // previous year, use the week number of "12/31" of
            // the "previous" year. Again, if the previous year is
            // a transition year, we need to take care of it.
            // Usually the previous day of the first day of a year
            // is December 31, which is not always true in the
            // Japanese imperial calendar system.
            Long fixedDec31 = fixedDateJan1 - 1;
            Long prevJan1;
            AutoPtr<LocalGregorianCalendar::Date> d = GetCalendarDate(fixedDec31);
            Integer normYear;
            if (!(transitionYear || (d->GetNormalizedYear(&normYear), IsTransitionYear(normYear)))) {
                prevJan1 = fixedDateJan1 - 365;
                Boolean leap;
                if (d->IsLeapYear(&leap), leap) {
                    --prevJan1;
                }
            }
            else if (transitionYear) {
                Integer y;
                if (mJdate->GetYear(&y), y == 1) {
                    // As of Heisei (since Meiji) there's no case
                    // that there are multiple transitions in a
                    // year.  Historically there was such
                    // case. There might be such case again in the
                    // future.
                    if (era > HEISEI) {
                        AutoPtr<ICalendarDate> pd;
                        sEras[era - 1]->GetSinceDate(&pd);
                        if (pd->GetYear(&y), normalizedYear == y) {
                            Integer m, dom;
                            pd->GetMonth(&m);
                            pd->GetDayOfMonth(&dom);
                            d->SetMonth(m);
                            d->SetDayOfMonth(dom);
                        }
                    }
                    else {
                        d->SetMonth(LocalGregorianCalendar::JANUARY);
                        d->SetDayOfMonth(1);
                    }
                    GetJcal()->Normalize(d);
                    IBaseCalendar::Probe(GetJcal())->GetFixedDate(d, &prevJan1);
                }
                else {
                    prevJan1 = fixedDateJan1 - 365;
                    Boolean leap;
                    if (d->IsLeapYear(&leap), leap) {
                        --prevJan1;
                    }
                }
            }
            else {
                AutoPtr<ICalendarDate> cd;
                sEras[GetEraIndex(mJdate)]->GetSinceDate(&cd);
                Integer m, dom;
                cd->GetMonth(&m);
                cd->GetDayOfMonth(&dom);
                d->SetMonth(m);
                d->SetDayOfMonth(dom);
                GetJcal()->Normalize(d);
                IBaseCalendar::Probe(GetJcal())->GetFixedDate(d, &prevJan1);
            }
            weekOfYear = GetWeekNumber(prevJan1, fixedDec31);
        }
        else {
            if (!transitionYear) {
                // Regular years
                if (weekOfYear >= 52) {
                    Long nextJan1 = fixedDateJan1 + 365;
                    Boolean leap;
                    if (mJdate->IsLeapYear(&leap), leap) {
                        nextJan1++;
                    }
                    Integer dayOfWeek;
                    GetFirstDayOfWeek(&dayOfWeek);
                    Long nextJan1st = LocalGregorianCalendar::GetDayOfWeekDateOnOrBefore(nextJan1 + 6,
                            dayOfWeek);
                    Integer ndays = (Integer)(nextJan1st - nextJan1);
                    Integer minDays;
                    GetMinimalDaysInFirstWeek(&minDays);
                    if (ndays >= minDays && fixedDate >= (nextJan1st - 7)) {
                        // The first days forms a week in which the date is included.
                        weekOfYear = 1;
                    }
                }
            }
            else {
                AutoPtr<ICalendarDate> d;
                mJdate->Clone(IID_ICalendarDate, (IInterface**)&d);
                Long nextJan1;
                Integer y;
                if (mJdate->GetYear(&y), y == 1) {
                    d->AddYear(+1);
                    d->SetMonth(LocalGregorianCalendar::JANUARY);
                    d->SetDayOfMonth(1);
                    IBaseCalendar::Probe(GetJcal())->GetFixedDate(d, &nextJan1);
                }
                else {
                    Integer nextEraIndex = GetEraIndex((LocalGregorianCalendar::Date*)d.Get()) + 1;
                    AutoPtr<ICalendarDate> cd;
                    sEras[nextEraIndex]->GetSinceDate(&cd);
                    d->SetEra(sEras[nextEraIndex]);
                    Integer m, dom;
                    cd->GetMonth(&m);
                    cd->GetDayOfMonth(&dom);
                    d->SetDate(1, m, dom);
                    GetJcal()->Normalize(d);
                    IBaseCalendar::Probe(GetJcal())->GetFixedDate(d, &nextJan1);
                }
                Integer dayOfWeek;
                GetFirstDayOfWeek(&dayOfWeek);
                Long nextJan1st = LocalGregorianCalendar::GetDayOfWeekDateOnOrBefore(nextJan1 + 6,
                        dayOfWeek);
                Integer ndays = (Integer)(nextJan1st - nextJan1);
                Integer minDays;
                GetMinimalDaysInFirstWeek(&minDays);
                if (ndays >= minDays && fixedDate >= (nextJan1st - 7)) {
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

Integer JapaneseImperialCalendar::GetWeekNumber(
    /* [in] */ Long fixedDay1,
    /* [in] */ Long fixedDate)
{
    // We can always use `jcal' since Julian and Gregorian are the
    // same thing for this calculation.
    Integer dayOfWeek;
    GetFirstDayOfWeek(&dayOfWeek);
    Long fixedDay1st = LocalGregorianCalendar::GetDayOfWeekDateOnOrBefore(fixedDay1 + 6,
            dayOfWeek);
    Integer ndays = (Integer)(fixedDay1st - fixedDay1);
    CHECK(ndays <= 7);
    Integer minDays;
    GetMinimalDaysInFirstWeek(&minDays);
    if (ndays >= minDays) {
        fixedDay1st -= 7;
    }
    Integer normalizedDayOfPeriod = (Integer)(fixedDate - fixedDay1st);
    if (normalizedDayOfPeriod >= 0) {
        return normalizedDayOfPeriod / 7 + 1;
    }
    return CalendarUtils::FloorDivide(normalizedDayOfPeriod, 7) + 1;
}

ECode JapaneseImperialCalendar::ComputeTime()
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
                if (value < (GetMinimum(field, &min), min) || value > (GetMaximum(field, &max), max)) {
                    Logger::E("JapaneseImperialCalendar", "illegal field %s", GetFieldName(field).string());
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
                }
            }
            mOriginalFields[field] = value;
        }
    }

    // Let the super class determine which calendar fields to be
    // used to calculate the time.
    Integer fieldMask = SelectFields();

    Integer year;
    Integer era;

    Boolean set;
    if (IsSet(ERA, &set), set) {
        era = InternalGet(ERA);
        year = (IsSet(YEAR, &set), set) ? InternalGet(YEAR) : 1;
    }
    else {
        if (IsSet(YEAR, &set), set) {
            era = sEras.GetLength() - 1;
            year = InternalGet(YEAR);
        }
        else {
            // Equivalent to 1970 (Gregorian)
            era = SHOWA;
            year = 45;
        }
    }

    // Calculate the time of day. We rely on the convention that
    // an UNSET field has 0.
    Long timeOfDay = 0;
    if (IsFieldSet(fieldMask, HOUR_OF_DAY)) {
        timeOfDay += (Long) InternalGet(HOUR_OF_DAY);
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
    fixedDate += GetFixedDate(era, year, fieldMask);

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
    if (mZoneOffsets.IsNull()) {
        mZoneOffsets = Array<Integer>(2);
    }
    Integer tzMask = fieldMask & (ZONE_OFFSET_MASK | DST_OFFSET_MASK);
    if (tzMask != (ZONE_OFFSET_MASK | DST_OFFSET_MASK)) {
        // Android-changed: remove ZoneInfo support
        Integer offset;
        zone->GetRawOffset(&offset);
        TimeZone::From(zone)->GetOffsets(millis - offset, mZoneOffsets);
    }
    if (tzMask != 0) {
        if (IsFieldSet(tzMask, ZONE_OFFSET)) {
            mZoneOffsets[0] = InternalGet(ZONE_OFFSET);
        }
        if (IsFieldSet(tzMask, DST_OFFSET)) {
            mZoneOffsets[1] = InternalGet(DST_OFFSET);
        }
    }

    // Adjust the time zone offset values to get the UTC time.
    millis -= mZoneOffsets[0] + mZoneOffsets[1];

    // Set this calendar's time in milliseconds
    mTime = millis;

    Integer mask = ComputeFields(fieldMask | GetSetStateFields(), tzMask);

    if (IsLenient(&lenient), !lenient) {
        for (Integer field = 0; field < FIELD_COUNT; field++) {
            if (!IsExternallySet(field)) {
                continue;
            }
            if (mOriginalFields[field] != InternalGet(field)) {
                Integer wrongValue = InternalGet(field);
                // Restore the original field values
                mFields.Copy(mOriginalFields, 0, mFields.GetLength());
                Logger::E("JapaneseImperialCalendar", "%s = %d, expected %d",
                            GetFieldName(field).string(), wrongValue, mOriginalFields[field]);
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }
    }
    SetFieldsNormalized(mask);
    return NOERROR;
}

Long JapaneseImperialCalendar::GetFixedDate(
    /* [in] */ Integer era,
    /* [in] */ Integer year,
    /* [in] */ Integer fieldMask)
{
    Integer month = JANUARY;
    Integer firstDayOfMonth = 1;
    if (IsFieldSet(fieldMask, MONTH)) {
        // No need to check if MONTH has been set (no isSet(MONTH)
        // call) since its unset value happens to be JANUARY (0).
        month = InternalGet(MONTH);

        // If the month is out of range, adjust it into range.
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
    else {
        if (year == 1 && era != 0) {
            AutoPtr<ICalendarDate> d;
            sEras[era]->GetSinceDate(&d);
            d->GetMonth(&month);
            month = month - 1;
            d->GetDayOfMonth(&firstDayOfMonth);
        }
    }

    // Adjust the base date if year is the minimum value.
    if (year == MIN_VALUES[YEAR]) {
        AutoPtr<ICalendarDate> dx;
        GetJcal()->GetCalendarDate(ILong::MIN_VALUE, GetZone(), &dx);
        Integer m;
        dx->GetMonth(&m);
        m = m - 1;
        if (month < m) {
            month = m;
        }
        if (month == m) {
            dx->GetDayOfMonth(&firstDayOfMonth);
        }
    }

    AutoPtr<ICalendarDate> date;
    GetJcal()->NewCalendarDate(TimeZone::NO_TIMEZONE, &date);
    date->SetEra(era > 0 ? sEras[era] : nullptr);
    date->SetDate(year, month + 1, firstDayOfMonth);
    GetJcal()->Normalize(date);

    // Get the fixed date since Jan 1, 1 (Gregorian). We are on
    // the first day of either `month' or January in 'year'.
    Long fixedDate;
    IBaseCalendar::Probe(GetJcal())->GetFixedDate(date, &fixedDate);

    if (IsFieldSet(fieldMask, MONTH)) {
        // Month-based calculations
        if (IsFieldSet(fieldMask, DAY_OF_MONTH)) {
            // We are on the "first day" of the month (which may
            // not be 1). Just add the offset if DAY_OF_MONTH is
            // set. If the isSet call returns false, that means
            // DAY_OF_MONTH has been selected just because of the
            // selected combination. We don't need to add any
            // since the default value is the "first day".
            Boolean set;
            if (IsSet(DAY_OF_MONTH, &set), set) {
                // To avoid underflow with DAY_OF_MONTH-firstDayOfMonth, add
                // DAY_OF_MONTH, then subtract firstDayOfMonth.
                fixedDate += InternalGet(DAY_OF_MONTH);
                fixedDate -= firstDayOfMonth;
            }
        }
        else {
            if (IsFieldSet(fieldMask, WEEK_OF_MONTH)) {
                Integer dayOfWeek;
                GetFirstDayOfWeek(&dayOfWeek);
                Long firstDayOfWeek = LocalGregorianCalendar::GetDayOfWeekDateOnOrBefore(
                        fixedDate + 6, dayOfWeek);
                // If we have enough days in the first week, then
                // move to the previous week.
                Integer minDays;
                GetMinimalDaysInFirstWeek(&minDays);
                if ((firstDayOfWeek - fixedDate) >= minDays) {
                    firstDayOfWeek -= 7;
                }
                if (IsFieldSet(fieldMask, DAY_OF_WEEK)) {
                    firstDayOfWeek = LocalGregorianCalendar::GetDayOfWeekDateOnOrBefore(
                            firstDayOfWeek + 6, InternalGet(DAY_OF_WEEK));
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
                    fixedDate = LocalGregorianCalendar::GetDayOfWeekDateOnOrBefore(
                            fixedDate + (7 * dowim) - 1, dayOfWeek);
                }
                else {
                    // Go to the first day of the next week of
                    // the specified week boundary.
                    Integer lastDate = MonthLength(month, year) + (7 * (dowim + 1));
                    // Then, get the day of week date on or before the last date.
                    fixedDate = LocalGregorianCalendar::GetDayOfWeekDateOnOrBefore(
                            fixedDate + lastDate - 1, dayOfWeek);
                }
            }
        }
    }
    else {
        // We are on the first day of the year.
        if (IsFieldSet(fieldMask, DAY_OF_YEAR)) {
            Integer normalizedYear;
            ((LocalGregorianCalendar::Date*)date.Get())->GetNormalizedYear(&normalizedYear);
            if (IsTransitionYear(normalizedYear)) {
                fixedDate = GetFixedDateJan1((LocalGregorianCalendar::Date*)date.Get(), fixedDate);
            }
            // Add the offset, then subtract 1. (Make sure to avoid underflow.)
            fixedDate += InternalGet(DAY_OF_YEAR);
            fixedDate--;
        }
        else {
            Integer dow;
            GetFirstDayOfWeek(&dow);
            Long firstDayOfWeek = LocalGregorianCalendar::GetDayOfWeekDateOnOrBefore(
                    fixedDate + 6, dow);
            // If we have enough days in the first week, then move
            // to the previous week.
            Integer minDays;
            GetMinimalDaysInFirstWeek(&minDays);
            if ((firstDayOfWeek - fixedDate) >= minDays) {
                firstDayOfWeek -= 7;
            }
            if (IsFieldSet(fieldMask, DAY_OF_WEEK)) {
                Integer dayOfWeek = InternalGet(DAY_OF_WEEK);
                if (dayOfWeek != dow) {
                    firstDayOfWeek = LocalGregorianCalendar::GetDayOfWeekDateOnOrBefore(
                            firstDayOfWeek + 6, dayOfWeek);
                }
            }
            fixedDate = firstDayOfWeek + 7 * ((Long)InternalGet(WEEK_OF_YEAR) - 1);
        }
    }
    return fixedDate;
}

Long JapaneseImperialCalendar::GetFixedDateJan1(
    /* [in] */ LocalGregorianCalendar::Date* date,
    /* [in] */ Long fixedDate)
{
    AutoPtr<IEra> era;
    date->GetEra(&era);
    Integer y;
    if (era != nullptr && (date->GetYear(&y), y) == 1) {
        for (Integer eraIndex = GetEraIndex(date); eraIndex > 0; eraIndex--) {
            AutoPtr<ICalendarDate> d;
            sEras[eraIndex]->GetSinceDate(&d);
            Long fd;
            IBaseCalendar::Probe(GetGcal())->GetFixedDate(d, &fd);
            // There might be multiple era transitions in a year.
            if (fd > fixedDate) {
                continue;
            }
            return fd;
        }
    }
    AutoPtr<ICalendarDate> d;
    GetGcal()->NewCalendarDate(TimeZone::NO_TIMEZONE, &d);
    Integer normalizedYear;
    date->GetNormalizedYear(&normalizedYear);
    d->SetDate(normalizedYear, IBaseCalendar::JANUARY, 1);
    Long fd;
    IBaseCalendar::Probe(GetGcal())->GetFixedDate(d, &fd);
    return fd;
}

Long JapaneseImperialCalendar::GetFixedDateMonth1(
    /* [in] */ LocalGregorianCalendar::Date* date,
    /* [in] */ Long fixedDate)
{
    Integer eraIndex = GetTransitionEraIndex(date);
    if (eraIndex != -1) {
        Long transition = sSinceFixedDates[eraIndex];
        // If the given date is on or after the transition date, then
        // return the transition date.
        if (transition <= fixedDate) {
            return transition;
        }
    }

    // Otherwise, we can use the 1st day of the month.
    Integer dom;
    date->GetDayOfMonth(&dom);
    return fixedDate - dom + 1;
}

AutoPtr<LocalGregorianCalendar::Date> JapaneseImperialCalendar::GetCalendarDate(
    /* [in] */ Long fd)
{
    AutoPtr<ICalendarDate> d;
    GetJcal()->NewCalendarDate(TimeZone::NO_TIMEZONE, &d);
    IBaseCalendar::Probe(GetJcal())->GetCalendarDateFromFixedDate(d, fd);
    return (LocalGregorianCalendar::Date*)d.Get();
}

Integer JapaneseImperialCalendar::MonthLength(
    /* [in] */ Integer month,
    /* [in] */ Integer gregorianYear)
{
    return CalendarUtils::IsGregorianLeapYear(gregorianYear) ?
            CGregorianCalendar::LEAP_MONTH_LENGTH[month] : CGregorianCalendar::MONTH_LENGTH[month];
}

Integer JapaneseImperialCalendar::MonthLength(
    /* [in] */ Integer month)
{
    BLOCK_CHECK() {
        Boolean normalized;
        mJdate->IsNormalized(&normalized);
        CHECK(normalized);
    }
    Boolean leap;
    return (mJdate->IsLeapYear(&leap), leap) ?
            CGregorianCalendar::LEAP_MONTH_LENGTH[month] : CGregorianCalendar::MONTH_LENGTH[month];
}

Integer JapaneseImperialCalendar::ActualMonthLength()
{
    Integer length;
    GetJcal()->GetMonthLength(mJdate, &length);
    Integer eraIndex = GetTransitionEraIndex(mJdate);
    if (eraIndex == -1) {
        Long transitionFixedDate = sSinceFixedDates[eraIndex];
        AutoPtr<ICalendarDate> d;
        sEras[eraIndex]->GetSinceDate(&d);
        Integer dom;
        d->GetDayOfMonth(&dom);
        if (transitionFixedDate <= mCachedFixedDate) {
            length -= dom - 1;
        }
        else {
            length = dom - 1;
        }
    }
    return length;
}

Integer JapaneseImperialCalendar::GetTransitionEraIndex(
    /* [in] */ LocalGregorianCalendar::Date* date)
{
    Integer eraIndex = GetEraIndex(date);
    AutoPtr<ICalendarDate> transitionDate;
    sEras[eraIndex]->GetSinceDate(&transitionDate);
    Integer ty, tm, dy, dm;
    if ((transitionDate->GetYear(&ty), ty) == (date->GetNormalizedYear(&dy), dy) &&
            (transitionDate->GetMonth(&tm), tm) == (date->GetMonth(&dm), dm)) {
        return eraIndex;
    }
    if (eraIndex < sEras.GetLength() - 1) {
        transitionDate = nullptr;
        sEras[++eraIndex]->GetSinceDate(&transitionDate);
        if ((transitionDate->GetYear(&ty), ty) == (date->GetNormalizedYear(&dy), dy) &&
                (transitionDate->GetMonth(&tm), tm) == (date->GetMonth(&dm), dm)) {
            return eraIndex;
        }
    }
    return -1;
}

Boolean JapaneseImperialCalendar::IsTransitionYear(
    /* [in] */ Integer normalizedYear)
{
    for (Integer i = sEras.GetLength() - 1; i > 0; i--) {
        AutoPtr<ICalendarDate> date;
        sEras[i]->GetSinceDate(&date);
        Integer transitionYear;
        date->GetYear(&transitionYear);
        if (normalizedYear == transitionYear) {
            return true;
        }
        if (normalizedYear > transitionYear) {
            break;
        }
    }
    return false;
}

Integer JapaneseImperialCalendar::GetEraIndex(
    /* [in] */ LocalGregorianCalendar::Date* date)
{
    AutoPtr<IEra> era;
    date->GetEra(&era);
    for (Integer i = sEras.GetLength() - 1; i > 0; i--) {
        if (sEras[i] == era) {
            return i;
        }
    }
    return 0;
}

AutoPtr<JapaneseImperialCalendar> JapaneseImperialCalendar::GetNormalizedCalendar()
{
    AutoPtr<JapaneseImperialCalendar> jc;
    if (IsFullyNormalized()) {
        jc = this;
    }
    else {
        // Create a clone and normalize the calendar fields
        AutoPtr<IJapaneseImperialCalendar> cal;
        Clone(IID_IJapaneseImperialCalendar, (IInterface**)&cal);
        jc = (JapaneseImperialCalendar*)cal.Get();
        jc->SetLenient(true);
        jc->Complete();
    }
    return jc;
}

void JapaneseImperialCalendar::PinDayOfMonth(
    /* [in] */ LocalGregorianCalendar::Date* date)
{
    Integer year, dom, y;
    date->GetYear(&year);
    date->GetDayOfMonth(&dom);
    if (year != (GetMinimum(YEAR, &y), y)) {
        date->SetDayOfMonth(1);
        GetJcal()->Normalize(date);
        Integer monthLength;
        GetJcal()->GetMonthLength(date, &monthLength);
        if (dom > monthLength) {
            date->SetDayOfMonth(monthLength);
        }
        else {
            date->SetDayOfMonth(dom);
        }
        GetJcal()->Normalize(date);
    }
    else {
        AutoPtr<ICalendarDate> d, realDate;
        GetJcal()->GetCalendarDate(ILong::MIN_VALUE, GetZone(), &d);
        GetJcal()->GetCalendarDate(mTime, GetZone(), &realDate);
        Long tod;
        realDate->GetTimeOfDay(&tod);
        // Use an equivalent year.
        realDate->AddYear(+400);
        Integer mon;
        date->GetMonth(&mon);
        realDate->SetMonth(mon);
        realDate->SetDayOfMonth(1);
        GetJcal()->Normalize(realDate);
        Integer monthLength;
        GetJcal()->GetMonthLength(realDate, &monthLength);
        if (dom > monthLength) {
            realDate->SetDayOfMonth(monthLength);
        }
        else {
            Integer dDom;
            d->GetDayOfMonth(&dDom);
            if (dom < dDom) {
                realDate->SetDayOfMonth(dDom);
            }
            else {
                realDate->SetDayOfMonth(dom);
            }
        }
        Integer rDom, dDom;
        Long dTod;
        if ((realDate->GetDayOfMonth(&rDom), rDom) == (d->GetDayOfMonth(&dDom), dDom) &&
                tod < (d->GetTimeOfDay(&dTod), dTod)) {
            realDate->SetDayOfMonth(Math::Min(dom + 1, monthLength));
        }
        // restore the year.
        realDate->GetMonth(&mon);
        date->SetDate(year, mon, rDom);
        // Don't normalize date here so as not to cause underflow.
    }
}

Integer JapaneseImperialCalendar::GetRolledValue(
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

Integer JapaneseImperialCalendar::InternalGetEra()
{
    Boolean set;
    return (IsSet(ERA, &set), set) ? InternalGet(ERA) : sEras.GetLength() - 1;
}

}
}
