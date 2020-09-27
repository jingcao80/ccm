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

#include "como/core/AutoLock.h"
#include "como/core/CStringBuilder.h"
#include "como/core/Math.h"
#include "como/util/Collections.h"
#include "como/util/Hashtable.h"
#include "como.core.IStringBuilder.h"
#include "como.util.ISet.h"
#include <comolog.h>

using como::core::AutoLock;
using como::core::CStringBuilder;
using como::core::IStringBuilder;
using como::core::IID_ICloneable;
using como::core::IID_IStringBuilder;
using como::core::Math;
using como::io::IID_ISerializable;

namespace como {
namespace util {

COMO_INTERFACE_IMPL_5(Hashtable, SyncObject, IHashtable, IDictionary, IMap, ICloneable, ISerializable);

ECode Hashtable::Constructor()
{
    return Constructor(11, 0.75);
}

ECode Hashtable::Constructor(
    /* [in] */ Integer initialCapacity)
{
    return Constructor(initialCapacity, 0.75);
}

ECode Hashtable::Constructor(
    /* [in] */ Integer initialCapacity,
    /* [in] */ Float loadFactor)
{
    if (initialCapacity < 0) {
        Logger::E("Hashtable", "Illegal Capacity: %d", initialCapacity);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (loadFactor <= 0 || Math::IsNaN(loadFactor)) {
        Logger::E("Hashtable", "Illegal Load: %f", loadFactor);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (initialCapacity == 0) {
        initialCapacity = 1;
    }
    mLoadFactor = loadFactor;
    mTable = Array<HashtableEntry*>(initialCapacity);
    // changed: Ignore loadFactor when calculating threshold from initialCapacity
    // threshold = (int)Math.min(initialCapacity * loadFactor, MAX_ARRAY_SIZE + 1);
    mThreshold = (Integer)Math::Min(initialCapacity, MAX_ARRAY_SIZE + 1);
    return NOERROR;
}

ECode Hashtable::Constructor(
    /* [in] */ IMap* t)
{
    Integer size;
    t->GetSize(size);
    FAIL_RETURN(Constructor(Math::Max(2 * size, 11), 0.75));
    return PutAll(t);
}

ECode Hashtable::GetSize(
    /* [out] */ Integer& size)
{
    AutoLock lock(this);
    size = mCount;
    return NOERROR;
}

ECode Hashtable::IsEmpty(
    /* [out] */ Boolean& result)
{
    AutoLock lock(this);
    result = mCount == 0;
    return NOERROR;
}

ECode Hashtable::GetKeys(
    /* [out] */ AutoPtr<IEnumeration>& keys)
{
    AutoLock lock(this);
    if (mCount == 0) {
        keys = Collections::GetEmptyEnumeration();
    }
    else {
        keys = new Enumerator(this, KEYS, false);
    }
    return NOERROR;
}

ECode Hashtable::GetElements(
    /* [out] */ AutoPtr<IEnumeration>& elements)
{
    AutoLock lock(this);
    if (mCount ==  0) {
        elements = Collections::GetEmptyEnumeration();
    }
    else {
        elements = new Enumerator(this, VALUES, false);
    }
    return NOERROR;
}

ECode Hashtable::Contains(
    /* [in] */ IInterface* value,
    /* [out] */ Boolean& result)
{
    if (value == nullptr) {
        return como::core::E_NULL_POINTER_EXCEPTION;
    }

    AutoLock lock(this);
    for (Integer i = mTable.GetLength() - 1; i >= 0; i--) {
        for (HashtableEntry* e = mTable[i]; e != nullptr; e = e->mNext) {
            if (Object::Equals(e->mValue, value)) {
                result = true;
                return NOERROR;
            }
        }
    }
    result = false;
    return NOERROR;
}

ECode Hashtable::ContainsValue(
    /* [in] */ IInterface* value,
    /* [out] */ Boolean& result)
{
    return Contains(value, result);
}

ECode Hashtable::ContainsKey(
    /* [in] */ IInterface* key,
    /* [out] */ Boolean& result)
{
    AutoLock lock(this);
    Integer hash = Object::GetHashCode(key);
    Integer index = (hash & 0x7FFFFFFF) % mTable.GetLength();
    for (HashtableEntry* e = mTable[index]; e != nullptr; e = e->mNext) {
        if ((e->mHash == hash) && Object::Equals(e->mKey, key)) {
            result = true;
            return NOERROR;
        }
    }
    result = false;
    return NOERROR;
}

ECode Hashtable::Get(
    /* [in] */ IInterface* key,
    /* [out] */ AutoPtr<IInterface>& value)
{
    AutoLock lock(this);
    Integer hash = Object::GetHashCode(key);
    Integer index = (hash & 0x7FFFFFFF) % mTable.GetLength();
    for (HashtableEntry* e = mTable[index]; e != nullptr; e = e->mNext) {
        if ((e->mHash == hash) && Object::Equals(e->mKey, key)) {
            value = e->mValue;
            return NOERROR;
        }
    }
    value = nullptr;
    return NOERROR;
}

void Hashtable::Rehash()
{
    Integer oldCapacity = mTable.GetLength();
    Array<HashtableEntry*> oldMap = mTable;

    // overflow-conscious code
    Integer newCapacity = (oldCapacity << 1) + 1;
    if (newCapacity - MAX_ARRAY_SIZE > 0) {
        if (oldCapacity == MAX_ARRAY_SIZE) {
            // Keep running with MAX_ARRAY_SIZE buckets
            return;
        }
        newCapacity = MAX_ARRAY_SIZE;
    }
    Array<HashtableEntry*> newMap = Array<HashtableEntry*>(newCapacity);

    mModCount++;
    mThreshold = (Integer)Math::Min(newCapacity * mLoadFactor, (Float)MAX_ARRAY_SIZE + 1);
    mTable = newMap;

    for (Integer i = oldCapacity - 1; i > 0; i--) {
        for (HashtableEntry* old = oldMap[i]; old != nullptr;) {
            HashtableEntry* e = old;
            old = old->mNext;

            Integer index = (e->mHash & 0x7FFFFFFF) % newCapacity;
            e->mNext = newMap[index];
            newMap.Set(index, e);
        }
    }
}

void Hashtable::AddEntry(
    /* [in] */ Integer hash,
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value,
    /* [in] */ Integer index)
{
    mModCount++;

    if (mCount >= mThreshold) {
        // Rehash the table if the threshold is exceeded
        Rehash();

        hash = Object::GetHashCode(key);
        index = (hash & 0x7FFFFFFF) % mTable.GetLength();
    }

    HashtableEntry* e = mTable[index];
    AutoPtr<HashtableEntry> entry = new HashtableEntry(hash, key, value, e);
    mTable.Set(index, entry);
    mCount++;
}

ECode Hashtable::Put(
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value,
    /* [out] */ IInterface** prevValue)
{
    if (value == nullptr) {
        return como::core::E_NULL_POINTER_EXCEPTION;
    }

    AutoLock lock(this);
    // Makes sure the key is not already in the hashtable.
    Integer hash = Object::GetHashCode(key);
    Integer index = (hash & 0x7FFFFFFF) % mTable.GetLength();
    HashtableEntry* entry = mTable[index];
    for (; entry != nullptr; entry = entry->mNext) {
        if ((entry->mHash == hash) && Object::Equals(entry->mKey, key)) {
            if (prevValue != nullptr) {
                *prevValue = entry->mValue;
                REFCOUNT_ADD(*prevValue);
            }
            entry->mValue = value;
            return NOERROR;
        }
    }

    AddEntry(hash, key, value, index);
    if (prevValue != nullptr) *prevValue = nullptr;
    return NOERROR;
}

ECode Hashtable::Remove(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** prevValue)
{
    AutoLock lock(this);
    Integer hash = Object::GetHashCode(key);
    Integer index = (hash & 0x7FFFFFFF) % mTable.GetLength();
    AutoPtr<HashtableEntry> e = mTable[index];
    for (HashtableEntry* prev = nullptr; e != nullptr; prev = e, e = e->mNext) {
        if ((e->mHash == hash) && Object::Equals(e->mKey, key)) {
            mModCount++;
            if (prev != nullptr) {
                prev->mNext = e->mNext;
            }
            else {
                mTable.Set(index, e->mNext);
            }
            mCount--;
            if (prevValue != nullptr) {
                *prevValue = e->mValue;
                REFCOUNT_ADD(*prevValue);
            }
            e = nullptr;
            return NOERROR;
        }
    }
    if (prevValue != nullptr) *prevValue = nullptr;
    return NOERROR;
}

ECode Hashtable::PutAll(
    /* [in] */ IMap* m)
{
    AutoLock lock(this);
    AutoPtr<ISet> entries;
    m->GetEntrySet(entries);
    AutoPtr<IIterator> it;
    entries->GetIterator(it);
    Boolean hasNext;
    while (it->HasNext(hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->Next(&obj);
        AutoPtr<IInterface> key, value;
        IMapEntry::Probe(obj)->GetKey(key);
        IMapEntry::Probe(obj)->GetValue(value);
        FAIL_RETURN(Put(key, value));
    }
    return NOERROR;
}

ECode Hashtable::Clear()
{
    AutoLock lock(this);
    mModCount++;
    for (Integer index = mTable.GetLength() - 1; index >= 0; index--) {
        mTable.Set(index, nullptr);
    }
    mCount = 0;
    return NOERROR;
}

ECode Hashtable::CloneImpl(
    /* [out] */ IHashtable* newObj)
{
    AutoLock lock(this);
    Hashtable* t = (Hashtable*)newObj;

    t->mCount = mCount;
    t->mThreshold = mThreshold;
    t->mLoadFactor = mLoadFactor;

    t->mTable = Array<HashtableEntry*>(mTable.GetLength());
    for (Integer i = mTable.GetLength() - 1; i >=0; i--) {
        if (mTable[i] != nullptr) {
            t->mTable.Set(i, mTable[i]->Clone());
        }
    }
    return NOERROR;
}

ECode Hashtable::ToString(
    /* [out] */ String& str)
{
    AutoLock lock(this);

    Integer max;
    GetSize(max);
    max = max - 1;
    if (max == -1) {
        str = "{}";
        return NOERROR;
    }

    AutoPtr<IStringBuilder> sb;
    CStringBuilder::New(IID_IStringBuilder, (IInterface**)&sb);
    AutoPtr<ISet> entries;
    GetEntrySet(entries);
    AutoPtr<IIterator> it;
    entries->GetIterator(it);

    sb->Append(U'{');
    for (Integer i = 0; ; i++) {
        AutoPtr<IInterface> e;
        it->Next(&e);
        AutoPtr<IInterface> key, value;
        IMapEntry::Probe(e)->GetKey(key);
        IMapEntry::Probe(e)->GetValue(value);
        sb->Append(IInterface::Equals(key, (IHashtable*)this) ?
                String("(this Map)") : Object::ToString(key));
        sb->Append(Object::Equals(value, (IHashtable*)this) ?
                String("(this Map") : Object::ToString(value));

        if (i == max) {
            sb->Append(U'}');
            return sb->ToString(str);
        }
        sb->Append(String(", "));
    }
}

ECode Hashtable::GetKeySet(
    /* [out] */ AutoPtr<ISet>& keys)
{
    VOLATILE_GET(AutoPtr<ISet> keySet, mKeySet);
    if (keySet == nullptr) {
        keySet = Collections::CreateSynchronizedSet(new KeySet(this), this);
        VOLATILE_SET(mKeySet, keySet);
    }
    keys = std::move(keySet);
    return NOERROR;
}

ECode Hashtable::GetEntrySet(
    /* [out] */ AutoPtr<ISet>& entries)
{
    VOLATILE_GET(AutoPtr<ISet> entrySet, mEntrySet);
    if (entrySet == nullptr) {
        entrySet = Collections::CreateSynchronizedSet(new EntrySet(this), this);
        VOLATILE_SET(mEntrySet, entrySet);
    }
    entries = std::move(entrySet);
    return NOERROR;
}

ECode Hashtable::GetValues(
    /* [out] */ AutoPtr<ICollection>& values)
{
    VOLATILE_GET(AutoPtr<ICollection> valueColl, mValues);
    if (valueColl == nullptr) {
        valueColl = Collections::CreateSynchronizedCollection(
                new ValueCollection(this), this);
        VOLATILE_SET(mValues, valueColl);
    }
    values = std::move(valueColl);
    return NOERROR;
}

ECode Hashtable::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean& result)
{
    AutoLock lock(this);
    if (IInterface::Equals(obj, (IHashtable*)this)) {
        result = true;
        return NOERROR;
    }

    if (IMap::Probe(obj) == nullptr) {
        result = false;
        return NOERROR;
    }
    IMap* t = IMap::Probe(obj);
    Integer othSize, thisSize;
    if (t->GetSize(othSize), GetSize(thisSize), othSize != thisSize) {
        result = false;
        return NOERROR;
    }

    AutoPtr<ISet> entries;
    GetEntrySet(entries);
    AutoPtr<IIterator> it;
    entries->GetIterator(it);
    Boolean hasNext;
    while (it->HasNext(hasNext), hasNext) {
        AutoPtr<IInterface> o;
        it->Next(&o);
        IMapEntry* e = IMapEntry::Probe(o);
        AutoPtr<IInterface> key, value;
        e->GetKey(key);
        e->GetValue(value);
        if (value == nullptr) {
            AutoPtr<IInterface> v1;
            Boolean contains;
            if ((t->Get(key, v1), v1 != nullptr) ||
                (t->ContainsKey(key, contains), !contains)) {
                result = false;
                return NOERROR;
            }
        }
        else {
            AutoPtr<IInterface> v1;
            t->Get(key, v1);
            if (!Object::Equals(value, v1)) {
                result = false;
                return NOERROR;
            }
        }
    }

    result = true;
    return NOERROR;
}

ECode Hashtable::GetHashCode(
    /* [out] */ Integer& hash)
{
    /*
     * This code detects the recursion caused by computing the hash code
     * of a self-referential hash table and prevents the stack overflow
     * that would otherwise result.  This allows certain 1.1-era
     * applets with self-referential hash tables to work.  This code
     * abuses the loadFactor field to do double-duty as a hashCode
     * in progress flag, so as not to worsen the space performance.
     * A negative load factor indicates that hash code computation is
     * in progress.
     */
    AutoLock lock(this);
    hash = 0;
    if (mCount == 0 || mLoadFactor < 0) {
        return NOERROR;
    }

    mLoadFactor = -mLoadFactor; // Mark hashCode computation in progress
    for (Integer i = 0; i < mTable.GetLength(); i++) {
        HashtableEntry* entry = mTable[i];
        while (entry != nullptr) {
            Integer eh;
            entry->GetHashCode(eh);
            hash += eh;
            entry = entry->mNext;
        }
    }

    mLoadFactor = -mLoadFactor; // Mark hashCode computation complete

    return NOERROR;
}

ECode Hashtable::PutIfAbsent(
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value,
    /* [out] */ IInterface** prevValue)
{
    if (value == nullptr) {
        return como::core::E_NULL_POINTER_EXCEPTION;
    }

    AutoLock lock(this);
    // Makes sure the key is not already in the hashtable.
    Integer hash = Object::GetHashCode(key);
    Integer index = (hash & 0x7FFFFFFF) % mTable.GetLength();
    HashtableEntry* entry = mTable[index];
    for (; entry != nullptr; entry = entry->mNext) {
        if ((entry->mHash == hash) && Object::Equals(entry->mKey, key)) {
            IInterface* old = entry->mValue;
            if (old == nullptr) {
                entry->mValue = value;
            }
            if (prevValue != nullptr) {
                *prevValue = old;
                REFCOUNT_ADD(*prevValue);
            }
            return NOERROR;
        }
    }

    AddEntry(hash, key, value, index);
    if (prevValue != nullptr) *prevValue = nullptr;
    return NOERROR;
}

//-------------------------------------------------------------------------

ECode Hashtable::KeySet::GetIterator(
    /* [out] */ AutoPtr<IIterator>& it)
{
    if (mOwner->mCount == 0) {
        it = Collections::GetEmptyIterator();
    }
    else {
        it = new Enumerator(mOwner, KEYS, true);
    }
    return NOERROR;
}

ECode Hashtable::KeySet::GetSize(
    /* [out] */ Integer& size)
{
    size = mOwner->mCount;
    return NOERROR;
}

ECode Hashtable::KeySet::Contains(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean& result)
{
    return mOwner->ContainsKey(obj, result);
}

ECode Hashtable::KeySet::Remove(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* contained)
{
    VALIDATE_NOT_NULL(contained);

    AutoPtr<IInterface> prevValue;
    mOwner->Remove(obj, &prevValue);
    *contained = prevValue != nullptr;
    return NOERROR;
}

ECode Hashtable::KeySet::Clear()
{
    return mOwner->Clear();
}

//-------------------------------------------------------------------------

ECode Hashtable::EntrySet::GetIterator(
    /* [out] */ AutoPtr<IIterator>& it)
{
    if (mOwner->mCount == 0) {
        it = Collections::GetEmptyIterator();
    }
    else {
        it = new Enumerator(mOwner, ENTRIES, true);
    }
    return NOERROR;
}

ECode Hashtable::EntrySet::Add(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* changed)
{
    return AbstractSet::Add(obj, changed);
}

ECode Hashtable::EntrySet::Contains(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean& result)
{
    if (IMapEntry::Probe(obj) == nullptr) {
        result = false;
        return NOERROR;
    }

    IMapEntry* entry = IMapEntry::Probe(obj);
    AutoPtr<IInterface> key;
    entry->GetKey(key);
    Integer hash = Object::GetHashCode(key);
    Integer index = (hash & 0x7FFFFFFF) % mOwner->mTable.GetLength();

    for (HashtableEntry* e = mOwner->mTable[index]; e != nullptr; e = e->mNext) {
        if (e->mHash == hash && Object::Equals(e, entry)) {
            result = true;
            return NOERROR;
        }
    }
    result = false;
    return NOERROR;
}

ECode Hashtable::EntrySet::Remove(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* changed)
{
    VALIDATE_NOT_NULL(changed);

    if (IMapEntry::Probe(obj) == nullptr) {
        *changed = false;
        return NOERROR;
    }

    IMapEntry* entry = IMapEntry::Probe(obj);
    AutoPtr<IInterface> key;
    entry->GetKey(key);
    Integer hash = Object::GetHashCode(key);
    Integer index = (hash & 0x7FFFFFFF) % mOwner->mTable.GetLength();

    AutoPtr<HashtableEntry> e = mOwner->mTable[index];
    for (HashtableEntry* prev = nullptr; e != nullptr; prev = e, e = e->mNext) {
        if (e->mHash == hash && Object::Equals(e, entry)) {
            mOwner->mModCount++;
            if (prev != nullptr) {
                prev->mNext = e->mNext;
            }
            else {
                mOwner->mTable.Set(index, e->mNext);
            }

            mOwner->mCount--;
            e = nullptr;
            *changed = true;
            return NOERROR;
        }
    }
    *changed = false;
    return NOERROR;
}

ECode Hashtable::EntrySet::GetSize(
    /* [out] */ Integer& size)
{
    size = mOwner->mCount;
    return NOERROR;
}

ECode Hashtable::EntrySet::Clear()
{
    return mOwner->Clear();
}

//-------------------------------------------------------------------------

ECode Hashtable::ValueCollection::GetIterator(
    /* [out] */ AutoPtr<IIterator>& it)
{
    if (mOwner->mCount == 0) {
        it = Collections::GetEmptyIterator();
    }
    else {
        it = new Enumerator(mOwner, VALUES, true);
    }
    return NOERROR;
}

ECode Hashtable::ValueCollection::GetSize(
    /* [out] */ Integer& size)
{
    size = mOwner->mCount;
    return NOERROR;
}

ECode Hashtable::ValueCollection::Contains(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean& result)
{
    return mOwner->ContainsValue(obj, result);
}

ECode Hashtable::ValueCollection::Clear()
{
    return mOwner->Clear();
}

//-------------------------------------------------------------------------

COMO_INTERFACE_IMPL_LIGHT_1(Hashtable::HashtableEntry, LightRefBase, IMapEntry);

AutoPtr<Hashtable::HashtableEntry> Hashtable::HashtableEntry::Clone()
{
    return new HashtableEntry(mHash, mKey, mValue,
            (mNext == nullptr ? nullptr : mNext->Clone()));
}

ECode Hashtable::HashtableEntry::GetKey(
    /* [out] */ AutoPtr<IInterface>& key)
{
    key = mKey;
    return NOERROR;
}

ECode Hashtable::HashtableEntry::GetValue(
    /* [out] */ AutoPtr<IInterface>& value)
{
    value = mValue;
    return NOERROR;
}

ECode Hashtable::HashtableEntry::SetValue(
    /* [in] */ IInterface* value,
    /* [out] */ IInterface** prevValue)
{
    if (value == nullptr) {
        return como::core::E_NULL_POINTER_EXCEPTION;
    }

    if (prevValue != nullptr) {
        *prevValue = mValue;
        REFCOUNT_ADD(*prevValue);
    }
    mValue = value;
    return NOERROR;
}

ECode Hashtable::HashtableEntry::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean& result)
{
    if (IMapEntry::Probe(obj) == nullptr) {
        result = false;
        return NOERROR;
    }
    IMapEntry* e = IMapEntry::Probe(obj);

    AutoPtr<IInterface> key, value;
    e->GetKey(key);
    e->GetValue(value);
    result = (mKey == nullptr ? key == nullptr : Object::Equals(mKey, key)) &&
            (mValue == nullptr ? value == nullptr : Object::Equals(mValue, value));
    return NOERROR;
}

ECode Hashtable::HashtableEntry::GetHashCode(
    /* [out] */ Integer& hash)
{
    hash = mHash ^ Object::GetHashCode(mValue);
    return NOERROR;
}

String Hashtable::HashtableEntry::ToString()
{
    return Object::ToString(mKey) + "=" + Object::ToString(mValue);
}

//-------------------------------------------------------------------------

COMO_INTERFACE_IMPL_LIGHT_2(Hashtable::Enumerator, LightRefBase, IEnumeration, IIterator);

ECode Hashtable::Enumerator::HasMoreElements(
    /* [out] */ Boolean& result)
{
    AutoPtr<HashtableEntry> e = mEntry;
    Integer i = mIndex;
    while (e == nullptr && i > 0) {
        e = mTable[--i];
    }
    mEntry = e;
    mIndex = i;
    result = e != nullptr;
    return NOERROR;
}

ECode Hashtable::Enumerator::NextElement(
    /* [out] */ IInterface** object)
{
    AutoPtr<HashtableEntry> et = mEntry;
    Integer i = mIndex;
    while (et == nullptr && i > 0) {
        et = mTable[--i];
    }
    mEntry = et;
    mIndex = i;
    if (et != nullptr) {
        HashtableEntry* e = mLastReturned = mEntry;
        mEntry = e->mNext;
        if (object != nullptr) {
            *object = mType == KEYS ? e->mKey : (
                    mType == VALUES ? e->mValue : e);
            REFCOUNT_ADD(*object);
        }
        return NOERROR;
    }
    return E_NO_SUCH_ELEMENT_EXCEPTION;
}

ECode Hashtable::Enumerator::HasNext(
    /* [out] */ Boolean& result)
{
    return HasMoreElements(result);
}

ECode Hashtable::Enumerator::Next(
    /* [out] */ IInterface** object)
{
    if (mOwner->mModCount != mExpectedModCount) {
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }
    return NextElement(object);
}

ECode Hashtable::Enumerator::Remove()
{
    if (!mIterator) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    if (mLastReturned == nullptr) {
        return como::core::E_ILLEGAL_STATE_EXCEPTION;
    }
    if (mOwner->mModCount != mExpectedModCount) {
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }

    AutoLock lock(mOwner);
    Array<HashtableEntry*>& tab = mOwner->mTable;
    Integer index = (mLastReturned->mHash & 0x7FFFFFFF) % tab.GetLength();

    HashtableEntry* e = tab[index];
    for (HashtableEntry* prev = nullptr; e != nullptr; prev = e, e = e->mNext) {
        if (e == mLastReturned) {
            mOwner->mModCount++;
            mExpectedModCount++;
            if (prev == nullptr) {
                tab.Set(index, e->mNext);
            }
            else {
                prev->mNext = e->mNext;
            }
            mOwner->mCount--;
            mLastReturned = nullptr;
            return NOERROR;
        }
    }
    return E_CONCURRENT_MODIFICATION_EXCEPTION;
}

}
}
