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

#include "ccm.core.ICharSequence.h"
#include "ccm.util.CHashtable.h"
#include "ccm.util.IHashtable.h"
#include <core/CoreUtils.h>
#include <ccmautoptr.h>
#include <gtest/gtest.h>

using ccm::core::CoreUtils;
using ccm::core::ICharSequence;
using ccm::util::CHashtable;
using ccm::util::IHashtable;
using ccm::util::IID_IHashtable;

TEST(HashtableTest, HashtableContainsTest)
{
    AutoPtr<IHashtable> ht;
    ECode ec = CHashtable::New(IID_IHashtable, (IInterface**)&ht);
    EXPECT_EQ(0, ec);
    ec = ht->Put(CoreUtils::Box(String("IInteger::MAX_VALUE")), CoreUtils::Box(String("0x7fffffff")));
    EXPECT_EQ(0, ec);
    Boolean contains;
    ec = ht->Contains(CoreUtils::Box(String("0x7fffffff")), &contains);
    EXPECT_EQ(0, ec);
    EXPECT_TRUE(contains);
    ec = ht->Contains(CoreUtils::Box(String("0x80000000")), &contains);
    EXPECT_EQ(0, ec);
    EXPECT_FALSE(contains);
}

TEST(HashtableTest, HashtableGetPutTest)
{
    AutoPtr<IHashtable> ht;
    ECode ec = CHashtable::New(IID_IHashtable, (IInterface**)&ht);
    EXPECT_EQ(0, ec);
    ec = ht->Put(CoreUtils::Box(String("IInteger::MAX_VALUE")), CoreUtils::Box(String("0x7fffffff")));
    EXPECT_EQ(0, ec);
    AutoPtr<IInterface> value;
    ec = ht->Get(CoreUtils::Box(String("IInteger::MAX_VALUE")), &value);
    EXPECT_EQ(0, ec);
    EXPECT_STREQ("0x7fffffff", CoreUtils::Unbox(ICharSequence::Probe(value)).string());
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
