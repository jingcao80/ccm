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

#ifndef __COMO_CORE_MATH_H__
#define __COMO_CORE_MATH_H__

#include <comotypes.h>
#include <math.h>

namespace como {
namespace core {

class Math
{
public:
    static Double Log(
        /* [in] */ Double a);

    static Double Log10(
        /* [in] */ Double a);

    static Double Sqrt(
        /* [in] */ Double a);

    static Double IEEEremainder(
        /* [in] */ Double f1,
        /* [in] */ Double f2);

    static Double Ceil(
        /* [in] */ Double a);

    static Double Pow(
        /* [in] */ Double a,
        /* [in] */ Double b);

    COM_PUBLIC static Double Random();

    COM_PUBLIC static void SetRandomSeedInternal(
        /* [in] */ Long seed);

    COM_PUBLIC static Integer RandomIntInternal();

    COM_PUBLIC static Long RandomLongInternal();

    static Integer Abs(
        /* [in] */ Integer a);

    static Long Abs(
        /* [in] */ Long a);

    static Double Abs(
        /* [in] */ Double a);

    static Integer Max(
        /* [in] */ Integer a,
        /* [in] */ Integer b);

    static Long Max(
        /* [in] */ Long a,
        /* [in] */ Long b);

    COM_PUBLIC static Float Max(
        /* [in] */ Float a,
        /* [in] */ Float b);

    COM_PUBLIC static Double Max(
        /* [in] */ Double a,
        /* [in] */ Double b);

    static Integer Min(
        /* [in] */ Integer a,
        /* [in] */ Integer b);

    static Long Min(
        /* [in] */ Long a,
        /* [in] */ Long b);

    COM_PUBLIC static Float Min(
        /* [in] */ Float a,
        /* [in] */ Float b);

    COM_PUBLIC static Double Min(
        /* [in] */ Double a,
        /* [in] */ Double b);

    COM_PUBLIC static Integer Signum(
        /* [in] */ Integer i);

    COM_PUBLIC static Integer Signum(
        /* [in] */ Long l);

    COM_PUBLIC static Short ReverseBytes(
        /* [in] */ Short s);

    COM_PUBLIC static Integer ReverseBytes(
        /* [in] */ Integer i);

    COM_PUBLIC static Long ReverseBytes(
        /* [in] */ Long l);

    COM_PUBLIC static Double CopySign(
        /* [in] */ Double magnitude,
        /* [in] */ Double sign);

    COM_PUBLIC static Integer GetExponent(
        /* [in] */ Double d);

    COM_PUBLIC static Double Scalb(
        /* [in] */ Double d,
        /* [in] */ Integer scaleFactor);

    static Boolean IsNaN(
        /* [in] */ Float f);

    static Boolean IsNaN(
        /* [in] */ Double v);

    COM_PUBLIC static Boolean IsInfinite(
        /* [in] */ Double v);

    COM_PUBLIC static Boolean IsFinite(
        /* [in] */ Double d);

    COM_PUBLIC static Integer FloatToIntegerBits(
        /* [in] */ Float value);

    COM_PUBLIC static Integer FloatToRawIntegerBits(
        /* [in] */ Float value);

    COM_PUBLIC static Float IntegerBitsToFloat(
        /* [in] */ Integer value);

    COM_PUBLIC static Long DoubleToLongBits(
        /* [in] */ Double value);

    COM_PUBLIC static Long DoubleToRawLongBits(
        /* [in] */ Double value);

    COM_PUBLIC static Double LongBitsToDouble(
        /* [in] */ Long value);

    COM_PUBLIC static Integer HighestOneBit(
        /* [in] */ Integer i);

    COM_PUBLIC static Integer NumberOfLeadingZeros(
        /* [in] */ Integer value);

    COM_PUBLIC static Integer NumberOfTrailingZeros(
        /* [in] */ Integer value);

    COM_PUBLIC static Integer BitCount(
        /* [in] */ Integer i);

    COM_PUBLIC static Integer NumberOfLeadingZeros(
        /* [in] */ Long value);

    COM_PUBLIC static Integer Compare(
        /* [in] */ Double d1,
        /* [in] */ Double d2);

private:
    static Double PowerOfTwoD(
        /* [in] */ Integer n);

    static Double GetsTwoToTheDoubleScaleUp();

    static Double GetsTwoToTheDoubleScaleDown();

public:
    COM_PUBLIC static const Long LONG_POWERS_OF_TEN[];

private:
    static Long sNegativeZeroFloatBits;
    static Long sNegativeZeroDoubleBits;
};

inline Double Math::Log(
    /* [in] */ Double a)
{
    return log(a);
}

inline Double Math::Log10(
    /* [in] */ Double a)
{
    return log10(a);
}

inline Double Math::Sqrt(
    /* [in] */ Double a)
{
    return sqrt(a);
}

inline Double Math::IEEEremainder(
    /* [in] */ Double f1,
    /* [in] */ Double f2)
{
    return remainder(f1, f2);
}

inline Double Math::Ceil(
        /* [in] */ Double a)
{
    return ceil(a);
}

inline Double Math::Pow(
    /* [in] */ Double a,
    /* [in] */ Double b)
{
    return pow(a, b);
}

inline Integer Math::Abs(
    /* [in] */ Integer a)
{
    return (a < 0) ? -a : a;
}

inline Long Math::Abs(
    /* [in] */ Long a)
{
    return (a < 0) ? -a : a;
}

inline Double Math::Abs(
    /* [in] */ Double a)
{
    return LongBitsToDouble(0x7fffffffffffffffll & DoubleToRawLongBits(a));
}

inline Integer Math::Max(
    /* [in] */ Integer a,
    /* [in] */ Integer b)
{
    return (a >= b) ? a : b;
}

inline Long Math::Max(
    /* [in] */ Long a,
    /* [in] */ Long b)
{
    return (a >= b) ? a : b;
}

inline Integer Math::Min(
    /* [in] */ Integer a,
    /* [in] */ Integer b)
{
    return (a <= b) ? a : b;
}

inline Long Math::Min(
    /* [in] */ Long a,
    /* [in] */ Long b)
{
    return (a <= b) ? a : b;
}

inline Boolean Math::IsNaN(
    /* [in] */ Float f)
{
    return (f != f);
}

inline Boolean Math::IsNaN(
    /* [in] */ Double v)
{
    return (v != v);
}

}
}

#endif //__COMO_CORE_MATH_H__
