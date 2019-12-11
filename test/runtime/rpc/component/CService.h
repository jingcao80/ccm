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
        /* [in] */ Integer arg1,
        /* [out] */ Integer& result1) override;

    ECode TestMethod2(
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
        /* [out] */ Float& result16) override;
};

} // namespace rpc
} // namespace test
} // namespace como

#endif // __COMO_TEST_RPC_CSERVICE_H__
