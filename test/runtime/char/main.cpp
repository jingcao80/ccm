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

#include <comosp.h>
#include <comoobj.h>
#include <gtest/gtest.h>

using namespace como;

TEST(CharTest, TestStringConstructorCharArray)
{
    Array<Char> charArray = { U'H', U'e', U'l', U'l', U'o', U' ',
            U'W', U'o', U'r', U'l', U'd', U'.' };
    String str(charArray);
    EXPECT_STREQ("Hello World.", str.string());
}

TEST(CharTest, TestStringConstructorCharArrayStartLength)
{
    Array<Char> charArray = { U'H', U'e', U'l', U'l', U'o', U' ',
            U'W', U'o', U'r', U'l', U'd', U'.' };
    String str(charArray, 2, 6);
    EXPECT_STREQ("llo Wo", str.string());
}

TEST(CharTest, TestStringGetChar)
{
    String str = "Hello World.";
    EXPECT_EQ(U'W', str.GetChar(6));
}

TEST(CharTest, TestStringValueOfChar)
{
    String str = String::ValueOf(U'W');
    EXPECT_STREQ("W", str.string());
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}