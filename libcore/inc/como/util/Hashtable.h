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

#ifndef __COMO_UTIL_HASHTABLE_H__
#define __COMO_UTIL_HASHTABLE_H__

#include "como/core/volatile.h"
#include "como/core/SyncObject.h"
#include "como/util/AbstractSet.h"
#include "como.core.ICloneable.h"
#include "como.core.IInteger.h"
#include "como.io.ISerializable.h"
#include "como.util.ICollection.h"
#include "como.util.IDictionary.h"
#include "como.util.IEnumeration.h"
#include "como.util.IHashtable.h"
#include "como.util.IMap.h"
#include "como.util.IMapEntry.h"
#include "como.util.IIterator.h"
#include <comosp.h>

using como::core::ICloneable;
using como::core::IInteger;
using como::core::SyncObject;
using como::io::ISerializable;

namespace como {
namespace util {

class Hashtable
    : public SyncObject
    , public IHashtable
    , public IDictionary
    , public IMap
    , public ICloneable
    , public ISerializable
{
private:
    class KeySet
        : public AbstractSet
    {
    public:
        KeySet(
            /* [in] */ Hashtable* owner)
            : mOwner(owner)
        {}

        ECode GetIterator(
            /* [out] */ AutoPtr<IIterator>& it) override;

        ECode GetSize(
            /* [out] */ Integer* size) override;

        ECode Contains(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* result) override;

        ECode Remove(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* contained = nullptr) override;

        ECode Clear() override;

    private:
        Hashtable* mOwner;
    };

    class EntrySet
        : public AbstractSet
    {
    public:
        EntrySet(
            /* [in] */ Hashtable* owner)
            : mOwner(owner)
        {}

        ECode GetIterator(
            /* [out] */ AutoPtr<IIterator>& it) override;

        ECode Add(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* changed = nullptr) override;

        ECode Contains(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* result) override;

        ECode Remove(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* changed = nullptr) override;

        ECode GetSize(
            /* [out] */ Integer* size) override;

        ECode Clear() override;

    private:
        Hashtable* mOwner;
    };

    class ValueCollection
        : public AbstractCollection
    {
    public:
        ValueCollection(
            /* [in] */ Hashtable* owner)
            : mOwner(owner)
        {}

        ECode GetIterator(
            /* [out] */ AutoPtr<IIterator>& it) override;

        ECode GetSize(
            /* [out] */ Integer* size) override;

        ECode Contains(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* result) override;

        ECode Clear() override;

    private:
        Hashtable* mOwner;
    };

    class HashtableEntry
        : public LightRefBase
        , public IMapEntry
    {
    public:
        HashtableEntry(
            /* [in] */ Integer hash,
            /* [in] */ IInterface* key,
            /* [in] */ IInterface* value,
            /* [in] */ HashtableEntry* next)
            : mHash(hash)
            , mKey(key)
            , mValue(value)
            , mNext(next)
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

        String ToString();

    protected:
        AutoPtr<HashtableEntry> Clone();

    public:
        Integer mHash;
        AutoPtr<IInterface> mKey;
        AutoPtr<IInterface> mValue;
        AutoPtr<HashtableEntry> mNext;

        friend class Hashtable;
    };

    class Enumerator
        : public LightRefBase
        , public IEnumeration
        , public IIterator
    {
    public:
        Enumerator(
            /* [in] */ Hashtable* owner,
            /* [in] */ Integer type,
            /* [in] */ Boolean iterator)
            : mOwner(owner)
            , mTable(owner->mTable)
            , mIndex(mTable.GetLength())
            , mType(type)
            , mIterator(iterator)
            , mExpectedModCount(mOwner->mModCount)
        {}

        COMO_INTERFACE_DECL();

        ECode HasMoreElements(
            /* [out] */ Boolean* result) override;

        ECode NextElement(
            /* [out] */ IInterface** object = nullptr) override;

        ECode HasNext(
            /* [out] */ Boolean* result) override;

        ECode Next(
            /* [out] */ IInterface** object = nullptr) override;

        ECode Remove() override;

    public:
        Hashtable* mOwner;
        Array<HashtableEntry*> mTable;
        Integer mIndex;
        AutoPtr<HashtableEntry> mEntry;
        AutoPtr<HashtableEntry> mLastReturned;
        Integer mType;

        /**
         * Indicates whether this Enumerator is serving as an Iterator
         * or an Enumeration.  (true -> Iterator).
         */
        Boolean mIterator;

        /**
         * The modCount value that the iterator believes that the backing
         * Hashtable should have.  If this expectation is violated, the iterator
         * has detected concurrent modification.
         */
        Integer mExpectedModCount;
    };

public:
    COMO_INTERFACE_DECL();

    ECode Constructor();

    ECode Constructor(
        /* [in] */ Integer initialCapacity);

    ECode Constructor(
        /* [in] */ Integer initialCapacity,
        /* [in] */ Float loadFactor);

    ECode Constructor(
        /* [in] */ IMap* t);

    ECode GetSize(
        /* [out] */ Integer* size) override;

    ECode IsEmpty(
        /* [out] */ Boolean* result) override;

    ECode GetKeys(
        /* [out] */ IEnumeration** keys) override;

    ECode GetElements(
        /* [out] */ IEnumeration** elements) override;

    ECode Contains(
        /* [in] */ IInterface* value,
        /* [out] */ Boolean* result) override;

    ECode ContainsValue(
        /* [in] */ IInterface* value,
        /* [out] */ Boolean* result) override;

    ECode ContainsKey(
        /* [in] */ IInterface* key,
        /* [out] */ Boolean* result) override;

    ECode Get(
        /* [in] */ IInterface* key,
        /* [out] */ IInterface** value) override;

    ECode Put(
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* value,
        /* [out] */ IInterface** prevValue = nullptr) override;

    ECode Remove(
        /* [in] */ IInterface* key,
        /* [out] */ IInterface** prevValue = nullptr) override;

    ECode PutAll(
        /* [in] */ IMap* m) override;

    ECode Clear() override;

    ECode ToString(
        /* [out] */ String& str) override;

    ECode GetKeySet(
        /* [out] */ ISet** keys) override;

    ECode GetEntrySet(
        /* [out] */ ISet** entries) override;

    ECode GetValues(
        /* [out] */ ICollection** values) override;

    ECode Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean& result) override;

    ECode GetHashCode(
        /* [out] */ Integer& hash) override;

    ECode PutIfAbsent(
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* value,
        /* [out] */ IInterface** prevValue = nullptr) override;

protected:
    void Rehash();

    ECode CloneImpl(
        /* [out] */ IHashtable* newObj);

private:
    void AddEntry(
        /* [in] */ Integer hash,
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* value,
        /* [in] */ Integer index);

private:
    /**
     * The hash table data.
     */
    Array<HashtableEntry*> mTable;

    /**
     * The total number of entries in the hash table.
     */
    Integer mCount = 0;

    /**
     * The table is rehashed when its size exceeds this threshold.  (The
     * value of this field is (int)(capacity * loadFactor).)
     */
    Integer mThreshold = 0;

    /**
     * The load factor for the hashtable.
     */
    Float mLoadFactor = 0;

    /**
     * The number of times this Hashtable has been structurally modified
     * Structural modifications are those that change the number of entries in
     * the Hashtable or otherwise modify its internal structure (e.g.,
     * rehash).  This field is used to make iterators on Collection-views of
     * the Hashtable fail-fast.  (See ConcurrentModificationException).
     */
    Integer mModCount = 0;

    static constexpr Integer MAX_ARRAY_SIZE = IInteger::MAX_VALUE - 8;

    VOLATILE AutoPtr<ISet> mKeySet;
    VOLATILE AutoPtr<ISet> mEntrySet;
    VOLATILE AutoPtr<ICollection> mValues;

    static constexpr Integer KEYS = 0;
    static constexpr Integer VALUES = 1;
    static constexpr Integer ENTRIES = 2;
};

}
}

#endif // __COMO_UTIL_HASHTABLE_H__
