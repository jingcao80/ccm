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

#ifndef __CCM_MISC_DOUBLECONSTS_H__
#define __CCM_MISC_DOUBLECONSTS_H__

#include <ccmtypes.h>

namespace ccm {
namespace misc {

class DoubleConsts
{
public:
    static const Double POSITIVE_INFINITY;
    static const Double NEGATIVE_INFINITY;
    static const Double NaN;
    static const Double MAX_VALUE;
    static const Double MIN_VALUE;

    /**
     * A constant holding the smallest positive normal value of type
     * <code>double</code>, 2<sup>-1022</sup>.  It is equal to the
     * value returned by
     * <code>Double::LongBitsToDouble(0x0010000000000000L)</code>.
     */
    static constexpr Double MIN_NORMAL = 2.2250738585072014E-308;

    /**
     * The number of logical bits in the significand of a
     * <code>double</code> number, including the implicit bit.
     */
    static constexpr Integer SIGNIFICAND_WIDTH = 53;

    /**
     * Maximum exponent a finite <code>double</code> number may have.
     */
    static constexpr Integer MAX_EXPONENT = 1023;

    /**
     * Minimum exponent a normalized <code>double</code> number may
     * have.
     */
    static constexpr Integer MIN_EXPONENT = -1022;

    /**
     * Bias used in representing a <code>double</code> exponent.
     */
    static constexpr Integer EXP_BIAS = 1023;

    /**
     * Bit mask to isolate the sign bit of a <code>double</code>.
     */
    static constexpr Long SIGN_BIT_MASK = 0x8000000000000000ll;

    /**
     * Bit mask to isolate the exponent field of a
     * <code>double</code>.
     */
    static constexpr Long EXP_BIT_MASK = 0x7FF0000000000000ll;

    /**
     * Bit mask to isolate the significand field of a
     * <code>double</code>.
     */
    static constexpr Long SIGNIF_BIT_MASK = 0x000FFFFFFFFFFFFFll;

private:
    DoubleConsts();
};

}
}

#endif // __CCM_MISC_DOUBLECONSTS_H__
