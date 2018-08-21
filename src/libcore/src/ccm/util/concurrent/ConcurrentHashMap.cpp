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

#include "coredef.h"
#include "ccm/core/AutoLock.h"
#include "ccm/core/CStringBuilder.h"
#include "ccm/core/CThread.h"
#include "ccm/core/Math.h"
#include "ccm/core/NativeAtomic.h"
#include "ccm/util/concurrent/ConcurrentHashMap.h"
#include "ccm/util/concurrent/ThreadLocalRandom.h"
#include "ccm.core.IInteger.h"
#include "ccm.core.IStringBuilder.h"
#include "ccm.util.IIterator.h"
#include "ccm.util.ISet.h"
#include <ccmlogger.h>

using ccm::core::AutoLock;
using ccm::core::CStringBuilder;
using ccm::core::CThread;
using ccm::core::IInteger;
using ccm::core::IStringBuilder;
using ccm::core::IID_IStringBuilder;
using ccm::core::Math;
using ccm::io::IID_ISerializable;

namespace ccm {
namespace util {
namespace concurrent {

static const InterfaceID IID_ForwardingNode =
        {{0x60a47a03,0x2ec4,0x48ed,0x9e61,{0x7,0x3,0x7,0xd,0xc,0xe,0xe,0x3,0x1,0xb,0x0,0x2}}, &CID_libcore};
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

ECode ConcurrentHashMap::GetValues(
    /* [out] */ ICollection** values)
{
    VALIDATE_NOT_NULL(values);

    if (mValues == nullptr) {
        mValues = new ValuesView(this);
    }
    *values = mValues.Get();
    REFCOUNT_ADD(*values);
    return NOERROR;
}

ECode ConcurrentHashMap::GetEntrySet(
    /* [out] */ ISet** entries)
{
    VALIDATE_NOT_NULL(entries);

    if (mEntrySet == nullptr) {
        mEntrySet = new EntrySetView(this);
    }
    *entries = mEntrySet;
    REFCOUNT_ADD(*entries);
    return NOERROR;
}

ECode ConcurrentHashMap::GetHashCode(
    /* [out] */ Integer* hash)
{
    VALIDATE_NOT_NULL(hash);

    Integer h = 0;
    VOLATILE_GET(Array<Node*>t, mTable);
    if (!t.IsNull()) {
        Traverser it(t, t.GetLength(), 0, t.GetLength());
        for (AutoPtr<Node> p; (p = it.Advance()) != nullptr;) {
            VOLATILE_GET(IInterface* val, p->mVal);
            h += Object::GetHashCode(p->mKey) ^ Object::GetHashCode(val);
        }
    }
    *hash = h;
    return NOERROR;
}

ECode ConcurrentHashMap::ToString(
    /* [out] */ String* desc)
{
    VALIDATE_NOT_NULL(desc);

    VOLATILE_GET(Array<Node*> t, mTable);
    Integer f = t.IsNull() ? 0 : t.GetLength();
    Traverser it(t, f, 0, f);
    AutoPtr<IStringBuilder> sb;
    CStringBuilder::New(IID_IStringBuilder, (IInterface**)&sb);
    sb->AppendChar('{');
    AutoPtr<Node> p;
    if ((p = it.Advance()) != nullptr) {
        for (;;) {
            AutoPtr<IInterface> k = p->mKey;
            VOLATILE_GET(AutoPtr<IInterface> v, p->mVal);
            sb->Append(IInterface::Equals(k, (IMap*)this) ?
                    String("(this Map)") : Object::ToString(k));
            sb->AppendChar('=');
            sb->Append(IInterface::Equals(v, (IMap*)this) ?
                    String("(this Map)") : Object::ToString(v));
            if ((p = it.Advance()) == nullptr) {
                break;
            }
            sb->AppendChar(',');
            sb->AppendChar(' ');
        }
    }
    sb->AppendChar('}');
    return sb->ToString(desc);
}

ECode ConcurrentHashMap::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (IInterface::Equals(obj, (IMap*)this)) {
        if (IMap::Probe(obj) == nullptr) {
            *result = false;
            return NOERROR;
        }
        IMap* m = IMap::Probe(obj);
        VOLATILE_GET(Array<Node*> t, mTable);
        Integer f = t.IsNull() ? 0 : t.GetLength();
        Traverser it(t, f, 0, f);
        for (AutoPtr<Node> p; (p = it.Advance()) != nullptr;) {
            VOLATILE_GET(AutoPtr<IInterface> val, p->mVal);
            AutoPtr<IInterface> v;
            m->Get(p->mKey, (IInterface**)&v);
            if (v == nullptr || (!IInterface::Equals(v, val) && !Object::Equals(v, val))) {
                *result = false;
                return NOERROR;
            }
        }
        AutoPtr<ISet> entrySet;
        m->GetEntrySet((ISet**)&entrySet);
        FOR_EACH(IMapEntry*, e, IMapEntry::Probe, entrySet) {
            AutoPtr<IInterface> mk, mv, v;
            e->GetKey((IInterface**)&mk);
            if (mk == nullptr) {
                *result = false;
                return NOERROR;
            }
            e->GetValue((IInterface**)&mv);
            if (mv == nullptr) {
                *result = false;
                return NOERROR;
            }
            Get(mk, (IInterface**)&v);
            if (v == nullptr || (!IInterface::Equals(mv ,v) && !Object::Equals(mv, v))) {
                *result = false;
                return NOERROR;
            }
        } END_FOR_EACH();
    }
    *result = true;
    return NOERROR;
}

ECode ConcurrentHashMap::PutIfAbsent(
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value,
    /* [out] */ IInterface** prevValue)
{
    return PutVal(key, value, true, prevValue);
}

ECode ConcurrentHashMap::Remove(
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value,
    /* [out] */ Boolean* result)
{
    if (key == nullptr) {
        return ccm::core::E_NULL_POINTER_EXCEPTION;
    }
    if (value == nullptr) {
        if (result != nullptr) {
            *result = false;
            return NOERROR;
        }
    }
    AutoPtr<IInterface> prevValue;
    FAIL_RETURN(ReplaceNode(key, nullptr, value, (IInterface**)&prevValue));
    *result = prevValue != nullptr;
    return NOERROR;
}

ECode ConcurrentHashMap::Replace(
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* oldValue,
    /* [in] */ IInterface* newValue,
    /* [out] */ Boolean* result)
{
    if (key == nullptr || oldValue == nullptr || newValue == nullptr) {
        return ccm::core::E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<IInterface> prevValue;
    FAIL_RETURN(ReplaceNode(key, newValue, oldValue, (IInterface**)&prevValue));
    *result = prevValue != nullptr;
    return NOERROR;
}

ECode ConcurrentHashMap::Replace(
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value,
    /* [out] */ IInterface** prevValue)
{
    if (key == nullptr || value == nullptr) {
        return ccm::core::E_NULL_POINTER_EXCEPTION;
    }
    return ReplaceNode(key, value, nullptr, prevValue);
}

ECode ConcurrentHashMap::Contains(
    /* [in] */ IInterface* value,
    /* [out] */ Boolean* result)
{
    return ContainsValue(value, result);
}

ECode ConcurrentHashMap::Keys(
    /* [out] */ IEnumeration** keys)
{
    VALIDATE_NOT_NULL(keys);

    VOLATILE_GET(Array<Node*> t, mTable);
    Integer f = t.IsNull() ? 0 : t.GetLength();
    *keys = new KeyIterator(t, f, 0, f, this);
    REFCOUNT_ADD(*keys);
    return NOERROR;
}

ECode ConcurrentHashMap::Elements(
    /* [out] */ IEnumeration** elements)
{
    VALIDATE_NOT_NULL(elements);

    VOLATILE_GET(Array<Node*> t, mTable);
    Integer f = t.IsNull() ? 0 : t.GetLength();
    *elements = new ValueIterator(t, f, 0, f, this);
    REFCOUNT_ADD(*elements);
    return NOERROR;
}

ECode ConcurrentHashMap::GetKeySet(
    /* [in] */ IInterface* mappedValue,
    /* [out] */ ISet** keys)
{
    VALIDATE_NOT_NULL(keys);

    if (mappedValue == nullptr) {
        return ccm::core::E_NULL_POINTER_EXCEPTION;
    }
    *keys = new KeySetView(this, mappedValue);
    REFCOUNT_ADD(*keys);
    return NOERROR;
}

Integer ConcurrentHashMap::ResizeStamp(
    /* [in] */ Integer n)
{
    return Math::NumberOfLeadingZeros(0) | (1 << (RESIZE_STAMP_BITS - 1));
}

Array<ConcurrentHashMap::Node*> ConcurrentHashMap::InitTable()
{
    Array<Node*> tab;
    Integer sc;
    while (true) {
        VOLATILE_GET(tab, mTable);
        if (tab.GetLength() > 0) {
            break;
        }
        VOLATILE_GET(sc, mSizeCtl);
        if (sc < 0) {
            CThread::Yield(); // lost initialization race; just spin
        }
        else if (COMPARE_AND_SWAP_INT(this, mSizeCtl, sc, -1)) {
            VOLATILE_GET(tab, mTable);
            if (tab.IsNull() || tab.GetLength() == 0) {
                Integer n = (sc > 0) ? sc : DEFAULT_CAPACITY;
                tab = Array<Node*>(n);
                VOLATILE_SET(mTable, tab);
                sc = n - (((unsigned Integer)n) >> 2);
            }
            VOLATILE_SET(mSizeCtl, sc);
            break;
        }
    }
    return tab;
}

void ConcurrentHashMap::AddCount(
    /* [in] */ Long x,
    /* [in] */ Integer check)
{
    Array<CounterCell*> as;
    Long b, s = 0;
    VOLATILE_GET(as, mCounterCells);
    if (!as.IsNull() ||
            !COMPARE_AND_SWAP_LONG(this, mBaseCount, b = mBaseCount, s = b + x)) {
        AutoPtr<CounterCell> a;
        Long v;
        Integer m;
        Boolean uncontended = true;
        if (as.IsNull() || (m = as.GetLength() - 1) < 0 ||
                (a = as[ThreadLocalRandom::GetProbe() & m]) == nullptr ||
                (VOLATILE_GET_INLINE(v, a->mValue),
                    !(uncontended = COMPARE_AND_SWAP_LONG(a, mValue, v, v + x)))) {
            FullAddCount(x, uncontended);
            return;
        }
        if (check <= 1) {
            return;
        }
        s = SumCount();
    }
    if (check > 0) {
        Array<Node*> tab, nt;
        Integer n, sc;
        while ((VOLATILE_GET_INLINE(sc, mSizeCtl), s >= sc) &&
                (VOLATILE_GET_INLINE(tab, mTable), !tab.IsNull()) &&
                (n = tab.GetLength()) < MAXIMUM_CAPACITY) {
            Integer rs = ResizeStamp(n);
            if (sc < 0) {
                Integer transferIndex;
                if ((((unsigned Integer)sc) >> RESIZE_STAMP_SHIFT) != rs || sc == rs + 1 ||
                        sc == rs + MAX_RESIZERS || (VOLATILE_GET_INLINE(nt, mNextTable), nt.IsNull()) ||
                        (VOLATILE_GET_INLINE(transferIndex, mTransferIndex), transferIndex <= 0)) {
                    break;
                }
                if (COMPARE_AND_SWAP_INT(this, mSizeCtl, sc, sc + 1)) {
                    Transfer(tab, &nt);
                }
            }
            else if (COMPARE_AND_SWAP_INT(this, mSizeCtl, sc,
                    (rs << RESIZE_STAMP_SHIFT) + 2)) {
                Transfer(tab, nullptr);
            }
            s = SumCount();
        }
    }
}

Array<ConcurrentHashMap::Node*> ConcurrentHashMap::HelpTransfer(
    /* [in] */ Array<Node*>& tab,
    /* [in] */ Node* f)
{
    Array<Node*> nextTab;
    Integer sc;
    if (!tab.IsNull() && f->Probe(IID_ForwardingNode) != nullptr &&
            (nextTab = ((ForwardingNode*)f)->mNextTable, !nextTab.IsNull())) {
        Integer rs = ResizeStamp(tab.GetLength());
        while (VOLATILE_EQUALS(nextTab, mNextTable) && VOLATILE_EQUALS(mTable, tab) &&
                (VOLATILE_GET_INLINE(sc, mSizeCtl), sc < 0)) {
            Integer transferIndex;
            if ((((unsigned Integer)sc) >> RESIZE_STAMP_SHIFT) != rs || sc == rs + 1 ||
                    sc == rs + MAX_RESIZERS ||
                    (VOLATILE_GET_INLINE(transferIndex, mTransferIndex), transferIndex <= 0)) {
                break;
            }
            if (COMPARE_AND_SWAP_INT(this, mSizeCtl, sc, sc + 1)) {
                Transfer(tab, &nextTab);
                break;
            }
        }
        return nextTab;
    }
    VOLATILE_GET(Array<Node*> t, mTable);
    return t;
}

void ConcurrentHashMap::TryPresize(
    /* [in] */ Integer size)
{
    Integer c = (size >= (((unsigned Integer)MAXIMUM_CAPACITY) >> 1)) ? MAXIMUM_CAPACITY :
            TableSizeFor(size + (((unsigned Integer)size) >> 1) + 1);
    Integer sc;
    while (VOLATILE_GET_INLINE(sc, mSizeCtl), sc >= 0) {
        VOLATILE_GET(Array<Node*> tab, mTable);
        Integer n;
        if (tab.IsNull() || (n = tab.GetLength()) == 0) {
            n = (sc > c) ? sc : c;
            if (COMPARE_AND_SWAP_INT(this, mSizeCtl, sc, -1)) {
                if (VOLATILE_EQUALS(mTable, tab)) {
                    Array<Node*> nt(n);
                    VOLATILE_SET(mTable, nt);
                    sc =  n - (((unsigned Integer)n) >> 2);
                }
                VOLATILE_SET(mSizeCtl, sc);
            }
        }
        else if (c <= sc || n >= MAXIMUM_CAPACITY) {
            break;
        }
        else if (VOLATILE_EQUALS(tab, mTable)) {
            Integer rs = ResizeStamp(n);
            if (COMPARE_AND_SWAP_INT(this, mSizeCtl, sc,
                    (rs << RESIZE_STAMP_SHIFT) + 2)) {
                Transfer(tab, nullptr);
            }
        }
    }
}

void ConcurrentHashMap::Transfer(
    /* [in] */ Array<Node*>& tab,
    /* [in] */ Array<Node*>* nextTab)
{
    Integer n = tab.GetLength();
    Integer stride;
    if ((stride = (GetNCPU() > 1) ? (((unsigned Integer)n) >> 3) / GetNCPU() : n) < MIN_TRANSFER_STRIDE) {
        stride = MIN_TRANSFER_STRIDE; // subdivide range
    }
    if (nextTab == nullptr) {
        Array<Node*> nt(n << 1);
        nextTab = &nt;
        VOLATILE_SET(mNextTable, nt);
        VOLATILE_SET(mTransferIndex, n);
    }
    Integer nextn = nextTab->GetLength();
    AutoPtr<ForwardingNode> fwd = new ForwardingNode(*nextTab);
    Boolean advance = true;
    Boolean finishing = false; // to ensure sweep before committing nextTab
    for (Integer i = 0, bound = 0;;) {
        AutoPtr<Node> f;
        Integer fh;
        while (advance) {
            Integer nextIndex, nextBound;
            if (--i >= bound || finishing) {
                advance = false;
            }
            else if (VOLATILE_GET_INLINE(nextIndex, mTransferIndex), nextIndex <= 0) {
                i = -1;
                advance = false;
            }
            else if (COMPARE_AND_SWAP_INT(this, mTransferIndex, nextIndex,
                    nextBound = (nextIndex > stride ?
                        nextIndex - stride : 0))) {
                bound = nextBound;
                i = nextIndex - 1;
                advance = false;
            }
        }
        if (i < 0 || i >= n || i + n >= nextn) {
            Integer sc;
            if (finishing) {
                VOLATILE_SET(mNextTable, Array<Node*>::Null());
                VOLATILE_SET(mTable, *nextTab);
                VOLATILE_SET(mSizeCtl, (n << 1) - (((unsigned Integer)n) >> 1));
                return;
            }
            if (VOLATILE_GET_INLINE(sc, mSizeCtl),
                    COMPARE_AND_SWAP_INT(this, mSizeCtl, sc, sc - 1)) {
                if ((sc - 2) != ResizeStamp(n) << RESIZE_STAMP_SHIFT) {
                    return;
                }
                finishing = advance = true;
                i = n; // recheck before commit
            }
        }
        else if ((f = TabAt(tab, i)) == nullptr) {
            advance = CasTabAt(tab, i, nullptr, (Node*)fwd.Get());
        }
        else if ((fh = f->mHash) == MOVED) {
            advance = true; // already processed
        }
        else {
            AutoLock lock(f);
            AutoPtr<Node> ln, hn;
            if (fh >= 0) {
                Integer runBit = fh & n;
                AutoPtr<Node> lastRun = f;
                VOLATILE_GET(AutoPtr<Node> p, f->mNext);
                for (; p != nullptr; VOLATILE_GET_INLINE(p, p->mNext)) {
                    Integer b = p->mHash & n;
                    if (b != runBit) {
                        runBit = b;
                        lastRun = p;
                    }
                }
                if (runBit == 0) {
                    ln = lastRun;
                    hn = nullptr;
                }
                else {
                    hn = lastRun;
                    ln = nullptr;
                }
                for (AutoPtr<Node> p = f; p != lastRun; VOLATILE_GET_INLINE(p, p->mNext)) {
                    Integer ph = p->mHash;
                    AutoPtr<IInterface> pk = p->mKey;
                    VOLATILE_GET(AutoPtr<IInterface> pv, p->mVal);
                    if ((ph & n) == 0) {
                        ln = new Node(ph, pk, pv, ln);
                    }
                    else {
                        hn = new Node(ph, pk, pv, hn);
                    }
                }
                SetTabAt(*nextTab, i, ln);
                SetTabAt(*nextTab, i + n, hn);
                SetTabAt(tab, i, fwd);
                advance = true;
            }
            else if (f->Probe(IID_TreeBin) != nullptr) {
                TreeBin* t = (TreeBin*)f.Get();
                AutoPtr<TreeNode> lo, loTail;
                AutoPtr<TreeNode> hi, hiTail;
                Integer lc = 0, hc = 0;
                VOLATILE_GET(AutoPtr<Node> e, t->mFirst.Get());
                for (; e != nullptr; VOLATILE_GET_INLINE(e, e->mNext)) {
                    Integer h = e->mHash;
                    VOLATILE_GET(IInterface* val, e->mVal);
                    AutoPtr<TreeNode> p = new TreeNode(
                            h, e->mKey, val, nullptr, nullptr);
                    if ((h & n) == 0) {
                        if ((p->mPrev = loTail) == nullptr) {
                            lo = p;
                        }
                        else {
                            VOLATILE_SET(loTail->mNext, p);
                        }
                        loTail = p;
                        ++lc;
                    }
                    else {
                        if ((p->mPrev = hiTail) == nullptr) {
                            hi = p;
                        }
                        else {
                            VOLATILE_SET(hiTail->mNext, p);
                        }
                        hiTail = p;
                        ++hc;
                    }
                }
                ln = (lc <= UNTREEIFY_THRESHOLD) ? Untreeify(lo) :
                        (hc != 0) ? new TreeBin(lo) : t;
                hn = (hc <= UNTREEIFY_THRESHOLD) ? Untreeify(hi) :
                        (lc != 0) ? new TreeBin(hi) : t;
                SetTabAt(*nextTab, i, ln);
                SetTabAt(*nextTab, i + n, hn);
                SetTabAt(tab, i, fwd);
                advance = true;
            }
        }
    }
}

Long ConcurrentHashMap::SumCount()
{
    VOLATILE_GET(Array<CounterCell*> as, mCounterCells);
    AutoPtr<CounterCell> a;
    Long sum = mBaseCount;
    if (!as.IsNull()) {
        for (Integer i = 0; i < as.GetLength(); ++i) {
            if ((a = as[i]) != nullptr) {
                VOLATILE_GET(Long v, a->mValue);
                sum += v;
            }
        }
    }
    return sum;
}

void ConcurrentHashMap::FullAddCount(
    /* [in] */ Long x,
    /* [in] */ Boolean wasUncontended)
{
    Integer h;
    if ((h = ThreadLocalRandom::GetProbe()) == 0) {
        ThreadLocalRandom::LocalInit();
        h = ThreadLocalRandom::GetProbe();
        wasUncontended = true;
    }
    Boolean collide = false;
    for (;;) {
        Array<CounterCell*> as;
        AutoPtr<CounterCell> a;
        Integer n;
        Long v;
        if ((VOLATILE_GET_INLINE(as, mCounterCells), !as.IsNull()) &&
                (n = as.GetLength()) > 0) {
            if ((a = as[(n - 1) & h]) == nullptr) {
                if (VOLATILE_EQUALS(mCellsBusy, 0)) { // Try to attach new Cell
                    AutoPtr<CounterCell> r = new CounterCell(x);
                    if (VOLATILE_EQUALS(mCellsBusy, 0) &&
                            COMPARE_AND_SWAP_INT(this, mCellsBusy, 0, 1)) {
                        Boolean created = false;
                        Array<CounterCell*> rs;
                        Integer m, j;
                        if ((VOLATILE_GET_INLINE(rs, mCounterCells), !rs.IsNull()) &&
                                (m = rs.GetLength()) > 0 &&
                                rs[j = (m - 1) & h] == nullptr) {
                            rs.Set(j, r);
                            created = true;
                        }
                        VOLATILE_SET(mCellsBusy, 0);
                        if (created) {
                            break;
                        }
                        continue; // Slot is now non-empty
                    }
                }
                collide = false;
            }
            else if (!wasUncontended) { // CAS already known to fail
                wasUncontended = true; // Continue after rehash
            }
            else if (VOLATILE_GET_INLINE(v, a->mValue),
                    COMPARE_AND_SWAP_LONG(a, mValue, v, v + x)) {
                break;
            }
            else if (!VOLATILE_EQUALS(mCounterCells, as) || n >= GetNCPU()) {
                collide = false; // At max size or stale
            }
            else if (!collide) {
                collide = true;
            }
            else if (VOLATILE_EQUALS(mCellsBusy, 0) &&
                    COMPARE_AND_SWAP_INT(this, mCellsBusy, 0, 1)) {
                if (VOLATILE_EQUALS(mCounterCells, as)) {  // Expand table unless stale
                    Array<CounterCell*> rs(n << 1);
                    for (Integer i = 0; i < n; ++i) {
                        rs.Set(i, as[i]);
                    }
                    VOLATILE_SET(mCounterCells, rs);
                }
                VOLATILE_SET(mCellsBusy, 0);
                collide = false;
                continue; // Retry with expanded table
            }
            h = ThreadLocalRandom::AdvanceProbe(h);
        }
        else if (VOLATILE_EQUALS(mCellsBusy, 0) && VOLATILE_EQUALS(mCounterCells, as) &&
                COMPARE_AND_SWAP_INT(this, mCellsBusy, 0, 1)) {
            Boolean init = false;
            if (VOLATILE_EQUALS(mCounterCells, as)) {
                Array<CounterCell*> rs(2);
                rs.Set(h & 1, new CounterCell(x));
                VOLATILE_SET(mCounterCells, rs);
                init = true;
            }
            VOLATILE_GET(mCellsBusy, 0);
            if (init) {
                break;
            }
        }
        else if (VOLATILE_GET_INLINE(v, mBaseCount),
                COMPARE_AND_SWAP_LONG(this, mBaseCount, v, v + x)) {
            break;
        }
    }
}

void ConcurrentHashMap::TreeifyBin(
    /* [in] */ Array<Node*>& tab,
    /* [in] */ Integer index)
{
    AutoPtr<Node> b;
    Integer n;
    if (!tab.IsNull()) {
        if ((n = tab.GetLength()) < MIN_TREEIFY_CAPACITY) {
            TryPresize(n << 1);
        }
        else if ((b = TabAt(tab, index)) != nullptr && b->mHash >= 0) {
            AutoLock lock(b);
            if (TabAt(tab, index) == b) {
                AutoPtr<TreeNode> hd, tl;
                for (AutoPtr<Node> e = b; e != nullptr; VOLATILE_GET_INLINE(e, e->mNext)) {
                    VOLATILE_GET(IInterface* val, e->mVal);
                    AutoPtr<TreeNode> p = new TreeNode(e->mHash, e->mKey, val,
                            nullptr, nullptr);
                    if ((p->mPrev = tl) == nullptr) {
                        hd = p;
                    }
                    else {
                        VOLATILE_SET(tl->mNext, p);
                    }
                    tl = p;
                }
                SetTabAt(tab, index, new TreeBin(hd));
            }
        }
    }
}

AutoPtr<ConcurrentHashMap::Node> ConcurrentHashMap::Untreeify(
    /* [in] */ Node* b)
{
    AutoPtr<Node> hd, tl;
    for (AutoPtr<Node> q = b; q != nullptr; VOLATILE_GET_INLINE(q, q->mNext)) {
        VOLATILE_GET(IInterface* val, q->mVal);
        AutoPtr<Node> p = new Node(q->mHash, q->mKey, val, nullptr);
        if (tl == nullptr) {
            hd = p;
        }
        else {
            VOLATILE_SET(tl->mNext, p);
        }
        tl = p;
    }
    return hd;
}

//-------------------------------------------------------------------------

}
}
}
