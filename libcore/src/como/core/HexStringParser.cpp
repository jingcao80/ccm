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

#include "como/core/Character.h"
#include "como/core/CoreUtils.h"
#include "como/core/HexStringParser.h"
#include "como/core/Math.h"
#include "como/core/StringUtils.h"
#include "como/util/regex/Pattern.h"
#include "como.core.ILong.h"
#include "como.util.regex.IMatcher.h"
#include "como.util.regex.IMatchResult.h"
#include <comolog.h>

using como::util::regex::IMatcher;
using como::util::regex::IMatchResult;
using como::util::regex::Pattern;

namespace como {
namespace core {

HexStringParser::HexStringParser(
    /* [in] */ Integer exponentWidth,
    /* [in] */ Integer mantissaWidth)
    : EXPONENT_WIDTH(exponentWidth)
    , MANTISSA_WIDTH(mantissaWidth)
    , EXPONENT_BASE(~(-1ll << (exponentWidth - 1)))
    , MAX_EXPONENT(~(-1ll << exponentWidth))
    , MIN_EXPONENT(-(MANTISSA_WIDTH + 1))
    , MANTISSA_MASK(~(-1ll << mantissaWidth))
{}

ECode HexStringParser::ParseFloat(
    /* [in] */ const String& s,
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);

    HexStringParser parser(FLOAT_EXPONENT_WIDTH, FLOAT_MANTISSA_WIDTH);
    Long result;
    FAIL_RETURN(parser.Parse(s, false, &result));
    *value = Math::IntegerBitsToFloat(result);
    return NOERROR;
}

ECode HexStringParser::ParseDouble(
    /* [in] */ const String& s,
    /* [out] */ Double* value)
{
    VALIDATE_NOT_NULL(value);

    HexStringParser parser(DOUBLE_EXPONENT_WIDTH, DOUBLE_MANTISSA_WIDTH);
    Long result;
    FAIL_RETURN(parser.Parse(s, true, &result));
    *value = Math::LongBitsToDouble(result);
    return NOERROR;
}

ECode HexStringParser::Parse(
    /* [in] */ const String& hexString,
    /* [in] */ Boolean isDouble,
    /* [out] */ Long* result)
{
    static const AutoPtr<IPattern> PATTERN = MAKE_PATTERN();

    AutoPtr<IMatcher> matcher;
    PATTERN->Matcher(CoreUtils::Box(hexString), &matcher);
    Boolean matched;
    if (matcher->Matches(&matched), !matched) {
        Logger::E("HexStringParser", "Invalid hex %s:%s",
                isDouble ? "double" : "float", hexString.string());
        return E_NUMBER_FORMAT_EXCEPTION;
    }

    IMatchResult* mr = IMatchResult::Probe(matcher);
    String signStr, significantStr, exponentStr;
    mr->Group(1, &signStr);
    mr->Group(2, &significantStr);
    mr->Group(3, &exponentStr);

    ParseHexSign(signStr);
    ParseExponent(exponentStr);
    ParseMantissa(significantStr);

    mSign <<= (MANTISSA_WIDTH + EXPONENT_WIDTH);
    mExponent <<= MANTISSA_WIDTH;
    *result = mSign | mExponent | mMantissa;
    return NOERROR;
}

void HexStringParser::ParseHexSign(
    /* [in] */ const String& signStr)
{
    mSign = signStr.Equals("-") ? 1 : 0;
}

void HexStringParser::ParseExponent(
    /* [in] */ String exponentStr)
{
    Char leadingChar = exponentStr.GetChar(0);
    Integer expSign = (leadingChar == U'-' ? -1 : 1);
    if (!Character::IsDigit(leadingChar)) {
        exponentStr = exponentStr.Substring(1);
    }

    Long value;
    ECode ec = StringUtils::ParseLong(exponentStr, &value);
    if (FAILED(ec)) {
        mExponent = expSign * ILong::MAX_VALUE;
        return;
    }
    mExponent = expSign * value;
    CheckedAddExponent(EXPONENT_BASE);
}

void HexStringParser::ParseMantissa(
    /* [in] */ const String& significantStr)
{
    Array<String> strings = StringUtils::Split(significantStr, String("\\."));
    String strIntegerPart = strings[0];
    String strDecimalPart = strings.GetLength() > 1 ? strings[1] : String("");

    String significand = GetNormalizedSignificand(strIntegerPart, strDecimalPart);
    if (significand.Equals("0")) {
        SetZero();
        return;
    }

    Integer offset = GetOffset(strIntegerPart, strDecimalPart);
    CheckedAddExponent(offset);

    if (mExponent >= MAX_EXPONENT) {
        SetInfinite();
        return;
    }

    if (mExponent <= MIN_EXPONENT) {
        SetZero();
        return;
    }

    if (significand.GetLength() > MAX_SIGNIFICANT_LENGTH) {
        mAbandonedNumber = significand.Substring(MAX_SIGNIFICANT_LENGTH);
        significand = significand.Substring(0, MAX_SIGNIFICANT_LENGTH);
    }

    StringUtils::ParseLong(significand, HEX_RADIX, &mMantissa);

    if (mExponent >= 1) {
        ProcessNormalNumber();
    }
    else{
        ProcessSubNormalNumber();
    }
}

void HexStringParser::SetInfinite()
{
    mExponent = MAX_EXPONENT;
    mMantissa = 0;
}

void HexStringParser::SetZero()
{
    mExponent = 0;
    mMantissa = 0;
}

void HexStringParser::CheckedAddExponent(
    /* [in] */ Long offset)
{
    Long result = mExponent + offset;
    Integer expSign = Math::Signum(mExponent);
    if (expSign * Math::Signum(offset) > 0 && expSign * Math::Signum(result) < 0) {
        mExponent = expSign * ILong::MAX_VALUE;
    }
    else {
        mExponent = result;
    }
}

void HexStringParser::ProcessNormalNumber()
{
    Integer desiredWidth = MANTISSA_WIDTH + 2;
    FitMantissaInDesiredWidth(desiredWidth);
    Round();
    mMantissa = mMantissa & MANTISSA_MASK;
}

void HexStringParser::ProcessSubNormalNumber()
{
    Integer desiredWidth = MANTISSA_WIDTH + 1;
    desiredWidth += (Integer)mExponent;//lends bit from mantissa to exponent
    mExponent = 0;
    FitMantissaInDesiredWidth(desiredWidth);
    Round();
    mMantissa = mMantissa & MANTISSA_MASK;
}

void HexStringParser::FitMantissaInDesiredWidth(
    /* [in] */ Integer desiredWidth)
{
    Integer bitLength = CountBitsLength(mMantissa);
    if (bitLength > desiredWidth) {
        DiscardTrailingBits(bitLength - desiredWidth);
    }
    else {
        mMantissa <<= (desiredWidth - bitLength);
    }
}

void HexStringParser::DiscardTrailingBits(
    /* [in] */ Long num)
{
    Long mask = ~(-1ll << num);
    mAbandonedNumber = String::Format("%s%lld",
            mAbandonedNumber.string(), mMantissa & mask);
    mMantissa >>= num;
}

void HexStringParser::Round()
{
    String result;
    StringUtils::ReplaceAll(mAbandonedNumber, String("0+"), String(""), &result);
    Boolean moreThanZero = (result.GetLength() > 0 ? true : false);

    Integer lastDiscardedBit = (Integer) (mMantissa & 1ll);
    mMantissa >>= 1;
    Integer tailBitInMantissa = (Integer) (mMantissa & 1ll);

    if (lastDiscardedBit == 1 && (moreThanZero || tailBitInMantissa == 1)) {
        Integer oldLength = CountBitsLength(mMantissa);
        mMantissa += 1ll;
        Integer newLength = CountBitsLength(mMantissa);

        //Rounds up to exponent when whole bits of mantissa are one-bits.
        if (oldLength >= MANTISSA_WIDTH && newLength > oldLength) {
            CheckedAddExponent(1);
        }
    }
}

String HexStringParser::GetNormalizedSignificand(
    /* [in] */ const String& strIntegerPart,
    /* [in] */ const String& strDecimalPart)
{
    String significand = strIntegerPart + strDecimalPart;
    StringUtils::ReplaceFirst(significand, String("^0+"), String(""), &significand);
    if (significand.GetLength() == 0) {
        significand = "0";
    }
    return significand;
}

Integer HexStringParser::GetOffset(
    /* [in] */ String strIntegerPart,
    /* [in] */ const String& strDecimalPart)
{
    StringUtils::ReplaceFirst(strIntegerPart, String("^0+"), String(""), &strIntegerPart);

    //If the Integer part is a nonzero number.
    if (strIntegerPart.GetLength() != 0) {
        String leadingNumber = strIntegerPart.Substring(0, 1);
        Long value;
        StringUtils::ParseLong(leadingNumber, HEX_RADIX, &value);
        return (strIntegerPart.GetLength() - 1) * 4 + CountBitsLength(value) - 1;
    }

    //If the Integer part is a zero number.
    Integer i;
    for (i = 0; i < strDecimalPart.GetLength() && strDecimalPart.GetChar(i) == U'0'; i++);
    if (i == strDecimalPart.GetLength()) {
        return 0;
    }
    String leadingNumber = strDecimalPart.Substring(i, i + 1);
    Long value;
    StringUtils::ParseLong(leadingNumber, HEX_RADIX, &value);
    return (-i - 1) * 4 + CountBitsLength(value) - 1;
}

Integer HexStringParser::CountBitsLength(
    /* [in] */ Long value)
{
    Integer leadingZeros = Math::NumberOfLeadingZeros(value);
    return ILong::SIZE - leadingZeros;
}

AutoPtr<IPattern> HexStringParser::MAKE_PATTERN()
{
    static const String HEX_SIGNIFICANT("0[xX](\\p{XDigit}+\\.?|\\p{XDigit}*\\.\\p{XDigit}+)");
    static const String BINARY_EXPONENT("[pP]([+-]?\\d+)");
    static const String FLOAT_TYPE_SUFFIX("[fFdD]?");
    static const String HEX_PATTERN = String("[\\x00-\\x20]*([+-]?)") + HEX_SIGNIFICANT
            + BINARY_EXPONENT + FLOAT_TYPE_SUFFIX + "[\\x00-\\x20]*";
    AutoPtr<IPattern> p;
    Pattern::Compile(HEX_PATTERN, &p);
    return p;
}

}
}
