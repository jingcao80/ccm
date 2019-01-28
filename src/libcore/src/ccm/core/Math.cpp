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

#include "ccm/core/Math.h"
#include "ccm/misc/DoubleConsts.h"
#include "ccm/util/CRandom.h"
#include "ccm.core.IDouble.h"
#include "ccm.util.IRandom.h"

using ccm::misc::DoubleConsts;
using ccm::util::CRandom;
using ccm::util::IRandom;
using ccm::util::IID_IRandom;

namespace ccm {
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

Double Math::Random()
{
    Double rv;
    GetRandomNumberGenerator()->NextDouble(&rv);
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
    GetRandomNumberGenerator()->NextInteger(&rv);
    return rv;
}

Long Math::RandomLongInternal()
{
    Long rv;
    GetRandomNumberGenerator()->NextLong(&rv);
    return rv;
}

Boolean Math::IsInfinite(
    /* [in] */ Double v)
{
    return (v == IDouble::POSITIVE_INFINITY) || (v == IDouble::NEGATIVE_INFINITY);
}

union FloatInteger
{
    unsigned int bits;
    Float f;
};

Integer Math::FloatToRawIntBits(
    /* [in] */ Float value)
{
    FloatInteger f;
    f.f = value;
    return f.bits;
}

Float Math::IntBitsToFloat(
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

Integer Math::NumberOfLeadingZeros(
    /* [in] */ Integer value)
{
    if (value == 0) {
        return 32;
    }
    Integer n = 1;
    Integer i = value;
    if (((unsigned Integer)i) >> 16 == 0) { n += 16; i <<= 16; }
    if (((unsigned Integer)i) >> 24 == 0) { n +=  8; i <<=  8; }
    if (((unsigned Integer)i) >> 28 == 0) { n +=  4; i <<=  4; }
    if (((unsigned Integer)i) >> 30 == 0) { n +=  2; i <<=  2; }
    n -= ((unsigned Integer)i) >> 31;
    return n;
}

Integer Math::NumberOfLeadingZeros(
    /* [in] */ Long value)
{
    if (value == 0) {
        return 64;
    }
    Integer n = 1;
    Integer x = (Integer)(((unsigned Long)value) >> 32);
    if (x == 0) { n += 32; x = (Integer)value; }
    if (((unsigned Integer)x) >> 16 == 0) { n += 16; x <<= 16; }
    if (((unsigned Integer)x) >> 24 == 0) { n +=  8; x <<=  8; }
    if (((unsigned Integer)x) >> 28 == 0) { n +=  4; x <<=  4; }
    if (((unsigned Integer)x) >> 30 == 0) { n +=  2; x <<=  2; }
    n -= ((unsigned Integer)x) >> 31;
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
