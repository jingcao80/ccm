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

#ifndef __CCM_MATH_CONVERSION_H__
#define __CCM_MATH_CONVERSION_H__

#include "ccm/math/CBigInteger.h"

namespace ccm {
namespace math {

class Conversion
{
public:
    static String BigInteger2String(
        /* [in] */ BigInteger* value,
        /* [in] */ Integer radix);

    static String ToDecimalScaledString(
        /* [in] */ BigInteger* value,
        /* [in] */ Integer scale);

    static String ToDecimalScaledString(
        /* [in] */ Long value,
        /* [in] */ Integer scale);

    static Long DivideLongByBillion(
        /* [in] */ Long a);

    static Double BigInteger2Double(
        /* [in] */ BigInteger* value);

private:
    Conversion();

public:
    /**
     * Holds the maximal exponent for each radix, so that radix<sup>digitFitInInt[radix]</sup>
     * fit in an Integer (32 bits).
     */
    static constexpr Integer sDigitFitInInt[] = {
        -1, -1, 31, 19, 15, 13, 11,
        11, 10, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 7, 7, 7, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 5 };

    /**
     * bigRadices values are precomputed maximal powers of radices (integer
     * numbers from 2 to 36) that fit into unsigned int (32 bits). bigRadices[0] =
     * 2 ^ 31, bigRadices[8] = 10 ^ 9, etc.
     */
    static constexpr Integer sBigRadices[] = {
        -2147483648, 1162261467,
        1073741824, 1220703125, 362797056, 1977326743, 1073741824,
        387420489, 1000000000, 214358881, 429981696, 815730721, 1475789056,
        170859375, 268435456, 410338673, 612220032, 893871739, 1280000000,
        1801088541, 113379904, 148035889, 191102976, 244140625, 308915776,
        387420489, 481890304, 594823321, 729000000, 887503681, 1073741824,
        1291467969, 1544804416, 1838265625, 60466176 };
};

}
}

#endif // __CCM_MATH_CONVERSION_H__
