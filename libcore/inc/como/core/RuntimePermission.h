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

#ifndef __COMO_CORE_RUNTIMEPERMISSION_H__
#define __COMO_CORE_RUNTIMEPERMISSION_H__

#include "como/security/BasicPermission.h"

using como::security::BasicPermission;

namespace como {
namespace core {

class RuntimePermission
    : public BasicPermission
{
public:
    ECode Constructor(
        /* [in] */ const String& name);

    ECode Constructor(
        /* [in] */ const String& name,
        /* [in] */ const String& actions);
};

inline ECode RuntimePermission::Constructor(
    /* [in] */ const String& name)
{
    return BasicPermission::Constructor(name);
}

inline ECode RuntimePermission::Constructor(
    /* [in] */ const String& name,
    /* [in] */ const String& actions)
{
    return BasicPermission::Constructor(name, actions);
}

}
}

#endif // __COMO_CORE_RUNTIMEPERMISSION_H__
