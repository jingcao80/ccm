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
    printf("==== call TestMethod1(arg:%d) ====\n", arg);
    result = arg;
    return NOERROR;
}

}
}
}
