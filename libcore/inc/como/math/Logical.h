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

#ifndef __COMO_MATH_LOGICAL_H__
#define __COMO_MATH_LOGICAL_H__

#include "como/math/CBigInteger.h"

namespace como {
namespace math {

class Logical
{
public:
    static ECode Not(
        /* [in] */ BigInteger* value,
        /* [out] */ AutoPtr<IBigInteger>& result);

    static ECode And(
        /* [in] */ BigInteger* value,
        /* [in] */ BigInteger* that,
        /* [out] */ AutoPtr<IBigInteger>& result);

    static ECode AddPositive(
        /* [in] */ BigInteger* value,
        /* [in] */ BigInteger* that,
        /* [out] */ AutoPtr<IBigInteger>& result);

    static ECode AddDiffSigns(
        /* [in] */ BigInteger* positive,
        /* [in] */ BigInteger* negative,
        /* [out] */ AutoPtr<IBigInteger>& result);

    static ECode AddNegative(
        /* [in] */ BigInteger* longer,
        /* [in] */ BigInteger* shorter,
        /* [out] */ AutoPtr<IBigInteger>& result);

    static ECode AndNot(
        /* [in] */ BigInteger* value,
        /* [in] */ BigInteger* that,
        /* [out] */ AutoPtr<IBigInteger>& result);

    static ECode AddNotPositive(
        /* [in] */ BigInteger* value,
        /* [in] */ BigInteger* that,
        /* [out] */ AutoPtr<IBigInteger>& result);

    static ECode AddNotPositiveNegative(
        /* [in] */ BigInteger* positive,
        /* [in] */ BigInteger* negative,
        /* [out] */ AutoPtr<IBigInteger>& result);

    static ECode AddNotNegativePositive(
        /* [in] */ BigInteger* negative,
        /* [in] */ BigInteger* positive,
        /* [out] */ AutoPtr<IBigInteger>& result);

    static ECode AddNotNegative(
        /* [in] */ BigInteger* value,
        /* [in] */ BigInteger* that,
        /* [out] */ AutoPtr<IBigInteger>& result);

    static ECode Or(
        /* [in] */ BigInteger* value,
        /* [in] */ BigInteger* that,
        /* [out] */ AutoPtr<IBigInteger>& result);

    static ECode OrPositive(
        /* [in] */ BigInteger* longer,
        /* [in] */ BigInteger* shorter,
        /* [out] */ AutoPtr<IBigInteger>& result);

    static ECode OrNegative(
        /* [in] */ BigInteger* value,
        /* [in] */ BigInteger* that,
        /* [out] */ AutoPtr<IBigInteger>& result);

    static ECode OrDiffSigns(
        /* [in] */ BigInteger* positive,
        /* [in] */ BigInteger* negative,
        /* [out] */ AutoPtr<IBigInteger>& result);

    static ECode Xor(
        /* [in] */ BigInteger* value,
        /* [in] */ BigInteger* that,
        /* [out] */ AutoPtr<IBigInteger>& result);

    static ECode XorPositive(
        /* [in] */ BigInteger* longer,
        /* [in] */ BigInteger* shorter,
        /* [out] */ AutoPtr<IBigInteger>& result);

    static ECode XorNegative(
        /* [in] */ BigInteger* value,
        /* [in] */ BigInteger* that,
        /* [out] */ AutoPtr<IBigInteger>& result);

    static ECode XorDiffSigns(
        /* [in] */ BigInteger* positive,
        /* [in] */ BigInteger* negative,
        /* [out] */ AutoPtr<IBigInteger>& result);

private:
    Logical();
};

}
}

#endif // __COMO_MATH_LOGICAL_H__
