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

#include "ccm/core/Math.h"
#include "ccm/core/Thread.h"
#include "ccm.core.CSystem.h"
#include "ccm.core.CThread.h"
#include "ccm.core.ISystem.h"
#include "ccm.core.IThread.h"
#include "ccm.io.IInterruptible.h"
#include <ccmautoptr.h>
#include <gtest/gtest.h>

using ccm::core::CSystem;
using ccm::core::CThread;
using ccm::core::ISystem;
using ccm::core::IThread;
using ccm::core::IID_ISystem;
using ccm::core::IID_IThread;
using ccm::core::Math;
using ccm::core::Thread;

TEST(ThreadTest, TestThreadSleep)
{
    Integer millis = 1000;
    AutoPtr<ISystem> system;
    CSystem::New(IID_ISystem, (IInterface**)&system);
    Long start;
    system->GetCurrentTimeMillis(&start);

    Thread::Sleep(millis);

    Long end;
    system->GetCurrentTimeMillis(&end);
    Long elapsed = end - start;
    Long offBy = Math::Abs(elapsed - millis);
    EXPECT_TRUE(offBy <= 250);
}

TEST(ThreadTest, TestThreadSleepIllegalArguments)
{
    ECode ec = Thread::Sleep(-1);
    EXPECT_NE(ec, NOERROR);

    ec = Thread::Sleep(0, -1);
    EXPECT_NE(ec, NOERROR);

    ec = Thread::Sleep(0, 1000000);
    EXPECT_NE(ec, NOERROR);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
