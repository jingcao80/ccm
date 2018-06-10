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

#include "core/CChar.h"

using ccm::io::IID_ISerializable;

namespace ccm {
namespace core {

CCM_INTERFACE_IMPL_3(CChar, SyncObject, IChar, ISerializable, IComparable);

CCM_OBJECT_IMPL(CChar);

ECode CChar::constructor(
    /* [in] */ Char value)
{
    mValue = value;
    return NOERROR;
}

ECode CChar::GetValue(
    /* [out] */ Char* value)
{
    VALIDATE_NOT_NULL(value);

    *value = mValue;
    return NOERROR;
}

ECode CChar::CompareTo(
    /* [in] */ IInterface* other,
    /* [out] */ Integer* result)
{
    VALIDATE_NOT_NULL(result);

    IChar* c = IChar::Probe(other);
    if (c == nullptr) {
        *result = -1;
        return NOERROR;
    }

    Char cv;
    c->GetValue(&cv);
    *result = mValue == cv ? 0 : (mValue > cv ? 1 : -1);
    return NOERROR;
}

ECode CChar::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    IChar* c = IChar::Probe(other);
    if (c == nullptr) {
        *result = false;
        return NOERROR;
    }

    Char cv;
    c->GetValue(&cv);
    *result = mValue == cv;
    return NOERROR;
}

ECode CChar::GetHashCode(
    /* [out] */ Integer* hash)
{
    VALIDATE_NOT_NULL(hash);

    *hash = (Integer)mValue;
    return NOERROR;
}

ECode CChar::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    *str = String::ValueOfChar(mValue);
    return NOERROR;
}

}
}
