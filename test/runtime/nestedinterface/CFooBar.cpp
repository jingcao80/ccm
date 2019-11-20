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

#include "CFooBar.h"

namespace como {
namespace test {

COMO_INTERFACE_REFCOUNT(CFooBar)

COMO_INTERFACE_PROBE_BEGIN(CFooBar)
COMO_INTERFACE_PROBE_NESTEDINTERFACE(IFooBar, IFoo)
COMO_INTERFACE_PROBE_NESTEDINTERFACE(IFooBar, IBar)
COMO_INTERFACE_PROBE_END(Object)

COMO_INTERFACE_GETINTERFACEID_BEGIN(CFooBar)
COMO_INTERFACE_GETINTERFACEID_NESTEDINTERFACE(IFooBar, IFoo)
COMO_INTERFACE_GETINTERFACEID_NESTEDINTERFACE(IFooBar, IBar)
COMO_INTERFACE_GETINTERFACEID_END(Object)

COMO_OBJECT_IMPL(CFooBar);

ECode CFooBar::Constructor(
    /* [in] */ IFooBar::IFoo* foo)
{
    return NOERROR;
}

ECode CFooBar::Foo1()
{
    return NOERROR;
}

ECode CFooBar::Foo2()
{
    return NOERROR;
}

ECode CFooBar::Bar1()
{
    return NOERROR;
}

ECode CFooBar::Bar2(
    /* [in] */ IFoo* foo)
{
    return NOERROR;
}

}
}
