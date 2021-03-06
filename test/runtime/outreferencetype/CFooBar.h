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

#ifndef __CCM_TEST_CFOOBAR_H__
#define __CCM_TEST_CFOOBAR_H__

#include "ccm.test.IBar.h"
#include "ccm.test.IFoo.h"
#include "ccm.test.IFooBar.h"
#include "_ccm_test_CFooBar.h"
#include <ccmapi.h>
#include <ccmobject.h>

namespace ccm {
namespace test {

Coclass(CFooBar)
    , public Object
    , public IFoo
    , public IBar
    , public IFooBar
{
public:
    CCM_INTERFACE_DECL();

    CCM_OBJECT_DECL();

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

}
}

#endif // __CCM_TEST_CFOOBAR_H__
