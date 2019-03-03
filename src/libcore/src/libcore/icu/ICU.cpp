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

#include "ccm/core/CInteger.h"
#include "libcore/icu/ICU.h"
#include "libcore/icu/StringByteSink.h"
#include <memory>
#include <unicode/brkiter.h>
#include <unicode/calendar.h>
#include <unicode/dtfmtsym.h>
#include <unicode/decimfmt.h>
#include <unicode/locid.h>
#include <unicode/ucurr.h>
#include <unicode/uloc.h>
#include <unicode/ures.h>

#include <unicode/utypes.h>
#include <unicode/udata.h>


using ccm::core::CInteger;
using ccm::core::IID_IInteger;
using ccm::core::IInteger;

U_CDECL_BEGIN


#define U_ICUDATA_LANG U_ICUDATA_NAME U_TREE_SEPARATOR_STRING "lang"
#define U_ICUDATA_REGION U_ICUDATA_NAME U_TREE_SEPARATOR_STRING "region"
#define U_ICUDATA_CURR U_ICUDATA_NAME U_TREE_SEPARATOR_STRING "curr"
#define U_ICUDATA_ZONE U_ICUDATA_NAME U_TREE_SEPARATOR_STRING "zone"
#define U_ICUDATA_UNIT U_ICUDATA_NAME U_TREE_SEPARATOR_STRING "unit"

U_CDECL_END

namespace libcore {
namespace icu {

inline static Char ToChar(
    /* [in] */ const ::icu::UnicodeString& value)
{
    return value.length() == 0 ? 0 : (Char)value.charAt(0);
}

inline static String ToUTF8String(
    /* [in] */ const ::icu::UnicodeString& value)
{
    StringByteSink sink;
    value.toUTF8(sink);
    return sink.ToString();
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

static String ICU_getCurrencySymbol(
    /* [in] */ const String& languageTag,
    /* [in] */ const String& currencyCode)
{
    return GetCurrencyName(languageTag, currencyCode, UCURR_SYMBOL_NAME);
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
    const char* chars;
    UResourceBundle* currentBundle = ures_getByIndex(bundle, index, nullptr, &status);
    switch (ures_getType(currentBundle)) {
        case URES_STRING:
            chars = ures_getUTF8String(currentBundle, nullptr, &charCount, FALSE, &status);
            break;
        case URES_ARRAY:
            // In case there is an array, ccm currently only cares about the
            // first string of that array, the rest of the array is used by ICU
            // for additional data ignored by ccm.
            chars = ures_getUTF8StringByIndex(currentBundle, 0, nullptr, &charCount, FALSE, &status);
            break;
      default:
            status = U_INVALID_FORMAT_ERROR;
    }
    ures_close(currentBundle);
    if (U_SUCCESS(status)) {
        return String(chars, charCount);
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
        currencySymbol = ICU_getCurrencySymbol(languageTag, internationalCurrencySymbol);
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

}
}
