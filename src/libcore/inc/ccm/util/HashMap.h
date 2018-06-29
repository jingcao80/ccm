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

#ifndef __CCM_UTIL_HASHMAP_H__
#define __CCM_UTIL_HASHMAP_H__

#include "ccm/util/AbstractMap.h"
#include "ccm.core.ICloneable.h"
#include "ccm.io.ISerializable.h"
#include "ccm.util.IMapEntry.h"
#include <ccmobject.h>

using ccm::core::ICloneable;
using ccm::io::ISerializable;

namespace ccm {
namespace util {

class HashMap
    : public AbstractMap
    , public ICloneable
    , public ISerializable
{
protected:
    class Node
        : public Object
        , public IMapEntry
    {
    public:
        Node(
            /* [in] */ Integer hash,
            /* [in] */ IInterface* key,
            /* [in] */ IInterface* value,
            /* [in] */ Node* next)
            : mHash(hash)
            , mKey(key)
            , mValue(value)
            , mNext(next)
        {}

        CCM_INTERFACE_DECL();

        ECode GetKey(
            /* [out] */ IInterface** key) override;

        ECode GetValue(
            /* [out] */ IInterface** value) override;

        ECode ToString(
            /* [out] */ String* str) override;

        ECode GetHashCode(
            /* [out] */ Integer* hash) override;

        ECode SetValue(
            /* [in] */ IInterface* value,
            /* [out] */ IInterface** prevValue = nullptr) override;

        ECode Equals(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* result) override;

    public:
        Integer mHash;

        AutoPtr<IInterface> mKey;

        AutoPtr<IInterface> mValue;

        AutoPtr<Node> mNext;
    };

};

}
}

#endif // __CCM_UTIL_HASHMAP_H__
