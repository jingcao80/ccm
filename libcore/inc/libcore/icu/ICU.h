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

#ifndef __LIBCORE_ICU_ICU_H__
#define __LIBCORE_ICU_ICU_H__

#include "ccm.util.ILocale.h"
#include "libcore/icu/LocaleData.h"
#include "libcore/util/BasicLruCache.h"

using ccm::util::ILocale;
using libcore::icu::LocaleData;
using libcore::util::BasicLruCache;

namespace libcore {
namespace icu {

class ICU
{
public:
    static Array<String> GetISOLanguages();

    static Array<String> GetISOCountries();

    static AutoPtr<ILocale> LocaleFromIcuLocaleId(
        /* [in] */ const String& localeId);

    static Array<ILocale*> LocalesFromStrings(
        /* [in] */ const Array<String>& localeNames);

    static Array<ILocale*> GetAvailableLocales();

    static String GetBestDateTimePattern(
        /* [in] */ const String& skeleton,
        /* [in] */ ILocale* locale);

    static String GetBestDateTimePattern(
        /* [in] */ const String& skeleton,
        /* [in] */ const String& languageTag);

    inline static Boolean U_FAILURE(
        /* [in] */ Integer error);

    static Array<String> GetAvailableCurrencyCodes();

    static String GetCurrencyCode(
        /* [in] */ const String& countryCode);

    static String GetCurrencyDisplayName(
        /* [in] */ ILocale* locale,
        /* [in] */ const String& currencyCode);

    static String GetCurrencyDisplayName(
        /* [in] */ const String& languageTag,
        /* [in] */ const String& currencyCode);

    static Integer GetCurrencyFractionDigits(
        /* [in] */ const String& currencyCode);

    static Integer GetCurrencyNumericCode(
        /* [in] */ const String& currencyCode);

    static String GetCurrencySymbol(
        /* [in] */ ILocale* locale,
        /* [in] */ const String& currencyCode);

    static String GetCurrencySymbol(
        /* [in] */ const String& languageTag,
        /* [in] */ const String& currencyCode);

    static String GetDisplayCountry(
        /* [in] */ ILocale* targetLocale,
        /* [in] */ ILocale* locale);

    static String GetDisplayLanguage(
        /* [in] */ ILocale* targetLocale,
        /* [in] */ ILocale* locale);

    static String GetDisplayVariant(
        /* [in] */ ILocale* targetLocale,
        /* [in] */ ILocale* locale);

    static String GetDisplayScript(
        /* [in] */ ILocale* targetLocale,
        /* [in] */ ILocale* locale);

    static String GetISO3Country(
        /* [in] */ const String& languageTag);

    static String GetISO3Language(
        /* [in] */ const String& languageTag);

    static Boolean InitLocaleData(
        /* [in] */ const String& languageTag,
        /* [in] */ LocaleData* result);

    static ECode SetDefaultLocale(
        /* [in] */ const String& languageTag);

private:
    ICU();

    static void ParseLangScriptRegionAndVariants(
        /* [in] */ const String& string,
        /* [out] */ Array<String>& outputArray);

    static AutoPtr<BasicLruCache> GetCACHED_PATTERNS();

public:
    // Just the subset of error codes needed by CharsetDecoderICU/CharsetEncoderICU.
    static constexpr Integer U_ZERO_ERROR_ = 0;
    static constexpr Integer U_INVALID_CHAR_FOUND_ = 10;
    static constexpr Integer U_TRUNCATED_CHAR_FOUND_ = 11;
    static constexpr Integer U_ILLEGAL_CHAR_FOUND_ = 12;
    static constexpr Integer U_BUFFER_OVERFLOW_ERROR_ = 15;

private:
    static Array<ILocale*> sAvailableLocalesCache;
    static Array<String> sIsoCountries;
    static Array<String> sIsoLanguages;

    static constexpr Integer IDX_LANGUAGE = 0;
    static constexpr Integer IDX_SCRIPT = 1;
    static constexpr Integer IDX_REGION = 2;
    static constexpr Integer IDX_VARIANT = 3;
};

Boolean ICU::U_FAILURE(
    /* [in] */ Integer error)
{
    return error > U_ZERO_ERROR_;
}

}
}

#endif // __LIBCORE_ICU_ICU_H__
