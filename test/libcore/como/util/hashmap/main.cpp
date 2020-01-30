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
#include "como.core.ICharSequence.h"
#include "como.util.CHashMap.h"
#include "como.util.IHashMap.h"
#include <como/core/CoreUtils.h>
#include <comosp.h>
#include <gtest/gtest.h>

using como::core::CoreUtils;
using como::core::ICharSequence;
using como::util::CHashMap;
using como::util::IHashMap;
using como::util::IID_IHashMap;

TEST(HashMapTest, HashMapGetPutTest)
{
    AutoPtr<IHashMap> map;
    ECode ec = CHashMap::New(IID_IHashMap, (IInterface**)&map);
    EXPECT_EQ(0, ec);
    ec = map->Put(CoreUtils::Box(String("IInteger::MAX_VALUE")), CoreUtils::Box(String("0x7fffffff")));
    EXPECT_EQ(0, ec);
    Boolean contains;
    ec = map->ContainsKey(CoreUtils::Box(String("IInteger::MAX_VALUE")), &contains);
    EXPECT_EQ(0, ec);
    EXPECT_TRUE(contains);
    AutoPtr<IInterface> value;
    ec = map->Get(CoreUtils::Box(String("IInteger::MAX_VALUE")), &value);
    EXPECT_EQ(0, ec);
    EXPECT_NE(nullptr, value.Get());
    EXPECT_STREQ("0x7fffffff", CoreUtils::Unbox(ICharSequence::Probe(value)).string());
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
