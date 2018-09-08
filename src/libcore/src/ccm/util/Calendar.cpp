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

#include "ccm/core/CoreUtils.h"
#include "ccm/core/Math.h"
#include "ccm/text/DateFormatSymbols.h"
#include "ccm/util/Calendar.h"
#include "ccm/util/CHashMap.h"
#include "ccm/util/CLocale.h"
#include "ccm/util/CDate.h"
#include "ccm/util/TimeZone.h"
#include "ccm/util/locale/provider/CalendarDataUtility.h"
#include "ccm.core.IInteger.h"
#include <ccmautoptr.h>

using ccm::core::CoreUtils;
using ccm::core::IID_ICloneable;
using ccm::core::IID_IComparable;
using ccm::core::IInteger;
using ccm::core::Math;
using ccm::io::IID_ISerializable;
using ccm::text::DateFormatSymbols;
using ccm::util::CHashMap;
using ccm::util::IID_IMap;
using ccm::util::locale::provider::CalendarDataUtility;

namespace ccm {
namespace util {

CCM_INTERFACE_IMPL_4(Calendar, SyncObject, ICalendar, ISerializable, ICloneable, IComparable);

ECode Calendar::Constructor()
{
    Constructor(TimeZone::GetDefaultRef(),
            CLocale::GetDefault(CLocale::Category::GetFORMAT()));
    mSharedZone = true;
    return NOERROR;
}

ECode Calendar::Constructor(
    /* [in] */ ITimeZone* zone,
    /* [in] */ ILocale* locale)
{
    AutoPtr<ILocale> aLocale = locale;
    if (aLocale == nullptr) {
        aLocale = CLocale::GetDefault();
    }
    mFields = Array<Integer>(FIELD_COUNT);
    mIsSet = Array<Boolean>(FIELD_COUNT);
    mStamp = Array<Integer>(FIELD_COUNT);

    mZone = zone;
    SetWeekCountData(aLocale);
    return NOERROR;
}

AutoPtr<ICalendar> Calendar::GetInstance()
{
    return CreateCalendar(TimeZone::GetDefault(),
            CLocale::GetDefault(CLocale::Category::GetFORMAT()));
}

AutoPtr<ICalendar> Calendar::GetInstance(
    /* [in] */ ITimeZone* zone)
{
    return CreateCalendar(zone,
            CLocale::GetDefault(CLocale::Category::GetFORMAT()));
}

AutoPtr<ICalendar> Calendar::GetInstance(
    /* [in] */ ILocale* aLocale)
{
    return CreateCalendar(TimeZone::GetDefault(), aLocale);
}

AutoPtr<ICalendar> Calendar::GetInstance(
    /* [in] */ ITimeZone* zone,
    /* [in] */ ILocale* aLocale)
{
    return CreateCalendar(zone, aLocale);
}

AutoPtr<ICalendar> Calendar::GetJapaneseImperialInstance(
    /* [in] */ ITimeZone* zone,
    /* [in] */ ILocale* aLocale)
{
    return nullptr;
}

AutoPtr<ICalendar> Calendar::CreateCalendar(
    /* [in] */ ITimeZone* zone,
    /* [in] */ ILocale* aLocale)
{
    return nullptr;
}

Array<ILocale*> Calendar::GetAvailableLocales()
{
    return Array<ILocale*>::Null();
}

ECode Calendar::GetTime(
    /* [out] */ IDate** date)
{
    VALIDATE_NOT_NULL(date);

    Long time;
    GetTimeInMillis(&time);
    return CDate::New(time, IID_IDate, (IInterface**)date);
}

ECode Calendar::SetTime(
    /* [in] */ IDate* date)
{
    Long time;
    date->GetTime(&time);
    return SetTimeInMillis(time);
}

ECode Calendar::GetTimeInMillis(
    /* [out] */ Long* time)
{
    VALIDATE_NOT_NULL(time);

    if (!mIsTimeSet) {
        UpdateTime();
    }
    *time = mTime;
    return NOERROR;
}

ECode Calendar::SetTimeInMillis(
    /* [in] */ Long millis)
{
    if (mTime = millis && mIsTimeSet && mAreFieldsSet && mAreAllFieldsSet) {
        return NOERROR;
    }
    mTime = millis;
    mIsTimeSet = true;
    mAreFieldsSet = false;
    ComputeFields();
    mAreAllFieldsSet = mAreFieldsSet = true;
    return NOERROR;
}

ECode Calendar::Get(
    /* [in] */ Integer field,
    /* [out] */ Integer* value)
{
    VALIDATE_NOT_NULL(value);

    if (field < 0 || field >= FIELD_COUNT) {
        return ccm::core::E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    Complete();
    *value = InternalGet(field);
    return NOERROR;
}

ECode Calendar::Set(
    /* [in] */ Integer field,
    /* [in] */ Integer value)
{
    if (field < 0 || field >= FIELD_COUNT) {
        return ccm::core::E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    if (mAreFieldsSet && ! mAreAllFieldsSet) {
        ComputeFields();
    }
    InternalSet(field, value);
    mIsTimeSet = false;
    mAreFieldsSet = false;
    mIsSet[field] = true;
    mStamp[field] = mNextStamp++;
    if (mNextStamp == IInteger::MAX_VALUE) {
        AdjustStamp();
    }
    return NOERROR;
}

ECode Calendar::Set(
    /* [in] */ Integer year,
    /* [in] */ Integer month,
    /* [in] */ Integer date)
{
    Set(YEAR, year);
    Set(MONTH, month);
    Set(DATE, date);
    return NOERROR;
}

ECode Calendar::Set(
    /* [in] */ Integer year,
    /* [in] */ Integer month,
    /* [in] */ Integer date,
    /* [in] */ Integer hourOfDay,
    /* [in] */ Integer minute)
{
    Set(YEAR, year);
    Set(MONTH, month);
    Set(DATE, date);
    Set(HOUR_OF_DAY, hourOfDay);
    Set(MINUTE, minute);
    return NOERROR;
}

ECode Calendar::Set(
    /* [in] */ Integer year,
    /* [in] */ Integer month,
    /* [in] */ Integer date,
    /* [in] */ Integer hourOfDay,
    /* [in] */ Integer minute,
    /* [in] */ Integer second)
{
    Set(YEAR, year);
    Set(MONTH, month);
    Set(DATE, date);
    Set(HOUR_OF_DAY, hourOfDay);
    Set(MINUTE, minute);
    Set(SECOND, second);
    return NOERROR;
}

ECode Calendar::Clear()
{
    for (Integer i = 0; i < mFields.GetLength(); ) {
        mStamp[i] = mFields[i] = 0;
        mIsSet[i++] = false;
    }
    mAreAllFieldsSet = mAreFieldsSet = false;
    mIsTimeSet = false;
    return NOERROR;
}

ECode Calendar::Clear(
    /* [in] */ Integer field)
{
    if (field < 0 || field >= FIELD_COUNT) {
        return ccm::core::E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    mFields[field] = 0;
    mStamp[field] = UNSET;
    mIsSet[field] = false;

    mAreAllFieldsSet = mAreFieldsSet = false;
    mIsTimeSet = false;
    return NOERROR;
}

ECode Calendar::IsSet(
    /* [in] */ Integer field,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (field < 0 || field >= FIELD_COUNT) {
        return ccm::core::E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    *result = mStamp[field] != UNSET;
    return NOERROR;
}

ECode Calendar::GetDisplayName(
    /* [in] */ Integer field,
    /* [in] */ Integer style,
    /* [in] */ ILocale* locale,
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    if (style = ALL_STYLES) {
        style = SHORT;
    }
    Boolean result;
    FAIL_RETURN(CheckDisplayNameParams(field, style, SHORT, NARROW_FORMAT, locale,
            ERA_MASK | MONTH_MASK | DAY_OF_WEEK_MASK | AM_PM_MASK, &result));
    if (!result) {
        *name = nullptr;
        return NOERROR;
    }

    String calendarType;
    GetCalendarType(&calendarType);
    Integer fieldValue;
    Get(field, &fieldValue);
    // the standalone and narrow styles are supported only through CalendarDataProviders.
    if (IsStandaloneStyle(style) || IsNarrowFormatStyle(style)) {
        String val;
        FAIL_RETURN(CalendarDataUtility::RetrieveFieldValueName(calendarType,
                field, fieldValue, style, locale, &val));

        // Perform fallback here to follow the CLDR rules
        if (val.IsNull()) {
            if (IsNarrowFormatStyle(style)) {
                FAIL_RETURN(CalendarDataUtility::RetrieveFieldValueName(calendarType,
                        field, fieldValue, ToStandaloneStyle(style), locale, &val));
            }
            else if (IsStandaloneStyle(style)) {
                FAIL_RETURN(CalendarDataUtility::RetrieveFieldValueName(calendarType,
                        field, fieldValue, GetBaseStyle(style), locale, &val));
            }
        }
        *name = val;
        return NOERROR;
    }

    AutoPtr<IDateFormatSymbols> symbols = DateFormatSymbols::GetInstance(locale);
    Array<String> strings = GetFieldStrings(field, style, symbols);
    if (!strings.IsNull()) {
        if (fieldValue < strings.GetLength()) {
            *name = strings[fieldValue];
            return NOERROR;
        }
    }
    *name = nullptr;
    return NOERROR;
}

ECode Calendar::GetDisplayNames(
    /* [in] */ Integer field,
    /* [in] */ Integer style,
    /* [in] */ ILocale* locale,
    /* [out] */ IMap** names)
{
    VALIDATE_NOT_NULL(names);

    Boolean result;
    FAIL_RETURN(CheckDisplayNameParams(field, style, ALL_STYLES, NARROW_FORMAT, locale,
            ERA_MASK | MONTH_MASK | DAY_OF_WEEK_MASK | AM_PM_MASK, &result));
    if (!result) {
        *names = nullptr;
        return NOERROR;
    }

    Complete();

    String calendarType;
    GetCalendarType(&calendarType);
    if (style == ALL_STYLES || IsStandaloneStyle(style) || IsNarrowFormatStyle(style)) {
        AutoPtr<IMap> map;
        FAIL_RETURN(CalendarDataUtility::RetrieveFieldValueNames(calendarType, field, style, locale, &map));

        if (map == nullptr) {
            if (IsNarrowFormatStyle(style)) {
                FAIL_RETURN(CalendarDataUtility::RetrieveFieldValueNames(calendarType, field,
                        ToStandaloneStyle(style), locale, &map));
            }
            else if (style != ALL_STYLES) {
                FAIL_RETURN(CalendarDataUtility::RetrieveFieldValueNames(calendarType, field,
                        GetBaseStyle(style), locale, &map));
            }
        }
        *names = map;
        REFCOUNT_ADD(*names);
        return NOERROR;
    }

    // SHORT or LONG
    AutoPtr<IMap> map = GetDisplayNamesImpl(field, style, locale);
    *names = map;
    REFCOUNT_ADD(*names);
    return NOERROR;
}

AutoPtr<IMap> Calendar::GetDisplayNamesImpl(
    /* [in] */ Integer field,
    /* [in] */ Integer style,
    /* [in] */ ILocale* locale)
{
    AutoPtr<IDateFormatSymbols> symbols = DateFormatSymbols::GetInstance(locale);
    Array<String> strings = GetFieldStrings(field, style, symbols);
    if (!strings.IsNull()) {
        AutoPtr<IMap> names;
        CHashMap::New(IID_IMap, (IInterface**)&names);
        for (Integer i = 0; i < strings.GetLength(); i++) {
            if (strings[i].GetLength() == 0) {
                continue;
            }
            names->Put(CoreUtils::Box(strings[i]), CoreUtils::Box(i));
        }
        return names;
    }
    return nullptr;
}

ECode Calendar::CheckDisplayNameParams(
    /* [in] */ Integer field,
    /* [in] */ Integer style,
    /* [in] */ Integer minStyle,
    /* [in] */ Integer maxStyle,
    /* [in] */ ILocale* locale,
    /* [in] */ Integer fieldMask,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Integer baseStyle = GetBaseStyle(style); // Ignore the standalone mask
    if (field < 0 || field > mFields.GetLength() ||
            baseStyle < minStyle || baseStyle > maxStyle) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // 3 is not a valid base style (unlike 1, 2 and 4). Throw if used.
    if (baseStyle == 3) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (locale == nullptr) {
        return ccm::core::E_NULL_POINTER_EXCEPTION;
    }
    *result = IsFieldSet(fieldMask, field);
    return NOERROR;
}

Array<String> Calendar::GetFieldStrings(
    /* [in] */ Integer field,
    /* [in] */ Integer style,
    /* [in] */ IDateFormatSymbols* symbols)
{
    Integer baseStyle = GetBaseStyle(style); // ignore the standalone mask

    // DateFormatSymbols doesn't support any narrow names.
    if (baseStyle == NARROW_FORMAT) {
        return Array<String>::Null();
    }

    Array<String> strings;
    switch (field) {
        case ERA:
            symbols->GetEras(&strings);
            break;

        case MONTH:
            baseStyle == LONG ? symbols->GetMonths(&strings) :
                                symbols->GetShortMonths(&strings);
            break;

        case DAY_OF_WEEK:
            baseStyle == LONG ? symbols->GetWeekdays(&strings) :
                                symbols->GetShortWeekdays(&strings);
            break;

        case AM_PM:
            symbols->GetAmPmStrings(&strings);
            break;
    }
    return strings;
}

ECode Calendar::Complete()
{
    if (!mIsTimeSet) {
        UpdateTime();
    }
    if (!mAreFieldsSet || !mAreAllFieldsSet) {
        ComputeFields(); // fills in unset fields
        mAreAllFieldsSet = mAreFieldsSet = true;
    }
    return NOERROR;
}

Integer Calendar::GetSetStateFields()
{
    Integer mask = 0;
    for (Integer i = 0; i < mFields.GetLength(); i++) {
        if (mStamp[i] != UNSET) {
            mask |= 1 << i;
        }
    }
    return mask;
}

void Calendar::SetFieldsComputed(
    /* [in] */ Integer fieldMask)
{
    if (fieldMask == ALL_FIELDS) {
        for (Integer i = 0; i < mFields.GetLength(); i++) {
            mStamp[i] = COMPUTED;
            mIsSet[i] = true;
        }
        mAreFieldsSet = mAreAllFieldsSet = true;
    }
    else {
        for (Integer i = 0; i < mFields.GetLength(); i++) {
            if ((fieldMask & 1) == 1) {
                mStamp[i] = COMPUTED;
                mIsSet[i] = true;
            }
            else {
                if (mAreAllFieldsSet && !mIsSet[i]) {
                    mAreAllFieldsSet = false;
                }
            }
            fieldMask = (unsigned Integer)fieldMask >> 1;
        }
    }
}

void Calendar::SetFieldsNormalized(
    /* [in] */ Integer fieldMask)
{
    if (fieldMask != ALL_FIELDS) {
        for (Integer i = 0; i < mFields.GetLength(); i++) {
            if ((fieldMask & 1) == 0) {
                mStamp[i] = mFields[i] = 0; // UNSET == 0
                mIsSet[i] = false;
            }
            fieldMask >>= 1;
        }
    }

    // Some or all of the fields are in sync with the
    // milliseconds, but the stamp values are not normalized yet.
    mAreFieldsSet = true;
    mAreAllFieldsSet = false;
}

Integer Calendar::SelectFields()
{
    // This implementation has been taken from the GregorianCalendar class.

    // The YEAR field must always be used regardless of its SET
    // state because YEAR is a mandatory field to determine the date
    // and the default value (EPOCH_YEAR) may change through the
    // normalization process.
    Integer fieldMask = YEAR_MASK;

    if (mStamp[ERA] != UNSET) {
        fieldMask |= ERA_MASK;
    }
    // Find the most recent group of fields specifying the day within
    // the year.  These may be any of the following combinations:
    //   MONTH + DAY_OF_MONTH
    //   MONTH + WEEK_OF_MONTH + DAY_OF_WEEK
    //   MONTH + DAY_OF_WEEK_IN_MONTH + DAY_OF_WEEK
    //   DAY_OF_YEAR
    //   WEEK_OF_YEAR + DAY_OF_WEEK
    // We look for the most recent of the fields in each group to determine
    // the age of the group.  For groups involving a week-related field such
    // as WEEK_OF_MONTH, DAY_OF_WEEK_IN_MONTH, or WEEK_OF_YEAR, both the
    // week-related field and the DAY_OF_WEEK must be set for the group as a
    // whole to be considered.  (See bug 4153860 - liu 7/24/98.)
    Integer dowStamp = mStamp[DAY_OF_WEEK];
    Integer monthStamp = mStamp[MONTH];
    Integer domStamp = mStamp[DAY_OF_MONTH];
    Integer womStamp = AggregateStamp(mStamp[WEEK_OF_MONTH], dowStamp);
    Integer dowimStamp = AggregateStamp(mStamp[DAY_OF_WEEK_IN_MONTH], dowStamp);
    Integer doyStamp = mStamp[DAY_OF_YEAR];
    Integer woyStamp = AggregateStamp(mStamp[WEEK_OF_YEAR], dowStamp);

    Integer bestStamp = domStamp;
    if (womStamp > bestStamp) {
        bestStamp = womStamp;
    }
    if (dowimStamp > bestStamp) {
        bestStamp = dowimStamp;
    }
    if (doyStamp > bestStamp) {
        bestStamp = doyStamp;
    }
    if (woyStamp > bestStamp) {
        bestStamp = woyStamp;
    }

    /* No complete combination exists.  Look for WEEK_OF_MONTH,
     * DAY_OF_WEEK_IN_MONTH, or WEEK_OF_YEAR alone.  Treat DAY_OF_WEEK alone
     * as DAY_OF_WEEK_IN_MONTH.
     */
    if (bestStamp == UNSET) {
        womStamp = mStamp[WEEK_OF_MONTH];
        dowimStamp = Math::Max(mStamp[DAY_OF_WEEK_IN_MONTH], dowStamp);
        woyStamp = mStamp[WEEK_OF_YEAR];
        bestStamp = Math::Max(Math::Max(womStamp, dowimStamp), woyStamp);

        /* Treat MONTH alone or no fields at all as DAY_OF_MONTH.  This may
         * result in bestStamp = domStamp = UNSET if no fields are set,
         * which indicates DAY_OF_MONTH.
         */
        if (bestStamp == UNSET) {
            bestStamp = domStamp = monthStamp;
        }
    }

    if (bestStamp == domStamp ||
       (bestStamp == womStamp && mStamp[WEEK_OF_MONTH] >= mStamp[WEEK_OF_YEAR]) ||
       (bestStamp == dowimStamp && mStamp[DAY_OF_WEEK_IN_MONTH] >= mStamp[WEEK_OF_YEAR])) {
        fieldMask |= MONTH_MASK;
        if (bestStamp == domStamp) {
            fieldMask |= DAY_OF_MONTH_MASK;
        }
        else {
            CHECK(bestStamp == womStamp || bestStamp == dowimStamp);
            if (dowStamp != UNSET) {
                fieldMask |= DAY_OF_WEEK_MASK;
            }
            if (womStamp == dowimStamp) {
                // When they are equal, give the priority to
                // WEEK_OF_MONTH for compatibility.
                if (mStamp[WEEK_OF_MONTH] >= mStamp[DAY_OF_WEEK_IN_MONTH]) {
                    fieldMask |= WEEK_OF_MONTH_MASK;
                }
                else {
                    fieldMask |= DAY_OF_WEEK_IN_MONTH_MASK;
                }
            }
            else {
                if (bestStamp == womStamp) {
                    fieldMask |= WEEK_OF_MONTH_MASK;
                }
                else {
                    CHECK(bestStamp == dowimStamp);
                    if (mStamp[DAY_OF_WEEK_IN_MONTH] != UNSET) {
                        fieldMask |= DAY_OF_WEEK_IN_MONTH_MASK;
                    }
                }
            }
        }
    }
    else {
        CHECK(bestStamp == doyStamp || bestStamp == woyStamp ||
                bestStamp == UNSET);
        if (bestStamp == doyStamp) {
            fieldMask |= DAY_OF_YEAR_MASK;
        }
        else {
            CHECK(bestStamp == woyStamp);
            if (dowStamp != UNSET) {
                fieldMask |= DAY_OF_WEEK_MASK;
            }
            fieldMask |= WEEK_OF_YEAR_MASK;
        }
    }

    // Find the best set of fields specifying the time of day.  There
    // are only two possibilities here; the HOUR_OF_DAY or the
    // AM_PM and the HOUR.
    Integer hourOfDayStamp = mStamp[HOUR_OF_DAY];
    Integer hourStamp = AggregateStamp(mStamp[HOUR], mStamp[AM_PM]);
    bestStamp = (hourStamp > hourOfDayStamp) ? hourStamp : hourOfDayStamp;

    // if bestStamp is still UNSET, then take HOUR or AM_PM. (See 4846659)
    if (bestStamp == UNSET) {
        bestStamp = Math::Max(mStamp[HOUR], mStamp[AM_PM]);
    }

    // Hours
    if (bestStamp != UNSET) {
        if (bestStamp == hourOfDayStamp) {
            fieldMask |= HOUR_OF_DAY_MASK;
        }
        else {
            fieldMask |= HOUR_MASK;
            if (mStamp[AM_PM] != UNSET) {
                fieldMask |= AM_PM_MASK;
            }
        }
    }
    if (mStamp[MINUTE] != UNSET) {
        fieldMask |= MINUTE_MASK;
    }
    if (mStamp[SECOND] != UNSET) {
        fieldMask |= SECOND_MASK;
    }
    if (mStamp[MILLISECOND] != UNSET) {
        fieldMask |= MILLISECOND_MASK;
    }
    if (mStamp[ZONE_OFFSET] >= MINIMUM_USER_STAMP) {
            fieldMask |= ZONE_OFFSET_MASK;
    }
    if (mStamp[DST_OFFSET] >= MINIMUM_USER_STAMP) {
        fieldMask |= DST_OFFSET_MASK;
    }

    return fieldMask;
}

}
}
