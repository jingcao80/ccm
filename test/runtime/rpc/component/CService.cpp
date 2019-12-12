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

#include "CService.h"
#include <cstdio>

namespace como {
namespace test {
namespace rpc {

COMO_INTERFACE_IMPL_1(CService, Object, IService);
COMO_OBJECT_IMPL(CService);

CService::CService()
{}

CService::~CService()
{}

ECode CService::TestMethod1(
    /* [in] */ Integer arg1,
    /* [out] */ Integer& result1)
{
    result1 = arg1;
    return NOERROR;
}

ECode CService::TestMethod2(
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
    /* [out] */ Long& result2,
    /* [out] */ Boolean& result3,
    /* [out] */ Char& result4,
    /* [out] */ Short& result5,
    /* [out] */ Double& result6,
    /* [out] */ Float& result7,
    /* [out] */ Integer& result8,
    /* [out] */ Float& result9,
    /* [out] */ Double& result10,
    /* [out] */ Double& result11,
    /* [out] */ Float& result12,
    /* [out] */ Float& result13,
    /* [out] */ Double& result14,
    /* [out] */ Double& result15,
    /* [out] */ Float& result16)
{
    result1 = arg1;
    result2 = arg2;
    result3 = arg3;
    result4 = arg4;
    result5 = arg5;
    result6 = arg6;
    result7 = arg7;
    result8 = arg8;
    result9 = arg9;
    result10 = arg10;
    result11 = arg11;
    result12 = arg12;
    result13 = arg13;
    result14 = arg14;
    result15 = arg15;
    result16 = arg16;
    return NOERROR;
}

ECode CService::TestMethod3(
    /* [in] */ Integer arg1,
    /* [in] */ const String& arg2,
    /* [out] */ Integer& result1,
    /* [out] */ String& result2)
{
    result1 = arg1;
    result2 = arg2;
    return NOERROR;
}

}
}
}
