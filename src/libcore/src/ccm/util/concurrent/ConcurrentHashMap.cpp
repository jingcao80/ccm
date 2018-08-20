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
#include "ccm/core/NativeAtomic.h"
#include "ccm/util/concurrent/ConcurrentHashMap.h"
#include "ccm.core.IInteger.h"
#include "ccm.util.IIterator.h"
#include "ccm.util.ISet.h"
#include <ccmlogger.h>

using ccm::core::AutoLock;
using ccm::core::IInteger;
using ccm::io::IID_ISerializable;

namespace ccm {
namespace util {
namespace concurrent {

static const InterfaceID IID_ReservationNode =
        {{0xf29f2849,0xecbb,0x42c1,0x9e1e,{0xb,0xe,0x8,0xe,0xf,0xb,0xc,0x6,0xf,0x7,0x8,0xa}}, &CID_libcore};
static const InterfaceID IID_TreeBin =
        {{0xd0cca191,0x9e15,0x48d4,0xb94a,{0xf,0xb,0x2,0x8,0x8,0x9,0x2,0xe,0x4,0x5,0xa,0x9}}, &CID_libcore};

CCM_INTERFACE_IMPL_3(ConcurrentHashMap, SyncObject, IConcurrentHashMap, IMap, ISerializable);

Integer ConcurrentHashMap::TableSizeFor(
    /* [in] */ Integer c)
{
    Integer n = c - 1;
    n |= ((unsigned Integer)n) >> 1;
    n |= ((unsigned Integer)n) >> 2;
    n |= ((unsigned Integer)n) >> 4;
    n |= ((unsigned Integer)n) >> 8;
    n |= ((unsigned Integer)n) >> 16;
    return (n < 0) ? 1 : (n >= MAXIMUM_CAPACITY) ? MAXIMUM_CAPACITY : n + 1;
}

AutoPtr<ConcurrentHashMap::Node> ConcurrentHashMap::TabAt(
    /* [in] */ Array<Node*>& tab,
    /* [in] */ Integer i)
{
    VOLATILE_GET(AutoPtr<Node> node, tab[i]);
    return node;
}

Boolean ConcurrentHashMap::CasTabAt(
    /* [in] */ Array<Node*>& tab,
    /* [in] */ Integer i,
    /* [in] */ Node* c,
    /* [in] */ Node* v)
{
    Boolean success = COMPARE_AND_SWAP_OBJECT_INADDR(&tab[i], c, v);
    if (success) {
        REFCOUNT_ADD(v);
        REFCOUNT_RELEASE(c);
    }
    return success;
}

void ConcurrentHashMap::SetTabAt(
    /* [in] */ Array<Node*>& tab,
    /* [in] */ Integer i,
    /* [in] */ Node* v)
{
    VOLATILE_ARRAY_SET(tab, i, v);
}

ECode ConcurrentHashMap::Constructor(
    /* [in] */ Integer initialCapacity)
{
    if (initialCapacity < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Integer cap = (initialCapacity >= (((unsigned Integer)MAXIMUM_CAPACITY) >> 1)) ?
            MAXIMUM_CAPACITY :
            TableSizeFor(initialCapacity + (((unsigned Integer)initialCapacity) >> 1) + 1);
    mSizeCtl = cap;
    return NOERROR;
}

ECode ConcurrentHashMap::Constructor(
    /* [in] */ IMap* m)
{
    mSizeCtl = DEFAULT_CAPACITY;
    PutAll(m);
    return NOERROR;
}

ECode ConcurrentHashMap::Constructor(
    /* [in] */ Integer initialCapacity,
    /* [in] */ Float loadFactor,
    /* [in] */ Integer concurrencyLevel)
{
    if (loadFactor <= 0.0f || initialCapacity < 0 || concurrencyLevel <= 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (initialCapacity < concurrencyLevel) {
        initialCapacity = concurrencyLevel;
    }
    Long size = (Long)(1.0 + (Long)initialCapacity / loadFactor);
    Integer cap = (size >= (Long)MAXIMUM_CAPACITY) ? MAXIMUM_CAPACITY : TableSizeFor((Integer)size);
    mSizeCtl = cap;
    return NOERROR;
}

ECode ConcurrentHashMap::GetSize(
    /* [out] */ Integer* size)
{
    VALIDATE_NOT_NULL(size);

    Long n = SumCount();
    *size = (n < 0ll) ? 0 :
            (n > (Long)IInteger::MAX_VALUE) ? IInteger::MAX_VALUE :
            (Integer)n;
    return NOERROR;
}

ECode ConcurrentHashMap::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = SumCount() <= 0ll;
    return NOERROR;
}

ECode ConcurrentHashMap::Get(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value);

    Array<Node*> tab;
    AutoPtr<Node> e, p;
    Integer n, eh;
    AutoPtr<IInterface> ek;
    Integer h = Spread(Object::GetHashCode(key));
    VOLATILE_GET(tab, mTable);
    if (!tab.IsNull() && (n = tab.GetLength()) > 0 &&
            (e = TabAt(tab, (n - 1) & h)) != nullptr) {
        if ((eh = e->mHash) == h) {
            if (((ek = e->mKey), IInterface::Equals(ek, key)) ||
                    (ek != nullptr && Object::Equals(key, ek))) {
                VOLATILE_GET(*value, e->mVal);
                REFCOUNT_ADD(*value);
                return NOERROR;
            }
        }
        else if (eh < 0) {
            p = e->Find(h, key);
            if (p != nullptr) {
                VOLATILE_GET(*value, p->mVal);
            }
            else {
                *value = nullptr;
            }
            REFCOUNT_ADD(*value);
            return NOERROR;
        }
        while (true) {
            VOLATILE_GET(e, e->mNext);
            if (e == nullptr) {
                break;
            }
            if (e->mHash == h &&
                    (((ek = e->mKey), IInterface::Equals(ek, key)) ||
                    (ek != nullptr && Object::Equals(key, ek)))) {
                VOLATILE_GET(*value, e->mVal);
                REFCOUNT_ADD(*value);
                return NOERROR;
            }
        }
    }
    *value = nullptr;
    return NOERROR;
}

ECode ConcurrentHashMap::ContainsKey(
    /* [in] */ IInterface* key,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IInterface> v;
    Get(key, (IInterface**)&v);
    *result = v != nullptr;
    return NOERROR;
}

ECode ConcurrentHashMap::ContainsValue(
    /* [in] */ IInterface* value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (value == nullptr) {
        return ccm::core::E_NULL_POINTER_EXCEPTION;
    }
    VOLATILE_GET(Array<Node*> t, mTable);
    if (!t.IsNull()) {
        Traverser it(t, t.GetLength(), 0, t.GetLength());
        for (AutoPtr<Node> p; (p = it.Advance()) != nullptr;) {
            VOLATILE_GET(AutoPtr<IInterface> v, p->mVal);
            if (IInterface::Equals(v, value) ||
                    (v != nullptr && Object::Equals(value ,v))) {
                *result = true;
                return NOERROR;
            }
        }
    }
    *result = false;
    return NOERROR;
}

ECode ConcurrentHashMap::Put(
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value,
    /* [out] */ IInterface** prevValue)
{
    return PutVal(key, value, false, prevValue);
}

ECode ConcurrentHashMap::PutVal(
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value,
    /* [in] */ Boolean onlyIfAbsent,
    /* [out] */ IInterface** prevValue)
{
    if (key == nullptr || value == nullptr) {
        return ccm::core::E_NULL_POINTER_EXCEPTION;
    }
    Integer hash = Spread(Object::GetHashCode(key));
    Integer binCount = 0;
    VOLATILE_GET(Array<Node*> tab, mTable);
    while (true) {
        AutoPtr<Node> f;
        Integer n, i, fh;
        if (tab.IsNull() || (n = tab.GetLength()) == 0) {
            tab = InitTable();
        }
        else if ((f = TabAt(tab, i = (n - 1) & hash)) == nullptr) {
            AutoPtr<Node> newNode = new Node(hash, key, value, nullptr);
            if (CasTabAt(tab, i, nullptr, newNode)) {
                break; // no lock when adding to empty bin
            }
        }
        else if ((fh = f->mHash) == MOVED) {
            tab = HelpTransfer(tab, f);
        }
        else {
            AutoPtr<IInterface> oldVal;
            {
                AutoLock lock(f);
                if (TabAt(tab, i) == f) {
                    if (fh >= 0) {
                        binCount = 1;
                        for (AutoPtr<Node> e = f;; ++binCount) {
                            AutoPtr<IInterface> ek;
                            if (e->mHash == hash &&
                                    (((ek = e->mKey), IInterface::Equals(ek, key)) ||
                                    (ek != nullptr && Object::Equals(key, ek)))) {
                                VOLATILE_GET(oldVal, e->mVal);
                                if (!onlyIfAbsent) {
                                    VOLATILE_SET(e->mVal, value);
                                }
                                break;
                            }
                            AutoPtr<Node> pred = e;
                            VOLATILE_GET(e, e->mNext);
                            if (e == nullptr) {
                                VOLATILE_SET(pred->mNext, new Node(hash, key, value, nullptr));
                                break;
                            }
                        }
                    }
                    else if (f->Probe(IID_TreeBin) != nullptr) {
                        AutoPtr<Node> p;
                        binCount = 2;
                        if ((p = TreeBin::From(f)->PutTreeVal(hash, key, value)) != nullptr) {
                            VOLATILE_GET(oldVal, p->mVal);
                            if (!onlyIfAbsent) {
                                VOLATILE_SET(p->mVal, value);
                            }
                        }
                    }
                    else if (f->Probe(IID_ReservationNode) != nullptr) {
                        Logger::E("ConcurrentHashMap", "Recursive update");
                        return ccm::core::E_ILLEGAL_STATE_EXCEPTION;
                    }
                }
            }
            if (binCount != 0) {
                if (binCount >= TREEIFY_THRESHOLD) {
                    TreeifyBin(tab, i);
                }
                if (oldVal != nullptr) {
                    if (prevValue != nullptr) {
                        *prevValue = oldVal;
                        REFCOUNT_ADD(*prevValue);
                    }
                    return NOERROR;
                }
                break;
            }
        }
    }
    AddCount(1ll, binCount);
    if (prevValue != nullptr) {
        *prevValue = nullptr;
    }
    return NOERROR;
}

ECode ConcurrentHashMap::PutAll(
    /* [in] */ IMap* m)
{
    Integer size;
    m->GetSize(&size);
    TryPresize(size);
    AutoPtr<ISet> entrySet;
    m->GetEntrySet((ISet**)&entrySet);
    FOR_EACH(IMapEntry*, e, IMapEntry::Probe, entrySet) {
        AutoPtr<IInterface> k, v;
        e->GetKey((IInterface**)&k);
        e->GetValue((IInterface**)&v);
        PutVal(k, v, false);
    } END_FOR_EACH();
    return NOERROR;
}

ECode ConcurrentHashMap::Remove(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** prevValue)
{
    return ReplaceNode(key, nullptr, nullptr, prevValue);
}

ECode ConcurrentHashMap::ReplaceNode(
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value,
    /* [in] */ IInterface* cv,
    /* [out] */ IInterface** prevValue)
{
    Integer hash = Spread(Object::GetHashCode(key));
    VOLATILE_GET(Array<Node*> tab, mTable);
    while (true) {
        AutoPtr<Node> f;
        Integer n, i, fh;
        if (tab.IsNull() || (n = tab.GetLength()) == 0 ||
                (f = TabAt(tab, i = (n - 1) & hash)) == nullptr) {
            break;
        }
        else if ((fh = f->mHash) == MOVED) {
            tab = HelpTransfer(tab, f);
        }
        else {
            AutoPtr<IInterface> oldVal;
            Boolean validated = false;
            {
                AutoLock lock(f);
                if (TabAt(tab, i) == f) {
                    if (fh >= 0) {
                        validated = true;
                        for (AutoPtr<Node> e = f, pred;;) {
                            AutoPtr<IInterface> ek;
                            if (e->mHash == hash &&
                                    (((ek = e->mKey), IInterface::Equals(ek, key)) ||
                                    (ek != nullptr && Object::Equals(key, ek)))) {
                                VOLATILE_GET(AutoPtr<IInterface> ev, e->mVal);
                                if (cv == nullptr || IInterface::Equals(cv, ev) ||
                                    (ev != nullptr && Object::Equals(cv, ev))) {
                                    oldVal = ev;
                                    if (value != nullptr) {
                                        VOLATILE_SET(e->mVal, value);
                                    }
                                    else if (pred != nullptr) {
                                        VOLATILE_GET(AutoPtr<Node> next, e->mNext);
                                        VOLATILE_SET(pred->mNext, next);
                                    }
                                    else {
                                        VOLATILE_GET(AutoPtr<Node> next, e->mNext);
                                        SetTabAt(tab, i, next);
                                    }
                                }
                                break;
                            }
                            pred = e;
                            VOLATILE_GET(e, e->mNext);
                            if (e == nullptr) {
                                break;
                            }
                        }
                    }
                    else if (f->Probe(IID_TreeBin) != nullptr) {
                        validated = true;
                        TreeBin* t = TreeBin::From(f);
                        AutoPtr<TreeNode> r, p;
                        if ((r = t->mRoot) != nullptr &&
                                (p = r->FindTreeNode(hash, key, false)) != nullptr) {
                            VOLATILE_GET(AutoPtr<IInterface> pv, p->mVal);
                            if (cv == nullptr || IInterface::Equals(cv, pv) ||
                                    (pv != nullptr && Object::Equals(cv, pv))) {
                                oldVal = pv;
                                if (value != nullptr) {
                                    VOLATILE_SET(p->mVal, value);
                                }
                                else if (t->RemoveTreeNode(p)) {
                                    VOLATILE_GET(AutoPtr<TreeNode> node, t->mFirst);
                                    SetTabAt(tab, i, Untreeify(node));
                                }
                            }
                        }
                    }
                    else if (f->Probe(IID_ReservationNode) != nullptr) {
                        Logger::E("ConcurrentHashMap", "Recursive update");
                        return ccm::core::E_ILLEGAL_STATE_EXCEPTION;
                    }
                }
            }
            if (validated) {
                if (oldVal != nullptr) {
                    if (value == nullptr) {
                        AddCount(-1ll, -1);
                    }
                    if (prevValue != nullptr) {
                        *prevValue = oldVal;
                        REFCOUNT_ADD(*prevValue);
                    }
                    return NOERROR;
                }
                break;
            }
        }
    }
    if (prevValue != nullptr) {
        *prevValue = nullptr;
    }
    return NOERROR;
}

ECode ConcurrentHashMap::Clear()
{
    Long delta = 0ll; // negative number of deletions
    Integer i = 0;
    VOLATILE_GET(Array<Node*> tab, mTable);
    while (!tab.IsNull() && i < tab.GetLength()) {
        Integer fh;
        AutoPtr<Node> f = TabAt(tab, i);
        if (f == nullptr) {
            ++i;
        }
        else if ((fh = f->mHash) == MOVED) {
            tab = HelpTransfer(tab, f);
            i = 0; // restart
        }
        else {
            AutoLock lock(f);
            if (TabAt(tab, i) == f) {
                AutoPtr<Node> p;
                if (fh >= 0) {
                    p = f;
                }
                else if (f->Probe(IID_TreeBin) != nullptr) {
                    VOLATILE_GET(p, TreeBin::From(f)->mFirst);
                }
                while (p != nullptr) {
                    --delta;
                    VOLATILE_GET(p, p->mNext);
                }
                SetTabAt(tab, i++, nullptr);
            }
        }
    }
    if (delta != 0ll) {
        AddCount(delta, -1);
    }
}

ECode ConcurrentHashMap::GetKeySet(
    /* [out] */ ISet** keys)
{
    VALIDATE_NOT_NULL(keys);

    if (mKeySet == nullptr) {
        mKeySet = new KeySetView(this, nullptr);
    }
    *keys = mKeySet.Get();
    REFCOUNT_ADD(*keys);
    return NOERROR;
}

//-------------------------------------------------------------------------

}
}
}
