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

#ifndef __CCM_UTIL_TREEMAP_H__
#define __CCM_UTIL_TREEMAP_H__

#include "ccm/core/SyncObject.h"
#include "ccm/util/AbstractMap.h"
#include "ccm/util/AbstractSet.h"
#include "ccm.core.ICloneable.h"
#include "ccm.io.IObjectInputStream.h"
#include "ccm.io.ISerializable.h"
#include "ccm.util.IComparator.h"
#include "ccm.util.IIterator.h"
#include "ccm.util.IMapEntry.h"
#include "ccm.util.INavigableMap.h"
#include "ccm.util.INavigableSet.h"
#include "ccm.util.ISortedMap.h"
#include "ccm.util.ISortedSet.h"
#include "ccm.util.ITreeMap.h"
#include <ccmautoptr.h>

using ccm::core::ICloneable;
using ccm::core::SyncObject;
using ccm::io::IObjectInputStream;
using ccm::io::ISerializable;

namespace ccm {
namespace util {

class TreeMap
    : public AbstractMap
    , public ITreeMap
    , public INavigableMap
    , public ISortedMap
    , public ICloneable
    , public ISerializable
{
private:
    class Values
        : public AbstractCollection
    {
    public:
        Values(
            /* [in] */ TreeMap* owner)
            : mOwner(owner)
        {}

        ECode GetIterator(
            /* [out] */ IIterator** it) override;

        ECode GetSize(
            /* [out] */ Integer* size) override;

        ECode Contains(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* result) override;

        ECode Remove(
            /* [in] */ IInterface* obj,
            /* [out]　*/ Boolean* contained = nullptr) override;

        ECode Clear() override;

    private:
        TreeMap* mOwner;
    };

    class EntrySet
        : public AbstractSet
    {
    public:
        EntrySet(
            /* [in] */ TreeMap* owner)
            : mOwner(owner)
        {}

        ECode GetIterator(
            /* [out] */ IIterator** it) override;

        ECode Contains(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* result) override;

        ECode Remove(
            /* [in] */ IInterface* obj,
            /* [out]　*/ Boolean* contained = nullptr) override;

        ECode GetSize(
            /* [out] */ Integer* size) override;

        ECode Clear() override;

    private:
        TreeMap* mOwner;
    };

    class KeySet
        : public AbstractSet
        , public INavigableSet
        , public ISortedSet
    {
    public:
        KeySet(
            /* [in] */ INavigableMap* map,
            /* [in] */ Boolean holdRef);

        ~KeySet();

        CCM_INTERFACE_DECL();

        ECode GetIterator(
            /* [out] */ IIterator** it) override;

        ECode GetDescendingIterator(
            /* [out] */ IIterator** it) override;

        ECode GetSize(
            /* [out] */ Integer* size) override;

        ECode IsEmpty(
            /* [out] */ Boolean* result) override;

        ECode Contains(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* result) override;

        ECode Clear() override;

        ECode Lower(
            /* [in] */ IInterface* e,
            /* [out] */ IInterface** lowerE) override;

        ECode Floor(
            /* [in] */ IInterface* e,
            /* [out] */ IInterface** floorE) override;

        ECode Ceiling(
            /* [in] */ IInterface* e,
            /* [out] */ IInterface** ceilingE) override;

        ECode Higher(
            /* [in] */ IInterface* e,
            /* [out] */ IInterface** higherE) override;

        ECode First(
            /* [out] */ IInterface** element) override;

        ECode Last(
            /* [out] */ IInterface** element) override;

        ECode Comparator(
            /* [out] */ IComparator** comparator) override;

        ECode PollFirst(
            /* [out] */ IInterface** e) override;

        ECode PollLast(
            /* [out] */ IInterface** e) override;

        ECode Remove(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* contained = nullptr) override;

        ECode SubSet(
            /* [in] */ IInterface* fromElement,
            /* [in] */ Boolean fromInclusive,
            /* [in] */ IInterface* toElement,
            /* [in] */ Boolean toInclusive,
            /* [out] */ INavigableSet** subset) override;

        ECode HeadSet(
            /* [in] */ IInterface* toElement,
            /* [in] */ Boolean inclusive,
            /* [out] */ INavigableSet** headset) override;

        ECode TailSet(
            /* [in] */ IInterface* fromElement,
            /* [in] */ Boolean inclusive,
            /* [out] */ INavigableSet** tailset) override;

        ECode SubSet(
            /* [in] */ IInterface* fromElement,
            /* [in] */ IInterface* toElement,
            /* [out] */ ISortedSet** subset) override;

        ECode HeadSet(
            /* [in] */ IInterface* toElement,
            /* [out] */ ISortedSet** headset) override;

        ECode TailSet(
            /* [in] */ IInterface* fromElement,
            /* [out] */ ISortedSet** tailset) override;

        ECode DescendingSet(
            /* [out] */ INavigableSet** set) override;

    private:
        INavigableMap* mMap;
        Boolean mHoldRef;
    };

    class TreeMapEntry;

    class PrivateEntryIterator
        : public Object
        , public IIterator
    {
    public:
        PrivateEntryIterator(
            /* [in] */ TreeMapEntry* first);

        CCM_INTERFACE_DECL();

        ECode HasNext(
            /* [out] */ Boolean* result) override;

        ECode Remove() override;
    };

    class EntryIterator
        : public PrivateEntryIterator
    {
    public:
        EntryIterator(
            /* [in] */ TreeMapEntry* first);

        ECode Next(
            /* [out] */ IInterface** object = nullptr) override;
    };

    class ValueIterator
        : public PrivateEntryIterator
    {
    public:
        ValueIterator(
            /* [in] */ TreeMapEntry* first);

        ECode Next(
            /* [out] */ IInterface** object = nullptr) override;
    };

    class KeyIterator
        : public PrivateEntryIterator
    {
    public:
        KeyIterator(
            /* [in] */ TreeMapEntry* first);

        ECode Next(
            /* [out] */ IInterface** object = nullptr) override;
    };

    class DescendingKeyIterator
        : public PrivateEntryIterator
    {
    public:
        DescendingKeyIterator(
            /* [in] */ TreeMapEntry* first);

        ECode Next(
            /* [out] */ IInterface** object = nullptr) override;

        ECode Remove() override;
    };

    class NavigableSubMap
        : public AbstractMap
        , public INavigableMap
        , public ISortedMap
        , public ISerializable
    {
    public:
        CCM_INTERFACE_DECL();

        virtual AutoPtr<IIterator> GetKeyIterator() = 0;

        virtual AutoPtr<IIterator> GetDescendingKeyIterator() = 0;

        ECode IsEmpty(
            /* [out] */ Boolean* result) override;

        ECode GetSize(
            /* [out] */ Integer* size) override;

        ECode ContainsKey(
            /* [in] */ IInterface* key,
            /* [out] */ Boolean* result) override;

        ECode Put(
            /* [in] */ IInterface* key,
            /* [in] */ IInterface* value,
            /* [out] */ IInterface** prevValue = nullptr) override;

        ECode Get(
            /* [in] */ IInterface* key,
            /* [out] */ IInterface** value) override;

        ECode Remove(
            /* [in] */ IInterface* key,
            /* [out] */ IInterface** prevValue = nullptr) override;

        ECode CeilingEntry(
            /* [in] */ IInterface* key,
            /* [out]　*/ IMapEntry** entry) override;

        ECode CeilingKey(
            /* [in] */ IInterface* key,
            /* [out] */ IInterface** ceilingkey) override;

        ECode HigherEntry(
            /* [in] */ IInterface* key,
            /* [out] */ IMapEntry** entry) override;

        ECode HigherKey(
            /* [in] */ IInterface* key,
            /* [out] */ IInterface** higherkey) override;

        ECode FloorEntry(
            /* [in] */ IInterface* key,
            /* [out] */ IMapEntry** entry) override;

        ECode FloorKey(
            /* [in] */ IInterface* key,
            /* [out] */ IInterface** floorkey) override;

        ECode LowerEntry(
            /* [in] */ IInterface* key,
            /* [out] */ IMapEntry** entry) override;

        ECode LowerKey(
            /* [in] */ IInterface* key,
            /* [out] */ IInterface** lowerkey) override;

        ECode FirstKey(
            /* [out] */ IInterface** key) override;

        ECode LastKey(
            /* [out] */ IInterface** key) override;

        ECode FirstEntry(
            /* [out] */ IMapEntry** entry) override;

        ECode LastEntry(
            /* [out] */ IMapEntry** entry) override;

        ECode PollFirstEntry(
            /* [out] */ IMapEntry** entry) override;

        ECode PollLastEntry(
            /* [out] */ IMapEntry** entry) override;

        ECode NavigableKeySet(
            /* [out] */ INavigableSet** keyset) override;

        ECode GetKeySet(
            /* [out] */ ISet** keys) override;

        ECode DescendingKeySet(
            /* [out] */ INavigableSet** keyset) override;

        ECode SubMap(
            /* [in] */ IInterface* fromKey,
            /* [in] */ IInterface* toKey,
            /* [out] */ ISortedMap** submap) override;

        ECode HeadMap(
            /* [in] */ IInterface* toKey,
            /* [out] */ ISortedMap** headmap) override;

        ECode TailMap(
            /* [in] */ IInterface* fromKey,
            /* [out] */ ISortedMap** tailmap) override;
    };

    class AscendingSubMap
        : public NavigableSubMap
    {
    public:
        ECode Constructor(
            /* [in] */ TreeMap* m,
            /* [in] */ Boolean fromStart,
            /* [in] */ IInterface* lo,
            /* [in] */ Boolean loInclusive,
            /* [in] */ Boolean toEnd,
            /* [in] */ IInterface* hi,
            /* [in] */ Boolean hiInclusive);

        ECode Comparator(
            /* [out] */ IComparator** comparator) override;

        ECode SubMap(
            /* [in] */ IInterface* fromKey,
            /* [in] */ Boolean fromInclusive,
            /* [in] */ IInterface* toKey,
            /* [in] */ Boolean toInclusive,
            /* [out] */ INavigableMap** submap) override;

        ECode HeadMap(
            /* [in] */ IInterface* key,
            /* [in] */ Boolean inclusive,
            /* [out] */ INavigableMap** headmap) override;

        ECode TailMap(
            /* [in] */ IInterface* fromKey,
            /* [in] */ Boolean inclusive,
            /* [out] */ INavigableMap** tailmap) override;

        ECode DescendingMap(
            /* [out] */ INavigableMap** map) override;

        AutoPtr<IIterator> GetKeyIterator() override;

        AutoPtr<IIterator> GetDescendingKeyIterator() override;

        ECode GetEntrySet(
            /* [out] */ ISet** entries) override;
    };

    class DescendingSubMap
        : public NavigableSubMap
    {
    public:
        ECode Constructor(
            /* [in] */ TreeMap* m,
            /* [in] */ Boolean fromStart,
            /* [in] */ IInterface* lo,
            /* [in] */ Boolean loInclusive,
            /* [in] */ Boolean toEnd,
            /* [in] */ IInterface* hi,
            /* [in] */ Boolean hiInclusive);

        ECode Comparator(
            /* [out] */ IComparator** comparator) override;

        ECode SubMap(
            /* [in] */ IInterface* fromKey,
            /* [in] */ Boolean fromInclusive,
            /* [in] */ IInterface* toKey,
            /* [in] */ Boolean toInclusive,
            /* [out] */ INavigableMap** submap) override;

        ECode HeadMap(
            /* [in] */ IInterface* key,
            /* [in] */ Boolean inclusive,
            /* [out] */ INavigableMap** headmap) override;

        ECode TailMap(
            /* [in] */ IInterface* fromKey,
            /* [in] */ Boolean inclusive,
            /* [out] */ INavigableMap** tailmap) override;

        ECode DescendingMap(
            /* [out] */ INavigableMap** map) override;

        AutoPtr<IIterator> GetKeyIterator() override;

        AutoPtr<IIterator> GetDescendingKeyIterator() override;

        ECode GetEntrySet(
            /* [out] */ ISet** entries) override;
    };

    class TreeMapEntry
        : public SyncObject
        , public IMapEntry
    {
    public:
        TreeMapEntry(
            /* [in] */ IInterface* key,
            /* [in] */ IInterface* value,
            /* [in] */ TreeMapEntry* parent)
            : mKey(key)
            , mValue(value)
            , mParent(parent)
        {}

        CCM_INTERFACE_DECL();

        ECode GetKey(
            /* [out] */ IInterface** key) override;

        ECode GetValue(
            /* [out] */ IInterface** value) override;

        ECode SetValue(
            /* [in] */ IInterface* value,
            /* [out] */ IInterface** prevValue = nullptr) override;

        ECode Equals(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* result) override;

        ECode GetHashCode(
            /* [out] */ Integer* hash) override;

        ECode ToString(
            /* [out] */ String* str) override;

    public:
        AutoPtr<IInterface> mKey;
        AutoPtr<IInterface> mValue;
        AutoPtr<TreeMapEntry> mLeft;
        AutoPtr<TreeMapEntry> mRight;
        TreeMapEntry* mParent;
        Boolean mColor = BLACK;
    };

public:
    CCM_INTERFACE_DECL();

    ECode Constructor();

    ECode Constructor(
        /* [in] */ IComparator* comparator);

    ECode Constructor(
        /* [in] */ IMap* m);

    ECode Constructor(
        /* [in] */ ISortedMap* m);

    ECode GetSize(
        /* [out] */ Integer* size) override;

    ECode ContainsKey(
        /* [in] */ IInterface* key,
        /* [out] */ Boolean* result) override;

    ECode ContainsValue(
        /* [in] */ IInterface* value,
        /* [out] */ Boolean* result) override;

    ECode Get(
        /* [in] */ IInterface* key,
        /* [out] */ IInterface** value) override;

    ECode Comparator(
        /* [out] */ IComparator** comparator) override;

    ECode FirstKey(
        /* [out] */ IInterface** key) override;

    ECode LastKey(
        /* [out] */ IInterface** key) override;

    ECode PutAll(
        /* [in] */ IMap* map) override;

    ECode Put(
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* value,
        /* [out] */ IInterface** prevValue = nullptr) override;

    ECode Remove(
        /* [in] */ IInterface* key,
        /* [out] */ IInterface** prevValue = nullptr) override;

    ECode Clear() override;

    ECode FirstEntry(
        /* [out] */ IMapEntry** entry) override;

    ECode LastEntry(
        /* [out] */ IMapEntry** entry) override;

    ECode PollFirstEntry(
        /* [out] */ IMapEntry** entry) override;

    ECode PollLastEntry(
        /* [out] */ IMapEntry** entry) override;

    ECode LowerEntry(
        /* [in] */ IInterface* key,
        /* [out] */ IMapEntry** entry) override;

    ECode LowerKey(
        /* [in] */ IInterface* key,
        /* [out] */ IInterface** lowerkey) override;

    ECode FloorEntry(
        /* [in] */ IInterface* key,
        /* [out] */ IMapEntry** entry) override;

    ECode FloorKey(
        /* [in] */ IInterface* key,
        /* [out] */ IInterface** floorkey) override;

    ECode CeilingEntry(
        /* [in] */ IInterface* key,
        /* [out]　*/ IMapEntry** entry) override;

    ECode CeilingKey(
        /* [in] */ IInterface* key,
        /* [out] */ IInterface** ceilingkey) override;

    ECode HigherEntry(
        /* [in] */ IInterface* key,
        /* [out] */ IMapEntry** entry) override;

    ECode HigherKey(
        /* [in] */ IInterface* key,
        /* [out] */ IInterface** higherkey) override;

    ECode GetKeySet(
        /* [out] */ ISet** keys) override;

    ECode NavigableKeySet(
        /* [out] */ INavigableSet** keyset) override;

    ECode DescendingKeySet(
        /* [out] */ INavigableSet** keyset) override;

    ECode GetValues(
        /* [out] */ ICollection** values) override;

    ECode GetEntrySet(
        /* [out] */ ISet** entries) override;

    ECode DescendingMap(
        /* [out] */ INavigableMap** map) override;

    ECode SubMap(
        /* [in] */ IInterface* fromKey,
        /* [in] */ Boolean fromInclusive,
        /* [in] */ IInterface* toKey,
        /* [in] */ Boolean toInclusive,
        /* [out] */ INavigableMap** submap) override;

    ECode HeadMap(
        /* [in] */ IInterface* key,
        /* [in] */ Boolean inclusive,
        /* [out] */ INavigableMap** headmap) override;

    ECode TailMap(
        /* [in] */ IInterface* fromKey,
        /* [in] */ Boolean inclusive,
        /* [out] */ INavigableMap** tailmap) override;

    ECode SubMap(
        /* [in] */ IInterface* fromKey,
        /* [in] */ IInterface* toKey,
        /* [out] */ ISortedMap** submap) override;

    ECode HeadMap(
        /* [in] */ IInterface* toKey,
        /* [out] */ ISortedMap** headmap) override;

    ECode TailMap(
        /* [in] */ IInterface* fromKey,
        /* [out] */ ISortedMap** tailmap) override;






    ECode AddAllForTreeSet(
        /* [in] */ ISortedSet* set,
        /* [in] */ IInterface* defaultVal)
    {
        return NOERROR;
    }

protected:
    ECode CloneImpl(
        /* [in] */ ITreeMap* newObj);

private:
    ECode GetEntry(
        /* [in] */ IInterface* key,
        /* [out] */ TreeMapEntry** entry);

    AutoPtr<TreeMapEntry> GetEntryUsingComparator(
        /* [in] */ IInterface* key);

    ECode GetCeilingEntry(
        /* [in] */ IInterface* key,
        /* [out] */ TreeMapEntry** entry);

    ECode GetFloorEntry(
        /* [in] */ IInterface* key,
        /* [out] */ TreeMapEntry** entry);

    ECode GetHigherEntry(
        /* [in] */ IInterface* key,
        /* [out] */ TreeMapEntry** entry);

    ECode GetLowerEntry(
        /* [in] */ IInterface* key,
        /* [out] */ TreeMapEntry** entry);

    AutoPtr<IIterator> GetKeyIterator();

    AutoPtr<IIterator> GetDescendingKeyIterator();




    static AutoPtr<IMapEntry> ExportEntry(
        /* [in] */ TreeMapEntry* e);

    Integer Compare(
        /* [in] */ IInterface* k1,
        /* [in] */ IInterface* k2);

    void DeleteEntry(
        /* [in] */ TreeMapEntry* p);

    void FixAfterInsertion(
        /* [in] */ TreeMapEntry* x);




    static Boolean ValEquals(
        /* [in] */ IInterface* o1,
        /* [in] */ IInterface* o2);

    AutoPtr<TreeMapEntry> GetFirstEntry();

    AutoPtr<TreeMapEntry> GetLastEntry();

    static AutoPtr<TreeMapEntry> Successor(
        /* [in] */ TreeMapEntry* t);

    static AutoPtr<TreeMapEntry> Predecessor(
        /* [in] */ TreeMapEntry* t);

    ECode BuildFromSorted(
        /* [in] */ Integer size,
        /* [in] */ IIterator* it,
        /* [in] */ IObjectInputStream* str,
        /* [in] */ IInterface* defaultVal,
        /* [out] */ TreeMapEntry** entry = nullptr);

    ECode BuildFromSorted(
        /* [in] */ Integer level,
        /* [in] */ Integer lo,
        /* [in] */ Integer hi,
        /* [in] */ Integer redLevel,
        /* [in] */ IIterator* it,
        /* [in] */ IObjectInputStream* str,
        /* [in] */ IInterface* defaultVal,
        /* [out] */ TreeMapEntry** entry = nullptr);

    static Integer ComputeRedLevel(
        /* [in] */ Integer sz);

private:
    AutoPtr<IComparator> mComparator;

    AutoPtr<TreeMapEntry> mRoot;

    /**
     * The number of entries in the tree
     */
    Integer mSize = 0;

    /**
     * The number of structural modifications to the tree.
     */
    Integer mModCount = 0;

    static constexpr Boolean RED = false;
    static constexpr Boolean BLACK = true;

    AutoPtr<EntrySet> mEntrySet;
    AutoPtr<KeySet> mNavigableKeySet;
    AutoPtr<INavigableMap> mDescendingMap;
};

}
}

#endif // __CCM_UTIL_TREEMAP_H__
