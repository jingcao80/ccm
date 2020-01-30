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

#include "como.util.concurrent.atomic.CAtomicInteger.h"
#include "como.util.concurrent.atomic.IAtomicInteger.h"
#include <comosp.h>
#include <gtest/gtest.h>

using namespace como;
using como::util::concurrent::atomic::CAtomicInteger;
using como::util::concurrent::atomic::IAtomicInteger;
using como::util::concurrent::atomic::IID_IAtomicInteger;

TEST(AtomicIntegerTest, AtomicIntegerGetTest)
{
    AutoPtr<IAtomicInteger> atomic;
    CAtomicInteger::New(46, IID_IAtomicInteger, (IInterface**)&atomic);
    Integer v;
    atomic->Get(&v);
    EXPECT_EQ(46, v);
}

TEST(AtomicIntegerTest, AtomicIntegerSetTest)
{
    AutoPtr<IAtomicInteger> atomic;
    CAtomicInteger::New(9, IID_IAtomicInteger, (IInterface**)&atomic);
    Integer v;
    atomic->Get(&v);
    EXPECT_EQ(9, v);
    atomic->Set(46);
    atomic->Get(&v);
    EXPECT_EQ(46, v);
}

TEST(AtomicIntegerTest, AtomicIntegerCompareAndSetTest)
{
    AutoPtr<IAtomicInteger> atomic;
    CAtomicInteger::New(9, IID_IAtomicInteger, (IInterface**)&atomic);
    Integer v;
    atomic->Get(&v);
    EXPECT_EQ(9, v);
    Boolean succeeded;
    atomic->CompareAndSet(9, 46, &succeeded);
    atomic->Get(&v);
    EXPECT_EQ(46, v);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}