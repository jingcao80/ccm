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

#ifndef __LIBCORE_ICU_NATIVEDECIMALFORMAT_H__
#define __LIBCORE_ICU_NATIVEDECIMALFORMAT_H__

#include "como/core/SyncObject.h"
#include "como.core.INumber.h"
#include "como.math.IBigDecimal.h"
#include "como.math.IBigInteger.h"
#include "como.text.IAttributedCharacterIterator.h"
#include "como.text.IDecimalFormatSymbols.h"
#include "como.text.IFieldPosition.h"
#include "como.text.INumberFormatField.h"
#include "como.text.IParsePosition.h"

using como::core::INumber;
using como::core::SyncObject;
using como::math::IBigDecimal;
using como::math::IBigInteger;
using como::math::RoundingMode;
using como::text::IAttributedCharacterIterator;
using como::text::IDecimalFormatSymbols;
using como::text::IFieldPosition;
using como::text::INumberFormatField;
using como::text::IParsePosition;

namespace libcore {
namespace icu {

class NativeDecimalFormat
    : public SyncObject
{
public:
    // Utility to get information about field positions from native (ICU) code.
    class FieldPositionIterator
        : public Object
    {
    public:
        static AutoPtr<FieldPositionIterator> ForFieldPosition(
            /* [in] */ IFieldPosition* fp);

        Boolean Next();

        Integer FieldId();

        AutoPtr<IAttributedCharacterIterator::IAttribute> Field();

        Integer Start();

        Integer Limit();

        // called by native
        void SetData(
            /* [in] */ Array<Integer>& data);

    private:
        friend class NativeDecimalFormat;

        FieldPositionIterator() {}

    private:
        Array<Integer> mData;
        Integer mPos = -3; // so first call to next() leaves pos at 0
    };

public:
    ~NativeDecimalFormat();

    ECode Constructor(
        /* [in] */ const String& pattern,
        /* [in] */ IDecimalFormatSymbols* dfs);

    void Close();

    ECode SetDecimalFormatSymbols(
        /* [in] */ IDecimalFormatSymbols* dfs);

    ECode FormatBigDecimal(
        /* [in] */ IBigDecimal* value,
        /* [in] */ IFieldPosition* field,
        /* [out, callee] */ Array<Char>* result);

    ECode FormatBigInteger(
        /* [in] */ IBigInteger* value,
        /* [in] */ IFieldPosition* field,
        /* [out, callee] */ Array<Char>* result);

    ECode FormatLong(
        /* [in] */ Long value,
        /* [in] */ IFieldPosition* field,
        /* [out, callee] */ Array<Char>* result);

    ECode FormatDouble(
        /* [in] */ Double value,
        /* [in] */ IFieldPosition* field,
        /* [out, callee] */ Array<Char>* result);

    ECode ApplyLocalizedPattern(
        /* [in] */ const String& pattern);

    ECode ApplyPattern(
        /* [in] */ const String& pattern);

    ECode FormatToCharacterIterator(
        /* [in] */ IInterface* object,
        /* [out] */ AutoPtr<IAttributedCharacterIterator>& it);

    ECode ToLocalizedPattern(
        /* [out] */ String* pattern);

    ECode ToPattern(
        /* [out] */ String* pattern);

    ECode Parse(
        /* [in] */ const String& string,
        /* [in] */ IParsePosition* position,
        /* [out] */ INumber** number);

    Integer GetMaximumFractionDigits();

    Integer GetMaximumIntegerDigits();

    Integer GetMinimumFractionDigits();

    Integer GetMinimumIntegerDigits();

    Integer GetGroupingSize();

    Integer GetMultiplier();

    ECode GetNegativePrefix(
        /* [out] */ String* prefix);

    ECode GetNegativeSuffix(
        /* [out] */ String* suffix);

    ECode GetPositivePrefix(
        /* [out] */ String* prefix);

    ECode GetPositiveSuffix(
        /* [out] */ String* suffix);

    Boolean IsDecimalSeparatorAlwaysShown();

    Boolean IsParseBigDecimal();

    Boolean IsParseIntegerOnly();

    Boolean IsGroupingUsed();

    ECode SetDecimalSeparatorAlwaysShown(
        /* [in] */ Boolean shown);

    ECode SetCurrency(
        /* [in] */ const String& currencySymbol,
        /* [in] */ const String& currencyCode);

    ECode SetGroupingSize(
        /* [in] */ Integer size);

    ECode SetGroupingUsed(
        /* [in] */ Boolean used);

    ECode SetMaximumFractionDigits(
        /* [in] */ Integer value);

    ECode SetMaximumIntegerDigits(
        /* [in] */ Integer value);

    ECode SetMinimumFractionDigits(
        /* [in] */ Integer value);

    ECode SetMinimumIntegerDigits(
        /* [in] */ Integer value);

    ECode SetMultiplier(
        /* [in] */ Integer multiplier);

    ECode SetNegativePrefix(
        /* [in] */ const String& prefix);

    ECode SetNegativeSuffix(
        /* [in] */ const String& suffix);

    ECode SetPositivePrefix(
        /* [in] */ const String& prefix);

    ECode SetPositiveSuffix(
        /* [in] */ const String& suffix);

    ECode SetParseBigDecimal(
        /* [in] */ Boolean value);

    ECode SetParseIntegerOnly(
        /* [in] */ Boolean value);

    ECode SetRoundingMode(
        /* [in] */ RoundingMode mode,
        /* [in] */ Double roundingIncrement);

    Boolean Equals(
        /* [in] */ NativeDecimalFormat* other);

    NativeDecimalFormat* Clone();

private:
    void UpdateFieldPosition(
        /* [in] */ IFieldPosition* fp,
        /* [in] */ FieldPositionIterator* fpi);

    static ECode ApplyPattern(
        /* [in] */ HANDLE addr,
        /* [in] */ Boolean localized,
        /* [in] */ const String& pattern);

    static ECode ApplyPatternImpl(
        /* [in] */ HANDLE addr,
        /* [in] */ Boolean localized,
        /* [in] */ const String& pattern);

    static ECode FormatLong(
        /* [in] */ HANDLE addr,
        /* [in] */ Long value,
        /* [in] */ FieldPositionIterator* fieldPositionIterator,
        /* [out, callee] */ Array<Char>* result);

    static ECode FormatDouble(
        /* [in] */ HANDLE addr,
        /* [in] */ Double value,
        /* [in] */ FieldPositionIterator* fieldPositionIterator,
        /* [out, callee] */ Array<Char>* result);

    static ECode FormatDigitList(
        /* [in] */ HANDLE addr,
        /* [in] */ const String& value,
        /* [in] */ FieldPositionIterator* fieldPositionIterator,
        /* [out, callee] */ Array<Char>* result);

    static Integer GetAttribute(
        /* [in] */ HANDLE addr,
        /* [in] */ Integer symbol);

    static ECode GetTextAttribute(
        /* [in] */ HANDLE addr,
        /* [in] */ Integer symbol,
        /* [out] */ String* textAttr);

    static ECode Open(
        /* [in] */ const String& pattern,
        /* [in] */ const String& currencySymbol,
        /* [in] */ Char decimalSeparator,
        /* [in] */ Char digit,
        /* [in] */ const String& exponentSeparator,
        /* [in] */ Char groupingSeparator,
        /* [in] */ const String& infinity,
        /* [in] */ const String& internationalCurrencySymbol,
        /* [in] */ const String& minusSign,
        /* [in] */ Char monetaryDecimalSeparator,
        /* [in] */ const String& nan,
        /* [in] */ Char patternSeparator,
        /* [in] */ Char percent,
        /* [in] */ Char perMill,
        /* [in] */ Char zeroDigit,
        /* [out] */ HANDLE* result);

    static AutoPtr<INumber> Parse(
        /* [in] */ HANDLE addr,
        /* [in] */ const String& text,
        /* [in] */ IParsePosition* position,
        /* [in] */ Boolean parseBigDecimal);

    static void SetDecimalFormatSymbols(
        /* [in] */ HANDLE addr,
        /* [in] */ const String& currencySymbol,
        /* [in] */ Char decimalSeparator,
        /* [in] */ Char digit,
        /* [in] */ const String& exponentSeparator,
        /* [in] */ Char groupingSeparator,
        /* [in] */ const String& infinity,
        /* [in] */ const String& internationalCurrencySymbol,
        /* [in] */ const String& minusSign,
        /* [in] */ Char monetaryDecimalSeparator,
        /* [in] */ const String& nan,
        /* [in] */ Char patternSeparator,
        /* [in] */ Char percent,
        /* [in] */ Char perMill,
        /* [in] */ Char zeroDigit);

    static ECode SetSymbol(
        /* [in] */ HANDLE addr,
        /* [in] */ Integer symbol,
        /* [in] */ const String& str);

    static void SetAttribute(
        /* [in] */ HANDLE addr,
        /* [in] */ Integer symbol,
        /* [in] */ Integer i);

    static void SetRoundingMode(
        /* [in] */ HANDLE addr,
        /* [in] */ Integer mode,
        /* [in] */ Double increment);

    static ECode SetTextAttribute(
        /* [in] */ HANDLE addr,
        /* [in] */ Integer symbol,
        /* [in] */ const String& str);

    static String ToPatternImpl(
        /* [in] */ HANDLE addr,
        /* [in] */ Boolean localized);

    static Integer TranslateFieldId(
        /* [in] */ IFieldPosition* fp);

    static const Array<INumberFormatField*>& Get_ICU4C_FIELD_IDS();

private:
    /**
     * Constants corresponding to the native type UNumberFormatSymbol, for setSymbol.
     */
    static constexpr Integer UNUM_DECIMAL_SEPARATOR_SYMBOL = 0;
    static constexpr Integer UNUM_GROUPING_SEPARATOR_SYMBOL = 1;
    static constexpr Integer UNUM_PATTERN_SEPARATOR_SYMBOL = 2;
    static constexpr Integer UNUM_PERCENT_SYMBOL = 3;
    static constexpr Integer UNUM_ZERO_DIGIT_SYMBOL = 4;
    static constexpr Integer UNUM_DIGIT_SYMBOL = 5;
    static constexpr Integer UNUM_MINUS_SIGN_SYMBOL = 6;
    static constexpr Integer UNUM_PLUS_SIGN_SYMBOL = 7;
    static constexpr Integer UNUM_CURRENCY_SYMBOL = 8;
    static constexpr Integer UNUM_INTL_CURRENCY_SYMBOL = 9;
    static constexpr Integer UNUM_MONETARY_SEPARATOR_SYMBOL = 10;
    static constexpr Integer UNUM_EXPONENTIAL_SYMBOL = 11;
    static constexpr Integer UNUM_PERMILL_SYMBOL = 12;
    static constexpr Integer UNUM_PAD_ESCAPE_SYMBOL = 13;
    static constexpr Integer UNUM_INFINITY_SYMBOL = 14;
    static constexpr Integer UNUM_NAN_SYMBOL = 15;
    static constexpr Integer UNUM_SIGNIFICANT_DIGIT_SYMBOL = 16;
    static constexpr Integer UNUM_MONETARY_GROUPING_SEPARATOR_SYMBOL = 17;
    static constexpr Integer UNUM_FORMAT_SYMBOL_COUNT = 18;

    /**
     * Constants corresponding to the native type UNumberFormatAttribute, for
     * getAttribute/setAttribute.
     */
    static constexpr Integer UNUM_PARSE_INT_ONLY = 0;
    static constexpr Integer UNUM_GROUPING_USED = 1;
    static constexpr Integer UNUM_DECIMAL_ALWAYS_SHOWN = 2;
    static constexpr Integer UNUM_MAX_INTEGER_DIGITS = 3;
    static constexpr Integer UNUM_MIN_INTEGER_DIGITS = 4;
    static constexpr Integer UNUM_INTEGER_DIGITS = 5;
    static constexpr Integer UNUM_MAX_FRACTION_DIGITS = 6;
    static constexpr Integer UNUM_MIN_FRACTION_DIGITS = 7;
    static constexpr Integer UNUM_FRACTION_DIGITS = 8;
    static constexpr Integer UNUM_MULTIPLIER = 9;
    static constexpr Integer UNUM_GROUPING_SIZE = 10;
    static constexpr Integer UNUM_ROUNDING_MODE = 11;
    static constexpr Integer UNUM_ROUNDING_INCREMENT = 12;
    static constexpr Integer UNUM_FORMAT_WIDTH = 13;
    static constexpr Integer UNUM_PADDING_POSITION = 14;
    static constexpr Integer UNUM_SECONDARY_GROUPING_SIZE = 15;
    static constexpr Integer UNUM_SIGNIFICANT_DIGITS_USED = 16;
    static constexpr Integer UNUM_MIN_SIGNIFICANT_DIGITS = 17;
    static constexpr Integer UNUM_MAX_SIGNIFICANT_DIGITS = 18;
    static constexpr Integer UNUM_LENIENT_PARSE = 19;

    /**
     * Constants corresponding to the native type UNumberFormatTextAttribute, for
     * getTextAttribute/setTextAttribute.
     */
    static constexpr Integer UNUM_POSITIVE_PREFIX = 0;
    static constexpr Integer UNUM_POSITIVE_SUFFIX = 1;
    static constexpr Integer UNUM_NEGATIVE_PREFIX = 2;
    static constexpr Integer UNUM_NEGATIVE_SUFFIX = 3;
    static constexpr Integer UNUM_PADDING_CHARACTER = 4;
    static constexpr Integer UNUM_CURRENCY_CODE = 5;
    static constexpr Integer UNUM_DEFAULT_RULESET = 6;
    static constexpr Integer UNUM_PUBLIC_RULESETS = 7;

    /**
     * The address of the ICU DecimalFormat* on the native heap.
     */
    HANDLE mAddress = 0;

    /**
     * The last pattern we gave to ICU, so we can make repeated applications cheap.
     * This helps in cases like String.format("%.2f,%.2f\n", x, y) where the DecimalFormat is
     * reused.
     */
    String mLastPattern;

    Boolean mNegPrefNull = false;
    Boolean mNegSuffNull = false;
    Boolean mPosPrefNull = false;
    Boolean mPosSuffNull = false;
    Boolean mParseBigDecimal = false;
};

}
}

#endif // __LIBCORE_ICU_NATIVEDECIMALFORMAT_H__
