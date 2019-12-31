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

#ifndef __CCM_MISC_FLOATCONSTS_H__
#define __CCM_MISC_FLOATCONSTS_H__

#include <ccmtypes.h>

namespace ccm {
namespace misc {

class FloatConsts
{
public:
    static const Float POSITIVE_INFINITY;
    static const Float NEGATIVE_INFINITY;
    static const Float NaN;
    static const Float MAX_VALUE;
    static const Float MIN_VALUE;

    /**
     * A constant holding the smallest positive normal value of type
     * <code>float</code>, 2<sup>-126</sup>.  It is equal to the value
     * returned by <code>Float.intBitsToFloat(0x00800000)</code>.
     */
    static constexpr Float MIN_NORMAL = 1.17549435E-38f;

    /**
     * The number of logical bits in the significand of a
     * <code>float</code> number, including the implicit bit.
     */
    static constexpr Integer SIGNIFICAND_WIDTH = 24;

    /**
     * Maximum exponent a finite <code>float</code> number may have.
     * It is equal to the value returned by
     * <code>Math.ilogb(Float.MAX_VALUE)</code>.
     */
    static constexpr Integer MAX_EXPONENT = 127;

    /**
     * Minimum exponent a normalized <code>float</code> number may
     * have.  It is equal to the value returned by
     * <code>Math.ilogb(Float.MIN_NORMAL)</code>.
     */
    static constexpr Integer MIN_EXPONENT = -126;

    /**
     * The exponent the smallest positive <code>float</code> subnormal
     * value would have if it could be normalized.  It is equal to the
     * value returned by <code>FpUtils.ilogb(Float.MIN_VALUE)</code>.
     */
    static constexpr Integer MIN_SUB_EXPONENT = MIN_EXPONENT - (SIGNIFICAND_WIDTH - 1);

    /**
     * Bias used in representing a <code>float</code> exponent.
     */
    static constexpr Integer EXP_BIAS = 127;

    /**
     * Bit mask to isolate the sign bit of a <code>float</code>.
     */
    static constexpr Integer SIGN_BIT_MASK = 0x80000000;

    /**
     * Bit mask to isolate the exponent field of a
     * <code>float</code>.
     */
    static constexpr Integer EXP_BIT_MASK = 0x7F800000;

    /**
     * Bit mask to isolate the significand field of a
     * <code>float</code>.
     */
    static constexpr Integer SIGNIF_BIT_MASK = 0x007FFFFF;

private:
    FloatConsts();
};

}
}

#endif // __CCM_MISC_FLOATCONSTS_H__
