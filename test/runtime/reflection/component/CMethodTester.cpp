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

#include "CMethodTester.h"

#include <cstdio>

namespace como {
namespace test {
namespace reflection {

COMO_INTERFACE_IMPL_1(CMethodTester, Object, IMethodTest);
COMO_OBJECT_IMPL(CMethodTester);

CMethodTester::CMethodTester()
{
    printf("==== call CMethodTester() ====\n");
}

CMethodTester::~CMethodTester()
{
    printf("==== call ~CMethodTester() ====\n");
}

ECode CMethodTester::TestMethod1(
    /* [in] */ Integer arg,
    /* [out] */ Integer& result)
{
    printf("==== call TestMethod1(arg:%d result addr:%p) ====\n", arg, &result);
    result = arg;
    return NOERROR;
}

ECode CMethodTester::TestMethod2(
    /* [in] */ Float arg,
    /* [out] */ Float& result)
{
    printf("==== call TestMethod2(arg:%f result addr:%p) ====\n", arg, &result);
    result = arg;
    return NOERROR;
}

ECode CMethodTester::TestMethod3(
    /* [in] */ Integer arg1,
    /* [in] */ Long arg2,
    /* [in] */ Boolean arg3,
    /* [in] */ Char arg4,
    /* [in] */ Short arg5,
    /* [in] */ Integer arg6)
{
    printf("==== call TestMethod3(arg1: %d\n", arg1);
    printf("                      arg2: %lld\n", arg2);
    printf("                      arg3: %d\n", arg3);
    printf("                      arg4: %d\n", arg4);
    printf("                      arg5: %d\n", arg5);
    printf("                      arg6: %d\n", arg6);
    return NOERROR;
}

ECode CMethodTester::TestMethod4(
    /* [in] */ Integer arg1,
    /* [in] */ Long arg2,
    /* [in] */ Boolean arg3,
    /* [in] */ Char arg4,
    /* [in] */ Short arg5,
    /* [in] */ Double arg6,
    /* [in] */ Float arg7,
    /* [in] */ Integer arg8,
    /* [out] */ Double& result)
{
    printf("==== call TestMethod4(arg1: %d\n", arg1);
    printf("                      arg2: %lld\n", arg2);
    printf("                      arg3: %d\n", arg3);
    printf("                      arg4: %d\n", arg4);
    printf("                      arg5: %d\n", arg5);
    printf("                      arg6: %f\n", arg6);
    printf("                      arg7: %f\n", arg7);
    printf("                      arg8: %d\n", arg8);
    printf("                      result addr: %p\n", &result);
    result = arg2;
    return NOERROR;
}

ECode CMethodTester::TestMethod5(
    /* [in] */ Integer arg1,
    /* [in] */ Long arg2,
    /* [in] */ Boolean arg3,
    /* [in] */ Char arg4,
    /* [in] */ Short arg5,
    /* [in] */ Double arg6,
    /* [in] */ Float arg7,
    /* [in] */ Integer arg8,
    /* [in] */ Float arg9,
    /* [in] */ Double arg10,
    /* [in] */ Double arg11,
    /* [in] */ Float arg12,
    /* [in] */ Float arg13,
    /* [in] */ Double arg14,
    /* [in] */ Double arg15,
    /* [in] */ Float arg16,
    /* [out] */ Double& result)
{
    printf("==== call TestMethod5(arg1: %d\n", arg1);
    printf("                      arg2: %lld\n", arg2);
    printf("                      arg3: %d\n", arg3);
    printf("                      arg4: %d\n", arg4);
    printf("                      arg5: %d\n", arg5);
    printf("                      arg6: %f\n", arg6);
    printf("                      arg7: %f\n", arg7);
    printf("                      arg8: %d\n", arg8);
    printf("                      arg9: %f\n", arg9);
    printf("                      arg10: %f\n", arg10);
    printf("                      arg11: %f\n", arg11);
    printf("                      arg12: %f\n", arg12);
    printf("                      arg13: %f\n", arg13);
    printf("                      arg14: %f\n", arg14);
    printf("                      arg15: %f\n", arg15);
    printf("                      arg16: %f\n", arg16);
    printf("                      result addr: %p\n", &result);
    result = arg16;
    return NOERROR;
}

ECode CMethodTester::TestMethod6(
    /* [in] */ Integer arg1,
    /* [in] */ Long arg2,
    /* [in] */ Boolean arg3,
    /* [in] */ Char arg4,
    /* [in] */ Short arg5,
    /* [in] */ Double arg6,
    /* [in] */ Float arg7,
    /* [in] */ Integer arg8,
    /* [in] */ Float arg9,
    /* [in] */ Double arg10,
    /* [in] */ Double arg11,
    /* [in] */ Float arg12,
    /* [in] */ Float arg13,
    /* [in] */ Double arg14,
    /* [in] */ Double arg15,
    /* [in] */ Float arg16,
    /* [out] */ Integer& result1,
    /* [out] */ Double& result2)
{
    printf("==== call TestMethod5(arg1: %d\n", arg1);
    printf("                      arg2: %lld\n", arg2);
    printf("                      arg3: %d\n", arg3);
    printf("                      arg4: %d\n", arg4);
    printf("                      arg5: %d\n", arg5);
    printf("                      arg6: %f\n", arg6);
    printf("                      arg7: %f\n", arg7);
    printf("                      arg8: %d\n", arg8);
    printf("                      arg9: %f\n", arg9);
    printf("                      arg10: %f\n", arg10);
    printf("                      arg11: %f\n", arg11);
    printf("                      arg12: %f\n", arg12);
    printf("                      arg13: %f\n", arg13);
    printf("                      arg14: %f\n", arg14);
    printf("                      arg15: %f\n", arg15);
    printf("                      arg16: %f\n", arg16);
    printf("                      result1 addr: %p\n", &result1);
    printf("                      result2 addr: %p\n", &result2);
    result1 = arg8;
    result2 = arg16;
    return NOERROR;
}

}
}
}
