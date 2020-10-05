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

#include "como/core/CoreUtils.h"
#include "como/core/System.h"
#include "como/security/action/GetPropertyAction.h"
#include "como.core.ICharSequence.h"
#include <comosp.h>

using como::core::CoreUtils;
using como::core::ICharSequence;
using como::core::System;

namespace como {
namespace security {
namespace action {

COMO_INTERFACE_IMPL_1(GetPropertyAction, SyncObject, IPrivilegedAction);

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
    /* [out] */ AutoPtr<IInterface>& result)
{
    String value;
    FAIL_RETURN(System::GetProperty(mTheProp, value));
    result = value.IsNull() ?
            CoreUtils::Box(mDefaultVal) : CoreUtils::Box(value);
    return NOERROR;
}

}
}
}
