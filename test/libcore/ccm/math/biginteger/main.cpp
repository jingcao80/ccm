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

#include <ccmautoptr.h>
#include <ccmobject.h>
#include "ccm.core.ILong.h"
#include "ccm.core.INumber.h"
#include "ccm.math.CBigInteger.h"
#include "ccm.math.CBigIntegerFactory.h"
#include "ccm.math.IBigInteger.h"
#include "ccm.math.IBigIntegerFactory.h"
#include "ccm.util.CRandom.h"
#include "ccm.util.IRandom.h"
#include <gtest/gtest.h>

using ccm::core::ILong;
using ccm::core::INumber;
using ccm::math::CBigInteger;
using ccm::math::CBigIntegerFactory;
using ccm::math::IBigInteger;
using ccm::math::IBigIntegerFactory;
using ccm::math::IID_IBigInteger;
using ccm::math::IID_IBigIntegerFactory;
using ccm::util::CRandom;
using ccm::util::IRandom;
using ccm::util::IID_IRandom;

TEST(BigIntegerTest, HashCodeTest)
{
    AutoPtr<IBigInteger> firstBig;
    CBigInteger::New(String("3000354366789831885"), IID_IBigInteger, (IInterface**)&firstBig);
    AutoPtr<IBigInteger> secondBig;
    CBigInteger::New(String("3298535022597"), IID_IBigInteger, (IInterface**)&secondBig);
    AutoPtr<IBigInteger> andedBigs;
    firstBig->Add(secondBig, &andedBigs);
    Long lv;
    INumber::Probe(andedBigs)->LongValue(&lv);
    AutoPtr<IBigIntegerFactory> factory;
    CBigIntegerFactory::New(IID_IBigIntegerFactory, (IInterface**)&factory);
    AutoPtr<IBigInteger> toCompareBig;
    factory->ValueOf(lv, &toCompareBig);
    EXPECT_TRUE(Object::Equals(andedBigs, toCompareBig));
}

TEST(BigIntegerTest, ValueOfTest)
{
    AutoPtr<IBigIntegerFactory> factory;
    CBigIntegerFactory::New(IID_IBigIntegerFactory, (IInterface**)&factory);
    for (Integer i = -1024; i <= 1024; ++i) {
        AutoPtr<IBigInteger> bi;
        factory->ValueOf(i, &bi);
        Integer iv;
        INumber::Probe(bi)->IntegerValue(&iv);
        EXPECT_EQ(i, iv);
    }
}

TEST(BigIntegerTest, ConstructorIntegerIRandomTest)
{
    AutoPtr<IRandom> rand;
    CRandom::New(IID_IRandom, (IInterface**)&rand);
    for (Integer i = 0; i < 1024; ++i) {
        AutoPtr<IBigInteger> b;
        CBigInteger::New(128, rand, IID_IBigInteger, (IInterface**)&b);
        Integer len;
        b->BitLength(&len);
        EXPECT_TRUE(len <= 128);

        b = nullptr;
        CBigInteger::New(16, rand, IID_IBigInteger, (IInterface**)&b);
        b->BitLength(&len);
        EXPECT_TRUE(len <= 16);

        b = nullptr;
        CBigInteger::New(5, rand, IID_IBigInteger, (IInterface**)&b);
        b->BitLength(&len);
        EXPECT_TRUE(len <= 5);
    }
}

TEST(BigIntegerTest, ConstructorIntegerIntegerIRandomTest)
{
    AutoPtr<IRandom> rand;
    CRandom::New(IID_IRandom, (IInterface**)&rand);
    for (Integer i = 0; i < 1024; ++i) {
        AutoPtr<IBigInteger> b;
        CBigInteger::New(128, 100, rand, IID_IBigInteger, (IInterface**)&b);
        Integer len;
        b->BitLength(&len);
        EXPECT_EQ(128, len);
        Boolean pp;
        b->IsProbablePrime(100, &pp);
        EXPECT_TRUE(pp);

        b = nullptr;
        CBigInteger::New(16, 100, rand, IID_IBigInteger, (IInterface**)&b);
        b->BitLength(&len);
        EXPECT_EQ(16, len);
        b->IsProbablePrime(100, &pp);
        EXPECT_TRUE(pp);

        b = nullptr;
        CBigInteger::New(5, 100, rand, IID_IBigInteger, (IInterface**)&b);
        b->BitLength(&len);
        EXPECT_EQ(5, len);
        b->IsProbablePrime(100, &pp);
        EXPECT_TRUE(pp);
    }

    Array<Integer> primes(1024);
    Boolean saw2 = false;
    Boolean saw3 = false;
    for (Integer i = 0; i < primes.GetLength(); ++i) {
        AutoPtr<IBigInteger> b;
        CBigInteger::New(2, 100, rand, IID_IBigInteger, (IInterface**)&b);
        Integer len;
        b->BitLength(&len);
        EXPECT_EQ(2, len);
        Boolean pp;
        b->IsProbablePrime(100, &pp);
        EXPECT_TRUE(pp);
        INumber::Probe(b)->IntegerValue(&primes[i]);
    }
    for (Integer i = 0; i < primes.GetLength(); ++i) {
        if (primes[i] == 2) {
            saw2 = true;
        }
        else if (primes[i] == 3) {
            saw3 = true;
        }
        else {
            EXPECT_FALSE(true);
        }
    }
    EXPECT_TRUE(saw2 && saw3);
}

TEST(BigIntegerTest, NegativeValuesSuperfluousZerosTest)
{
    Array<Byte> trimmedBytes = {
        (Byte)0xae, (Byte)0x0f, (Byte)0xa1, (Byte)0x93
    };
    Array<Byte> extraZeroesBytes = {
        (Byte)0xff, (Byte)0xae, (Byte)0x0f, (Byte)0xa1, (Byte)0x93
    };

    AutoPtr<IBigInteger> trimmed;
    CBigInteger::New(trimmedBytes, IID_IBigInteger, (IInterface**)&trimmed);
    AutoPtr<IBigInteger> extraZeroes;
    CBigInteger::New(extraZeroesBytes, IID_IBigInteger, (IInterface**)&extraZeroes);

    EXPECT_TRUE(Object::Equals(trimmed, extraZeroes));
}

TEST(BigIntegerTest, PositiveValuesSuperfluousZerosTest)
{
    Array<Byte> trimmedBytes = {
        (Byte)0x2e, (Byte)0x0f, (Byte)0xa1, (Byte)0x93
    };
    Array<Byte> extraZeroesBytes = {
        (Byte)0x00, (Byte)0x2e, (Byte)0x0f, (Byte)0xa1, (Byte)0x93
    };

    AutoPtr<IBigInteger> trimmed;
    CBigInteger::New(trimmedBytes, IID_IBigInteger, (IInterface**)&trimmed);
    AutoPtr<IBigInteger> extraZeroes;
    CBigInteger::New(extraZeroesBytes, IID_IBigInteger, (IInterface**)&extraZeroes);

    EXPECT_TRUE(Object::Equals(trimmed, extraZeroes));
}

TEST(BigIntegerTest, DivideTest)
{
    AutoPtr<IBigIntegerFactory> factory;
    CBigIntegerFactory::New(IID_IBigIntegerFactory, (IInterface**)&factory);
    AutoPtr<IBigInteger> negV;
    factory->ValueOf(ILong::MIN_VALUE, &negV);
    AutoPtr<IBigInteger> divisor;
    factory->ValueOf(-1, &divisor);
    AutoPtr<IBigInteger> posV;
    negV->Divide(divisor, &posV);
    EXPECT_STREQ("-9223372036854775808", Object::ToString(negV).string());
    EXPECT_STREQ("9223372036854775808", Object::ToString(posV).string());
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
