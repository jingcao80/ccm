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
#include "como/core/CDouble.h"
#include "como/core/CLong.h"
#include "como/math/CBigDecimal.h"
#include "como/text/CAttributedString.h"
#include "como/text/NumberFormat.h"
#include "como.core.IFloat.h"
#include "como.text.IFormatField.h"
#include "libcore/icu/NativeDecimalFormat.h"
#include <unicode/decimfmt.h>
#include <vector>

using como::core::AutoLock;
using como::core::CDouble;
using como::core::CLong;
using como::core::E_ASSERTION_ERROR;
using como::core::IDouble;
using como::core::IFloat;
using como::core::IID_INumber;
using como::math::CBigDecimal;
using como::text::CAttributedString;
using como::text::IAttributedString;
using como::text::IID_IAttributedString;
using como::text::IFormatField;

namespace libcore {
namespace icu {

extern ECode MaybeThrowIcuException(
    /* [in] */ const char* provider,
    /* [in] */ UErrorCode errorCode);

extern String ToUTF8String(
    /* [in] */ const ::icu::UnicodeString& value);

AutoPtr<NativeDecimalFormat::FieldPositionIterator>
NativeDecimalFormat::FieldPositionIterator::ForFieldPosition(
    /* [in] */ IFieldPosition* fp)
{
    return (fp != nullptr) ? new FieldPositionIterator() : nullptr;
}

Boolean NativeDecimalFormat::FieldPositionIterator::Next()
{
    if (mData.IsNull()) {
        return false;
    }
    mPos += 3;
    return mPos < mData.GetLength();
}

Integer NativeDecimalFormat::FieldPositionIterator::FieldId()
{
    return mData[mPos];
}

AutoPtr<IAttributedCharacterIterator::IAttribute> NativeDecimalFormat::FieldPositionIterator::Field()
{
    return IAttributedCharacterIterator::IAttribute::Probe(Get_ICU4C_FIELD_IDS()[mData[mPos]]);
}

Integer NativeDecimalFormat::FieldPositionIterator::Start()
{
    return mData[mPos + 1];
}

Integer NativeDecimalFormat::FieldPositionIterator::Limit()
{
    return mData[mPos + 2];
}

void NativeDecimalFormat::FieldPositionIterator::SetData(
    /* [in] */ Array<Integer>& data)
{
    mData = data;
    mPos = -3;
}

//------------------------------------------------------------------------------

inline static U_ICU_NAMESPACE::DecimalFormat* ToDecimalFormat(
    /* [in] */ HANDLE addr)
{
    return reinterpret_cast<U_ICU_NAMESPACE::DecimalFormat*>(addr);
}

inline static UNumberFormat* ToUNumberFormat(
    /* [in] */ HANDLE addr)
{
    return reinterpret_cast<UNumberFormat*>(addr);
}

static Array<INumberFormatField*> Create_ICU4C_FIELD_IDS()
{
    Array<INumberFormatField*> IDS = {
        como::text::NumberFormat::Field::GetINTEGER(),               //  0 UNUM_INTEGER_FIELD
        como::text::NumberFormat::Field::GetFRACTION(),              //  1 UNUM_FRACTION_FIELD
        como::text::NumberFormat::Field::GetDECIMAL_SEPARATOR(),     //  2 UNUM_DECIMAL_SEPARATOR_FIELD
        como::text::NumberFormat::Field::GetEXPONENT_SYMBOL(),       //  3 UNUM_EXPONENT_SYMBOL_FIELD
        como::text::NumberFormat::Field::GetEXPONENT_SIGN(),         //  4 UNUM_EXPONENT_SIGN_FIELD
        como::text::NumberFormat::Field::GetEXPONENT(),              //  5 UNUM_EXPONENT_FIELD
        como::text::NumberFormat::Field::GetGROUPING_SEPARATOR(),    //  6 UNUM_GROUPING_SEPARATOR_FIELD
        como::text::NumberFormat::Field::GetCURRENCY(),              //  7 UNUM_CURRENCY_FIELD
        como::text::NumberFormat::Field::GetPERCENT(),               //  8 UNUM_PERCENT_FIELD
        como::text::NumberFormat::Field::GetPERMILLE(),              //  9 UNUM_PERMILL_FIELD
        como::text::NumberFormat::Field::GetSIGN(),                  // 10 UNUM_SIGN_FIELD
    };
    return IDS;
}

const Array<INumberFormatField*>& NativeDecimalFormat::Get_ICU4C_FIELD_IDS()
{
    /**
     * A table for translating between NumberFormat.Field instances
     * and icu4c UNUM_x_FIELD constants.
     */
    static const Array<INumberFormatField*> ICU4C_FIELD_IDS = Create_ICU4C_FIELD_IDS();
    return ICU4C_FIELD_IDS;
}

NativeDecimalFormat::~NativeDecimalFormat()
{
    Close();
}

ECode NativeDecimalFormat::Constructor(
    /* [in] */ const String& pattern,
    /* [in] */ IDecimalFormatSymbols* dfs)
{
    String currencySymbol;
    dfs->GetCurrencySymbol(currencySymbol);
    Char decimalSeparator;
    dfs->GetDecimalSeparator(decimalSeparator);
    Char digit;
    dfs->GetDigit(digit);
    String exponentSeparator;
    dfs->GetExponentSeparator(exponentSeparator);
    Char groupingSeparator;
    dfs->GetGroupingSeparator(groupingSeparator);
    String infinity;
    dfs->GetInfinity(infinity);
    String internationalCurrencySymbol;
    dfs->GetInternationalCurrencySymbol(internationalCurrencySymbol);
    String minusSign;
    dfs->GetMinusSignString(minusSign);
    Char monetaryDecimalSeparator;
    dfs->GetMonetaryDecimalSeparator(monetaryDecimalSeparator);
    String naN;
    dfs->GetNaN(naN);
    Char patternSeparator;
    dfs->GetPatternSeparator(patternSeparator);
    Char percent;
    dfs->GetPercent(percent);
    Char perMill;
    dfs->GetPerMill(perMill);
    Char zeroDigit;
    dfs->GetZeroDigit(zeroDigit);
    ECode ec = Open(pattern, currencySymbol,
            decimalSeparator, digit, exponentSeparator,
            groupingSeparator, infinity,
            internationalCurrencySymbol, minusSign,
            monetaryDecimalSeparator, naN, patternSeparator,
            percent, perMill, zeroDigit, &mAddress);
    if (FAILED(ec)) {
        return ec == E_RUNTIME_EXCEPTION ? E_ILLEGAL_ARGUMENT_EXCEPTION : ec;
    }
    mLastPattern = pattern;
    return NOERROR;
}

void NativeDecimalFormat::Close()
{
    AutoLock lock(this);

    if (mAddress != 0) {
        delete ToDecimalFormat(mAddress);
        mAddress = 0;
    }
}

ECode NativeDecimalFormat::SetDecimalFormatSymbols(
    /* [in] */ IDecimalFormatSymbols* dfs)
{
    String currencySymbol;
    dfs->GetCurrencySymbol(currencySymbol);
    Char decimalSeparator;
    dfs->GetDecimalSeparator(decimalSeparator);
    Char digit;
    dfs->GetDigit(digit);
    String exponentSeparator;
    dfs->GetExponentSeparator(exponentSeparator);
    Char groupingSeparator;
    dfs->GetGroupingSeparator(groupingSeparator);
    String infinity;
    dfs->GetInfinity(infinity);
    String internationalCurrencySymbol;
    dfs->GetInternationalCurrencySymbol(internationalCurrencySymbol);
    String minusSign;
    dfs->GetMinusSignString(minusSign);
    Char monetaryDecimalSeparator;
    dfs->GetMonetaryDecimalSeparator(monetaryDecimalSeparator);
    String NaN;
    dfs->GetNaN(NaN);
    Char patternSeparator;
    dfs->GetPatternSeparator(patternSeparator);
    Char percent;
    dfs->GetPercent(percent);
    Char perMill;
    dfs->GetPerMill(perMill);
    Char zeroDigit;
    dfs->GetZeroDigit(zeroDigit);
    SetDecimalFormatSymbols(mAddress, currencySymbol, decimalSeparator,
            digit, exponentSeparator, groupingSeparator,
            infinity, internationalCurrencySymbol, minusSign,
            monetaryDecimalSeparator, NaN, patternSeparator,
            percent, perMill, zeroDigit);
    return NOERROR;
}

ECode NativeDecimalFormat::FormatBigDecimal(
    /* [in] */ IBigDecimal* value,
    /* [in] */ IFieldPosition* field,
    /* [out, callee] */ Array<Char>* result)
{
    AutoPtr<FieldPositionIterator> fpi = FieldPositionIterator::ForFieldPosition(field);
    FAIL_RETURN(FormatDigitList(mAddress, Object::ToString(value), fpi, result));
    if (fpi != nullptr && field != nullptr) {
        UpdateFieldPosition(field, fpi);
    }
    return NOERROR;
}

ECode NativeDecimalFormat::FormatBigInteger(
    /* [in] */ IBigInteger* value,
    /* [in] */ IFieldPosition* field,
    /* [out, callee] */ Array<Char>* result)
{
    AutoPtr<FieldPositionIterator> fpi = FieldPositionIterator::ForFieldPosition(field);
    FAIL_RETURN(FormatDigitList(mAddress, Object::ToString(value), fpi, result));
    if (fpi != nullptr && field != nullptr) {
        UpdateFieldPosition(field, fpi);
    }
    return NOERROR;
}

ECode NativeDecimalFormat::FormatLong(
    /* [in] */ Long value,
    /* [in] */ IFieldPosition* field,
    /* [out, callee] */ Array<Char>* result)
{
    AutoPtr<FieldPositionIterator> fpi = FieldPositionIterator::ForFieldPosition(field);
    FAIL_RETURN(FormatLong(mAddress, value, fpi, result));
    if (fpi != nullptr && field != nullptr) {
        UpdateFieldPosition(field, fpi);
    }
    return NOERROR;
}

ECode NativeDecimalFormat::FormatDouble(
    /* [in] */ Double value,
    /* [in] */ IFieldPosition* field,
    /* [out, callee] */ Array<Char>* result)
{
    AutoPtr<FieldPositionIterator> fpi = FieldPositionIterator::ForFieldPosition(field);
    FAIL_RETURN(FormatDouble(mAddress, value, fpi, result));
    if (fpi != nullptr && field != nullptr) {
        UpdateFieldPosition(field, fpi);
    }
    return NOERROR;
}

void NativeDecimalFormat::UpdateFieldPosition(
    /* [in] */ IFieldPosition* fp,
    /* [in] */ FieldPositionIterator* fpi)
{
    Integer field = TranslateFieldId(fp);
    if (field != -1) {
        while (fpi->Next()) {
            Integer id = fpi->FieldId();
            if (id == field) {
                id = fpi->Start();
                fp->SetBeginIndex(id);
                id = fpi->Limit();
                fp->SetEndIndex(id);
                return;
            }
        }
    }
}

ECode NativeDecimalFormat::ApplyLocalizedPattern(
    /* [in] */ const String& pattern)
{
    FAIL_RETURN(ApplyPattern(mAddress, true, pattern));
    mLastPattern = nullptr;
    return NOERROR;
}

ECode NativeDecimalFormat::ApplyPattern(
    /* [in] */ const String& pattern)
{
    if (!mLastPattern.IsNull() && pattern.Equals(mLastPattern)) {
        return NOERROR;
    }
    FAIL_RETURN(ApplyPattern(mAddress, false, pattern));
    mLastPattern = pattern;
    return NOERROR;
}

ECode NativeDecimalFormat::FormatToCharacterIterator(
    /* [in] */ IInterface* object,
    /* [out] */ AutoPtr<IAttributedCharacterIterator>& it)
{
    INumber* number = INumber::Probe(object);
    if (number == nullptr) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<FieldPositionIterator> fpIter = new FieldPositionIterator();
    String text;
    if (IBigInteger::Probe(number) != nullptr ||
            IBigDecimal::Probe(number) != nullptr) {
        Array<Char> out;
        FAIL_RETURN(FormatDigitList(mAddress, Object::ToString(number), fpIter, &out));
        text = String::ValueOf(out);
    }
    else if (IDouble::Probe(number) != nullptr ||
            IFloat::Probe(number) != nullptr) {
        Double dv;
        number->DoubleValue(dv);
        Array<Char> out;
        FAIL_RETURN(FormatDouble(mAddress, dv, fpIter, &out));
        text = String::ValueOf(out);
    }
    else {
        Long lv;
        number->LongValue(lv);
        Array<Char> out;
        FAIL_RETURN(FormatLong(mAddress, lv, fpIter, &out));
        text = String::ValueOf(out);
    }

    AutoPtr<IAttributedString> as;
    CAttributedString::New(text, IID_IAttributedString, (IInterface**)&as);

    while (fpIter->Next()) {
        AutoPtr<IAttributedCharacterIterator::IAttribute> field = fpIter->Field();
        as->AddAttribute(field, field, fpIter->Start(), fpIter->Limit());
    }

    // return the CharacterIterator from StructAttributedString
    return as->GetIterator(it);
}

Integer NativeDecimalFormat::GetMaximumFractionDigits()
{
    return GetAttribute(mAddress, UNUM_MAX_FRACTION_DIGITS);
}

Integer NativeDecimalFormat::GetMaximumIntegerDigits()
{
    return GetAttribute(mAddress, UNUM_MAX_INTEGER_DIGITS);
}

Integer NativeDecimalFormat::GetMinimumFractionDigits()
{
    return GetAttribute(mAddress, UNUM_MIN_FRACTION_DIGITS);
}

Integer NativeDecimalFormat::GetMinimumIntegerDigits()
{
    return GetAttribute(mAddress, UNUM_MIN_INTEGER_DIGITS);
}

Integer NativeDecimalFormat::GetGroupingSize()
{
    // Work around http://bugs.icu-project.org/trac/ticket/10864 in icu4c 53.
    if (!IsGroupingUsed()) {
        return 0;
    }
    return GetAttribute(mAddress, UNUM_GROUPING_SIZE);
}

Integer NativeDecimalFormat::GetMultiplier()
{
    return GetAttribute(mAddress, UNUM_MULTIPLIER);
}

ECode NativeDecimalFormat::GetNegativePrefix(
    /* [out] */ String* prefix)
{
    if (mNegPrefNull) {
        *prefix = nullptr;
        return NOERROR;
    }
    return GetTextAttribute(mAddress, UNUM_NEGATIVE_PREFIX, prefix);
}

ECode NativeDecimalFormat::GetNegativeSuffix(
    /* [out] */ String* suffix)
{
    if (mNegSuffNull) {
        *suffix = nullptr;
        return NOERROR;
    }
    return GetTextAttribute(mAddress, UNUM_NEGATIVE_SUFFIX, suffix);
}

ECode NativeDecimalFormat::GetPositivePrefix(
    /* [out] */ String* prefix)
{
    if (mPosPrefNull) {
        *prefix = nullptr;
        return NOERROR;
    }
    return GetTextAttribute(mAddress, UNUM_POSITIVE_PREFIX, prefix);
}

ECode NativeDecimalFormat::GetPositiveSuffix(
    /* [out] */ String* suffix)
{
    if (mPosSuffNull) {
        *suffix = nullptr;
        return NOERROR;
    }
    return GetTextAttribute(mAddress, UNUM_POSITIVE_SUFFIX, suffix);
}

Boolean NativeDecimalFormat::IsDecimalSeparatorAlwaysShown()
{
    return GetAttribute(mAddress, UNUM_DECIMAL_ALWAYS_SHOWN) != 0;
}

Boolean NativeDecimalFormat::IsParseBigDecimal()
{
    return mParseBigDecimal;
}

Boolean NativeDecimalFormat::IsParseIntegerOnly()
{
    return GetAttribute(mAddress, UNUM_PARSE_INT_ONLY) != 0;
}

Boolean NativeDecimalFormat::IsGroupingUsed()
{
    return GetAttribute(mAddress, UNUM_GROUPING_USED) != 0;
}

ECode NativeDecimalFormat::SetDecimalSeparatorAlwaysShown(
    /* [in] */ Boolean shown)
{
    SetAttribute(mAddress, UNUM_DECIMAL_ALWAYS_SHOWN, shown ? -1 : 0);
    return NOERROR;
}

ECode NativeDecimalFormat::SetCurrency(
    /* [in] */ const String& currencySymbol,
    /* [in] */ const String& currencyCode)
{
    FAIL_RETURN(SetSymbol(mAddress, UNUM_CURRENCY_SYMBOL, currencySymbol));
    return SetSymbol(mAddress, UNUM_INTL_CURRENCY_SYMBOL, currencyCode);
}

ECode NativeDecimalFormat::SetGroupingSize(
    /* [in] */ Integer size)
{
    SetAttribute(mAddress, UNUM_GROUPING_SIZE, size);
    return NOERROR;
}

ECode NativeDecimalFormat::SetGroupingUsed(
    /* [in] */ Boolean used)
{
    SetAttribute(mAddress, UNUM_GROUPING_USED, used ? -1 : 0);
    return NOERROR;
}

ECode NativeDecimalFormat::SetMaximumFractionDigits(
    /* [in] */ Integer value)
{
    SetAttribute(mAddress, UNUM_MAX_FRACTION_DIGITS, value);
    return NOERROR;
}

ECode NativeDecimalFormat::SetMaximumIntegerDigits(
    /* [in] */ Integer value)
{
    SetAttribute(mAddress, UNUM_MAX_INTEGER_DIGITS, value);
    return NOERROR;
}

ECode NativeDecimalFormat::SetMinimumFractionDigits(
    /* [in] */ Integer value)
{
    SetAttribute(mAddress, UNUM_MIN_FRACTION_DIGITS, value);
    return NOERROR;
}

ECode NativeDecimalFormat::SetMinimumIntegerDigits(
    /* [in] */ Integer value)
{
    SetAttribute(mAddress, UNUM_MIN_INTEGER_DIGITS, value);
    return NOERROR;
}

ECode NativeDecimalFormat::SetMultiplier(
    /* [in] */ Integer multiplier)
{
    SetAttribute(mAddress, UNUM_MULTIPLIER, multiplier);
    return NOERROR;
}

ECode NativeDecimalFormat::SetNegativePrefix(
    /* [in] */ const String& prefix)
{
    mNegPrefNull = prefix.IsNull();
    if (!mNegPrefNull) {
        return SetTextAttribute(mAddress, UNUM_NEGATIVE_PREFIX, prefix);
    }
    return NOERROR;
}

ECode NativeDecimalFormat::SetNegativeSuffix(
    /* [in] */ const String& suffix)
{
    mNegSuffNull = suffix.IsNull();
    if (!mNegSuffNull) {
        return SetTextAttribute(mAddress, UNUM_NEGATIVE_SUFFIX, suffix);
    }
    return NOERROR;
}

ECode NativeDecimalFormat::SetPositivePrefix(
    /* [in] */ const String& prefix)
{
    mPosPrefNull = prefix.IsNull();
    if (!mPosPrefNull) {
        return SetTextAttribute(mAddress, UNUM_POSITIVE_PREFIX, prefix);
    }
    return NOERROR;
}

ECode NativeDecimalFormat::SetPositiveSuffix(
    /* [in] */ const String& suffix)
{
    mPosSuffNull = suffix.IsNull();
    if (!mPosSuffNull) {
        return SetTextAttribute(mAddress, UNUM_POSITIVE_SUFFIX, suffix);
    }
    return NOERROR;
}

ECode NativeDecimalFormat::SetParseBigDecimal(
    /* [in] */ Boolean value)
{
    mParseBigDecimal = value;
    return NOERROR;
}

ECode NativeDecimalFormat::SetParseIntegerOnly(
    /* [in] */ Boolean value)
{
    SetAttribute(mAddress, UNUM_PARSE_INT_ONLY, value ? -1 : 0);
    return NOERROR;
}

ECode NativeDecimalFormat::ApplyPattern(
    /* [in] */ HANDLE addr,
    /* [in] */ Boolean localized,
    /* [in] */ const String& pattern)
{
    return ApplyPatternImpl(addr, localized, pattern);
}

ECode NativeDecimalFormat::SetRoundingMode(
    /* [in] */ RoundingMode mode,
    /* [in] */ Double roundingIncrement)
{
    Integer nativeRoundingMode;
    switch (mode) {
        case RoundingMode::CEILING:
            nativeRoundingMode = 0;
            break;
        case RoundingMode::FLOOR:
            nativeRoundingMode = 1;
            break;
        case RoundingMode::DOWN:
            nativeRoundingMode = 2;
            break;
        case RoundingMode::UP:
            nativeRoundingMode = 3;
            break;
        case RoundingMode::HALF_EVEN:
            nativeRoundingMode = 4;
            break;
        case RoundingMode::HALF_DOWN:
            nativeRoundingMode = 5;
            break;
        case RoundingMode::HALF_UP:
            nativeRoundingMode = 6;
            break;
        case RoundingMode::UNNECESSARY:
            nativeRoundingMode = 7;
            break;
        default: //throw new AssertionError();
            return E_ASSERTION_ERROR;
    }
    SetRoundingMode(mAddress, nativeRoundingMode, roundingIncrement);
    return NOERROR;
}

ECode NativeDecimalFormat::ToLocalizedPattern(
    /* [out] */ String* pattern)
{
    *pattern = ToPatternImpl(mAddress, true);
    return NOERROR;
}

ECode NativeDecimalFormat::ToPattern(
    /* [out] */ String* pattern)
{
    *pattern = ToPatternImpl(mAddress, false);
    return NOERROR;
}

ECode NativeDecimalFormat::Parse(
    /* [in] */ const String& string,
    /* [in] */ IParsePosition* position,
    /* [out] */ INumber** number)
{
    AutoPtr<INumber> temp = Parse(mAddress, string, position, mParseBigDecimal);
    temp.MoveTo(number);
    return NOERROR;
}

static U_ICU_NAMESPACE::DecimalFormatSymbols* MakeDecimalFormatSymbols(
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
    /* [in] */ Char zeroDigit)
{
    UErrorCode status = U_ZERO_ERROR;
    U_ICU_NAMESPACE::DecimalFormatSymbols* result = new U_ICU_NAMESPACE::DecimalFormatSymbols(status);

    result->setSymbol(U_ICU_NAMESPACE::DecimalFormatSymbols::kCurrencySymbol,
            UnicodeString::fromUTF8(currencySymbol.string()));
    result->setSymbol(U_ICU_NAMESPACE::DecimalFormatSymbols::kDecimalSeparatorSymbol,
            UnicodeString((UChar32)decimalSeparator));
    result->setSymbol(U_ICU_NAMESPACE::DecimalFormatSymbols::kDigitSymbol,
            UnicodeString((UChar32)digit));
    result->setSymbol(U_ICU_NAMESPACE::DecimalFormatSymbols::kExponentialSymbol,
            UnicodeString::fromUTF8(exponentSeparator.string()));
    result->setSymbol(U_ICU_NAMESPACE::DecimalFormatSymbols::kGroupingSeparatorSymbol,
            UnicodeString((UChar32)groupingSeparator));
    result->setSymbol(U_ICU_NAMESPACE::DecimalFormatSymbols::kMonetaryGroupingSeparatorSymbol,
            UnicodeString((UChar32)groupingSeparator));
    result->setSymbol(U_ICU_NAMESPACE::DecimalFormatSymbols::kInfinitySymbol,
            UnicodeString::fromUTF8(infinity.string()));
    result->setSymbol(U_ICU_NAMESPACE::DecimalFormatSymbols::kIntlCurrencySymbol,
            UnicodeString::fromUTF8(internationalCurrencySymbol.string()));
    result->setSymbol(U_ICU_NAMESPACE::DecimalFormatSymbols::kMinusSignSymbol,
            UnicodeString::fromUTF8(minusSign.string()));
    result->setSymbol(U_ICU_NAMESPACE::DecimalFormatSymbols::kMonetarySeparatorSymbol,
            UnicodeString((UChar32)monetaryDecimalSeparator));
    result->setSymbol(U_ICU_NAMESPACE::DecimalFormatSymbols::kNaNSymbol,
            UnicodeString::fromUTF8(nan.string()));
    result->setSymbol(U_ICU_NAMESPACE::DecimalFormatSymbols::kPatternSeparatorSymbol,
            UnicodeString((UChar32)patternSeparator));
    result->setSymbol(U_ICU_NAMESPACE::DecimalFormatSymbols::kPercentSymbol,
            UnicodeString((UChar32)percent));
    result->setSymbol(U_ICU_NAMESPACE::DecimalFormatSymbols::kPerMillSymbol,
            UnicodeString((UChar32)perMill));

    // como.text.DecimalFormatSymbols just uses a zero digit,
    // but ICU >= 4.6 has a field for each decimal digit.
    result->setSymbol(U_ICU_NAMESPACE::DecimalFormatSymbols::kZeroDigitSymbol,
            UnicodeString((UChar32)(zeroDigit + 0)));
    result->setSymbol(U_ICU_NAMESPACE::DecimalFormatSymbols::kOneDigitSymbol,
            UnicodeString((UChar32)(zeroDigit + 1)));
    result->setSymbol(U_ICU_NAMESPACE::DecimalFormatSymbols::kTwoDigitSymbol,
            UnicodeString((UChar32)(zeroDigit + 2)));
    result->setSymbol(U_ICU_NAMESPACE::DecimalFormatSymbols::kThreeDigitSymbol,
            UnicodeString((UChar32)(zeroDigit + 3)));
    result->setSymbol(U_ICU_NAMESPACE::DecimalFormatSymbols::kFourDigitSymbol,
            UnicodeString((UChar32)(zeroDigit + 4)));
    result->setSymbol(U_ICU_NAMESPACE::DecimalFormatSymbols::kFiveDigitSymbol,
            UnicodeString((UChar32)(zeroDigit + 5)));
    result->setSymbol(U_ICU_NAMESPACE::DecimalFormatSymbols::kSixDigitSymbol,
            UnicodeString((UChar32)(zeroDigit + 6)));
    result->setSymbol(U_ICU_NAMESPACE::DecimalFormatSymbols::kSevenDigitSymbol,
            UnicodeString((UChar32)(zeroDigit + 7)));
    result->setSymbol(U_ICU_NAMESPACE::DecimalFormatSymbols::kEightDigitSymbol,
            UnicodeString((UChar32)(zeroDigit + 8)));
    result->setSymbol(U_ICU_NAMESPACE::DecimalFormatSymbols::kNineDigitSymbol,
            UnicodeString((UChar32)(zeroDigit + 9)));
    return result;
}

static Array<Char> FormatResult(
    /* [in] */ const UnicodeString& s,
    /* [in] */ U_ICU_NAMESPACE::FieldPositionIterator* fpi,
    /* [in] */ NativeDecimalFormat::FieldPositionIterator* fieldPositionIterator)
{
    if (fpi != nullptr) {
        std::vector<Integer> data;
        FieldPosition fp;
        while (fpi->next(fp)) {
            data.push_back(fp.getField());
            data.push_back(fp.getBeginIndex());
            data.push_back(fp.getEndIndex());
        }

        Array<Integer> aiData;
        if (!data.empty()) {
            aiData = Array<Integer>(data.size());
            for (Integer i = 0; i < data.size(); i++) {
                aiData[i] = data[i];
            }
        }
        fieldPositionIterator->SetData(aiData);
    }

    return ToUTF8String(s).GetChars();
}

template <typename T>
static ECode Format(
    /* [in] */ HANDLE addr,
    /* [in] */ NativeDecimalFormat::FieldPositionIterator* fieldPositionIterator,
    /* [in] */ T value,
    /* [out, callee] */ Array<Char>* result)
{
    UErrorCode status = U_ZERO_ERROR;
    UnicodeString s;
    U_ICU_NAMESPACE::DecimalFormat* fmt = ToDecimalFormat(addr);
    U_ICU_NAMESPACE::FieldPositionIterator nativeFieldPositionIterator;
    U_ICU_NAMESPACE::FieldPositionIterator* fpi = fieldPositionIterator ? &nativeFieldPositionIterator : nullptr;
    fmt->format(value, s, fpi, status);
    FAIL_RETURN(MaybeThrowIcuException("format", status));
    *result = FormatResult(s, fpi, fieldPositionIterator);
    return NOERROR;
}

ECode NativeDecimalFormat::ApplyPatternImpl(
    /* [in] */ HANDLE addr,
    /* [in] */ Boolean localized,
    /* [in] */ const String& pattern)
{
    if (pattern.IsNull()) {
        return NOERROR;
    }
    U_ICU_NAMESPACE::DecimalFormat* fmt = ToDecimalFormat(addr);
    UErrorCode status = U_ZERO_ERROR;
    const char* function;
    if (localized) {
        function = "DecimalFormat::applyLocalizedPattern";
        fmt->applyLocalizedPattern(
                UnicodeString::fromUTF8(pattern.string()), status);
    }
    else {
        function = "DecimalFormat::applyPattern";
        fmt->applyPattern(
                UnicodeString::fromUTF8(pattern.string()), status);
    }
    return MaybeThrowIcuException(function, status);
}

ECode NativeDecimalFormat::FormatLong(
    /* [in] */ HANDLE addr,
    /* [in] */ Long value,
    /* [in] */ FieldPositionIterator* fieldPositionIterator,
    /* [out, callee] */ Array<Char>* result)
{
    return Format<int64_t>(addr, fieldPositionIterator, value, result);
}

ECode NativeDecimalFormat::FormatDouble(
    /* [in] */ HANDLE addr,
    /* [in] */ Double value,
    /* [in] */ FieldPositionIterator* fieldPositionIterator,
    /* [out, callee] */ Array<Char>* result)
{
    return Format(addr, fieldPositionIterator, value, result);
}

ECode NativeDecimalFormat::FormatDigitList(
    /* [in] */ HANDLE addr,
    /* [in] */ const String& value,
    /* [in] */ FieldPositionIterator* fieldPositionIterator,
    /* [out, callee] */ Array<Char>* result)
{
    if (value.IsNull()) {
        *result = Array<Char>::Null();
        return NOERROR;
    }
    StringPiece sp(value.string());
    return Format(addr, fieldPositionIterator, sp, result);
}

static AutoPtr<INumber> NewBigDecimal(
    /* [in] */ const char* value,
    /* [in] */ size_t len)
{
    // this is painful...
    // value is a UTF-8 string of invariant characters, but isn't guaranteed to be
    // null-terminated.  NewStringUTF requires a terminated UTF-8 string.  So we copy the
    // data to jchars using UnicodeString, and call NewString instead.
    UnicodeString tmp(value, len, UnicodeString::kInvariant);
    AutoPtr<INumber> num;
    CBigDecimal::New(ToUTF8String(tmp), IID_INumber, (IInterface**)&num);
    return num;
}

Integer NativeDecimalFormat::GetAttribute(
    /* [in] */ HANDLE addr,
    /* [in] */ Integer symbol)
{
    UNumberFormatAttribute attr = static_cast<UNumberFormatAttribute>(symbol);
    return unum_getAttribute(ToUNumberFormat(addr), attr);
}

ECode NativeDecimalFormat::GetTextAttribute(
    /* [in] */ HANDLE addr,
    /* [in] */ Integer symbol,
    /* [out] */ String* textAttr)
{
    VALIDATE_NOT_NULL(textAttr);

    UErrorCode status = U_ZERO_ERROR;
    UNumberFormat* fmt = ToUNumberFormat(addr);
    UNumberFormatTextAttribute attr = static_cast<UNumberFormatTextAttribute>(symbol);

    // Find out how long the result will be...
    UChar* chars = nullptr;
    uint32_t charCount = 0;
    uint32_t desiredCount = unum_getTextAttribute(fmt, attr, chars, charCount, &status);
    if (status == U_BUFFER_OVERFLOW_ERROR) {
        // ...then get it.
        status = U_ZERO_ERROR;
        charCount = desiredCount + 1;
        chars = new UChar[charCount];
        charCount = unum_getTextAttribute(fmt, attr, chars, charCount, &status);
    }
    *textAttr = ToUTF8String(UnicodeString(chars, charCount));
    delete[] chars;
    return MaybeThrowIcuException("unum_getTextAttribute", status);
}

ECode NativeDecimalFormat::Open(
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
    /* [out] */ HANDLE* result)
{
    UErrorCode status = U_ZERO_ERROR;
    UParseError parseError;
    if (pattern.IsNull()) {
        *result = 0;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    U_ICU_NAMESPACE::DecimalFormatSymbols* symbols = MakeDecimalFormatSymbols(
            currencySymbol, decimalSeparator, digit, exponentSeparator, groupingSeparator,
            infinity, internationalCurrencySymbol, minusSign,
            monetaryDecimalSeparator, nan, patternSeparator, percent, perMill,
            zeroDigit);
    U_ICU_NAMESPACE::DecimalFormat* fmt = new U_ICU_NAMESPACE::DecimalFormat(
            UnicodeString::fromUTF8(pattern.string()), symbols, parseError, status);
    if (fmt == nullptr) {
        delete symbols;
    }
    FAIL_RETURN(MaybeThrowIcuException("DecimalFormat::DecimalFormat", status));
    *result = reinterpret_cast<uintptr_t>(fmt);
    return NOERROR;
}

AutoPtr<INumber> NativeDecimalFormat::Parse(
    /* [in] */ HANDLE addr,
    /* [in] */ const String& text,
    /* [in] */ IParsePosition* position,
    /* [in] */ Boolean parseBigDecimal)
{
    if (text.IsNull()) {
        return nullptr;
    }
    // make sure the ParsePosition is valid. Actually icu4c would parse a number
    // correctly even if the parsePosition is set to -1, but since the RI fails
    // for that case we have to fail too
    Integer parsePos;
    position->GetIndex(parsePos);
    if (parsePos < 0 || parsePos > text.GetLength()) {
        return nullptr;
    }

    U_ICU_NAMESPACE::Formattable res;
    ParsePosition pp(parsePos);
    U_ICU_NAMESPACE::DecimalFormat* fmt = ToDecimalFormat(addr);
    fmt->parse(UnicodeString::fromUTF8(text.string()), res, pp);

    if (pp.getErrorIndex() == -1) {
        position->SetIndex(pp.getIndex());
    }
    else {
        position->SetErrorIndex(pp.getErrorIndex());
        return nullptr;
    }

    if (parseBigDecimal) {
        UErrorCode status = U_ZERO_ERROR;
        U_ICU_NAMESPACE::StringPiece str = res.getDecimalNumber(status);
        if (U_SUCCESS(status)) {
            int len = str.length();
            const char* data = str.data();
            if (strncmp(data, "NaN", 3) == 0 ||
                strncmp(data, "Inf", 3) == 0 ||
                strncmp(data, "-Inf", 4) == 0) {
                double resultDouble = res.getDouble(status);
                AutoPtr<INumber> num;
                CDouble::New(resultDouble, IID_INumber, (IInterface**)&num);
                return num;
            }
            return NewBigDecimal(data, len);
        }
        return nullptr;
    }

    U_ICU_NAMESPACE::Formattable::Type numType = res.getType();
    switch(numType) {
        case U_ICU_NAMESPACE::Formattable::kDouble: {
            double resultDouble = res.getDouble();
            AutoPtr<INumber> num;
            CDouble::New(resultDouble, IID_INumber, (IInterface**)&num);
            return num;
        }
        case U_ICU_NAMESPACE::Formattable::kLong: {
            long resultLong = res.getLong();
            AutoPtr<INumber> num;
            CLong::New(resultLong, IID_INumber, (IInterface**)&num);
            return num;
        }
        case U_ICU_NAMESPACE::Formattable::kInt64: {
            int64_t resultInt64 = res.getInt64();
            AutoPtr<INumber> num;
            CLong::New(resultInt64, IID_INumber, (IInterface**)&num);
            return num;
        }
        default: {
            return nullptr;
        }
    }
    return nullptr;
}

void NativeDecimalFormat::SetDecimalFormatSymbols(
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
    /* [in] */ Char zeroDigit)
{
    U_ICU_NAMESPACE::DecimalFormatSymbols* symbols = MakeDecimalFormatSymbols(
            currencySymbol, decimalSeparator, digit, exponentSeparator, groupingSeparator,
            infinity, internationalCurrencySymbol, minusSign,
            monetaryDecimalSeparator, nan, patternSeparator, percent, perMill,
            zeroDigit);
    ToDecimalFormat(addr)->adoptDecimalFormatSymbols(symbols);
}

ECode NativeDecimalFormat::SetSymbol(
    /* [in] */ HANDLE addr,
    /* [in] */ Integer _symbol,
    /* [in] */ const String& str)
{
    if (str.IsNull()) {
        return NOERROR;
    }
    UnicodeString _s = UnicodeString::fromUTF8(str.string());
    UnicodeString& s(_s);
    UErrorCode status = U_ZERO_ERROR;
    UNumberFormatSymbol symbol = static_cast<UNumberFormatSymbol>(_symbol);
    unum_setSymbol(ToUNumberFormat(addr), symbol, s.getBuffer(), s.length(), &status);
    return MaybeThrowIcuException("unum_setSymbol", status);
}

void NativeDecimalFormat::SetAttribute(
    /* [in] */ HANDLE addr,
    /* [in] */ Integer symbol,
    /* [in] */ Integer i)
{
    UNumberFormatAttribute attr = static_cast<UNumberFormatAttribute>(symbol);
    unum_setAttribute(ToUNumberFormat(addr), attr, i);
}

void NativeDecimalFormat::SetRoundingMode(
    /* [in] */ HANDLE addr,
    /* [in] */ Integer mode,
    /* [in] */ Double increment)
{
    U_ICU_NAMESPACE::DecimalFormat* fmt = ToDecimalFormat(addr);
    fmt->setRoundingMode(static_cast<U_ICU_NAMESPACE::DecimalFormat::ERoundingMode>(mode));
    fmt->setRoundingIncrement(increment);
}

ECode NativeDecimalFormat::SetTextAttribute(
    /* [in] */ HANDLE addr,
    /* [in] */ Integer symbol,
    /* [in] */ const String& str)
{
    if (str.IsNull()) {
        return NOERROR;
    }
    UnicodeString _s = UnicodeString::fromUTF8(str.string());
    UnicodeString& s(_s);
    UErrorCode status = U_ZERO_ERROR;
    UNumberFormatTextAttribute attr = static_cast<UNumberFormatTextAttribute>(symbol);
    unum_setTextAttribute(ToUNumberFormat(addr), attr, s.getBuffer(), s.length(), &status);
    return MaybeThrowIcuException("unum_setTextAttribute", status);
}

String NativeDecimalFormat::ToPatternImpl(
    /* [in] */ HANDLE addr,
    /* [in] */ Boolean localized)
{
    U_ICU_NAMESPACE::DecimalFormat* fmt = ToDecimalFormat(addr);
    UnicodeString pattern;
    if (localized) {
        fmt->toLocalizedPattern(pattern);
    }
    else {
        fmt->toPattern(pattern);
    }
    return ToUTF8String(pattern);
}

Boolean NativeDecimalFormat::Equals(
    /* [in] */ NativeDecimalFormat* other)
{
    if (this == other) {
        return true;
    }
    if (other == nullptr) {
        return false;
    }
    if (mAddress == other->mAddress) {
        return true;
    }
    String thisPattern, otherPattern;
    this->ToPattern(&thisPattern);
    other->ToPattern(&otherPattern);
    String thisNegativePrefix, otherNegativePrefix;
    this->GetNegativePrefix(&thisNegativePrefix);
    other->GetNegativePrefix(&otherNegativePrefix);
    String thisNegativeSuffix, otherNegativeSuffix;
    this->GetNegativeSuffix(&thisNegativeSuffix);
    other->GetNegativeSuffix(&otherNegativeSuffix);
    String thisPositivePrefix, otherPositivePrefix;
    this->GetPositivePrefix(&thisPositivePrefix);
    other->GetPositivePrefix(&otherPositivePrefix);
    String thisPositiveSuffix, otherPositiveSuffix;
    this->GetPositiveSuffix(&thisPositiveSuffix);
    other->GetPositiveSuffix(&otherPositiveSuffix);
    return thisPattern.Equals(otherPattern) &&
            IsDecimalSeparatorAlwaysShown() == other->IsDecimalSeparatorAlwaysShown() &&
            GetGroupingSize() == other->GetGroupingSize() &&
            GetMultiplier() == other->GetMultiplier() &&
            thisNegativePrefix.Equals(otherNegativePrefix) &&
            thisNegativeSuffix.Equals(otherNegativeSuffix) &&
            thisPositivePrefix.Equals(otherPositivePrefix) &&
            thisPositiveSuffix.Equals(otherPositiveSuffix) &&
            GetMaximumIntegerDigits() == other->GetMaximumIntegerDigits() &&
            GetMaximumFractionDigits() == other->GetMaximumFractionDigits() &&
            GetMinimumIntegerDigits() == other->GetMinimumIntegerDigits() &&
            GetMinimumFractionDigits() == other->GetMinimumFractionDigits() &&
            IsGroupingUsed() == other->IsGroupingUsed();
}

NativeDecimalFormat* NativeDecimalFormat::Clone()
{
    NativeDecimalFormat* clone = new NativeDecimalFormat();

    clone->mAddress = reinterpret_cast<HANDLE>(ToDecimalFormat(mAddress)->clone());
    clone->mLastPattern = mLastPattern;
    clone->mNegPrefNull = mNegPrefNull;
    clone->mNegSuffNull = mNegSuffNull;
    clone->mPosPrefNull = mPosPrefNull;
    clone->mPosSuffNull = mPosSuffNull;
    return clone;
}

Integer NativeDecimalFormat::TranslateFieldId(
    /* [in] */ IFieldPosition* fp)
{
    Integer id;
    fp->GetField(id);
    if (id < -1 || id > 1) {
        id = -1;
    }
    if (-1 == id) {
        AutoPtr<IFormatField> attr;
        fp->GetFieldAttribute(attr);
        if (attr != NULL) {
            for (Long i = 0; i < Get_ICU4C_FIELD_IDS().GetLength(); ++i) {
                if (Object::Equals(Get_ICU4C_FIELD_IDS()[i], attr)) {
                    id = i;
                    break;
                }
            }
        }
    }
    return id;
}

}
}
