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

#include "como/core/CoreUtils.h"
#include "como/text/CDateFormatSymbols.h"
#include "como/util/CLinkedHashMap.h"
#include "como/util/locale/provider/CalendarDataUtility.h"
#include "como.util.ICalendar.h"

using como::core::CoreUtils;
using como::text::CDateFormatSymbols;
using como::text::IID_IDateFormatSymbols;
using como::util::ICalendar;

namespace como {
namespace util {
namespace locale {
namespace provider {

const String CalendarDataUtility::ISLAMIC_CALENDAR("islamic");
const String CalendarDataUtility::GREGORIAN_CALENDAR("gregorian");
const String CalendarDataUtility::BUDDHIST_CALENDAR("buddhist");
const String CalendarDataUtility::JAPANESE_CALENDAR("japanese");

Array<Integer> CalendarDataUtility::REST_OF_STYLES = {
    ICalendar::SHORT_STANDALONE,
    ICalendar::LONG_FORMAT,
    ICalendar::LONG_STANDALONE,
    ICalendar::NARROW_FORMAT,
    ICalendar::NARROW_STANDALONE
};

ECode CalendarDataUtility::RetrieveFieldValueName(
    /* [in] */ const String& id,
    /* [in] */ Integer field,
    /* [in] */ Integer value,
    /* [in] */ Integer style,
    /* [in] */ ILocale* locale,
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    if (field == ICalendar::ERA) {
        // For era the field value does not always equal the index into the names array.
        String type = NormalizeCalendarType(id);
        if (type.Equals(BUDDHIST_CALENDAR) || type.Equals(ISLAMIC_CALENDAR)) {
            // These calendars have only one era, but represented it by the value 1.
            value -= 1;
        }
        else if (type.Equals(JAPANESE_CALENDAR)) {
            // CLDR contains full data for historical eras, java.time only supports the 4
            // modern eras and numbers the modern eras starting with 1 (MEIJI). There are
            // 232 historical eras in CLDR/ICU so to get the real offset, we add 231.
            value += 231;
        }
        // Other eras use 0-based values (e.g. 0=BCE, 1=CE for gregorian).
    }
    if (value < 0) {
        *name = nullptr;
        return NOERROR;
    }
    Array<String> names;
    FAIL_RETURN(GetNames(id, field, style, locale, &names));
    if (value >= names.GetLength()) {
        *name = nullptr;
        return NOERROR;
    }
    *name = names[value];
    return NOERROR;
}

ECode CalendarDataUtility::RetrieveCOMOTimeFieldValueName(
    /* [in] */ const String& id,
    /* [in] */ Integer field,
    /* [in] */ Integer value,
    /* [in] */ Integer style,
    /* [in] */ ILocale* locale,
    /* [out] */ String* name)
{
    return RetrieveFieldValueName(id, field, value, style, locale, name);
}

ECode CalendarDataUtility::RetrieveFieldValueNames(
    /* [in] */ const String& id,
    /* [in] */ Integer field,
    /* [in] */ Integer style,
    /* [in] */ ILocale* locale,
    /* [out] */ IMap** retNames)
{
    VALIDATE_NOT_NULL(retNames);

    AutoPtr<IMap> names;
    if (style == ICalendar::ALL_STYLES) {
        FAIL_RETURN(RetrieveFieldValueNamesImpl(id, field, ICalendar::SHORT_FORMAT, locale, &names));
        for (Integer st : REST_OF_STYLES) {
            AutoPtr<IMap> restNames;
            FAIL_RETURN(RetrieveFieldValueNamesImpl(id, field, st, locale, &restNames));
            names->PutAll(restNames);
        }
    }
    else {
        FAIL_RETURN(RetrieveFieldValueNamesImpl(id, field, style, locale, &names));
    }
    Boolean isEmpty;
    if (names->IsEmpty(isEmpty), isEmpty) {
        *retNames = nullptr;
    }
    else {
        names.MoveTo(retNames);
    }
    return NOERROR;
}

ECode CalendarDataUtility::RetrieveFieldValueNamesImpl(
    /* [in] */ const String& id,
    /* [in] */ Integer field,
    /* [in] */ Integer style,
    /* [in] */ ILocale* locale,
    /* [out] */ IMap** retNames)
{
    Array<String> names;
    FAIL_RETURN(GetNames(id, field, style, locale, &names));
    Integer skipped = 0;
    Integer offset = 0;
    if (field == ICalendar::ERA) {
        String type = NormalizeCalendarType(id);
        if (type.Equals(BUDDHIST_CALENDAR) || type.Equals(ISLAMIC_CALENDAR)) {
            offset = 1;
        }
        else if (type.Equals(JAPANESE_CALENDAR)) {
            skipped = 232;
            offset = -231;
        }
    }
    AutoPtr<IMap> result;
    CLinkedHashMap::New(IID_IMap, (IInterface**)&result);
    for (Integer i = skipped; i < names.GetLength(); i++) {
        if (names[i].IsEmpty()) {
            continue;
        }

        AutoPtr<IInterface> prev;
        if (result->Put(CoreUtils::Box(names[i]), CoreUtils::Box(i + offset), &prev), prev != nullptr) {
            // Duplicate names indicate that the names would be ambiguous. Skip this style for
            // ALL_STYLES. In other cases this results in null being returned in
            // retrieveValueNames(), which is required by Calendar.getDisplayNames().
            return CLinkedHashMap::New(IID_IMap, (IInterface**)retNames);
        }
    }
    result.MoveTo(retNames);
    return NOERROR;
}

ECode CalendarDataUtility::RetrieveCOMOTimeFieldValueNames(
    /* [in] */ const String& id,
    /* [in] */ Integer field,
    /* [in] */ Integer style,
    /* [in] */ ILocale* locale,
    /* [out] */ IMap** names)
{
    return RetrieveFieldValueNames(id, field, style, locale, names);
}

ECode CalendarDataUtility::GetNames(
    /* [in] */ const String& id,
    /* [in] */ Integer field,
    /* [in] */ Integer style,
    /* [in] */ ILocale* locale,
    /* [out] */ Array<String>* names)
{
    AutoPtr<IDateFormatSymbols> symbols = GetDateFormatSymbols(id, locale);
    switch (field) {
        case ICalendar::MONTH:
            return symbols->GetMonths(names);
        case ICalendar::ERA:
            return symbols->GetEras(names);
        case ICalendar::DAY_OF_WEEK:
            return symbols->GetWeekdays(names);
        case ICalendar::AM_PM:
            return symbols->GetAmPmStrings(names);
        default:
            Logger::E("CalendarDataUtility", "Unknown field: %d", field);
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
}

AutoPtr<IDateFormatSymbols> CalendarDataUtility::GetDateFormatSymbols(
    /* [in] */ const String& id,
    /* [in] */ ILocale* locale)
{
    AutoPtr<IDateFormatSymbols> dfs;
    CDateFormatSymbols::New(locale, IID_IDateFormatSymbols, (IInterface**)&dfs);
    return dfs;
}

String CalendarDataUtility::NormalizeCalendarType(
    /* [in] */ const String& requestID)
{
    String type;
    if (requestID.Equals("gregory") || requestID.Equals("iso8601")) {
        type = GREGORIAN_CALENDAR;
    }
    else if (requestID.StartsWith(ISLAMIC_CALENDAR)) {
        type = ISLAMIC_CALENDAR;
    }
    else {
        type = requestID;
    }
    return type;
}

}
}
}
}
