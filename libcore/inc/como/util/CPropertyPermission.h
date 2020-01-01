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

#ifndef __COMO_UTIL_CPROPERTYPERMISSION_H__
#define __COMO_UTIL_CPROPERTYPERMISSION_H__

#include "como/security/BasicPermission.h"
#include "_como_util_CPropertyPermission.h"

using como::security::BasicPermission;

namespace como {
namespace util {

Coclass(CPropertyPermission)
    , public BasicPermission
{
public:
    COMO_OBJECT_DECL();

    ECode Constructor(
        /* [in] */ const String& name,
        /* [in] */ const String& actioins);
};

inline ECode CPropertyPermission::Constructor(
    /* [in] */ const String& name,
    /* [in] */ const String& actioins)
{
    return BasicPermission::Constructor(String(""), String(""));
}

}
}

#endif // __COMO_UTIL_CPROPERTYPERMISSION_H__
