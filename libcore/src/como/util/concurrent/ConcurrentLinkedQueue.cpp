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

#include "coredef.h"
#include "como/util/Arrays.h"
#include "como/util/concurrent/Helpers.h"
#include "como/util/concurrent/ConcurrentLinkedQueue.h"
#include "como.util.IIterator.h"

using como::io::IID_ISerializable;

namespace como {
namespace util {
namespace concurrent {

COMO_INTERFACE_IMPL_2(ConcurrentLinkedQueue, AbstractQueue, IConcurrentLinkedQueue, ISerializable);

AutoPtr<ConcurrentLinkedQueue::Node> ConcurrentLinkedQueue::NewNode(
    /* [in] */ IInterface* item)
{
    AutoPtr<Node> node = new Node();
    PUT_OBJECT(node, mItem, item);
    return node;
}

Boolean ConcurrentLinkedQueue::CasItem(
    /* [in] */ Node* node,
    /* [in] */ IInterface* cmp,
    /* [in] */ IInterface* val)
{
    Boolean success = COMPARE_AND_SWAP_OBJECT(node, mItem, cmp, val);
    if (success) {
        REFCOUNT_ADD(val);
        REFCOUNT_RELEASE(cmp);
    }
    return success;
}

void ConcurrentLinkedQueue::LazySetNext(
    /* [in] */ Node* node,
    /* [in] */ Node* val)
{
    PUT_ORDERED_OBJECT(node, mNext, val);
}

void ConcurrentLinkedQueue::LazySetOff(
    /* [in] */ Node* node,
    /* [in] */ Boolean val)
{
    PUT_ORDERED_OBJECT(node, mIsOff, val);
}

Boolean ConcurrentLinkedQueue::CasNext(
    /* [in] */ Node* node,
    /* [in] */ Node* cmp,
    /* [in] */ Node* val)
{
    Boolean success = COMPARE_AND_SWAP_OBJECT(node, mNext, cmp, val);
    if (success) {
        REFCOUNT_ADD(val);
        REFCOUNT_RELEASE(cmp);
    }
    return success;
}

ECode ConcurrentLinkedQueue::Constructor()
{
    AutoPtr<Node> node = NewNode(nullptr);
    VOLATILE_SET(mHead, node);
    VOLATILE_SET(mTail, node);
    return NOERROR;
}

ECode ConcurrentLinkedQueue::Constructor(
    /* [in] */ ICollection* c)
{
    AutoPtr<Node> h, t;
    FOR_EACH(IInterface*, e, , c) {
        if (e == nullptr) {
            return como::core::E_NULL_POINTER_EXCEPTION;
        }
        AutoPtr<Node> newNode = NewNode(e);
        if (h == nullptr) {
            h = t = newNode;
        }
        else {
            LazySetNext(t, newNode);
            t = newNode;
        }
    } END_FOR_EACH();
    if (h == nullptr) {
        h = t = NewNode(nullptr);
    }
    VOLATILE_SET(mHead, h);
    VOLATILE_SET(mTail, t);
    return NOERROR;
}

ECode ConcurrentLinkedQueue::Add(
    /* [in] */ IInterface* e,
    /* [out] */ Boolean* changed)
{
    return Offer(e, changed);
}

void ConcurrentLinkedQueue::UpdateHead(
    /* [in] */ Node* h,
    /* [in] */ Node* p)
{
    if (h != p && CasHead(h, p)) {
        LazySetOff(h, true);
    }
}

AutoPtr<ConcurrentLinkedQueue::Node> ConcurrentLinkedQueue::Succ(
    /* [in] */ Node* p)
{
    VOLATILE_GET(Boolean off, p->mIsOff);
    AutoPtr<Node> next;
    if (off) {
        VOLATILE_GET(next, mHead);
    }
    else {
        VOLATILE_GET(next, p->mNext);
    }
    return next;
}

ECode ConcurrentLinkedQueue::Offer(
    /* [in] */ IInterface* e,
    /* [out] */ Boolean* changed)
{
    if (e == nullptr) {
        return como::core::E_NULL_POINTER_EXCEPTION;
    }

    AutoPtr<Node> newNode = NewNode(e);

    VOLATILE_GET(AutoPtr<Node> t, mTail);
    for (AutoPtr<Node> p = t;;) {
        VOLATILE_GET(AutoPtr<Node> q, p->mNext);
        if (q == nullptr) {
            // p is last node
            if (CasNext(p, nullptr, newNode)) {
                // Successful CAS is the linearization point
                // for e to become an element of this queue,
                // and for newNode to become "live".
                if (p != t) { // hop two nodes at a time
                    CasTail(t, newNode); // Failure is OK.
                }
                if (changed != nullptr) {
                    *changed = true;
                }
                return NOERROR;
            }
            // Lost CAS race to another thread; re-read next
        }
        else {
            VOLATILE_GET(Boolean off, p->mIsOff);
            if (off) {
                // We have fallen off list.  If tail is unchanged, it
                // will also be off-list, in which case we need to
                // jump to head, from which all live nodes are always
                // reachable.  Else the new tail is a better bet.
                AutoPtr<Node> oldT = t;
                VOLATILE_GET(t, mTail);
                if (oldT != t) {
                    p = t;
                }
                else {
                    VOLATILE_GET(p, mHead);
                }
            }
            else {
                // Check for tail updates after two hops.
                if (p != t) {
                    AutoPtr<Node> oldT = t;
                    VOLATILE_GET(t, mTail);
                    p = (oldT != t) ? t : q;
                }
                else {
                    p = q;
                }
            }
        }
    }
}

ECode ConcurrentLinkedQueue::Poll(
    /* [out] */ AutoPtr<IInterface>& head)
{
RESTART_FROM_HEAD:
    for (;;) {
        VOLATILE_GET(AutoPtr<Node> h, mHead);
        for (AutoPtr<Node> p = h, q;;) {
            VOLATILE_GET(AutoPtr<IInterface> item, p->mItem);

            if (item != nullptr && CasItem(p, item, nullptr)) {
                // Successful CAS is the linearization point
                // for item to be removed from this queue.
                if (p != h) { // hop two nodes at a time
                    VOLATILE_GET(q, p->mNext);
                    UpdateHead(h, (q != nullptr) ? q : p);
                }
                head = item;
                return NOERROR;
            }
            else {
                VOLATILE_GET(q, p->mNext);
                if (q == nullptr) {
                    UpdateHead(h, p);
                    head = nullptr;
                    return NOERROR;
                }
                else {
                    VOLATILE_GET(Boolean off, p->mIsOff);
                    if (off) {
                        goto RESTART_FROM_HEAD;
                    }
                    else {
                        p = q;
                    }
                }
            }
        }
    }
}

ECode ConcurrentLinkedQueue::Peek(
    /* [out] */ AutoPtr<IInterface>& head)
{
RESTART_FROM_HEAD:
    for (;;) {
        VOLATILE_GET(AutoPtr<Node> h, mHead);
        for (AutoPtr<Node> p = h, q;;) {
            VOLATILE_GET(AutoPtr<IInterface> item, p->mItem);
            if (item != nullptr) {
                UpdateHead(h, p);
                head = std::move(item);
                return NOERROR;
            }
            else {
                VOLATILE_GET(q, p->mNext);
                if (q == nullptr) {
                    UpdateHead(h, p);
                    head = std::move(item);
                    return NOERROR;
                }
                else {
                    VOLATILE_GET(Boolean off, p->mIsOff);
                    if (off) {
                        goto RESTART_FROM_HEAD;
                    }
                    else {
                        p = q;
                    }
                }
            }
        }
    }
}

AutoPtr<ConcurrentLinkedQueue::Node> ConcurrentLinkedQueue::First()
{
RESTART_FROM_HEAD:
    for (;;) {
        VOLATILE_GET(AutoPtr<Node> h, mHead);
        for (AutoPtr<Node> p = h, q;;) {
            VOLATILE_GET(AutoPtr<IInterface> item, p->mItem);
            Boolean hasItem = item != nullptr;
            if (hasItem) {
                UpdateHead(h, p);
                return p;
            }
            else {
                VOLATILE_GET(q, p->mNext);
                if (q == nullptr) {
                    UpdateHead(h, p);
                    return nullptr;
                }
                else {
                    VOLATILE_GET(Boolean off, p->mIsOff);
                    if (off) {
                        goto RESTART_FROM_HEAD;
                    }
                    else {
                        p = q;
                    }
                }
            }
        }
    }
}

ECode ConcurrentLinkedQueue::IsEmpty(
    /* [out] */ Boolean& empty)
{
    empty = First() == nullptr;
    return NOERROR;
}

ECode ConcurrentLinkedQueue::GetSize(
    /* [out] */ Integer& size)
{
RESTART_FROM_HEAD:
    for (;;) {
        Integer count = 0;
        for (AutoPtr<Node> p = First(); p != nullptr;) {
            VOLATILE_GET(AutoPtr<IInterface> item, p->mItem);
            if (item != nullptr) {
                if (++count == IInteger::MAX_VALUE) {
                    break;
                }
            }
            VOLATILE_GET(Boolean off, p->mIsOff);
            if (off) {
                goto RESTART_FROM_HEAD;
            }
            VOLATILE_GET(p, p->mNext);
        }
        size = count;
        return NOERROR;
    }
}

ECode ConcurrentLinkedQueue::Contains(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean& result)
{
    if (obj != nullptr) {
        for (AutoPtr<Node> p = First(); p != nullptr; p = Succ(p)) {
            VOLATILE_GET(AutoPtr<IInterface> item, p->mItem);
            if (item != nullptr && Object::Equals(obj, item)) {
                result = true;
                return NOERROR;
            }
        }
    }
    result = false;
    return NOERROR;
}

ECode ConcurrentLinkedQueue::Remove(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* changed)
{
    if (obj != nullptr) {
        AutoPtr<Node> next, pred;
        for (AutoPtr<Node> p = First(); p != nullptr; pred = p, p = next) {
            Boolean removed = false;
            VOLATILE_GET(AutoPtr<IInterface> item, p->mItem);
            if (item != nullptr) {
                if (!Object::Equals(obj, item)) {
                    next = Succ(p);
                    continue;
                }
                removed = CasItem(p, item, nullptr);
            }

            next = Succ(p);
            if (pred != nullptr && next != nullptr) { // unlink
                CasNext(pred, p, next);
            }
            if (removed) {
                if (changed != nullptr) {
                    *changed = true;
                }
                return NOERROR;
            }
        }
    }
    if (changed != nullptr) {
        *changed = false;
    }
    return NOERROR;
}

ECode ConcurrentLinkedQueue::AddAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* changed)
{
    if (c == (ICollection*)this) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // Copy c into a private chain of Nodes
    AutoPtr<Node> beginningOfTheEnd, last;
    FOR_EACH(IInterface*, e, , c) {
        if (e == nullptr) {
            return como::core::E_NULL_POINTER_EXCEPTION;
        }
        AutoPtr<Node> newNode = NewNode(e);
        if (beginningOfTheEnd == nullptr) {
            beginningOfTheEnd = last = newNode;
        }
        else {
            LazySetNext(last, newNode);
            last = newNode;
        }
    } END_FOR_EACH()
    if (beginningOfTheEnd == nullptr) {
        if (changed != nullptr) {
            *changed = false;
        }
        return NOERROR;
    }

    // Atomically append the chain at the tail of this collection
    VOLATILE_GET(AutoPtr<Node> t, mTail);
    for (AutoPtr<Node> p = t;;) {
        VOLATILE_GET(AutoPtr<Node> q, p->mNext);
        if (q == nullptr) {
            // p is last node
            if (CasNext(p, nullptr, beginningOfTheEnd)) {
                // Successful CAS is the linearization point
                // for all elements to be added to this queue.
                if (!CasTail(t, last)) {
                    // Try a little harder to update tail,
                    // since we may be adding many elements.
                    VOLATILE_GET(t, mTail);
                    VOLATILE_GET(Node* next, last->mNext);
                    if (next == nullptr) {
                        CasTail(t, last);
                    }
                }
                if (changed != nullptr) {
                    *changed = true;
                }
                return NOERROR;
            }
            // Lost CAS race to another thread; re-read next
        }
        else {
            VOLATILE_GET(Boolean off, p->mIsOff);
            if (off) {
                // We have fallen off list.  If tail is unchanged, it
                // will also be off-list, in which case we need to
                // jump to head, from which all live nodes are always
                // reachable.  Else the new tail is a better bet.
                AutoPtr<Node> oldT = t;
                VOLATILE_GET(t, mTail);
                if (oldT != t) {
                    p = t;
                }
                else {
                    VOLATILE_GET(p, mHead);
                }
            }
            else {
                // Check for tail updates after two hops.
                if (p != t) {
                    AutoPtr<Node> oldT = t;
                    VOLATILE_GET(t, mTail);
                    p = (oldT != t) ? t : q;
                }
                else {
                    p = q;
                }
            }
        }
    }
}

ECode ConcurrentLinkedQueue::ToString(
    /* [out] */ String& desc)
{
    Array<String> a;
RESTART_FROM_HEAD:
    for (;;) {
        Integer charLength = 0;
        Integer size = 0;
        for (AutoPtr<Node> p = First(); p != nullptr;) {
            VOLATILE_GET(AutoPtr<IInterface> item, p->mItem);
            if (item != nullptr) {
                if (a.IsNull()) {
                    a = Array<String>(4);
                }
                else if (size == a.GetLength()) {
                    Arrays::CopyOf(a, 2 * size, &a);
                }
                String s = Object::ToString(item);
                a[size++] = s;
                charLength += s.GetLength();
            }
            VOLATILE_GET(Boolean off, p->mIsOff);
            if (off) {
                goto RESTART_FROM_HEAD;
            }
            VOLATILE_GET(p, p->mNext);
        }

        if (size == 0) {
            desc = "[]";
            return NOERROR;
        }

        desc = Helpers::ToString(a, size, charLength);
        return NOERROR;
    }
}

Array<IInterface*> ConcurrentLinkedQueue::ToArrayInternal()
{
    Array<IInterface*> x;
RESTART_FROM_HEAD:
    for (;;) {
        Integer size = 0;
        for (AutoPtr<Node> p = First(); p != nullptr;) {
            VOLATILE_GET(AutoPtr<IInterface> item, p->mItem);
            if (item != nullptr) {
                if (x.IsNull()) {
                    x = Array<IInterface*>(4);
                }
                else if (size == x.GetLength()) {
                    Arrays::CopyOf(x, 2 * (size + 4), &x);
                }
                x.Set(size++, item);
            }
            VOLATILE_GET(Boolean off, p->mIsOff);
            if (off) {
                goto RESTART_FROM_HEAD;
            }
            VOLATILE_GET(p, p->mNext);
        }
        if (x.IsNull()) {
            return Array<IInterface*>(0);
        }
        if (size == x.GetLength()) {
            return x;
        }
        else {
            Arrays::CopyOf(x, size, &x);
            return x;
        }
    }
}

Array<IInterface*> ConcurrentLinkedQueue::ToArrayInternal(
    /* [in] */ const InterfaceID& iid)
{
    Array<IInterface*> x;
RESTART_FROM_HEAD:
    for (;;) {
        Integer size = 0;
        for (AutoPtr<Node> p = First(); p != nullptr;) {
            VOLATILE_GET(AutoPtr<IInterface> item, p->mItem);
            if (item != nullptr) {
                if (x.IsNull()) {
                    x = Array<IInterface*>(4);
                }
                else if (size == x.GetLength()) {
                    Arrays::CopyOf(x, 2 * (size + 4), &x);
                }
                x.Set(size++, item->Probe(iid));
            }
            VOLATILE_GET(Boolean off, p->mIsOff);
            if (off) {
                goto RESTART_FROM_HEAD;
            }
            VOLATILE_GET(p, p->mNext);
        }
        if (x.IsNull()) {
            return Array<IInterface*>(0);
        }
        if (size == x.GetLength()) {
            return x;
        }
        else {
            Arrays::CopyOf(x, size, &x);
            return x;
        }
    }
}

ECode ConcurrentLinkedQueue::ToArray(
    /* [out, callee] */ Array<IInterface*>* objs)
{
    VALIDATE_NOT_NULL(objs);

    *objs = ToArrayInternal();
    return NOERROR;
}

ECode ConcurrentLinkedQueue::ToArray(
    /* [in] */ const InterfaceID& iid,
    /* [out, callee] */ Array<IInterface*>* objs)
{
    VALIDATE_NOT_NULL(objs);

    *objs = ToArrayInternal(iid);
    return NOERROR;
}

ECode ConcurrentLinkedQueue::GetIterator(
    /* [out] */ AutoPtr<IIterator>& it)
{
    it = new Itr(this);
    return NOERROR;
}

Boolean ConcurrentLinkedQueue::CasTail(
    /* [in] */ Node* cmp,
    /* [in] */ Node* val)
{
    Boolean success = COMPARE_AND_SWAP_OBJECT(this, mTail, cmp, val);
    if (success) {
        REFCOUNT_ADD(val);
        REFCOUNT_RELEASE(cmp);
    }
    return success;
}

Boolean ConcurrentLinkedQueue::CasHead(
    /* [in] */ Node* cmp,
    /* [in] */ Node* val)
{
    Boolean success = COMPARE_AND_SWAP_OBJECT(this, mHead, cmp, val);
    if (success) {
        REFCOUNT_ADD(val);
        REFCOUNT_RELEASE(cmp);
    }
    return success;
}

ECode ConcurrentLinkedQueue::Clear()
{
    return AbstractQueue::Clear();
}

ECode ConcurrentLinkedQueue::ContainsAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean& result)
{
    return AbstractQueue::ContainsAll(c, result);
}

ECode ConcurrentLinkedQueue::Element(
    /* [out] */ AutoPtr<IInterface>& head)
{
    return AbstractQueue::Element(head);
}

ECode ConcurrentLinkedQueue::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean& result)
{
    return AbstractQueue::Equals(obj, result);
}

ECode ConcurrentLinkedQueue::GetHashCode(
    /* [out] */ Integer& hash)
{
    return AbstractQueue::GetHashCode(hash);
}

ECode ConcurrentLinkedQueue::Remove(
    /* [out] */ IInterface** head)
{
    return AbstractQueue::Remove(head);
}

ECode ConcurrentLinkedQueue::RemoveAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* changed)
{
    return AbstractQueue::RemoveAll(c, changed);
}

ECode ConcurrentLinkedQueue::RetainAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* changed)
{
    return AbstractQueue::RetainAll(c, changed);
}

//-------------------------------------------------------------------------

COMO_INTERFACE_IMPL_LIGHT_1(ConcurrentLinkedQueue::Itr, LightRefBase, IIterator);

ConcurrentLinkedQueue::Itr::Itr(
    /* [in] */ ConcurrentLinkedQueue* owner)
    : mOwner(owner)
{
RESTART_FROM_HEAD:
    for (;;) {
        AutoPtr<Node> h, p, q;
        VOLATILE_GET(h, mOwner->mHead);
        for (p = h;; p = q) {
            VOLATILE_GET(AutoPtr<IInterface> item, p->mItem);
            if (item != nullptr) {
                mNextNode = p;
                mNextItem = item;
                break;
            }
            VOLATILE_GET(q, p->mNext);
            if (q == nullptr) {
                break;
            }
            else {
                VOLATILE_GET(Boolean off, p->mIsOff);
                if (off) {
                    goto RESTART_FROM_HEAD;
                }
            }
        }
        mOwner->UpdateHead(h, p);
        return;
    }
}

ECode ConcurrentLinkedQueue::Itr::HasNext(
    /* [out] */ Boolean& result)
{
    result = mNextItem != nullptr;
    return NOERROR;
}

ECode ConcurrentLinkedQueue::Itr::Next(
    /* [out] */ IInterface** object)
{
    AutoPtr<Node> pred = mNextNode;
    if (pred == nullptr) {
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }
    mLastRet = pred;
    AutoPtr<IInterface> item;

    for (AutoPtr<Node> p = mOwner->Succ(pred), q;; p = q) {
        if (p == nullptr) {
            mNextNode = p;
            if (object != nullptr) {
                *object = mNextItem;
                REFCOUNT_ADD(*object);
            }
            mNextItem = item;
            return NOERROR;
        }
        VOLATILE_GET(item, p->mItem);
        if (item != nullptr) {
            mNextNode = p;
            if (object != nullptr) {
                *object = mNextItem;
                REFCOUNT_ADD(*object);
            }
            mNextItem = item;
            return NOERROR;
        }
        // unlink deleted nodes
        if ((q = mOwner->Succ(p)) != nullptr) {
            CasNext(pred, p, q);
        }
    }
}

ECode ConcurrentLinkedQueue::Itr::Remove()
{
    AutoPtr<Node> l = mLastRet;
    if (l == nullptr) {
        return como::core::E_ILLEGAL_STATE_EXCEPTION;
    }
    // rely on a future traversal to relink.
    VOLATILE_SET(l->mItem, nullptr);
    mLastRet = nullptr;
    return NOERROR;
}

}
}
}
