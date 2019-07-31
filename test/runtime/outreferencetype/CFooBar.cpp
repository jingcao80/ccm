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

#include "CFooBar.h"

namespace ccm {
namespace test {

CCM_INTERFACE_IMPL_3(CFooBar, Object, IFoo, IBar, IFooBar);

CCM_OBJECT_IMPL(CFooBar);

ECode CFooBar::Constructor(
    /* [in] */ Integer initValue)
{
    return NOERROR;
}

ECode CFooBar::Foo(
    /* [in] */ Integer value,
    /* [out] */ Integer& result)
{
    return NOERROR;
}

ECode CFooBar::Bar(
    /* [in] */ const String& name,
    /* [out] */ String& id)
{
    return NOERROR;
}

ECode CFooBar::FooBar(
    /* [in] */ IFoo* foo,
    /* [out] */ AutoPtr<IBar>& bar)
{
    return NOERROR;
}

}
}
