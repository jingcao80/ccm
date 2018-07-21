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

#include "ccm/core/CoreUtils.h"
#include "ccm/core/System.h"
#include "ccm/security/action/GetPropertyAction.h"

using ccm::core::CoreUtils;
using ccm::core::System;

namespace ccm {
namespace security {
namespace action {

CCM_INTERFACE_IMPL_1(GetPropertyAction, SyncObject, IPrivilegedAction);

ECode GetPropertyAction::Constructor(
    /* [in] */ const String& theProp)
{
    mTheProp = theProp;
    return NOERROR;
}

ECode GetPropertyAction::Constructor(
    /* [in] */ const String& theProp,
    /* [in] */ const String& defaultVal)
{
    mTheProp = theProp;
    mDefaultVal = defaultVal;
    return NOERROR;
}

ECode GetPropertyAction::Run(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);

    String value;
    FAIL_RETURN(System::GetProperty(mTheProp, &value));
    *result = value.IsNull() ? CoreUtils::Box(mDefaultVal)
                             : CoreUtils::Box(value);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

}
}
}
