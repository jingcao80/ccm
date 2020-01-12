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
#include "como.util.concurrent.CConcurrentLinkedQueue.h"
#include "como.util.concurrent.IConcurrentLinkedQueue.h"
#include <ccmautoptr.h>
#include <comotypes.h>
#include <test/gtest.h>

using namespace como;
using como::core::CoreUtils;
using como::util::concurrent::CConcurrentLinkedQueue;
using como::util::concurrent::IConcurrentLinkedQueue;
using como::util::concurrent::IID_IConcurrentLinkedQueue;

TEST(CConcurrentLinkedQueueTest, AddTest)
{
    AutoPtr<IConcurrentLinkedQueue> queue;
    CConcurrentLinkedQueue::New(IID_IConcurrentLinkedQueue, (IInterface**)&queue);
    Integer size;
    queue->GetSize(&size);
    EXPECT_EQ(0, size);
    queue->Add(CoreUtils::Box(String("Hello")));
    queue->GetSize(&size);
    EXPECT_EQ(1, size);
    queue->Add(CoreUtils::Box(String("World")));
    queue->GetSize(&size);
    EXPECT_EQ(2, size);
}

ECode Main(Array<String>& args)
{
    testing::InitGoogleTest(args);
    ECode ec = RUN_ALL_TESTS() == 0 ? NOERROR : E_FAILED_EXCEPTION;
    return ec;
}
