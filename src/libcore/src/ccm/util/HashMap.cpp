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

#include "ccm/util/HashMap.h"

namespace ccm {
namespace util {

//-------------------------------------------------------------------------

CCM_INTERFACE_IMPL_1(HashMap::Node, Object, IMapEntry);

ECode HashMap::Node::GetKey(
    /* [out] */ IInterface** key)
{
    CHECK(key != nullptr);
    *key = mKey;
    REFCOUNT_ADD(*key);
    return NOERROR;
}

ECode HashMap::Node::GetValue(
    /* [out] */ IInterface** value)
{
    CHECK(value != nullptr);
    *value = mValue;
    REFCOUNT_ADD(*value);
    return NOERROR;
}

ECode HashMap::Node::ToString(
    /* [out] */ String* str)
{
    CHECK(str != nullptr);
    *str = Object::ToString(mKey) + "=" + Object::ToString(mValue);
    return NOERROR;
}

ECode HashMap::Node::GetHashCode(
    /* [out] */ Integer* hash)
{
    CHECK(hash);
    *hash = Object::GetHashCode(mKey) ^ Object::GetHashCode(mValue);
    return NOERROR;
}

ECode HashMap::Node::SetValue(
    /* [in] */ IInterface* value,
    /* [out] */ IInterface** prevValue)
{
    if (prevValue != nullptr) {
        *prevValue = mValue;
        REFCOUNT_ADD(*prevValue);
    }
    mValue = value;
    return NOERROR;
}

ECode HashMap::Node::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    CHECK(result);
    if (IInterface::Equals(obj, (IMapEntry*)this)) {
        *result = true;
        return NOERROR;
    }
    if (IMapEntry::Probe(obj) != nullptr) {
        IMapEntry* e = IMapEntry::Probe(obj);
        AutoPtr<IInterface> key, value;
        e->GetKey((IInterface**)&key);
        e->GetValue((IInterface**)&value);
        if (Object::Equals(mKey, key) &&
                Object::Equals(mValue, value)) {
            *result = true;
            return NOERROR;
        }
    }
    *result = false;
    return NOERROR;
}

}
}
