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

    INTERFACE_ID(2b2b51d9-c2c4-4312-8a06-67de1c1f754b)
    interface ITreeNode : public IInterface
    {
        inline static ITreeNode* Probe(
            /* [in] */ IInterface* object)
        {
            static const InterfaceID IID_ITreeNode =
                    {{0x2b2b51d9,0xc2c4,0x4312,0x8a06,
                    {0x6,0x7,0xd,0xe,0x1,0xc,0x1,0xf,0x7,0x5,0x4,0xb}},
                    &CID_libcore};
            if (object == nullptr) return nullptr;
            return (ITreeNode*)object->Probe(IID_ITreeNode);
        }
    };

    class TreeNode
        : public ITreeNode
    {

    };

public:
    ECode Constructor(
        /* [in] */ Integer initialCapacity,
        /* [in] */ Float loadFactor);

    ECode Constructor(
        /* [in] */ Integer initialCapacity);

    ECode Constructor();

    ECode Constructor(
        /* [in] */ IMap* m);

    ECode GetSize(
        /* [out] */ Integer* size);

    ECode IsEmpty(
        /* [out] */ Boolean* result);

    ECode Get(
        /* [in] */ IInterface* key,
        /* [out] */ IInterface** value);

protected:
    static Integer Hash(
        /* [in] */ IInterface* key);

    /**
     * Returns a power of two size for the given target capacity.
     */
    static Integer TableSizeFor(
        /* [in] */ Integer cap);

    void PutMapEntries(
        /* [in] */ IMap* m,
        /* [in] */ Boolean evict);

    AutoPtr<Node> GetNode(
        /* [in] */ Integer hash,
        /* [in] */ IInterface* key);

    AutoPtr<IInterface> PutVal(
        /* [in] */ Integer hash,
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* value,
        /* [in] */ Boolean onlyIfAbsent,
        /* [in] */ Boolean evict);

    Array<Node*> Resize();

protected:
    /**
     * The maximum capacity, used if a higher value is implicitly specified
     * by either of the constructors with arguments.
     * MUST be a power of two <= 1<<30.
     */
    static constexpr Integer MAXIMUM_CAPACITY = 1 << 30;

    /**
     * The load factor used when none specified in constructor.
     */
    static constexpr Float DEFAULT_LOAD_FACTOR = 0.75f;

    /**
     * The table, initialized on first use, and resized as
     * necessary. When allocated, length is always a power of two.
     * (We also tolerate length zero in some operations to allow
     * bootstrapping mechanics that are currently not needed.)
     */
    Array<Node*> mTable;

    /**
     * The number of key-value mappings contained in this map.
     */
    Integer mSize = 0;

    /**
     * The next size value at which to resize (capacity * load factor).
     */
    Integer mThreshold = 0;

    /**
     * The load factor for the hash table.
     */
    Float mLoadFactor = 0;
};

}
}

#endif // __CCM_UTIL_HASHMAP_H__
