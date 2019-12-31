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

#ifndef __CCM_UTIL_CALENDAR_LOCALGREGORIANCALENDAR_H__
#define __CCM_UTIL_CALENDAR_LOCALGREGORIANCALENDAR_H__

#include "ccm/util/calendar/BaseCalendar.h"
#include "ccm.util.calendar.IEra.h"
#include "ccm.util.calendar.ILocalGregorianCalendar.h"

namespace ccm {
namespace util {
class JapaneseImperialCalendar;
}
}

namespace ccm {
namespace util {
namespace calendar {

class LocalGregorianCalendar
    : public BaseCalendar
    , public ILocalGregorianCalendar
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

        ECode AddYear(
            /* [in] */ Integer n) override;

        ECode SetYear(
            /* [in] */ Integer year) override;

        ECode GetNormalizedYear(
            /* [out] */ Integer* normalizedYear) override;

        ECode SetNormalizedYear(
            /* [in] */ Integer normalizedYear) override;

        void SetLocalEra(
            /* [in] */ IEra* era);

        void SetLocalYear(
            /* [in] */ Integer year);

        ECode ToString(
            /* [out] */ String* desc) override;

        ECode Clone(
            /* [in] */ const InterfaceID& iid,
            /* [out] */ IInterface** obj) override;

    private:
        Integer mGregorianYear = FIELD_UNDEFINED;

        friend class LocalGregorianCalendar;
    };

protected:
    static ECode GetLocalGregorianCalendar(
        /* [in] */ const String& name,
        /* [out] */ ILocalGregorianCalendar** calendar);

public:
    CCM_INTERFACE_DECL();

    ECode Constructor(
        /* [in] */ const String& name,
        /* [in] */ const Array<IEra*>& eras);

    ECode GetName(
        /* [out] */ String* name) override;

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

    ECode NewCalendarDate(
        /* [out] */ ICalendarDate** date) override;

    ECode NewCalendarDate(
        /* [in] */ ITimeZone* zone,
        /* [out] */ ICalendarDate** date) override;

    ECode Validate(
        /* [in] */ ICalendarDate* date,
        /* [out] */ Boolean* result) override;

    ECode Normalize(
        /* [in] */ ICalendarDate* date,
        /* [out] */ Boolean* result = nullptr) override;

    ECode IsLeapYear(
        /* [in] */ Integer gregorianYear,
        /* [out] */ Boolean* leapYear) override;

    ECode IsLeapYear(
        /* [in] */ IEra* era,
        /* [in] */ Integer year,
        /* [out] */ Boolean* leapYear) override;

    ECode GetCalendarDateFromFixedDate(
        /* [in] */ ICalendarDate* date,
        /* [in] */ Long fixedDate) override;

protected:
    void NormalizeMonth(
        /* [in] */ ICalendarDate* date) override;

    void NormalizeYear(
        /* [in] */ ICalendarDate* date);

private:
    void AdjustYear(
        /* [in] */ Date* ldate,
        /* [in] */ Long millis,
        /* [in] */ Integer zoneOffset);

    Boolean ValidateEra(
        /* [in] */ IEra* era);

private:
    String mName;
    Array<IEra*> mEras;

    friend class CalendarSystem;
    friend class ccm::util::JapaneseImperialCalendar;
};

}
}
}

#endif // __CCM_UTIL_CALENDAR_LOCALGREGORIANCALENDAR_H__
