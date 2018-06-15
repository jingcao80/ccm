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

#ifndef __CCM_CORE_HEXSTRINGPARSER_H__
#define __CCM_CORE_HEXSTRINGPARSER_H__

#include "ccm.util.regex.IPattern.h"
#include <ccmautoptr.h>
#include <ccmtypes.h>

using ccm::util::regex::IPattern;

namespace ccm {
namespace core {

class HexStringParser
{
public:
    static ECode ParseFloat(
        /* [in] */ const String& s,
        /* [out] */ Float* value);

    static ECode ParseDouble(
        /* [in] */ const String& s,
        /* [out] */ Double* value);

private:
    HexStringParser(
        /* [in] */ Integer exponentWidth,
        /* [in] */ Integer mantissaWidth);

    ECode Parse(
        /* [in] */ const String& hexString,
        /* [in] */ Boolean isDouble,
        /* [out] */ Long* result);

    void ParseHexSign(
        /* [in] */ const String& signStr);

    void ParseExponent(
        /* [in] */ const String& exponentStr);

    void ParseMantissa(
        /* [in] */ const String& significantStr);

    static AutoPtr<IPattern> MAKE_PATTERN();

private:
    static constexpr Integer DOUBLE_EXPONENT_WIDTH = 11;

    static constexpr Integer DOUBLE_MANTISSA_WIDTH = 52;

    static constexpr Integer FLOAT_EXPONENT_WIDTH = 8;

    static constexpr Integer FLOAT_MANTISSA_WIDTH = 23;

    const Integer EXPONENT_WIDTH;

    const Integer MANTISSA_WIDTH;

    const Long EXPONENT_BASE;

    const Long MAX_EXPONENT;

    const Long MIN_EXPONENT;

    const Long MANTISSA_MASK;

    Long mSign;

    Long mExponent;

    Long mMantissa;
};

}
}

#endif // __CCM_CORE_HEXSTRINGPARSER_H__
