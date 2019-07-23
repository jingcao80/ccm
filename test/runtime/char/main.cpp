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

#include "ccm.core.CStringBuilder.h"
#include "ccm.core.IStringBuilder.h"
#include <ccmautoptr.h>
#include <ccmobject.h>
#include <gtest/gtest.h>

using namespace ccm;
using ccm::core::CStringBuilder;
using ccm::core::IID_IStringBuilder;
using ccm::core::IStringBuilder;

TEST(CharTest, TestIStringBuilderAppend)
{
    AutoPtr<IStringBuilder> sb;
    CStringBuilder::New(IID_IStringBuilder, (IInterface**)&sb);
    sb->Append(U'H');
    EXPECT_STREQ("H", Object::ToString(sb).string());
    sb->Append(U'e');
    sb->Append(U'l');
    sb->Append(U'l');
    sb->Append(U'o');
    EXPECT_STREQ("Hello", Object::ToString(sb).string());
    sb->Append(String(" World."));
    EXPECT_STREQ("Hello World.", Object::ToString(sb).string());
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}