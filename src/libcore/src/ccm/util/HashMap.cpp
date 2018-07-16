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

#include "ccm/core/Math.h"
#include "ccm/util/HashMap.h"
#include "ccm/util/TreeNode.h"
#include "ccm.core.IInteger.h"
#include "ccm.util.IIterator.h"
#include <ccmlogger.h>

using ccm::core::IInteger;
using ccm::core::Math;
using ccm::core::IID_ICloneable;
using ccm::io::IID_ISerializable;

namespace ccm {
namespace util {

CCM_INTERFACE_IMPL_3(HashMap, AbstractMap, IHashMap, ICloneable, ISerializable);

Integer HashMap::Hash(
    /* [in] */ IInterface* key)
{
    Integer h;
    if (key == nullptr) {
        return 0;
    }
    h = Object::GetHashCode(key);
    h = h ^ (((unsigned Integer)h) >> 16);
    return h;
}

Integer HashMap::CompareComparables(
    /* [in] */ IInterface* k,
    /* [in] */ IInterface* x)
{
    if (x == nullptr || IComparable::Probe(x) == nullptr) {
        return 0;
    }
    Integer result;
    IComparable::Probe(k)->CompareTo(x, &result);
    return result;
}

Integer HashMap::TableSizeFor(
    /* [in] */ Integer cap)
{
    Integer n = cap - 1;
    n |= ((unsigned Integer)n) >> 1;
    n |= ((unsigned Integer)n) >> 2;
    n |= ((unsigned Integer)n) >> 4;
    n |= ((unsigned Integer)n) >> 8;
    n |= ((unsigned Integer)n) >> 16;
    return (n < 0) ? 1 : (n >= MAXIMUM_CAPACITY) ? MAXIMUM_CAPACITY : n + 1;
}

ECode HashMap::Constructor(
    /* [in] */ Integer initialCapacity,
    /* [in] */ Float loadFactor)
{
    if (initialCapacity < 0) {
        Logger::E("HashMap", "Illegal initial capacity: %d", initialCapacity);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (initialCapacity > MAXIMUM_CAPACITY) {
        initialCapacity = MAXIMUM_CAPACITY;
    }
    if (loadFactor <= 0 || Math::IsNaN(loadFactor)) {
        Logger::E("HashMap", "Illegal load factor: %f", loadFactor);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mLoadFactor = loadFactor;
    mThreshold = TableSizeFor(initialCapacity);
    return NOERROR;
}

ECode HashMap::Constructor(
    /* [in] */ Integer initialCapacity)
{
    return Constructor(initialCapacity, DEFAULT_LOAD_FACTOR);
}

ECode HashMap::Constructor()
{
    mLoadFactor = DEFAULT_LOAD_FACTOR;
    return NOERROR;
}

ECode HashMap::Constructor(
    /* [in] */ IMap* m)
{
    mLoadFactor = DEFAULT_LOAD_FACTOR;
    PutMapEntries(m, false);
    return NOERROR;
}

void HashMap::PutMapEntries(
    /* [in] */ IMap* m,
    /* [in] */ Boolean evict)
{
    Integer s;
    m->GetSize(&s);
    if (s > 0) {
        if (mTable.IsNull()) {
            Float ft = ((Float)s / mLoadFactor) + 1.0f;
            Integer t = ((ft < (Float)MAXIMUM_CAPACITY) ?
                         (Integer)ft : MAXIMUM_CAPACITY);
            if (t > mThreshold) {
                mThreshold = TableSizeFor(t);
            }
        }
        else if (s > mThreshold) {
            Resize();
        }
        AutoPtr<ISet> entries;
        m->GetEntrySet((ISet**)&entries);
        AutoPtr<IIterator> it;
        entries->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> o;
            it->Next((IInterface**)&o);
            IMapEntry* e = IMapEntry::Probe(o);
            AutoPtr<IInterface> key, value;
            e->GetKey((IInterface**)&key);
            e->GetValue((IInterface**)&value);
            PutVal(Hash(key), key, value, false, evict);
        }
    }
}

ECode HashMap::GetSize(
    /* [out] */ Integer* size)
{
    VALIDATE_NOT_NULL(size);

    *size = mSize;
    return NOERROR;
}

ECode HashMap::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mSize == 0;
    return NOERROR;
}

ECode HashMap::Get(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value);

    AutoPtr<Node> e = GetNode(Hash(key), key);
    *value = e == nullptr ? nullptr : e->mValue;
    REFCOUNT_ADD(*value);
    return NOERROR;
}

AutoPtr<HashMap::Node> HashMap::GetNode(
    /* [in] */ Integer hash,
    /* [in] */ IInterface* key)
{
    AutoPtr<Node> first, e;
    Integer n;
    AutoPtr<IInterface> k;
    if (!mTable.IsNull() && (n = mTable.GetLength()) > 0 &&
            (first = mTable[(n - 1) & hash]) != nullptr) {
        if (first->mHash == hash && // always check first node
                ((k = first->mKey, IInterface::Equals(k, key)) ||
                (key != nullptr && Object::Equals(key, k)))) {
            return first;
        }
        if ((e = first->mNext) != nullptr) {
            if (TreeNode::IsTreeNode(first)) {
                return ((TreeNode*)first.Get())->GetTreeNode(hash, key).Get();
            }
            do {
                if (e->mHash == hash &&
                    ((k = e->mKey, IInterface::Equals(k, key)) ||
                    (key != nullptr && Object::Equals(key, k)))) {
                    return e;
                }
            } while ((e = e->mNext) != nullptr);
        }
    }
    return nullptr;
}

ECode HashMap::ContainsKey(
    /* [in] */ IInterface* key,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = GetNode(Hash(key), key) != nullptr;
    return NOERROR;
}

ECode HashMap::Put(
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value,
    /* [out] */ IInterface** prevValue)
{
    AutoPtr<IInterface> v = PutVal(Hash(key), key, value, false, true);
    if (prevValue != nullptr) {
        *prevValue = v;
        REFCOUNT_ADD(*prevValue);
    }
    return NOERROR;
}

AutoPtr<IInterface> HashMap::PutVal(
    /* [in] */ Integer hash,
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value,
    /* [in] */ Boolean onlyIfAbsent,
    /* [in] */ Boolean evict)
{
    Array<Node*>& tab = mTable;
    AutoPtr<Node> p;
    Integer n, i;
    if (tab.IsNull() || (n = tab.GetLength()) == 0) {
        n = (tab = Resize()).GetLength();
    }
    if ((p = tab[i = (n - 1) & hash]) == nullptr) {
        tab.Set(i, NewNode(hash, key, value, nullptr));
    }
    else {
        AutoPtr<Node> e;
        AutoPtr<IInterface> k;
        if (p->mHash == hash && ((k = p->mKey, IInterface::Equals(k, key)) ||
                (key != nullptr && Object::Equals(key, k)))) {
            e = p;
        }
        else if (TreeNode::IsTreeNode(p)) {
            e = ((TreeNode*)p.Get())->PutTreeVal(this, tab, hash, key, value);
        }
        else {
            for (Integer binCount = 0; ; ++binCount) {
                if (e = p->mNext, e == nullptr) {
                    p->mNext = NewNode(hash, key, value, nullptr);
                    if (binCount >= TREEIFY_THRESHOLD - 1) {
                        TreeifyBin(tab, hash);
                    }
                    break;
                }
                if (e->mHash == hash && ((k = e->mKey, IInterface::Equals(k, key)) ||
                        (key != nullptr && Object::Equals(key, k)))) {
                    break;
                }
                p = e;
            }
        }
        if (e != nullptr) {
            AutoPtr<IInterface> oldValue = e->mValue;
            if (!onlyIfAbsent || oldValue == nullptr) {
                e->mValue = value;
            }
            AfterNodeAccess(e);
            return oldValue;
        }
    }
    ++mModCount;
    if (++mSize > mThreshold) {
        Resize();
    }
    AfterNodeInsertion(evict);
    return nullptr;
}

Array<HashMap::Node*> HashMap::Resize()
{
    Array<Node*> oldTab = mTable;
    Integer oldCap = oldTab.IsNull() ? 0 : oldTab.GetLength();
    Integer oldThr = mThreshold;
    Integer newCap, newThr = 0;
    if (oldCap > 0) {
        if (oldCap >= MAXIMUM_CAPACITY) {
            mThreshold = IInteger::MAX_VALUE;
            return oldTab;
        }
        else if ((newCap = oldCap << 1) < MAXIMUM_CAPACITY &&
                oldCap >= DEFAULT_INITIAL_CAPACITY) {
            newThr = oldThr << 1;
        }
    }
    else if (oldThr > 0) {
        newCap = oldThr;
    }
    else {
        newCap = DEFAULT_INITIAL_CAPACITY;
        newThr = (Integer)(DEFAULT_LOAD_FACTOR * DEFAULT_INITIAL_CAPACITY);
    }
    if (newThr == 0) {
        Float ft = (Float)newCap * mLoadFactor;
        newThr = newCap < MAXIMUM_CAPACITY && ft < (Float)MAXIMUM_CAPACITY ?
                (Integer)ft : IInteger::MAX_VALUE;
    }
    mThreshold = newThr;
    Array<Node*> newTab(newCap);
    mTable = newTab;
    if (!oldTab.IsNull()) {
        for (Integer j = 0; j < oldCap; ++j) {
            AutoPtr<Node> e;
            if (e = oldTab[j], e != nullptr) {
                oldTab.Set(j, nullptr);
                if (e->mNext == nullptr) {
                    newTab.Set(e->mHash & (newCap - 1), e);
                }
                else if (TreeNode::IsTreeNode(e)) {
                    ((TreeNode*)e.Get())->Split(this, newTab, j, oldCap);
                }
                else {
                    Node* loHead = nullptr;
                    Node* loTail = nullptr;
                    Node* hiHead = nullptr;
                    Node* hiTail = nullptr;
                    Node* next = nullptr;
                    do {
                        next = e->mNext;
                        if ((e->mHash & oldCap) == 0) {
                            if (loTail == nullptr) {
                                loHead = e;
                            }
                            else {
                                loTail->mNext = e;
                            }
                            loTail = e;
                        }
                        else {
                            if (hiTail == nullptr) {
                                hiHead = e;
                            }
                            else {
                                hiTail->mNext = e;
                            }
                            hiTail = e;
                        }
                    } while (e = next, e != nullptr);
                    if (loTail != nullptr) {
                        loTail->mNext = nullptr;
                        newTab.Set(j, loHead);
                    }
                    if (hiTail != nullptr) {
                        hiTail->mNext = nullptr;
                        newTab.Set(j + oldCap, hiHead);
                    }
                }
            }
        }
    }
    return newTab;
}

void HashMap::TreeifyBin(
    /* [in] */ Array<Node*>& tab,
    /* [in] */ Integer hash)
{
    Integer n, index;
    AutoPtr<Node> e;
    if (tab.IsNull() || (n = tab.GetLength(), n < MIN_TREEIFY_CAPACITY)) {
        Resize();
    }
    else if (e = tab[index = (n - 1) & hash], e != nullptr) {
        AutoPtr<TreeNode> hd, tl;
        do {
            AutoPtr<TreeNode> p = ReplacementTreeNode(e, nullptr);
            if (tl == nullptr) {
                hd = p;
            }
            else {
                p->mPrev = tl;
                tl->mNext = p;
            }
            tl = p;
        } while (e = e->mNext, e != nullptr);
        tab.Set(index, hd);
        if (hd != nullptr) {
            hd->Treeify(tab);
        }
    }
}

ECode HashMap::PutAll(
    /* [in] */ IMap* m)
{
    PutMapEntries(m, true);
    return NOERROR;
}

ECode HashMap::Remove(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** prevValue)
{
    AutoPtr<Node> e = RemoveNode(Hash(key), key, nullptr, false, true);
    if (prevValue != nullptr) {
        *prevValue = e == nullptr ? nullptr : e->mValue;
        REFCOUNT_ADD(*prevValue);
    }
    return NOERROR;
}

AutoPtr<HashMap::Node> HashMap::RemoveNode(
    /* [in] */ Integer hash,
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value,
    /* [in] */ Boolean matchValue,
    /* [in] */ Boolean movable)
{
    Array<Node*>& tab = mTable;
    AutoPtr<Node> p;
    Integer n, index;
    if (!tab.IsNull() && (n = tab.GetLength(), n > 0) &&
            (p = tab[index = (n - 1) & hash], p != nullptr)) {
        AutoPtr<Node> node, e;
        AutoPtr<IInterface> k, v;
        if (p->mHash == hash && ((k = p->mKey, IInterface::Equals(k, key)) ||
                (key != nullptr && Object::Equals(key, k)))) {
            node = p;
        }
        else if (e = p->mNext, e != nullptr) {
            if (TreeNode::IsTreeNode(p)) {
                node = ((TreeNode*)p.Get())->GetTreeNode(hash, key).Get();
            }
            else {
                do {
                    if (e->mHash == hash && ((k = e->mKey, IInterface::Equals(k, key)) ||
                            (key != nullptr && Object::Equals(key, k)))) {
                        node = e;
                        break;
                    }
                    p = e;
                } while (e = e->mNext, e != nullptr);
            }
        }
        if (node != nullptr && (!matchValue || (v = node->mValue, IInterface::Equals(v, value)) ||
                (value != nullptr && Object::Equals(value, v)))) {
            if (TreeNode::IsTreeNode(node)) {
                ((TreeNode*)node.Get())->RemoveTreeNode(this, tab, movable);
            }
            else if (node == p) {
                tab.Set(index, node->mNext);
            }
            else {
                p->mNext = node->mNext;
            }
            ++mModCount;
            --mSize;
            AfterNodeRemoval(node);
            return node;
        }
    }
    return nullptr;
}

ECode HashMap::Clear()
{
    mModCount++;
    if (!mTable.IsNull() && mSize > 0) {
        mSize = 0;
        for (Integer i = 0; i < mTable.GetLength(); ++i) {
            mTable.Set(i, nullptr);
        }
    }
    return NOERROR;
}

ECode HashMap::ContainsValue(
    /* [in] */ IInterface* value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (!mTable.IsNull() && mSize > 0) {
        for (Integer i = 0; i < mTable.GetLength(); ++i) {
            for (AutoPtr<Node> e = mTable[i]; e != nullptr; e = e->mNext) {
                AutoPtr<IInterface> v = e->mValue;
                if (IInterface::Equals(v, value) ||
                        (value != nullptr && Object::Equals(value, v))) {
                    *result = true;
                    return NOERROR;
                }
            }
        }
    }
    *result = false;
    return NOERROR;
}

ECode HashMap::GetKeySet(
    /* [out] */ ISet** keys)
{
    VALIDATE_NOT_NULL(*keys);

    AutoPtr<ISet> ks = mKeySet;
    if (ks == nullptr) {
        ks = new KeySet(this);
        mKeySet = ks;
    }
    *keys = ks;
    REFCOUNT_ADD(*keys);
    return NOERROR;
}

ECode HashMap::GetValues(
    /* [out] */ ICollection** values)
{
    VALIDATE_NOT_NULL(values);

    AutoPtr<ICollection> vs = mValues;
    if (vs == nullptr) {
        vs = new Values(this);
        mValues = vs;
    }
    *values = vs;
    REFCOUNT_ADD(*values);
    return NOERROR;
}

ECode HashMap::GetEntrySet(
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

ECode HashMap::CloneImpl(
    /* [in] */ IHashMap* newObj)
{
    HashMap* clone = (HashMap*)newObj;
    clone->Reinitialize();
    clone->PutMapEntries(this, false);
    return NOERROR;
}

Float HashMap::GetLoadFactor()
{
    return mLoadFactor;
}

Integer HashMap::GetCapacity()
{
    return !mTable.IsNull() ? mTable.GetLength() :
            (mThreshold > 0) ? mThreshold : DEFAULT_INITIAL_CAPACITY;
}

AutoPtr<HashMap::Node> HashMap::NewNode(
    /* [in] */ Integer hash,
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value,
    /* [in] */ Node* next)
{
    return new Node(hash, key, value, next);
}

AutoPtr<HashMap::Node> HashMap::ReplacementNode(
    /* [in] */ Node* p,
    /* [in] */ Node* next)
{
    return new Node(p->mHash, p->mKey, p->mValue, next);
}

AutoPtr<TreeNode> HashMap::NewTreeNode(
    /* [in] */ Integer hash,
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value,
    /* [in] */ Node* next)
{
    return new TreeNode(hash, key, value, next);
}

AutoPtr<TreeNode> HashMap::ReplacementTreeNode(
    /* [in] */ Node* p,
    /* [in] */ Node* next)
{
    return new TreeNode(p->mHash, p->mKey, p->mValue, next);
}

void HashMap::Reinitialize()
{
    mTable.Clear();
    mEntrySet = nullptr;
    mKeySet = nullptr;
    mValues = nullptr;
    mModCount = 0;
    mThreshold = 0;
    mSize = 0;
}

ECode HashMap::AfterNodeAccess(
    /* [in] */ Node* p)
{
    return NOERROR;
}

ECode HashMap::AfterNodeInsertion(
    /* [in] */ Boolean evict)
{
    return NOERROR;
}

ECode HashMap::AfterNodeRemoval(
    /* [in] */ Node* p)
{
    return NOERROR;
}

ECode HashMap::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    return AbstractMap::Equals(obj, result);
}

ECode HashMap::GetHashCode(
    /* [out] */ Integer* hash)
{
    return AbstractMap::GetHashCode(hash);
}

//-------------------------------------------------------------------------

CCM_INTERFACE_IMPL_1(HashMap::Node, Object, IMapEntry);

ECode HashMap::Node::GetKey(
    /* [out] */ IInterface** key)
{
    CHECK(key != nullptr);
    *key = mKey;
    REFCOUNT_ADD(*key);
    return NOERROR;
}

ECode HashMap::Node::GetValue(
    /* [out] */ IInterface** value)
{
    CHECK(value != nullptr);
    *value = mValue;
    REFCOUNT_ADD(*value);
    return NOERROR;
}

ECode HashMap::Node::ToString(
    /* [out] */ String* str)
{
    CHECK(str != nullptr);
    *str = Object::ToString(mKey) + "=" + Object::ToString(mValue);
    return NOERROR;
}

ECode HashMap::Node::GetHashCode(
    /* [out] */ Integer* hash)
{
    CHECK(hash);
    *hash = Object::GetHashCode(mKey) ^ Object::GetHashCode(mValue);
    return NOERROR;
}

ECode HashMap::Node::SetValue(
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

ECode HashMap::Node::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    CHECK(result);
    if (IInterface::Equals(obj, (IMapEntry*)this)) {
        *result = true;
        return NOERROR;
    }
    if (IMapEntry::Probe(obj) != nullptr) {
        IMapEntry* e = IMapEntry::Probe(obj);
        AutoPtr<IInterface> key, value;
        e->GetKey((IInterface**)&key);
        e->GetValue((IInterface**)&value);
        if (Object::Equals(mKey, key) &&
                Object::Equals(mValue, value)) {
            *result = true;
            return NOERROR;
        }
    }
    *result = false;
    return NOERROR;
}

//------------------------------------------------------------------------

ECode HashMap::KeySet::GetSize(
    /* [out] */ Integer* size)
{
    VALIDATE_NOT_NULL(size);

    *size = mOwner->mSize;
    return NOERROR;
}

ECode HashMap::KeySet::Clear()
{
    return mOwner->Clear();
}

ECode HashMap::KeySet::GetIterator(
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it);

    *it = new KeyIterator(mOwner);
    REFCOUNT_ADD(*it);
    return NOERROR;
}

ECode HashMap::KeySet::Contains(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    return mOwner->ContainsKey(obj, result);
}

ECode HashMap::KeySet::Remove(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* contained)
{
    AutoPtr<HashMap::Node> e = mOwner->RemoveNode(
            Hash(obj), obj, nullptr, false, true);
    if (contained != nullptr) {
        *contained = e != nullptr;
    }
    return NOERROR;
}

//----------------------------------------------------------------------

ECode HashMap::Values::GetSize(
    /* [out] */ Integer* size)
{
    VALIDATE_NOT_NULL(size);

    *size = mOwner->mSize;
    return NOERROR;
}

ECode HashMap::Values::Clear()
{
    return mOwner->Clear();
}

ECode HashMap::Values::GetIterator(
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it);

    *it = new ValueIterator(mOwner);
    REFCOUNT_ADD(*it);
    return NOERROR;
}

ECode HashMap::Values::Contains(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    return mOwner->ContainsValue(obj, result);
}

//----------------------------------------------------------------------

ECode HashMap::EntrySet::GetSize(
    /* [out] */ Integer* size)
{
    VALIDATE_NOT_NULL(size);

    *size = mOwner->mSize;
    return NOERROR;
}

ECode HashMap::EntrySet::Clear()
{
    return mOwner->Clear();
}

ECode HashMap::EntrySet::GetIterator(
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it);

    *it = new EntryIterator(mOwner);
    REFCOUNT_ADD(*it);
    return NOERROR;
}

ECode HashMap::EntrySet::Contains(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (IMapEntry::Probe(obj) == nullptr) {
        *result = false;
        return NOERROR;
    }
    IMapEntry* e = IMapEntry::Probe(obj);
    AutoPtr<IInterface> key;
    e->GetKey((IInterface**)&key);
    AutoPtr<Node> candidate = mOwner->GetNode(Hash(key), key);
    *result = candidate != nullptr && Object::Equals((IMapEntry*)candidate, e);
    return NOERROR;
}

ECode HashMap::EntrySet::Remove(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* contained)
{
    if (IMapEntry::Probe(obj) != nullptr) {
        IMapEntry* e = IMapEntry::Probe(obj);
        AutoPtr<IInterface> key, value;
        e->GetKey((IInterface**)&key);
        e->GetKey((IInterface**)&value);
        AutoPtr<Node> node = mOwner->RemoveNode(
                Hash(key), key, value, true, true);
        if (contained != nullptr) {
            *contained = node != nullptr;
        }
        return NOERROR;
    }
    if (contained != nullptr) {
        *contained = false;
    }
    return NOERROR;
}

//----------------------------------------------------------------------

CCM_INTERFACE_IMPL_LIGHT_1(HashMap::HashIterator, IIterator);

HashMap::HashIterator::HashIterator(
    /* [in] */ HashMap* owner)
    : mOwner(owner)
{
    mExpectedModCount = mOwner->mModCount;
    Array<Node*>& t = mOwner->mTable;
    mIndex = 0;
    if (!t.IsNull() && mOwner->mSize > 0) {
        do {}
        while (mIndex < t.GetLength() && (mNext = t[mIndex++], mNext == nullptr));
    }
}

ECode HashMap::HashIterator::HasNext(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mNext != nullptr;
    return NOERROR;
}

ECode HashMap::HashIterator::GetNextNode(
    /* [out] */ Node** node)
{
    if (mOwner->mModCount != mExpectedModCount) {
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }
    AutoPtr<Node> e = mNext;
    if (e == nullptr) {
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }
    Array<Node*>& t = mOwner->mTable;
    if ((mCurrent = e, mNext = e->mNext, mNext == nullptr) && !t.IsNull()) {
        do {}
        while (mIndex < t.GetLength() && (mNext = t[mIndex++], mNext == nullptr));
    }
    *node = e;
    REFCOUNT_ADD(*node);
    return NOERROR;
}

ECode HashMap::HashIterator::Remove()
{
    AutoPtr<Node> p = mCurrent;
    if (p == nullptr) {
        return ccm::core::E_ILLEGAL_STATE_EXCEPTION;
    }
    if (mOwner->mModCount != mExpectedModCount) {
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }
    mCurrent = nullptr;
    AutoPtr<IInterface> key = p->mKey;
    mOwner->RemoveNode(Hash(key), key, nullptr, false, false);
    mExpectedModCount = mOwner->mModCount;
    return NOERROR;
}

//----------------------------------------------------------------------------

HashMap::KeyIterator::KeyIterator(
    /* [in] */ HashMap* owner)
    : HashIterator(owner)
{}

ECode HashMap::KeyIterator::Next(
    /* [out] */ IInterface** object)
{
    AutoPtr<Node> node;
    FAIL_RETURN(GetNextNode((Node**)&node));
    if (object != nullptr) {
        *object = node->mKey;
        REFCOUNT_ADD(*object);
    }
    return NOERROR;
}

//---------------------------------------------------------------------------

HashMap::ValueIterator::ValueIterator(
    /* [in] */ HashMap* owner)
    : HashIterator(owner)
{}

ECode HashMap::ValueIterator::Next(
    /* [out] */ IInterface** object)
{
    AutoPtr<Node> node;
    FAIL_RETURN(GetNextNode((Node**)&node));
    if (object != nullptr) {
        *object = node->mValue;
        REFCOUNT_ADD(*object);
    }
    return NOERROR;
}

//---------------------------------------------------------------------------

HashMap::EntryIterator::EntryIterator(
    /* [in] */ HashMap* owner)
    : HashIterator(owner)
{}

ECode HashMap::EntryIterator::Next(
    /* [out] */ IInterface** object)
{
    AutoPtr<Node> node;
    FAIL_RETURN(GetNextNode((Node**)&node));
    if (object != nullptr) {
        *object = (IMapEntry*)node;
        REFCOUNT_ADD(*object);
    }
    return NOERROR;
}

}
}
