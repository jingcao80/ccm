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

#ifndef __COMO_UTIL_CONCURRENT_CONCURRENTHASHMAP_H__
#define __COMO_UTIL_CONCURRENT_CONCURRENTHASHMAP_H__

#include "innerdef.h"
#include "como/core/volatile.h"
#include "como/core/SyncObject.h"
#include "como.core.IInteger.h"
#include "como.core.IThread.h"
#include "como.io.ISerializable.h"
#include "como.util.ICollection.h"
#include "como.util.IEnumeration.h"
#include "como.util.IIterator.h"
#include "como.util.IMap.h"
#include "como.util.IMapEntry.h"
#include "como.util.ISet.h"
#include "como.util.concurrent.IConcurrentMap.h"
#include "como.util.concurrent.IConcurrentHashMap.h"
#include <comoref.h>

using como::core::IInteger;
using como::core::IThread;
using como::core::SyncObject;
using como::io::ISerializable;

namespace como {
namespace util {
namespace concurrent {

class ConcurrentHashMap
    : public SyncObject
    , public IConcurrentHashMap
    , public IConcurrentMap
    , public IMap
    , public ISerializable
{
public:
    class Node
        : public SyncObject
        , public IMapEntry
    {
    public:
        inline Node(
            /* [in] */ Integer hash,
            /* [in] */ IInterface* key,
            /* [in] */ IInterface* val,
            /* [in] */ Node* next)
            : mHash(hash)
            , mKey(key)
        {
            VOLATILE_SET(mVal, val);
            VOLATILE_SET(mNext, next);
        }

        COMO_INTERFACE_DECL();

        ECode GetKey(
            /* [out] */ AutoPtr<IInterface>& key) override;

        ECode GetValue(
            /* [out] */ AutoPtr<IInterface>& value) override;

        ECode GetHashCode(
            /* [out] */ Integer& hash) override;

        ECode ToString(
            /* [out] */ String& desc) override;

        ECode SetValue(
            /* [in] */ IInterface* value,
            /* [out] */ IInterface** prevValue = nullptr) override;

        ECode Equals(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean& result) override;

        virtual AutoPtr<Node> Find(
            /* [in] */ Integer h,
            /* [in] */ IInterface* k);

    public:
        Integer mHash;
        AutoPtr<IInterface> mKey;
        VOLATILE AutoPtr<IInterface> mVal;
        VOLATILE AutoPtr<Node> mNext;
    };

    /**
     * A node inserted at head of bins during transfer operations.
     */
    class ForwardingNode
        : public Node
    {
    public:
        inline ForwardingNode(
            /* [in] */ Array<Node*>& tab)
            : Node(MOVED, nullptr, nullptr, nullptr)
            , mNextTable(tab)
        {}

        IInterface* Probe(
            /* [in] */ const InterfaceID& iid) override;

        AutoPtr<Node> Find(
            /* [in] */ Integer h,
            /* [in] */ IInterface* k) override;

    public:
        Array<Node*> mNextTable;
    };

    class ReservationNode
        : public Node
    {
    public:
        inline ReservationNode()
            : Node(RESERVED, nullptr, nullptr, nullptr)
        {}

        IInterface* Probe(
            /* [in] */ const InterfaceID& iid) override;

        AutoPtr<Node> Find(
            /* [in] */ Integer h,
            /* [in] */ IInterface* k) override;
    };

    class CounterCell
        : public LightRefBase
    {
    public:
        inline CounterCell(
            /* [in] */ Long x)
        {
            VOLATILE_SET(mValue, x);
        }

    public:
        VOLATILE Long mValue;
    };

    class TreeNode
        : public Node
    {
    public:
        inline TreeNode(
            /* [in] */ Integer hash,
            /* [in] */ IInterface* key,
            /* [in] */ IInterface* val,
            /* [in] */ Node* next,
            /* [in] */ TreeNode* parent)
            : Node(hash, key, val, next)
            , mParent(parent)
        {}

        AutoPtr<Node> Find(
            /* [in] */ Integer h,
            /* [in] */ IInterface* k) override;

        AutoPtr<TreeNode> FindTreeNode(
            /* [in] */ Integer h,
            /* [in] */ IInterface* k,
            /* [in] */ Boolean compare);

    public:
        TreeNode* mParent = nullptr;
        TreeNode* mLeft = nullptr;
        TreeNode* mRight = nullptr;
        TreeNode* mPrev = nullptr;
        Boolean mRed;
    };

    /**
     * TreeNodes used at the heads of bins. TreeBins do not hold user
     * keys or values, but instead point to list of TreeNodes and
     * their root. They also maintain a parasitic read-write lock
     * forcing writers (who hold bin lock) to wait for readers (who do
     * not) to complete before tree restructuring operations.
     */
    class TreeBin
        : public Node
    {
    public:
        TreeBin(
            /* [in] */ TreeNode* b);

        IInterface* Probe(
            /* [in] */ const InterfaceID& iid) override;

        static Integer TieBreakOrder(
            /* [in] */ IInterface* a,
            /* [in] */ IInterface* b);

        AutoPtr<Node> Find(
            /* [in] */ Integer h,
            /* [in] */ IInterface* k) override;

        AutoPtr<TreeNode> PutTreeVal(
            /* [in] */ Integer h,
            /* [in] */ IInterface* k,
            /* [in] */ IInterface* v);

        Boolean RemoveTreeNode(
            /* [in] */ TreeNode* p);

        /* ------------------------------------------------------------ */
        // Red-black tree methods, all adapted from CLR

        static AutoPtr<TreeNode> RotateLeft(
            /* [in] */ TreeNode* root,
            /* [in] */ TreeNode* p);

        static AutoPtr<TreeNode> RotateRight(
            /* [in] */ TreeNode* root,
            /* [in] */ TreeNode* p);

        static AutoPtr<TreeNode> BalanceInsertion(
            /* [in] */ TreeNode* root,
            /* [in] */ TreeNode* x);

        static AutoPtr<TreeNode> BalanceDeletion(
            /* [in] */ TreeNode* root,
            /* [in] */ TreeNode* x);

        static Boolean CheckInvariants(
            /* [in] */ TreeNode* t);

        inline static TreeBin* From(
            /* [in] */ Node* node)
        {
            return (TreeBin*)node;
        }

    private:
        /**
         * Acquires write lock for tree restructuring.
         */
        void LockRoot();

        /**
         * Releases write lock for tree restructuring.
         */
        void UnlockRoot();

        /**
         * Possibly blocks awaiting root lock.
         */
        void ContendedLock();

    public:
        AutoPtr<TreeNode> mRoot;
        VOLATILE AutoPtr<TreeNode> mFirst;
        VOLATILE AutoPtr<IThread> mWaiter;
        VOLATILE Integer mLockState;
        // values for lockState
        static constexpr Integer WRITER = 1; // set while holding write lock
        static constexpr Integer WAITER = 2; // set when waiting for write lock
        static constexpr Integer READER = 4; // increment value for setting read lock
    };

    /**
     * Records the table, its length, and current traversal index for a
     * traverser that must process a region of a forwarded table before
     * proceeding with current table.
     */
    class TableStack
        : public LightRefBase
    {
    public:
        Integer mLength;
        Integer mIndex;
        Array<Node*> mTab;
        AutoPtr<TableStack> mNext;
    };

    /**
     * Encapsulates traversal for methods such as containsValue; also
     * serves as a base class for other iterators and spliterators.
     *
     * Method advance visits once each still-valid node that was
     * reachable upon iterator construction. It might miss some that
     * were added to a bin after the bin was visited, which is OK wrt
     * consistency guarantees. Maintaining this property in the face
     * of possible ongoing resizes requires a fair amount of
     * bookkeeping state that is difficult to optimize away amidst
     * volatile accesses.  Even so, traversal maintains reasonable
     * throughput.
     *
     * Normally, iteration proceeds bin-by-bin traversing lists.
     * However, if the table has been resized, then all future steps
     * must traverse both the bin at the current index as well as at
     * (index + baseSize); and so on for further resizings. To
     * paranoically cope with potential sharing by users of iterators
     * across threads, iteration terminates if a bounds checks fails
     * for a table read.
     */
    class Traverser
        : public LightRefBase
    {
    public:
        inline Traverser(
            /* [in] */ Array<Node*>& tab,
            /* [in] */ Integer size,
            /* [in] */ Integer index,
            /* [in] */ Integer limit)
            : mTab(tab)
            , mBaseSize(size)
            , mIndex(index)
            , mBaseIndex(index)
            , mBaseLimit(limit)
        {}

        /**
         * Advances if possible, returning next valid node, or null if none.
         */
        AutoPtr<Node> Advance();

        /**
         * Saves traversal state upon encountering a forwarding node.
         */
        void PushState(
            /* [in] */ Array<Node*>& t,
            /* [in] */ Integer i,
            /* [in] */ Integer n);

        void RecoverState(
            /* [in] */ Integer n);

    public:
        Array<Node*> mTab; // current table; updated if resized
        AutoPtr<Node> mNext; // the next entry to use
        AutoPtr<TableStack> mStack, mSpare; // to save/restore on ForwardingNodes
        Integer mIndex; // index of bin to use next
        Integer mBaseIndex; // current index of initial table
        Integer mBaseLimit; // index bound for initial table
        Integer mBaseSize; // initial table size
    };

    class BaseIterator
        : public Traverser
    {
    public:
        inline BaseIterator(
            /* [in] */ Array<Node*>& tab,
            /* [in] */ Integer size,
            /* [in] */ Integer index,
            /* [in] */ Integer limit,
            /* [in] */ ConcurrentHashMap* map)
            : Traverser(tab, size, index, limit)
            , mMap(map)
        {
            Advance();
        }

        ECode HasNext(
            /* [out] */ Boolean& result);

        ECode HasMoreElements(
            /* [out] */ Boolean& result);

        ECode Remove();

    public:
        ConcurrentHashMap* mMap;
        AutoPtr<Node> mLastReturned;
    };

    class KeyIterator
        : public BaseIterator
        , public IIterator
        , public IEnumeration
    {
    public:
        inline KeyIterator(
            /* [in] */ Array<Node*>& tab,
            /* [in] */ Integer size,
            /* [in] */ Integer index,
            /* [in] */ Integer limit,
            /* [in] */ ConcurrentHashMap* map)
            : BaseIterator(tab, index, size, limit, map)
        {}

        COMO_INTERFACE_DECL();

        ECode Next(
            /* [out] */ AutoPtr<IInterface>& object) override;

        ECode NextElement(
            /* [out] */ IInterface** object = nullptr) override;

        ECode HasNext(
            /* [out] */ Boolean& result) override;

        ECode Remove() override;

        ECode HasMoreElements(
            /* [out] */ Boolean& result) override;
    };

    class ValueIterator
        : public BaseIterator
        , public IIterator
        , public IEnumeration
    {
    public:
        inline ValueIterator(
            /* [in] */ Array<Node*>& tab,
            /* [in] */ Integer size,
            /* [in] */ Integer index,
            /* [in] */ Integer limit,
            /* [in] */ ConcurrentHashMap* map)
            : BaseIterator(tab, index, size, limit, map)
        {}

        COMO_INTERFACE_DECL();

        ECode Next(
            /* [out] */ AutoPtr<IInterface>& object) override;

        ECode NextElement(
            /* [out] */ IInterface** object = nullptr) override;

        ECode HasNext(
            /* [out] */ Boolean& result) override;

        ECode Remove() override;

        ECode HasMoreElements(
            /* [out] */ Boolean& result) override;
    };

    class EntryIterator
        : public BaseIterator
        , public IIterator
    {
    public:
        inline EntryIterator(
            /* [in] */ Array<Node*>& tab,
            /* [in] */ Integer size,
            /* [in] */ Integer index,
            /* [in] */ Integer limit,
            /* [in] */ ConcurrentHashMap* map)
            : BaseIterator(tab, index, size, limit, map)
        {}

        COMO_INTERFACE_DECL();

        ECode Next(
            /* [out] */ AutoPtr<IInterface>& object) override;

        ECode HasNext(
            /* [out] */ Boolean& result) override;

        ECode Remove() override;
    };

    class MapEntry
        : public Object
        , public IMapEntry
    {
    public:
        inline MapEntry(
            /* [in] */ IInterface* key,
            /* [in] */ IInterface* val,
            /* [in] */ ConcurrentHashMap* map)
            : mKey(key)
            , mVal(val)
            , mMap(map)
        {}

        COMO_INTERFACE_DECL();

        ECode GetKey(
            /* [out] */ AutoPtr<IInterface>& key) override;

        ECode GetValue(
            /* [out] */ AutoPtr<IInterface>& value) override;

        ECode GetHashCode(
            /* [out] */ Integer& hash) override;

        ECode ToString(
            /* [out] */ String& desc) override;

        ECode Equals(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean& result) override;

        ECode SetValue(
            /* [in] */ IInterface* value,
            /* [out] */ IInterface** prevValue = nullptr) override;

    public:
        AutoPtr<IInterface> mKey;
        AutoPtr<IInterface> mVal;
        ConcurrentHashMap* mMap;
    };

    class CollectionView
        : public SyncObject
        , public ICollection
        , public ISerializable
    {
    public:
        CollectionView(
            /* [in] */ ConcurrentHashMap* map)
            : mMap(map)
        {}

        COMO_INTERFACE_DECL();

        ECode Clear() override;

        ECode GetSize(
            /* [out] */ Integer& size) override;

        ECode IsEmpty(
            /* [out] */ Boolean& empty) override;

        ECode ToArray(
            /* [out, callee] */ Array<IInterface*>* objs) override;

        ECode ToArray(
            /* [in] */ const InterfaceID& iid,
            /* [out, callee] */ Array<IInterface*>* objs) override;

        ECode ToString(
            /* [out] */ String& desc) override;

        ECode ContainsAll(
            /* [in] */ ICollection* c,
            /* [out] */ Boolean& result) override;

        ECode RemoveAll(
            /* [in] */ ICollection* c,
            /* [out] */ Boolean* changed = nullptr) override;

        ECode RetainAll(
            /* [in] */ ICollection* c,
            /* [out] */ Boolean* changed = nullptr) override;

        using SyncObject::Equals;
        using SyncObject::GetHashCode;

    public:
        ConcurrentHashMap* mMap;
    };

    class KeySetView
        : public CollectionView
        , public ISet
    {
    public:
        inline KeySetView(
            /* [in] */ ConcurrentHashMap* map,
            /* [in] */ IInterface* value)
            : CollectionView(map)
            , mValue(value)
        {}

        COMO_INTERFACE_DECL();

        ECode Contains(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean& result) override;

        ECode Remove(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* changed = nullptr) override;

        ECode GetIterator(
            /* [out] */ AutoPtr<IIterator>& it) override;

        ECode Add(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* changed = nullptr) override;

        ECode AddAll(
            /* [in] */ ICollection* c,
            /* [out] */ Boolean* changed = nullptr) override;

        ECode GetHashCode(
            /* [out] */ Integer& hash) override;

        ECode Equals(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean& result) override;

        ECode Clear() override;

        ECode ContainsAll(
            /* [in] */ ICollection* c,
            /* [out] */ Boolean& result) override;

        ECode GetSize(
            /* [out] */ Integer& size) override;

        ECode IsEmpty(
            /* [out] */ Boolean& empty) override;

        ECode RemoveAll(
            /* [in] */ ICollection* c,
            /* [out] */ Boolean* changed = nullptr) override;

        ECode RetainAll(
            /* [in] */ ICollection* c,
            /* [out] */ Boolean* changed = nullptr) override;

        ECode ToArray(
            /* [out, callee] */ Array<IInterface*>* objs) override;

        ECode ToArray(
            /* [in] */ const InterfaceID& iid,
            /* [out, callee] */ Array<IInterface*>* objs) override;

    private:
        AutoPtr<IInterface> mValue;
    };

    class ValuesView
        : public CollectionView
    {
    public:
        inline ValuesView(
            /* [in] */ ConcurrentHashMap* map)
            : CollectionView(map)
        {}

        ECode Contains(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean& result) override;

        ECode Remove(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* changed = nullptr) override;

        ECode GetIterator(
            /* [out] */ AutoPtr<IIterator>& it) override;

        ECode Add(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* changed = nullptr) override;

        ECode AddAll(
            /* [in] */ ICollection* c,
            /* [out] */ Boolean* changed = nullptr) override;

        ECode Equals(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean& result) override;

        ECode GetHashCode(
            /* [out] */ Integer& hash) override;
    };

    class EntrySetView
        : public CollectionView
        , public ISet
    {
    public:
        inline EntrySetView(
            /* [in] */ ConcurrentHashMap* map)
            : CollectionView(map)
        {}

        COMO_INTERFACE_DECL();

        ECode Contains(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean& result) override;

        ECode Remove(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* changed = nullptr) override;

        ECode GetIterator(
            /* [out] */ AutoPtr<IIterator>& it) override;

        ECode Add(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* changed = nullptr) override;

        ECode AddAll(
            /* [in] */ ICollection* c,
            /* [out] */ Boolean* changed = nullptr) override;

        ECode GetHashCode(
            /* [out] */ Integer& hash) override;

        ECode Equals(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean& result) override;

        ECode Clear() override;

        ECode ContainsAll(
            /* [in] */ ICollection* c,
            /* [out] */ Boolean& result) override;

        ECode GetSize(
            /* [out] */ Integer& size) override;

        ECode IsEmpty(
            /* [out] */ Boolean& empty) override;

        ECode RemoveAll(
            /* [in] */ ICollection* c,
            /* [out] */ Boolean* changed = nullptr) override;

        ECode RetainAll(
            /* [in] */ ICollection* c,
            /* [out] */ Boolean* changed = nullptr) override;

        ECode ToArray(
            /* [out, callee] */ Array<IInterface*>* objs) override;

        ECode ToArray(
            /* [in] */ const InterfaceID& iid,
            /* [out, callee] */ Array<IInterface*>* objs) override;
    };

public:
    COMO_INTERFACE_DECL();

    static Integer GetNCPU();

    static Integer Spread(
        /* [in] */ Integer h);

    static Integer CompareComparables(
        /* [in] */ IInterface* k,
        /* [in] */ IInterface* x);

    static AutoPtr<Node> TabAt(
        /* [in] */ Array<Node*>& tab,
        /* [in] */ Integer i);

    static Boolean CasTabAt(
        /* [in] */ Array<Node*>& tab,
        /* [in] */ Integer i,
        /* [in] */ Node* c,
        /* [in] */ Node* v);

    static void SetTabAt(
        /* [in] */ Array<Node*>& tab,
        /* [in] */ Integer i,
        /* [in] */ Node* v);

    ECode Constructor();

    ECode Constructor(
        /* [in] */ Integer initialCapacity);

    ECode Constructor(
        /* [in] */ IMap* m);

    ECode Constructor(
        /* [in] */ Integer initialCapacity,
        /* [in] */ Float loadFactor);

    ECode Constructor(
        /* [in] */ Integer initialCapacity,
        /* [in] */ Float loadFactor,
        /* [in] */ Integer concurrencyLevel);

    ECode GetSize(
        /* [out] */ Integer& size) override;

    ECode IsEmpty(
        /* [out] */ Boolean& result) override;

    ECode Get(
        /* [in] */ IInterface* key,
        /* [out] */ AutoPtr<IInterface>& value) override;

    ECode ContainsKey(
        /* [in] */ IInterface* key,
        /* [out] */ Boolean& result) override;

    ECode ContainsValue(
        /* [in] */ IInterface* value,
        /* [out] */ Boolean& result) override;

    ECode Put(
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* value,
        /* [out] */ IInterface** prevValue = nullptr) override;

    ECode PutVal(
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* value,
        /* [in] */ Boolean onlyIfAbsent,
        /* [out] */ IInterface** prevValue = nullptr);

    ECode PutAll(
        /* [in] */ IMap* m) override;

    ECode Remove(
        /* [in] */ IInterface* key,
        /* [out] */ IInterface** prevValue = nullptr) override;

    ECode ReplaceNode(
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* value,
        /* [in] */ IInterface* cv,
        /* [out] */ IInterface** prevValue = nullptr);

    ECode Clear() override;

    ECode GetKeySet(
        /* [out] */ AutoPtr<ISet>& keys) override;

    ECode GetValues(
        /* [out] */ AutoPtr<ICollection>& values) override;

    ECode GetEntrySet(
        /* [out] */ AutoPtr<ISet>& entries) override;

    ECode GetHashCode(
        /* [out] */ Integer& hash) override;

    ECode ToString(
        /* [out] */ String& desc) override;

    ECode Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean& result) override;

    ECode PutIfAbsent(
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* value,
        /* [out] */ IInterface** prevValue = nullptr) override;

    ECode Remove(
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* value,
        /* [out] */ Boolean* result = nullptr) override;

    ECode Replace(
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* oldValue,
        /* [in] */ IInterface* newValue,
        /* [out] */ Boolean* result = nullptr) override;

    ECode Replace(
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* value,
        /* [out] */ IInterface** prevValue = nullptr) override;

    ECode Contains(
        /* [in] */ IInterface* value,
        /* [out] */ Boolean& result) override;

    ECode Keys(
        /* [out] */ AutoPtr<IEnumeration>& keys) override;

    ECode Elements(
        /* [out] */ AutoPtr<IEnumeration>& elements) override;

    ECode GetMappingCount(
        /* [out] */ Long& count) override;

    ECode GetKeySet(
        /* [in] */ IInterface* mappedValue,
        /* [out] */ AutoPtr<ISet>& keys) override;

    static Integer ResizeStamp(
        /* [in] */ Integer n);

    Array<Node*> HelpTransfer(
        /* [in] */ Array<Node*>& tab,
        /* [in] */ Node* f);

    Long SumCount();

    AutoPtr<Node> Untreeify(
        /* [in] */ Node* b);

private:
    static Integer TableSizeFor(
        /* [in] */ Integer c);

    Array<Node*> InitTable();

    void AddCount(
        /* [in] */ Long x,
        /* [in] */ Integer check);

    void TryPresize(
        /* [in] */ Integer size);

    void Transfer(
        /* [in] */ Array<Node*>& tab,
        /* [in] */ Array<Node*>* nextTab);

    void FullAddCount(
        /* [in] */ Long x,
        /* [in] */ Boolean wasUncontended);

    void TreeifyBin(
        /* [in] */ Array<Node*>& tab,
        /* [in] */ Integer index);

public:
    /**
     * The largest possible (non-power of two) array size.
     * Needed by toArray and related methods.
     */
    static constexpr Integer MAX_ARRAY_SIZE = IInteger::MAX_VALUE - 8;

    /**
     * The bin count threshold for using a tree rather than list for a
     * bin.  Bins are converted to trees when adding an element to a
     * bin with at least this many nodes. The value must be greater
     * than 2, and should be at least 8 to mesh with assumptions in
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
     * The value should be at least 4 * TREEIFY_THRESHOLD to avoid
     * conflicts between resizing and treeification thresholds.
     */
    static constexpr Integer MIN_TREEIFY_CAPACITY = 64;

    static constexpr Integer MOVED = -1; // hash for forwarding nodes
    static constexpr Integer TREEBIN = -2; // hash for roots of trees
    static constexpr Integer RESERVED = -3; // hash for transient reservations
    static constexpr Integer HASH_BITS = 0x7fffffff; // usable bits of normal node hash

private:
    /**
     * The largest possible table capacity.  This value must be
     * exactly 1<<30, and is further required
     * because the top two bits of 32bit hash fields are used for
     * control purposes.
     */
    static constexpr Integer MAXIMUM_CAPACITY = 1 << 30;

    /**
     * The default initial table capacity.  Must be a power of 2
     * (i.e., at least 1) and at most MAXIMUM_CAPACITY.
     */
    static constexpr Integer DEFAULT_CAPACITY = 16;

    /**
     * Minimum number of rebinnings per transfer step. Ranges are
     * subdivided to allow multiple resizer threads.  This value
     * serves as a lower bound to avoid resizers encountering
     * excessive memory contention.  The value should be at least
     * DEFAULT_CAPACITY.
     */
    static constexpr Integer MIN_TRANSFER_STRIDE = 16;

    /**
     * The number of bits used for generation stamp in sizeCtl.
     * Must be at least 6 for 32bit arrays.
     */
    static constexpr Integer RESIZE_STAMP_BITS = 16;

    /**
     * The maximum number of threads that can help resize.
     * Must fit in 32 - RESIZE_STAMP_BITS bits.
     */
    static constexpr Integer MAX_RESIZERS = (1 << (32 - RESIZE_STAMP_BITS)) - 1;

    /**
     * The bit shift for recording size stamp in sizeCtl.
     */
    static constexpr Integer RESIZE_STAMP_SHIFT = 32 - RESIZE_STAMP_BITS;

    /**
     * The array of bins. Lazily initialized upon first insertion.
     * Size is always a power of two. Accessed directly by iterators.
     */
    VOLATILE Array<Node*> mTable;

    /**
     * The next table to use; non-null only while resizing.
     */
    VOLATILE Array<Node*> mNextTable;

    /**
     * Base counter value, used mainly when there is no contention,
     * but also as a fallback during table initialization
     * races. Updated via CAS.
     */
    VOLATILE Long mBaseCount;

    /**
     * Table initialization and resizing control.  When negative, the
     * table is being initialized or resized: -1 for initialization,
     * else -(1 + the number of active resizing threads).  Otherwise,
     * when table is null, holds the initial table size to use upon
     * creation, or 0 for default. After initialization, holds the
     * next element count value upon which to resize the table.
     */
    VOLATILE Integer mSizeCtl;

    /**
     * The next table index (plus one) to split while resizing.
     */
    VOLATILE Integer mTransferIndex;

    /**
     * Spinlock (locked via CAS) used when resizing and/or creating CounterCells.
     */
    VOLATILE Integer mCellsBusy;

    /**
     * Table of counter cells. When non-null, size is a power of 2.
     */
    VOLATILE Array<CounterCell*> mCounterCells;

    AutoPtr<KeySetView> mKeySet;
    AutoPtr<ValuesView> mValues;
    AutoPtr<EntrySetView> mEntrySet;
};

inline Integer ConcurrentHashMap::Spread(
    /* [in] */ Integer h)
{
    return (h ^ (((UInteger)h) >> 16)) & HASH_BITS;
}

inline ECode ConcurrentHashMap::Constructor()
{
    return NOERROR;
}

inline ECode ConcurrentHashMap::Constructor(
    /* [in] */ Integer initialCapacity,
    /* [in] */ Float loadFactor)
{
    return Constructor(initialCapacity, loadFactor, 1);
}

}
}
}

#endif // __COMO_UTIL_CONCURRENT_CONCURRENTHASHMAP_H__
