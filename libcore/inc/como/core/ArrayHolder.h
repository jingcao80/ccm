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

#ifndef __COMO_CORE_ARRAYHOLDER_H__
#define __COMO_CORE_ARRAYHOLDER_H__

#include "como.core.IArrayHolder.h"
#include <ccmobject.h>

namespace como {
namespace core {

class ArrayHolder
    : public Object
    , public IArrayHolder
{
public:
    ~ArrayHolder();

    COMO_INTERFACE_DECL();

    ECode Constructor(
        /* [in] */ const Triple& array);

    ECode GetArray(
        /* [out] */ Triple* array) override;

    ECode GetArrayType(
        /* [out] */ Integer* type) override;

private:
    Triple mArray;
};

}
}

#endif // __COMO_CORE_ARRAYHOLDER_H__
