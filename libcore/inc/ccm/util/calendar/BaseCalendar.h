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

#ifndef __CCM_UTIL_CALENDAR_BASECALENDAR_H__
#define __CCM_UTIL_CALENDAR_BASECALENDAR_H__

#include "ccm/util/calendar/AbstractCalendar.h"
#include "ccm/util/calendar/CalendarDate.h"
#include "ccm.util.calendar.IBaseCalendar.h"
#include "ccm.util.calendar.IBaseCalendarDate.h"

namespace ccm {
namespace util {
namespace calendar {

class BaseCalendar
    : public AbstractCalendar
    , public IBaseCalendar
{
public:
    class Date
        : public CalendarDate
        , public IBaseCalendarDate
    {
    public:
        CCM_INTERFACE_DECL();

        ECode SetNormalizedDate(
            /* [in] */ Integer normalizedYear,
            /* [in] */ Integer month,
            /* [in] */ Integer dayOfMonth) override;

        Boolean Hit(
            /* [in] */ Integer year);

        Boolean Hit(
            /* [in] */ Long fixedDate);

        Integer GetCachedYear();

        Long GetCachedJan1();

        void SetCache(
            /* [in] */ Integer year,
            /* [in] */ Long jan1,
            /* [in] */ Integer len);

    protected:
        ECode Constructor();

        ECode Constructor(
            /* [in] */ ITimeZone* zone);

        ECode CloneImpl(
            /* [out] */ IBaseCalendarDate* newObj);

    protected:
        // Cache for the fixed date of January 1 and year length of the
        // cachedYear. A simple benchmark showed 7% performance
        // improvement with >90% cache hit. The initial values are for Gregorian.
        Integer mCachedYear = 2004;
        Long mCachedFixedDateJan1 = 731581ll;
        Long mCachedFixedDateNextJan1 = mCachedFixedDateJan1 + 366;

        friend class BaseCalendar;
    };

public:
    CCM_INTERFACE_DECL();

    ECode Validate(
        /* [in] */ ICalendarDate* date,
        /* [out] */ Boolean* result) override;

    ECode Normalize(
        /* [in] */ ICalendarDate* date,
        /* [out] */ Boolean* result = nullptr) override;

    ECode GetYearLength(
        /* [in] */ ICalendarDate* date,
        /* [out] */ Integer* days) override;

    ECode GetYearLengthInMonths(
        /* [in] */ ICalendarDate* date,
        /* [out] */ Integer* months) override;

    ECode GetMonthLength(
        /* [in] */ ICalendarDate* date,
        /* [out] */ Integer* days) override;

    ECode GetDayOfYear(
        /* [in] */ ICalendarDate* date,
        /* [out] */ Long* days) override;

    ECode GetFixedDate(
        /* [in] */ ICalendarDate* date,
        /* [out] */ Long* fraction) override;

    ECode GetFixedDate(
        /* [in] */ Integer year,
        /* [in] */ Integer month,
        /* [in] */ Integer dayOfMonth,
        /* [in] */ IBaseCalendarDate* cache,
        /* [out] */ Long* fraction) override;

    ECode GetCalendarDateFromFixedDate(
        /* [in] */ ICalendarDate* date,
        /* [in] */ Long fixedDate) override;

    static Integer GetDayOfWeekFromFixedDate(
        /* [in] */ Long fixedDate);

    ECode GetDayOfWeek(
        /* [in] */ ICalendarDate* date,
        /* [out] */ Integer* days) override;

    ECode GetYearFromFixedDate(
        /* [in] */ Long fixedDate,
        /* [out] */ Integer* year) override;

protected:
    virtual void NormalizeMonth(
        /* [in] */ ICalendarDate* date);

    Long GetDayOfYear(
        /* [in] */ Integer year,
        /* [in] */ Integer month,
        /* [in] */ Integer dayOfMonth);

    Integer GetGregorianYearFromFixedDate(
        /* [in] */ Long fixedDate);

    Boolean IsLeapYear(
        /* [in] */ ICalendarDate* date) override;

    virtual ECode IsLeapYear(
        /* [in] */ Integer normalizedYear,
        /* [out] */ Boolean* leapYear);

private:
    Integer GetMonthLength(
        /* [in] */ Integer year,
        /* [in] */ Integer month);

protected:
    static constexpr Integer DAYS_IN_MONTH[]
        //  12   1   2   3   4   5   6   7   8   9  10  11  12
        = {31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    static constexpr Integer ACCUMULATED_DAYS_IN_MONTH[]
        //  12/1 1/1 2/1 3/1 4/1 5/1 6/1 7/1 8/1 9/1 10/1 11/1 12/1
        = {-30, 0, 31, 59, 90,120,151,181,212,243, 273, 304, 334};

    static constexpr Integer ACCUMULATED_DAYS_IN_MONTH_LEAP[]
        //  12/1 1/1 2/1   3/1   4/1   5/1   6/1   7/1   8/1   9/1   10/1   11/1   12/1
        = {-30, 0, 31, 59+1, 90+1,120+1,151+1,181+1,212+1,243+1, 273+1, 304+1, 334+1};

private:
    // The base Gregorian year of FIXED_DATES[]
    static constexpr Integer BASE_YEAR = 1970;

    // Pre-calculated fixed dates of January 1 from BASE_YEAR
    // (Gregorian). This table covers all the years that can be
    // supported by the POSIX time_t (32-bit) after the Epoch. Note
    // that the data type is Integer[].
    static constexpr Integer FIXED_DATES[] = {
        719163, // 1970
        719528, // 1971
        719893, // 1972
        720259, // 1973
        720624, // 1974
        720989, // 1975
        721354, // 1976
        721720, // 1977
        722085, // 1978
        722450, // 1979
        722815, // 1980
        723181, // 1981
        723546, // 1982
        723911, // 1983
        724276, // 1984
        724642, // 1985
        725007, // 1986
        725372, // 1987
        725737, // 1988
        726103, // 1989
        726468, // 1990
        726833, // 1991
        727198, // 1992
        727564, // 1993
        727929, // 1994
        728294, // 1995
        728659, // 1996
        729025, // 1997
        729390, // 1998
        729755, // 1999
        730120, // 2000
        730486, // 2001
        730851, // 2002
        731216, // 2003
        731581, // 2004
        731947, // 2005
        732312, // 2006
        732677, // 2007
        733042, // 2008
        733408, // 2009
        733773, // 2010
        734138, // 2011
        734503, // 2012
        734869, // 2013
        735234, // 2014
        735599, // 2015
        735964, // 2016
        736330, // 2017
        736695, // 2018
        737060, // 2019
        737425, // 2020
        737791, // 2021
        738156, // 2022
        738521, // 2023
        738886, // 2024
        739252, // 2025
        739617, // 2026
        739982, // 2027
        740347, // 2028
        740713, // 2029
        741078, // 2030
        741443, // 2031
        741808, // 2032
        742174, // 2033
        742539, // 2034
        742904, // 2035
        743269, // 2036
        743635, // 2037
        744000, // 2038
        744365, // 2039
    };
};

}
}
}

#endif // __CCM_UTIL_CALENDAR_BASECALENDAR_H__
