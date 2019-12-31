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

#ifndef __CCM_UTIL_HASHMAP_H__
#define __CCM_UTIL_HASHMAP_H__

#include "ccm/util/AbstractCollection.h"
#include "ccm/util/AbstractMap.h"
#include "ccm/util/AbstractSet.h"
#include "ccm.core.ICloneable.h"
#include "ccm.io.ISerializable.h"
#include "ccm.util.IHashMap.h"
#include "ccm.util.IIterator.h"
#include "ccm.util.IMapEntry.h"
#include <ccmobject.h>

using ccm::core::ICloneable;
using ccm::io::ISerializable;

namespace ccm {
namespace util {

class TreeNode;

class HashMap
    : public AbstractMap
    , public IHashMap
    , public ICloneable
    , public ISerializable
{
public:
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

    class KeySet
        : public AbstractSet
    {
    public:
        KeySet(
            /* [in] */ HashMap* owner)
            : mOwner(owner)
        {}

        ECode GetSize(
            /* [out] */ Integer* size) override;

        ECode Clear() override;

        ECode GetIterator(
            /* [out] */ IIterator** it) override;

        ECode Contains(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* result) override;

        ECode Remove(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* contained = nullptr) override;

    private:
        HashMap* mOwner;
    };

    class Values
        : public AbstractCollection
    {
    public:
        Values(
            /* [in] */ HashMap* owner)
            : mOwner(owner)
        {}

        ECode GetSize(
            /* [out] */ Integer* size) override;

        ECode Clear() override;

        ECode GetIterator(
            /* [out] */ IIterator** it) override;

        ECode Contains(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* result) override;

    private:
        HashMap* mOwner;
    };

    class EntrySet
        : public AbstractSet
    {
    public:
        EntrySet(
            /* [in] */ HashMap* owner)
            : mOwner(owner)
        {}

        ECode GetSize(
            /* [out] */ Integer* size) override;

        ECode Clear() override;

        ECode GetIterator(
            /* [out] */ IIterator** it) override;

        ECode Contains(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* result) override;

        ECode Remove(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* contained = nullptr) override;

    private:
        HashMap* mOwner;
    };

    class HashIterator
        : public LightRefBase
        , public IIterator
    {
    public:
        HashIterator(
            /* [in] */ HashMap* owner);

        CCM_INTERFACE_DECL();

        ECode HasNext(
            /* [out] */ Boolean* result) override;

        ECode GetNextNode(
            /* [out] */ Node** node);

        ECode Remove() override;

    private:
        HashMap* mOwner;
        AutoPtr<Node> mNext;
        AutoPtr<Node> mCurrent;
        Integer mExpectedModCount;
        Integer mIndex;
    };

    class KeyIterator
        : public HashIterator
    {
    public:
        KeyIterator(
            /* [in] */ HashMap* owner);

        ECode Next(
            /* [out] */ IInterface** object = nullptr) override;
    };

    class ValueIterator
        : public HashIterator
    {
    public:
        ValueIterator(
            /* [in] */ HashMap* owner);

        ECode Next(
            /* [out] */ IInterface** object = nullptr) override;
    };

    class EntryIterator
        : public HashIterator
    {
    public:
        EntryIterator(
            /* [in] */ HashMap* owner);

        ECode Next(
            /* [out] */ IInterface** object = nullptr) override;
    };

public:
    CCM_INTERFACE_DECL();

    ECode Constructor(
        /* [in] */ Integer initialCapacity,
        /* [in] */ Float loadFactor);

    ECode Constructor(
        /* [in] */ Integer initialCapacity);

    ECode Constructor();

    ECode Constructor(
        /* [in] */ IMap* m);

    ECode GetSize(
        /* [out] */ Integer* size) override;

    ECode IsEmpty(
        /* [out] */ Boolean* result) override;

    ECode Get(
        /* [in] */ IInterface* key,
        /* [out] */ IInterface** value) override;

    ECode ContainsKey(
        /* [in] */ IInterface* key,
        /* [out] */ Boolean* result) override;

    ECode Put(
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* value,
        /* [out] */ IInterface** prevValue = nullptr) override;

    ECode PutAll(
        /* [in] */ IMap* m) override;

    ECode Remove(
        /* [in] */ IInterface* key,
        /* [out] */ IInterface** prevValue = nullptr) override;

    ECode Clear() override;

    ECode ContainsValue(
        /* [in] */ IInterface* value,
        /* [out] */ Boolean* result) override;

    ECode GetKeySet(
        /* [out] */ ISet** keys) override;

    ECode GetValues(
        /* [out] */ ICollection** values) override;

    ECode GetEntrySet(
        /* [out] */ ISet** entries) override;

    ECode PutIfAbsent(
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* value,
        /* [out] */ IInterface** prevValue = nullptr) override;

    ECode Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result) override;

    ECode GetHashCode(
        /* [out] */ Integer* hash) override;

protected:
    static Integer Hash(
        /* [in] */ IInterface* key);

    static Integer CompareComparables(
        /* [in] */ IInterface* k,
        /* [in] */ IInterface* x);

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

    void TreeifyBin(
        /* [in] */ Array<Node*>& tab,
        /* [in] */ Integer hash);

    AutoPtr<Node> RemoveNode(
        /* [in] */ Integer hash,
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* value,
        /* [in] */ Boolean matchValue,
        /* [in] */ Boolean movable);

    ECode CloneImpl(
        /* [in] */ IHashMap* newObj);

    Float GetLoadFactor();

    Integer GetCapacity();

    virtual AutoPtr<Node> NewNode(
        /* [in] */ Integer hash,
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* value,
        /* [in] */ Node* next);

    virtual AutoPtr<Node> ReplacementNode(
        /* [in] */ Node* p,
        /* [in] */ Node* next);

    virtual AutoPtr<TreeNode> NewTreeNode(
        /* [in] */ Integer hash,
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* value,
        /* [in] */ Node* next);

    virtual AutoPtr<TreeNode> ReplacementTreeNode(
        /* [in] */ Node* p,
        /* [in] */ Node* next);

    virtual void Reinitialize();

    virtual ECode AfterNodeAccess(
        /* [in] */ Node* p);

    virtual ECode AfterNodeInsertion(
        /* [in] */ Boolean evict);

    virtual ECode AfterNodeRemoval(
        /* [in] */ Node* p);

protected:
    /**
     * The default initial capacity - MUST be a power of two.
     */
    static constexpr Integer DEFAULT_INITIAL_CAPACITY = 1 << 4; // aka 16

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
     * The bin count threshold for using a tree rather than list for a
     * bin.  Bins are converted to trees when adding an element to a
     * bin with at least this many nodes. The value must be greater
     * than 2 and should be at least 8 to mesh with assumptions in
     * tree removal about conversion back to plain bins upon
     * shrinkage.
     */
    static constexpr Integer TREEIFY_THRESHOLD = 8;

    /**
     * The bin count threshold for untreeifying a (split) bin during a
     * resize operation. Should be less than TREEIFY_THRESHOLD, and at
     * most 6 to mesh with shrinkage detection under removal.
     */
    static constexpr Integer UNTREEIFY_THRESHOLD = 6;

    /**
     * The smallest table capacity for which bins may be treeified.
     * (Otherwise the table is resized if too many nodes in a bin.)
     * Should be at least 4 * TREEIFY_THRESHOLD to avoid conflicts
     * between resizing and treeification thresholds.
     */
    static constexpr Integer MIN_TREEIFY_CAPACITY = 64;

    /**
     * The table, initialized on first use, and resized as
     * necessary. When allocated, length is always a power of two.
     * (We also tolerate length zero in some operations to allow
     * bootstrapping mechanics that are currently not needed.)
     */
    Array<Node*> mTable;

    AutoPtr<ISet> mEntrySet;

    /**
     * The number of key-value mappings contained in this map.
     */
    Integer mSize = 0;

    /**
     * The number of times this HashMap has been structurally modified
     * Structural modifications are those that change the number of mappings in
     * the HashMap or otherwise modify its internal structure (e.g.,
     * rehash).  This field is used to make iterators on Collection-views of
     * the HashMap fail-fast.  (See ConcurrentModificationException).
     */
    Integer mModCount = 0;

    /**
     * The next size value at which to resize (capacity * load factor).
     */
    Integer mThreshold = 0;

    /**
     * The load factor for the hash table.
     */
    Float mLoadFactor = 0;

    friend class TreeNode;
};

}
}

#endif // __CCM_UTIL_HASHMAP_H__
