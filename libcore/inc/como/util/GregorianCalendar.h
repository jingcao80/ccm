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

#ifndef __COMO_UTIL_GREGORIANCALENDAR_H__
#define __COMO_UTIL_GREGORIANCALENDAR_H__

#include "como/util/Calendar.h"
#include "como.core.ILong.h"
#include "como.util.IDate.h"
#include "como.util.IGregorianCalendar.h"
#include "como.util.ILocale.h"
#include "como.util.ITimeZone.h"
#include "como.util.calendar.IBaseCalendar.h"
#include "como.util.calendar.IBaseCalendarDate.h"
#include "como.util.calendar.IEra.h"
#include "como.util.calendar.IGregorian.h"
#include "como.util.calendar.IJulianCalendar.h"

using como::core::ILong;
using como::util::calendar::IBaseCalendar;
using como::util::calendar::IBaseCalendarDate;
using como::util::calendar::IEra;
using como::util::calendar::IGregorian;
using como::util::calendar::IJulianCalendar;

namespace como {
namespace util {

class GregorianCalendar
    : public Calendar
    , public IGregorianCalendar
{
public:
    COMO_INTERFACE_DECL();

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
        /* [out] */ Boolean& same) override;

    ECode GetHashCode(
        /* [out] */ Integer& hash) override;

    ECode Add(
        /* [in] */ Integer field,
        /* [in] */ Integer amount) override;

    ECode Roll(
        /* [in] */ Integer field,
        /* [in] */ Boolean up) override;

    ECode Roll(
        /* [in] */ Integer field,
        /* [in] */ Integer amount) override;

    ECode GetMinimum(
        /* [in] */ Integer field,
        /* [out] */ Integer* value) override;

    ECode GetMaximum(
        /* [in] */ Integer field,
        /* [out] */ Integer* value) override;

    ECode GetGreatestMinimum(
        /* [in] */ Integer field,
        /* [out] */ Integer* value) override;

    ECode GetLeastMaximum(
        /* [in] */ Integer field,
        /* [out] */ Integer* value) override;

    ECode GetActualMinimum(
        /* [in] */ Integer field,
        /* [out] */ Integer* value) override;

    ECode GetActualMaximum(
        /* [in] */ Integer field,
        /* [out] */ Integer* value) override;

    ECode GetTimeZone(
        /* [out] */ ITimeZone** zone) override;

    ECode SetTimeZone(
        /* [in] */ ITimeZone* zone) override;

    ECode IsWeekDateSupported(
        /* [out] */ Boolean* supported) override;

    ECode GetWeekYear(
        /* [out] */ Integer* weekYear) override;

    ECode SetWeekDate(
        /* [in] */ Integer weekYear,
        /* [in] */ Integer weekOfYear,
        /* [in] */ Integer dayOfWeek) override;

    ECode GetWeeksInWeekYear(
        /* [out] */ Integer* weeks) override;

protected:
    ECode CloneImpl(
        /* [in] */ IGregorianCalendar* newObj);

    ECode ComputeFields() override;

    ECode ComputeTime() override;

private:
    static AutoPtr<IGregorian> GetGcal();

    void SetGregorianChange(
        /* [in] */ Long cutoverTime);

    Long GetYearOffsetInMillis();

    Integer ComputeFields(
        /* [in] */ Integer fieldMask,
        /* [in] */ Integer tzMask);

    Integer GetWeekNumber(
        /* [in] */ Long fixedDay1,
        /* [in] */ Long fixedDate);

    Long AdjustForZoneAndDaylightSavingsTime(
        /* [in] */ Integer tzMask,
        /* [in] */ Long utcTimeInMillis,
        /* [in] */ ITimeZone* zone);

    Integer AdjustDstOffsetForInvalidWallClock(
        /* [in] */ Long standardTimeInZone,
        /* [in] */ ITimeZone* zone,
        /* [in] */ Integer dstOffset);

    Long GetFixedDate(
        /* [in] */ IBaseCalendar* cal,
        /* [in] */ Integer year,
        /* [in] */ Integer fieldMask);

    AutoPtr<GregorianCalendar> GetNormalizedCalendar();

    static AutoPtr<IBaseCalendar> GetJulianCalendarSystem();

    AutoPtr<IBaseCalendar> GetCutoverCalendarSystem();

    Boolean IsCutoverYear(
        /* [in] */ Integer normalizedYear);

    Long GetFixedDateJan1(
        /* [in] */ IBaseCalendarDate* date,
        /* [in] */ Long fixedDate);

    Long GetFixedDateMonth1(
        /* [in] */ IBaseCalendarDate* date,
        /* [in] */ Long fixedDate);

    AutoPtr<IBaseCalendarDate> GetCalendarDate(
        /* [in] */ Long fd);

    AutoPtr<IBaseCalendarDate> GetGregorianCutoverDate();

    AutoPtr<IBaseCalendarDate> GetLastJulianDate();

    Integer MonthLength(
        /* [in] */ Integer month,
        /* [in] */ Integer year);

    Integer MonthLength(
        /* [in] */ Integer month);

    Integer ActualMonthLength();

    void PinDayOfMonth();

    Long GetCurrentFixedDate();

    static Integer GetRolledValue(
        /* [in] */ Integer value,
        /* [in] */ Integer amount,
        /* [in] */ Integer min,
        /* [in] */ Integer max);

    Integer InternalGetEra();

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

public:
    static constexpr Integer BCE = 0;

    static constexpr Integer CE = 1;

    static constexpr Integer MONTH_LENGTH[]
        = {31,28,31,30,31,30,31,31,30,31,30,31}; // 0-based
    static constexpr Integer LEAP_MONTH_LENGTH[]
        = {31,29,31,30,31,30,31,31,30,31,30,31}; // 0-based

    static constexpr Integer MIN_VALUES[] = {
        BCE,            // ERA
        1,              // YEAR
        JANUARY,        // MONTH
        1,              // WEEK_OF_YEAR
        0,              // WEEK_OF_MONTH
        1,              // DAY_OF_MONTH
        1,              // DAY_OF_YEAR
        SUNDAY,         // DAY_OF_WEEK
        1,              // DAY_OF_WEEK_IN_MONTH
        AM,             // AM_PM
        0,              // HOUR
        0,              // HOUR_OF_DAY
        0,              // MINUTE
        0,              // SECOND
        0,              // MILLISECOND
        -13 * ONE_HOUR, // ZONE_OFFSET (UNIX compatibility)
        0               // DST_OFFSET
    };
    static constexpr Integer LEAST_MAX_VALUES[] = {
        CE,             // ERA
        292269054,      // YEAR
        DECEMBER,       // MONTH
        52,             // WEEK_OF_YEAR
        4,              // WEEK_OF_MONTH
        28,             // DAY_OF_MONTH
        365,            // DAY_OF_YEAR
        SATURDAY,       // DAY_OF_WEEK
        4,              // DAY_OF_WEEK_IN
        PM,             // AM_PM
        11,             // HOUR
        23,             // HOUR_OF_DAY
        59,             // MINUTE
        59,             // SECOND
        999,            // MILLISECOND
        14 * ONE_HOUR,  // ZONE_OFFSET
        20 * ONE_MINUTE // DST_OFFSET (historical least maximum)
    };
    static constexpr Integer MAX_VALUES[] = {
        CE,             // ERA
        292278994,      // YEAR
        DECEMBER,       // MONTH
        53,             // WEEK_OF_YEAR
        6,              // WEEK_OF_MONTH
        31,             // DAY_OF_MONTH
        366,            // DAY_OF_YEAR
        SATURDAY,       // DAY_OF_WEEK
        6,              // DAY_OF_WEEK_IN
        PM,             // AM_PM
        11,             // HOUR
        23,             // HOUR_OF_DAY
        59,             // MINUTE
        59,             // SECOND
        999,            // MILLISECOND
        14 * ONE_HOUR,  // ZONE_OFFSET
        2 * ONE_HOUR    // DST_OFFSET (double summer time)
    };

    static constexpr Long DEFAULT_GREGORIAN_CUTOVER = -12219292800000ll;

private:
    // Reference to the JulianCalendar instance (singleton), set as needed. See
    // getJulianCalendarSystem().
    static AutoPtr<IJulianCalendar> sJcal;
    static SyncObject sJcalLock;

    // JulianCalendar eras. See getJulianCalendarSystem().
    static Array<IEra*> sJeras;

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

    /**
     * gdate always has a sun.util.calendar.Gregorian.Date instance to
     * avoid overhead of creating it. The assumption is that most
     * app
     */
    AutoPtr<IBaseCalendarDate> mGdate;

    /**
     * Reference to either gdate or a JulianCalendar.Date
     * instance. After calling complete(), this value is guaranteed to
     * be set.
     */
    AutoPtr<IBaseCalendarDate> mCdate;

    AutoPtr<IBaseCalendar> mCalsys;

    /**
     * Temporary int[2] to get time zone offsets. zoneOffsets[0] gets
     * the GMT offset value and zoneOffsets[1] gets the DST saving
     * value.
     */
    Array<Integer> mZoneOffsets;

    /**
     * Temporary storage for saving original fields[] values in
     * non-lenient mode.
     */
    Array<Integer> mOriginalFields;

    Long mCachedFixedDate = ILong::MIN_VALUE;
};

}
}

#endif // __COMO_UTIL_GREGORIANCALENDAR_H__
