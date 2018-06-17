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

}
}
