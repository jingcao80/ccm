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

#ifndef __CCM_UTIL_SIMPLETIMEZONE_H__
#define __CCM_UTIL_SIMPLETIMEZONE_H__

#include "ccm/util/TimeZone.h"
#include "ccm.util.ISimpleTimeZone.h"
#include "ccm.util.calendar.IBaseCalendar.h"
#include "ccm.util.calendar.IBaseCalendarDate.h"
#include "ccm.util.calendar.ICalendarDate.h"
#include "ccm.util.calendar.ICalendarSystem.h"
#include "ccm.util.calendar.IGregorian.h"

using ccm::util::calendar::IBaseCalendar;
using ccm::util::calendar::IBaseCalendarDate;
using ccm::util::calendar::ICalendarDate;
using ccm::util::calendar::ICalendarSystem;
using ccm::util::calendar::IGregorian;

namespace ccm {
namespace util {

class SimpleTimeZone
    : public TimeZone
    , public ISimpleTimeZone
{
public:
    CCM_INTERFACE_DECL();

    ECode Constructor(
        /* [in] */ Integer rawOffset,
        /* [in] */ const String& id);

    ECode Constructor(
        /* [in] */ Integer rawOffset,
        /* [in] */ const String& id,
        /* [in] */ Integer startMonth,
        /* [in] */ Integer startDay,
        /* [in] */ Integer startDayOfWeek,
        /* [in] */ Integer startTime,
        /* [in] */ Integer endMonth,
        /* [in] */ Integer endDay,
        /* [in] */ Integer endDayOfWeek,
        /* [in] */ Integer endTime);

    ECode Constructor(
        /* [in] */ Integer rawOffset,
        /* [in] */ const String& id,
        /* [in] */ Integer startMonth,
        /* [in] */ Integer startDay,
        /* [in] */ Integer startDayOfWeek,
        /* [in] */ Integer startTime,
        /* [in] */ Integer endMonth,
        /* [in] */ Integer endDay,
        /* [in] */ Integer endDayOfWeek,
        /* [in] */ Integer endTime,
        /* [in] */ Integer dstSavings);

    ECode Constructor(
        /* [in] */ Integer rawOffset,
        /* [in] */ const String& id,
        /* [in] */ Integer startMonth,
        /* [in] */ Integer startDay,
        /* [in] */ Integer startDayOfWeek,
        /* [in] */ Integer startTime,
        /* [in] */ Integer startTimeMode,
        /* [in] */ Integer endMonth,
        /* [in] */ Integer endDay,
        /* [in] */ Integer endDayOfWeek,
        /* [in] */ Integer endTime,
        /* [in] */ Integer endTimeMode,
        /* [in] */ Integer dstSavings);

    ECode SetStartYear(
        /* [in] */ Integer year) override;

    ECode SetStartRule(
        /* [in] */ Integer startMonth,
        /* [in] */ Integer startDay,
        /* [in] */ Integer startDayOfWeek,
        /* [in] */ Integer startTime) override;

    ECode SetStartRule(
        /* [in] */ Integer startMonth,
        /* [in] */ Integer startDay,
        /* [in] */ Integer startTime) override;

    ECode SetStartRule(
        /* [in] */ Integer startMonth,
        /* [in] */ Integer startDay,
        /* [in] */ Integer startDayOfWeek,
        /* [in] */ Integer startTime,
        /* [in] */ Boolean after) override;

    ECode SetEndRule(
        /* [in] */ Integer endMonth,
        /* [in] */ Integer endDay,
        /* [in] */ Integer endDayOfWeek,
        /* [in] */ Integer endTime) override;

    ECode SetEndRule(
        /* [in] */ Integer endMonth,
        /* [in] */ Integer endDay,
        /* [in] */ Integer endTime) override;

    ECode SetEndRule(
        /* [in] */ Integer endMonth,
        /* [in] */ Integer endDay,
        /* [in] */ Integer endDayOfWeek,
        /* [in] */ Integer endTime,
        /* [in] */ Boolean after) override;

    ECode GetOffset(
        /* [in] */ Long date,
        /* [out] */ Integer* offset) override;

    virtual Integer GetOffsets(
        /* [in] */ Long date,
        /* [in] */ Array<Integer>&& offsets);

    ECode GetOffset(
        /* [in] */ Integer era,
        /* [in] */ Integer year,
        /* [in] */ Integer month,
        /* [in] */ Integer day,
        /* [in] */ Integer dayOfWeek,
        /* [in] */ Integer milliseconds,
        /* [out] */ Integer* offset) override;

    ECode GetRawOffset(
        /* [out] */ Integer* offset) override;

    ECode SetRawOffset(
        /* [in] */ Integer offsetMillis) override;

    ECode SetDSTSavings(
        /* [in] */ Integer millisSavedDuringDST) override;

    ECode GetDSTSavings(
        /* [out] */ Integer* savingTime) override;

    ECode UseDaylightTime(
        /* [out] */ Boolean* result) override;

    ECode ObservesDaylightTime(
        /* [out] */ Boolean* result) override;

    ECode InDaylightTime(
        /* [in] */ IDate* date,
        /* [out] */ Boolean* result) override;

    ECode GetHashCode(
        /* [out] */ Integer* hash) override;

    ECode Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* same) override;

    ECode HasSameRules(
        /* [in] */ ITimeZone* other,
        /* [out] */ Boolean* result) override;

    ECode ToString(
        /* [out] */ String* desc) override;

protected:
    ECode CloneImpl(
        /* [in] */ ISimpleTimeZone* newObj);

private:
    Integer GetOffset(
        /* [in] */ ICalendarSystem* cal,
        /* [in] */ ICalendarDate* cdate,
        /* [in] */ Integer year,
        /* [in] */ Long time);

    Long GetStart(
        /* [in] */ ICalendarSystem* cal,
        /* [in] */ ICalendarDate* cdate,
        /* [in] */ Integer year);

    Long GetEnd(
        /* [in] */ ICalendarSystem* cal,
        /* [in] */ ICalendarDate* cdate,
        /* [in] */ Integer year);

    Long GetTransition(
        /* [in] */ ICalendarSystem* cal,
        /* [in] */ ICalendarDate* cdate,
        /* [in] */ Integer mode,
        /* [in] */ Integer year,
        /* [in] */ Integer month,
        /* [in] */ Integer dayOfMonth,
        /* [in] */ Integer dayOfWeek,
        /* [in] */ Integer timeOfDay);

    void InvalidateCache();

    ECode DecodeRules();

    ECode DecodeStartRule();

    ECode DecodeEndRule();

    static const AutoPtr<IGregorian> GetGcal();

private:
    /**
     * The month in which daylight saving time starts.  This value must be
     * between <code>Calendar.JANUARY</code> and
     * <code>Calendar.DECEMBER</code> inclusive.  This value must not equal
     * <code>endMonth</code>.
     * <p>If <code>useDaylight</code> is false, this value is ignored.
     */
    Integer mStartMonth;

    /**
     * This field has two possible interpretations:
     * <dl>
     * <dt><code>startMode == DOW_IN_MONTH</code></dt>
     * <dd>
     * <code>startDay</code> indicates the day of the month of
     * <code>startMonth</code> on which daylight
     * saving time starts, from 1 to 28, 30, or 31, depending on the
     * <code>startMonth</code>.
     * </dd>
     * <dt><code>startMode != DOW_IN_MONTH</code></dt>
     * <dd>
     * <code>startDay</code> indicates which <code>startDayOfWeek</code> in the
     * month <code>startMonth</code> daylight
     * saving time starts on.  For example, a value of +1 and a
     * <code>startDayOfWeek</code> of <code>Calendar.SUNDAY</code> indicates the
     * first Sunday of <code>startMonth</code>.  Likewise, +2 would indicate the
     * second Sunday, and -1 the last Sunday.  A value of 0 is illegal.
     * </dd>
     * </dl>
     * <p>If <code>useDaylight</code> is false, this value is ignored.
     */
    Integer mStartDay;

    /**
     * The day of the week on which daylight saving time starts.  This value
     * must be between <code>Calendar.SUNDAY</code> and
     * <code>Calendar.SATURDAY</code> inclusive.
     * <p>If <code>useDaylight</code> is false or
     * <code>startMode == DAY_OF_MONTH</code>, this value is ignored.
     */
    Integer mStartDayOfWeek;

    /**
     * The time in milliseconds after midnight at which daylight saving
     * time starts.  This value is expressed as wall time, standard time,
     * or UTC time, depending on the setting of <code>startTimeMode</code>.
     * <p>If <code>useDaylight</code> is false, this value is ignored.
     */
    Integer mStartTime;

    /**
     * The format of startTime, either WALL_TIME, STANDARD_TIME, or UTC_TIME.
     */
    Integer mStartTimeMode;

    /**
     * The month in which daylight saving time ends.  This value must be
     * between <code>Calendar.JANUARY</code> and
     * <code>Calendar.UNDECIMBER</code>.  This value must not equal
     * <code>startMonth</code>.
     * <p>If <code>useDaylight</code> is false, this value is ignored.
     */
    Integer mEndMonth;

    /**
     * This field has two possible interpretations:
     * <dl>
     * <dt><code>endMode == DOW_IN_MONTH</code></dt>
     * <dd>
     * <code>endDay</code> indicates the day of the month of
     * <code>endMonth</code> on which daylight
     * saving time ends, from 1 to 28, 30, or 31, depending on the
     * <code>endMonth</code>.
     * </dd>
     * <dt><code>endMode != DOW_IN_MONTH</code></dt>
     * <dd>
     * <code>endDay</code> indicates which <code>endDayOfWeek</code> in th
     * month <code>endMonth</code> daylight
     * saving time ends on.  For example, a value of +1 and a
     * <code>endDayOfWeek</code> of <code>Calendar.SUNDAY</code> indicates the
     * first Sunday of <code>endMonth</code>.  Likewise, +2 would indicate the
     * second Sunday, and -1 the last Sunday.  A value of 0 is illegal.
     * </dd>
     * </dl>
     * <p>If <code>useDaylight</code> is false, this value is ignored.
     */
    Integer mEndDay;

    /**
     * The day of the week on which daylight saving time ends.  This value
     * must be between <code>Calendar.SUNDAY</code> and
     * <code>Calendar.SATURDAY</code> inclusive.
     * <p>If <code>useDaylight</code> is false or
     * <code>endMode == DAY_OF_MONTH</code>, this value is ignored.
     */
    Integer mEndDayOfWeek;

    /**
     * The time in milliseconds after midnight at which daylight saving
     * time ends.  This value is expressed as wall time, standard time,
     * or UTC time, depending on the setting of <code>endTimeMode</code>.
     * <p>If <code>useDaylight</code> is false, this value is ignored.
     */
    Integer mEndTime;

    /**
     * The format of endTime, either <code>WALL_TIME</code>,
     * <code>STANDARD_TIME</code>, or <code>UTC_TIME</code>.
     */
    Integer mEndTimeMode;

    /**
     * The year in which daylight saving time is first observed.  This is an {@link GregorianCalendar#AD AD}
     * value.  If this value is less than 1 then daylight saving time is observed
     * for all <code>AD</code> years.
     * <p>If <code>useDaylight</code> is false, this value is ignored.
     */
    Integer mStartYear;

    /**
     * The offset in milliseconds between this zone and GMT.  Negative offsets
     * are to the west of Greenwich.  To obtain local <em>standard</em> time,
     * add the offset to GMT time.  To obtain local wall time it may also be
     * necessary to add <code>dstSavings</code>.
     */
    Integer mRawOffset;

    /**
     * A boolean value which is true if and only if this zone uses daylight
     * saving time.  If this value is false, several other fields are ignored.
     */
    Boolean mUseDaylight = false; // indicate if this time zone uses DST

    static constexpr Integer mMillisPerHour = 60 * 60 * 1000;
    static constexpr Integer mMillisPerDay = 24 * mMillisPerHour;

    static constexpr Byte sStaticMonthLength[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    static constexpr Byte sStaticLeapMonthLength[] = {31,29,31,30,31,30,31,31,30,31,30,31};

    /**
     * Variables specifying the mode of the start rule.  Takes the following
     * values:
     * <dl>
     * <dt><code>DOM_MODE</code></dt>
     * <dd>
     * Exact day of week; e.g., March 1.
     * </dd>
     * <dt><code>DOW_IN_MONTH_MODE</code></dt>
     * <dd>
     * Day of week in month; e.g., last Sunday in March.
     * </dd>
     * <dt><code>DOW_GE_DOM_MODE</code></dt>
     * <dd>
     * Day of week after day of month; e.g., Sunday on or after March 15.
     * </dd>
     * <dt><code>DOW_LE_DOM_MODE</code></dt>
     * <dd>
     * Day of week before day of month; e.g., Sunday on or before March 15.
     * </dd>
     * </dl>
     * The setting of this field affects the interpretation of the
     * <code>startDay</code> field.
     * <p>If <code>useDaylight</code> is false, this value is ignored.
     */
    Integer mStartMode;

    /**
     * Variables specifying the mode of the end rule.  Takes the following
     * values:
     * <dl>
     * <dt><code>DOM_MODE</code></dt>
     * <dd>
     * Exact day of week; e.g., March 1.
     * </dd>
     * <dt><code>DOW_IN_MONTH_MODE</code></dt>
     * <dd>
     * Day of week in month; e.g., last Sunday in March.
     * </dd>
     * <dt><code>DOW_GE_DOM_MODE</code></dt>
     * <dd>
     * Day of week after day of month; e.g., Sunday on or after March 15.
     * </dd>
     * <dt><code>DOW_LE_DOM_MODE</code></dt>
     * <dd>
     * Day of week before day of month; e.g., Sunday on or before March 15.
     * </dd>
     * </dl>
     * The setting of this field affects the interpretation of the
     * <code>endDay</code> field.
     * <p>If <code>useDaylight</code> is false, this value is ignored.
     */
    Integer mEndMode;

    /**
     * A positive value indicating the amount of time saved during DST in
     * milliseconds.
     * Typically one hour (3600000); sometimes 30 minutes (1800000).
     * <p>If <code>useDaylight</code> is false, this value is ignored.
     */
    Integer mDstSavings;

    /**
     * Cache values representing a single period of daylight saving
     * time. When the cache values are valid, cacheStart is the start
     * time (inclusive) of daylight saving time and cacheEnd is the
     * end time (exclusive).
     *
     * cacheYear has a year value if both cacheStart and cacheEnd are
     * in the same year. cacheYear is set to startYear - 1 if
     * cacheStart and cacheEnd are in different years. cacheStart is 0
     * if the cache values are void. cacheYear is a long to support
     * Integer.MIN_VALUE - 1 (JCK requirement).
     */
    Long mCacheYear;
    Long mCacheStart;
    Long mCacheEnd;

    /**
     * Constants specifying values of startMode and endMode.
     */
    static constexpr Integer DOM_MODE = 1; // Exact day of month, "Mar 1"
    static constexpr Integer DOW_IN_MONTH_MODE = 2; // Day of week in month, "lastSun"
    static constexpr Integer DOW_GE_DOM_MODE = 3; // Day of week after day of month, "Sun>=15"
    static constexpr Integer DOW_LE_DOM_MODE = 4; // Day of week before day of month, "Sun<=21"

};

}
}

#endif // __CCM_UTIL_SIMPLETIMEZONE_H__
