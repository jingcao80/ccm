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

#ifndef __CCM_TEXT_CALENDARBUILDER_H__
#define __CCM_TEXT_CALENDARBUILDER_H__

#include "ccm/core/SyncObject.h"
#include "ccm.util.ICalendar.h"

using ccm::core::SyncObject;
using ccm::util::ICalendar;

namespace ccm {
namespace text {

class CalendarBuilder
    : public SyncObject
{
public:
    CalendarBuilder();

    void Set(
        /* [in] */ Integer index,
        /* [in] */ Integer value);

    void AddYear(
        /* [in] */ Integer value);

    Boolean IsSet(
        /* [in] */ Integer index);

    void Clear(
        /* [in] */ Integer index);

    void Establish(
        /* [in] */ ICalendar* cal);

    static Integer ToISODayOfWeek(
        /* [in] */ Integer calendarDayOfWeek);

    static Integer ToCalendarDayOfWeek(
        /* [in] */ Integer isoDayOfWeek);

    static Boolean IsValidDayOfWeek(
        /* [in] */ Integer dayOfWeek);

public:
    static constexpr Integer WEEK_YEAR = ICalendar::FIELD_COUNT;
    static constexpr Integer ISO_DAY_OF_WEEK = 1000; // pseudo field index

private:
    static constexpr Integer UNSET = 0;
    static constexpr Integer COMPUTED = 1;
    static constexpr Integer MINIMUM_USER_STAMP = 2;

    static constexpr Integer MAX_FIELD = ICalendar::FIELD_COUNT + 1;

    Array<Integer> mField;
    Integer mNextStamp;
    Integer mMaxFieldIndex;
};

inline CalendarBuilder::CalendarBuilder()
    : mField(MAX_FIELD * 2)
    , mNextStamp(MINIMUM_USER_STAMP)
    , mMaxFieldIndex(-1)
{}

inline void CalendarBuilder::AddYear(
    /* [in] */ Integer value)
{
    mField[MAX_FIELD + ICalendar::YEAR] += value;
    mField[MAX_FIELD + WEEK_YEAR] += value;
}

inline Boolean CalendarBuilder::IsSet(
    /* [in] */ Integer index)
{
    if (index == ISO_DAY_OF_WEEK) {
        index = ICalendar::DAY_OF_WEEK;
    }
    return mField[index] > UNSET;
}

inline void CalendarBuilder::Clear(
    /* [in] */ Integer index)
{
    if (index == ISO_DAY_OF_WEEK) {
        index = ICalendar::DAY_OF_WEEK;
    }
    mField[index] = UNSET;
    mField[MAX_FIELD + index] = 0;
}

inline Integer CalendarBuilder::ToISODayOfWeek(
    /* [in] */ Integer calendarDayOfWeek)
{
    return calendarDayOfWeek == ICalendar::SUNDAY ? 7 : calendarDayOfWeek -1 ;
}

inline Boolean CalendarBuilder::IsValidDayOfWeek(
    /* [in] */ Integer dayOfWeek)
{
    return dayOfWeek > 0 && dayOfWeek <= 7;
}

}
}

#endif // __CCM_TEXT_CALENDARBUILDER_H__
