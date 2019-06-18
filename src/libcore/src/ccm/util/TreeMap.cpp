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

#include "ccm/util/TreeMap.h"
#include "ccm.core.IComparable.h"
#include "ccm.io.IObjectInput.h"
#include <ccmlogger.h>

using ccm::core::E_CLASS_CAST_EXCEPTION;
using ccm::core::E_NULL_POINTER_EXCEPTION;
using ccm::core::IComparable;
using ccm::core::IID_ICloneable;
using ccm::io::IID_ISerializable;
using ccm::io::IObjectInput;

namespace ccm {
namespace util {

CCM_INTERFACE_IMPL_5(TreeMap, AbstractMap, ITreeMap, INavigableMap, ISortedMap, ICloneable, ISerializable);

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
    IMap::Probe(m)->GetSize(&size);
    AutoPtr<ISet> entrySet;
    IMap::Probe(m)->GetEntrySet(&entrySet);
    AutoPtr<IIterator> it;
    entrySet->GetIterator(&it);
    BuildFromSorted(size, it, nullptr, nullptr);
    return NOERROR;
}

ECode TreeMap::GetSize(
    /* [out] */ Integer* size)
{
    VALIDATE_NOT_NULL(size);

    *size = mSize;
    return NOERROR;
}

ECode TreeMap::ContainsKey(
    /* [in] */ IInterface* key,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<TreeMapEntry> entry;
    FAIL_RETURN(GetEntry(key, &entry));
    *result = entry != nullptr;
    return NOERROR;
}

ECode TreeMap::ContainsValue(
    /* [in] */ IInterface* value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    for (AutoPtr<TreeMapEntry> e = GetFirstEntry(); e != nullptr; e = Successor(e)) {
        if (ValEquals(value, e->mValue)) {
            *result = true;
            return NOERROR;
        }
    }
    *result = false;
    return NOERROR;
}

ECode TreeMap::Get(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value);

    AutoPtr<TreeMapEntry> entry;
    FAIL_RETURN(GetEntry(key, &entry));
    if (entry == nullptr) {
        *value = nullptr;
    }
    else {
        entry->mValue.MoveTo(value);
    }
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
    map->GetSize(&mapSize);
    if (mSize == 0 && mapSize != 0 && ISortedMap::Probe(map) != nullptr) {
        AutoPtr<IComparator> c;
        ISortedMap::Probe(map)->Comparator(&c);
        if (c == mComparator || (c != nullptr && Object::Equals(c, mComparator))) {
            ++mModCount;
            AutoPtr<ISet> entrySet;
            map->GetEntrySet(&entrySet);
            AutoPtr<IIterator> it;
            entrySet->GetIterator(&it);
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
        k->CompareTo(p->mKey, &cmp);
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
            cpr->Compare(key, p->mKey, &cmp);
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
                FAIL_RETURN(mComparator->Compare(key, key, &cmp));
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
            return NOERROR;
        }
    }
    Integer cmp;
    TreeMapEntry* parent;
    // split comparator and comparable paths
    IComparator* cpr = mComparator;
    if (cpr != nullptr) {
        do {
            parent = t;
            cpr->Compare(key, t->mKey, &cmp);
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
            k->CompareTo(t->mKey, &cmp);
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
    *prevValue = nullptr;
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
    GetEntrySet(&entrySet);
    AutoPtr<IIterator> it;
    entrySet->GetIterator(&it);
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
    /* [out] */ ISet** keys)
{
    VALIDATE_NOT_NULL(keys);

    AutoPtr<INavigableSet> navKeys;
    NavigableKeySet(&navKeys);
    *keys = ISet::Probe(navKeys);
    REFCOUNT_ADD(*keys);
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
    /* [out] */ ICollection** values)
{
    VALIDATE_NOT_NULL(values);

    if (mValues == nullptr) {
        mValues = new Values(this);
    }
    *values = mValues;
    REFCOUNT_ADD(*values);
    return NOERROR;
}

ECode TreeMap::GetEntrySet(
    /* [out] */ ISet** entries)
{
    VALIDATE_NOT_NULL(entries);

    if (mEntrySet == nullptr) {
        mEntrySet = new EntrySet(this);
    }
    *entries = mEntrySet;
    REFCOUNT_ADD(*entries);
    return NOERROR;
}

ECode TreeMap::DescendingMap(
    /* [out] */ INavigableMap** map)
{
    VALIDATE_NOT_NULL(map);

    if (mDescendingMap == nullptr) {
        AutoPtr<DescendingSubMap> m = new DescendingSubMap();
        m->Constructor(this, true, nullptr, true,
                true, nullptr, true);
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
            false, toKey, toInclusive);
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
            false, toKey, inclusive);
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
            true, nullptr, true);
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
    return new KeyIterator(GetFirstEntry());
}

AutoPtr<IIterator> TreeMap::GetDescendingKeyIterator()
{
    return new DescendingKeyIterator(GetLastEntry());
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

    Integer mid = ((unsigned Integer)(lo + hi)) >> 1;

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
            IMapEntry::Probe(entry)->GetKey(&key);
            IMapEntry::Probe(entry)->GetValue(&value);
        }
        else {
            it->Next(&key);
            value = defaultVal;
        }
    }
    else {
        IObjectInput::Probe(str)->ReadObject(&key);
        if (defaultVal != nullptr) {
            value = defaultVal;
        }
        else {
            IObjectInput::Probe(str)->ReadObject(&value);
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
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it);

    *it = new ValueIterator(mOwner->GetFirstEntry());
    REFCOUNT_ADD(*it);
    return NOERROR;
}

ECode TreeMap::Values::GetSize(
    /* [out] */ Integer* size)
{
    return mOwner->GetSize(size);
}

ECode TreeMap::Values::Contains(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
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
        e->GetValue(&v);
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
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it);

    *it = new EntryIterator(mOwner->GetFirstEntry());
    REFCOUNT_ADD(*it);
    return NOERROR;
}

ECode TreeMap::EntrySet::Contains(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    IMapEntry* entry = IMapEntry::Probe(obj);
    if (entry == nullptr) {
        *result = false;
        return NOERROR;
    }
    AutoPtr<IInterface> key, value;
    entry->GetKey(&key);
    entry->GetValue(&value);
    AutoPtr<TreeMapEntry> p;
    FAIL_RETURN(mOwner->GetEntry(key, &p));
    if (p == nullptr) {
        *result = false;
        return NOERROR;
    }
    AutoPtr<IInterface> pv;
    p->GetValue(&pv);
    *result = ValEquals(pv, value);
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
    entry->GetKey(&key);
    entry->GetValue(&value);
    AutoPtr<TreeMapEntry> p;
    FAIL_RETURN(mOwner->GetEntry(key, &p));
    if (p == nullptr) {
        if (contained != nullptr) {
            *contained = false;
            return NOERROR;
        }
    }
    AutoPtr<IInterface> pv;
    p->GetValue(&pv);
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
}

ECode TreeMap::EntrySet::GetSize(
    /* [out] */ Integer* size)
{
    return mOwner->GetSize(size);
}

ECode TreeMap::EntrySet::Clear()
{
    return mOwner->Clear();
}

//--------------------------------------------------------------------------

CCM_INTERFACE_IMPL_2(TreeMap::KeySet, AbstractSet, INavigableSet, ISortedSet);

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
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it);

    if (ITreeMap::Probe(mMap)) {
        ((TreeMap*)mMap)->GetKeyIterator().MoveTo(it);
        return NOERROR;
    }
    else {
        ((TreeMap::NavigableSubMap*)mMap)->GetKeyIterator().MoveTo(it);
        return NOERROR;
    }
}

ECode TreeMap::KeySet::GetDescendingIterator(
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it);

    if (ITreeMap::Probe(mMap)) {
        ((TreeMap*)mMap)->GetDescendingKeyIterator().MoveTo(it);
        return NOERROR;
    }
    else {
        ((TreeMap::NavigableSubMap*)mMap)->GetDescendingKeyIterator().MoveTo(it);
        return NOERROR;
    }
}

ECode TreeMap::KeySet::GetSize(
    /* [out] */ Integer* size)
{
    return IMap::Probe(mMap)->GetSize(size);
}

ECode TreeMap::KeySet::IsEmpty(
    /* [out] */ Boolean* result)
{
    return IMap::Probe(mMap)->IsEmpty(result);
}

ECode TreeMap::KeySet::Contains(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
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
    return entry->GetKey(e);
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
    return entry->GetKey(e);
}

ECode TreeMap::KeySet::Remove(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* contained)
{
    Integer oldSize, newSize;
    GetSize(&oldSize);
    IMap::Probe(mMap)->Remove(obj);
    GetSize(&newSize);
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

//--------------------------------------------------------------------------

CCM_INTERFACE_IMPL_1(TreeMap::TreeMapEntry, SyncObject, IMapEntry);

ECode TreeMap::TreeMapEntry::GetKey(
    /* [out] */ IInterface** key)
{
    VALIDATE_NOT_NULL(key);

    *key = mKey;
    REFCOUNT_ADD(*key);
    return NOERROR;
}

ECode TreeMap::TreeMapEntry::GetValue(
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value);

    *value = mValue;
    REFCOUNT_ADD(*value);
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
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    IMapEntry* other = IMapEntry::Probe(obj);
    if (other == nullptr) {
        *result = false;
        return NOERROR;
    }

    AutoPtr<IInterface> otherKey, otherValue;
    other->GetKey(&otherKey);
    other->GetValue(&otherValue);
    *result = ValEquals(mKey, otherKey) && ValEquals(mValue, otherValue);
    return NOERROR;
}

ECode TreeMap::TreeMapEntry::GetHashCode(
    /* [out] */ Integer* hash)
{
    VALIDATE_NOT_NULL(hash);

    Integer keyHash = 0;
    Integer valueHash = 0;
    if (IObject::Probe(mKey) != nullptr) {
        IObject::Probe(mKey)->GetHashCode(&keyHash);
    }
    if (IObject::Probe(mValue) != nullptr) {
        IObject::Probe(mValue)->GetHashCode(&valueHash);
    }
    *hash = keyHash ^ valueHash;
    return NOERROR;
}

ECode TreeMap::TreeMapEntry::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    *str = Object::ToString(mKey) + "=" + Object::ToString(mValue);
    return NOERROR;
}

}
}
