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

#include "innerdef.h"
#include "como/util/Collections.h"
#include "como/util/TreeMap.h"
#include "como.core.IComparable.h"
#include "como.io.IObjectInput.h"
#include <comolog.h>

using como::core::E_CLASS_CAST_EXCEPTION;
using como::core::E_INTERNAL_ERROR;
using como::core::E_ILLEGAL_STATE_EXCEPTION;
using como::core::E_NULL_POINTER_EXCEPTION;
using como::core::IComparable;
using como::core::IID_ICloneable;
using como::io::IID_ISerializable;
using como::io::IObjectInput;

namespace como {
namespace util {

AutoPtr<IInterface> TreeMap::GetUNBOUNDED()
{
    static AutoPtr<IInterface> UNBOUNDED = (IObject*)new SyncObject();
    return UNBOUNDED;
}

COMO_INTERFACE_IMPL_5(TreeMap, AbstractMap, ITreeMap, INavigableMap, ISortedMap, ICloneable, ISerializable);

ECode TreeMap::Constructor()
{
    return NOERROR;
}

ECode TreeMap::Constructor(
    /* [in] */ IComparator* comparator)
{
    mComparator = comparator;
    return NOERROR;
}

ECode TreeMap::Constructor(
    /* [in] */ IMap* m)
{
    return PutAll(m);
}

ECode TreeMap::Constructor(
    /* [in] */ ISortedMap* m)
{
    m->Comparator(&mComparator);
    Integer size;
    IMap::Probe(m)->GetSize(size);
    AutoPtr<ISet> entrySet;
    IMap::Probe(m)->GetEntrySet(entrySet);
    AutoPtr<IIterator> it;
    entrySet->GetIterator(it);
    BuildFromSorted(size, it, nullptr, nullptr);
    return NOERROR;
}

ECode TreeMap::GetSize(
    /* [out] */ Integer& size)
{
    size = mSize;
    return NOERROR;
}

ECode TreeMap::ContainsKey(
    /* [in] */ IInterface* key,
    /* [out] */ Boolean& result)
{
    AutoPtr<TreeMapEntry> entry;
    FAIL_RETURN(GetEntry(key, &entry));
    result = entry != nullptr;
    return NOERROR;
}

ECode TreeMap::ContainsValue(
    /* [in] */ IInterface* value,
    /* [out] */ Boolean& result)
{
    for (AutoPtr<TreeMapEntry> e = GetFirstEntry(); e != nullptr; e = Successor(e)) {
        if (ValEquals(value, e->mValue)) {
            result = true;
            return NOERROR;
        }
    }
    result = false;
    return NOERROR;
}

ECode TreeMap::Get(
    /* [in] */ IInterface* key,
    /* [out] */ AutoPtr<IInterface>& value)
{
    AutoPtr<TreeMapEntry> entry;
    FAIL_RETURN(GetEntry(key, &entry));
    value = entry != nullptr
            ? entry->mValue
            : nullptr;
    return NOERROR;
}

ECode TreeMap::Comparator(
    /* [out] */ IComparator** comparator)
{
    VALIDATE_NOT_NULL(comparator);

    *comparator = mComparator;
    REFCOUNT_ADD(*comparator);
    return NOERROR;
}

ECode TreeMap::FirstKey(
    /* [out] */ IInterface** key)
{
    VALIDATE_NOT_NULL(key);

    AutoPtr<TreeMapEntry> entry = GetFirstEntry();
    if (entry == nullptr) {
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }
    *key = entry->mKey;
    REFCOUNT_ADD(*key);
    return NOERROR;
}

ECode TreeMap::LastKey(
    /* [out] */ IInterface** key)
{
    VALIDATE_NOT_NULL(key);

    AutoPtr<TreeMapEntry> entry = GetLastEntry();
    if (entry == nullptr) {
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }
    *key = entry->mKey;
    REFCOUNT_ADD(*key);
    return NOERROR;
}

ECode TreeMap::PutAll(
    /* [in] */ IMap* map)
{
    Integer mapSize;
    map->GetSize(mapSize);
    if (mSize == 0 && mapSize != 0 && ISortedMap::Probe(map) != nullptr) {
        AutoPtr<IComparator> c;
        ISortedMap::Probe(map)->Comparator(&c);
        if (c == mComparator || (c != nullptr && Object::Equals(c, mComparator))) {
            ++mModCount;
            AutoPtr<ISet> entrySet;
            map->GetEntrySet(entrySet);
            AutoPtr<IIterator> it;
            entrySet->GetIterator(it);
            BuildFromSorted(mapSize, it, nullptr, nullptr);
            return NOERROR;
        }
    }
    return AbstractMap::PutAll(map);
}

ECode TreeMap::GetEntry(
    /* [in] */ IInterface* key,
    /* [out] */ TreeMapEntry** entry)
{
    if (mComparator != nullptr) {
        GetEntryUsingComparator(key).MoveTo(entry);
        return NOERROR;
    }
    if (key == nullptr) {
        return E_NULL_POINTER_EXCEPTION;
    }
    IComparable* k = IComparable::Probe(key);
    TreeMapEntry* p = mRoot;
    while (p != nullptr) {
        Integer cmp;
        k->CompareTo(p->mKey, cmp);
        if (cmp < 0) {
            p = p->mLeft;
        }
        else if (cmp > 0) {
            p = p->mRight;
        }
        else {
            *entry = p;
            REFCOUNT_ADD(*entry);
            return NOERROR;
        }
    }
    *entry = nullptr;
    return NOERROR;
}

AutoPtr<TreeMap::TreeMapEntry> TreeMap::GetEntryUsingComparator(
    /* [in] */ IInterface* key)
{
    IComparator* cpr = mComparator;
    if (cpr != nullptr) {
        TreeMapEntry* p = mRoot;
        while (p != nullptr) {
            Integer cmp;
            cpr->Compare(key, p->mKey, cmp);
            if (cmp < 0) {
                p = p->mLeft;
            }
            else if (cmp > 0) {
                p = p->mRight;
            }
            else {
                return p;
            }
        }
    }
    return nullptr;
}

ECode TreeMap::GetCeilingEntry(
    /* [in] */ IInterface* key,
    /* [out] */ TreeMapEntry** entry)
{
    if (key == nullptr) {
        return E_NULL_POINTER_EXCEPTION;
    }
    TreeMapEntry* p = mRoot;
    while (p != nullptr){
        Integer cmp = Compare(key, p->mKey);
        if (cmp < 0) {
            if (p->mLeft != nullptr) {
                p = p->mLeft;
            }
            else {
                *entry = p;
                REFCOUNT_ADD(*entry);
                return NOERROR;
            }
        }
        else if (cmp > 0) {
            if (p->mRight != nullptr) {
                p = p->mRight;
            }
            else {
                TreeMapEntry* parent = p->mParent;
                TreeMapEntry* ch = p;
                while (parent != nullptr && ch == parent->mRight) {
                    ch = parent;
                    parent = parent->mParent;
                }
                *entry = parent;
                REFCOUNT_ADD(*entry);
                return NOERROR;
            }
        }
        else {
            *entry = p;
            REFCOUNT_ADD(*entry);
            return NOERROR;
        }
    }
    *entry = nullptr;
    return NOERROR;
}

ECode TreeMap::GetFloorEntry(
    /* [in] */ IInterface* key,
    /* [out] */ TreeMapEntry** entry)
{
    if (key == nullptr) {
        return E_NULL_POINTER_EXCEPTION;
    }
    TreeMapEntry* p = mRoot;
    while (p != nullptr) {
        Integer cmp = Compare(key, p->mKey);
        if (cmp > 0) {
            if (p->mRight != nullptr) {
                p = p->mRight;
            }
            else {
                *entry = p;
                REFCOUNT_ADD(*entry);
                return NOERROR;
            }
        }
        else if (cmp < 0) {
            if (p->mLeft != nullptr) {
                p = p->mLeft;
            }
            else {
                TreeMapEntry* parent = p->mParent;
                TreeMapEntry* ch = p;
                while (parent != nullptr && ch == parent->mLeft) {
                    ch = parent;
                    parent = parent->mParent;
                }
                *entry = parent;
                REFCOUNT_ADD(*entry);
                return NOERROR;
            }
        }
        else {
            *entry = p;
            REFCOUNT_ADD(*entry);
            return NOERROR;
        }
    }
    *entry = nullptr;
    return NOERROR;
}

ECode TreeMap::GetHigherEntry(
    /* [in] */ IInterface* key,
    /* [out] */ TreeMapEntry** entry)
{
    if (key == nullptr) {
        return E_NULL_POINTER_EXCEPTION;
    }
    TreeMapEntry* p = mRoot;
    while (p != nullptr) {
        Integer cmp = Compare(key, p->mKey);
        if (cmp < 0) {
            if (p->mLeft != nullptr) {
                p = p->mLeft;
            }
            else {
                *entry = p;
                REFCOUNT_ADD(*entry);
                return NOERROR;
            }
        }
        else {
            if (p->mRight != nullptr) {
                p = p->mRight;
            }
            else {
                TreeMapEntry* parent = p->mParent;
                TreeMapEntry* ch = p;
                while (parent != nullptr && ch == parent->mRight) {
                    ch = parent;
                    parent = parent->mParent;
                }
                *entry = parent;
                REFCOUNT_ADD(*entry);
                return NOERROR;
            }
        }
    }
    *entry = nullptr;
    return NOERROR;
}

ECode TreeMap::GetLowerEntry(
    /* [in] */ IInterface* key,
    /* [out] */ TreeMapEntry** entry)
{
    if (key == nullptr) {
        return E_NULL_POINTER_EXCEPTION;
    }
    TreeMapEntry* p = mRoot;
    while (p != nullptr) {
        Integer cmp = Compare(key, p->mKey);
        if (cmp > 0) {
            if (p->mRight != nullptr) {
                p = p->mRight;
            }
            else {
                *entry = p;
                REFCOUNT_ADD(*entry);
                return NOERROR;
            }
        }
        else {
            if (p->mLeft != nullptr) {
                p = p->mLeft;
            }
            else {
                TreeMapEntry* parent = p->mParent;
                TreeMapEntry* ch = p;
                while (parent != nullptr && ch == parent->mLeft) {
                    ch = parent;
                    parent = parent->mParent;
                }
                *entry = parent;
                REFCOUNT_ADD(*entry);
                return NOERROR;
            }
        }
    }
    *entry = nullptr;
    return NOERROR;
}

ECode TreeMap::Put(
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value,
    /* [out] */ IInterface** prevValue)
{
    TreeMapEntry* t = mRoot;
    if (t == nullptr) {
        if (mComparator != nullptr) {
            if (key == nullptr) {
                Integer cmp;
                FAIL_RETURN(mComparator->Compare(key, key, cmp));
            }
        }
        else {
            if (key == nullptr) {
                Logger::E("TreeMap", "key == nullptr");
                return E_NULL_POINTER_EXCEPTION;
            }
            else if (IComparable::Probe(key) == nullptr) {
                Logger::E("TreeMap", "Cannot cast %s to Comparable.",
                        Object::GetCoclassName(key).string());
                return E_CLASS_CAST_EXCEPTION;
            }
        }
        mRoot = new TreeMapEntry(key, value, nullptr);
        mSize = 1;
        mModCount++;
        if (prevValue != nullptr) {
            *prevValue = nullptr;
        }
        return NOERROR;
    }
    Integer cmp;
    TreeMapEntry* parent;
    // split comparator and comparable paths
    IComparator* cpr = mComparator;
    if (cpr != nullptr) {
        do {
            parent = t;
            cpr->Compare(key, t->mKey, cmp);
            if (cmp < 0) {
                t = t->mLeft;
            }
            else if (cmp > 0) {
                t = t->mRight;
            }
            else {
                return t->SetValue(value, prevValue);
            }
        } while (t != nullptr);
    }
    else {
        if (key == nullptr) {
            return E_NULL_POINTER_EXCEPTION;
        }
        IComparable* k = IComparable::Probe(key);
        do {
            parent = t;
            k->CompareTo(t->mKey, cmp);
            if (cmp < 0) {
                t = t->mLeft;
            }
            else if (cmp > 0) {
                t = t->mRight;
            }
            else {
                return t->SetValue(value, prevValue);
            }
        } while (t != nullptr);
    }
    AutoPtr<TreeMapEntry> e = new TreeMapEntry(key, value, parent);
    if (cmp < 0) {
        parent->mLeft = e;
    }
    else {
        parent->mRight = e;
    }
    FixAfterInsertion(e);
    mSize++;
    mModCount++;
    if (prevValue != nullptr) {
        *prevValue = nullptr;
    }
    return NOERROR;
}

ECode TreeMap::Remove(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** prevValue)
{
    AutoPtr<TreeMapEntry> p;
    FAIL_RETURN(GetEntry(key, &p));
    if (p == nullptr) {
        if (prevValue != nullptr) {
            *prevValue = nullptr;
        }
        return NOERROR;
    }

    if (prevValue != nullptr) {
        p->mValue.MoveTo(prevValue);
    }
    DeleteEntry(p);
    return NOERROR;
}

ECode TreeMap::Clear()
{
    mModCount++;
    mSize = 0;
    mRoot = nullptr;
    return NOERROR;
}

ECode TreeMap::CloneImpl(
    /* [in] */ ITreeMap* newObj)
{
    TreeMap* clone = (TreeMap*)newObj;
    clone->mKeySet = mKeySet;
    clone->mValues = mValues;

    AutoPtr<ISet> entrySet;
    GetEntrySet(entrySet);
    AutoPtr<IIterator> it;
    entrySet->GetIterator(it);
    clone->BuildFromSorted(mSize, it, nullptr, nullptr);
    return NOERROR;
}

ECode TreeMap::FirstEntry(
    /* [out] */ IMapEntry** entry)
{
    VALIDATE_NOT_NULL(entry);

    AutoPtr<IMapEntry> e = ExportEntry(GetFirstEntry());
    e.MoveTo(entry);
    return NOERROR;
}

ECode TreeMap::LastEntry(
    /* [out] */ IMapEntry** entry)
{
    VALIDATE_NOT_NULL(entry);

    AutoPtr<IMapEntry> e = ExportEntry(GetLastEntry());
    e.MoveTo(entry);
    return NOERROR;
}

ECode TreeMap::PollFirstEntry(
    /* [out] */ IMapEntry** entry)
{
    VALIDATE_NOT_NULL(entry);

    AutoPtr<TreeMapEntry> p = GetFirstEntry();
    AutoPtr<IMapEntry> e = ExportEntry(p);
    if (p != nullptr) {
        DeleteEntry(p);
    }
    e.MoveTo(entry);
    return NOERROR;
}

ECode TreeMap::PollLastEntry(
    /* [out] */ IMapEntry** entry)
{
    VALIDATE_NOT_NULL(entry);

    AutoPtr<TreeMapEntry> p = GetLastEntry();
    AutoPtr<IMapEntry> e = ExportEntry(p);
    if (p != nullptr) {
        DeleteEntry(p);
    }
    e.MoveTo(entry);
    return NOERROR;
}

ECode TreeMap::LowerEntry(
    /* [in] */ IInterface* key,
    /* [out] */ IMapEntry** entry)
{
    VALIDATE_NOT_NULL(entry);

    AutoPtr<TreeMapEntry> lentry;
    FAIL_RETURN(GetLowerEntry(key, &lentry));
    AutoPtr<IMapEntry> e = ExportEntry(lentry);
    e.MoveTo(entry);
    return NOERROR;
}

ECode TreeMap::LowerKey(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** lowerkey)
{
    VALIDATE_NOT_NULL(lowerkey);

    AutoPtr<TreeMapEntry> lentry;
    FAIL_RETURN(GetLowerEntry(key, &lentry));
    *lowerkey = (lentry == nullptr) ? nullptr : lentry->mKey;
    REFCOUNT_ADD(*lowerkey);
    return NOERROR;
}

ECode TreeMap::FloorEntry(
    /* [in] */ IInterface* key,
    /* [out] */ IMapEntry** entry)
{
    VALIDATE_NOT_NULL(entry);

    AutoPtr<TreeMapEntry> fentry;
    FAIL_RETURN(GetFloorEntry(key, &fentry));
    AutoPtr<IMapEntry> e = ExportEntry(fentry);
    e.MoveTo(entry);
    return NOERROR;
}

ECode TreeMap::FloorKey(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** floorkey)
{
    VALIDATE_NOT_NULL(floorkey);

    AutoPtr<TreeMapEntry> fentry;
    FAIL_RETURN(GetFloorEntry(key, &fentry));
    *floorkey = (fentry == nullptr) ? nullptr : fentry->mKey;
    REFCOUNT_ADD(*floorkey);
    return NOERROR;
}

ECode TreeMap::CeilingEntry(
    /* [in] */ IInterface* key,
    /* [out]　*/ IMapEntry** entry)
{
    VALIDATE_NOT_NULL(entry);

    AutoPtr<TreeMapEntry> centry;
    FAIL_RETURN(GetCeilingEntry(key, &centry));
    AutoPtr<IMapEntry> e = ExportEntry(centry);
    e.MoveTo(entry);
    return NOERROR;
}

ECode TreeMap::CeilingKey(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** ceilingkey)
{
    VALIDATE_NOT_NULL(ceilingkey);

    AutoPtr<TreeMapEntry> centry;
    FAIL_RETURN(GetCeilingEntry(key, &centry));
    *ceilingkey = (centry == nullptr) ? nullptr : centry->mKey;
    REFCOUNT_ADD(*ceilingkey);
    return NOERROR;
}

ECode TreeMap::HigherEntry(
    /* [in] */ IInterface* key,
    /* [out] */ IMapEntry** entry)
{
    VALIDATE_NOT_NULL(entry);

    AutoPtr<TreeMapEntry> hentry;
    FAIL_RETURN(GetHigherEntry(key, &hentry));
    AutoPtr<IMapEntry> e = ExportEntry(hentry);
    e.MoveTo(entry);
    return NOERROR;
}

ECode TreeMap::HigherKey(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** higherkey)
{
    VALIDATE_NOT_NULL(higherkey);

    AutoPtr<TreeMapEntry> hentry;
    FAIL_RETURN(GetHigherEntry(key, &hentry));
    *higherkey = (hentry == nullptr) ? nullptr : hentry->mKey;
    REFCOUNT_ADD(*higherkey);
    return NOERROR;
}

ECode TreeMap::GetKeySet(
    /* [out] */ AutoPtr<ISet>& keys)
{
    AutoPtr<INavigableSet> navKeys;
    NavigableKeySet(&navKeys);
    keys = std::move(navKeys);
    return NOERROR;
}

ECode TreeMap::NavigableKeySet(
    /* [out] */ INavigableSet** keyset)
{
    VALIDATE_NOT_NULL(keyset);

    if (mNavigableKeySet == nullptr) {
        mNavigableKeySet = new KeySet(this, false);
    }
    *keyset = mNavigableKeySet;
    REFCOUNT_ADD(*keyset);
    return NOERROR;
}

ECode TreeMap::DescendingKeySet(
    /* [out] */ INavigableSet** keyset)
{
    VALIDATE_NOT_NULL(keyset);

    AutoPtr<INavigableMap> dmap;
    DescendingMap(&dmap);
    return dmap->NavigableKeySet(keyset);
}

ECode TreeMap::GetValues(
    /* [out] */ AutoPtr<ICollection>& values)
{
    if (mValues == nullptr) {
        mValues = new Values(this);
    }
    values = mValues;
    return NOERROR;
}

ECode TreeMap::GetEntrySet(
    /* [out] */ AutoPtr<ISet>& entries)
{
    if (mEntrySet == nullptr) {
        mEntrySet = new EntrySet(this);
    }
    entries = mEntrySet;
    return NOERROR;
}

ECode TreeMap::DescendingMap(
    /* [out] */ INavigableMap** map)
{
    VALIDATE_NOT_NULL(map);

    if (mDescendingMap == nullptr) {
        AutoPtr<DescendingSubMap> m = new DescendingSubMap();
        m->Constructor(this, true, nullptr, true,
                true, nullptr, true, false);
        mDescendingMap = m;
    }
    *map = mDescendingMap;
    REFCOUNT_ADD(*map);
    return NOERROR;
}

ECode TreeMap::SubMap(
    /* [in] */ IInterface* fromKey,
    /* [in] */ Boolean fromInclusive,
    /* [in] */ IInterface* toKey,
    /* [in] */ Boolean toInclusive,
    /* [out] */ INavigableMap** submap)
{
    VALIDATE_NOT_NULL(submap);

    AutoPtr<AscendingSubMap> m = new AscendingSubMap();
    m->Constructor(this, false, fromKey, fromInclusive,
            false, toKey, toInclusive, true);
    *submap = (INavigableMap*)m.Get();
    REFCOUNT_ADD(*submap);
    return NOERROR;
}

ECode TreeMap::HeadMap(
    /* [in] */ IInterface* toKey,
    /* [in] */ Boolean inclusive,
    /* [out] */ INavigableMap** headmap)
{
    VALIDATE_NOT_NULL(headmap);

    AutoPtr<AscendingSubMap> m = new AscendingSubMap();
    m->Constructor(this, true, nullptr, true,
            false, toKey, inclusive, true);
    *headmap = (INavigableMap*)m.Get();
    REFCOUNT_ADD(*headmap);
    return NOERROR;
}

ECode TreeMap::TailMap(
    /* [in] */ IInterface* fromKey,
    /* [in] */ Boolean inclusive,
    /* [out] */ INavigableMap** tailmap)
{
    VALIDATE_NOT_NULL(tailmap);

    AutoPtr<AscendingSubMap> m = new AscendingSubMap();
    m->Constructor(this, false, fromKey, inclusive,
            true, nullptr, true, true);
    *tailmap = (INavigableMap*)m.Get();
    REFCOUNT_ADD(*tailmap);
    return NOERROR;
}

ECode TreeMap::SubMap(
    /* [in] */ IInterface* fromKey,
    /* [in] */ IInterface* toKey,
    /* [out] */ ISortedMap** submap)
{
    VALIDATE_NOT_NULL(submap);

    AutoPtr<INavigableMap> m;
    SubMap(fromKey, true, toKey, false, &m);
    m.MoveTo(submap);
    return NOERROR;
}

ECode TreeMap::HeadMap(
    /* [in] */ IInterface* toKey,
    /* [out] */ ISortedMap** headmap)
{
    VALIDATE_NOT_NULL(headmap);

    AutoPtr<INavigableMap> m;
    HeadMap(toKey, false, &m);
    m.MoveTo(headmap);
    return NOERROR;
}

ECode TreeMap::TailMap(
    /* [in] */ IInterface* fromKey,
    /* [out] */ ISortedMap** tailmap)
{
    VALIDATE_NOT_NULL(tailmap);

    AutoPtr<INavigableMap> m;
    TailMap(fromKey, true, &m);
    m.MoveTo(tailmap);
    return NOERROR;
}

AutoPtr<IIterator> TreeMap::GetKeyIterator()
{
    return new KeyIterator(GetFirstEntry(), this);
}

AutoPtr<IIterator> TreeMap::GetDescendingKeyIterator()
{
    return new DescendingKeyIterator(GetLastEntry(), this);
}

Integer TreeMap::Compare(
    /* [in] */ IInterface* k1,
    /* [in] */ IInterface* k2)
{
    Integer cmp;
    if (mComparator == nullptr) {
        IComparable::Probe(k1)->CompareTo(k2, cmp);
    }
    else {
        mComparator->Compare(k1, k2, cmp);
    }
    return cmp;
}

Boolean TreeMap::ValEquals(
    /* [in] */ IInterface* o1,
    /* [in] */ IInterface* o2)
{
    return (o1 == nullptr ? o2 == nullptr : Object::Equals(o1, o2));
}

AutoPtr<IMapEntry> TreeMap::ExportEntry(
    /* [in] */ TreeMapEntry* e)
{
    if (e == nullptr) {
        return nullptr;
    }
    AutoPtr<AbstractMap::SimpleImmutableEntry> entry =
            new AbstractMap::SimpleImmutableEntry();
    entry->Constructor(e);
    return (IMapEntry*)entry.Get();
}

AutoPtr<TreeMap::TreeMapEntry> TreeMap::GetFirstEntry()
{
    TreeMapEntry* p = mRoot;
    if (p != nullptr) {
        while (p->mLeft != nullptr) {
            p = p->mLeft;
        }
    }
    return p;
}

AutoPtr<TreeMap::TreeMapEntry> TreeMap::GetLastEntry()
{
    TreeMapEntry* p = mRoot;
    if (p != nullptr) {
        while (p->mRight != nullptr) {
            p = p->mRight;
        }
    }
    return p;
}

AutoPtr<TreeMap::TreeMapEntry> TreeMap::Successor(
    /* [in] */ TreeMapEntry* t)
{
    if (t == nullptr) {
        return nullptr;
    }
    else if (t->mRight != nullptr) {
        TreeMapEntry* p = t->mRight;
        while (p->mLeft != nullptr) {
            p = p->mLeft;
        }
        return p;
    }
    else {
        TreeMapEntry* p = t->mParent;
        TreeMapEntry* ch = t;
        while (p != nullptr && ch == p->mRight) {
            ch = p;
            p = p->mParent;
        }
        return p;
    }
}

AutoPtr<TreeMap::TreeMapEntry> TreeMap::Predecessor(
    /* [in] */ TreeMapEntry* t)
{
    if (t == nullptr) {
        return nullptr;
    }
    else if (t->mLeft != nullptr) {
        TreeMapEntry* p = t->mLeft;
        while (p->mRight != nullptr) {
            p = p->mRight;
        }
        return p;
    }
    else {
        TreeMapEntry* p = t->mParent;
        TreeMapEntry* ch = t;
        while (p != nullptr && ch == p->mLeft) {
            ch = p;
            p = p->mParent;
        }
        return p;
    }
}

Boolean TreeMap::ColorOf(
    /* [in] */ TreeMapEntry* p)
{
    return (p == nullptr) ? BLACK : p->mColor;
}

AutoPtr<TreeMap::TreeMapEntry> TreeMap::ParentOf(
    /* [in] */ TreeMapEntry* p)
{
    return (p == nullptr) ? nullptr : p->mParent;
}

void TreeMap::SetColor(
    /* [in] */ TreeMapEntry* p,
    /* [in] */ Boolean c)
{
    if (p != nullptr) {
        p->mColor = c;
    }
}

AutoPtr<TreeMap::TreeMapEntry> TreeMap::LeftOf(
    /* [in] */ TreeMapEntry* p)
{
    return (p == nullptr) ? nullptr : p->mLeft;
}

AutoPtr<TreeMap::TreeMapEntry> TreeMap::RightOf(
    /* [in] */ TreeMapEntry* p)
{
    return (p == nullptr) ? nullptr : p->mRight;
}

void TreeMap::RotateLeft(
    /* [in] */ TreeMapEntry* p)
{
    if (p != nullptr) {
        AutoPtr<TreeMapEntry> r = p->mRight;
        p->mRight = r->mLeft;
        if (r->mLeft != nullptr) {
            r->mLeft->mParent = p;
        }
        r->mParent = p->mParent;
        if (p->mParent == nullptr) {
            mRoot = r;
        }
        else if (p->mParent->mLeft == p) {
            p->mParent->mLeft = r;
        }
        else {
            p->mParent->mRight = r;
        }
        r->mLeft = p;
        p->mParent = std::move(r);
    }
}

void TreeMap::RotateRight(
    /* [in] */ TreeMapEntry* p)
{
    if (p != nullptr) {
        AutoPtr<TreeMapEntry> l = p->mLeft;
        p->mLeft = l->mRight;
        if (l->mRight != nullptr) {
            l->mRight->mParent = p;
        }
        l->mParent = p->mParent;
        if (p->mParent == nullptr) {
            mRoot = l;
        }
        else if (p->mParent->mRight == p) {
            p->mParent->mRight = l;
        }
        else {
            p->mParent->mLeft = l;
        }
        l->mRight = p;
        p->mParent = std::move(l);
    }
}

void TreeMap::FixAfterInsertion(
    /* [in] */ AutoPtr<TreeMapEntry> x)
{
    x->mColor = RED;

    while (x != nullptr && x != mRoot && x->mParent->mColor == RED) {
        if (ParentOf(x) == LeftOf(ParentOf(ParentOf(x)))) {
            AutoPtr<TreeMapEntry> y = RightOf(ParentOf(ParentOf(x)));
            if (ColorOf(y) == RED) {
                SetColor(ParentOf(x), BLACK);
                SetColor(y, BLACK);
                SetColor(ParentOf(ParentOf(x)), RED);
                x = ParentOf(ParentOf(x));
            }
            else {
                if (x == RightOf(ParentOf(x))) {
                    x = ParentOf(x);
                    RotateLeft(x);
                }
                SetColor(ParentOf(x), BLACK);
                SetColor(ParentOf(ParentOf(x)), RED);
                RotateRight(ParentOf(ParentOf(x)));
            }
        }
        else {
            AutoPtr<TreeMapEntry> y = LeftOf(ParentOf(ParentOf(x)));
            if (ColorOf(y) == RED) {
                SetColor(ParentOf(x), BLACK);
                SetColor(y, BLACK);
                SetColor(ParentOf(ParentOf(x)), RED);
                x = ParentOf(ParentOf(x));
            }
            else {
                if (x == LeftOf(ParentOf(x))) {
                    x = ParentOf(x);
                    RotateRight(x);
                }
                SetColor(ParentOf(x), BLACK);
                SetColor(ParentOf(ParentOf(x)), RED);
                RotateLeft(ParentOf(ParentOf(x)));
            }
        }
    }
    mRoot->mColor = BLACK;
}

void TreeMap::DeleteEntry(
    /* [in] */ AutoPtr<TreeMapEntry> p)
{
    mModCount++;
    mSize--;

    // If strictly internal, copy successor's element to p and then make p
    // point to successor.
    if (p->mLeft != nullptr && p->mRight != nullptr) {
        AutoPtr<TreeMapEntry> s = Successor(p);
        p->mKey = s->mKey;
        p->mValue = s->mValue;
        p = std::move(s);
    }

    // Start fixup at replacement node, if it exists.
    AutoPtr<TreeMapEntry> replacement = (p->mLeft != nullptr ? p->mLeft : p->mRight);

    if (replacement != nullptr) {
        // Link replacement to parent
        replacement->mParent = p->mParent;
        if (p->mParent == nullptr) {
            mRoot = replacement;
        }
        else if (p == p->mParent->mLeft) {
            p->mParent->mLeft = replacement;
        }
        else {
            p->mParent->mRight = replacement;
        }

        // Null out links so they are OK to use by fixAfterDeletion.
        p->mLeft = p->mRight = p->mParent = nullptr;

        // Fix replacement
        if (p->mColor == BLACK) {
            FixAfterDeletion(replacement);
        }
    }
    else if (p->mParent == nullptr) {
        // return if we are the only node.
        mRoot = nullptr;
    }
    else {
        //  No children. Use self as phantom replacement and unlink.
        if (p->mColor == BLACK) {
            FixAfterDeletion(p);
        }

        if (p->mParent != nullptr) {
            if (p == p->mParent->mLeft){
                p->mParent->mLeft = nullptr;
            }
            else if (p == p->mParent->mRight) {
                p->mParent->mRight = nullptr;
            }
            p->mParent = nullptr;
        }
    }
}

void TreeMap::FixAfterDeletion(
    /* [in] */ AutoPtr<TreeMapEntry> x)
{
    while (x != mRoot && ColorOf(x) == BLACK) {
        if (x == LeftOf(ParentOf(x))) {
            AutoPtr<TreeMapEntry> sib = RightOf(ParentOf(x));

            if (ColorOf(sib) == RED) {
                SetColor(sib, BLACK);
                SetColor(ParentOf(x), RED);
                RotateLeft(ParentOf(x));
                sib = RightOf(ParentOf(x));
            }

            if (ColorOf(LeftOf(sib)) == BLACK &&
                ColorOf(RightOf(sib)) == BLACK) {
                SetColor(sib, RED);
                x = ParentOf(x);
            }
            else {
                if (ColorOf(RightOf(sib)) == BLACK) {
                    SetColor(LeftOf(sib), BLACK);
                    SetColor(sib, RED);
                    RotateRight(sib);
                    sib = RightOf(ParentOf(x));
                }
                SetColor(sib, ColorOf(ParentOf(x)));
                SetColor(ParentOf(x), BLACK);
                SetColor(RightOf(sib), BLACK);
                RotateLeft(ParentOf(x));
                x = mRoot;
            }
        }
        else {
            AutoPtr<TreeMapEntry> sib = LeftOf(ParentOf(x));

            if (ColorOf(sib) == RED) {
                SetColor(sib, BLACK);
                SetColor(ParentOf(x), RED);
                RotateRight(ParentOf(x));
                sib = LeftOf(ParentOf(x));
            }

            if (ColorOf(RightOf(sib)) == BLACK &&
                    ColorOf(LeftOf(sib)) == BLACK) {
                SetColor(sib, RED);
                x = ParentOf(x);
            }
            else {
                if (ColorOf(LeftOf(sib)) == BLACK) {
                    SetColor(RightOf(sib), BLACK);
                    SetColor(sib, RED);
                    RotateLeft(sib);
                    sib = LeftOf(ParentOf(x));
                }
                SetColor(sib, ColorOf(ParentOf(x)));
                SetColor(ParentOf(x), BLACK);
                SetColor(LeftOf(sib), BLACK);
                RotateRight(ParentOf(x));
                x = mRoot;
            }
        }
    }

    SetColor(x, BLACK);
}

ECode TreeMap::AddAllForTreeSet(
    /* [in] */ ISortedSet* set,
    /* [in] */ IInterface* defaultVal)
{
    Integer size;
    ISet::Probe(set)->GetSize(size);
    AutoPtr<IIterator> it;
    ISet::Probe(set)->GetIterator(it);
    BuildFromSorted(size, it, nullptr, defaultVal);
    return NOERROR;
}

ECode TreeMap::BuildFromSorted(
    /* [in] */ Integer size,
    /* [in] */ IIterator* it,
    /* [in] */ IObjectInputStream* str,
    /* [in] */ IInterface* defaultVal,
    /* [out] */ TreeMapEntry** entry)
{
    mSize = size;
    return BuildFromSorted(0, 0, size - 1, ComputeRedLevel(size),
            it, str, defaultVal, &mRoot);
}

ECode TreeMap::BuildFromSorted(
    /* [in] */ Integer level,
    /* [in] */ Integer lo,
    /* [in] */ Integer hi,
    /* [in] */ Integer redLevel,
    /* [in] */ IIterator* it,
    /* [in] */ IObjectInputStream* str,
    /* [in] */ IInterface* defaultVal,
    /* [out] */ TreeMapEntry** entry)
{
    /*
     * Strategy: The root is the middlemost element. To get to it, we
     * have to first recursively construct the entire left subtree,
     * so as to grab all of its elements. We can then proceed with right
     * subtree.
     *
     * The lo and hi arguments are the minimum and maximum
     * indices to pull out of the iterator or stream for current subtree.
     * They are not actually indexed, we just proceed sequentially,
     * ensuring that items are extracted in corresponding order.
     */

    if (hi < lo) {
        if (entry != nullptr) {
            *entry = nullptr;
        }
        return NOERROR;
    }

    Integer mid = ((UInteger)(lo + hi)) >> 1;

    AutoPtr<TreeMapEntry> left;
    if (lo < mid) {
        FAIL_RETURN(BuildFromSorted(level + 1, lo, mid - 1, redLevel,
                it, str, defaultVal, &left));
    }

    // extract key and/or value from iterator or stream
    AutoPtr<IInterface> key, value;
    if (it != nullptr) {
        if (defaultVal == nullptr) {
            AutoPtr<IInterface> entry;
            it->Next(&entry);
            IMapEntry::Probe(entry)->GetKey(key);
            IMapEntry::Probe(entry)->GetValue(value);
        }
        else {
            it->Next(&key);
            value = defaultVal;
        }
    }
    else {
        IObjectInput::Probe(str)->ReadObject(key);
        if (defaultVal != nullptr) {
            value = defaultVal;
        }
        else {
            IObjectInput::Probe(str)->ReadObject(value);
        }
    }

    AutoPtr<TreeMapEntry> middle = new TreeMapEntry(key, value, nullptr);

    // color nodes in non-full bottommost level red
    if (level == redLevel) {
        middle->mColor = RED;
    }

    if (left != nullptr) {
        middle->mLeft = left;
        left->mParent = middle;
    }

    if (mid < hi) {
        AutoPtr<TreeMapEntry> right;
        FAIL_RETURN(BuildFromSorted(level + 1, mid + 1, hi, redLevel,
                it, str, defaultVal, &right));
        middle->mRight = right;
        right->mParent = middle;
    }

    if (entry != nullptr) {
        middle.MoveTo(entry);
    }
    return NOERROR;
}

Integer TreeMap::ComputeRedLevel(
    /* [in] */ Integer sz)
{
    Integer level = 0;
    for (Integer m = sz - 1; m >= 0; m = m / 2 - 1) {
        level++;
    }
    return level;
}

//--------------------------------------------------------------------------

ECode TreeMap::Values::GetIterator(
    /* [out] */ AutoPtr<IIterator>& it)
{
    it = new ValueIterator(mOwner->GetFirstEntry(), mOwner);
    return NOERROR;
}

ECode TreeMap::Values::GetSize(
    /* [out] */ Integer& size)
{
    return mOwner->GetSize(size);
}

ECode TreeMap::Values::Contains(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean& result)
{
    return mOwner->ContainsValue(obj, result);
}

ECode TreeMap::Values::Remove(
    /* [in] */ IInterface* obj,
    /* [out]　*/ Boolean* contained)
{
    for (AutoPtr<TreeMapEntry> e = mOwner->GetFirstEntry();
            e != nullptr; e = Successor(e)) {
        AutoPtr<IInterface> v;
        e->GetValue(v);
        if (ValEquals(v, obj)) {
            mOwner->DeleteEntry(e);
            if (contained != nullptr) {
                *contained = true;
            }
            return NOERROR;
        }
    }
    if (contained != nullptr) {
        *contained = false;
    }
    return NOERROR;
}

ECode TreeMap::Values::Clear()
{
    return mOwner->Clear();
}

//--------------------------------------------------------------------------

ECode TreeMap::EntrySet::GetIterator(
    /* [out] */ AutoPtr<IIterator>& it)
{
    it = new EntryIterator(mOwner->GetFirstEntry(), mOwner);
    return NOERROR;
}

ECode TreeMap::EntrySet::Contains(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean& result)
{
    IMapEntry* entry = IMapEntry::Probe(obj);
    if (entry == nullptr) {
        result = false;
        return NOERROR;
    }
    AutoPtr<IInterface> key, value;
    entry->GetKey(key);
    entry->GetValue(value);
    AutoPtr<TreeMapEntry> p;
    FAIL_RETURN(mOwner->GetEntry(key, &p));
    if (p == nullptr) {
        result = false;
        return NOERROR;
    }
    AutoPtr<IInterface> pv;
    p->GetValue(pv);
    result = ValEquals(pv, value);
    return NOERROR;
}

ECode TreeMap::EntrySet::Remove(
    /* [in] */ IInterface* obj,
    /* [out]　*/ Boolean* contained)
{
    IMapEntry* entry = IMapEntry::Probe(obj);
    if (entry == nullptr) {
        if (contained != nullptr) {
            *contained = false;
        }
        return NOERROR;
    }
    AutoPtr<IInterface> key, value;
    entry->GetKey(key);
    entry->GetValue(value);
    AutoPtr<TreeMapEntry> p;
    FAIL_RETURN(mOwner->GetEntry(key, &p));
    if (p == nullptr) {
        if (contained != nullptr) {
            *contained = false;
            return NOERROR;
        }
    }
    AutoPtr<IInterface> pv;
    p->GetValue(pv);
    if (ValEquals(pv, value)) {
        mOwner->DeleteEntry(p);
        if (contained != nullptr) {
            *contained = true;
        }
        return NOERROR;
    }
    if (contained != nullptr) {
        *contained = false;
        return NOERROR;
    }
    return NOERROR;
}

ECode TreeMap::EntrySet::GetSize(
    /* [out] */ Integer& size)
{
    return mOwner->GetSize(size);
}

ECode TreeMap::EntrySet::Clear()
{
    return mOwner->Clear();
}

//--------------------------------------------------------------------------

COMO_INTERFACE_IMPL_2(TreeMap::KeySet, AbstractSet, INavigableSet, ISortedSet);

TreeMap::KeySet::KeySet(
    /* [in] */ INavigableMap* map,
    /* [in] */ Boolean holdRef)
    : mMap(map)
    , mHoldRef(holdRef)
{
    if (mHoldRef) {
        REFCOUNT_ADD(mMap);
    }
}

TreeMap::KeySet::~KeySet()
{
    if (mHoldRef) {
        REFCOUNT_RELEASE(mMap);
    }
}

ECode TreeMap::KeySet::GetIterator(
    /* [out] */ AutoPtr<IIterator>& it)
{
    if (ITreeMap::Probe(mMap)) {
        it = ((TreeMap*)mMap)->GetKeyIterator();
    }
    else {
        it = ((TreeMap::NavigableSubMap*)mMap)->GetKeyIterator();
    }
    return NOERROR;
}

ECode TreeMap::KeySet::GetDescendingIterator(
    /* [out] */ AutoPtr<IIterator>& it)
{
    if (ITreeMap::Probe(mMap)) {
        it = ((TreeMap*)mMap)->GetDescendingKeyIterator();
    }
    else {
        it = ((TreeMap::NavigableSubMap*)mMap)->GetDescendingKeyIterator();
    }
    return NOERROR;
}

ECode TreeMap::KeySet::GetSize(
    /* [out] */ Integer& size)
{
    return IMap::Probe(mMap)->GetSize(size);
}

ECode TreeMap::KeySet::IsEmpty(
    /* [out] */ Boolean& result)
{
    return IMap::Probe(mMap)->IsEmpty(result);
}

ECode TreeMap::KeySet::Contains(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean& result)
{
    return IMap::Probe(mMap)->ContainsKey(obj, result);
}

ECode TreeMap::KeySet::Clear()
{
    return IMap::Probe(mMap)->Clear();
}

ECode TreeMap::KeySet::Lower(
    /* [in] */ IInterface* e,
    /* [out] */ IInterface** lowerE)
{
    return mMap->LowerKey(e, lowerE);
}

ECode TreeMap::KeySet::Floor(
    /* [in] */ IInterface* e,
    /* [out] */ IInterface** floorE)
{
    return mMap->FloorKey(e, floorE);
}

ECode TreeMap::KeySet::Ceiling(
    /* [in] */ IInterface* e,
    /* [out] */ IInterface** ceilingE)
{
    return mMap->CeilingKey(e, ceilingE);
}

ECode TreeMap::KeySet::Higher(
    /* [in] */ IInterface* e,
    /* [out] */ IInterface** higherE)
{
    return mMap->HigherKey(e, higherE);
}

ECode TreeMap::KeySet::First(
    /* [out] */ IInterface** element)
{
    return ISortedMap::Probe(mMap)->FirstKey(element);
}

ECode TreeMap::KeySet::Last(
    /* [out] */ IInterface** element)
{
    return ISortedMap::Probe(mMap)->LastKey(element);
}

ECode TreeMap::KeySet::Comparator(
    /* [out] */ IComparator** comparator)
{
    return ISortedMap::Probe(mMap)->Comparator(comparator);
}

ECode TreeMap::KeySet::PollFirst(
    /* [out] */ IInterface** e)
{
    VALIDATE_NOT_NULL(e);

    AutoPtr<IMapEntry> entry;
    mMap->PollFirstEntry(&entry);
    if (entry == nullptr) {
        *e = nullptr;
        return NOERROR;
    }
    AutoPtr<IInterface> v;
    ECode ec = entry->GetKey(v);
    v.MoveTo(e);
    return ec;
}

ECode TreeMap::KeySet::PollLast(
    /* [out] */ IInterface** e)
{
    VALIDATE_NOT_NULL(e);

    AutoPtr<IMapEntry> entry;
    mMap->PollLastEntry(&entry);
    if (entry == nullptr) {
        *e = nullptr;
        return NOERROR;
    }
    AutoPtr<IInterface> v;
    ECode ec = entry->GetKey(v);
    v.MoveTo(e);
    return ec;
}

ECode TreeMap::KeySet::Remove(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* contained)
{
    Integer oldSize, newSize;
    GetSize(oldSize);
    IMap::Probe(mMap)->Remove(obj);
    GetSize(newSize);
    if (contained != nullptr) {
        *contained = oldSize != newSize;
    }
    return NOERROR;
}

ECode TreeMap::KeySet::SubSet(
    /* [in] */ IInterface* fromElement,
    /* [in] */ Boolean fromInclusive,
    /* [in] */ IInterface* toElement,
    /* [in] */ Boolean toInclusive,
    /* [out] */ INavigableSet** subset)
{
    VALIDATE_NOT_NULL(subset);

    AutoPtr<INavigableMap> submap;
    mMap->SubMap(fromElement, fromInclusive, toElement, toInclusive, &submap);
    *subset = new KeySet(submap, true);
    REFCOUNT_ADD(*subset);
    return NOERROR;
}

ECode TreeMap::KeySet::HeadSet(
    /* [in] */ IInterface* toElement,
    /* [in] */ Boolean inclusive,
    /* [out] */ INavigableSet** headset)
{
    VALIDATE_NOT_NULL(headset);

    AutoPtr<INavigableMap> headmap;
    mMap->HeadMap(toElement, inclusive, &headmap);
    *headset = new KeySet(headmap, true);
    REFCOUNT_ADD(*headset);
    return NOERROR;
}

ECode TreeMap::KeySet::TailSet(
    /* [in] */ IInterface* fromElement,
    /* [in] */ Boolean inclusive,
    /* [out] */ INavigableSet** tailset)
{
    VALIDATE_NOT_NULL(tailset);

    AutoPtr<INavigableMap> tailmap;
    mMap->TailMap(fromElement, inclusive, &tailmap);
    *tailset = new KeySet(tailmap, true);
    REFCOUNT_ADD(*tailset);
    return NOERROR;
}

ECode TreeMap::KeySet::SubSet(
    /* [in] */ IInterface* fromElement,
    /* [in] */ IInterface* toElement,
    /* [out] */ ISortedSet** subset)
{
    VALIDATE_NOT_NULL(subset);

    AutoPtr<INavigableSet> navSet;
    SubSet(fromElement, true, toElement, false, &navSet);
    navSet.MoveTo(subset);
    return NOERROR;
}

ECode TreeMap::KeySet::HeadSet(
    /* [in] */ IInterface* toElement,
    /* [out] */ ISortedSet** headset)
{
    VALIDATE_NOT_NULL(headset);

    AutoPtr<INavigableSet> navSet;
    HeadSet(toElement, false, &navSet);
    navSet.MoveTo(headset);
    return NOERROR;
}

ECode TreeMap::KeySet::TailSet(
    /* [in] */ IInterface* fromElement,
    /* [out] */ ISortedSet** tailset)
{
    VALIDATE_NOT_NULL(tailset);

    AutoPtr<INavigableSet> navSet;
    TailSet(fromElement, true, &navSet);
    navSet.MoveTo(tailset);
    return NOERROR;
}

ECode TreeMap::KeySet::DescendingSet(
    /* [out] */ INavigableSet** set)
{
    VALIDATE_NOT_NULL(set);

    AutoPtr<INavigableMap> navMap;
    mMap->DescendingMap(&navMap);
    *set = new KeySet(navMap, true);
    REFCOUNT_ADD(*set);
    return NOERROR;
}

ECode TreeMap::KeySet::Add(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* modified)
{
    return AbstractSet::Add(obj, modified);
}

//--------------------------------------------------------------------------

COMO_INTERFACE_IMPL_1(TreeMap::PrivateEntryIterator, Object, IIterator);

TreeMap::PrivateEntryIterator::PrivateEntryIterator(
    /* [in] */ TreeMapEntry* first,
    /* [in] */ TreeMap* owner)
    : mNext(first)
    , mExpectedModCount(owner->mModCount)
    , mOwner(owner)
{}

ECode TreeMap::PrivateEntryIterator::HasNext(
    /* [out] */ Boolean& result)
{
    result = mNext != nullptr;
    return NOERROR;
}

ECode TreeMap::PrivateEntryIterator::NextEntry(
    /* [out] */ TreeMapEntry** entry)
{
    if (mNext == nullptr) {
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }
    if (mOwner->mModCount != mExpectedModCount) {
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }
    AutoPtr<TreeMapEntry> e = mNext;
    mNext = Successor(e);
    mLastReturned = e;
    e.MoveTo(entry);
    return NOERROR;
}

ECode TreeMap::PrivateEntryIterator::PrevEntry(
    /* [out] */ TreeMapEntry** entry)
{
    if (mNext == nullptr) {
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }
    if (mOwner->mModCount != mExpectedModCount) {
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }
    AutoPtr<TreeMapEntry> e = mNext;
    mNext = Predecessor(e);
    mLastReturned = e;
    e.MoveTo(entry);
    return NOERROR;
}

ECode TreeMap::PrivateEntryIterator::Remove()
{
    if (mLastReturned == nullptr) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (mOwner->mModCount != mExpectedModCount) {
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }
    if (mLastReturned->mLeft != nullptr && mLastReturned->mRight != nullptr) {
        mNext = mLastReturned;
    }
    mOwner->DeleteEntry(mLastReturned);
    mExpectedModCount = mOwner->mModCount;
    mLastReturned = nullptr;
    return NOERROR;
}

//--------------------------------------------------------------------------

ECode TreeMap::EntryIterator::Next(
    /* [out] */ IInterface** object)
{
    AutoPtr<TreeMapEntry> entry;
    FAIL_RETURN(NextEntry(&entry));
    if (object != nullptr) {
        *object = (IMapEntry*)entry.Get();
        REFCOUNT_ADD(*object);
    }
    return NOERROR;
}

//--------------------------------------------------------------------------

ECode TreeMap::ValueIterator::Next(
    /* [out] */ IInterface** object)
{
    AutoPtr<TreeMapEntry> entry;
    FAIL_RETURN(NextEntry(&entry));
    if (object != nullptr) {
        *object = entry->mValue;
        REFCOUNT_ADD(*object);
    }
    return NOERROR;
}

//--------------------------------------------------------------------------

ECode TreeMap::KeyIterator::Next(
    /* [out] */ IInterface** object)
{
    AutoPtr<TreeMapEntry> entry;
    FAIL_RETURN(NextEntry(&entry));
    if (object != nullptr) {
        *object = entry->mKey;
        REFCOUNT_ADD(*object);
    }
    return NOERROR;
}

//--------------------------------------------------------------------------

ECode TreeMap::DescendingKeyIterator::Next(
    /* [out] */ IInterface** object)
{
    AutoPtr<TreeMapEntry> entry;
    FAIL_RETURN(PrevEntry(&entry));
    if (object != nullptr) {
        *object = entry->mKey;
        REFCOUNT_ADD(*object);
    }
    return NOERROR;
}

ECode TreeMap::DescendingKeyIterator::Remove()
{
    if (mLastReturned == nullptr) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (mOwner->mModCount != mExpectedModCount) {
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }
    mOwner->DeleteEntry(mLastReturned);
    mLastReturned = nullptr;
    mExpectedModCount = mOwner->mModCount;
    return NOERROR;
}

//--------------------------------------------------------------------------

COMO_INTERFACE_IMPL_3(TreeMap::NavigableSubMap, AbstractMap, INavigableMap, ISortedMap, ISerializable);

TreeMap::NavigableSubMap::~NavigableSubMap()
{
    if (mHoldRef) {
        REFCOUNT_RELEASE(mMap);
    }
}

ECode TreeMap::NavigableSubMap::Constructor(
    /* [in] */ TreeMap* m,
    /* [in] */ Boolean fromStart,
    /* [in] */ IInterface* lo,
    /* [in] */ Boolean loInclusive,
    /* [in] */ Boolean toEnd,
    /* [in] */ IInterface* hi,
    /* [in] */ Boolean hiInclusive,
    /* [in] */ Boolean holdRef)
{
    if (!fromStart && !toEnd) {
        if (m->Compare(lo, hi) > 0) {
            Logger::E("TreeMap::NavigableSubMap", "fromKey > toKey");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    mMap = m;
    mFromStart = fromStart;
    mLo = lo;
    mLoInclusive = loInclusive;
    mToEnd = toEnd;
    mHi = hi;
    mHiInclusive = hiInclusive;

    if (mHoldRef) {
        REFCOUNT_ADD(mMap);
    }

    return NOERROR;
}

Boolean TreeMap::NavigableSubMap::TooLow(
    /* [in] */ IInterface* key)
{
    if (!mFromStart) {
        Integer c = mMap->Compare(key, mLo);
        if (c < 0 || (c == 0 && !mLoInclusive)) {
            return true;
        }
    }
    return false;
}

Boolean TreeMap::NavigableSubMap::TooHigh(
    /* [in] */ IInterface* key)
{
    if (!mToEnd) {
        Integer c = mMap->Compare(key, mHi);
        if (c > 0 || (c == 0 && !mHiInclusive)) {
            return true;
        }

    }
    return false;
}

Boolean TreeMap::NavigableSubMap::InRange(
    /* [in] */ IInterface* key)
{
    return !TooLow(key) && !TooHigh(key);
}

Boolean TreeMap::NavigableSubMap::InClosedRange(
    /* [in] */ IInterface* key)
{
    return (mFromStart || mMap->Compare(key, mLo) >= 0) &&
            (mToEnd || mMap->Compare(mHi, key) >= 0);
}

Boolean TreeMap::NavigableSubMap::InRange(
    /* [in] */ IInterface* key,
    /* [in] */ Boolean inclusive)
{
    return inclusive ? InRange(key) : InClosedRange(key);
}

AutoPtr<TreeMap::TreeMapEntry> TreeMap::NavigableSubMap::AbsLowest()
{
    AutoPtr<TreeMapEntry> e;
    if (mFromStart) {
        e = mMap->GetFirstEntry();
    }
    else if (mLoInclusive) {
        mMap->GetCeilingEntry(mLo, &e);
    }
    else {
        mMap->GetHigherEntry(mLo, &e);
    }
    return (e == nullptr || TooHigh(e->mKey)) ? nullptr : e;
}

AutoPtr<TreeMap::TreeMapEntry> TreeMap::NavigableSubMap::AbsHighest()
{
    AutoPtr<TreeMapEntry> e;
    if (mToEnd) {
        e = mMap->GetLastEntry();
    }
    else if (mHiInclusive) {
        mMap->GetFloorEntry(mHi, &e);
    }
    else {
        mMap->GetLowerEntry(mHi, &e);
    }
    return (e == nullptr || TooLow(e->mKey)) ? nullptr : e;
}

AutoPtr<TreeMap::TreeMapEntry> TreeMap::NavigableSubMap::AbsCeiling(
    /* [in] */ IInterface* key)
{
    if (TooLow(key)) {
        return AbsLowest();
    }
    AutoPtr<TreeMapEntry> e;
    mMap->GetCeilingEntry(key, &e);
    return (e == nullptr || TooHigh(e->mKey)) ? nullptr : e;
}

AutoPtr<TreeMap::TreeMapEntry> TreeMap::NavigableSubMap::AbsHigher(
    /* [in] */ IInterface* key)
{
    if (TooLow(key)) {
        return AbsLowest();
    }
    AutoPtr<TreeMapEntry> e;
    mMap->GetHigherEntry(key, &e);
    return (e == nullptr || TooHigh(e->mKey)) ? nullptr : e;
}

AutoPtr<TreeMap::TreeMapEntry> TreeMap::NavigableSubMap::AbsFloor(
    /* [in] */ IInterface* key)
{
    if (TooHigh(key)) {
        return AbsHighest();
    }
    AutoPtr<TreeMapEntry> e;
    mMap->GetFloorEntry(key, &e);
    return (e == nullptr || TooLow(e->mKey)) ? nullptr : e;
}

AutoPtr<TreeMap::TreeMapEntry> TreeMap::NavigableSubMap::AbsLower(
    /* [in] */ IInterface* key)
{
    if (TooHigh(key)) {
        return AbsHighest();
    }
    AutoPtr<TreeMapEntry> e;
    mMap->GetLowerEntry(key, &e);
    return (e == nullptr || TooLow(e->mKey)) ? nullptr : e;
}

AutoPtr<TreeMap::TreeMapEntry> TreeMap::NavigableSubMap::AbsHighFence()
{
    AutoPtr<TreeMapEntry> e;
    if (mToEnd) {
        return e;
    }
    else if (mHiInclusive) {
        mMap->GetHigherEntry(mHi, &e);
        return e;
    }
    else {
        mMap->GetCeilingEntry(mHi, &e);
        return e;
    }
}

AutoPtr<TreeMap::TreeMapEntry> TreeMap::NavigableSubMap::AbsLowFence()
{
    AutoPtr<TreeMapEntry> e;
    if (mFromStart) {
        return e;
    }
    else if (mLoInclusive) {
        mMap->GetLowerEntry(mLo, &e);
        return e;
    }
    else {
        mMap->GetFloorEntry(mLo, &e);
        return e;
    }
}

ECode TreeMap::NavigableSubMap::IsEmpty(
    /* [out] */ Boolean& result)
{
    if (mFromStart && mToEnd) {
        return mMap->IsEmpty(result);
    }
    else {
        AutoPtr<ISet> entries;
        GetEntrySet(entries);
        return entries->IsEmpty(result);
    }
}

ECode TreeMap::NavigableSubMap::GetSize(
    /* [out] */ Integer& size)
{
    if (mFromStart && mToEnd) {
        return mMap->GetSize(size);
    }
    else {
        AutoPtr<ISet> entries;
        GetEntrySet(entries);
        return entries->GetSize(size);
    }
}

ECode TreeMap::NavigableSubMap::ContainsKey(
    /* [in] */ IInterface* key,
    /* [out] */ Boolean& result)
{
    if (key == nullptr) {
        Logger::E("TreeMap::NavigableSubMap", "key == nullptr");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (!InRange(key)) {
        result = false;
        return NOERROR;
    }
    return mMap->ContainsKey(key, result);
}

ECode TreeMap::NavigableSubMap::Put(
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value,
    /* [out] */ IInterface** prevValue)
{
    if (key == nullptr) {
        Logger::E("TreeMap::NavigableSubMap", "key == nullptr");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (!InRange(key)) {
        Logger::E("TreeMap::NavigableSubMap", "key out of range");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return mMap->Put(key, value, prevValue);
}

ECode TreeMap::NavigableSubMap::Get(
    /* [in] */ IInterface* key,
    /* [out] */ AutoPtr<IInterface>& value)
{
    if (key == nullptr) {
        Logger::E("TreeMap::NavigableSubMap", "key == nullptr");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (!InRange(key)) {
        value = nullptr;
        return NOERROR;
    }
    return mMap->Get(key, value);
}

ECode TreeMap::NavigableSubMap::Remove(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** prevValue)
{
    if (key == nullptr) {
        Logger::E("TreeMap::NavigableSubMap", "key == nullptr");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (!InRange(key)) {
        if (prevValue != nullptr) {
            *prevValue = nullptr;
        }
        return NOERROR;
    }
    return mMap->Remove(key, prevValue);
}

ECode TreeMap::NavigableSubMap::CeilingEntry(
    /* [in] */ IInterface* key,
    /* [out]　*/ IMapEntry** entry)
{
    VALIDATE_NOT_NULL(entry);

    ExportEntry(SubCeiling(key)).MoveTo(entry);
    return NOERROR;
}

ECode TreeMap::NavigableSubMap::CeilingKey(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** ceilingkey)
{
    VALIDATE_NOT_NULL(ceilingkey);

    AutoPtr<TreeMapEntry> e = SubCeiling(key);
    *ceilingkey = (e != nullptr) ? e->mKey : nullptr;
    REFCOUNT_ADD(*ceilingkey);
    return NOERROR;
}

ECode TreeMap::NavigableSubMap::HigherEntry(
    /* [in] */ IInterface* key,
    /* [out] */ IMapEntry** entry)
{
    VALIDATE_NOT_NULL(entry);

    ExportEntry(SubHigher(key)).MoveTo(entry);
    return NOERROR;
}

ECode TreeMap::NavigableSubMap::HigherKey(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** higherkey)
{
    VALIDATE_NOT_NULL(higherkey);

    AutoPtr<TreeMapEntry> e = SubHigher(key);
    *higherkey = (e != nullptr) ? e->mKey : nullptr;
    REFCOUNT_ADD(*higherkey);
    return NOERROR;
}

ECode TreeMap::NavigableSubMap::FloorEntry(
    /* [in] */ IInterface* key,
    /* [out] */ IMapEntry** entry)
{
    VALIDATE_NOT_NULL(entry);

    ExportEntry(SubFloor(key)).MoveTo(entry);
    return NOERROR;
}

ECode TreeMap::NavigableSubMap::FloorKey(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** floorkey)
{
    VALIDATE_NOT_NULL(floorkey);

    AutoPtr<TreeMapEntry> e = SubFloor(key);
    *floorkey = (e != nullptr) ? e->mKey : nullptr;
    REFCOUNT_ADD(*floorkey);
    return NOERROR;
}

ECode TreeMap::NavigableSubMap::LowerEntry(
    /* [in] */ IInterface* key,
    /* [out] */ IMapEntry** entry)
{
    VALIDATE_NOT_NULL(entry);

    ExportEntry(SubLower(key)).MoveTo(entry);
    return NOERROR;
}

ECode TreeMap::NavigableSubMap::LowerKey(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** lowerkey)
{
    VALIDATE_NOT_NULL(lowerkey);

    AutoPtr<TreeMapEntry> e = SubLower(key);
    *lowerkey = (e != nullptr) ? e->mKey : nullptr;
    REFCOUNT_ADD(*lowerkey);
    return NOERROR;
}

ECode TreeMap::NavigableSubMap::FirstKey(
    /* [out] */ IInterface** key)
{
    VALIDATE_NOT_NULL(key);

    AutoPtr<TreeMapEntry> e = SubLowest();
    if (e == nullptr) {
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }
    *key = e->mKey;
    REFCOUNT_ADD(*key);
    return NOERROR;
}

ECode TreeMap::NavigableSubMap::LastKey(
    /* [out] */ IInterface** key)
{
    VALIDATE_NOT_NULL(key);

    AutoPtr<TreeMapEntry> e = SubHighest();
    if (e == nullptr) {
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }
    *key = e->mKey;
    REFCOUNT_ADD(*key);
    return NOERROR;
}

ECode TreeMap::NavigableSubMap::FirstEntry(
    /* [out] */ IMapEntry** entry)
{
    VALIDATE_NOT_NULL(entry);

    ExportEntry(SubLowest()).MoveTo(entry);
    return NOERROR;
}

ECode TreeMap::NavigableSubMap::LastEntry(
    /* [out] */ IMapEntry** entry)
{
    VALIDATE_NOT_NULL(entry);

    ExportEntry(SubHighest()).MoveTo(entry);
    return NOERROR;
}

ECode TreeMap::NavigableSubMap::PollFirstEntry(
    /* [out] */ IMapEntry** entry)
{
    VALIDATE_NOT_NULL(entry);

    AutoPtr<TreeMapEntry> e = SubLowest();
    ExportEntry(e).MoveTo(entry);
    if (e != nullptr) {
        mMap->DeleteEntry(e);
    }
    return NOERROR;
}

ECode TreeMap::NavigableSubMap::PollLastEntry(
    /* [out] */ IMapEntry** entry)
{
    VALIDATE_NOT_NULL(entry);

    AutoPtr<TreeMapEntry> e = SubHighest();
    ExportEntry(e).MoveTo(entry);
    if (e != nullptr) {
        mMap->DeleteEntry(e);
    }
    return NOERROR;
}

ECode TreeMap::NavigableSubMap::NavigableKeySet(
    /* [out] */ INavigableSet** keyset)
{
    VALIDATE_NOT_NULL(keyset);

    if (mNavigableKeySetView == nullptr) {
        mNavigableKeySetView = new KeySet(this, false);
    }
    *keyset = mNavigableKeySetView;
    REFCOUNT_ADD(*keyset);
    return NOERROR;
}

ECode TreeMap::NavigableSubMap::GetKeySet(
    /* [out] */ AutoPtr<ISet>& keys)
{
    AutoPtr<INavigableSet> navSet;
    NavigableKeySet(&navSet);
    keys = std::move(navSet);
    return NOERROR;
}

ECode TreeMap::NavigableSubMap::DescendingKeySet(
    /* [out] */ INavigableSet** keyset)
{
    VALIDATE_NOT_NULL(keyset);

    AutoPtr<INavigableMap> navMap;
    DescendingMap(&navMap);
    return navMap->NavigableKeySet(keyset);
}

ECode TreeMap::NavigableSubMap::SubMap(
    /* [in] */ IInterface* fromKey,
    /* [in] */ IInterface* toKey,
    /* [out] */ ISortedMap** submap)
{
    VALIDATE_NOT_NULL(submap);

    AutoPtr<INavigableMap> navMap;
    FAIL_RETURN(SubMap(fromKey, true, toKey, false, &navMap));
    navMap.MoveTo(submap);
    return NOERROR;
}

ECode TreeMap::NavigableSubMap::HeadMap(
    /* [in] */ IInterface* toKey,
    /* [out] */ ISortedMap** headmap)
{
    VALIDATE_NOT_NULL(headmap);

    AutoPtr<INavigableMap> navMap;
    FAIL_RETURN(HeadMap(toKey, false, &navMap));
    navMap.MoveTo(headmap);
    return NOERROR;
}

ECode TreeMap::NavigableSubMap::TailMap(
    /* [in] */ IInterface* fromKey,
    /* [out] */ ISortedMap** tailmap)
{
    VALIDATE_NOT_NULL(tailmap);

    AutoPtr<INavigableMap> navMap;
    FAIL_RETURN(TailMap(fromKey, true, &navMap));
    navMap.MoveTo(tailmap);
    return NOERROR;
}

//--------------------------------------------------------------------------

ECode TreeMap::NavigableSubMap::EntrySetView::GetSize(
    /* [out] */ Integer& size)
{
    if (mOwner->mFromStart && mOwner->mToEnd) {
        return mOwner->mMap->GetSize(size);
    }
    if (mSize == -1 || mSizeModCount != mOwner->mMap->mModCount) {
        mSizeModCount = mOwner->mMap->mModCount;
        mSize = 0;
        AutoPtr<IIterator> i;
        GetIterator(i);
        Boolean hasNext;
        while (i->HasNext(hasNext), hasNext) {
            mSize++;
            i->Next();
        }
    }
    size = mSize;
    return NOERROR;
}

ECode TreeMap::NavigableSubMap::EntrySetView::IsEmpty(
    /* [out] */ Boolean& result)
{
    AutoPtr<TreeMapEntry> e = mOwner->AbsLowest();
    result = (e == nullptr) || mOwner->TooHigh(e->mKey);
    return NOERROR;
}

ECode TreeMap::NavigableSubMap::EntrySetView::Contains(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean& result)
{
    IMapEntry* entry = IMapEntry::Probe(obj);
    if (entry == nullptr) {
        result = false;
        return NOERROR;
    }
    AutoPtr<IInterface> key;
    entry->GetKey(key);
    if (key == nullptr) {
        Logger::E("TreeMap::NavigableSubMap::EntrySetView", "key == nullptr");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (!mOwner->InRange(key)) {
        result = false;
        return NOERROR;
    }
    AutoPtr<TreeMapEntry> node;
    FAIL_RETURN(mOwner->mMap->GetEntry(key, &node));
    if (node == nullptr) {
        result = false;
        return NOERROR;
    }
    AutoPtr<IInterface> nv, ev;
    node->GetValue(nv);
    entry->GetValue(ev);
    result = ValEquals(nv, ev);
    return NOERROR;
}

ECode TreeMap::NavigableSubMap::EntrySetView::Remove(
    /* [in] */ IInterface* obj,
    /* [out]　*/ Boolean* contained)
{
    IMapEntry* entry = IMapEntry::Probe(obj);
    if (entry == nullptr) {
        if (contained != nullptr) {
            *contained = false;
        }
        return NOERROR;
    }
    AutoPtr<IInterface> key;
    entry->GetKey(key);
    if (key == nullptr) {
        Logger::E("TreeMap::NavigableSubMap::EntrySetView", "key == nullptr");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (!mOwner->InRange(key)) {
        if (contained != nullptr) {
            *contained = false;
        }
        return NOERROR;
    }
    AutoPtr<TreeMapEntry> node;
    FAIL_RETURN(mOwner->mMap->GetEntry(key, &node));
    if (node == nullptr) {
        if (contained != nullptr) {
            *contained = false;
        }
        return NOERROR;
    }
    AutoPtr<IInterface> nv, ev;
    node->GetValue(nv);
    entry->GetValue(ev);
    if (ValEquals(nv, ev)) {
        mOwner->mMap->DeleteEntry(node);
        if (contained != nullptr) {
            *contained = true;
        }
        return NOERROR;
    }
    if (contained != nullptr) {
        *contained = false;
    }
    return NOERROR;
}

//--------------------------------------------------------------------------

COMO_INTERFACE_IMPL_1(TreeMap::NavigableSubMap::SubMapIterator, Object, IIterator);

TreeMap::NavigableSubMap::SubMapIterator::SubMapIterator(
    /* [in] */ TreeMapEntry* first,
    /* [in] */ TreeMapEntry* fence,
    /* [in] */ NavigableSubMap* owner)
    : mNext(first)
    , mExpectedModCount(owner->mMap->mModCount)
    , mOwner(owner)
{
    mFenceKey = (fence == nullptr) ? GetUNBOUNDED() : fence->mKey;
}

ECode TreeMap::NavigableSubMap::SubMapIterator::HasNext(
    /* [out] */ Boolean& result)
{
    result = mNext != nullptr && (!IInterface::Equals(mNext->mKey, mFenceKey));
    return NOERROR;
}

ECode TreeMap::NavigableSubMap::SubMapIterator::NextEntry(
    /* [out] */ TreeMapEntry** entry)
{
    AutoPtr<TreeMapEntry> e = mNext;
    if (e == nullptr || IInterface::Equals(e->mKey, mFenceKey)) {
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }
    if (mOwner->mMap->mModCount != mExpectedModCount) {
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }
    mNext = Successor(e);
    mLastReturned = e;
    e.MoveTo(entry);
    return NOERROR;
}

ECode TreeMap::NavigableSubMap::SubMapIterator::PrevEntry(
    /* [out] */ TreeMapEntry** entry)
{
    AutoPtr<TreeMapEntry> e = mNext;
    if (e == nullptr || IInterface::Equals(e->mKey, mFenceKey)) {
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }
    if (mOwner->mMap->mModCount != mExpectedModCount) {
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }
    mNext = Predecessor(e);
    mLastReturned = e;
    e.MoveTo(entry);
    return NOERROR;
}

ECode TreeMap::NavigableSubMap::SubMapIterator::RemoveAscending()
{
    if (mLastReturned == nullptr) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (mOwner->mMap->mModCount != mExpectedModCount) {
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }
    if (mLastReturned->mLeft != nullptr && mLastReturned->mRight != nullptr) {
        mNext = mLastReturned;
    }
    mOwner->mMap->DeleteEntry(mLastReturned);
    mLastReturned = nullptr;
    mExpectedModCount = mOwner->mMap->mModCount;
    return NOERROR;
}

ECode TreeMap::NavigableSubMap::SubMapIterator::RemoveDescending()
{
    if (mLastReturned == nullptr) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (mOwner->mMap->mModCount != mExpectedModCount) {
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }
    mOwner->mMap->DeleteEntry(mLastReturned);
    mLastReturned = nullptr;
    mExpectedModCount = mOwner->mMap->mModCount;
    return NOERROR;
}

//--------------------------------------------------------------------------

ECode TreeMap::NavigableSubMap::SubMapEntryIterator::Next(
    /* [out] */ IInterface** object)
{
    AutoPtr<TreeMapEntry> entry;
    FAIL_RETURN(NextEntry(&entry));
    *object = (IMapEntry*)entry.Get();
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode TreeMap::NavigableSubMap::SubMapEntryIterator::Remove()
{
    return RemoveAscending();
}

//--------------------------------------------------------------------------

ECode TreeMap::NavigableSubMap::DescendingSubMapEntryIterator::Next(
    /* [out] */ IInterface** object)
{
    AutoPtr<TreeMapEntry> entry;
    FAIL_RETURN(PrevEntry(&entry));
    *object = (IMapEntry*)entry.Get();
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode TreeMap::NavigableSubMap::DescendingSubMapEntryIterator::Remove()
{
    return RemoveDescending();
}

//--------------------------------------------------------------------------

ECode TreeMap::NavigableSubMap::SubMapKeyIterator::Next(
    /* [out] */ IInterface** object)
{
    AutoPtr<TreeMapEntry> entry;
    FAIL_RETURN(NextEntry(&entry));
    *object = entry->mKey;
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode TreeMap::NavigableSubMap::SubMapKeyIterator::Remove()
{
    return RemoveAscending();
}

//--------------------------------------------------------------------------

ECode TreeMap::NavigableSubMap::DescendingSubMapKeyIterator::Next(
    /* [out] */ IInterface** object)
{
    AutoPtr<TreeMapEntry> entry;
    FAIL_RETURN(PrevEntry(&entry));
    *object = entry->mKey;
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode TreeMap::NavigableSubMap::DescendingSubMapKeyIterator::Remove()
{
    return RemoveDescending();
}

//--------------------------------------------------------------------------

ECode TreeMap::AscendingSubMap::Constructor(
    /* [in] */ TreeMap* m,
    /* [in] */ Boolean fromStart,
    /* [in] */ IInterface* lo,
    /* [in] */ Boolean loInclusive,
    /* [in] */ Boolean toEnd,
    /* [in] */ IInterface* hi,
    /* [in] */ Boolean hiInclusive,
    /* [in] */ Boolean holdRef)
{
    return NavigableSubMap::Constructor(m, fromStart, lo, loInclusive,
            toEnd, hi, hiInclusive, holdRef);
}

ECode TreeMap::AscendingSubMap::Comparator(
    /* [out] */ IComparator** comparator)
{
    return mMap->Comparator(comparator);
}

ECode TreeMap::AscendingSubMap::SubMap(
    /* [in] */ IInterface* fromKey,
    /* [in] */ Boolean fromInclusive,
    /* [in] */ IInterface* toKey,
    /* [in] */ Boolean toInclusive,
    /* [out] */ INavigableMap** submap)
{
    VALIDATE_NOT_NULL(submap);

    if (fromKey == nullptr) {
        Logger::E("TreeMap::AscendingSubMap", "fromKey == nullptr");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (toKey == nullptr) {
        Logger::E("TreeMap::AscendingSubMap", "toKey == nullptr");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (!InRange(fromKey, fromInclusive)) {
        Logger::E("TreeMap::AscendingSubMap", "fromKey out of range");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (!InRange(toKey, toInclusive)) {
        Logger::E("TreeMap::AscendingSubMap", "toKey out of range");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<AscendingSubMap> map = new AscendingSubMap();
    map->Constructor(mMap, false, fromKey, fromInclusive,
            false, toKey, toInclusive, true);
    *submap = (INavigableMap*)map.Get();
    REFCOUNT_ADD(*submap);
    return NOERROR;
}

ECode TreeMap::AscendingSubMap::HeadMap(
    /* [in] */ IInterface* toKey,
    /* [in] */ Boolean inclusive,
    /* [out] */ INavigableMap** headmap)
{
    VALIDATE_NOT_NULL(headmap);

    if (toKey == nullptr) {
        Logger::E("TreeMap::AscendingSubMap", "toKey == nullptr");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (!InRange(toKey) && !(!mToEnd && (mMap->Compare(toKey, mHi) == 0) &&
            !mHiInclusive && !inclusive)) {
        Logger::E("TreeMap::AscendingSubMap", "toKey out of range");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<AscendingSubMap> map = new AscendingSubMap();
    map->Constructor(mMap, mFromStart, mLo, mLoInclusive,
            false, toKey, inclusive, true);
    *headmap = (INavigableMap*)map.Get();
    REFCOUNT_ADD(*headmap);
    return NOERROR;
}

ECode TreeMap::AscendingSubMap::TailMap(
    /* [in] */ IInterface* fromKey,
    /* [in] */ Boolean inclusive,
    /* [out] */ INavigableMap** tailmap)
{
    VALIDATE_NOT_NULL(tailmap);

    if (fromKey == nullptr) {
        Logger::E("TreeMap::AscendingSubMap", "fromKey == nullptr");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (!InRange(fromKey) && !(!mFromStart && (mMap->Compare(fromKey, mLo) == 0) &&
            !mLoInclusive && !inclusive)) {
        Logger::E("TreeMap::AscendingSubMap", "fromKey out of range");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<AscendingSubMap> map = new AscendingSubMap();
    map->Constructor(mMap, false, fromKey, inclusive,
            mToEnd, mHi, mHiInclusive, true);
    *tailmap = (INavigableMap*)map.Get();
    REFCOUNT_ADD(*tailmap);
    return NOERROR;
}

ECode TreeMap::AscendingSubMap::DescendingMap(
    /* [out] */ INavigableMap** map)
{
    VALIDATE_NOT_NULL(map);

    if (mDescendingMapView == nullptr) {
        AutoPtr<DescendingSubMap> submap = new DescendingSubMap();
        submap->Constructor(mMap, mFromStart, mLo, mLoInclusive,
                mToEnd, mHi, mHiInclusive, mHoldRef);
        mDescendingMapView = (INavigableMap*)submap.Get();
    }
    *map = mDescendingMapView;
    REFCOUNT_ADD(*map);
    return NOERROR;
}

AutoPtr<IIterator> TreeMap::AscendingSubMap::GetKeyIterator()
{
    return new SubMapKeyIterator(AbsLowest(), AbsHighFence(), this);
}

AutoPtr<IIterator> TreeMap::AscendingSubMap::GetDescendingKeyIterator()
{
    return new DescendingSubMapKeyIterator(AbsHighest(), AbsLowFence(), this);
}

ECode TreeMap::AscendingSubMap::GetEntrySet(
    /* [out] */ AutoPtr<ISet>& entries)
{
    if (mEntrySetView == nullptr) {
        mEntrySetView = new AscendingEntrySetView(this);
    }
    entries = (ISet*)mEntrySetView.Get();
    return NOERROR;
}

AutoPtr<TreeMap::TreeMapEntry> TreeMap::AscendingSubMap::SubLowest()
{
    return AbsLowest();
}

AutoPtr<TreeMap::TreeMapEntry> TreeMap::AscendingSubMap::SubHighest()
{
    return AbsHighest();
}

AutoPtr<TreeMap::TreeMapEntry> TreeMap::AscendingSubMap::SubCeiling(
    /* [in] */ IInterface* key)
{
    return AbsCeiling(key);
}

AutoPtr<TreeMap::TreeMapEntry> TreeMap::AscendingSubMap::SubHigher(
    /* [in] */ IInterface* key)
{
    return AbsHigher(key);
}

AutoPtr<TreeMap::TreeMapEntry> TreeMap::AscendingSubMap::SubFloor(
    /* [in] */ IInterface* key)
{
    return AbsFloor(key);
}

AutoPtr<TreeMap::TreeMapEntry> TreeMap::AscendingSubMap::SubLower(
    /* [in] */ IInterface* key)
{
    return AbsLower(key);
}

//--------------------------------------------------------------------------

ECode TreeMap::AscendingSubMap::AscendingEntrySetView::GetIterator(
    /* [out] */ AutoPtr<IIterator>& it)
{
    it = new SubMapEntryIterator(mOwner->AbsLowest(), mOwner->AbsHighFence(), mOwner);
    return NOERROR;
}

//--------------------------------------------------------------------------

ECode TreeMap::DescendingSubMap::Constructor(
    /* [in] */ TreeMap* m,
    /* [in] */ Boolean fromStart,
    /* [in] */ IInterface* lo,
    /* [in] */ Boolean loInclusive,
    /* [in] */ Boolean toEnd,
    /* [in] */ IInterface* hi,
    /* [in] */ Boolean hiInclusive,
    /* [in] */ Boolean holdRef)
{
    mReverseComparator = Collections::ReverseOrder(m->mComparator);

    return NavigableSubMap::Constructor(m, fromStart, lo, loInclusive,
            toEnd, hi, hiInclusive, holdRef);
}

ECode TreeMap::DescendingSubMap::Comparator(
    /* [out] */ IComparator** comparator)
{
    VALIDATE_NOT_NULL(comparator);

    *comparator = mReverseComparator;
    REFCOUNT_ADD(*comparator);
    return NOERROR;
}

ECode TreeMap::DescendingSubMap::SubMap(
    /* [in] */ IInterface* fromKey,
    /* [in] */ Boolean fromInclusive,
    /* [in] */ IInterface* toKey,
    /* [in] */ Boolean toInclusive,
    /* [out] */ INavigableMap** submap)
{
    VALIDATE_NOT_NULL(submap);

    if (fromKey == nullptr) {
        Logger::E("TreeMap::DescendingSubMap", "fromKey == nullptr");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (toKey == nullptr) {
        Logger::E("TreeMap::DescendingSubMap", "toKey == nullptr");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (!InRange(fromKey, fromInclusive)) {
        Logger::E("TreeMap::DescendingSubMap", "fromKey out of range");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (!InRange(toKey, toInclusive)) {
        Logger::E("TreeMap::DescendingSubMap", "toKey out of range");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<DescendingSubMap> map = new DescendingSubMap();
    map->Constructor(mMap, false, toKey, toInclusive,
            false, fromKey, fromInclusive, true);
    *submap = (INavigableMap*)map.Get();
    REFCOUNT_ADD(*submap);
    return NOERROR;
}

ECode TreeMap::DescendingSubMap::HeadMap(
    /* [in] */ IInterface* toKey,
    /* [in] */ Boolean inclusive,
    /* [out] */ INavigableMap** headmap)
{
    VALIDATE_NOT_NULL(headmap);

    if (toKey == nullptr) {
        Logger::E("TreeMap::DescendingSubMap", "toKey == nullptr");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (!InRange(toKey) && !(!mFromStart && mMap->Compare(toKey, mLo) == 0 &&
            !mLoInclusive && !inclusive)) {
        Logger::E("TreeMap::DescendingSubMap", "toKey out of range");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<DescendingSubMap> map = new DescendingSubMap();
    map->Constructor(mMap, false, toKey, inclusive,
            mToEnd, mHi, mHiInclusive, true);
    *headmap = (INavigableMap*)map.Get();
    REFCOUNT_ADD(*headmap);
    return NOERROR;
}

ECode TreeMap::DescendingSubMap::TailMap(
    /* [in] */ IInterface* fromKey,
    /* [in] */ Boolean inclusive,
    /* [out] */ INavigableMap** tailmap)
{
    VALIDATE_NOT_NULL(tailmap);

    if (fromKey == nullptr) {
        Logger::E("TreeMap::DescendingSubMap", "fromKey == nullptr");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (!InRange(fromKey) && !(!mToEnd && mMap->Compare(fromKey, mHi) == 0 &&
            !mHiInclusive && !inclusive)) {
        Logger::E("TreeMap::DescendingSubMap", "fromKey out of range");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<DescendingSubMap> map = new DescendingSubMap();
    map->Constructor(mMap, mFromStart, mLo, mLoInclusive,
            false, fromKey, inclusive, true);
    *tailmap = (INavigableMap*)map.Get();
    REFCOUNT_ADD(*tailmap);
    return NOERROR;
}

ECode TreeMap::DescendingSubMap::DescendingMap(
    /* [out] */ INavigableMap** map)
{
    VALIDATE_NOT_NULL(map);

    if (mDescendingMapView == nullptr) {
        AutoPtr<AscendingSubMap> submap = new AscendingSubMap();
        submap->Constructor(mMap, mFromStart, mLo, mLoInclusive,
                mToEnd, mHi, mHiInclusive, mHoldRef);
        mDescendingMapView = (INavigableMap*)submap.Get();
    }
    *map = mDescendingMapView;
    REFCOUNT_ADD(*map);
    return NOERROR;
}

AutoPtr<IIterator> TreeMap::DescendingSubMap::GetKeyIterator()
{
    return new DescendingSubMapKeyIterator(AbsHighest(), AbsLowFence(), this);
}

AutoPtr<IIterator> TreeMap::DescendingSubMap::GetDescendingKeyIterator()
{
    return new SubMapKeyIterator(AbsLowest(), AbsHighFence(), this);
}

ECode TreeMap::DescendingSubMap::GetEntrySet(
    /* [out] */ AutoPtr<ISet>& entries)
{
    if (mEntrySetView == nullptr) {
        mEntrySetView = new DescendingEntrySetView(this);
    }
    entries = (ISet*)mEntrySetView.Get();
    return NOERROR;
}

AutoPtr<TreeMap::TreeMapEntry> TreeMap::DescendingSubMap::SubLowest()
{
    return AbsHighest();
}

AutoPtr<TreeMap::TreeMapEntry> TreeMap::DescendingSubMap::SubHighest()
{
    return AbsLowest();
}

AutoPtr<TreeMap::TreeMapEntry> TreeMap::DescendingSubMap::SubCeiling(
    /* [in] */ IInterface* key)
{
    return AbsFloor(key);
}

AutoPtr<TreeMap::TreeMapEntry> TreeMap::DescendingSubMap::SubHigher(
    /* [in] */ IInterface* key)
{
    return AbsLower(key);
}

AutoPtr<TreeMap::TreeMapEntry> TreeMap::DescendingSubMap::SubFloor(
    /* [in] */ IInterface* key)
{
    return AbsCeiling(key);
}

AutoPtr<TreeMap::TreeMapEntry> TreeMap::DescendingSubMap::SubLower(
    /* [in] */ IInterface* key)
{
    return AbsHigher(key);
}

//--------------------------------------------------------------------------

ECode TreeMap::DescendingSubMap::DescendingEntrySetView::GetIterator(
    /* [out] */ AutoPtr<IIterator>& it)
{
    it = new DescendingSubMapEntryIterator(mOwner->AbsHighest(), mOwner->AbsLowFence(), mOwner);
    return NOERROR;
}

//--------------------------------------------------------------------------

COMO_INTERFACE_IMPL_2(TreeMap::SubMapInternal, AbstractMap, ISortedMap, ISerializable);

ECode TreeMap::SubMapInternal::GetEntrySet(
    /* [out] */ AutoPtr<ISet>& entries)
{
    return E_INTERNAL_ERROR;
}

ECode TreeMap::SubMapInternal::LastKey(
    /* [out] */ IInterface** key)
{
    return E_INTERNAL_ERROR;
}

ECode TreeMap::SubMapInternal::FirstKey(
    /* [out] */ IInterface** key)
{
    return E_INTERNAL_ERROR;
}

ECode TreeMap::SubMapInternal::SubMap(
    /* [in] */ IInterface* fromKey,
    /* [in] */ IInterface* toKey,
    /* [out] */ ISortedMap** submap)
{
    return E_INTERNAL_ERROR;
}

ECode TreeMap::SubMapInternal::HeadMap(
    /* [in] */ IInterface* toKey,
    /* [out] */ ISortedMap** headmap)
{
    return E_INTERNAL_ERROR;
}

ECode TreeMap::SubMapInternal::TailMap(
    /* [in] */ IInterface* fromKey,
    /* [out] */ ISortedMap** tailmap)
{
    return E_INTERNAL_ERROR;
}

ECode TreeMap::SubMapInternal::Comparator(
    /* [out] */ IComparator** comparator)
{
    return E_INTERNAL_ERROR;
}

//--------------------------------------------------------------------------

COMO_INTERFACE_IMPL_1(TreeMap::TreeMapEntry, SyncObject, IMapEntry);

ECode TreeMap::TreeMapEntry::GetKey(
    /* [out] */ AutoPtr<IInterface>& key)
{
    key = mKey;
    return NOERROR;
}

ECode TreeMap::TreeMapEntry::GetValue(
    /* [out] */ AutoPtr<IInterface>& value)
{
    value = mValue;
    return NOERROR;
}

ECode TreeMap::TreeMapEntry::SetValue(
    /* [in] */ IInterface* value,
    /* [out] */ IInterface** prevValue)
{
    if (prevValue != nullptr) {
        *prevValue = mValue;
        REFCOUNT_ADD(*prevValue);
    }
    mValue = value;
    return NOERROR;
}

ECode TreeMap::TreeMapEntry::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean& result)
{
    IMapEntry* other = IMapEntry::Probe(obj);
    if (other == nullptr) {
        result = false;
        return NOERROR;
    }

    AutoPtr<IInterface> otherKey, otherValue;
    other->GetKey(otherKey);
    other->GetValue(otherValue);
    result = ValEquals(mKey, otherKey) && ValEquals(mValue, otherValue);
    return NOERROR;
}

ECode TreeMap::TreeMapEntry::GetHashCode(
    /* [out] */ Integer& hash)
{
    Integer keyHash = 0;
    Integer valueHash = 0;
    if (IObject::Probe(mKey) != nullptr) {
        IObject::Probe(mKey)->GetHashCode(keyHash);
    }
    if (IObject::Probe(mValue) != nullptr) {
        IObject::Probe(mValue)->GetHashCode(valueHash);
    }
    hash = keyHash ^ valueHash;
    return NOERROR;
}

ECode TreeMap::TreeMapEntry::ToString(
    /* [out] */ String& str)
{
    str = Object::ToString(mKey) + "=" + Object::ToString(mValue);
    return NOERROR;
}

}
}
