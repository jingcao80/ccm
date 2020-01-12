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

#include <como/core/CoreUtils.h>
#include "como.core.ICharSequence.h"
#include "como.util.concurrent.CConcurrentHashMap.h"
#include "como.util.concurrent.IConcurrentHashMap.h"
#include <ccmautoptr.h>
#include <comotypes.h>
#include <test/gtest.h>

using namespace como;
using como::core::CoreUtils;
using como::core::ICharSequence;
using como::util::concurrent::CConcurrentHashMap;
using como::util::concurrent::IConcurrentHashMap;
using como::util::concurrent::IID_IConcurrentHashMap;

TEST(CConcurrentHashMapTest, PutTest)
{
    AutoPtr<IConcurrentHashMap> map;
    ECode ec = CConcurrentHashMap::New(IID_IConcurrentHashMap, (IInterface**)&map);
    EXPECT_EQ(ec, NOERROR);
    AutoPtr<IInterface> ov;
    map->Put(CoreUtils::Box(String("Hello")), CoreUtils::Box(String("World")), &ov);
    EXPECT_TRUE(ov == nullptr);
    Integer size;
    map->GetSize(&size);
    EXPECT_EQ(size, 1);
    AutoPtr<IInterface> v;
    map->Get(CoreUtils::Box(String("Hello")), &v);
    EXPECT_STREQ(CoreUtils::Unbox(ICharSequence::Probe(v)).string(), "World");
}

ECode Main(Array<String>& args)
{
    testing::InitGoogleTest(args);
    ECode ec = RUN_ALL_TESTS() == 0 ? NOERROR : E_FAILED_EXCEPTION;
    return ec;
}
