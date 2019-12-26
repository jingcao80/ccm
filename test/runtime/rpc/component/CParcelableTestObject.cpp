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

#include "CParcelableTestObject.h"

namespace como {
namespace test {
namespace rpc {

COMO_INTERFACE_IMPL_2(CParcelableTestObject, Object, ITestInterface, IParcelable);
COMO_OBJECT_IMPL(CParcelableTestObject);

ECode CParcelableTestObject::TestMethod1(
    /* [in] */ Integer arg1,
    /* [out] */ Integer& result1)
{
    result1 = arg1 + 6;
    return NOERROR;
}

ECode CParcelableTestObject::GetValue(
    /* [out] */ Integer& value)
{
    value = mValue;
    return NOERROR;
}

ECode CParcelableTestObject::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInteger(mValue);
    return NOERROR;
}

ECode CParcelableTestObject::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInteger(23);
    return NOERROR;
}

} // namespace rpc
} // namespace test
} // namespace como
