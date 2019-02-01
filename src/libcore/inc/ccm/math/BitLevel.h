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

#ifndef __CCM_MATH_BITLEVEL_H__
#define __CCM_MATH_BITLEVEL_H__

#include "ccm/math/CBigInteger.h"

namespace ccm {
namespace math {

class BitLevel
{
public:
    static Integer BitLength(
        /* [in] */ IBigInteger* value)
    {
        return NOERROR;
    }

    static Integer BitCount(
        /* [in] */ IBigInteger* value)
    {
        return NOERROR;
    }

    static Boolean NonZeroDroppedBits(
        /* [in] */ Integer numberOfBits,
        /* [in] */ Array<Integer>& digits)
    {
        return false;
    }

    static ECode ShiftLeftOneBit(
        /* [in] */ IBigInteger* source,
        /* [out] */ IBigInteger** result)
    {
        return NOERROR;
    }

    static ECode ShiftRight(
        /* [in] */ IBigInteger* source,
        /* [in] */ Integer count,
        /* [out] */ IBigInteger** result)
    {
        return NOERROR;
    }

    static ECode FlipBit(
        /* [in] */ IBigInteger* value,
        /* [in] */ Integer n,
        /* [out] */ IBigInteger** result)
    {
        return NOERROR;
    }

private:
    BitLevel();
};

}
}

#endif // __CCM_MATH_BITLEVEL_H__
