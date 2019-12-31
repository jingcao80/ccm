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

#ifndef __COMO_CORE_CARRAY_H__
#define __COMO_CORE_CARRAY_H__

#include "como.core.IArray.h"
#include "_como_core_CArray.h"
#include "como/core/SyncObject.h"
#include <ccmarray.h>

namespace como {
namespace core {

Coclass(CArray)
    , public SyncObject
    , public IArray
{
public:
    COMO_INTERFACE_DECL();

    COMO_OBJECT_DECL();

    ECode Constructor(
        /* [in] */ const InterfaceID& elemId,
        /* [in] */ Long size);

    ECode GetLength(
        /* [out] */ Long* size);

    ECode Get(
        /* [in] */ Long index,
        /* [out] */ IInterface** element);

    ECode Set(
        /* [in] */ Long index,
        /* [in] */ IInterface* element);

    ECode GetTypeId(
        /* [out] */ InterfaceID* id);

private:
    Array<IInterface*> mElements;
    InterfaceID mElementTypeId;
};

}
}

#endif // __COMO_CORE_CARRAY_H__
