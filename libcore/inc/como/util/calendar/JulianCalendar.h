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

#ifndef __COMO_UTIL_CALENDAR_JULIANCALENDAR_H__
#define __COMO_UTIL_CALENDAR_JULIANCALENDAR_H__

#include "como/util/calendar/BaseCalendar.h"
#include "como.util.calendar.IJulianCalendar.h"

namespace como {
namespace util {
namespace calendar {

class JulianCalendar
    : public BaseCalendar
    , public IJulianCalendar
{
protected:
    class Date
        : public BaseCalendar::Date
    {
    public:
        ECode Constructor();

        ECode Constructor(
            /* [in] */ ITimeZone* zone);

        ECode SetEra(
            /* [in] */ IEra* era) override;

        void SetKnownEra(
            /* [in] */ IEra* era);

        ECode GetNormalizedYear(
            /* [out] */ Integer& normalizedYear) override;

        ECode SetNormalizedYear(
            /* [in] */ Integer normalizedYear) override;

        ECode ToString(
            /* [out] */ String& desc) override;

        ECode Clone(
            /* [in] */ const InterfaceID& iid,
            /* [out] */ IInterface** obj) override;

        friend class JulianCalendar;
    };

public:
    COMO_INTERFACE_DECL();

    ECode Constructor();

    ECode GetName(
        /* [out] */ String& name) override;

    ECode GetCalendarDate(
        /* [out] */ AutoPtr<ICalendarDate>& date) override;

    ECode GetCalendarDate(
        /* [in] */ Long millis,
        /* [out] */ AutoPtr<ICalendarDate>& date) override;

    ECode GetCalendarDate(
        /* [in] */ Long millis,
        /* [in] */ ICalendarDate* date) override;

    ECode GetCalendarDate(
        /* [in] */ Long millis,
        /* [in] */ ITimeZone* zone,
        /* [out] */ AutoPtr<ICalendarDate>& date) override;

    ECode NewCalendarDate(
        /* [out] */ AutoPtr<ICalendarDate>& date) override;

    ECode NewCalendarDate(
        /* [in] */ ITimeZone* zone,
        /* [out] */ AutoPtr<ICalendarDate>& date) override;

    ECode GetFixedDate(
        /* [in] */ Integer year,
        /* [in] */ Integer month,
        /* [in] */ Integer dayOfMonth,
        /* [in] */ IBaseCalendarDate* cache,
        /* [out] */ Long& fraction) override;

    ECode GetCalendarDateFromFixedDate(
        /* [in] */ ICalendarDate* date,
        /* [in] */ Long fixedDate) override;

    ECode GetYearFromFixedDate(
        /* [in] */ Long fixedDate,
        /* [out] */ Integer& year) override;

    ECode GetDayOfWeek(
        /* [in] */ ICalendarDate* date,
        /* [out] */ Integer& days) override;

    ECode IsLeapYear(
        /* [in] */ Integer jyear,
        /* [out] */ Boolean& leapYear) override;

    using BaseCalendar::GetFixedDate;

private:
    static const Array<IEra*>& GetEras();

private:
    static constexpr Integer BCE = 0;
    static constexpr Integer CE = 1;
    static constexpr Integer JULIAN_EPOCH = -1;
};

}
}
}

#endif // __COMO_UTIL_CALENDAR_JULIANCALENDAR_H__
