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

#include <core/StringUtils.h>
#include "ccm.core.ILong.h"
#include "ccm.math.CBigDecimal.h"
#include "ccm.math.CBigDecimalFactory.h"
#include "ccm.math.IBigDecimal.h"
#include "ccm.math.IBigDecimalFactory.h"
#include <ccmautoptr.h>
#include <gtest/gtest.h>

using ccm::core::ILong;
using ccm::core::StringUtils;
using ccm::math::CBigDecimal;
using ccm::math::CBigDecimalFactory;
using ccm::math::IBigDecimal;
using ccm::math::IBigDecimalFactory;
using ccm::math::IID_IBigDecimal;
using ccm::math::IID_IBigDecimalFactory;

void AssertPrecision(
    /* [in] */ Integer expectedPrecision,
    /* [in] */ const String& value)
{
    AutoPtr<IBigDecimal> parsed;
    CBigDecimal::New(value, IID_IBigDecimal, (IInterface**)&parsed);
    Integer precision;
    parsed->Precision(&precision);
    EXPECT_EQ(expectedPrecision, precision);

    AutoPtr<IBigDecimalFactory> factory;
    CBigDecimalFactory::New(IID_IBigDecimalFactory, (IInterface**)&factory);
    AutoPtr<IBigDecimal> ONE;
    factory->GetONE(&ONE);
    AutoPtr<IBigDecimal> computed;
    parsed->Divide(ONE, &computed);
    computed->Precision(&precision);
    EXPECT_EQ(expectedPrecision, precision);
}

TEST(BigDecimalTest, GetPrecisionTest)
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

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
