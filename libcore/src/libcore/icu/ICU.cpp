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

#include "como/core/AutoLock.h"
#include "como/core/CInteger.h"
#include "como/core/CoreUtils.h"
#include "como/core/StringUtils.h"
#include "como/util/CHashMap.h"
#include "como/util/CLinkedHashSet.h"
#include "como/util/CLocaleBuilder.h"
#include "como/util/Collections.h"
#include "como.core.IChar.h"
#include "como.core.ICharSequence.h"
#include "como.util.IIterator.h"
#include "como.util.IMap.h"
#include "como.util.IMapEntry.h"
#include "como.util.ISet.h"
#include "libcore/icu/ICU.h"
#include "libcore/icu/StringByteSink.h"
#include "libcore/icu/UStringEnumeration.h"
#include <coredef.h>
#include <memory>
#include <unicode/brkiter.h>
#include <unicode/calendar.h>
#include <unicode/decimfmt.h>
#include <unicode/dtfmtsym.h>
#include <unicode/dtptngen.h>
#include <unicode/locid.h>
#include <unicode/ucurr.h>
#include <unicode/uloc.h>
#include <unicode/ures.h>

#include <unicode/utypes.h>
#include <unicode/udata.h>

using como::core::AutoLock;
using como::core::CInteger;
using como::core::CoreUtils;
using como::core::E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
using como::core::IChar;
using como::core::ICharSequence;
using como::core::IID_IInteger;
using como::core::IInteger;
using como::core::StringUtils;
using como::util::CHashMap;
using como::util::CLinkedHashSet;
using como::util::CLocaleBuilder;
using como::util::Collections;
using como::util::IID_ILocale;
using como::util::IID_ILocaleBuilder;
using como::util::IID_IMap;
using como::util::IID_ISet;
using como::util::IIterator;
using como::util::ILocaleBuilder;
using como::util::IMap;
using como::util::IMapEntry;
using como::util::ISet;

U_CDECL_BEGIN

#define U_ICUDATA_LANG U_ICUDATA_NAME U_TREE_SEPARATOR_STRING "lang"
#define U_ICUDATA_REGION U_ICUDATA_NAME U_TREE_SEPARATOR_STRING "region"
#define U_ICUDATA_CURR U_ICUDATA_NAME U_TREE_SEPARATOR_STRING "curr"
#define U_ICUDATA_ZONE U_ICUDATA_NAME U_TREE_SEPARATOR_STRING "zone"
#define U_ICUDATA_UNIT U_ICUDATA_NAME U_TREE_SEPARATOR_STRING "unit"

U_CDECL_END

namespace libcore {
namespace icu {

static Array<String> ToStringArray(
    /* [in] */ const char* const* strings)
{
    size_t count = 0;
    while (strings[count] != nullptr) {
        count++;
    }

    Array<String> strArray(count);
    for (size_t i = 0; i < count; i++) {
        strArray[i] = strings[i];
    }
    return strArray;
}

template <typename Counter, typename Getter>
static Array<String> ToStringArray(Counter* counter, Getter* getter)
{
    size_t count = (*counter)();
    Array<String> strArray(count);
    for (size_t i = 0; i < count; ++i) {
        strArray[i] = (*getter)(i);
    }
    return strArray;
}

ECode MaybeThrowIcuException(
    /* [in] */ const char* provider,
    /* [in] */ UErrorCode errorCode)
{
    if (U_SUCCESS(errorCode)) {
        return NOERROR;
    }

    switch (errorCode) {
        case U_ILLEGAL_ARGUMENT_ERROR:
            Logger::E("ICU", "icu error: %s E_ILLEGAL_ARGUMENT_EXCEPTION", provider);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        case U_INDEX_OUTOFBOUNDS_ERROR:
        case U_BUFFER_OVERFLOW_ERROR:
            Logger::E("ICU", "icu error: %s E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION", provider);
            return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
        case U_UNSUPPORTED_ERROR:
            Logger::E("ICU", "icu error: %s E_UNSUPPORTED_OPERATION_EXCEPTION", provider);
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
        default:
            Logger::E("ICU", "icu error: %s E_RUNTIME_EXCEPTION", provider);
            return E_RUNTIME_EXCEPTION;
    }
}

inline static Char ToChar(
    /* [in] */ const ::icu::UnicodeString& value)
{
    return value.length() == 0 ? 0 : (Char)value.charAt(0);
}

String ToUTF8String(
    /* [in] */ const ::icu::UnicodeString& value)
{
    StringByteSink sink;
    value.toUTF8(sink);
    return sink.ToString();
}

Array<String> FromStringEnumeration(
    /* [in] */ UErrorCode& status,
    /* [in] */ const char* provider,
    /* [in] */ ::icu::StringEnumeration* se)
{
    Array<String> result;
    if (FAILED(MaybeThrowIcuException(provider, status))) {
        return result;
    }

    int32_t count = se->count(status);
    if (FAILED(MaybeThrowIcuException("StringEnumeration::count", status))) {
        return result;
    }

    result = Array<String>(count);
    for (int32_t i = 0; i < count; ++i) {
        const ::icu::UnicodeString* string = se->snext(status);
        if (FAILED(MaybeThrowIcuException("StringEnumeration::snext", status))) {
            return Array<String>::Null();
        }
        result[i] = ToUTF8String(*string);
    }
    return result;
}

class ScopedResourceBundle
{
public:
    explicit ScopedResourceBundle(UResourceBundle* bundle)
        : bundle_(bundle)
    {}

    ~ScopedResourceBundle()
    {
        if (bundle_ != NULL) {
            ures_close(bundle_);
        }
    }

    UResourceBundle* get() {
        return bundle_;
    }

    bool hasKey(const char* key)
    {
        UErrorCode status = U_ZERO_ERROR;
        ures_getStringByKey(bundle_, key, nullptr, &status);
        return U_SUCCESS(status);
    }

private:
    UResourceBundle* bundle_;
};

static String ICU_getCurrencyCode(
    /* [in] */ const String& countryCode)
{
    UErrorCode status = U_ZERO_ERROR;
    ScopedResourceBundle supplData(ures_openDirect(U_ICUDATA_CURR, "supplementalData", &status));
    if (U_FAILURE(status)) {
        return String();
    }

    ScopedResourceBundle currencyMap(ures_getByKey(supplData.get(), "CurrencyMap", nullptr, &status));
    if (U_FAILURE(status)) {
        return String();
    }

    ScopedResourceBundle currency(ures_getByKey(currencyMap.get(), countryCode.string(), nullptr, &status));
    if (U_FAILURE(status)) {
        return String();
    }

    ScopedResourceBundle currencyElem(ures_getByIndex(currency.get(), 0, nullptr, &status));
    if (U_FAILURE(status)) {
        return String("XXX");
    }

    // Check if there's a 'to' date. If there is, the currency isn't used anymore.
    ScopedResourceBundle currencyTo(ures_getByKey(currencyElem.get(), "to", nullptr, &status));
    if (!U_FAILURE(status)) {
        return String();
    }
    // Ignore the failure to find a 'to' date.
    status = U_ZERO_ERROR;

    ScopedResourceBundle currencyId(ures_getByKey(currencyElem.get(), "id", nullptr, &status));
    if (U_FAILURE(status)) {
        // No id defined for this country
        return String("XXX");
    }

    int32_t charCount;
    const char* chars = ures_getUTF8String(currencyId.get(), nullptr, &charCount, FALSE, &status);
    return (charCount == 0) ? String("XXX") : String(chars, charCount);
}

static String GetCurrencyName(
    /* [in] */ const String& languageTag,
    /* [in] */ const String& currencyCode,
    /* [in] */ UCurrNameStyle nameStyle)
{
    if (languageTag.IsNull() || currencyCode.IsNull()) {
        return String();
    }
    ::icu::UnicodeString icuCurrencyCode(currencyCode.string(), currencyCode.GetByteLength());
    UErrorCode status = U_ZERO_ERROR;
    UBool isChoiceFormat = false;
    int32_t charCount;
    const UChar* chars = ucurr_getName(icuCurrencyCode.getTerminatedBuffer(), languageTag.string(),
                nameStyle, &isChoiceFormat, &charCount, &status);
    if (status == U_USING_DEFAULT_WARNING) {
        if (nameStyle == UCURR_SYMBOL_NAME) {
            // ICU doesn't distinguish between falling back to the root locale and meeting a genuinely
            // unknown currency. The Currency class does.
            if (!ucurr_isAvailable(icuCurrencyCode.getTerminatedBuffer(), U_DATE_MIN, U_DATE_MAX, &status)) {
                return String();
            }
        }
        if (nameStyle == UCURR_LONG_NAME) {
            // ICU's default is English. We want the ISO 4217 currency code instead.
            chars = icuCurrencyCode.getBuffer();
            charCount = icuCurrencyCode.length();
        }
    }
    return (charCount == 0) ? String() : ToUTF8String(::icu::UnicodeString(chars, charCount));
}

static Array<String> GetStringArrayField(
    /* [in] */ const char* fieldName,
    /* [in] */ const ::icu::UnicodeString* valueArray,
    /* [in] */ int32_t size)
{
    Array<String> result(size);
    StringByteSink sink;
    for (int32_t i = 0; i < size; i++) {
        valueArray[i].toUTF8(sink);
        result[i] = sink.ToString();
        sink.Clear();
    }
    return result;
}

static String GetStringField(
    /* [in] */ const char* fieldName,
    /* [in] */ UResourceBundle* bundle,
    /* [in] */ int index)
{
    UErrorCode status = U_ZERO_ERROR;
    int charCount;
    const UChar* chars;
    UResourceBundle* currentBundle = ures_getByIndex(bundle, index, nullptr, &status);
    switch (ures_getType(currentBundle)) {
        case URES_STRING:
            chars = ures_getString(currentBundle, &charCount, &status);
            break;
        case URES_ARRAY:
            // In case there is an array, como currently only cares about the
            // first string of that array, the rest of the array is used by ICU
            // for additional data ignored by como.
            chars = ures_getStringByIndex(currentBundle, 0, &charCount, &status);
            break;
      default:
            status = U_INVALID_FORMAT_ERROR;
    }
    ures_close(currentBundle);
    if (U_SUCCESS(status)) {
        return ToUTF8String(::icu::UnicodeString(chars, charCount));
    }
    else {
        Logger::E("ICU", "Error setting String field %s from ICU resource (index %d): %s",
                fieldName, index, u_errorName(status));
        CHECK(0);
        return String();
    }
}

static void SetNumberPatterns(
    /* [in] */ LocaleData* localeData,
    /* [in] */ ::icu::Locale& locale)
{
    UErrorCode status = U_ZERO_ERROR;

    ::icu::UnicodeString pattern;
    std::unique_ptr<::icu::DecimalFormat> fmt(static_cast<::icu::DecimalFormat*>(::icu::NumberFormat::createInstance(locale, UNUM_CURRENCY, status)));
    pattern = fmt->toPattern(pattern.remove());
    localeData->mCurrencyPattern = ToUTF8String(pattern);

    fmt.reset(static_cast<::icu::DecimalFormat*>(::icu::NumberFormat::createInstance(locale, UNUM_DECIMAL, status)));
    pattern = fmt->toPattern(pattern.remove());
    localeData->mNumberPattern = ToUTF8String(pattern);

    fmt.reset(static_cast<::icu::DecimalFormat*>(::icu::NumberFormat::createInstance(locale, UNUM_PERCENT, status)));
    pattern = fmt->toPattern(pattern.remove());
    localeData->mPercentPattern = ToUTF8String(pattern);
}

static void SetDecimalFormatSymbolsData(
    /* [in] */ LocaleData* localeData,
    /* [in] */ ::icu::Locale& locale)
{
    UErrorCode status = U_ZERO_ERROR;
    ::icu::DecimalFormatSymbols dfs(locale, status);

    localeData->mDecimalSeparator = ToChar(dfs.getSymbol(::icu::DecimalFormatSymbols::kDecimalSeparatorSymbol));
    localeData->mGroupingSeparator = ToChar(dfs.getSymbol(::icu::DecimalFormatSymbols::kGroupingSeparatorSymbol));
    localeData->mPatternSeparator = ToChar(dfs.getSymbol(::icu::DecimalFormatSymbols::kPatternSeparatorSymbol));
    localeData->mPercent = ToUTF8String(dfs.getSymbol(::icu::DecimalFormatSymbols::kPercentSymbol));
    localeData->mPerMill = ToChar(dfs.getSymbol(::icu::DecimalFormatSymbols::kPerMillSymbol));
    localeData->mMonetarySeparator = ToChar(dfs.getSymbol(::icu::DecimalFormatSymbols::kMonetarySeparatorSymbol));
    localeData->mMinusSign = ToUTF8String(dfs.getSymbol(::icu::DecimalFormatSymbols:: kMinusSignSymbol));
    localeData->mExponentSeparator = ToUTF8String(dfs.getSymbol(::icu::DecimalFormatSymbols::kExponentialSymbol));
    localeData->mInfinity = ToUTF8String(dfs.getSymbol(::icu::DecimalFormatSymbols::kInfinitySymbol));
    localeData->mNaN = ToUTF8String(dfs.getSymbol(::icu::DecimalFormatSymbols::kNaNSymbol));
    localeData->mZeroDigit = ToChar(dfs.getSymbol(::icu::DecimalFormatSymbols::kZeroDigitSymbol));
}

// Iterates up through the locale hierarchy. So "en_US" would return "en_US", "en", "".
class LocaleNameIterator
{
public:
    LocaleNameIterator(const char* locale_name, UErrorCode& status)
        : status_(status), has_next_(true)
    {
        strcpy(locale_name_, locale_name);
        locale_name_length_ = strlen(locale_name_);
    }

    const char* Get()
    {
        return locale_name_;
    }

    Boolean HasNext()
    {
        return has_next_;
    }

    void Up()
    {
        if (locale_name_length_ == 0) {
            has_next_ = false;
        }
        else {
            locale_name_length_ = uloc_getParent(locale_name_, locale_name_, sizeof(locale_name_), &status_);
        }
    }

private:
    UErrorCode& status_;
    Boolean has_next_;
    char locale_name_[ULOC_FULLNAME_CAPACITY];
    int32_t locale_name_length_;
};

static Boolean GetAmPmMarkersNarrow(
    /* [in] */ LocaleData* localeData,
    /* [in]*/ const char* locale_name)
{
    UErrorCode status = U_ZERO_ERROR;
    ScopedResourceBundle root(ures_open(nullptr, locale_name, &status));
    if (U_FAILURE(status)) {
        return false;
    }
    ScopedResourceBundle calendar(ures_getByKey(root.get(), "calendar", nullptr, &status));
    if (U_FAILURE(status)) {
        return false;
    }
    ScopedResourceBundle gregorian(ures_getByKey(calendar.get(), "gregorian", nullptr, &status));
    if (U_FAILURE(status)) {
        return false;
    }
    ScopedResourceBundle amPmMarkersNarrow(ures_getByKey(gregorian.get(), "AmPmMarkersNarrow", nullptr, &status));
    if (U_FAILURE(status)) {
        return false;
    }
    localeData->mNarrowAm = GetStringField("narrowAm", amPmMarkersNarrow.get(), 0);
    localeData->mNarrowPm = GetStringField("narrowPm", amPmMarkersNarrow.get(), 1);
    return true;
}

static Boolean GetDateTimePatterns(
    /* [in] */ LocaleData* localeData,
    /* [in] */ const char* locale_name)
{
    UErrorCode status = U_ZERO_ERROR;
    ScopedResourceBundle root(ures_open(nullptr, locale_name, &status));
    if (U_FAILURE(status)) {
        return false;
    }
    ScopedResourceBundle calendar(ures_getByKey(root.get(), "calendar", nullptr, &status));
    if (U_FAILURE(status)) {
        return false;
    }
    ScopedResourceBundle gregorian(ures_getByKey(calendar.get(), "gregorian", nullptr, &status));
    if (U_FAILURE(status)) {
        return false;
    }
    ScopedResourceBundle dateTimePatterns(ures_getByKey(gregorian.get(), "DateTimePatterns", nullptr, &status));
    if (U_FAILURE(status)) {
        return false;
    }
    localeData->mFullTimeFormat = GetStringField("fullTimeFormat", dateTimePatterns.get(), 0);
    localeData->mLongTimeFormat = GetStringField("longTimeFormat", dateTimePatterns.get(), 1);
    localeData->mMediumTimeFormat = GetStringField("mediumTimeFormat", dateTimePatterns.get(), 2);
    localeData->mShortTimeFormat = GetStringField("shortTimeFormat", dateTimePatterns.get(), 3);
    localeData->mFullDateFormat = GetStringField("fullDateFormat", dateTimePatterns.get(), 4);
    localeData->mLongDateFormat = GetStringField("longDateFormat", dateTimePatterns.get(), 5);
    localeData->mMediumDateFormat = GetStringField("mediumDateFormat", dateTimePatterns.get(), 6);
    localeData->mShortDateFormat = GetStringField("shortDateFormat", dateTimePatterns.get(), 7);
    return true;
}

static Boolean GetYesterdayTodayAndTomorrow(
    /* [in] */ LocaleData* localeData,
    /* [in] */ const ::icu::Locale& locale,
    /* [in] */ const char* locale_name)
{
    UErrorCode status = U_ZERO_ERROR;
    ScopedResourceBundle root(ures_open(nullptr, locale_name, &status));
    ScopedResourceBundle fields(ures_getByKey(root.get(), "fields", nullptr, &status));
    ScopedResourceBundle day(ures_getByKey(fields.get(), "day", nullptr, &status));
    ScopedResourceBundle relative(ures_getByKey(day.get(), "relative", nullptr, &status));
    if (U_FAILURE(status)) {
        return false;
    }

    ::icu::UnicodeString yesterday(::icu::ures_getUnicodeStringByKey(relative.get(), "-1", &status));
    ::icu::UnicodeString today(::icu::ures_getUnicodeStringByKey(relative.get(), "0", &status));
    ::icu::UnicodeString tomorrow(::icu::ures_getUnicodeStringByKey(relative.get(), "1", &status));
    if (U_FAILURE(status)) {
        Logger::E("ICU", "Error getting yesterday/today/tomorrow for %s: %s", locale_name, u_errorName(status));
        return false;
    }

    // We title-case the strings so they have consistent capitalization (http://b/14493853).
    std::unique_ptr<::icu::BreakIterator> brk(::icu::BreakIterator::createSentenceInstance(locale, status));
    if (U_FAILURE(status)) {
        Logger::E("ICU", "Error getting yesterday/today/tomorrow break iterator for %s: %s", locale_name, u_errorName(status));
        return false;
    }
    yesterday.toTitle(brk.get(), locale, U_TITLECASE_NO_LOWERCASE | U_TITLECASE_NO_BREAK_ADJUSTMENT);
    today.toTitle(brk.get(), locale, U_TITLECASE_NO_LOWERCASE | U_TITLECASE_NO_BREAK_ADJUSTMENT);
    tomorrow.toTitle(brk.get(), locale, U_TITLECASE_NO_LOWERCASE | U_TITLECASE_NO_BREAK_ADJUSTMENT);

    localeData->mYesterday = ToUTF8String(yesterday);
    localeData->mToday = ToUTF8String(today);
    localeData->mTomorrow = ToUTF8String(tomorrow);
    return true;
}

Array<ILocale*> ICU::sAvailableLocalesCache;
Array<String> ICU::sIsoCountries;
Array<String> ICU::sIsoLanguages;

static AutoPtr<BasicLruCache> CreateBasicLruCache(
    /* [in] */ Integer maxSize)
{
    AutoPtr<BasicLruCache> blc = new BasicLruCache();
    blc->Constructor(maxSize);
    return blc;
}

AutoPtr<BasicLruCache> ICU::GetCACHED_PATTERNS()
{
    static AutoPtr<BasicLruCache> CACHED_PATTERNS = CreateBasicLruCache(8);
    return CACHED_PATTERNS;
}

Array<String> ICU::GetISOLanguages()
{
    if (sIsoLanguages.IsNull()) {
        sIsoLanguages = ToStringArray(::icu::Locale::getISOLanguages());
    }
    return sIsoLanguages.Clone();
}

Array<String> ICU::GetISOCountries()
{
    if (sIsoCountries.IsNull()) {
        sIsoCountries = ToStringArray(::icu::Locale::getISOCountries());
    }
    return sIsoCountries.Clone();
}

void ICU::ParseLangScriptRegionAndVariants(
    /* [in] */ const String& string,
    /* [out] */ Array<String>& outputArray)
{
    Integer first = string.IndexOf('_');
    Integer second = string.IndexOf('_', first + 1);
    Integer third = string.IndexOf('_', second + 1);

    if (first == -1) {
        outputArray[IDX_LANGUAGE] = string;
    }
    else if (second == -1) {
        // Language and country ("ja_JP") OR
        // Language and script ("en_Latn") OR
        // Language and variant ("en_POSIX").
        outputArray[IDX_LANGUAGE] = string.Substring(0, first);
        String secondString = string.Substring(first + 1);

        if (secondString.GetLength() == 4) {
            // 4 Letter ISO script code.
            outputArray[IDX_SCRIPT] = secondString;
        }
        else if (secondString.GetLength() == 2 || secondString.GetLength() == 3) {
            // 2 or 3 Letter region code.
            outputArray[IDX_REGION] = secondString;
        }
        else {
            // If we're here, the length of the second half is either 1 or greater
            // than 5. Assume that ICU won't hand us malformed tags, and therefore
            // assume the rest of the string is a series of variant tags.
            outputArray[IDX_VARIANT] = secondString;
        }
    }
    else if (third == -1) {
        // Language and country and variant ("ja_JP_TRADITIONAL") OR
        // Language and script and variant ("en_Latn_POSIX") OR
        // Language and script and region ("en_Latn_US"). OR
        // Language and variant with multiple subtags ("en_POSIX_XISOP")
        outputArray[IDX_LANGUAGE] = string.Substring(0, first);
        String secondString = string.Substring(first + 1, second);
        String thirdString = string.Substring(second + 1);

        if (secondString.GetLength() == 4) {
            // The second subtag is a script.
            outputArray[IDX_SCRIPT] = secondString;

            // The third subtag can be either a region or a variant, depending
            // on its length.
            if (thirdString.GetLength() == 2 || thirdString.GetLength() == 3 ||
                    thirdString.IsEmpty()) {
                outputArray[IDX_REGION] = thirdString;
            }
            else {
                outputArray[IDX_VARIANT] = thirdString;
            }
        }
        else if (secondString.IsEmpty() || secondString.GetLength() == 2 ||
                secondString.GetLength() == 3) {
            // The second string is a region, and the third a variant.
            outputArray[IDX_REGION] = secondString;
            outputArray[IDX_VARIANT] = thirdString;
        }
        else {
            // Variant with multiple subtags.
            outputArray[IDX_VARIANT] = string.Substring(first + 1);
        }
    }
    else {
        // Language, script, region and variant with 1 or more subtags
        // ("en_Latn_US_POSIX") OR
        // Language, region and variant with 2 or more subtags
        // (en_US_POSIX_VARIANT).
        outputArray[IDX_LANGUAGE] = string.Substring(0, first);
        String secondString = string.Substring(first + 1, second);
        if (secondString.GetLength() == 4) {
            outputArray[IDX_SCRIPT] = secondString;
            outputArray[IDX_REGION] = string.Substring(second + 1, third);
            outputArray[IDX_VARIANT] = string.Substring(third + 1);
        }
        else {
            outputArray[IDX_REGION] = secondString;
            outputArray[IDX_VARIANT] = string.Substring(second + 1);
        }
    }
}

AutoPtr<ILocale> ICU::LocaleFromIcuLocaleId(
    /* [in] */ const String& localeId)
{
    Integer extensionsIndex = localeId.IndexOf('@');

    AutoPtr<IMap> extensionsMap;
    AutoPtr<IMap> unicodeKeywordsMap;
    AutoPtr<ISet> unicodeAttributeSet;

    if (extensionsIndex != -1) {
        CHashMap::New(IID_IMap, (IInterface**)&extensionsMap);
        CHashMap::New(IID_IMap, (IInterface**)&unicodeKeywordsMap);
        CHashMap::New(IID_IMap, (IInterface**)&unicodeAttributeSet);

        // ICU sends us a semi-colon (ULOC_KEYWORD_ITEM_SEPARATOR) delimited string
        // containing all "keywords" it could parse. An ICU keyword is a key-value pair
        // separated by an "=" (ULOC_KEYWORD_ASSIGN).
        //
        // Each keyword item can be one of three things :
        // - A unicode extension attribute list: In this case the item key is "attribute"
        //   and the value is a hyphen separated list of unicode attributes.
        // - A unicode extension keyword: In this case, the item key will be larger than
        //   1 char in length, and the value will be the unicode extension value.
        // - A BCP-47 extension subtag: In this case, the item key will be exactly one
        //   char in length, and the value will be a sequence of unparsed subtags that
        //   represent the extension.
        //
        // Note that this implies that unicode extension keywords are "promoted" to
        // to the same namespace as the top level extension subtags and their values.
        // There can't be any collisions in practice because the BCP-47 spec imposes
        // restrictions on their lengths.
        String extensionsString = localeId.Substring(extensionsIndex + 1);
        Array<String> extensions = StringUtils::Split(extensionsString, String(";"));
        for (String extension : extensions) {
            // This is the special key for the unicode attributes
            if (extension.StartsWith("attribute=")) {
                String unicodeAttributeValues = extension.Substring(strlen("attribute="));
                for (String unicodeAttribute : StringUtils::Split(unicodeAttributeValues, String("-"))) {
                    unicodeAttributeSet->Add(CoreUtils::Box(unicodeAttribute));
                }
            }
            else {
                Integer separatorIndex = extension.IndexOf('=');

                if (separatorIndex == 1) {
                    // This is a BCP-47 extension subtag.
                    String value = extension.Substring(2);
                    Char extensionId = extension.GetChar(0);

                    extensionsMap->Put(CoreUtils::Box(extensionId), CoreUtils::Box(value));
                }
                else {
                    unicodeKeywordsMap->Put(CoreUtils::Box(extension.Substring(0, separatorIndex)),
                                CoreUtils::Box(extension.Substring(separatorIndex + 1)));
                }
            }
        }
    }
    else {
        extensionsMap = Collections::GetEMPTY_MAP();
        unicodeKeywordsMap = Collections::GetEMPTY_MAP();
        unicodeAttributeSet = Collections::GetEMPTY_SET();
    }

    Array<String> outputArray{ String(""), String(""), String(""), String("") };
    if (extensionsIndex == -1) {
        ParseLangScriptRegionAndVariants(localeId, outputArray);
    }
    else {
        ParseLangScriptRegionAndVariants(localeId.Substring(0, extensionsIndex), outputArray);
    }
    AutoPtr<ILocaleBuilder> builder;
    CLocaleBuilder::New(IID_ILocaleBuilder, (IInterface**)&builder);
    builder->SetLanguage(outputArray[IDX_LANGUAGE]);
    builder->SetRegion(outputArray[IDX_REGION]);
    builder->SetVariant(outputArray[IDX_VARIANT]);
    builder->SetScript(outputArray[IDX_SCRIPT]);
    FOR_EACH(ICharSequence*, attribute, ICharSequence::Probe, unicodeAttributeSet) {
        builder->AddUnicodeLocaleAttribute(CoreUtils::Unbox(attribute));
    } END_FOR_EACH();
    AutoPtr<ISet> entries;
    unicodeKeywordsMap->GetEntrySet(entries);
    FOR_EACH(IMapEntry*, keyword, IMapEntry::Probe, entries) {
        AutoPtr<IInterface> key, value;
        keyword->GetKey(key);
        keyword->GetValue(value);
        builder->SetUnicodeLocaleKeyword(
                CoreUtils::Unbox(ICharSequence::Probe(key)),
                CoreUtils::Unbox(ICharSequence::Probe(value)));
    } END_FOR_EACH();

    extensionsMap->GetEntrySet(entries);
    FOR_EACH(IMapEntry*, extension, IMapEntry::Probe, entries) {
        AutoPtr<IInterface> key, value;
        extension->GetKey(key);
        extension->GetValue(value);
        builder->SetExtension(
                CoreUtils::Unbox(IChar::Probe(key)),
                CoreUtils::Unbox(ICharSequence::Probe(value)));
    } END_FOR_EACH();

    AutoPtr<ILocale> locale;
    builder->Build(locale);
    return locale;
}

Array<ILocale*> ICU::LocalesFromStrings(
    /* [in] */ const Array<String>& localeNames)
{
    AutoPtr<ISet> set;
    CLinkedHashSet::New(IID_ISet, (IInterface**)&set);
    for (String localeName : localeNames) {
        set->Add(LocaleFromIcuLocaleId(localeName));
    }
    Array<ILocale*> locales;
    set->ToArray(IID_ILocale, (Array<IInterface*>*)&locales);
    return locales;
}

Array<ILocale*> ICU::GetAvailableLocales()
{
    if (sAvailableLocalesCache.IsNull()) {
        sAvailableLocalesCache = LocalesFromStrings(ToStringArray(uloc_countAvailable, uloc_getAvailable));
    }
    return sAvailableLocalesCache.Clone();
}

String ICU::GetBestDateTimePattern(
    /* [in] */ const String& skeleton,
    /* [in] */ ILocale* locale)
{
    String languageTag;
    locale->ToLanguageTag(languageTag);
    String key = skeleton + "\t" + languageTag;
    AutoPtr<BasicLruCache> cache = GetCACHED_PATTERNS();
    {
        AutoLock lock(cache);

        AutoPtr<IInterface> value;
        cache->Get(CoreUtils::Box(key), value);
        AutoPtr<ICharSequence> pattern = std::move(value);
        if (pattern == nullptr) {
            pattern = CoreUtils::Box(GetBestDateTimePattern(skeleton, languageTag));
            cache->Put(CoreUtils::Box(key), pattern);
        }
        return CoreUtils::Unbox(pattern);
    }
}

String ICU::GetBestDateTimePattern(
    /* [in] */ const String& skeleton,
    /* [in] */ const String& languageTag)
{
    if (languageTag.IsNull() || skeleton.IsNull()) {
        return String();
    }

    ::icu::Locale icuLocale;
    icuLocale.setToBogus();

    icuLocale = ::icu::Locale::createFromName(languageTag.string());
    if (icuLocale.isBogus()) {
        return String();
    }

    UErrorCode status = U_ZERO_ERROR;
    std::unique_ptr<::icu::DateTimePatternGenerator> generator(::icu::DateTimePatternGenerator::createInstance(icuLocale, status));
    if (FAILED(MaybeThrowIcuException("DateTimePatternGenerator::createInstance", status))) {
        return String();
    }

    ::icu::UnicodeString skeletonHolder(skeleton.string(), skeleton.GetByteLength());
    ::icu::UnicodeString result(generator->getBestPattern(skeletonHolder, status));
    if (FAILED(MaybeThrowIcuException("DateTimePatternGenerator::getBestPattern", status))) {
        return String();
    }

    return ToUTF8String(result);
}

Array<String> ICU::GetAvailableCurrencyCodes()
{
    UErrorCode status = U_ZERO_ERROR;
    UStringEnumeration e(ucurr_openISOCurrencies(UCURR_COMMON|UCURR_NON_DEPRECATED, &status));
    return FromStringEnumeration(status, "ucurr_openISOCurrencies", &e);
}

String ICU::GetCurrencyCode(
    /* [in] */ const String& countryCode)
{
    UErrorCode status = U_ZERO_ERROR;
    ScopedResourceBundle supplData(ures_openDirect(U_ICUDATA_CURR, "supplementalData", &status));
    if (U_FAILURE(status)) {
        return String();
    }

    ScopedResourceBundle currencyMap(ures_getByKey(supplData.get(), "CurrencyMap", nullptr, &status));
    if (U_FAILURE(status)) {
        return String();
    }

    ScopedResourceBundle currency(ures_getByKey(currencyMap.get(), countryCode.string(), nullptr, &status));
    if (U_FAILURE(status)) {
        return String();
    }

    ScopedResourceBundle currencyElem(ures_getByIndex(currency.get(), 0, nullptr, &status));
    if (U_FAILURE(status)) {
        return String("XXX");
    }

    // Check if there's a 'to' date. If there is, the currency isn't used anymore.
    ScopedResourceBundle currencyTo(ures_getByKey(currencyElem.get(), "to", nullptr, &status));
    if (!U_FAILURE(status)) {
        return String();
    }
    // Ignore the failure to find a 'to' date.
    status = U_ZERO_ERROR;

    ScopedResourceBundle currencyId(ures_getByKey(currencyElem.get(), "id", nullptr, &status));
    if (U_FAILURE(status)) {
        // No id defined for this country
        return String("XXX");
    }

    int32_t charCount;
    const UChar* chars = ures_getString(currencyId.get(), &charCount, &status);
    return (charCount == 0) ? String("XXX") : ToUTF8String(::icu::UnicodeString(chars, charCount));
}

String ICU::GetCurrencyDisplayName(
    /* [in] */ ILocale* locale,
    /* [in] */ const String& currencyCode)
{
    String languageTag;
    locale->ToLanguageTag(languageTag);
    return GetCurrencyDisplayName(languageTag, currencyCode);
}

String ICU::GetCurrencyDisplayName(
    /* [in] */ const String& languageTag,
    /* [in] */ const String& currencyCode)
{
    return GetCurrencyName(languageTag, currencyCode, UCURR_LONG_NAME);
}

Integer ICU::GetCurrencyFractionDigits(
    /* [in] */ const String& currencyCode)
{
    if (currencyCode.IsNull()) {
        return 0;
    }
    ::icu::UnicodeString icuCurrencyCode(currencyCode.string(), currencyCode.GetByteLength());
    UErrorCode status = U_ZERO_ERROR;
    return ucurr_getDefaultFractionDigits(icuCurrencyCode.getTerminatedBuffer(), &status);
}

Integer ICU::GetCurrencyNumericCode(
    /* [in] */ const String& currencyCode)
{
    if (currencyCode.IsNull()) {
        return 0;
    }
    ::icu::UnicodeString icuCurrencyCode(currencyCode.string(), currencyCode.GetByteLength());
    return ucurr_getNumericCode(icuCurrencyCode.getTerminatedBuffer());
}

String ICU::GetCurrencySymbol(
    /* [in] */ ILocale* locale,
    /* [in] */ const String& currencyCode)
{
    String languageTag;
    locale->ToLanguageTag(languageTag);
    return GetCurrencySymbol(languageTag, currencyCode);
}

String ICU::GetCurrencySymbol(
    /* [in] */ const String& languageTag,
    /* [in] */ const String& currencyCode)
{
    return GetCurrencyName(languageTag, currencyCode, UCURR_SYMBOL_NAME);
}

String ICU::GetDisplayCountry(
    /* [in] */ ILocale* targetLocale,
    /* [in] */ ILocale* locale)
{
    String targetLanguageTag, languageTag;
    targetLocale->ToLanguageTag(targetLanguageTag);
    locale->ToLanguageTag(languageTag);

    if (languageTag.IsNull()) {
        return String();
    }

    ::icu::Locale icuLocale;
    icuLocale.setToBogus();

    icuLocale = ::icu::Locale::createFromName(languageTag.string());
    if (icuLocale.isBogus()) {
        return String();
    }

    ::icu::Locale icuTargetLocale;
    icuTargetLocale.setToBogus();

    icuTargetLocale = ::icu::Locale::createFromName(targetLanguageTag.string());
    if (icuTargetLocale.isBogus()) {
        return String();
    }

    ::icu::UnicodeString str;
    icuTargetLocale.getDisplayCountry(icuLocale, str);
    return ToUTF8String(str);
}

String ICU::GetDisplayLanguage(
    /* [in] */ ILocale* targetLocale,
    /* [in] */ ILocale* locale)
{
    String targetLanguageTag, languageTag;
    targetLocale->ToLanguageTag(targetLanguageTag);
    locale->ToLanguageTag(languageTag);

    if (languageTag.IsNull()) {
        return String();
    }

    ::icu::Locale icuLocale;
    icuLocale.setToBogus();

    icuLocale = ::icu::Locale::createFromName(languageTag.string());
    if (icuLocale.isBogus()) {
        return String();
    }

    ::icu::Locale icuTargetLocale;
    icuTargetLocale.setToBogus();

    icuTargetLocale = ::icu::Locale::createFromName(targetLanguageTag.string());
    if (icuTargetLocale.isBogus()) {
        return String();
    }

    ::icu::UnicodeString str;
    icuTargetLocale.getDisplayLanguage(icuLocale, str);
    return ToUTF8String(str);
}

String ICU::GetDisplayVariant(
    /* [in] */ ILocale* targetLocale,
    /* [in] */ ILocale* locale)
{
    String targetLanguageTag, languageTag;
    targetLocale->ToLanguageTag(targetLanguageTag);
    locale->ToLanguageTag(languageTag);

    if (languageTag.IsNull()) {
        return String();
    }

    ::icu::Locale icuLocale;
    icuLocale.setToBogus();

    icuLocale = ::icu::Locale::createFromName(languageTag.string());
    if (icuLocale.isBogus()) {
        return String();
    }

    ::icu::Locale icuTargetLocale;
    icuTargetLocale.setToBogus();

    icuTargetLocale = ::icu::Locale::createFromName(targetLanguageTag.string());
    if (icuTargetLocale.isBogus()) {
        return String();
    }

    ::icu::UnicodeString str;
    icuTargetLocale.getDisplayVariant(icuLocale, str);
    return ToUTF8String(str);
}

String ICU::GetDisplayScript(
    /* [in] */ ILocale* targetLocale,
    /* [in] */ ILocale* locale)
{
    String targetLanguageTag, languageTag;
    targetLocale->ToLanguageTag(targetLanguageTag);
    locale->ToLanguageTag(languageTag);

    if (languageTag.IsNull()) {
        return String();
    }

    ::icu::Locale icuLocale;
    icuLocale.setToBogus();

    icuLocale = ::icu::Locale::createFromName(languageTag.string());
    if (icuLocale.isBogus()) {
        return String();
    }

    ::icu::Locale icuTargetLocale;
    icuTargetLocale.setToBogus();

    icuTargetLocale = ::icu::Locale::createFromName(targetLanguageTag.string());
    if (icuTargetLocale.isBogus()) {
        return String();
    }

    ::icu::UnicodeString str;
    icuTargetLocale.getDisplayScript(icuLocale, str);
    return ToUTF8String(str);
}

String ICU::GetISO3Country(
    /* [in] */ const String& languageTag)
{
    if (languageTag.IsNull()) {
        return String();
    }

    ::icu::Locale locale;
    locale.setToBogus();

    locale = ::icu::Locale::createFromName(languageTag.string());
    if (locale.isBogus()) {
        return String();
    }

    return String(locale.getISO3Country());
}

String ICU::GetISO3Language(
    /* [in] */ const String& languageTag)
{
    if (languageTag.IsNull()) {
        return String();
    }

    ::icu::Locale locale;
    locale.setToBogus();

    locale = ::icu::Locale::createFromName(languageTag.string());
    if (locale.isBogus()) {
        return String();
    }

    return String(locale.getISO3Language());
}

Boolean ICU::InitLocaleData(
    /* [in] */ const String& languageTag,
    /* [in] */ LocaleData* result)
{
    if (languageTag.IsNull()) {
        return false;
    }
    if (languageTag.GetLength() >= ULOC_FULLNAME_CAPACITY) {
        return false;
    }

    ::icu::Locale icuLocale = ::icu::Locale::createFromName(languageTag.string());
    if (icuLocale.isBogus()) {
        return false;
    }

    // Get the DateTimePatterns.
    UErrorCode status = U_ZERO_ERROR;
    Boolean foundDateTimePatterns = false;
    for (LocaleNameIterator it(icuLocale.getBaseName(), status); it.HasNext(); it.Up()) {
        if (GetDateTimePatterns(result, it.Get())) {
            foundDateTimePatterns = true;
            break;
        }
    }
    if (!foundDateTimePatterns) {
        Logger::E("ICU", "Couldn't find ICU DateTimePatterns for %s", languageTag.string());
        return false;
    }

    // Get the "Yesterday", "Today", and "Tomorrow" strings.
    Boolean foundYesterdayTodayAndTomorrow = false;
    for (LocaleNameIterator it(icuLocale.getBaseName(), status); it.HasNext(); it.Up()) {
        if (GetYesterdayTodayAndTomorrow(result, icuLocale, it.Get())) {
            foundYesterdayTodayAndTomorrow = true;
            break;
        }
    }
    if (!foundYesterdayTodayAndTomorrow) {
        Logger::E("ICU", "Couldn't find ICU yesterday/today/tomorrow for %s", languageTag.string());
        return false;
    }

#ifdef __android__
    // Get the narrow "AM" and "PM" strings.
    Boolean foundAmPmMarkersNarrow = false;
    for (LocaleNameIterator it(icuLocale.getBaseName(), status); it.HasNext(); it.Up()) {
        if (GetAmPmMarkersNarrow(result, it.Get())) {
            foundAmPmMarkersNarrow = true;
            break;
        }
    }
    if (!foundAmPmMarkersNarrow) {
        Logger::E("ICU", "Couldn't find ICU AmPmMarkersNarrow for %s", languageTag.string());
        return false;
    }
#endif

    status = U_ZERO_ERROR;
    std::unique_ptr<::icu::Calendar> cal(::icu::Calendar::createInstance(icuLocale, status));
    if (U_FAILURE(status)) {
        return false;
    }
    AutoPtr<IInteger> intObj;
    CInteger::New(cal->getFirstDayOfWeek(), IID_IInteger, (IInterface**)&intObj);
    result->mFirstDayOfWeek = std::move(intObj);
    CInteger::New(cal->getMinimalDaysInFirstWeek(), IID_IInteger, (IInterface**)&intObj);
    result->mMinimalDaysInFirstWeek = std::move(intObj);

    // Get DateFormatSymbols.
    status = U_ZERO_ERROR;
    ::icu::DateFormatSymbols dateFormatSym(icuLocale, status);
    if (U_FAILURE(status)) {
        return false;
    }

    // Get AM/PM and BC/AD.
    int32_t count = 0;
    const ::icu::UnicodeString* amPmStrs = dateFormatSym.getAmPmStrings(count);
    result->mAmPm = GetStringArrayField("amPm", amPmStrs, count);
    const ::icu::UnicodeString* erasStrs = dateFormatSym.getEras(count);
    result->mEras = GetStringArrayField("eras", erasStrs, count);


    const ::icu::UnicodeString* longMonthNames =
            dateFormatSym.getMonths(count, ::icu::DateFormatSymbols::FORMAT, ::icu::DateFormatSymbols::WIDE);
    result->mLongMonthNames = GetStringArrayField("longMonthNames", longMonthNames, count);
    const ::icu::UnicodeString* shortMonthNames =
            dateFormatSym.getMonths(count, ::icu::DateFormatSymbols::FORMAT, ::icu::DateFormatSymbols::ABBREVIATED);
    result->mShortMonthNames = GetStringArrayField("shortMonthNames", shortMonthNames, count);
    const ::icu::UnicodeString* tinyMonthNames =
            dateFormatSym.getMonths(count, ::icu::DateFormatSymbols::FORMAT, ::icu::DateFormatSymbols::NARROW);
    result->mTinyMonthNames = GetStringArrayField("tinyMonthNames", tinyMonthNames, count);
    const ::icu::UnicodeString* longWeekdayNames =
            dateFormatSym.getWeekdays(count, ::icu::DateFormatSymbols::FORMAT, ::icu::DateFormatSymbols::WIDE);
    result->mLongWeekdayNames = GetStringArrayField("longWeekdayNames", longWeekdayNames, count);
    const ::icu::UnicodeString* shortWeekdayNames =
            dateFormatSym.getWeekdays(count, ::icu::DateFormatSymbols::FORMAT, ::icu::DateFormatSymbols::ABBREVIATED);
    result->mShortWeekdayNames = GetStringArrayField("shortWeekdayNames", shortWeekdayNames, count);
    const ::icu::UnicodeString* tinyWeekdayNames =
            dateFormatSym.getWeekdays(count, ::icu::DateFormatSymbols::FORMAT, ::icu::DateFormatSymbols::NARROW);
    result->mTinyWeekdayNames = GetStringArrayField("tinyWeekdayNames", tinyWeekdayNames, count);

    const ::icu::UnicodeString* longStandAloneMonthNames =
            dateFormatSym.getMonths(count, ::icu::DateFormatSymbols::STANDALONE, ::icu::DateFormatSymbols::WIDE);
    result->mLongStandAloneMonthNames = GetStringArrayField("longStandAloneMonthNames", longStandAloneMonthNames, count);
    const ::icu::UnicodeString* shortStandAloneMonthNames =
            dateFormatSym.getMonths(count, ::icu::DateFormatSymbols::STANDALONE, ::icu::DateFormatSymbols::ABBREVIATED);
    result->mShortStandAloneMonthNames = GetStringArrayField("shortStandAloneMonthNames", shortStandAloneMonthNames, count);
    const ::icu::UnicodeString* tinyStandAloneMonthNames =
            dateFormatSym.getMonths(count, ::icu::DateFormatSymbols::STANDALONE, ::icu::DateFormatSymbols::NARROW);
    result->mTinyStandAloneMonthNames = GetStringArrayField("tinyStandAloneMonthNames", tinyStandAloneMonthNames, count);
    const ::icu::UnicodeString* longStandAloneWeekdayNames =
            dateFormatSym.getWeekdays(count, ::icu::DateFormatSymbols::STANDALONE, ::icu::DateFormatSymbols::WIDE);
    result->mLongStandAloneWeekdayNames = GetStringArrayField("longStandAloneWeekdayNames", longStandAloneWeekdayNames, count);
    const ::icu::UnicodeString* shortStandAloneWeekdayNames =
            dateFormatSym.getWeekdays(count, ::icu::DateFormatSymbols::STANDALONE, ::icu::DateFormatSymbols::ABBREVIATED);
    result->mShortStandAloneWeekdayNames = GetStringArrayField("shortStandAloneWeekdayNames", shortStandAloneWeekdayNames, count);
    const ::icu::UnicodeString* tinyStandAloneWeekdayNames =
            dateFormatSym.getWeekdays(count, ::icu::DateFormatSymbols::STANDALONE, ::icu::DateFormatSymbols::NARROW);
    result->mTinyStandAloneWeekdayNames = GetStringArrayField("tinyStandAloneWeekdayNames", tinyStandAloneWeekdayNames, count);

    status = U_ZERO_ERROR;

    // For numberPatterns and symbols.
    SetNumberPatterns(result, icuLocale);
    SetDecimalFormatSymbolsData(result, icuLocale);

    String countryCode = ToUTF8String(icuLocale.getCountry());
    String internationalCurrencySymbol = ICU_getCurrencyCode(countryCode);

    String currencySymbol;
    if (!internationalCurrencySymbol.IsNull()){
        currencySymbol = GetCurrencySymbol(languageTag, internationalCurrencySymbol);
    }
    else {
        internationalCurrencySymbol = "XXX";
    }
    if (currencySymbol.IsNull()) {
        // This is the UTF-8 encoding of U+00A4 (CURRENCY SIGN).
        currencySymbol = String(Array<Char>{ 0x00C2, 0x00A4 });
    }
    result->mCurrencySymbol = currencySymbol;
    result->mInternationalCurrencySymbol = internationalCurrencySymbol;

    return true;
}

ECode ICU::SetDefaultLocale(
    /* [in] */ const String& languageTag)
{
    if (languageTag.IsNull()) {
        return NOERROR;
    }

    ::icu::Locale locale;
    locale.setToBogus();

    locale = ::icu::Locale::createFromName(languageTag.string());
    if (locale.isBogus()) {
        return NOERROR;
    }

    UErrorCode status = U_ZERO_ERROR;
    ::icu::Locale::setDefault(locale, status);
    return MaybeThrowIcuException("ICU::SetDefaultLocale", status);
}

}
}
