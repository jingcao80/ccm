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
        /* [in] */ Integer year);

    ECode SetStartRule(
        /* [in] */ Integer startMonth,
        /* [in] */ Integer startDay,
        /* [in] */ Integer startDayOfWeek,
        /* [in] */ Integer startTime);

    ECode SetStartRule(
        /* [in] */ Integer startMonth,
        /* [in] */ Integer startDay,
        /* [in] */ Integer startTime);

    ECode SetStartRule(
        /* [in] */ Integer startMonth,
        /* [in] */ Integer startDay,
        /* [in] */ Integer startDayOfWeek,
        /* [in] */ Integer startTime,
        /* [in] */ Boolean after);

    ECode SetEndRule(
        /* [in] */ Integer endMonth,
        /* [in] */ Integer endDay,
        /* [in] */ Integer endDayOfWeek,
        /* [in] */ Integer endTime);

private:
    void InvalidateCache()
    {}

    ECode DecodeRules()
    {
        return NOERROR;
    }

    ECode DecodeStartRule()
    {
        return NOERROR;
    }

    ECode DecodeEndRule()
    {
        return NOERROR;
    }

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

    static constexpr Integer mMillisPerHour = 60 * 60 * 1000;
    static constexpr Integer mMillisPerDay = 24 * mMillisPerHour;

    /**
     * A positive value indicating the amount of time saved during DST in
     * milliseconds.
     * Typically one hour (3600000); sometimes 30 minutes (1800000).
     * <p>If <code>useDaylight</code> is false, this value is ignored.
     */
    Integer mDstSavings;
};

}
}

#endif // __CCM_UTIL_SIMPLETIMEZONE_H__
