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

#ifndef __CCM_UTIL_GREGORIANCALENDAR_H__
#define __CCM_UTIL_GREGORIANCALENDAR_H__

#include "ccm/util/Calendar.h"
#include "ccm.util.IDate.h"
#include "ccm.util.IGregorianCalendar.h"
#include "ccm.util.ILocale.h"
#include "ccm.util.ITimeZone.h"
#include "ccm.util.calendar.IBaseCalendar.h"
#include "ccm.util.calendar.IBaseCalendarDate.h"
#include "ccm.util.calendar.IGregorian.h"

using ccm::util::calendar::IBaseCalendar;
using ccm::util::calendar::IBaseCalendarDate;
using ccm::util::calendar::IGregorian;

namespace ccm {
namespace util {

class GregorianCalendar
    : public Calendar
    , public IGregorianCalendar
{
public:
    CCM_INTERFACE_DECL();

    ECode Constructor();

    ECode Constructor(
        /* [in] */ ITimeZone* zone);

    ECode Constructor(
        /* [in] */ ILocale* locale);

    ECode Constructor(
        /* [in] */ ITimeZone* zone,
        /* [in] */ ILocale* locale);

    ECode Constructor(
        /* [in] */ Integer year,
        /* [in] */ Integer month,
        /* [in] */ Integer dayOfMonth);

    ECode Constructor(
        /* [in] */ Integer year,
        /* [in] */ Integer month,
        /* [in] */ Integer dayOfMonth,
        /* [in] */ Integer hourOfDay,
        /* [in] */ Integer minute);

    ECode Constructor(
        /* [in] */ Integer year,
        /* [in] */ Integer month,
        /* [in] */ Integer dayOfMonth,
        /* [in] */ Integer hourOfDay,
        /* [in] */ Integer minute,
        /* [in] */ Integer second);

    ECode Constructor(
        /* [in] */ Integer year,
        /* [in] */ Integer month,
        /* [in] */ Integer dayOfMonth,
        /* [in] */ Integer hourOfDay,
        /* [in] */ Integer minute,
        /* [in] */ Integer second,
        /* [in] */ Integer millis);

    ECode Constructor(
        /* [in] */ ITimeZone* zone,
        /* [in] */ ILocale* locale,
        /* [in] */ Boolean flag);

    ECode Constructor(
        /* [in] */ Long milliseconds);

    ECode SetGregorianChange(
        /* [in] */ IDate* date) override;

    ECode GetGregorianChange(
        /* [out] */ IDate** date) override final;

    ECode IsLeapYear(
        /* [in] */ Integer year,
        /* [out] */ Boolean* leap) override;

    ECode GetCalendarType(
        /* [out] */ String* type) override;

    ECode Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* same) override;

    ECode GetHashCode(
        /* [out] */ Integer* hash) override;

    ECode Add(
        /* [in] */ Integer field,
        /* [in] */ Integer amount) override;

private:
    static AutoPtr<IGregorian> GetGcal();

    void SetGregorianChange(
        /* [in] */ Long cutoverTime);


    static AutoPtr<IBaseCalendar> GetJulianCalendarSystem()
    {
        return nullptr;
    }

    AutoPtr<IBaseCalendarDate> GetCalendarDate(
        /* [in] */ Long fd)
    {
        return nullptr;
    }

    Long AdjustForZoneAndDaylightSavingsTime(
        /* [in] */ Integer tzMask,
        /* [in] */ Long utcTimeInMillis,
        /* [in] */ ITimeZone* zone)
    {
        return 0;
    }

    AutoPtr<IBaseCalendarDate> GetGregorianCutoverDate()
    {
        return nullptr;
    }

    Integer InternalGetEra()
    {
        return 0;
    }

    void PinDayOfMonth()
    {}

    Long GetCurrentFixedDate()
    {
        return 0;
    }

public:
    static constexpr Integer BCE = 0;

    static constexpr Integer CE = 1;

    static constexpr Long DEFAULT_GREGORIAN_CUTOVER = -12219292800000ll;

private:
    static constexpr Integer EPOCH_OFFSET = 719163; // Fixed date of January 1, 1970 (Gregorian)
    static constexpr Integer EPOCH_YEAR = 1970;

    // Useful millisecond constants.  Although ONE_DAY and ONE_WEEK can fit
    // into ints, they must be longs in order to prevent arithmetic overflow
    // when performing (bug 4173516).
    static constexpr Integer ONE_SECOND = 1000;
    static constexpr Integer ONE_MINUTE = 60 * ONE_SECOND;
    static constexpr Integer ONE_HOUR = 60 * ONE_MINUTE;
    static constexpr Long ONE_DAY = 24 * ONE_HOUR;
    static constexpr Long ONE_WEEK = 7 * ONE_DAY;

    /**
     * The point at which the Gregorian calendar rules are used, measured in
     * milliseconds from the standard epoch.  Default is October 15, 1582
     * (Gregorian) 00:00:00 UTC or -12219292800000L.  For this value, October 4,
     * 1582 (Julian) is followed by October 15, 1582 (Gregorian).  This
     * corresponds to Julian day number 2299161.
     */
    Long mGregorianCutover = DEFAULT_GREGORIAN_CUTOVER;

    /**
     * The fixed date of the gregorianCutover.
     */
    Long mGregorianCutoverDate =
            (((DEFAULT_GREGORIAN_CUTOVER + 1) / ONE_DAY) - 1) + EPOCH_OFFSET; // == 577736

    /**
     * The normalized year of the gregorianCutover in Gregorian, with
     * 0 representing 1 BCE, -1 representing 2 BCE, etc.
     */
    Integer mGregorianCutoverYear = 1582;

    /**
     * The normalized year of the gregorianCutover in Julian, with 0
     * representing 1 BCE, -1 representing 2 BCE, etc.
     */
    Integer mGregorianCutoverYearJulian = 1582;

    AutoPtr<IBaseCalendarDate> mGdate;
};

}
}

#endif // __CCM_UTIL_GREGORIANCALENDAR_H__
