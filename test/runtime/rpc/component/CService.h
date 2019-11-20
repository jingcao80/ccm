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

#ifndef __COMO_TEST_RPC_CSERVICE_H__
#define __COMO_TEST_RPC_CSERVICE_H__

#include <comoapi.h>
#include <comoobj.h>
#include "_como_test_rpc_CService.h"

namespace como {
namespace test {
namespace rpc {

Coclass(CService)
    , public Object
    , public IService
{
public:
    CService();

    ~CService();

    COMO_INTERFACE_DECL();

    COMO_OBJECT_DECL();

    ECode TestMethod1(
        /* [in] */ Integer arg1);

    ECode TestMethod2(
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
        /* [in] */ Double arg18);
};

} // namespace rpc
} // namespace test
} // namespace como

#endif // __COMO_TEST_RPC_CSERVICE_H__
