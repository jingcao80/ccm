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

#include "ccm/util/SimpleTimeZone.h"
#include <ccmlogger.h>

namespace ccm {
namespace util {

CCM_INTERFACE_IMPL_1(SimpleTimeZone, TimeZone, ISimpleTimeZone);

ECode SimpleTimeZone::Constructor(
    /* [in] */ Integer rawOffset,
    /* [in] */ const String& id)
{
    mRawOffset = rawOffset;
    FAIL_RETURN(SetID(id));
    mDstSavings = mMillisPerHour;
    return NOERROR;
}

ECode SimpleTimeZone::Constructor(
    /* [in] */ Integer rawOffset,
    /* [in] */ const String& id,
    /* [in] */ Integer startMonth,
    /* [in] */ Integer startDay,
    /* [in] */ Integer startDayOfWeek,
    /* [in] */ Integer startTime,
    /* [in] */ Integer endMonth,
    /* [in] */ Integer endDay,
    /* [in] */ Integer endDayOfWeek,
    /* [in] */ Integer endTime)
{
    return Constructor(rawOffset, id, startMonth, startDay,
            startDayOfWeek, startTime, WALL_TIME, endMonth,
            endDay, endDayOfWeek, endTime, WALL_TIME, mMillisPerHour);
}

ECode SimpleTimeZone::Constructor(
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
    /* [in] */ Integer dstSavings)
{
    return Constructor(rawOffset, id, startMonth, startDay,
            startDayOfWeek, startTime, WALL_TIME, endMonth,
            endDay, endDayOfWeek, endTime, WALL_TIME, dstSavings);
}

ECode SimpleTimeZone::Constructor(
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
    /* [in] */ Integer dstSavings)
{
    FAIL_RETURN(SetID(id));
    mRawOffset = rawOffset;
    mStartMonth = startMonth;
    mStartDay = startDay;
    mStartDayOfWeek = startDayOfWeek;
    mStartTime = startTime;
    mStartTimeMode = startTimeMode;
    mEndMonth = endMonth;
    mEndDay = endDay;
    mEndDayOfWeek = endDayOfWeek;
    mEndTime = endTime;
    mEndTimeMode = endTimeMode;
    mDstSavings = dstSavings;

    FAIL_RETURN(DecodeRules());
    if (dstSavings <= 0) {
        Logger::E("SimpleTimeZone", "Illegal daylight saving value: %d", dstSavings);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode SimpleTimeZone::SetStartYear(
    /* [in] */ Integer year)
{
    mStartYear = year;
    InvalidateCache();
    return NOERROR;
}

ECode SimpleTimeZone::SetStartRule(
    /* [in] */ Integer startMonth,
    /* [in] */ Integer startDay,
    /* [in] */ Integer startDayOfWeek,
    /* [in] */ Integer startTime)
{
    mStartMonth = startMonth;
    mStartDay = startDay;
    mStartDayOfWeek = startDayOfWeek;
    mStartTime = startTime;
    mStartTimeMode = WALL_TIME;
    FAIL_RETURN(DecodeStartRule());
    InvalidateCache();
    return NOERROR;
}

ECode SimpleTimeZone::SetStartRule(
    /* [in] */ Integer startMonth,
    /* [in] */ Integer startDay,
    /* [in] */ Integer startTime)
{
    return SetStartRule(startMonth, startDay, 0, startTime);
}

ECode SimpleTimeZone::SetStartRule(
    /* [in] */ Integer startMonth,
    /* [in] */ Integer startDay,
    /* [in] */ Integer startDayOfWeek,
    /* [in] */ Integer startTime,
    /* [in] */ Boolean after)
{
    if (after) {
        return SetStartRule(startMonth, startDay, -startDayOfWeek, startTime);
    }
    else {
        return SetStartRule(startMonth, -startDay, -startDayOfWeek, startTime);
    }
}

ECode SimpleTimeZone::SetEndRule(
    /* [in] */ Integer endMonth,
    /* [in] */ Integer endDay,
    /* [in] */ Integer endDayOfWeek,
    /* [in] */ Integer endTime)
{
    mEndMonth = endMonth;
    mEndDay = endDay;
    mEndDayOfWeek = endDayOfWeek;
    mEndTime = endTime;
    mEndTimeMode = WALL_TIME;
    FAIL_RETURN(DecodeEndRule());
    InvalidateCache();
    return NOERROR;
}

}
}
