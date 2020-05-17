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

#ifndef __COMO_SECURITY_PERMISSION_H__
#define __COMO_SECURITY_PERMISSION_H__

#include "como/core/SyncObject.h"
#include "como.io.ISerializable.h"
#include "como.security.IGuard.h"
#include "como.security.IPermission.h"

using como::core::SyncObject;
using como::io::ISerializable;

namespace como {
namespace security {

class Permission
    : public SyncObject
    , public IPermission
    , public IGuard
    , public ISerializable
{
public:
    COMO_INTERFACE_DECL();

    ECode Constructor(
        /* [in] */ const String& name);

    ECode CheckGuard(
        /* [in] */ IInterface* object) override;

    ECode GetName(
        /* [out] */ String& name) override;

    ECode NewPermissionCollection(
        /* [out] */ AutoPtr<IPermissionCollection>& permissions) override;
};

inline ECode Permission::Constructor(
    /* [in] */ const String& name)
{
    return NOERROR;
}

}
}

#endif // __COMO_SECURITY_PERMISSION_H__
