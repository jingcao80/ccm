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

#ifndef __CCM_UTIL_CALENDAR_ABSTRACTCALENDAR_H__
#define __CCM_UTIL_CALENDAR_ABSTRACTCALENDAR_H__

#include "ccm/util/calendar/CalendarSystem.h"
#include "ccm.util.ITimeZone.h"
#include "ccm.util.calendar.ICalendarDate.h"
#include "ccm.util.calendar.IEra.h"

namespace ccm {
namespace util {
namespace calendar {

class AbstractCalendar
    : public CalendarSystem
{
public:
    ECode GetEra(
        /* [in] */ const String& eraName,
        /* [out] */ IEra** era) override;

    ECode GetEras(
        /* [out, callee] */ Array<IEra*>* eras) override;

    ECode SetEra(
        /* [in] */ ICalendarDate* date,
        /* [in] */ const String& eraName) override;

    ECode GetCalendarDate(
        /* [out] */ ICalendarDate** date) override;

    ECode GetCalendarDate(
        /* [in] */ Long millis,
        /* [out] */ ICalendarDate** date) override;

    ECode GetCalendarDate(
        /* [in] */ Long millis,
        /* [in] */ ITimeZone* zone,
        /* [out] */ ICalendarDate** date) override;

    ECode GetCalendarDate(
        /* [in] */ Long millis,
        /* [in] */ ICalendarDate* date) override;

    ECode GetTime(
        /* [in] */ ICalendarDate* date,
        /* [out] */ Long* time) override;

    virtual Long GetTimeOfDayValue(
        /* [in] */ ICalendarDate* date);

    ECode SetTimeOfDay(
        /* [in] */ ICalendarDate* date,
        /* [in] */ Integer timeOfDay) override;

    ECode GetWeekLength(
        /* [out] */ Integer* weeks) override;

    ECode GetNthDayOfWeek(
        /* [in] */ Integer nth,
        /* [in] */ Integer dayOfWeek,
        /* [in] */ ICalendarDate* inDate,
        /* [out] */ ICalendarDate** outDate) override;

    static Long GetDayOfWeekDateOnOrBefore(
        /* [in] */ Long fixedDate,
        /* [in] */ Integer dayOfWeek);

    Boolean ValidateTime(
        /* [in] */ ICalendarDate* date);

protected:
    AbstractCalendar()
    {}

    virtual void SetEras(
        /* [in] */ const Array<IEra*>& eras);

    virtual Long GetTimeOfDay(
        /* [in] */ ICalendarDate* date);

    virtual Boolean IsLeapYear(
        /* [in] */ ICalendarDate* date) = 0;

    static Long GetDayOfWeekDateBefore(
        /* [in] */ Long fixedDate,
        /* [in] */ Integer dayOfWeek);

    static Long GetDayOfWeekDateAfter(
        /* [in] */ Long fixedDate,
        /* [in] */ Integer dayOfWeek);

    virtual Long GetFixedDate(
        /* [in] */ ICalendarDate* date) = 0;

    virtual void GetCalendarDateFromFixedDate(
        /* [in] */ ICalendarDate* date,
        /* [in] */ Long fixedDate) = 0;

    Integer NormalizeTime(
        /* [in] */ ICalendarDate* date);

protected:
    // The constants assume no leap seconds support.
    static constexpr Integer SECOND_IN_MILLIS = 1000;
    static constexpr Integer MINUTE_IN_MILLIS = SECOND_IN_MILLIS * 60;
    static constexpr Integer HOUR_IN_MILLIS = MINUTE_IN_MILLIS * 60;
    static constexpr Integer DAY_IN_MILLIS = HOUR_IN_MILLIS * 24;

    // The number of days between January 1, 1 and January 1, 1970 (Gregorian)
    static constexpr Integer EPOCH_OFFSET = 719163;

private:
    Array<IEra*> mEras;
};

}
}
}

#endif // __CCM_UTIL_CALENDAR_ABSTRACTCALENDAR_H__
