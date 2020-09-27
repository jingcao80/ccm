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

#ifndef __COMO_UTIL_LOCALE_PROVIDER_CALENDARDATAUTILITY_H__
#define __COMO_UTIL_LOCALE_PROVIDER_CALENDARDATAUTILITY_H__

#include "como.text.IDateFormatSymbols.h"
#include "como.util.ILocale.h"
#include "como.util.IMap.h"
#include <comosp.h>

using como::text::IDateFormatSymbols;

namespace como {
namespace util {
namespace locale {
namespace provider {

class CalendarDataUtility
{
public:
    static ECode RetrieveFieldValueName(
        /* [in] */ const String& id,
        /* [in] */ Integer field,
        /* [in] */ Integer value,
        /* [in] */ Integer style,
        /* [in] */ ILocale* locale,
        /* [out] */ String& name);

    static ECode RetrieveCOMOTimeFieldValueName(
        /* [in] */ const String& id,
        /* [in] */ Integer field,
        /* [in] */ Integer value,
        /* [in] */ Integer style,
        /* [in] */ ILocale* locale,
        /* [out] */ String& name);

    static ECode RetrieveFieldValueNames(
        /* [in] */ const String& id,
        /* [in] */ Integer field,
        /* [in] */ Integer style,
        /* [in] */ ILocale* locale,
        /* [out] */ AutoPtr<IMap>& names);

    static ECode RetrieveCOMOTimeFieldValueNames(
        /* [in] */ const String& id,
        /* [in] */ Integer field,
        /* [in] */ Integer style,
        /* [in] */ ILocale* locale,
        /* [out] */ AutoPtr<IMap>& names);

private:
    static ECode RetrieveFieldValueNamesImpl(
        /* [in] */ const String& id,
        /* [in] */ Integer field,
        /* [in] */ Integer style,
        /* [in] */ ILocale* locale,
        /* [out] */ AutoPtr<IMap>& names);

    static ECode GetNames(
        /* [in] */ const String& id,
        /* [in] */ Integer field,
        /* [in] */ Integer style,
        /* [in] */ ILocale* locale,
        /* [out] */ Array<String>* names);

    static AutoPtr<IDateFormatSymbols> GetDateFormatSymbols(
        /* [in] */ const String& id,
        /* [in] */ ILocale* locale);

    static String NormalizeCalendarType(
        /* [in] */ const String& requestID);

private:
    static const String ISLAMIC_CALENDAR;
    static const String GREGORIAN_CALENDAR;
    static const String BUDDHIST_CALENDAR;
    static const String JAPANESE_CALENDAR;

    static Array<Integer> REST_OF_STYLES;
};

}
}
}
}

#endif // __COMO_UTIL_LOCALE_PROVIDER_CALENDARDATAUTILITY_H__
