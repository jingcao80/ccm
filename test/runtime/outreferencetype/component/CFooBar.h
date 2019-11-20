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

#ifndef __COMO_TEST_CFOOBAR_H__
#define __COMO_TEST_CFOOBAR_H__

#include "como.test.IBar.h"
#include "como.test.IFoo.h"
#include "como.test.IFooBar.h"
#include "_como_test_CFooBar.h"
#include <comoapi.h>
#include <comoobj.h>

namespace como {
namespace test {

Coclass(CFooBar)
    , public Object
    , public IFoo
    , public IBar
    , public IFooBar
{
public:
    COMO_INTERFACE_DECL();

    COMO_OBJECT_DECL();

    ECode Constructor(
        /* [in] */ Integer initValue);

    ECode Foo(
        /* [in] */ Integer value,
        /* [out] */ Integer& result) override;

    ECode Bar(
        /* [in] */ const String& name,
        /* [out] */ String& id) override;

    ECode FooBar(
        /* [in] */ IFoo* foo,
        /* [out] */ AutoPtr<IBar>& bar) override;
};

} // namespace test
} // namespace como

#endif // __COMO_TEST_CFOOBAR_H__
