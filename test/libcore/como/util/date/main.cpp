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

#include "como.util.CDate.h"
#include "como.util.IDate.h"
#include <comosp.h>
#include <gtest/gtest.h>

using namespace como;
using como::util::CDate;
using como::util::IDate;
using como::util::IID_IDate;

TEST(DateTest, ToGMTStringTest)
{
    AutoPtr<IDate> date;
    CDate::New(IID_IDate, (IInterface**)&date);
    String dateStr;
    ECode ec = date->ToGMTString(&dateStr);
    EXPECT_TRUE(ec == NOERROR && !dateStr.IsEmpty());
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
