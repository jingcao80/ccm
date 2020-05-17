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

#ifndef __COMO_SECURITY_BASICPERMISSION_H__
#define __COMO_SECURITY_BASICPERMISSION_H__

#include "como/security/Permission.h"
#include "como.io.ISerializable.h"

using como::io::ISerializable;

namespace como {
namespace security {

class BasicPermission
    : public Permission
{
public:
    ECode Constructor(
        /* [in] */ const String& name);

    ECode Constructor(
        /* [in] */ const String& name,
        /* [in] */ const String& actions);

    ECode Implies(
        /* [in] */ IPermission* permission,
        /* [out] */ Boolean& result) override;

    ECode GetActions(
        /* [out] */ String& actions) override;
};

inline ECode BasicPermission::Constructor(
    /* [in] */ const String& name)
{
    return Permission::Constructor(String(""));
}

inline ECode BasicPermission::Constructor(
    /* [in] */ const String& name,
    /* [in] */ const String& actions)
{
    return Permission::Constructor(String(""));
}

}
}

#endif // __COMO_SECURITY_BASICPERMISSION_H__
