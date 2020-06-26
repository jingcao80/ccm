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

#ifndef __COMO_UTIL_LINKEDHASHMAP_H__
#define __COMO_UTIL_LINKEDHASHMAP_H__

#include "como/util/AbstractSet.h"
#include "como/util/HashMap.h"
#include "como.util.ILinkedHashMap.h"

namespace como {
namespace util {

class LinkedHashMap
    : public HashMap
    , public ILinkedHashMap
{
public:
    class LinkedHashMapEntry
        : public HashMap::Node
    {
    public:
        LinkedHashMapEntry(
            /* [in] */ Integer hash,
            /* [in] */ IInterface* key,
            /* [in] */ IInterface* value,
            /* [in] */ Node* next)
            : HashMap::Node(hash, key, value, next)
        {}

    public:
        LinkedHashMapEntry* mBefore = nullptr;
        LinkedHashMapEntry* mAfter = nullptr;
    };

    class LinkedKeySet
        : public AbstractSet
    {
    public:
        inline LinkedKeySet(
            /* [in] */ LinkedHashMap* owner)
            : mOwner(owner)
        {}

        ECode GetSize(
            /* [out] */ Integer& size) override;

        ECode Clear() override;

        ECode GetIterator(
            /* [out] */ AutoPtr<IIterator>& it) override;

        ECode Contains(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean& result) override;

        ECode Remove(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* contained = nullptr) override;

    private:
        LinkedHashMap* mOwner;
    };

    class LinkedValues
        : public AbstractCollection
    {
    public:
        inline LinkedValues(
            /* [in] */ LinkedHashMap* owner)
            : mOwner(owner)
        {}

        ECode GetSize(
            /* [out] */ Integer& size) override;

        ECode Clear() override;

        ECode GetIterator(
            /* [out] */ AutoPtr<IIterator>& it) override;

        ECode Contains(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean& result) override;

    private:
        LinkedHashMap* mOwner;
    };

    class LinkedEntrySet
        : public AbstractSet
    {
    public:
        inline LinkedEntrySet(
            /* [in] */ LinkedHashMap* owner)
            : mOwner(owner)
        {}

        ECode GetSize(
            /* [out] */ Integer& size) override;

        ECode Clear() override;

        ECode GetIterator(
            /* [out] */ AutoPtr<IIterator>& it) override;

        ECode Contains(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean& result) override;

        ECode Remove(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* contained = nullptr) override;

    private:
        LinkedHashMap* mOwner;
    };

    class LinkedHashIterator
        : public LightRefBase
        , public IIterator
    {
    public:
        inline LinkedHashIterator(
            /* [in] */ LinkedHashMap* owner)
            : mOwner(owner)
            , mNext(mOwner->mHead)
            , mExpectedModCount(mOwner->mModCount)
        {}

        COMO_INTERFACE_DECL();

        ECode HasNext(
            /* [out] */ Boolean* result) override;

        ECode Remove() override;

    protected:
        ECode GetNextNode(
            /* [out] */ LinkedHashMapEntry** node);

    private:
        LinkedHashMap* mOwner;
        AutoPtr<LinkedHashMapEntry> mNext;
        AutoPtr<LinkedHashMapEntry> mCurrent;
        Integer mExpectedModCount;
    };

    class LinkedKeyIterator
        : public LinkedHashIterator
    {
    public:
        inline LinkedKeyIterator(
            /* [in] */ LinkedHashMap* owner)
            : LinkedHashIterator(owner)
        {}

        ECode Next(
            /* [out] */ IInterface** object = nullptr) override;
    };

    class LinkedValueIterator
        : public LinkedHashIterator
    {
    public:
        inline LinkedValueIterator(
            /* [in] */ LinkedHashMap* owner)
            : LinkedHashIterator(owner)
        {}

        ECode Next(
            /* [out] */ IInterface** object = nullptr) override;
    };

    class LinkedEntryIterator
        : public LinkedHashIterator
    {
    public:
        inline LinkedEntryIterator(
            /* [in] */ LinkedHashMap* owner)
            : LinkedHashIterator(owner)
        {}

        ECode Next(
            /* [out] */ IInterface** object = nullptr) override;
    };

public:
    COMO_INTERFACE_DECL();

    ECode Constructor(
        /* [in] */ Integer initialCapacity,
        /* [in] */ Float loadFactor);

    ECode Constructor(
        /* [in] */ Integer initialCapacity);

    ECode Constructor();

    ECode Constructor(
        /* [in] */ IMap* m);

    ECode Constructor(
        /* [in] */ Integer initialCapacity,
        /* [in] */ Float loadFactor,
        /* [in] */ Boolean accessOrder);

    ECode ContainsValue(
        /* [in] */ IInterface* value,
        /* [out] */ Boolean& result) override;

    ECode Get(
        /* [in] */ IInterface* key,
        /* [out] */ AutoPtr<IInterface>& value) override;

    ECode Clear() override;

    ECode GetEldest(
        /* [out] */ IMapEntry** entry) override;

    ECode GetKeySet(
        /* [out] */ AutoPtr<ISet>& keys) override;

    ECode GetValues(
        /* [out] */ AutoPtr<ICollection>& values) override;

    ECode GetEntrySet(
        /* [out] */ AutoPtr<ISet>& entries) override;

protected:
    void Reinitialize() override;

    AutoPtr<Node> NewNode(
        /* [in] */ Integer hash,
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* value,
        /* [in] */ Node* next) override;

    AutoPtr<Node> ReplacementNode(
        /* [in] */ Node* p,
        /* [in] */ Node* next) override;

    AutoPtr<TreeNode> NewTreeNode(
        /* [in] */ Integer hash,
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* value,
        /* [in] */ Node* next) override;

    AutoPtr<TreeNode> ReplacementTreeNode(
        /* [in] */ Node* p,
        /* [in] */ Node* next) override;

    ECode AfterNodeRemoval(
        /* [in] */ Node* e) override;

    ECode AfterNodeInsertion(
        /* [in] */ Boolean evict) override;

    ECode AfterNodeAccess(
        /* [in] */ HashMap::Node* e) override;

    virtual Boolean RemoveEldestEntry(
        /* [in] */ IMapEntry* eldest);

    ECode CloneImpl(
        /* [in] */ ILinkedHashMap* newObj);

private:
    void LinkNodeLast(
        /* [in] */ LinkedHashMapEntry* p);

    void TransferLinks(
        /* [in] */ LinkedHashMapEntry* src,
        /* [in] */ LinkedHashMapEntry* dst);

protected:
    AutoPtr<LinkedHashMapEntry> mHead;

    AutoPtr<LinkedHashMapEntry> mTail;

    Boolean mAccessOrder;
};

}
}

#endif // __COMO_UTIL_LINKEDHASHMAP_H__
