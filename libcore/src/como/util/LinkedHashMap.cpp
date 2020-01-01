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

#include "como/util/LinkedHashMap.h"
#include "como/util/TreeNode.h"

namespace como {
namespace util {

COMO_INTERFACE_IMPL_1(LinkedHashMap, HashMap, ILinkedHashMap);

ECode LinkedHashMap::Constructor(
    /* [in] */ Integer initialCapacity,
    /* [in] */ Float loadFactor)
{
    FAIL_RETURN(HashMap::Constructor(initialCapacity, loadFactor));
    mAccessOrder = false;
    return NOERROR;
}

ECode LinkedHashMap::Constructor(
    /* [in] */ Integer initialCapacity)
{
    FAIL_RETURN(HashMap::Constructor(initialCapacity));
    mAccessOrder = false;
    return NOERROR;
}

ECode LinkedHashMap::Constructor()
{
    FAIL_RETURN(HashMap::Constructor());
    mAccessOrder = false;
    return NOERROR;
}

ECode LinkedHashMap::Constructor(
    /* [in] */ IMap* m)
{
    FAIL_RETURN(HashMap::Constructor());
    mAccessOrder = false;
    PutMapEntries(m, false);
    return NOERROR;
}

ECode LinkedHashMap::Constructor(
    /* [in] */ Integer initialCapacity,
    /* [in] */ Float loadFactor,
    /* [in] */ Boolean accessOrder)
{
    FAIL_RETURN(HashMap::Constructor(initialCapacity, loadFactor));
    mAccessOrder = accessOrder;
    return NOERROR;
}

void LinkedHashMap::LinkNodeLast(
    /* [in] */ LinkedHashMapEntry* p)
{
    AutoPtr<LinkedHashMapEntry> last = mTail;
    mTail = p;
    if (last == nullptr) {
        mHead = p;
    }
    else {
        p->mBefore = last;
        last->mAfter = p;
    }
}

void LinkedHashMap::TransferLinks(
    /* [in] */ LinkedHashMapEntry* src,
    /* [in] */ LinkedHashMapEntry* dst)
{
    LinkedHashMapEntry* b = dst->mBefore = src->mBefore;
    LinkedHashMapEntry* a = dst->mAfter = src->mAfter;
    if (b == nullptr) {
        mHead = dst;
    }
    else {
        b->mAfter = dst;
    }
    if (a == nullptr) {
        mTail = dst;
    }
    else {
        a->mBefore = dst;
    }
}

void LinkedHashMap::Reinitialize()
{
    HashMap::Reinitialize();
    mHead = mTail = nullptr;
}

AutoPtr<HashMap::Node> LinkedHashMap::NewNode(
    /* [in] */ Integer hash,
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value,
    /* [in] */ Node* next)
{
    AutoPtr<LinkedHashMapEntry> p = new LinkedHashMapEntry(
            hash, key, value, next);
    LinkNodeLast(p);
    return p.Get();
}

AutoPtr<HashMap::Node> LinkedHashMap::ReplacementNode(
    /* [in] */ Node* p,
    /* [in] */ Node* next)
{
    LinkedHashMapEntry* q = (LinkedHashMapEntry*)p;
    AutoPtr<LinkedHashMapEntry> t = new LinkedHashMapEntry(
            q->mHash, q->mKey, q->mValue, next);
    TransferLinks(q, t);
    return t.Get();
}

AutoPtr<TreeNode> LinkedHashMap::NewTreeNode(
    /* [in] */ Integer hash,
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value,
    /* [in] */ Node* next)
{
    AutoPtr<TreeNode> p = new TreeNode(hash, key, value, next);
    LinkNodeLast(p);
    return p;
}

AutoPtr<TreeNode> LinkedHashMap::ReplacementTreeNode(
    /* [in] */ Node* p,
    /* [in] */ Node* next)
{
    LinkedHashMapEntry* q = (LinkedHashMapEntry*)p;
    AutoPtr<TreeNode> t = new TreeNode(q->mHash, q->mKey, q->mValue, next);
    TransferLinks(q, t);
    return t;
}

ECode LinkedHashMap::AfterNodeRemoval(
    /* [in] */ Node* e)
{
    LinkedHashMapEntry* p = (LinkedHashMapEntry*)e;
    LinkedHashMapEntry* b = p->mBefore;
    LinkedHashMapEntry* a = p->mAfter;
    p->mBefore = p->mAfter = nullptr;
    if (b == nullptr) {
        mHead = a;
    }
    else {
        b->mAfter = a;
    }
    if (a == nullptr) {
        mTail = b;
    }
    else {
        a->mBefore = b;
    }
    return NOERROR;
}

ECode LinkedHashMap::AfterNodeInsertion(
    /* [in] */ Boolean evict)
{
    LinkedHashMapEntry* first;
    if (evict && (first = mHead, first != nullptr) &&
            RemoveEldestEntry(first)) {
        IInterface* key = first->mKey;
        RemoveNode(Hash(key), key, nullptr, false, true);
    }
    return NOERROR;
}

ECode LinkedHashMap::AfterNodeAccess(
    /* [in] */ HashMap::Node* e)
{
    LinkedHashMapEntry* last;
    if (mAccessOrder && (last = mTail, last != (LinkedHashMapEntry*)e)) {
        LinkedHashMapEntry* p = (LinkedHashMapEntry*)e;
        LinkedHashMapEntry* b = p->mBefore;
        LinkedHashMapEntry* a = p->mAfter;
        p->mAfter = nullptr;
        if (b == nullptr) {
            mHead = a;
        }
        else {
            b->mAfter = a;
        }
        if (a != nullptr) {
            a->mBefore = b;
        }
        else {
            last = b;
        }
        if (last == nullptr) {
            mHead = p;
        }
        else {
            p->mBefore = last;
            last->mAfter = p;
        }
        mTail = p;
        ++mModCount;
    }
    return NOERROR;
}

ECode LinkedHashMap::ContainsValue(
    /* [in] */ IInterface* value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    for (LinkedHashMapEntry* e = mHead; e != nullptr; e = e->mAfter) {
        IInterface* v = e->mValue;
        if (IInterface::Equals(v, value) ||
                (value != nullptr && Object::Equals(v, value))) {
            *result = true;
            return NOERROR;
        }
    }
    *result = false;
    return NOERROR;
}

ECode LinkedHashMap::Get(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value);

    AutoPtr<Node> e = GetNode(Hash(key), key);
    if (e == nullptr) {
        *value = nullptr;
        return NOERROR;
    }
    if (mAccessOrder) {
        AfterNodeAccess(e);
    }
    *value = e->mValue;
    REFCOUNT_ADD(*value);
    return NOERROR;
}

ECode LinkedHashMap::Clear()
{
    HashMap::Clear();
    mHead = mTail = nullptr;
    return NOERROR;
}

ECode LinkedHashMap::GetEldest(
    /* [out] */ IMapEntry** entry)
{
    VALIDATE_NOT_NULL(entry);

    *entry = mHead.Get();
    REFCOUNT_ADD(*entry);
    return NOERROR;
}

Boolean LinkedHashMap::RemoveEldestEntry(
    /* [in] */ IMapEntry* eldest)
{
    return false;
}

ECode LinkedHashMap::GetKeySet(
    /* [out] */ ISet** keys)
{
    VALIDATE_NOT_NULL(keys);

    if (mKeySet == nullptr) {
        mKeySet = new LinkedKeySet(this);
    }
    *keys = mKeySet;
    REFCOUNT_ADD(*keys);
    return NOERROR;
}

ECode LinkedHashMap::GetValues(
    /* [out] */ ICollection** values)
{
    VALIDATE_NOT_NULL(values);

    if (mValues == nullptr) {
        mValues = new LinkedValues(this);
    }
    *values = mValues;
    REFCOUNT_ADD(*values);
    return NOERROR;
}

ECode LinkedHashMap::GetEntrySet(
    /* [out] */ ISet** entries)
{
    VALIDATE_NOT_NULL(entries);

    if (mEntrySet == nullptr) {
        mEntrySet = new LinkedEntrySet(this);
    }
    *entries = mEntrySet;
    REFCOUNT_ADD(*entries);
    return NOERROR;
}

ECode LinkedHashMap::CloneImpl(
    /* [in] */ ILinkedHashMap* newObj)
{
    LinkedHashMap* clone = (LinkedHashMap*)newObj;
    HashMap::CloneImpl(clone);
    clone->mHead = mHead;
    clone->mTail = mTail;
    clone->mAccessOrder = mAccessOrder;
    return NOERROR;
}

//--------------------------------------------------------------------------

ECode LinkedHashMap::LinkedKeySet::GetSize(
    /* [out] */ Integer* size)
{
    VALIDATE_NOT_NULL(size);

    *size = mOwner->mSize;
    return NOERROR;
}

ECode LinkedHashMap::LinkedKeySet::Clear()
{
    return mOwner->Clear();
}

ECode LinkedHashMap::LinkedKeySet::GetIterator(
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it);

    *it = new LinkedKeyIterator(mOwner);
    REFCOUNT_ADD(*it);
    return NOERROR;
}

ECode LinkedHashMap::LinkedKeySet::Contains(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    return mOwner->ContainsKey(obj, result);
}

ECode LinkedHashMap::LinkedKeySet::Remove(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* contained)
{
    AutoPtr<Node> node = mOwner->RemoveNode(
            Hash(obj), obj, nullptr, false, true);
    if (contained != nullptr) *contained = node != nullptr;
    return NOERROR;
}

//--------------------------------------------------------------------------

ECode LinkedHashMap::LinkedValues::GetSize(
    /* [out] */ Integer* size)
{
    VALIDATE_NOT_NULL(size);

    *size = mOwner->mSize;
    return NOERROR;
}

ECode LinkedHashMap::LinkedValues::Clear()
{
    return mOwner->Clear();
}

ECode LinkedHashMap::LinkedValues::GetIterator(
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it);

    *it = new LinkedValueIterator(mOwner);
    REFCOUNT_ADD(*it);
    return NOERROR;
}

ECode LinkedHashMap::LinkedValues::Contains(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    return mOwner->ContainsValue(obj, result);
}

//--------------------------------------------------------------------------

ECode LinkedHashMap::LinkedEntrySet::GetSize(
    /* [out] */ Integer* size)
{
    VALIDATE_NOT_NULL(size);

    *size = mOwner->mSize;
    return NOERROR;
}

ECode LinkedHashMap::LinkedEntrySet::Clear()
{
    return mOwner->Clear();
}

ECode LinkedHashMap::LinkedEntrySet::GetIterator(
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it);

    *it = new LinkedEntryIterator(mOwner);
    REFCOUNT_ADD(*it);
    return NOERROR;
}

ECode LinkedHashMap::LinkedEntrySet::Contains(
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
    e->GetKey(&key);
    AutoPtr<Node> candiate = mOwner->GetNode(Hash(key), key);
    *result = candiate != nullptr && Object::Equals((IMapEntry*)candiate.Get(), e);
    return NOERROR;
}

ECode LinkedHashMap::LinkedEntrySet::Remove(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* contained)
{
    AutoPtr<Node> node = mOwner->RemoveNode(
            Hash(obj), obj, nullptr, false, true);
    if (contained != nullptr) *contained = node != nullptr;
    return NOERROR;
}

//--------------------------------------------------------------------------

COMO_INTERFACE_IMPL_LIGHT_1(LinkedHashMap::LinkedHashIterator, LightRefBase, IIterator);

ECode LinkedHashMap::LinkedHashIterator::HasNext(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mNext != nullptr;
    return NOERROR;
}

ECode LinkedHashMap::LinkedHashIterator::GetNextNode(
    /* [out] */ LinkedHashMapEntry** node)
{
    AutoPtr<LinkedHashMapEntry> e = mNext;
    if (mOwner->mModCount != mExpectedModCount) {
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }
    if (e == nullptr) {
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }
    mCurrent = e;
    mNext = e->mAfter;
    e.MoveTo(node);
    return NOERROR;
}

ECode LinkedHashMap::LinkedHashIterator::Remove()
{
    AutoPtr<Node> p = mCurrent.Get();
    if (p == nullptr) {
        return como::core::E_ILLEGAL_STATE_EXCEPTION;
    }
    if (mOwner->mModCount != mExpectedModCount) {
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }
    mCurrent = nullptr;
    IInterface* key = p->mKey;
    mOwner->RemoveNode(Hash(key), key, nullptr, false, false);
    mExpectedModCount = mOwner->mModCount;
    return NOERROR;
}

//--------------------------------------------------------------------------

ECode LinkedHashMap::LinkedKeyIterator::Next(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    AutoPtr<LinkedHashMapEntry> node;
    FAIL_RETURN(GetNextNode(&node));
    return node->GetKey(object);
}

//--------------------------------------------------------------------------

ECode LinkedHashMap::LinkedValueIterator::Next(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    AutoPtr<LinkedHashMapEntry> node;
    FAIL_RETURN(GetNextNode(&node));
    return node->GetValue(object);
}

//--------------------------------------------------------------------------

ECode LinkedHashMap::LinkedEntryIterator::Next(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    AutoPtr<LinkedHashMapEntry> node;
    FAIL_RETURN(GetNextNode(&node));
    *object = (IMapEntry*)node.Get();
    return NOERROR;
}

}
}
