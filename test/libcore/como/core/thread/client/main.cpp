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

#include "como/core/Math.h"
#include "como/core/Thread.h"
#include "como.core.CSystem.h"
#include "como.core.CThread.h"
#include "como.core.ILong.h"
#include "como.core.ISystem.h"
#include "como.core.IThread.h"
#include "como.io.IInterruptible.h"
#include "como.util.concurrent.atomic.CAtomicBoolean.h"
#include "como.util.concurrent.atomic.IAtomicBoolean.h"
#include <comosp.h>
#include <gtest/gtest.h>

using como::core::CSystem;
using como::core::CThread;
using como::core::ILong;
using como::core::ISystem;
using como::core::IThread;
using como::core::IID_ISystem;
using como::core::IID_IThread;
using como::core::Math;
using como::core::Thread;
using como::util::concurrent::atomic::CAtomicBoolean;
using como::util::concurrent::atomic::IAtomicBoolean;
using como::util::concurrent::atomic::IID_IAtomicBoolean;

class WakupTestThread
    : public Thread
{
public:
    ECode Run() override
    {
        mDone = false;

        Thread::Sleep(60000);

        mDone = true;

        return NOERROR;
    }

public:
    Boolean mDone;
};

TEST(ThreadTest, TestThreadSleep)
{
    Integer millis = 1000;
    AutoPtr<ISystem> system;
    CSystem::New(IID_ISystem, (IInterface**)&system);
    Long start;
    system->GetCurrentTimeMillis(start);

    Thread::Sleep(millis);

    Long end;
    system->GetCurrentTimeMillis(end);
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

TEST(ThreadTest, TestThreadWakeup)
{
    AutoPtr<WakupTestThread> t1 = new WakupTestThread();
    AutoPtr<WakupTestThread> t2 = new WakupTestThread();
    t1->Constructor();
    t2->Constructor();

    t1->Start();
    t2->Start();
    EXPECT_TRUE(!t1->mDone && !t2->mDone);

    t1->Interrupt();
    t2->Interrupt();

    Thread::Sleep(1000);
    EXPECT_TRUE(t1->mDone && t2->mDone);
}

TEST(ThreadTest, TestParkUntilUnderflowValue)
{
    AutoPtr<IThread> current;
    Thread::GetCurrentThread(&current);

    AutoPtr<IAtomicBoolean> afterPark;
    CAtomicBoolean::New(false, IID_IAtomicBoolean, (IInterface**)&afterPark);
    AutoPtr<IAtomicBoolean> wasParkedForLongTime;
    CAtomicBoolean::New(false, IID_IAtomicBoolean, (IInterface**)&wasParkedForLongTime);

    class _Thread
        : public Thread
    {
    public:
        _Thread(
            /* [in] */ IAtomicBoolean* afterPark,
            /* [in] */ IAtomicBoolean* wasParkedForLongTime,
            /* [in] */ IThread* current)
            : mAfterPark(afterPark)
            , mWasParkedForLongTime(wasParkedForLongTime)
            , mCurrent(current)
        {}

        ECode Run() override
        {
            Sleep(5000);

            Boolean value;
            if (mAfterPark->Get(value), !value) {
                mWasParkedForLongTime->Set(true);
                mCurrent->Unpark();
            }

            return NOERROR;
        }

    private:
        IAtomicBoolean* mAfterPark;
        IAtomicBoolean* mWasParkedForLongTime;
        IThread* mCurrent;
    };
    AutoPtr<_Thread> watchdog = new _Thread(afterPark, wasParkedForLongTime, current);
    watchdog->Constructor();
    watchdog->Start();

    current->ParkUntil(ILong::MIN_VALUE);
    Boolean value;
    if (wasParkedForLongTime->Get(value), value) {
        EXPECT_TRUE(false);
    }
    afterPark->Set(true);
    watchdog->Interrupt();
    watchdog->Join();
}

TEST(ThreadTest, TestThreadRestart)
{
    AutoPtr<IThread> thread;
    CThread::New(IID_IThread, (IInterface**)&thread);
    ECode ec = thread->Start();
    EXPECT_EQ(ec, NOERROR);
    ec = thread->Join();
    EXPECT_EQ(ec, NOERROR);
    ec = thread->Start();
    EXPECT_NE(ec, NOERROR);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
