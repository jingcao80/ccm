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

#ifndef __CCM_CORE_MATH_H__
#define __CCM_CORE_MATH_H__

#include <ccmtypes.h>
#include <math.h>

namespace ccm {
namespace core {

class Math
{
public:
    static Double Ceil(
        /* [in] */ Double a);

    static Double Log(
        /* [in] */ Double a);

    static Integer Max(
        /* [in] */ Integer a,
        /* [in] */ Integer b);

    static Integer Min(
        /* [in] */ Integer a,
        /* [in] */ Integer b);

    static Integer Signum(
        /* [in] */ Long i);

    COM_PUBLIC static Integer FloatToRawIntBits(
        /* [in] */ Float value);

    COM_PUBLIC static Float IntBitsToFloat(
        /* [in] */ Integer value);

    COM_PUBLIC static Long DoubleToRawLongBits(
        /* [in] */ Double value);

    COM_PUBLIC static Double LongBitsToDouble(
        /* [in] */ Long value);

    COM_PUBLIC static Integer NumberOfLeadingZeros(
        /* [in] */ Long value);

public:
    COM_PUBLIC static const Long LONG_POWERS_OF_TEN[];
};

inline Double Math::Ceil(
        /* [in] */ Double a)
{
    return ceil(a);
}

inline Double Math::Log(
    /* [in] */ Double a)
{
    return log(a);
}

inline Integer Math::Max(
    /* [in] */ Integer a,
    /* [in] */ Integer b)
{
    return (a >= b) ? a : b;
}

inline Integer Math::Min(
    /* [in] */ Integer a,
    /* [in] */ Integer b)
{
    return (a <= b) ? a : b;
}

inline Integer Math::Signum(
    /* [in] */ Long i)
{
    // HD, Section 2-7
    return (Integer) ((i >> 63) | (((unsigned Long)-i) >> 63));
}

}
}

#endif //__CCM_CORE_MATH_H__