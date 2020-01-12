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

#ifndef __COMO_UTIL_TREEMAP_H__
#define __COMO_UTIL_TREEMAP_H__

#include "como/core/SyncObject.h"
#include "como/util/AbstractMap.h"
#include "como/util/AbstractSet.h"
#include "como.core.ICloneable.h"
#include "como.io.IObjectInputStream.h"
#include "como.io.ISerializable.h"
#include "como.util.IComparator.h"
#include "como.util.IIterator.h"
#include "como.util.IMapEntry.h"
#include "como.util.INavigableMap.h"
#include "como.util.INavigableSet.h"
#include "como.util.ISortedMap.h"
#include "como.util.ISortedSet.h"
#include "como.util.ITreeMap.h"
#include <comosp.h>

using como::core::ICloneable;
using como::core::SyncObject;
using como::io::IObjectInputStream;
using como::io::ISerializable;

namespace como {
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

        COMO_INTERFACE_DECL();

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

        ECode Add(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* modified = nullptr) override;

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
            /* [in] */ TreeMapEntry* first,
            /* [in] */ TreeMap* owner);

        COMO_INTERFACE_DECL();

        ECode HasNext(
            /* [out] */ Boolean* result) override;

        ECode Remove() override;

    protected:
        ECode NextEntry(
            /* [out] */ TreeMapEntry** entry);

        ECode PrevEntry(
            /* [out] */ TreeMapEntry** entry);

    protected:
        AutoPtr<TreeMapEntry> mNext;
        AutoPtr<TreeMapEntry> mLastReturned;
        Integer mExpectedModCount = 0;
        AutoPtr<TreeMap> mOwner;
    };

    class EntryIterator
        : public PrivateEntryIterator
    {
    public:
        EntryIterator(
            /* [in] */ TreeMapEntry* first,
            /* [in] */ TreeMap* owner)
            : PrivateEntryIterator(first, owner)
        {}

        ECode Next(
            /* [out] */ IInterface** object = nullptr) override;
    };

    class ValueIterator
        : public PrivateEntryIterator
    {
    public:
        ValueIterator(
            /* [in] */ TreeMapEntry* first,
            /* [in] */ TreeMap* owner)
            : PrivateEntryIterator(first, owner)
        {}

        ECode Next(
            /* [out] */ IInterface** object = nullptr) override;
    };

    class KeyIterator
        : public PrivateEntryIterator
    {
    public:
        KeyIterator(
            /* [in] */ TreeMapEntry* first,
            /* [in] */ TreeMap* owner)
            : PrivateEntryIterator(first, owner)
        {}

        ECode Next(
            /* [out] */ IInterface** object = nullptr) override;
    };

    class DescendingKeyIterator
        : public PrivateEntryIterator
    {
    public:
        DescendingKeyIterator(
            /* [in] */ TreeMapEntry* first,
            /* [in] */ TreeMap* owner)
            : PrivateEntryIterator(first, owner)
        {}

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
    protected:
        class EntrySetView
            : public AbstractSet
        {
        public:
            EntrySetView(
                /* [in] */ NavigableSubMap* owner)
                : mOwner(owner)
            {}

            ECode GetSize(
                /* [out] */ Integer* size) override;

            ECode IsEmpty(
                /* [out] */ Boolean* result) override;

            ECode Contains(
                /* [in] */ IInterface* obj,
                /* [out] */ Boolean* result) override;

            ECode Remove(
                /* [in] */ IInterface* obj,
                /* [out]　*/ Boolean* contained = nullptr) override;

        protected:
            NavigableSubMap* mOwner;

        private:
            Integer mSize = -1;
            Integer mSizeModCount = 0;
        };

        class SubMapIterator
            : public Object
            , public IIterator
        {
        public:
            SubMapIterator(
                /* [in] */ TreeMapEntry* first,
                /* [in] */ TreeMapEntry* fence,
                /* [in] */ NavigableSubMap* owner);

            COMO_INTERFACE_DECL();

            ECode HasNext(
                /* [out] */ Boolean* result) override;

        protected:
            ECode NextEntry(
                /* [out] */ TreeMapEntry** entry);

            ECode PrevEntry(
                /* [out] */ TreeMapEntry** entry);

            ECode RemoveAscending();

            ECode RemoveDescending();

        protected:
            AutoPtr<TreeMapEntry> mLastReturned;
            AutoPtr<TreeMapEntry> mNext;
            AutoPtr<IInterface> mFenceKey;
            Integer mExpectedModCount;

            AutoPtr<NavigableSubMap> mOwner;
        };

        class SubMapEntryIterator
            : public SubMapIterator
        {
        public:
            SubMapEntryIterator(
                /* [in] */ TreeMapEntry* first,
                /* [in] */ TreeMapEntry* fence,
                /* [in] */ NavigableSubMap* owner)
                : SubMapIterator(first, fence, owner)
            {}

            ECode Next(
                /* [out] */ IInterface** object = nullptr) override;

            ECode Remove() override;
        };

        class DescendingSubMapEntryIterator
            : public SubMapIterator
        {
        public:
            DescendingSubMapEntryIterator(
                /* [in] */ TreeMapEntry* first,
                /* [in] */ TreeMapEntry* fence,
                /* [in] */ NavigableSubMap* owner)
                : SubMapIterator(first, fence, owner)
            {}

            ECode Next(
                /* [out] */ IInterface** object = nullptr) override;

            ECode Remove() override;
        };

        class SubMapKeyIterator
            : public SubMapIterator
        {
        public:
            SubMapKeyIterator(
                /* [in] */ TreeMapEntry* first,
                /* [in] */ TreeMapEntry* fence,
                /* [in] */ NavigableSubMap* owner)
                : SubMapIterator(first, fence, owner)
            {}

            ECode Next(
                /* [out] */ IInterface** object = nullptr) override;

            ECode Remove() override;
        };

        class DescendingSubMapKeyIterator
            : public SubMapIterator
        {
        public:
            DescendingSubMapKeyIterator(
                /* [in] */ TreeMapEntry* first,
                /* [in] */ TreeMapEntry* fence,
                /* [in] */ NavigableSubMap* owner)
                : SubMapIterator(first, fence, owner)
            {}

            ECode Next(
                /* [out] */ IInterface** object = nullptr) override;

            ECode Remove() override;
        };

    public:
        ~NavigableSubMap();

        COMO_INTERFACE_DECL();

        ECode Constructor(
            /* [in] */ TreeMap* m,
            /* [in] */ Boolean fromStart,
            /* [in] */ IInterface* lo,
            /* [in] */ Boolean loInclusive,
            /* [in] */ Boolean toEnd,
            /* [in] */ IInterface* hi,
            /* [in] */ Boolean hiInclusive,
            /* [in] */ Boolean holdRef);

        Boolean TooLow(
            /* [in] */ IInterface* key);

        Boolean TooHigh(
            /* [in] */ IInterface* key);

        Boolean InRange(
            /* [in] */ IInterface* key);

        Boolean InClosedRange(
            /* [in] */ IInterface* key);

        Boolean InRange(
            /* [in] */ IInterface* key,
            /* [in] */ Boolean inclusive);

        AutoPtr<TreeMapEntry> AbsLowest();

        AutoPtr<TreeMapEntry> AbsHighest();

        AutoPtr<TreeMapEntry> AbsCeiling(
            /* [in] */ IInterface* key);

        AutoPtr<TreeMapEntry> AbsHigher(
            /* [in] */ IInterface* key);

        AutoPtr<TreeMapEntry> AbsFloor(
            /* [in] */ IInterface* key);

        AutoPtr<TreeMapEntry> AbsLower(
            /* [in] */ IInterface* key);

        AutoPtr<TreeMapEntry> AbsHighFence();

        AutoPtr<TreeMapEntry> AbsLowFence();

        virtual AutoPtr<TreeMapEntry> SubLowest() = 0;

        virtual AutoPtr<TreeMapEntry> SubHighest() = 0;

        virtual AutoPtr<TreeMapEntry> SubCeiling(
            /* [in] */ IInterface* key) = 0;

        virtual AutoPtr<TreeMapEntry> SubHigher(
            /* [in] */ IInterface* key) = 0;

        virtual AutoPtr<TreeMapEntry> SubFloor(
            /* [in] */ IInterface* key) = 0;

        virtual AutoPtr<TreeMapEntry> SubLower(
            /* [in] */ IInterface* key) = 0;

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

        using INavigableMap::SubMap;

        ECode HeadMap(
            /* [in] */ IInterface* toKey,
            /* [out] */ ISortedMap** headmap) override;

        using INavigableMap::HeadMap;

        ECode TailMap(
            /* [in] */ IInterface* fromKey,
            /* [out] */ ISortedMap** tailmap) override;

        using INavigableMap::TailMap;

    protected:
        TreeMap* mMap;

        /**
         * Endpoints are represented as triples (fromStart, lo,
         * loInclusive) and (toEnd, hi, hiInclusive). If fromStart is
         * true, then the low (absolute) bound is the start of the
         * backing map, and the other values are ignored. Otherwise,
         * if loInclusive is true, lo is the inclusive bound, else lo
         * is the exclusive bound. Similarly for the upper bound.
         */
        AutoPtr<IInterface> mLo;
        AutoPtr<IInterface> mHi;
        Boolean mFromStart;
        Boolean mToEnd;
        Boolean mLoInclusive;
        Boolean mHiInclusive;

        AutoPtr<INavigableMap> mDescendingMapView;
        AutoPtr<EntrySetView> mEntrySetView;
        AutoPtr<KeySet> mNavigableKeySetView;

        Boolean mHoldRef;
    };

    class AscendingSubMap final
        : public NavigableSubMap
    {
    public:
        class AscendingEntrySetView
            : public EntrySetView
        {
        public:
            AscendingEntrySetView(
                /* [in] */ NavigableSubMap* owner)
                : EntrySetView(owner)
            {}

            ECode GetIterator(
                /* [out] */ IIterator** it) override;
        };

    public:
        ECode Constructor(
            /* [in] */ TreeMap* m,
            /* [in] */ Boolean fromStart,
            /* [in] */ IInterface* lo,
            /* [in] */ Boolean loInclusive,
            /* [in] */ Boolean toEnd,
            /* [in] */ IInterface* hi,
            /* [in] */ Boolean hiInclusive,
            /* [in] */ Boolean holdRef);

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

        AutoPtr<TreeMapEntry> SubLowest() override;

        AutoPtr<TreeMapEntry> SubHighest() override;

        AutoPtr<TreeMapEntry> SubCeiling(
            /* [in] */ IInterface* key) override;

        AutoPtr<TreeMapEntry> SubHigher(
            /* [in] */ IInterface* key) override;

        AutoPtr<TreeMapEntry> SubFloor(
            /* [in] */ IInterface* key) override;

        AutoPtr<TreeMapEntry> SubLower(
            /* [in] */ IInterface* key) override;
    };

    class DescendingSubMap
        : public NavigableSubMap
    {
    public:
        class DescendingEntrySetView
            : public EntrySetView
        {
        public:
            DescendingEntrySetView(
                /* [in] */ NavigableSubMap* owner)
                : EntrySetView(owner)
            {}

            ECode GetIterator(
                /* [out] */ IIterator** it) override;
        };

    public:
        ECode Constructor(
            /* [in] */ TreeMap* m,
            /* [in] */ Boolean fromStart,
            /* [in] */ IInterface* lo,
            /* [in] */ Boolean loInclusive,
            /* [in] */ Boolean toEnd,
            /* [in] */ IInterface* hi,
            /* [in] */ Boolean hiInclusive,
            /* [in] */ Boolean holdRef);

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

        AutoPtr<TreeMapEntry> SubLowest() override;

        AutoPtr<TreeMapEntry> SubHighest() override;

        AutoPtr<TreeMapEntry> SubCeiling(
            /* [in] */ IInterface* key) override;

        AutoPtr<TreeMapEntry> SubHigher(
            /* [in] */ IInterface* key) override;

        AutoPtr<TreeMapEntry> SubFloor(
            /* [in] */ IInterface* key) override;

        AutoPtr<TreeMapEntry> SubLower(
            /* [in] */ IInterface* key) override;

    private:
        AutoPtr<IComparator> mReverseComparator;
    };

    class SubMapInternal
        : public AbstractMap
        , public ISortedMap
        , public ISerializable
    {
    public:
        COMO_INTERFACE_DECL();

        ECode GetEntrySet(
            /* [out] */ ISet** entries) override;

        ECode LastKey(
            /* [out] */ IInterface** key) override;

        ECode FirstKey(
            /* [out] */ IInterface** key) override;

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

        ECode Comparator(
            /* [out] */ IComparator** comparator) override;
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

        COMO_INTERFACE_DECL();

        ECode GetKey(
            /* [out] */ IInterface** key) override;

        ECode GetValue(
            /* [out] */ IInterface** value) override;

        ECode SetValue(
            /* [in] */ IInterface* value,
            /* [out] */ IInterface** prevValue = nullptr) override;

        ECode Equals(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean& result) override;

        ECode GetHashCode(
            /* [out] */ Integer& hash) override;

        ECode ToString(
            /* [out] */ String& str) override;

    public:
        AutoPtr<IInterface> mKey;
        AutoPtr<IInterface> mValue;
        AutoPtr<TreeMapEntry> mLeft;
        AutoPtr<TreeMapEntry> mRight;
        TreeMapEntry* mParent;
        Boolean mColor = BLACK;
    };

public:
    COMO_INTERFACE_DECL();

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
        /* [in] */ IInterface* defaultVal);

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

    Integer Compare(
        /* [in] */ IInterface* k1,
        /* [in] */ IInterface* k2);

    static Boolean ValEquals(
        /* [in] */ IInterface* o1,
        /* [in] */ IInterface* o2);

    static AutoPtr<IMapEntry> ExportEntry(
        /* [in] */ TreeMapEntry* e);

    AutoPtr<TreeMapEntry> GetFirstEntry();

    AutoPtr<TreeMapEntry> GetLastEntry();

    static AutoPtr<TreeMapEntry> Successor(
        /* [in] */ TreeMapEntry* t);

    static AutoPtr<TreeMapEntry> Predecessor(
        /* [in] */ TreeMapEntry* t);

    static Boolean ColorOf(
        /* [in] */ TreeMapEntry* p);

    static AutoPtr<TreeMapEntry> ParentOf(
        /* [in] */ TreeMapEntry* p);

    static void SetColor(
        /* [in] */ TreeMapEntry* p,
        /* [in] */ Boolean c);

    static AutoPtr<TreeMapEntry> LeftOf(
        /* [in] */ TreeMapEntry* p);

    static AutoPtr<TreeMapEntry> RightOf(
        /* [in] */ TreeMapEntry* p);

    void RotateLeft(
        /* [in] */ TreeMapEntry* p);

    void RotateRight(
        /* [in] */ TreeMapEntry* p);

    void FixAfterInsertion(
        /* [in] */ AutoPtr<TreeMapEntry> x);

    void DeleteEntry(
        /* [in] */ AutoPtr<TreeMapEntry> p);

    void FixAfterDeletion(
        /* [in] */ AutoPtr<TreeMapEntry> x);

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

    static AutoPtr<IInterface> GetUNBOUNDED();

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

#endif // __COMO_UTIL_TREEMAP_H__
