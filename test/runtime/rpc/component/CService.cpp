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

#include "CService.h"

#include <stdio.h>

namespace ccm {
namespace test {
namespace rpc {

CCM_INTERFACE_IMPL_1(CService, Object, IService);
CCM_OBJECT_IMPL(CService);

CService::CService()
{}

CService::~CService()
{}

ECode CService::TestMethod1(
    /* [in] */ Integer arg1)
{
    printf("==== [CService::TestMethod1] arg1: %d ====\n", arg1);
    return NOERROR;
}

ECode CService::TestMethod2(
    /* [in] */ Long arg1,
    /* [in] */ Long arg2,
    /* [in] */ Long arg3,
    /* [in] */ Long arg4,
    /* [in] */ Long arg5,
    /* [in] */ Long arg6,
    /* [in] */ Long arg7,
    /* [in] */ Long arg8,
    /* [in] */ Double arg9,
    /* [in] */ Double arg10,
    /* [in] */ Double arg11,
    /* [in] */ Double arg12,
    /* [in] */ Double arg13,
    /* [in] */ Double arg14,
    /* [in] */ Double arg15,
    /* [in] */ Double arg16,
    /* [in] */ Double arg17,
    /* [in] */ Double arg18)
{
    printf("==== [CService::TestMethod2] arg1: %lld, arg2: %lld, arg3: %lld, arg4: %lld, "
            "arg5: %lld, arg6: %lld, arg7: %lld, arg8: %lld, arg9: %f, arg10: %f, arg11: %f"
            "arg12: %f, arg13: %f, arg14: %f, arg15: %f, arg16: %f, arg17: %f, arg18: %f ====\n",
            arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9,
            arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, arg18);
    return NOERROR;
}

}
}
}
