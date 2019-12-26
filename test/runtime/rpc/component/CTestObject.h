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

#ifndef __COMO_TEST_RPC_CTESTOBJECT_H__
#define __COMO_TEST_RPC_CTESTOBJECT_H__

#include <comoobj.h>
#include "_como_test_rpc_CTestObject.h"

namespace como {
namespace test {
namespace rpc {

Coclass(CTestObject)
    , public Object
    , public ITestInterface
{
public:
    COMO_INTERFACE_DECL();

    COMO_OBJECT_DECL();

    ECode TestMethod1(
        /* [in] */ Integer arg1,
        /* [out] */ Integer& result1) override;

    ECode GetValue(
        /* [out] */ Integer& value) override;
};

} // namespace rpc
} // namespace test
} // namespace como

#endif // __COMO_TEST_RPC_CTESTOBJECT_H__
