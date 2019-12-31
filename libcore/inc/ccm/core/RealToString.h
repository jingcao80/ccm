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

#ifndef __CCM_CORE_REALTOSTRING_H__
#define __CCM_CORE_REALTOSTRING_H__

#include "ccm.core.IStringBuilder.h"
#include <ccmtypes.h>

namespace ccm {
namespace core {

class RealToString
{
public:
    static String FloatToString(
        /* [in] */ Float f);

    static String ConvertFloat(
        /* [in] */ IStringBuilder* sb,
        /* [in] */ Float inputNumber);

    static String DoubleToString(
        /* [in] */ Double d);

    static String ConvertDouble(
        /* [in] */ IStringBuilder* sb,
        /* [in] */ Double inputNumber);

private:
    static String ResultOrSideEffect(
        /* [in] */ IStringBuilder* sb,
        /* [in] */ const String& s);

    static void FreeFormatExponential(
        /* [in] */ IStringBuilder* sb,
        /* [in] */ Boolean positive);

    static void FreeFormat(
        /* [in] */ IStringBuilder* sb,
        /* [in] */ Boolean positive);

    static void BigIntDigitGenerator(
        /* [in] */ Long f,
        /* [in] */ Integer e,
        /* [in] */ Boolean isDenormalized,
        /* [in] */ Integer p);

    static void LongDigitGenerator(
        /* [in] */ Long f,
        /* [in] */ Integer e,
        /* [in] */ Boolean isDenormalized,
        /* [in] */ Boolean mantissaIsZero,
        /* [in] */ Integer p);

private:
    static constexpr Integer FLOAT_EXPONENT_BIAS = 127;
    static constexpr Integer FLOAT_EXPONENT_BITS = 9;
    static constexpr Integer FLOAT_MANTISSA_BITS = 23;
    static constexpr Integer FLOAT_SIGN_MASK = 0x80000000;
    static constexpr Integer FLOAT_EXPONENT_MASK = 0x7f800000;
    static constexpr Integer FLOAT_MANTISSA_MASK = 0x007fffff;

    static constexpr Integer DOUBLE_EXPONENT_BIAS = 1023;
    static constexpr Integer DOUBLE_EXPONENT_BITS = 12;
    static constexpr Integer DOUBLE_MANTISSA_BITS = 52;
    static constexpr Long DOUBLE_SIGN_MASK = 0x8000000000000000ll;
    static constexpr Long DOUBLE_EXPONENT_MASK = 0x7ff0000000000000ll;
    static constexpr Long DOUBLE_MANTISSA_MASK = 0x000fffffffffffffll;

    static Integer sFirstK;

    /**
     * An array of decimal digits, filled by longDigitGenerator or bigIntDigitGenerator.
     */
    static Integer sDigits[64];

    /**
     * Number of valid entries in 'digits'.
     */
    static Integer sDigitCount;
};

}
}

#endif // __CCM_CORE_REALTOSTRING_H__
