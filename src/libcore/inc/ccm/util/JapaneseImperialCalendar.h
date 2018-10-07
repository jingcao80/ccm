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

#ifndef __CCM_UTIL_JAPANESEIMPERIALCALENDAR_H__
#define __CCM_UTIL_JAPANESEIMPERIALCALENDAR_H__

#include "ccm/util/Calendar.h"
#include "ccm/util/calendar/LocalGregorianCalendar.h"
#include "ccm.core.ILong.h"
#include "ccm.util.IJapaneseImperialCalendar.h"
#include "ccm.util.calendar.ICalendarDate.h"
#include "ccm.util.calendar.ICalendarSystem.h"
#include "ccm.util.calendar.IEra.h"

using ccm::core::ILong;
using ccm::util::calendar::ICalendarDate;
using ccm::util::calendar::ICalendarSystem;
using ccm::util::calendar::IEra;
using ccm::util::calendar::LocalGregorianCalendar;

namespace ccm {
namespace util {

class JapaneseImperialCalendar
    : public Calendar
    , public IJapaneseImperialCalendar
{
public:
    CCM_INTERFACE_DECL();

    ECode Constructor(
        /* [in] */ ITimeZone* zone,
        /* [in] */ ILocale* locale);

    ECode Constructor(
        /* [in] */ ITimeZone* zone,
        /* [in] */ ILocale* locale,
        /* [in] */ Boolean flag);

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

    ECode Roll(
        /* [in] */ Integer field,
        /* [in] */ Boolean up) override;

    ECode Roll(
        /* [in] */ Integer field,
        /* [in] */ Integer amount) override;

    ECode GetDisplayName(
        /* [in] */ Integer field,
        /* [in] */ Integer style,
        /* [in] */ ILocale* locale,
        /* [out] */ String* name) override;

    ECode GetDisplayNames(
        /* [in] */ Integer field,
        /* [in] */ Integer style,
        /* [in] */ ILocale* locale,
        /* [out] */ IMap** names) override;

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

    ECode Clone(
        /* [in] */ const InterfaceID& iid,
        /* [out] */ IInterface** obj) override;

    ECode GetTimeZone(
        /* [out] */ ITimeZone** zone) override;

    ECode SetTimeZone(
        /* [in] */ ITimeZone* zone) override;

protected:
    ECode ComputeFields() override;

    ECode ComputeTime() override;

private:
    static AutoPtr<ICalendarSystem> GetJcal();

    // Gregorian calendar instance. This is required because era
    // transition dates are given in Gregorian dates.
    static AutoPtr<ICalendarSystem> GetGcal();

    // The Era instance representing "before Meiji".
    static AutoPtr<IEra> GetBEFORE_MEIJI_ERA();

    static ECode StaticInitialize();

    Long GetYearOffsetInMillis(
        /* [in] */ ICalendarDate* date);

    Integer ComputeFields(
        /* [in] */ Integer fieldMask,
        /* [in] */ Integer tzMask);

    Integer GetWeekNumber(
        /* [in] */ Long fixedDay1,
        /* [in] */ Long fixedDate);

    Long GetFixedDate(
        /* [in] */ Integer era,
        /* [in] */ Integer year,
        /* [in] */ Integer fieldMask);

    Long GetFixedDateJan1(
        /* [in] */ LocalGregorianCalendar::Date* date,
        /* [in] */ Long fixedDate);

    Long GetFixedDateMonth1(
        /* [in] */ LocalGregorianCalendar::Date* date,
        /* [in] */ Long fixedDate);

    static AutoPtr<LocalGregorianCalendar::Date> GetCalendarDate(
        /* [in] */ Long fd);

    Integer MonthLength(
        /* [in] */ Integer month,
        /* [in] */ Integer gregorianYear);

    Integer MonthLength(
        /* [in] */ Integer month);

    Integer ActualMonthLength();


    /**
     * After adjustments such as add(MONTH), add(YEAR), we don't want the
     * month to jump around.  E.g., we don't want Jan 31 + 1 month to go to Mar
     * 3, we want it to go to Feb 28.  Adjustments which might run into this
     * problem call this method to retain the proper month.
     */
    void PinDayOfMonth(
        /* [in] */ LocalGregorianCalendar::Date* date)
    {}

    static Integer GetEraIndex(
        /* [in] */ LocalGregorianCalendar::Date* date)
    {
        return 0;
    }



    /**
     * Returns the new value after 'roll'ing the specified value and amount.
     */
    static Integer GetRolledValue(
        /* [in] */ Integer value,
        /* [in] */ Integer amount,
        /* [in] */ Integer min,
        /* [in] */ Integer max)
    {
        return 0;
    }

    Boolean IsTransitionYear(
        /* [in] */ Integer normalizedYear)
    {
        return false;
    }



    static Integer GetTransitionEraIndex(
        /* [in] */ LocalGregorianCalendar::Date* date)
    {
        return 0;
    }





    AutoPtr<JapaneseImperialCalendar> GetNormalizedCalendar()
    {
        return nullptr;
    }

private:
    static constexpr Integer EPOCH_OFFSET   = 719163; // Fixed date of January 1, 1970 (Gregorian)
    static constexpr Integer EPOCH_YEAR     = 1970;

    // Useful millisecond constants.  Although ONE_DAY and ONE_WEEK can fit
    // into ints, they must be longs in order to prevent arithmetic overflow
    // when performing (bug 4173516).
    static constexpr Integer ONE_SECOND = 1000;
    static constexpr Integer ONE_MINUTE = 60 * ONE_SECOND;
    static constexpr Integer ONE_HOUR = 60 * ONE_MINUTE;
    static constexpr Long ONE_DAY = 24 * ONE_HOUR;
    static constexpr Long ONE_WEEK = 7 * ONE_DAY;

    // Imperial eras. The sun.util.calendar.LocalGregorianCalendar
    // doesn't have an Era representing before Meiji, which is
    // inconvenient for a Calendar. So, era[0] is a reference to
    // BEFORE_MEIJI_ERA.
    static Array<IEra*> sEras;

    // Fixed date of the first date of each era.
    static Array<Long> sSinceFixedDates;

    /*
     * <pre>
     *                                 Greatest       Least
     * Field name             Minimum   Minimum     Maximum     Maximum
     * ----------             -------   -------     -------     -------
     * ERA                          0         0           1           1
     * YEAR                -292275055         1           ?           ?
     * MONTH                        0         0          11          11
     * WEEK_OF_YEAR                 1         1          52*         53
     * WEEK_OF_MONTH                0         0           4*          6
     * DAY_OF_MONTH                 1         1          28*         31
     * DAY_OF_YEAR                  1         1         365*        366
     * DAY_OF_WEEK                  1         1           7           7
     * DAY_OF_WEEK_IN_MONTH        -1        -1           4*          6
     * AM_PM                        0         0           1           1
     * HOUR                         0         0          11          11
     * HOUR_OF_DAY                  0         0          23          23
     * MINUTE                       0         0          59          59
     * SECOND                       0         0          59          59
     * MILLISECOND                  0         0         999         999
     * ZONE_OFFSET             -13:00    -13:00       14:00       14:00
     * DST_OFFSET                0:00      0:00        0:20        2:00
     * </pre>
     * *: depends on eras
     */
    static constexpr Integer MIN_VALUES[] = {
        0,              // ERA
        -292275055,     // YEAR
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
        -13*ONE_HOUR,   // ZONE_OFFSET (UNIX compatibility)
        0               // DST_OFFSET
    };
    static constexpr Integer LEAST_MAX_VALUES[] = {
        0,              // ERA (initialized later)
        0,              // YEAR (initialized later)
        JANUARY,        // MONTH (Showa 64 ended in January.)
        0,              // WEEK_OF_YEAR (Showa 1 has only 6 days which could be 0 weeks.)
        4,              // WEEK_OF_MONTH
        28,             // DAY_OF_MONTH
        0,              // DAY_OF_YEAR (initialized later)
        SATURDAY,       // DAY_OF_WEEK
        4,              // DAY_OF_WEEK_IN
        PM,             // AM_PM
        11,             // HOUR
        23,             // HOUR_OF_DAY
        59,             // MINUTE
        59,             // SECOND
        999,            // MILLISECOND
        14*ONE_HOUR,    // ZONE_OFFSET
        20*ONE_MINUTE   // DST_OFFSET (historical least maximum)
    };
    static constexpr Integer MAX_VALUES[] = {
        0,              // ERA
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
        14*ONE_HOUR,    // ZONE_OFFSET
        2*ONE_HOUR      // DST_OFFSET (double summer time)
    };

    /**
     * jdate always has a ccm::util::calendar::LocalGregorianCalendar::Date instance to
     * avoid overhead of creating it for each calculation.
     */
    AutoPtr<LocalGregorianCalendar::Date> mJdate;

    /**
     * Temporary int[2] to get time zone offsets. zoneOffsets[0] gets
     * the GMT offset value and zoneOffsets[1] gets the daylight saving
     * value.
     */
    Array<Integer> mZoneOffsets;

    /**
     * Temporary storage for saving original fields[] values in
     * non-lenient mode.
     */
    Array<Integer> mOriginalFields;

    /**
     * The fixed date corresponding to jdate. If the value is
     * Long.MIN_VALUE, the fixed date value is unknown.
     */
    Long mCachedFixedDate = ILong::MIN_VALUE;

    static Boolean sInitialized;
    static SyncObject sInitLock;
};

}
}

#endif // __CCM_UTIL_JAPANESEIMPERIALCALENDAR_H__
