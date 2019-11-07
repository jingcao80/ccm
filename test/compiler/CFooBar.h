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

#ifndef __NAMESPACE1_NAMESPACE2_NAMESPACE3_CFOOBAR_H__
#define __NAMESPACE1_NAMESPACE2_NAMESPACE3_CFOOBAR_H__

#include <comoobj.h>
#include "_Namespace1_Namespace2_Namespace3_CFooBar.h"

namespace Namespace1 {
namespace Namespace2 {
namespace Namespace3 {

Coclass(CFooBar)
    , public Object
    , public IFoo3
    , public IFooBar
{
public:
    COMO_INTERFACE_DECL();

    COMO_OBJECT_DECL();

    ECode Constructor();

    ECode Constructor(
        /* [in] */ const String& name);

    ECode Foo(
        /* [in] */ const Array<Byte>& value) override;

    ECode Foo(
        /* [in] */ const Array<Array<Integer>*>& value) override;

    ECode Foo() override;

    ECode Bar(
        /* [in] */ Integer value) override;

    ECode FooBar(
        /* [in] */ const String& value) override;
};

} // Namespace3
} // Namespace2
} // Namespace1

#endif // __NAMESPACE1_NAMESPACE2_NAMESPACE3_CFOOBAR_H__
