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

#ifndef __CCM_UTIL_CALENDAR_BASECALENDAR_H__
#define __CCM_UTIL_CALENDAR_BASECALENDAR_H__

#include "ccm/util/calendar/AbstractCalendar.h"
#include "ccm.util.calendar.IBaseCalendar.h"

namespace ccm {
namespace util {
namespace calendar {

class BaseCalendar
    : public AbstractCalendar
    , public IBaseCalendar
{
public:
    class Date
        : public SyncObject
        , public ICalendarDate
    {
    public:
        CCM_INTERFACE_DECL();

    protected:
        ECode Constructor();

        ECode Constructor(
            /* [in] */ ITimeZone* zone);
    };

public:
    CCM_INTERFACE_DECL();

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
