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

#ifndef __CCM_UTIL_LOCALE_PROVIDER_CALENDARDATAUTILITY_H__
#define __CCM_UTIL_LOCALE_PROVIDER_CALENDARDATAUTILITY_H__

#include "ccm.text.IDateFormatSymbols.h"
#include "ccm.util.ILocale.h"
#include "ccm.util.IMap.h"
#include <ccmautoptr.h>

using ccm::text::IDateFormatSymbols;

namespace ccm {
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
        /* [out] */ String* name);

    static ECode RetrieveCCMTimeFieldValueName(
        /* [in] */ const String& id,
        /* [in] */ Integer field,
        /* [in] */ Integer value,
        /* [in] */ Integer style,
        /* [in] */ ILocale* locale,
        /* [out] */ String* name);

    static ECode RetrieveFieldValueNames(
        /* [in] */ const String& id,
        /* [in] */ Integer field,
        /* [in] */ Integer style,
        /* [in] */ ILocale* locale,
        /* [out] */ IMap** names);

    static ECode RetrieveCCMTimeFieldValueNames(
        /* [in] */ const String& id,
        /* [in] */ Integer field,
        /* [in] */ Integer style,
        /* [in] */ ILocale* locale,
        /* [out] */ IMap** names);

private:
    static ECode RetrieveFieldValueNamesImpl(
        /* [in] */ const String& id,
        /* [in] */ Integer field,
        /* [in] */ Integer style,
        /* [in] */ ILocale* locale,
        /* [out] */ IMap** names);

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

#endif // __CCM_UTIL_LOCALE_PROVIDER_CALENDARDATAUTILITY_H__
