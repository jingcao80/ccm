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

#ifndef __COMO_UTIL_CLINKEDHASHMAP_H__
#define __COMO_UTIL_CLINKEDHASHMAP_H__

#include "como/util/LinkedHashMap.h"
#include "_como_util_CLinkedHashMap.h"

namespace como {
namespace util {

Coclass(CLinkedHashMap)
    , public LinkedHashMap
{
public:
    COMO_OBJECT_DECL();

    ECode Clone(
        /* [in] */ const InterfaceID& iid,
        /* [out] */ IInterface** obj) override;
};

}
}

#endif // __COMO_UTIL_CLINKEDHASHMAP_H__
