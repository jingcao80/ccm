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

#include "RPCTestUnit.h"
#include <comoapi.h>
#include <comosp.h>
#include <ServiceManager.h>
#include <cstdio>
#include <gtest/gtest.h>

using como::test::rpc::CID_CService;
using como::test::rpc::IService;
using jing::ServiceManager;

static AutoPtr<IService> SERVICE;

TEST(RPCTest, TestGetRPCService)
{
    AutoPtr<IInterface> obj;
    ServiceManager::GetInstance()->GetService("rpcservice", obj);
    SERVICE = IService::Probe(obj);
    EXPECT_TRUE(SERVICE != nullptr);
}

TEST(RPCTest, TestCallTestMethod1)
{
    EXPECT_TRUE(SERVICE != nullptr);
    ECode ec = E_REMOTE_EXCEPTION;
    Integer result;
    ec = SERVICE->TestMethod1(9, result);
    EXPECT_EQ(9, result);
    EXPECT_EQ(ec, NOERROR);
}

TEST(RPCTest, TestCallTestMethod2)
{
    EXPECT_TRUE(SERVICE != nullptr);
    Integer arg1 = 9, result1;
    Long arg2 = 99, result2;
    Boolean arg3 = true, result3;
    Char arg4 = U'C', result4;
    Short arg5 = 999, result5;
    Double arg6 = 9.9, result6;
    Float arg7 = 9.99, result7;
    Integer arg8 = 999, result8;
    Float arg9 = 99.9, result9;
    Double arg10 = 9.009, result10;
    Double arg11 = 0.009, result11;
    Float arg12 = 9.09, result12;
    Float arg13 = 0.09, result13;
    Double arg14 = 99.009, result14;
    Double arg15 = -999.009, result15;
    Float arg16 = -0.09, result16;
    ECode ec = E_REMOTE_EXCEPTION;
    ec = SERVICE->TestMethod2(
            arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8,
            arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16,
            result1, result2, result3, result4, result5, result6, result7, result8,
            result9, result10, result11, result12, result13, result14, result15, result16);
    EXPECT_EQ(ec, NOERROR);
    EXPECT_EQ(arg1, result1);
    EXPECT_EQ(arg2, result2);
    EXPECT_EQ(arg3, result3);
    EXPECT_EQ(arg4, result4);
    EXPECT_EQ(arg5, result5);
    EXPECT_DOUBLE_EQ(arg6, result6);
    EXPECT_FLOAT_EQ(arg7, result7);
    EXPECT_EQ(arg8, result8);
    EXPECT_FLOAT_EQ(arg9, result9);
    EXPECT_DOUBLE_EQ(arg10, result10);
    EXPECT_DOUBLE_EQ(arg11, result11);
    EXPECT_FLOAT_EQ(arg12, result12);
    EXPECT_FLOAT_EQ(arg13, result13);
    EXPECT_DOUBLE_EQ(arg14, result14);
    EXPECT_DOUBLE_EQ(arg15, result15);
    EXPECT_FLOAT_EQ(arg16, result16);
}

TEST(RPCTest, TestCallTestMethod3)
{
    EXPECT_TRUE(SERVICE != nullptr);
    ECode ec = E_REMOTE_EXCEPTION;
    Integer arg1 = 9;
    Integer result1;
    String arg2 = "Hello World!";
    String result2;
    ec = SERVICE->TestMethod3(arg1, arg2, result1, result2);
    EXPECT_EQ(arg1, result1);
    EXPECT_STREQ(arg2.string(), result2.string());
    EXPECT_EQ(ec, NOERROR);
}

TEST(RPCTest, TestIsStubAlive)
{
    EXPECT_TRUE(SERVICE != nullptr);
    IProxy* proxy = IProxy::Probe(SERVICE);
    EXPECT_TRUE(proxy != nullptr);
    Boolean alive;
    proxy->IsStubAlive(alive);
    EXPECT_TRUE(alive);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
