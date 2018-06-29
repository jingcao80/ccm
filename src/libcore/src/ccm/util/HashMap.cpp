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
#include "ccm.util.IIterator.h"
#include <ccmlogger.h>

using ccm::core::Math;

namespace ccm {
namespace util {

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
            it->GetNext((IInterface**)&o);
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
            // if (first instanceof TreeNode)
            //         return ((TreeNode<K,V>)first).getTreeNode(hash, key);
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

}
}
