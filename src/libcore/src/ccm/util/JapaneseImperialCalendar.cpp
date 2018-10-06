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
#include "ccm.util.calendar.IBaseCalendar.h"
#include "ccm.util.calendar.ICalendarDate.h"

using ccm::core::AutoLock;
using ccm::core::IInteger;
using ccm::core::Math;
using ccm::core::System;
using ccm::util::calendar::CalendarSystem;
using ccm::util::calendar::CEra;
using ccm::util::calendar::IBaseCalendar;
using ccm::util::calendar::ICalendarDate;
using ccm::util::calendar::ICalendarSystem;
using ccm::util::calendar::IID_ICalendarDate;
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

}
}
