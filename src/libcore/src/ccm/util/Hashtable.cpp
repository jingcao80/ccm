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

#include "ccm/core/AutoLock.h"
#include "ccm/core/CStringBuilder.h"
#include "ccm/core/Math.h"
#include "ccm/util/Collections.h"
#include "ccm/util/Hashtable.h"
#include "ccm.core.IStringBuilder.h"
#include "ccm.util.ISet.h"

using ccm::core::AutoLock;
using ccm::core::CStringBuilder;
using ccm::core::IStringBuilder;
using ccm::core::IID_IStringBuilder;
using ccm::core::Math;

namespace ccm {
namespace util {

ECode Hashtable::GetSize(
    /* [out] */ Integer* size)
{
    VALIDATE_NOT_NULL(size);

    AutoLock lock(this);
    *size = mCount;
    return NOERROR;
}

ECode Hashtable::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoLock lock(this);
    *result = mCount == 0;
    return NOERROR;
}

ECode Hashtable::GetKeys(
    /* [out] */ IEnumeration** keys)
{
    VALIDATE_NOT_NULL(keys);

    AutoLock lock(this);
    if (mCount == 0) {
        *keys = Collections::GetEmptyEnumeration();
    }
    else {
        *keys = new Enumerator(KEYS, false);
    }
    REFCOUNT_ADD(*keys);
    return NOERROR;
}

ECode Hashtable::GetElements(
    /* [out] */ IEnumeration** elements)
{
    VALIDATE_NOT_NULL(elements);

    AutoLock lock(this);
    if (mCount ==  0) {
        *elements = Collections::GetEmptyEnumeration();
    }
    else {
        *elements = new Enumerator(VALUES, false);
    }
    REFCOUNT_ADD(*elements);
    return NOERROR;
}

ECode Hashtable::Contains(
    /* [in] */ IInterface* value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (value == nullptr) {
        return ccm::core::E_NULL_POINTER_EXCEPTION;
    }

    AutoLock lock(this);
    for (Integer i = mTable.GetLength() - 1; i >= 0; i--) {
        for (HashtableEntry* e = mTable[i]; e != nullptr; e = e->mNext) {
            if (Object::Equals(e->mValue, value)) {
                *result = true;
                return NOERROR;
            }
        }
    }
    *result = false;
    return NOERROR;
}

ECode Hashtable::ContainsValue(
    /* [in] */ IInterface* value,
    /* [out] */ Boolean* result)
{
    return Contains(value, result);
}

ECode Hashtable::ContainsKey(
    /* [in] */ IInterface* key,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoLock lock(this);
    Integer hash = Object::GetHashCode(key);
    Integer index = (hash & 0x7FFFFFFF) % mTable.GetLength();
    for (HashtableEntry* e = mTable[index]; e != nullptr; e = e->mNext) {
        if ((e->mHash == hash) && Object::Equals(e->mKey, key)) {
            *result = true;
            return NOERROR;
        }
    }
    *result = false;
    return NOERROR;
}

ECode Hashtable::Get(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value);

    AutoLock lock(this);
    Integer hash = Object::GetHashCode(key);
    Integer index = (hash & 0x7FFFFFFF) % mTable.GetLength();
    for (HashtableEntry* e = mTable[index]; e != nullptr; e = e->mNext) {
        if ((e->mHash == hash) && Object::Equals(e->mKey, key)) {
            *value = e->mValue;
            REFCOUNT_ADD(*value);
            return NOERROR;
        }
    }
    *value = nullptr;
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
    mThreshold = (Integer)Math::Min(newCapacity * mLoadFactor, MAX_ARRAY_SIZE + 1);
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
        return ccm::core::E_NULL_POINTER_EXCEPTION;
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
    HashtableEntry* e = mTable[index];
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
            e->mValue = nullptr;
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
    m->GetEntrySet((ISet**)&entries);
    AutoPtr<IIterator> it;
    entries->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IInterface> key, value;
        IMapEntry::Probe(obj)->GetKey((IInterface**)&key);
        IMapEntry::Probe(obj)->GetValue((IInterface**)&value);
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
}

ECode Hashtable::Clone(
    /* [out] */ IInterface** obj)
{
    AutoLock lock(this);
}

ECode Hashtable::ToString(
    /* [out] */ String* str)
{
    AutoLock lock(this);

    Integer max;
    GetSize(&max);
    max = max - 1;
    if (max == -1) {
        *str = "{}";
        return NOERROR;
    }

    AutoPtr<IStringBuilder> sb;
    CStringBuilder::New(IID_IStringBuilder, (IInterface**)&sb);
    AutoPtr<ISet> entries;
    GetEntrySet((ISet**)&entries);
    AutoPtr<IIterator> it;
    entries->GetIterator((IIterator**)&it);

    sb->AppendChar('{');
    for (Integer i = 0; ; i++) {
        AutoPtr<IInterface> e;
        it->GetNext((IInterface**)&e);
        AutoPtr<IInterface> key, value;
        IMapEntry::Probe(e)->GetKey((IInterface**)&key);
        IMapEntry::Probe(e)->GetValue((IInterface**)&value);
        sb->Append(IInterface::Equals(key, (IHashtable*)this) ?
                String("(this Map)") : Object::ToString(key));
        sb->Append(Object::Equals(value, (IHashtable*)this) ?
                String("(this Map") : Object::ToString(value));

        if (i == max) {
            sb->AppendChar('}');
            return sb->ToString(str);
        }
        sb->Append(String(", "));
    }
}

ECode Hashtable::GetKeySet(
    /* [out] */ ISet** keys)
{
    VALIDATE_NOT_NULL(keys);

    if (mKeySet == nullptr) {
        mKeySet = Collections::CreateSynchronizedSet(new KeySet(), this);
    }
    *keys = mKeySet;
    REFCOUNT_ADD(*keys);
    return NOERROR;
}

ECode Hashtable::GetEntrySet(
    /* [out] */ ISet** entries)
{
    VALIDATE_NOT_NULL(entries);

    if (mEntrySet == nullptr) {
        mEntrySet = Collections::CreateSynchronizedSet(new EntrySet(), this);
    }
    *entries = mEntrySet;
    REFCOUNT_ADD(*entries);
    return NOERROR;
}

ECode Hashtable::GetValues(
    /* [out] */ ICollection** values)
{
    VALIDATE_NOT_NULL(values);

    if (mValues == nullptr) {
        mValues = Collections::CreateSynchronizedCollection(
                new ValueCollection(), this);
    }
    *values = mValues;
    REFCOUNT_ADD(*values);
    return NOERROR;
}


//-------------------------------------------------------------------------

CCM_INTERFACE_IMPL_LIGHT_2(Hashtable::Enumerator, IEnumeration, IIterator);

ECode Hashtable::Enumerator::HasMoreElements(
    /* [out] */ Boolean* result)
{
    return NOERROR;
}

ECode Hashtable::Enumerator::GetNextElement(
    /* [out] */ IInterface** object)
{
    return NOERROR;
}

ECode Hashtable::Enumerator::GetNext(
    /* [out] */ IInterface** object)
{
    return NOERROR;
}

ECode Hashtable::Enumerator::HasNext(
    /* [out] */ Boolean* result)
{
    return NOERROR;
}

ECode Hashtable::Enumerator::Remove()
{
    return NOERROR;
}

}
}
