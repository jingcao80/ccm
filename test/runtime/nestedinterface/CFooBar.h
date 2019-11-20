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

#include <comoapi.h>
#include <comoobj.h>
#include "como.test.IFooBar.h"
#include "_como_test_CFooBar.h"

namespace como {
namespace test {

Coclass(CFooBar)
    , public Object
    , public IFooBar::IFoo
    , public IFooBar::IBar
{
public:
    COMO_INTERFACE_DECL();

    COMO_OBJECT_DECL();

    ECode Constructor(
        /* [in] */ IFooBar::IFoo* foo);

    ECode Foo1() override;

    ECode Foo2() override;

    ECode Bar1() override;

    ECode Bar2(
        /* [in] */ IFoo* foo);
};

} // namespace test
} // namespace como

#endif // __COMO_TEST_CFOOBAR_H__
