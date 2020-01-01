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

#ifndef __COMO_MATH_MULTIPLICATION_H__
#define __COMO_MATH_MULTIPLICATION_H__

#include "como/math/CBigInteger.h"

namespace como {
namespace math {

class Multiplication
{
public:
    static AutoPtr<IBigInteger> MultiplyByPositiveInteger(
        /* [in] */ BigInteger* value,
        /* [in] */ Integer factor);

    static AutoPtr<IBigInteger> MultiplyByTenPow(
        /* [in] */ BigInteger* value,
        /* [in] */ Long exp);

    static AutoPtr<IBigInteger> PowerOf10(
        /* [in] */ Long exp);

    static AutoPtr<IBigInteger> MultiplyByFivePow(
        /* [in] */ BigInteger* value,
        /* [in] */ Integer exp);

    static const Array<IBigInteger*>& GetBigTenPows();

    static const Array<IBigInteger*>& GetBigFivePows();

private:
    Multiplication();

public:
    /**
     * An array with powers of ten that fit in the type {@code Integer}.
     * ({@code 10^0,10^1,...,10^9})
     */
    static const Array<Integer> sTenPows;

    /**
     * An array with powers of five that fit in the type {@code int}.
     * ({@code 5^0,5^1,...,5^13})
     */
    static const Array<Integer> sFivePows;
};

}
}

#endif // __COMO_MATH_MULTIPLICATION_H__
