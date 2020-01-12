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

#include "como/core/CoreUtils.h"
#include "como.util.CArrayList.h"
#include "como.util.IArrayList.h"
#include <ccmautoptr.h>
#include <gtest/gtest.h>

using namespace como;
using como::core::CoreUtils;
using como::util::CArrayList;
using como::util::IArrayList;
using como::util::IID_IArrayList;

TEST(ArrayListTest, ArrayListContainsTest)
{
    AutoPtr<IArrayList> list;
    CArrayList::New(IID_IArrayList, (IInterface**)&list);
    Boolean contained;
    list->Contains(CoreUtils::Box(String("hello")), &contained);
    EXPECT_FALSE(contained);
    list->Add(CoreUtils::Box(String("hello")));
    list->Add(CoreUtils::Box(String("world")));
    list->Contains(CoreUtils::Box(String("world")), &contained);
    EXPECT_TRUE(contained);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
