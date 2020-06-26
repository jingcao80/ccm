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

#include "innerdef.h"
#include "como/core/Math.h"
#include "como/misc/DoubleConsts.h"
#include "como/misc/FloatConsts.h"
#include "como/util/CRandom.h"
#include "como.core.IDouble.h"
#include "como.util.IRandom.h"

using como::misc::DoubleConsts;
using como::misc::FloatConsts;
using como::util::CRandom;
using como::util::IRandom;
using como::util::IID_IRandom;

namespace como {
namespace core {

const Long Math::LONG_POWERS_OF_TEN[] = {
    1ll,
    10ll,
    100ll,
    1000ll,
    10000ll,
    100000ll,
    1000000ll,
    10000000ll,
    100000000ll,
    1000000000ll,
    10000000000ll,
    100000000000ll,
    1000000000000ll,
    10000000000000ll,
    100000000000000ll,
    1000000000000000ll,
    10000000000000000ll,
    100000000000000000ll,
    1000000000000000000ll,
};

Long Math::sNegativeZeroFloatBits = FloatToRawIntegerBits(-0.0f);
Long Math::sNegativeZeroDoubleBits = DoubleToRawLongBits(-0.0);

static AutoPtr<IRandom> CreateRandom()
{
    AutoPtr<IRandom> random;
    CRandom::New(IID_IRandom, (IInterface**)&random);
    return random;
}

static AutoPtr<IRandom> GetRandomNumberGenerator()
{
    static AutoPtr<IRandom> sRandomNumberGenerator = CreateRandom();
    return sRandomNumberGenerator;
}

Double Math::GetsTwoToTheDoubleScaleUp()
{
    static Double sTwoToTheDoubleScaleUp = PowerOfTwoD(512);
    return sTwoToTheDoubleScaleUp;
}

Double Math::GetsTwoToTheDoubleScaleDown()
{
    static Double sTwoToTheDoubleScaleDown = PowerOfTwoD(-512);
    return sTwoToTheDoubleScaleDown;
}

Double Math::Random()
{
    Double rv;
    GetRandomNumberGenerator()->NextDouble(rv);
    return rv;
}

void Math::SetRandomSeedInternal(
    /* [in] */ Long seed)
{
    GetRandomNumberGenerator()->SetSeed(seed);
}

Integer Math::RandomIntInternal()
{
    Integer rv;
    GetRandomNumberGenerator()->NextInteger(rv);
    return rv;
}

Long Math::RandomLongInternal()
{
    Long rv;
    GetRandomNumberGenerator()->NextLong(rv);
    return rv;
}

Float Math::Max(
    /* [in] */ Float a,
    /* [in] */ Float b)
{
    if (a != a) {
        // a is NaN
        return a;
    }
    if ((a == 0.0f) &&
        (b == 0.0f) &&
        (FloatToRawIntegerBits(a) == sNegativeZeroFloatBits)) {
        // Raw conversion ok since NaN can't map to -0.0.
        return b;
    }
    return (a >= b) ? a : b;
}

Double Math::Max(
    /* [in] */ Double a,
    /* [in] */ Double b)
{
    if (a != a) {
        // a is NaN
        return a;
    }
    if ((a == 0.0) &&
        (b == 0.0) &&
        (DoubleToRawLongBits(a) == sNegativeZeroDoubleBits)) {
        // Raw conversion ok since NaN can't map to -0.0.
        return b;
    }
    return (a >= b) ? a : b;
}

Float Math::Min(
    /* [in] */ Float a,
    /* [in] */ Float b)
{
    if (a != a) {
        // a is NaN
        return a;
    }
    if ((a == 0.0f) &&
        (b == 0.0f) &&
        (FloatToRawIntegerBits(b) == sNegativeZeroFloatBits)) {
        // Raw conversion ok since NaN can't map to -0.0.
        return b;
    }
    return (a <= b) ? a : b;
}

Double Math::Min(
    /* [in] */ Double a,
    /* [in] */ Double b)
{
    if (a != a) {
        // a is NaN
        return a;
    }
    if ((a == 0.0) &&
        (b == 0.0) &&
        (DoubleToRawLongBits(b) == sNegativeZeroDoubleBits)) {
        // Raw conversion ok since NaN can't map to -0.0.
        return b;
    }
    return (a <= b) ? a : b;
}

Integer Math::Signum(
    /* [in] */ Integer i)
{
    return (i >> 31) | (((UInteger)-i) >> 31);
}

Integer Math::Signum(
    /* [in] */ Long l)
{
    return (Integer) ((l >> 63) | (((ULong)-l) >> 63));
}

Short Math::ReverseBytes(
    /* [in] */ Short s)
{
    return (Short) (((s & 0xFF00) >> 8) | (s << 8));
}

Integer Math::ReverseBytes(
    /* [in] */ Integer i)
{
    return ((((UInteger)i) >> 24)) |
           ((i >> 8) & 0xFF00) |
           ((i << 8) & 0xFF0000) |
           ((i << 24));
}

Long Math::ReverseBytes(
    /* [in] */ Long l)
{
    l = (l & 0x00ff00ff00ff00ffLL) << 8 | ((((ULong)l) >> 8) & 0x00ff00ff00ff00ffLL);
    return (l << 48) | ((l & 0xffff0000LL) << 16) |
            ((((ULong)l) >> 16) & 0xffff0000LL) | (((ULong)l) >> 48);
}

Double Math::CopySign(
    /* [in] */ Double magnitude,
    /* [in] */ Double sign)
{
    return LongBitsToDouble((DoubleToRawLongBits(sign) &
            (DoubleConsts::SIGNIF_BIT_MASK)) |
            (DoubleToRawLongBits(magnitude) &
            (DoubleConsts::EXP_BIT_MASK |
            DoubleConsts::SIGNIF_BIT_MASK)));
}

Integer Math::GetExponent(
    /* [in] */ Double d)
{
    /*
     * Bitwise convert d to long, mask out exponent bits, shift
     * to the right and then subtract out double's bias adjust to
     * get true exponent value.
     */
    return (Integer)(((DoubleToRawLongBits(d) & DoubleConsts::EXP_BIT_MASK) >>
            (DoubleConsts::SIGNIFICAND_WIDTH - 1)) - DoubleConsts::EXP_BIAS);
}

Double Math::Scalb(
    /* [in] */ Double d,
    /* [in] */ Integer scaleFactor)
{
    /*
     * This method does not need to be declared strictfp to
     * compute the same correct result on all platforms.  When
     * scaling up, it does not matter what order the
     * multiply-store operations are done; the result will be
     * finite or overflow regardless of the operation ordering.
     * However, to get the correct result when scaling down, a
     * particular ordering must be used.
     *
     * When scaling down, the multiply-store operations are
     * sequenced so that it is not possible for two consecutive
     * multiply-stores to return subnormal results.  If one
     * multiply-store result is subnormal, the next multiply will
     * round it away to zero.  This is done by first multiplying
     * by 2 ^ (scaleFactor % n) and then multiplying several
     * times by by 2^n as needed where n is the exponent of number
     * that is a covenient power of two.  In this way, at most one
     * real rounding error occurs.  If the double value set is
     * being used exclusively, the rounding will occur on a
     * multiply.  If the double-extended-exponent value set is
     * being used, the products will (perhaps) be exact but the
     * stores to d are guaranteed to round to the double value
     * set.
     *
     * It is _not_ a valid implementation to first multiply d by
     * 2^MIN_EXPONENT and then by 2 ^ (scaleFactor %
     * MIN_EXPONENT) since even in a strictfp program double
     * rounding on underflow could occur; e.g. if the scaleFactor
     * argument was (MIN_EXPONENT - n) and the exponent of d was a
     * little less than -(MIN_EXPONENT - n), meaning the final
     * result would be subnormal.
     *
     * Since exact reproducibility of this method can be achieved
     * without any undue performance burden, there is no
     * compelling reason to allow double rounding on underflow in
     * scalb.
     */

    // magnitude of a power of two so large that scaling a finite
    // nonzero value by it would be guaranteed to over or
    // underflow; due to rounding, scaling down takes takes an
    // additional power of two which is reflected here
    Integer MAX_SCALE = DoubleConsts::MAX_EXPONENT + -DoubleConsts::MIN_EXPONENT +
            DoubleConsts::SIGNIFICAND_WIDTH + 1;
    Integer exp_adjust = 0;
    Integer scale_increment = 0;
    Double exp_delta = IDouble::NaN;

    // Make sure scaling factor is in a reasonable range

    if (scaleFactor < 0) {
        scaleFactor = Math::Max(scaleFactor, -MAX_SCALE);
        scale_increment = -512;
        exp_delta = GetsTwoToTheDoubleScaleDown();
    }
    else {
        scaleFactor = Math::Min(scaleFactor, MAX_SCALE);
        scale_increment = 512;
        exp_delta = GetsTwoToTheDoubleScaleUp();
    }

    // Calculate (scaleFactor % +/-512), 512 = 2^9, using
    // technique from "Hacker's Delight" section 10-2.
    Integer t = ((UInteger)(scaleFactor >> (9 - 1))) >> (32 - 9);
    exp_adjust = ((scaleFactor + t) & (512 - 1)) - t;

    d *= PowerOfTwoD(exp_adjust);
    scaleFactor -= exp_adjust;

    while (scaleFactor != 0) {
        d *= exp_delta;
        scaleFactor -= scale_increment;
    }
    return d;
}

Double Math::PowerOfTwoD(
    /* [in] */ Integer n)
{
    CHECK(n >= DoubleConsts::MIN_EXPONENT && n <= DoubleConsts::MAX_EXPONENT);
    return LongBitsToDouble((((Long)n + (Long)DoubleConsts::EXP_BIAS) <<
            (DoubleConsts::SIGNIFICAND_WIDTH - 1)) & DoubleConsts::EXP_BIT_MASK);
}

Boolean Math::IsInfinite(
    /* [in] */ Double v)
{
    return (v == IDouble::POSITIVE_INFINITY) || (v == IDouble::NEGATIVE_INFINITY);
}

Boolean Math::IsFinite(
    /* [in] */ Double d)
{
    return Abs(d) <= DoubleConsts::MAX_VALUE;
}

union FloatInteger
{
    unsigned int bits;
    Float f;
};

Integer Math::FloatToIntegerBits(
    /* [in] */ Float value)
{
    Integer result = FloatToRawIntegerBits(value);
    // Check for NaN based on values of bit fields, maximum
    // exponent and nonzero significand.
    if ((result & FloatConsts::EXP_BIT_MASK) == FloatConsts::EXP_BIT_MASK &&
            (result & FloatConsts::SIGNIF_BIT_MASK) != 0) {
        result = 0x7fc00000;
    }
    return result;
}

Integer Math::FloatToRawIntegerBits(
    /* [in] */ Float value)
{
    FloatInteger f;
    f.f = value;
    return f.bits;
}

Float Math::IntegerBitsToFloat(
    /* [in] */ Integer value)
{
    FloatInteger f;
    f.bits = value;
    return f.f;
}

Long Math::DoubleToLongBits(
    /* [in] */ Double value)
{
    Long result = DoubleToRawLongBits(value);
    // Check for NaN based on values of bit fields, maximum
    // exponent and nonzero significand.
    if (((result & DoubleConsts::EXP_BIT_MASK) ==
            DoubleConsts::EXP_BIT_MASK) &&
            (result & DoubleConsts::SIGNIF_BIT_MASK) != 0ll) {
        result = 0x7ff8000000000000ll;
    }
    return result;
}

union DoubleInteger
{
    uint64_t bits;
    Double d;
};

Long Math::DoubleToRawLongBits(
    /* [in] */ Double value)
{
    DoubleInteger d;
    d.d = value;
    return d.bits;
}

Double Math::LongBitsToDouble(
    /* [in] */ Long value)
{
    DoubleInteger d;
    d.bits = value;
    return d.d;
}

Integer Math::HighestOneBit(
    /* [in] */ Integer i)
{
    i |= (i >> 1);
    i |= (i >> 2);
    i |= (i >> 4);
    i |= (i >> 8);
    i |= (i >> 16);
    return i - (((UInteger)i) >> 1);
}

Integer Math::NumberOfLeadingZeros(
    /* [in] */ Integer value)
{
    if (value == 0) {
        return 32;
    }
    Integer n = 1;
    Integer i = value;
    if (((UInteger)i) >> 16 == 0) { n += 16; i <<= 16; }
    if (((UInteger)i) >> 24 == 0) { n +=  8; i <<=  8; }
    if (((UInteger)i) >> 28 == 0) { n +=  4; i <<=  4; }
    if (((UInteger)i) >> 30 == 0) { n +=  2; i <<=  2; }
    n -= ((UInteger)i) >> 31;
    return n;
}

Integer Math::NumberOfTrailingZeros(
    /* [in] */ Integer value)
{
    Integer y;
    if (value == 0) return 32;
    Integer n = 31;
    Integer i = value;
    y = i <<16; if (y != 0) { n = n -16; i = y; }
    y = i << 8; if (y != 0) { n = n - 8; i = y; }
    y = i << 4; if (y != 0) { n = n - 4; i = y; }
    y = i << 2; if (y != 0) { n = n - 2; i = y; }
    n -= ((UInteger)(i << 1)) >> 31;
    return n;
}

Integer Math::BitCount(
    /* [in] */ Integer i)
{
    i = i - ((((UInteger)i) >> 1) & 0x55555555);
    i = (i & 0x33333333) + ((((UInteger)i) >> 2) & 0x33333333);
    i = (i + (((UInteger)i) >> 4)) & 0x0f0f0f0f;
    i = i + (((UInteger)i) >> 8);
    i = i + (((UInteger)i) >> 16);
    return i & 0x3f;
}

Integer Math::NumberOfLeadingZeros(
    /* [in] */ Long value)
{
    if (value == 0) {
        return 64;
    }
    Integer n = 1;
    Integer x = (Integer)(((ULong)value) >> 32);
    if (x == 0) { n += 32; x = (Integer)value; }
    if (((UInteger)x) >> 16 == 0) { n += 16; x <<= 16; }
    if (((UInteger)x) >> 24 == 0) { n +=  8; x <<=  8; }
    if (((UInteger)x) >> 28 == 0) { n +=  4; x <<=  4; }
    if (((UInteger)x) >> 30 == 0) { n +=  2; x <<=  2; }
    n -= ((UInteger)x) >> 31;
    return n;
}

Integer Math::Compare(
    /* [in] */ Double d1,
    /* [in] */ Double d2)
{
    if (d1 < d2) {
        return -1;           // Neither val is NaN, thisVal is smaller
    }
    if (d1 > d2) {
        return 1;            // Neither val is NaN, thisVal is larger
    }

    // Cannot use doubleToRawLongBits because of possibility of NaNs.
    Long thisBits = DoubleToLongBits(d1);
    Long anotherBits = DoubleToLongBits(d2);

    return (thisBits == anotherBits ?  0 : // Values are equal
            (thisBits < anotherBits ? -1 : // (-0.0, 0.0) or (!NaN, NaN)
             1));                          // (0.0, -0.0) or (NaN, !NaN)
}

}
}
