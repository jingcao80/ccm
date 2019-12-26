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

#include "CTestObject.h"

namespace como {
namespace test {
namespace rpc {

COMO_INTERFACE_IMPL_1(CTestObject, Object, ITestInterface);
COMO_OBJECT_IMPL(CTestObject);

ECode CTestObject::TestMethod1(
    /* [in] */ Integer arg1,
    /* [out] */ Integer& result1)
{
    result1 = arg1 + 3;
    return NOERROR;
}

ECode CTestObject::GetValue(
    /* [out] */ Integer& value)
{
    value = 12;
    return NOERROR;
}

} // namespace rpc
} // namespace test
} // namespace como
