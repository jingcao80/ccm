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
#include <ccmapi.h>
#include <gtest/gtest.h>

class A
{};

class B
{};

class C : public A
{};

class D : public A, public B
{
public:
    class D1
    {};

    class D2 : public D1
    {};
};

TEST(MacroTest, TestSUPERSUBCLASS)
{
    EXPECT_TRUE(SUPERSUBCLASS(A, A));
    EXPECT_TRUE(SUPERSUBCLASS(D, D));
    EXPECT_TRUE(SUPERSUBCLASS(D::D1, D::D1));
    EXPECT_TRUE(SUPERSUBCLASS(D::D2, D::D2));

    EXPECT_FALSE(SUPERSUBCLASS(A, B));
    EXPECT_FALSE(SUPERSUBCLASS(B, A));
    EXPECT_FALSE(SUPERSUBCLASS(B, C));
    EXPECT_FALSE(SUPERSUBCLASS(C, B));

    EXPECT_TRUE(SUPERSUBCLASS(A, C));
    EXPECT_FALSE(SUPERSUBCLASS(C, A));

    EXPECT_TRUE(SUPERSUBCLASS(A, D));
    EXPECT_TRUE(SUPERSUBCLASS(B, D));
    EXPECT_FALSE(SUPERSUBCLASS(D, A));
    EXPECT_FALSE(SUPERSUBCLASS(D, B));

    EXPECT_TRUE(SUPERSUBCLASS(D::D1, D::D2));
    EXPECT_FALSE(SUPERSUBCLASS(D::D2, D::D1));
}

TEST(MacroTest, TestSUPERSUBCLASS_STRICT)
{
    EXPECT_TRUE(SUPERSUBCLASS_STRICT(A, C));
    EXPECT_TRUE(SUPERSUBCLASS_STRICT(A, D));
    EXPECT_TRUE(SUPERSUBCLASS_STRICT(B, D));
    EXPECT_TRUE(SUPERSUBCLASS_STRICT(D::D1, D::D2));
    EXPECT_FALSE(SUPERSUBCLASS_STRICT(C, A));
    EXPECT_FALSE(SUPERSUBCLASS_STRICT(D, A));
    EXPECT_FALSE(SUPERSUBCLASS_STRICT(D, B));
    EXPECT_FALSE(SUPERSUBCLASS_STRICT(D::D2, D::D1));

    EXPECT_FALSE(SUPERSUBCLASS_STRICT(A, A));
    EXPECT_FALSE(SUPERSUBCLASS_STRICT(D, D));
    EXPECT_FALSE(SUPERSUBCLASS_STRICT(D::D1, D::D1));
    EXPECT_FALSE(SUPERSUBCLASS_STRICT(D::D2, D::D2));

    EXPECT_FALSE(SUPERSUBCLASS_STRICT(A, B));
    EXPECT_FALSE(SUPERSUBCLASS_STRICT(B, C));
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
