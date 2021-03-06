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

#include "ccm/core/CBoolean.h"

using ccm::io::IID_ISerializable;

namespace ccm {
namespace core {

CCM_INTERFACE_IMPL_3(CBoolean, SyncObject, IBoolean, IComparable, ISerializable);

CCM_OBJECT_IMPL(CBoolean);

ECode CBoolean::Constructor(
    /* [in] */ Boolean value)
{
    mValue = value;
    return NOERROR;
}

ECode CBoolean::GetValue(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);

    *value = mValue;
    return NOERROR;
}

ECode CBoolean::CompareTo(
    /* [in] */ IInterface* other,
    /* [out] */ Integer* result)
{
    VALIDATE_NOT_NULL(result);

    IBoolean* bo = IBoolean::Probe(other);
    if (bo == nullptr) {
        *result = -1;
        return NOERROR;
    }
    Boolean ov;
    bo->GetValue(&ov);
    *result = mValue == ov ? 0 : mValue ? 1 : -1;
    return NOERROR;
}

ECode CBoolean::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    IBoolean* bo = IBoolean::Probe(other);
    if (bo == nullptr) {
        *result = false;
        return NOERROR;
    }
    Boolean ov;
    bo->GetValue(&ov);
    *result = mValue == ov;
    return NOERROR;
}

ECode CBoolean::GetHashCode(
    /* [out] */ Integer* hash)
{
    VALIDATE_NOT_NULL(hash);

    *hash = mValue ? 1231 : 1237;
    return NOERROR;
}

ECode CBoolean::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    *str = mValue ? "true" : "false";
    return NOERROR;
}

}
}
