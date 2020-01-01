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

#ifndef __COMO_MATH_PRIMALITY_H__
#define __COMO_MATH_PRIMALITY_H__

#include "como/math/CBigInteger.h"

namespace como {
namespace math {

class Primality
{
public:
    static ECode NextProbablePrime(
        /* [in] */ BigInteger* n,
        /* [out] */ IBigInteger** value);

private:
    Primality();

    /** All BigInteger prime numbers with bit length lesser than 10 bits. */
    static Array<IBigInteger*> GetBIprimes();

private:
    /** All prime numbers with bit length lesser than 10 bits. */
    static const Array<Integer> sPrimes;
};

}
}

#endif // __COMO_MATH_PRIMALITY_H__
