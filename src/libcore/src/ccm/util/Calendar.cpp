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

#include "ccm/core/CArray.h"
#include "ccm/core/CoreUtils.h"
#include "ccm/core/CStringBuilder.h"
#include "ccm/core/Math.h"
#include "ccm/text/DateFormatSymbols.h"
#include "ccm/util/Calendar.h"
#include "ccm/util/CGregorianCalendar.h"
#include "ccm/util/CHashMap.h"
#include "ccm/util/CLocale.h"
#include "ccm/util/CDate.h"
#include "ccm/util/JapaneseImperialCalendar.h"
#include "ccm/util/TimeZone.h"
#include "ccm/util/concurrent/CConcurrentHashMap.h"
#include "ccm/util/locale/provider/CalendarDataUtility.h"
#include "libcore/icu/LocaleData.h"
#include "ccm.core.IArray.h"
#include "ccm.core.IInteger.h"
#include <ccmautoptr.h>

using ccm::core::CArray;
using ccm::core::CoreUtils;
using ccm::core::CStringBuilder;
using ccm::core::IArray;
using ccm::core::IID_IArray;
using ccm::core::IID_ICloneable;
using ccm::core::IID_IComparable;
using ccm::core::IID_IInteger;
using ccm::core::IID_IStringBuilder;
using ccm::core::IInteger;
using ccm::core::Math;
using ccm::io::IID_ISerializable;
using ccm::text::DateFormatSymbols;
using ccm::util::CHashMap;
using ccm::util::IID_IMap;
using ccm::util::concurrent::CConcurrentHashMap;
using ccm::util::concurrent::IID_IConcurrentMap;
using ccm::util::locale::provider::CalendarDataUtility;
using libcore::icu::ILocaleData;
using libcore::icu::LocaleData;

namespace ccm {
namespace util {

Calendar::Calendar()
{
    CConcurrentHashMap::New(3, IID_IConcurrentMap, (IInterface**)&mCachedLocaleData);
}

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
    AutoPtr<JapaneseImperialCalendar> jCal = new JapaneseImperialCalendar();
    jCal->Constructor(zone, aLocale);
    return (ICalendar*)jCal.Get();
}

AutoPtr<ICalendar> Calendar::CreateCalendar(
    /* [in] */ ITimeZone* zone,
    /* [in] */ ILocale* aLocale)
{
    AutoPtr<ICalendar> cal;
    CGregorianCalendar::New(zone, aLocale, IID_ICalendar, (IInterface**)&cal);
    return cal;
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

ECode Calendar::GetCalendarType(
    /* [out] */ String* type)
{
    VALIDATE_NOT_NULL(type);

    *type = GetCoclassName((IObject*)this);
    return NOERROR;
}

ECode Calendar::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* same)
{
    VALIDATE_NOT_NULL(same);

    if (IInterface::Equals((IObject*)this, obj)) {
        *same = true;
        return NOERROR;
    }
    Calendar* that = (Calendar*)ICalendar::Probe(obj);
    if (that == nullptr) {
        *same = false;
        return NOERROR;
    }
    *same = CompareTo(GetMillisOf(that)) == 0 &&
            mLenient == that->mLenient &&
            mFirstDayOfWeek == that->mFirstDayOfWeek &&
            mMinimalDaysInFirstWeek == that->mMinimalDaysInFirstWeek &&
            Object::Equals(mZone, that->mZone);
    return NOERROR;
}

ECode Calendar::GetHashCode(
    /* [out] */ Integer* hash)
{
    VALIDATE_NOT_NULL(hash);

    Integer otheritems = (mLenient ? 1 : 0)
            | (mFirstDayOfWeek << 1)
            | (mMinimalDaysInFirstWeek << 4)
            | (Object::GetHashCode(mZone) << 7);
    Long t = GetMillisOf(this);
    *hash = (Integer) t ^ (Integer)(t >> 32) ^ otheritems;
    return NOERROR;
}

ECode Calendar::Before(
    /* [in] */ IInterface* when,
    /* [out] */ Boolean* before)
{
    VALIDATE_NOT_NULL(before);

    ICalendar* cal = ICalendar::Probe(when);
    if (cal == nullptr) {
        *before = false;
        return NOERROR;
    }
    Integer compare;
    CompareTo(cal, &compare);
    *before = compare < 0;
    return NOERROR;
}

ECode Calendar::After(
    /* [in] */ IInterface* when,
    /* [out] */ Boolean* after)
{
    VALIDATE_NOT_NULL(after);

    ICalendar* cal = ICalendar::Probe(when);
    if (cal == nullptr) {
        *after = false;
        return NOERROR;
    }
    Integer compare;
    CompareTo(cal, &compare);
    *after = compare > 0;
    return NOERROR;
}

ECode Calendar::CompareTo(
    /* [in] */ IInterface* another,
    /* [out] */ Integer* result)
{
    VALIDATE_NOT_NULL(result);

    if (ICalendar::Probe(another) == nullptr) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *result = CompareTo(GetMillisOf((Calendar*)ICalendar::Probe(another)));
    return NOERROR;
}

ECode Calendar::Roll(
    /* [in] */ Integer field,
    /* [in] */ Integer amount)
{
    while (amount > 0) {
        Roll(field, true);
        amount--;
    }
    while (amount < 0) {
        Roll(field, false);
        amount++;
    }
    return NOERROR;
}

ECode Calendar::SetTimeZone(
    /* [in] */ ITimeZone* value)
{
    mZone = value;
    mSharedZone = false;
    /* Recompute the fields from the time using the new zone.  This also
     * works if mIsTimeSet is false (after a call to Set()).  In that case
     * the time will be computed from the fields using the new zone, then
     * the fields will get recomputed from that.  Consider the sequence of
     * calls: cal->SetTimeZone(EST); cal->Set(HOUR, 1); cal->SetTimeZone(PST).
     * Is cal set to 1 o'clock EST or 1 o'clock PST?  Answer: PST.  More
     * generally, a call to SetTimeZone() affects calls to Set() BEFORE AND
     * AFTER it up to the next call to Complete().
     */
    mAreAllFieldsSet = mAreFieldsSet = false;
    return NOERROR;
}

ECode Calendar::GetTimeZone(
    /* [out] */ ITimeZone** zone)
{
    VALIDATE_NOT_NULL(zone);

    // If the TimeZone object is shared by other Calendar instances, then
    // create a clone.
    if (mSharedZone) {
        AutoPtr<ITimeZone> clone;
        ICloneable::Probe(mZone)->Clone(IID_ITimeZone, (IInterface**)&clone);
        mZone = clone;
        mSharedZone = false;
    }
    *zone = mZone;
    REFCOUNT_ADD(*zone);
    return NOERROR;
}

AutoPtr<ITimeZone> Calendar::GetZone()
{
    return mZone;
}

ECode Calendar::SetLenient(
    /* [in] */ Boolean lenient)
{
    mLenient = lenient;
    return NOERROR;
}

ECode Calendar::IsLenient(
    /* [out] */ Boolean* lenient)
{
    VALIDATE_NOT_NULL(lenient);

    *lenient = mLenient;
    return NOERROR;
}

ECode Calendar::SetFirstDayOfWeek(
    /* [in] */ Integer value)
{
    if (mFirstDayOfWeek == value) {
        return NOERROR;
    }
    mFirstDayOfWeek = value;
    InvalidateWeekFields();
    return NOERROR;
}

ECode Calendar::GetFirstDayOfWeek(
    /* [out] */ Integer* value)
{
    VALIDATE_NOT_NULL(value);

    *value = mFirstDayOfWeek;
    return NOERROR;
}

ECode Calendar::SetMinimalDaysInFirstWeek(
    /* [in] */ Integer value)
{
    if (mMinimalDaysInFirstWeek == value) {
        return NOERROR;
    }
    mMinimalDaysInFirstWeek = value;
    InvalidateWeekFields();
    return NOERROR;
}

ECode Calendar::GetMinimalDaysInFirstWeek(
    /* [out] */ Integer* value)
{
    VALIDATE_NOT_NULL(value);

    *value = mMinimalDaysInFirstWeek;
    return NOERROR;
}

ECode Calendar::IsWeekDateSupported(
    /* [out] */ Boolean* supported)
{
    VALIDATE_NOT_NULL(supported);

    *supported = false;
    return NOERROR;
}

ECode Calendar::GetWeekYear(
    /* [out] */ Integer* weekYear)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Calendar::SetWeekDate(
    /* [in] */ Integer weekYear,
    /* [in] */ Integer weekOfYear,
    /* [in] */ Integer dayOfWeek)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Calendar::GetWeeksInWeekYear(
    /* [out] */ Integer* weeks)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Calendar::GetActualMinimum(
    /* [in] */ Integer field,
    /* [out] */ Integer* value)
{
    VALIDATE_NOT_NULL(value);

    Integer fieldValue, endValue;
    GetGreatestMinimum(field, &fieldValue);
    GetMinimum(field, &endValue);

    // if we know that the minimum value is always the same, just return it
    if (fieldValue == endValue) {
        *value = fieldValue;
        return NOERROR;
    }

    // clone the calendar so we don't mess with the real one, and set it to
    // accept anything for the field values
    AutoPtr<ICalendar> work;
    Clone(IID_ICalendar, (IInterface**)&work);
    work->SetLenient(true);

    // now try each value from GetLeastMinimum() to GetMinimum() one by one until
    // we get a value that normalizes to another value.  The last value that
    // normalizes to itself is the actual minimum for the current date
    Integer result = fieldValue;

    do {
        work->Set(field, fieldValue);
        if (work->Get(field, value), *value != fieldValue) {
            break;
        }
        else {
            result = fieldValue;
            fieldValue--;
        }
    } while (fieldValue >= endValue);

    *value = result;
    return NOERROR;
}

ECode Calendar::GetActualMaximum(
    /* [in] */ Integer field,
    /* [out] */ Integer* value)
{
    VALIDATE_NOT_NULL(value);

    Integer fieldValue, endValue;
    GetLeastMaximum(field, &fieldValue);
    GetMaximum(field, &endValue);

    // if we know that the maximum value is always the same, just return it.
    if (fieldValue == endValue) {
        *value = fieldValue;
        return NOERROR;
    }

    // clone the calendar so we don't mess with the real one, and set it to
    // accept anything for the field values
    AutoPtr<ICalendar> work;
    Clone(IID_ICalendar, (IInterface**)&work);
    work->SetLenient(true);

    // if we're counting weeks, set the day of the week to Sunday.  We know the
    // last week of a month or year will contain the first day of the week.
    if (field == WEEK_OF_YEAR || field == WEEK_OF_MONTH) {
        work->Set(DAY_OF_WEEK, mFirstDayOfWeek);
    }

    // now try each value from GetLeastMaximum() to GetMaximum() one by one until
    // we get a value that normalizes to another value.  The last value that
    // normalizes to itself is the actual maximum for the current date
    Integer result = fieldValue;

    do {
        work->Set(field, fieldValue);
        if (work->Get(field, value), *value != fieldValue) {
            break;
        }
        else {
            result = fieldValue;
            fieldValue++;
        }
    } while (fieldValue <= endValue);

    *value = result;
    return NOERROR;
}

ECode Calendar::CloneImpl(
    /* [in] */ ICalendar* newObj)
{
    Calendar* other = (Calendar*)newObj;

    other->mFields = Array<Integer>(FIELD_COUNT);
    other->mIsSet = Array<Boolean>(FIELD_COUNT);
    other->mStamp = Array<Integer>(FIELD_COUNT);
    for (Integer i = 0; i < FIELD_COUNT; i++) {
        other->mFields[i] = mFields[i];
        other->mStamp[i] = mStamp[i];
        other->mIsSet[i] = mIsSet[i];
    }
    AutoPtr<ITimeZone> zone;
    ICloneable::Probe(mZone)->Clone(IID_ITimeZone, (IInterface**)&zone);
    other->mZone = zone;
    return NOERROR;
}

static Array<String> CreateFIELD_NAME()
{
    Array<String> names(17);
    names[0] = "ERA";
    names[1] = "YEAR";
    names[2] = "MONTH";
    names[3] = "WEEK_OF_YEAR";
    names[4] = "WEEK_OF_MONTH";
    names[5] = "DAY_OF_MONTH";
    names[6] = "DAY_OF_YEAR";
    names[7] = "DAY_OF_WEEK";
    names[8] = "DAY_OF_WEEK_IN_MONTH";
    names[9] = "AM_PM";
    names[10] = "HOUR";
    names[11] = "HOUR_OF_DAY";
    names[12] = "MINUTE";
    names[13] = "SECOND";
    names[14] = "MILLISECOND";
    names[15] = "ZONE_OFFSET";
    names[16] = "DST_OFFSET";
    return names;
}

Array<String>& Calendar::GetFIELD_NAME()
{
    static Array<String> FIELD_NAME = CreateFIELD_NAME();
    return FIELD_NAME;
}

String Calendar::GetFieldName(
    /* [in] */ Integer field)
{
    return GetFIELD_NAME()[field];
}

ECode Calendar::ToString(
    /* [out] */ String* desc)
{
    VALIDATE_NOT_NULL(desc);

    AutoPtr<IStringBuilder> buffer;
    CStringBuilder::New(800, IID_IStringBuilder, (IInterface**)&buffer);
    buffer->Append(GetCoclassName((IObject*)this));
    buffer->AppendChar('[');
    AppendValue(buffer, String("time"), mIsTimeSet, mTime);
    buffer->Append(String(",areFieldsSet="));
    buffer->Append(mAreFieldsSet);
    buffer->Append(String(",areAllFieldsSet="));
    buffer->Append(mAreAllFieldsSet);
    buffer->Append(String(",lenient="));
    buffer->Append(mLenient);
    buffer->Append(String(",zone="));
    buffer->Append(mZone);
    AppendValue(buffer, String(",firstDayOfWeek"), true, (Long)mFirstDayOfWeek);
    AppendValue(buffer, String(",minimalDaysInFirstWeek"), true, (Long)mMinimalDaysInFirstWeek);
    for (Integer i = 0; i < FIELD_COUNT; ++i) {
        Boolean set;
        IsSet(i, &set);
        buffer->AppendChar(',');
        AppendValue(buffer, GetFIELD_NAME()[i], set, (Long)mFields[i]);
    }
    buffer->AppendChar(']');
    return buffer->ToString(desc);
}

void Calendar::AppendValue(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ const String& item,
    /* [in] */ Boolean valid,
    /* [in] */ Long value)
{
    sb->Append(item);
    sb->AppendChar('=');
    if (valid) {
        sb->Append(value);
    }
    else {
        sb->AppendChar('?');
    }
}

void Calendar::SetWeekCountData(
    /* [in] */ ILocale* desiredLocale)
{
    /* try to get the Locale data from the cache */
    AutoPtr<IArray> data;
    IMap::Probe(mCachedLocaleData)->Get(desiredLocale, (IInterface**)&data);
    if (data == nullptr) {
        CArray::New(IID_IInteger, 2, IID_IArray, (IInterface**)&data);
        AutoPtr<ILocaleData> localeData;
        LocaleData::Get(desiredLocale, &localeData);
        AutoPtr<IInteger> day, days;
        localeData->GetFirstDayOfWeek(&day);
        localeData->GetMinimalDaysInFirstWeek(&days);
        mCachedLocaleData->PutIfAbsent(desiredLocale, data);
    }
    AutoPtr<IInteger> v0, v1;
    data->Get(0, (IInterface**)&v0);
    data->Get(1, (IInterface**)&v1);
    mFirstDayOfWeek = CoreUtils::Unbox(v0);
    mMinimalDaysInFirstWeek = CoreUtils::Unbox(v1);
}

void Calendar::UpdateTime()
{
    ComputeTime();
    mIsTimeSet = true;
}

Integer Calendar::CompareTo(
    /* [in] */ Long t)
{
    Long thisTime = GetMillisOf(this);
    return (thisTime > t) ? 1 : (thisTime == t) ? 0 : -1;
}

Long Calendar::GetMillisOf(
    /* [in] */ Calendar* calendar)
{
    if (calendar->mIsTimeSet) {
        return calendar->mTime;
    }
    AutoPtr<ICalendar> cal;
    ((ICloneable*)calendar)->Clone(IID_ICalendar, (IInterface**)&cal);
    cal->SetLenient(true);
    Long time;
    cal->GetTimeInMillis(&time);
    return time;
}

void Calendar::AdjustStamp()
{
    Integer max = MINIMUM_USER_STAMP;
    Integer newStamp = MINIMUM_USER_STAMP;

    for (;;) {
        Integer min = IInteger::MAX_VALUE;
        for (Integer i = 0; i < mStamp.GetLength(); i++) {
            Integer v = mStamp[i];
            if (v >= newStamp && min > v) {
                min = v;
            }
            if (max < v) {
                max = v;
            }
        }
        if (max != min && min == IInteger::MAX_VALUE) {
            break;
        }
        for (Integer i = 0; i < mStamp.GetLength(); i++) {
            if (mStamp[i] == min) {
                mStamp[i] = newStamp;
            }
        }
        newStamp++;
        if (min == max) {
            break;
        }
    }
    mNextStamp = newStamp;
}

void Calendar::InvalidateWeekFields()
{
    if (mStamp[WEEK_OF_MONTH] != COMPUTED &&
        mStamp[WEEK_OF_YEAR] != COMPUTED) {
        return;
    }

    // We have to check the new values of these fields after changing
    // firstDayOfWeek and/or minimalDaysInFirstWeek. If the field values
    // have been changed, then set the new values. (4822110)
    AutoPtr<ICalendar> cal;
    Clone(IID_ICalendar, (IInterface**)&cal);
    cal->SetLenient(true);
    cal->Clear(WEEK_OF_MONTH);
    cal->Clear(WEEK_OF_YEAR);

    if (mStamp[WEEK_OF_MONTH] == COMPUTED) {
        Integer weekOfMonth;
        cal->Get(WEEK_OF_MONTH, &weekOfMonth);
        if (mFields[WEEK_OF_MONTH] != weekOfMonth) {
            mFields[WEEK_OF_MONTH] = weekOfMonth;
        }
    }

    if (mStamp[WEEK_OF_YEAR] == COMPUTED) {
        Integer weekOfYear;
        cal->Get(WEEK_OF_YEAR, &weekOfYear);
        if (mFields[WEEK_OF_YEAR] != weekOfYear) {
            mFields[WEEK_OF_YEAR] = weekOfYear;
        }
    }
}

}
}
