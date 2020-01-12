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

#include "como.util.CRandom.h"
#include "como.util.IRandom.h"
#include <ccmautoptr.h>
#include <gtest/gtest.h>
#include <stdio.h>

using namespace como;
using como::util::CRandom;
using como::util::IRandom;
using como::util::IID_IRandom;

TEST(RandomTest, NextIntTest)
{
    AutoPtr<IRandom> random;
    CRandom::New(IID_IRandom, (IInterface**)&random);
    Integer r1, r2;
    random->NextInteger(&r1);
    random->NextInteger(&r2);
    EXPECT_TRUE(r1 != r2);
    printf("==== r1: %d, r2: %d ====\n", r1, r2);
}

TEST(RandomTest, NextDoubleTest)
{
    AutoPtr<IRandom> random;
    CRandom::New(IID_IRandom, (IInterface**)&random);
    Double r1, r2;
    random->NextDouble(&r1);
    random->NextDouble(&r2);
    EXPECT_TRUE(r1 != r2);
    printf("==== r1: %lf, r2: %lf ====\n", r1, r2);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
