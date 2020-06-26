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

#include <como/core/StringUtils.h>
#include <gtest/gtest.h>

using namespace como;
using como::core::StringUtils;

Byte ParseByte(const char* s, Integer radix = 10)
{
    Byte value = 0;
    StringUtils::ParseByte(String(s), radix, value);
    return value;
}

const char* ByteToString(Byte b)
{
    return StringUtils::ToString(b).string();
}

Short ParseShort(const char* s, Integer radix = 10)
{
    Short value = 0;
    StringUtils::ParseShort(String(s), radix, value);
    return value;
}

const char* ShortToString(Short s)
{
    return StringUtils::ToString(s).string();
}

Float ParseFloat(const char* s)
{
    Float value = 0;
    StringUtils::ParseFloat(String(s), value);
    return value;
}

const char* FloatToString(Float f)
{
    return StringUtils::ToString(f).string();
}

Double ParseDouble(const char* s)
{
    Double value = 0;
    StringUtils::ParseDouble(String(s), value);
    return value;
}

const char* DoubleToString(Double d)
{
    return StringUtils::ToString(d).string();
}

TEST(StringTest, ParseByteTest)
{
    EXPECT_EQ(119, ParseByte("119"));
    EXPECT_EQ(119, ParseByte("77", 16));
}

TEST(StringTest, ByteToStringTest)
{
    EXPECT_STREQ("119", ByteToString(119));
}

TEST(StringTest, ParseShortTest)
{
    EXPECT_EQ(12345, ParseShort("12345"));
    EXPECT_EQ(12345, ParseShort("3039", 16));
}

TEST(StringTest, ShortToStringTest)
{
    EXPECT_STREQ("12345", ShortToString(12345));
}

TEST(StringTest, ParseFloatTest)
{
    EXPECT_FLOAT_EQ(0.01234, ParseFloat("0.01234"));
}

TEST(StringTest, ParseDoubleTest)
{
    EXPECT_DOUBLE_EQ(0.056789e-23, ParseDouble("0.056789e-23"));
}

TEST(StringTest, DoubleToStringTest)
{
    EXPECT_STREQ("5.6789E-25", DoubleToString(0.056789e-23));
}

TEST(StringTest, StringEndsWithTest)
{
    EXPECT_TRUE(String("HelloWorld").EndsWith("World"));
}

TEST(StringTest, ToLowerCaseTest)
{
    EXPECT_STREQ("helloworld:)", String("HellOWoRlD:)").ToLowerCase().string());
}

TEST(StringTest, ToUpperCaseTest)
{
    EXPECT_STREQ("HELLOWORLD:)", String("HellOWoRlD:)").ToUpperCase().string());
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
