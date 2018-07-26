//=========================================================================
// Copyright (C) 2018 The C++ Component Model(CCM) Open Source Project
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

#ifndef __CCM_SECURITY_PERMISSIONS_H__
#define __CCM_SECURITY_PERMISSIONS_H__

#include "ccm/security/PermissionCollection.h"
#include "ccm.io.ISerializable.h"
#include "ccm.util.IEnumeration.h"

using ccm::io::ISerializable;
using ccm::util::IEnumeration;

namespace ccm {
namespace security {

class Permissions
    : public PermissionCollection
    , public ISerializable
{
public:
    CCM_INTERFACE_DECL();

    ECode Add(
        /* [in] */ IPermission* permission) override;

    ECode Implies(
        /* [in] */ IPermission* permission,
        /* [out] */ Boolean* result) override;

    ECode GetElements(
        /* [out] */ IEnumeration** elements) override;
};

}
}

#endif // __CCM_SECURITY_PERMISSIONS_H__