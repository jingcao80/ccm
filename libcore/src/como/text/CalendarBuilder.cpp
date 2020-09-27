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

#include "como/text/CalendarBuilder.h"

namespace como {
namespace text {

void CalendarBuilder::Set(
    /* [in] */ Integer index,
    /* [in] */ Integer value)
{
    if (index == ISO_DAY_OF_WEEK) {
        index = ICalendar::DAY_OF_WEEK;
        value = ToCalendarDayOfWeek(value);
    }
    mField[index] = mNextStamp++;
    mField[MAX_FIELD + index] = value;
    if (index > mMaxFieldIndex && index < ICalendar::FIELD_COUNT) {
        mMaxFieldIndex = index;
    }
}

void CalendarBuilder::Establish(
    /* [in] */ ICalendar* cal)
{
    Boolean weekDate = IsSet(WEEK_YEAR) &&
            mField[WEEK_YEAR] > mField[ICalendar::YEAR];
    Boolean supported;
    if (weekDate && (cal->IsWeekDateSupported(supported), !supported)) {
        // Use YEAR instead
        if (!IsSet(ICalendar::YEAR)) {
            Set(ICalendar::YEAR, mField[MAX_FIELD + WEEK_YEAR]);
        }
        weekDate = false;
    }

    cal->Clear();
    // Set the fields from the min stamp to the max stamp so that
    // the field resolution works in the Calendar.
    for (Integer stamp = MINIMUM_USER_STAMP; stamp < mNextStamp; stamp++) {
        for (Integer index = 0; index <= mMaxFieldIndex; index++) {
            if (mField[index] == stamp) {
                cal->Set(index, mField[MAX_FIELD + index]);
                break;
            }
        }
    }

    if (weekDate) {
        Integer weekOfYear;
        if (IsSet(ICalendar::WEEK_OF_YEAR)) {
            weekOfYear = mField[MAX_FIELD + ICalendar::WEEK_OF_YEAR];
        }
        else {
            weekOfYear = 1;
        }
        Integer dayOfWeek;
        if (IsSet(ICalendar::DAY_OF_WEEK)) {
            dayOfWeek = mField[MAX_FIELD + ICalendar::DAY_OF_WEEK];
        }
        else {
            cal->GetFirstDayOfWeek(dayOfWeek);
        }
        Boolean lenient;
        if (!IsValidDayOfWeek(dayOfWeek) && (cal->IsLenient(lenient), lenient)) {
            if (dayOfWeek >= 8) {
                dayOfWeek--;
                weekOfYear += dayOfWeek / 7;
                dayOfWeek = (dayOfWeek % 7) + 1;
            }
            else {
                while (dayOfWeek <= 0) {
                    dayOfWeek += 7;
                    weekOfYear--;
                }
            }
            dayOfWeek = ToCalendarDayOfWeek(dayOfWeek);
        }
        cal->SetWeekDate(mField[MAX_FIELD + WEEK_YEAR], weekOfYear, dayOfWeek);
    }
}

Integer CalendarBuilder::ToCalendarDayOfWeek(
    /* [in] */ Integer isoDayOfWeek)
{
    if (!IsValidDayOfWeek(isoDayOfWeek)) {
        // adjust later for lenient mode
        return isoDayOfWeek;
    }
    return isoDayOfWeek == 7 ? ICalendar::SUNDAY : isoDayOfWeek + 1;
}

}
}
