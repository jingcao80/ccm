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

#ifndef __CCM_MATH_BITLEVEL_H__
#define __CCM_MATH_BITLEVEL_H__

#include "ccm/math/CBigInteger.h"

namespace ccm {
namespace math {

class BitLevel
{
public:
    static Integer BitLength(
        /* [in] */ BigInteger* value);

    static Integer BitCount(
        /* [in] */ BigInteger* value);

    static Boolean TestBit(
        /* [in] */ BigInteger* value,
        /* [in] */ Integer n);

    static Boolean NonZeroDroppedBits(
        /* [in] */ Integer numberOfBits,
        /* [in] */ Array<Integer>& digits);

    static void ShiftLeftOneBit(
        /* [in] */ Array<Integer>& result,
        /* [in] */ Array<Integer>& source,
        /* [in] */ Integer srcLen);

    static ECode ShiftLeftOneBit(
        /* [in] */ BigInteger* source,
        /* [out] */ IBigInteger** result);

    static ECode ShiftRight(
        /* [in] */ BigInteger* source,
        /* [in] */ Integer count,
        /* [out] */ IBigInteger** result);

    static Boolean ShiftRight(
        /* [in] */ Array<Integer>& result,
        /* [in] */ Integer resultLen,
        /* [in] */ Array<Integer>& source,
        /* [in] */ Integer intCount,
        /* [in] */ Integer count);

    static ECode FlipBit(
        /* [in] */ BigInteger* value,
        /* [in] */ Integer n,
        /* [out] */ IBigInteger** result);

private:
    BitLevel();
};

inline Boolean BitLevel::TestBit(
    /* [in] */ BigInteger* value,
    /* [in] */ Integer n)
{
    value->PrepareRepresentation();
    return ((value->mDigits[n >> 5] & (1 << (n & 31))) != 0);
}

}
}

#endif // __CCM_MATH_BITLEVEL_H__
