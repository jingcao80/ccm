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

#include <como/core/Math.h>
#include <como/core/StringUtils.h>
#include "como/math/BigDecimalFactory.h"
#include "como/math/BigIntegerFactory.h"
#include "como.core.IComparable.h"
#include "como.core.IInteger.h"
#include "como.core.ILong.h"
#include "como.math.CBigDecimal.h"
#include "como.math.CMathContext.h"
#include "como.math.IBigDecimal.h"
#include "como.math.IBigInteger.h"
#include "como.math.IMathContext.h"
#include <ccmautoptr.h>
#include <ccmobject.h>
#include <gtest/gtest.h>

using como::Object;
using como::core::IComparable;
using como::core::IInteger;
using como::core::ILong;
using como::core::Math;
using como::core::StringUtils;
using como::math::BigDecimalFactory;
using como::math::BigIntegerFactory;
using como::math::CBigDecimal;
using como::math::CMathContext;
using como::math::IBigDecimal;
using como::math::IBigInteger;
using como::math::IID_IBigDecimal;
using como::math::IID_IMathContext;
using como::math::IMathContext;
using como::math::RoundingMode;

void AssertPrecision(
    /* [in] */ Integer expectedPrecision,
    /* [in] */ const String& value)
{
    AutoPtr<IBigDecimal> parsed;
    CBigDecimal::New(value, IID_IBigDecimal, (IInterface**)&parsed);
    Integer precision;
    parsed->Precision(&precision);
    EXPECT_EQ(expectedPrecision, precision);

    AutoPtr<IBigDecimal> ONE;
    BigDecimalFactory::GetONE(&ONE);
    AutoPtr<IBigDecimal> computed;
    parsed->Divide(ONE, &computed);
    computed->Precision(&precision);
    EXPECT_EQ(expectedPrecision, precision);
}

TEST(BigDecimalTest, TestGetPrecision)
{
    AssertPrecision(1, String("0"));
    AssertPrecision(1, String("0.9"));
    AssertPrecision(16, String("0.9999999999999999"));
    AssertPrecision(16, String("9999999999999999"));
    AssertPrecision(19, String("1000000000000000000"));
    AssertPrecision(19, String("1000000000000000001"));
    AssertPrecision(19, String("-1000000000000000001"));
    AssertPrecision(19, String("-1000000000000000000"));

    String tenNines("9999999999");
    String fiftyNines = tenNines + tenNines + tenNines + tenNines + tenNines;
    AssertPrecision(10, String("0.") + tenNines);
    AssertPrecision(50, String("0.") + fiftyNines);
    AssertPrecision(250, String("0.") + fiftyNines + fiftyNines + fiftyNines + fiftyNines + fiftyNines);
    AssertPrecision(10, tenNines);
    AssertPrecision(50, fiftyNines);
    AssertPrecision(250, fiftyNines + fiftyNines + fiftyNines + fiftyNines + fiftyNines);

    // test these special cases because we know precision() uses longs internally
    String maxLong = StringUtils::ToString(ILong::MAX_VALUE);
    AssertPrecision(maxLong.GetLength(), maxLong);
    String minLong = StringUtils::ToString(ILong::MIN_VALUE);
    AssertPrecision(minLong.GetLength() - 1, minLong);
}

TEST(BigDecimalTest, TestRound)
{
    AutoPtr<IBigDecimal> bigDecimal;
    CBigDecimal::New(String("0.999999999999999"), IID_IBigDecimal, (IInterface**)&bigDecimal);
    AutoPtr<IMathContext> mc;
    CMathContext::New(2, RoundingMode::FLOOR, IID_IMathContext, (IInterface**)&mc);
    AutoPtr<IBigDecimal> rounded;
    bigDecimal->Round(mc, &rounded);
    EXPECT_STREQ("0.99", Object::ToString(rounded).string());
}

TEST(BigDecimalTest, TestPrecisionFromString)
{
    AutoPtr<IBigDecimal> a;
    CBigDecimal::New(String("-0.011111111111111111111"), IID_IBigDecimal, (IInterface**)&a);
    AutoPtr<IBigDecimal> ONE;
    BigDecimalFactory::GetONE(&ONE);
    AutoPtr<IBigDecimal> b;
    a->Multiply(ONE, &b);

    EXPECT_STREQ("-0.011111111111111111111", Object::ToString(a).string());
    EXPECT_STREQ("-0.011111111111111111111", Object::ToString(b).string());

    Integer precision;
    a->Precision(&precision);
    EXPECT_EQ(20, precision);
    b->Precision(&precision);
    EXPECT_EQ(20, precision);

    Integer scale;
    a->Scale(&scale);
    EXPECT_EQ(21, scale);
    EXPECT_EQ(21, scale);

    AutoPtr<IBigInteger> aBI, bBI;
    a->UnscaledValue(&aBI);
    b->UnscaledValue(&bBI);
    EXPECT_STREQ("-11111111111111111111", Object::ToString(aBI).string());
    EXPECT_STREQ("-11111111111111111111", Object::ToString(bBI).string());

    EXPECT_TRUE(Object::Equals(a, b));
    EXPECT_TRUE(Object::Equals(b, a));

    AutoPtr<IBigDecimal> bd;
    a->Subtract(b, &bd);
    Integer sign;
    bd->Signum(&sign);
    EXPECT_EQ(0, sign);
    Integer comp;
    IComparable::Probe(a)->CompareTo(b, &comp);
    EXPECT_EQ(0, comp);
}

TEST(BigDecimalTest, TestPrecisionFromStringSimplePowersOfTen)
{
    AutoPtr<IBigInteger> V_10, V_1, V1, V10;
    BigIntegerFactory::ValueOf(-10, &V_10);
    BigIntegerFactory::ValueOf(-1, &V_1);
    BigIntegerFactory::ValueOf(10, &V10);
    BigIntegerFactory::ValueOf(1, &V1);

    AutoPtr<IBigDecimal> lBD, rBD;
    CBigDecimal::New(V_10, 1, IID_IBigDecimal, (IInterface**)&lBD);
    CBigDecimal::New(String("-1.0"), IID_IBigDecimal, (IInterface**)&rBD);
    EXPECT_TRUE(Object::Equals(lBD, rBD));

    lBD = rBD = nullptr;
    CBigDecimal::New(V_1, 1, IID_IBigDecimal, (IInterface**)&lBD);
    CBigDecimal::New(String("-0.1"), IID_IBigDecimal, (IInterface**)&rBD);
    EXPECT_TRUE(Object::Equals(lBD, rBD));

    lBD = rBD = nullptr;
    CBigDecimal::New(V_1, -1, IID_IBigDecimal, (IInterface**)&lBD);
    CBigDecimal::New(String("-1E+1"), IID_IBigDecimal, (IInterface**)&rBD);
    EXPECT_TRUE(Object::Equals(lBD, rBD));

    lBD = rBD = nullptr;
    CBigDecimal::New(V10, 1, IID_IBigDecimal, (IInterface**)&lBD);
    CBigDecimal::New(String("1.0"), IID_IBigDecimal, (IInterface**)&rBD);
    EXPECT_TRUE(Object::Equals(lBD, rBD));

    lBD = rBD = nullptr;
    CBigDecimal::New(V1, 0, IID_IBigDecimal, (IInterface**)&lBD);
    CBigDecimal::New(String("1"), IID_IBigDecimal, (IInterface**)&rBD);
    EXPECT_TRUE(Object::Equals(lBD, rBD));

    lBD = rBD = nullptr;
    CBigDecimal::New(String("1.0"), IID_IBigDecimal, (IInterface**)&lBD);
    CBigDecimal::New(String("1"), IID_IBigDecimal, (IInterface**)&rBD);
    EXPECT_FALSE(Object::Equals(lBD, rBD));
}

TEST(BigDecimalTest, Test54580)
{
    AutoPtr<IBigDecimal> a;
    CBigDecimal::New(String("1.200002"), IID_IBigDecimal, (IInterface**)&a);
    String str;
    a->ToPlainString(&str);
    EXPECT_STREQ("1.200002", str.string());

    AutoPtr<IMathContext> mc;
    CMathContext::New(3, RoundingMode::HALF_UP, IID_IMathContext, (IInterface**)&mc);
    AutoPtr<IBigDecimal> abs;
    a->Abs(mc, &abs);
    abs->ToPlainString(&str);
    EXPECT_STREQ("1.20", str.string());

    a->ToPlainString(&str);
    EXPECT_STREQ("1.200002", str.string());
}

TEST(BigDecimalTest, Test191227)
{
    AutoPtr<IBigDecimal> tempBD;
    AutoPtr<IBigDecimal> zero;
    BigDecimalFactory::GetZERO(&zero);
    zero->SetScale(2, RoundingMode::HALF_EVEN, &tempBD);
    zero = std::move(tempBD);

    AutoPtr<IBigDecimal> other;
    BigDecimalFactory::ValueOf(999999998000000001.00, &other);
    other->SetScale(2, RoundingMode::HALF_EVEN, &tempBD);
    other = std::move(tempBD);

    EXPECT_FALSE(Object::Equals(zero, other));
    EXPECT_FALSE(Object::Equals(other, zero));
}

void CheckDivide(
    /* [in] */ const String& expected,
    /* [in] */ Long n,
    /* [in] */ Long d,
    /* [in] */ Integer scale,
    /* [in] */ RoundingMode rm)
{
    AutoPtr<IBigDecimal> expBD;
    CBigDecimal::New(expected, IID_IBigDecimal, (IInterface**)&expBD);
    AutoPtr<IBigDecimal> dividend;
    CBigDecimal::New(n, IID_IBigDecimal, (IInterface**)&dividend);
    AutoPtr<IBigDecimal> divisor;
    CBigDecimal::New(d, IID_IBigDecimal, (IInterface**)&divisor);
    AutoPtr<IBigDecimal> result;
    dividend->Divide(divisor, scale, rm, &result);
    EXPECT_TRUE(Object::Equals(expBD, result));
}

TEST(BigDecimalTest, TestDivideRounding)
{
    CheckDivide(String("0"), 1, ILong::MIN_VALUE, 0, RoundingMode::DOWN);
    CheckDivide(String("-1"), 1, ILong::MIN_VALUE, 0, RoundingMode::UP);
    CheckDivide(String("-1"), 1, ILong::MIN_VALUE, 0, RoundingMode::FLOOR);
    CheckDivide(String("0"), 1, ILong::MIN_VALUE, 0, RoundingMode::CEILING);
    CheckDivide(String("0"), 1, ILong::MIN_VALUE, 0, RoundingMode::HALF_EVEN);
    CheckDivide(String("0"), 1, ILong::MIN_VALUE, 0, RoundingMode::HALF_UP);
    CheckDivide(String("0"), 1, ILong::MIN_VALUE, 0, RoundingMode::HALF_DOWN);

    CheckDivide(String("1"), ILong::MAX_VALUE, ILong::MAX_VALUE / 2 + 1, 0, RoundingMode::DOWN);
    CheckDivide(String("2"), ILong::MAX_VALUE, ILong::MAX_VALUE / 2, 0, RoundingMode::DOWN);
    CheckDivide(String("0.50"), ILong::MAX_VALUE / 2, ILong::MAX_VALUE, 2, RoundingMode::HALF_UP);
    CheckDivide(String("0.50"), ILong::MIN_VALUE / 2, ILong::MIN_VALUE, 2, RoundingMode::HALF_UP);
    CheckDivide(String("0.5000"), ILong::MIN_VALUE / 2, ILong::MIN_VALUE, 4, RoundingMode::HALF_UP);

    CheckDivide(String("0"), ILong::MIN_VALUE / 2 + 1, ILong::MIN_VALUE, 0, RoundingMode::HALF_UP);
    CheckDivide(String("1"), ILong::MIN_VALUE / 2, ILong::MIN_VALUE, 0, RoundingMode::HALF_UP);
    CheckDivide(String("0"), ILong::MIN_VALUE / 2, ILong::MIN_VALUE, 0, RoundingMode::HALF_DOWN);
    CheckDivide(String("1"), ILong::MIN_VALUE / 2 - 1, ILong::MIN_VALUE, 0, RoundingMode::HALF_DOWN);
}

TEST(BigDecimalTest, TestDivideRoundingSign)
{
    // positive dividend and divisor, even/odd values
    CheckDivide(String("0"), 49, 100, 0, RoundingMode::HALF_UP);
    CheckDivide(String("1"), 50, 100, 0, RoundingMode::HALF_UP);
    CheckDivide(String("1"), 51, 101, 0, RoundingMode::HALF_UP);
    CheckDivide(String("0"), 50, 101, 0, RoundingMode::HALF_UP);
    CheckDivide(String("0"), ILong::MAX_VALUE / 2, ILong::MAX_VALUE, 0, RoundingMode::HALF_UP);

    // Same with negative dividend and divisor
    CheckDivide(String("0"), -49, -100, 0, RoundingMode::HALF_UP);
    CheckDivide(String("1"), -50, -100, 0, RoundingMode::HALF_UP);
    CheckDivide(String("1"), -51, -101, 0, RoundingMode::HALF_UP);
    CheckDivide(String("0"), -50, -101, 0, RoundingMode::HALF_UP);
    CheckDivide(String("0"), -(ILong::MAX_VALUE / 2), -ILong::MAX_VALUE, 0, RoundingMode::HALF_UP);

    // Same with negative dividend
    CheckDivide(String("0"), -49, 100, 0, RoundingMode::HALF_UP);
    CheckDivide(String("-1"), -50, 100, 0, RoundingMode::HALF_UP);
    CheckDivide(String("-1"), -51, 101, 0, RoundingMode::HALF_UP);
    CheckDivide(String("0"), -50, 101, 0, RoundingMode::HALF_UP);
    CheckDivide(String("0"), -(ILong::MAX_VALUE / 2), ILong::MAX_VALUE, 0, RoundingMode::HALF_UP);

    // Same with negative divisor
    CheckDivide(String("0"), 49, -100, 0, RoundingMode::HALF_UP);
    CheckDivide(String("-1"), 50, -100, 0, RoundingMode::HALF_UP);
    CheckDivide(String("-1"), 51, -101, 0, RoundingMode::HALF_UP);
    CheckDivide(String("0"), 50, -101, 0, RoundingMode::HALF_UP);
    CheckDivide(String("0"), ILong::MAX_VALUE / 2, -ILong::MAX_VALUE, 0, RoundingMode::HALF_UP);
}

TEST(BigDecimalTest, TestDivideByOne)
{
    Array<Long> dividends{
        ILong::MIN_VALUE,
        ILong::MIN_VALUE + 1,
        ILong::MAX_VALUE,
        ILong::MAX_VALUE - 1,
        0,
        -1,
        1,
        10, 43, 314159265358979323LL, // arbitrary values
    };
    for (Integer i = 0; i < dividends.GetLength(); ++i) {
        Long dividend = dividends[i];
        String expected = StringUtils::ToString(dividend);
        CheckDivide(expected, dividend, 1, 0, RoundingMode::UNNECESSARY);
    }
}

void CheckNegate(
    /* [in] */ IBigDecimal* a,
    /* [in] */ IBigDecimal* b)
{
    if (!Object::ToString(a).Equals("0")) {
        EXPECT_FALSE(Object::Equals(a, b));
    }
    AutoPtr<IBigDecimal> neg;
    a->Negate(&neg);
    EXPECT_TRUE(Object::Equals(neg, b));
    neg = nullptr;
    b->Negate(&neg);
    EXPECT_TRUE(Object::Equals(a, neg));
    neg = nullptr;
    a->Negate(&neg);
    AutoPtr<IBigDecimal> neg2;
    neg->Negate(&neg2);
    EXPECT_TRUE(Object::Equals(a, neg2));
}

TEST(BigDecimalTest, TestNegate)
{
    AutoPtr<IBigDecimal> v1, v2;
    BigDecimalFactory::ValueOf(0LL, &v1);
    BigDecimalFactory::ValueOf(0LL, &v2);
    CheckNegate(v1, v2);

    v1 = v2 = nullptr;
    BigDecimalFactory::ValueOf(1LL, &v1);
    BigDecimalFactory::ValueOf(-1LL, &v2);
    CheckNegate(v1, v2);

    v1 = v2 = nullptr;
    BigDecimalFactory::ValueOf(43LL, &v1);
    BigDecimalFactory::ValueOf(-43LL, &v2);
    CheckNegate(v1, v2);

    v1 = v2 = nullptr;
    BigDecimalFactory::ValueOf(ILong::MAX_VALUE, &v1);
    BigDecimalFactory::ValueOf(-ILong::MAX_VALUE, &v2);
    CheckNegate(v1, v2);

    v1 = v2 = nullptr;
    CBigDecimal::New(String("9223372036854775808"), IID_IBigDecimal, (IInterface**)&v1);
    CBigDecimal::New(ILong::MIN_VALUE, IID_IBigDecimal, (IInterface**)&v2);
    CheckNegate(v1, v2);

    v1 = v2 = nullptr;
    CBigDecimal::New(String("342343243546465623424321423112321.43243434343412321"),
            IID_IBigDecimal, (IInterface**)&v1);
    CBigDecimal::New(String("-342343243546465623424321423112321.43243434343412321"),
            IID_IBigDecimal, (IInterface**)&v2);
    CheckNegate(v1, v2);
}

void AssertSum(
    /* [in] */ const String& expectedSumAsString,
    /* [in] */ Long a,
    /* [in] */ Long b)
{
    if (a == ILong::MIN_VALUE || b == ILong::MIN_VALUE) {
        // - (Long.MIN_VALUE) can't be represented as a long, so don't allow it here.
        printf("Long.MIN_VALUE not allowed\n");
        EXPECT_TRUE(false);
    }
    AutoPtr<IBigDecimal> bigA, bigB;
    AutoPtr<IBigDecimal> bigMinusA, bigMinusB;
    BigDecimalFactory::ValueOf(a, &bigA);
    BigDecimalFactory::ValueOf(b, &bigB);
    BigDecimalFactory::ValueOf(-a, &bigMinusA);
    BigDecimalFactory::ValueOf(-b, &bigMinusB);

    AutoPtr<IBigDecimal> result;
    bigA->Add(bigB, &result);
    EXPECT_STREQ(expectedSumAsString.string(), Object::ToString(result).string());

    result = nullptr;
    bigB->Add(bigA, &result);
    EXPECT_STREQ(expectedSumAsString.string(), Object::ToString(result).string());

    result = nullptr;
    bigA->Subtract(bigMinusB, &result);
    EXPECT_STREQ(expectedSumAsString.string(), Object::ToString(result).string());

    result = nullptr;
    bigB->Subtract(bigMinusA, &result);
    EXPECT_STREQ(expectedSumAsString.string(), Object::ToString(result).string());
}

TEST(BigDecimalTest, TestAddAndSubtractNear64BitOverflow)
{
    EXPECT_STREQ("-9223372036854775808", StringUtils::ToString(ILong::MIN_VALUE).string());
    EXPECT_STREQ("9223372036854775807", StringUtils::ToString(ILong::MAX_VALUE).string());

    // Exactly MIN_VALUE and MAX_VALUE
    AssertSum(String("-9223372036854775808"), -(1LL << 62LL), -(1LL << 62LL));
    AssertSum(String("9223372036854775807"), (1LL << 62LL) - 1LL, 1LL << 62LL);

    // One beyond MIN_VALUE and MAX_VALUE
    AssertSum(String("-9223372036854775809"), -(1LL << 62LL), -(1LL << 62LL) - 1);
    AssertSum(String("-9223372036854775809"), ILong::MIN_VALUE + 1, -2);
    AssertSum(String("9223372036854775808"), 1LL << 62LL, 1LL << 62LL);
    AssertSum(String("9223372036854775808"), ILong::MAX_VALUE, 1);
}

TEST(BigDecimalTest, TestDivideAvoids64bitOverflow)
{
    AutoPtr<IBigDecimal> minLong;
    CBigDecimal::New(String("-9223372036854775808"), IID_IBigDecimal, (IInterface**)&minLong);

    AutoPtr<IBigDecimal> lBD, rBD, dividend, divisor;
    CBigDecimal::New(String("9223372036854775808"), IID_IBigDecimal, (IInterface**)&lBD);
    CBigDecimal::New(String("-1"), IID_IBigDecimal, (IInterface**)&divisor);
    minLong->Divide(divisor, 0, RoundingMode::UNNECESSARY, &rBD);
    EXPECT_TRUE(Object::Equals(lBD, rBD));

    lBD = rBD = divisor = nullptr;
    CBigDecimal::New(String("9223372036854775808"), IID_IBigDecimal, (IInterface**)&lBD);
    CBigDecimal::New(String("-922337203685477580.8"), IID_IBigDecimal, (IInterface**)&dividend);
    CBigDecimal::New(String("-0.1"), IID_IBigDecimal, (IInterface**)&divisor);
    dividend->Divide(divisor, 0, RoundingMode::UNNECESSARY, &rBD);
    EXPECT_TRUE(Object::Equals(lBD, rBD));

    lBD = rBD = dividend = divisor = nullptr;
    CBigDecimal::New(String("9223372036854775808"), IID_IBigDecimal, (IInterface**)&lBD);
    CBigDecimal::New(String("-92233720368547758080"), IID_IBigDecimal, (IInterface**)&dividend);
    CBigDecimal::New(String("-1E+1"), IID_IBigDecimal, (IInterface**)&divisor);
    dividend->Divide(divisor, 0, RoundingMode::UNNECESSARY, &rBD);
    EXPECT_TRUE(Object::Equals(lBD, rBD));

    lBD = rBD = dividend = divisor = nullptr;
    CBigDecimal::New(String("922337203685477580.8"), IID_IBigDecimal, (IInterface**)&lBD);
    CBigDecimal::New(String("-1E+1"), IID_IBigDecimal, (IInterface**)&divisor);
    minLong->Divide(divisor, 1, RoundingMode::UNNECESSARY, &rBD);
    EXPECT_TRUE(Object::Equals(lBD, rBD));

    lBD = rBD = dividend = divisor = nullptr;
    CBigDecimal::New(String("9223372036854775808.0"), IID_IBigDecimal, (IInterface**)&lBD);
    CBigDecimal::New(String("-1"), IID_IBigDecimal, (IInterface**)&divisor);
    minLong->Divide(divisor, 1, RoundingMode::UNNECESSARY, &rBD);
    EXPECT_TRUE(Object::Equals(lBD, rBD));

    lBD = rBD = dividend = divisor = nullptr;
    CBigDecimal::New(String("9223372036854775808"), IID_IBigDecimal, (IInterface**)&lBD);
    CBigDecimal::New(String("-1.0"), IID_IBigDecimal, (IInterface**)&divisor);
    minLong->Divide(divisor, 0, RoundingMode::UNNECESSARY, &rBD);
    EXPECT_TRUE(Object::Equals(lBD, rBD));

    lBD = rBD = dividend = divisor = nullptr;
    CBigDecimal::New(String("9223372036854775808.0"), IID_IBigDecimal, (IInterface**)&lBD);
    CBigDecimal::New(String("-1.0"), IID_IBigDecimal, (IInterface**)&divisor);
    minLong->Divide(divisor, 1, RoundingMode::UNNECESSARY, &rBD);
    EXPECT_TRUE(Object::Equals(lBD, rBD));

    lBD = rBD = dividend = divisor = nullptr;
    CBigDecimal::New(String("9223372036854775808"), IID_IBigDecimal, (IInterface**)&lBD);
    CBigDecimal::New(String("-4611686018427387904"), IID_IBigDecimal, (IInterface**)&dividend);
    CBigDecimal::New(String("-5E-1"), IID_IBigDecimal, (IInterface**)&divisor);
    dividend->Divide(divisor, 0, RoundingMode::UNNECESSARY, &rBD);
    EXPECT_TRUE(Object::Equals(lBD, rBD));
}

void CheckCommonOperations(
    /* [in] */ Long value)
{
    if (value % 2 != 0) {
        printf("Expected even value, got %lld\n", value);
        EXPECT_TRUE(false);
    }
    AutoPtr<IBigDecimal> bigHalfValue, bigValue, two;
    BigDecimalFactory::ValueOf(value / 2, &bigHalfValue);
    BigDecimalFactory::ValueOf(value, &bigValue);
    BigDecimalFactory::ValueOf(2LL, &two);

    AutoPtr<IBigDecimal> result;
    bigHalfValue->Multiply(two, &result);
    EXPECT_TRUE(Object::Equals(bigValue, result));

    result = nullptr;
    bigHalfValue->Add(bigHalfValue, &result);
    EXPECT_TRUE(Object::Equals(bigValue, result));

    result = nullptr;
    bigValue->Subtract(bigHalfValue, &result);
    EXPECT_TRUE(Object::Equals(bigHalfValue, result));

    result = nullptr;
    bigValue->Divide(two, RoundingMode::UNNECESSARY, &result);
    EXPECT_TRUE(Object::Equals(bigHalfValue, result));

    if (value != 0) {
        result = nullptr;
        bigValue->Divide(bigHalfValue, RoundingMode::UNNECESSARY, &result);
        EXPECT_TRUE(Object::Equals(two, result));
    }
}

TEST(BigDecimalTest, TestCommonOperationsHalfOfEvenLongValue)
{
    CheckCommonOperations(0);
    CheckCommonOperations(2);
    CheckCommonOperations(-2);
    CheckCommonOperations(ILong::MIN_VALUE);
    CheckCommonOperations(1LL << 62LL);
    CheckCommonOperations(-(1LL << 62LL));
    CheckCommonOperations(1LL << 62LL + 1 << 30 + 1 << 10);
    CheckCommonOperations(ILong::MAX_VALUE - 1);
}

void CheckMultiplyConsistentWithLong(
    /* [in] */ Long a,
    /* [in] */ Long b)
{
    if (a != 0 && b != 0 && Math::Abs(ILong::MAX_VALUE / 2) <= Math::Abs(b)) {
        printf("Multiplication might overflow: %lld * %lld", a, b);
        EXPECT_TRUE(false);
    }
    Long expectedResult = a * b;

    AutoPtr<IBigDecimal> va, vb, result;
    BigDecimalFactory::ValueOf(a, &va);
    BigDecimalFactory::ValueOf(b, &vb);
    va->Multiply(vb, &result);
    EXPECT_STREQ(StringUtils::ToString(expectedResult).string(), Object::ToString(result).string());

    va = vb = result = nullptr;
    BigDecimalFactory::ValueOf(a, 2, &va);
    BigDecimalFactory::ValueOf(b, 3, &vb);
    va->Multiply(vb, &result);
    AutoPtr<IBigDecimal> vexp;
    BigDecimalFactory::ValueOf(expectedResult, 5, &vexp);
    EXPECT_TRUE(Object::Equals(vexp, result));
}

TEST(BigDecimalTest, TestMultiplyConsistentWithLong)
{
    CheckMultiplyConsistentWithLong(0, 0);
    CheckMultiplyConsistentWithLong(0, 1);
    CheckMultiplyConsistentWithLong(1, 1);
    CheckMultiplyConsistentWithLong(2, 3);
    CheckMultiplyConsistentWithLong(123, 456);
    CheckMultiplyConsistentWithLong(9, 9);
    CheckMultiplyConsistentWithLong(34545, 3423421);
    CheckMultiplyConsistentWithLong(5465653, 342343234568LL);
    CheckMultiplyConsistentWithLong(IInteger::MAX_VALUE, IInteger::MAX_VALUE);
    CheckMultiplyConsistentWithLong((1LL << 40) + 454LL, 34324);
}

TEST(BigDecimalTest, TestMultiplyNear64BitOverflowScaled)
{
    AutoPtr<IBigDecimal> v1, v2, result;
    // -((2^31) / 100) * (-2/10) == (2^64)/1000
    BigDecimalFactory::ValueOf(-(1LL << 62LL), 2, &v1);
    BigDecimalFactory::ValueOf(-2, 1, &v2);
    v1->Multiply(v2, &result);
    EXPECT_STREQ("9223372036854775.808", Object::ToString(result).string());

    // -((2^31) / 100) * (2/10) == -(2^64)/1000
    v1 = v2 = result = nullptr;
    BigDecimalFactory::ValueOf(-(1LL << 62LL), 2, &v1);
    BigDecimalFactory::ValueOf(2, 1, &v2);
    v1->Multiply(v2, &result);
    EXPECT_STREQ("-9223372036854775.808", Object::ToString(result).string());

    // -((2^31) * 100) * (-2/10) == (2^64) * 10
    v1 = v2 = result = nullptr;
    AutoPtr<IBigDecimal> lBD;
    CBigDecimal::New(String("9223372036854775808E1"), IID_IBigDecimal, (IInterface**)&lBD);
    BigDecimalFactory::ValueOf(-(1LL << 62LL), -2, &v1);
    BigDecimalFactory::ValueOf(-2, 1, &v2);
    v1->Multiply(v2, &result);
    EXPECT_TRUE(Object::Equals(lBD, result));
}

AutoPtr<IBigDecimal> BigMultiply(
    /* [in] */ Long a,
    /* [in] */ Long b)
{
    AutoPtr<IBigDecimal> bigA, bigB, result;
    BigDecimalFactory::ValueOf(a, &bigA);
    BigDecimalFactory::ValueOf(b, &bigB);
    bigA->Multiply(bigB, &result);
    AutoPtr<IBigDecimal> rBD;
    bigB->Multiply(bigA, &rBD);
    EXPECT_TRUE(Object::Equals(result, rBD));
    return result;
}

TEST(BigDecimalTest, TestMultiplyNear64BitOverflowPositive)
{
    // Results of exactly +2^63, which doesn't fit into a long even though -2^63 does
    EXPECT_STREQ("9223372036854775808", Object::ToString(BigMultiply(ILong::MIN_VALUE, -1)).string());
    EXPECT_STREQ("9223372036854775808", Object::ToString(BigMultiply(ILong::MIN_VALUE / 2, -2)).string());
    EXPECT_STREQ("9223372036854775808", Object::ToString(BigMultiply(-(ILong::MIN_VALUE / 2), 2)).string());
    EXPECT_STREQ("9223372036854775808", Object::ToString(BigMultiply(1LL << 31, 1LL << 32)).string());
    EXPECT_STREQ("9223372036854775808", Object::ToString(BigMultiply(-(1LL << 31), -(1LL << 32))).string());

    // Results near but not exactly +2^63
    EXPECT_STREQ("9223372036854775806", Object::ToString(BigMultiply(2147483647LL, 4294967298LL)).string());
    EXPECT_STREQ("9223372036854775807", Object::ToString(BigMultiply(ILong::MAX_VALUE, 1)).string());
    EXPECT_STREQ("9223372036854775807", Object::ToString(BigMultiply(42128471623LL, 218934409LL)).string());
    EXPECT_STREQ("9223372036854775809", Object::ToString(BigMultiply(77158673929LL, 119537721LL)).string());
    EXPECT_STREQ("9223372036854775810", Object::ToString(BigMultiply((1LL << 62LL) + 1, 2)).string());
}

TEST(BigDecimalTest, TestMultiplyNear64BitOverflowNegative)
{
    EXPECT_STREQ("-9223372036854775808", Object::ToString(BigMultiply(ILong::MIN_VALUE, 1)).string());
    EXPECT_STREQ("-9223372036854775808", Object::ToString(BigMultiply(ILong::MIN_VALUE / 2, 2)).string());
    EXPECT_STREQ("-9223372036854775808", Object::ToString(BigMultiply(-(1LL << 31), 1LL << 32)).string());
    EXPECT_STREQ("-9223372036854775807", Object::ToString(BigMultiply(-42128471623LL, 218934409LL)).string());
    EXPECT_STREQ("-9223372036854775810", Object::ToString(BigMultiply(-(ILong::MIN_VALUE / 2) + 1, -2)).string());
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
