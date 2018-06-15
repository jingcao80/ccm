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

#include "ccm/core/CoreUtils.h"
#include "ccm/core/HexStringParser.h"
#include "ccm/core/Math.h"
#include "ccm/util/regex/Pattern.h"
#include "ccm.util.regex.IMatcher.h"
#include "ccm.util.regex.IMatchResult.h"
#include <ccmlogger.h>

using ccm::util::regex::IMatcher;
using ccm::util::regex::IMatchResult;
using ccm::util::regex::Pattern;

namespace ccm {
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
    *value = Math::IntBitsToFloat(result);
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
    PATTERN->Matcher(CoreUtils::Box(hexString), (IMatcher**)&matcher);
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
    /* [in] */ const String& exponentStr)
{}

void HexStringParser::ParseMantissa(
    /* [in] */ const String& significantStr)
{}

AutoPtr<IPattern> HexStringParser::MAKE_PATTERN()
{
    static const String HEX_SIGNIFICANT("0[xX](\\p{XDigit}+\\.?|\\p{XDigit}*\\.\\p{XDigit}+)");
    static const String BINARY_EXPONENT("[pP]([+-]?\\d+)");
    static const String FLOAT_TYPE_SUFFIX("[fFdD]?");
    static const String HEX_PATTERN = String("[\\x00-\\x20]*([+-]?)") + HEX_SIGNIFICANT
            + BINARY_EXPONENT + FLOAT_TYPE_SUFFIX + "[\\x00-\\x20]*";
    AutoPtr<IPattern> p;
    Pattern::Compile(HEX_PATTERN, (IPattern**)&p);
    return p;
}

}
}
