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

#ifndef __CCM_MATH_LOGICAL_H__
#define __CCM_MATH_LOGICAL_H__

#include "ccm/math/CBigInteger.h"

namespace ccm {
namespace math {

class Logical
{
public:
    static ECode Not(
        /* [in] */ IBigInteger* value,
        /* [out] */ IBigInteger** result)
    {
        return NOERROR;
    }

    static ECode And(
        /* [in] */ IBigInteger* value,
        /* [in] */ IBigInteger* that,
        /* [out] */ IBigInteger** result)
    {
        return NOERROR;
    }

    static ECode AndNot(
        /* [in] */ BigInteger* value,
        /* [in] */ BigInteger* that,
        /* [out] */ IBigInteger** result);

    static ECode AddNotPositive(
        /* [in] */ BigInteger* value,
        /* [in] */ BigInteger* that,
        /* [out] */ IBigInteger** result);

    static ECode AddNotPositiveNegative(
        /* [in] */ BigInteger* positive,
        /* [in] */ BigInteger* negative,
        /* [out] */ IBigInteger** result);

    static ECode AddNotNegativePositive(
        /* [in] */ BigInteger* negative,
        /* [in] */ BigInteger* positive,
        /* [out] */ IBigInteger** result);

    static ECode AddNotNegative(
        /* [in] */ BigInteger* value,
        /* [in] */ BigInteger* that,
        /* [out] */ IBigInteger** result);

    static ECode Or(
        /* [in] */ IBigInteger* value,
        /* [in] */ IBigInteger* that,
        /* [out] */ IBigInteger** result)
    {
        return NOERROR;
    }

    static ECode Xor(
        /* [in] */ IBigInteger* value,
        /* [in] */ IBigInteger* that,
        /* [out] */ IBigInteger** result)
    {
        return NOERROR;
    }

private:
    Logical();
};

}
}

#endif // __CCM_MATH_LOGICAL_H__
