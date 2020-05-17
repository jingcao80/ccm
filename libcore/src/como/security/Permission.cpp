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

#include "como/security/CPermissions.h"
#include "como/security/Permission.h"

using como::io::IID_ISerializable;

namespace como {
namespace security {

COMO_INTERFACE_IMPL_3(Permission, SyncObject, IPermission, IGuard, ISerializable);

ECode Permission::CheckGuard(
    /* [in] */ IInterface* object)
{
    return NOERROR;
}

ECode Permission::GetName(
    /* [out] */ String& name)
{
    name = nullptr;
    return NOERROR;
}

ECode Permission::NewPermissionCollection(
    /* [out] */ AutoPtr<IPermissionCollection>& permissions)
{
    return CPermissions::New(IID_IPermissionCollection, (IInterface**)&permissions);
}

}
}
