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

#include "como/util/LinkedList.h"
#include <comolog.h>

using como::core::E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
using como::core::E_ILLEGAL_STATE_EXCEPTION;
using como::core::ICloneable;
using como::core::IID_ICloneable;
using como::io::IID_ISerializable;

namespace como {
namespace util {

COMO_INTERFACE_IMPL_5(LinkedList, AbstractSequentialList, ILinkedList, IDeque, IQueue, ICloneable, ISerializable);

ECode LinkedList::Constructor()
{
    return AbstractSequentialList::Constructor();
}

ECode LinkedList::Constructor(
    /* [in] */ ICollection* c)
{
    Constructor();
    return AddAll(c);
}

void LinkedList::LinkFirst(
    /* [in] */ IInterface* e)
{
    AutoPtr<Node> f = mFirst;
    AutoPtr<Node> newNode = new Node(nullptr, e, f);
    mFirst = std::move(newNode);
    if (f == nullptr) {
        mLast = mFirst;
    }
    else {
        f->mPrev = mFirst;
    }
    mSize++;
    mModCount++;
}

void LinkedList::LinkLast(
    /* [in] */ IInterface* e)
{
    AutoPtr<Node> l = mLast;
    AutoPtr<Node> newNode = new Node(l, e, nullptr);
    mLast = std::move(newNode);
    if (l == nullptr) {
        mFirst = mLast;
    }
    else {
        l->mNext = mLast;
    }
    mSize++;
    mModCount++;
}

void LinkedList::LinkBefore(
    /* [in] */ IInterface* e,
    /* [in] */ Node* succ)
{
    AutoPtr<Node> pred = succ->mPrev;
    AutoPtr<Node> newNode = new Node(pred, e, succ);
    succ->mPrev = newNode;
    if (pred == nullptr) {
        mFirst = std::move(newNode);
    }
    else {
        pred->mNext = std::move(newNode);
    }
    mSize++;
    mModCount++;
}

AutoPtr<IInterface> LinkedList::UnlinkFirst(
    /* [in] */ Node* f)
{
    AutoPtr<IInterface> element = std::move(f->mItem);
    AutoPtr<Node> next = std::move(f->mNext);
    f->mItem = nullptr;
    f->mNext = nullptr;
    mFirst = std::move(next);
    if (mFirst == nullptr) {
        mLast = nullptr;
    }
    else {
        mFirst->mPrev = nullptr;
    }
    mSize--;
    mModCount++;
    return element;
}

AutoPtr<IInterface> LinkedList::UnlinkLast(
    /* [in] */ Node* l)
{
    AutoPtr<IInterface> element = std::move(l->mItem);
    AutoPtr<Node> prev = l->mPrev;
    l->mItem = nullptr;
    l->mPrev = nullptr;
    mLast = std::move(prev);
    if (mLast == nullptr) {
        mFirst = nullptr;
    }
    else {
        mLast->mNext = nullptr;
    }
    mSize--;
    mModCount++;
    return element;
}

AutoPtr<IInterface> LinkedList::Unlink(
    /* [in] */ Node* x)
{
    AutoPtr<IInterface> element = std::move(x->mItem);
    AutoPtr<Node> next = std::move(x->mNext);
    Node* prev = x->mPrev;

    if (prev == nullptr) {
        mFirst = next;
    }
    else {
        prev->mNext = next;
        x->mPrev = nullptr;
    }

    if (next == nullptr) {
        mLast = prev;
    }
    else {
        next->mPrev = prev;
        x->mNext = nullptr;
    }

    x->mItem = nullptr;
    mSize--;
    mModCount++;
    return element;
}

ECode LinkedList::GetFirst(
    /* [out] */ AutoPtr<IInterface>& e)
{
    if (mFirst == nullptr) {
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }
    e = mFirst->mItem;
    return NOERROR;
}

ECode LinkedList::GetLast(
    /* [out] */ AutoPtr<IInterface>& e)
{
    if (mLast == nullptr) {
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }
    e = mLast->mItem;
    return NOERROR;
}

ECode LinkedList::RemoveFirst(
    /* [out] */ IInterface** e)
{
    if (mFirst == nullptr) {
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }
    AutoPtr<IInterface> element = UnlinkFirst(mFirst);
    if (e != nullptr) {
        element.MoveTo(e);
    }
    return NOERROR;
}

ECode LinkedList::RemoveLast(
    /* [out] */ IInterface** e)
{
    if (mLast == nullptr) {
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }
    AutoPtr<IInterface> element = UnlinkLast(mLast);
    if (e != nullptr) {
        element.MoveTo(e);
    }
    return NOERROR;
}

ECode LinkedList::AddFirst(
    /* [in] */ IInterface* e)
{
    LinkFirst(e);
    return NOERROR;
}

ECode LinkedList::AddLast(
    /* [in] */ IInterface* e)
{
    LinkLast(e);
    return NOERROR;
}

ECode LinkedList::Contains(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean& result)
{
    Integer idx;
    IndexOf(obj, idx);
    result = idx != -1;
    return NOERROR;
}

ECode LinkedList::GetSize(
    /* [out] */ Integer& size)
{
    size = mSize;
    return NOERROR;
}

ECode LinkedList::Add(
    /* [in] */ IInterface* e,
    /* [out] */ Boolean* changed)
{
    LinkLast(e);
    if (changed != nullptr) {
        *changed = true;
    }
    return NOERROR;
}

ECode LinkedList::Remove(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    if (obj == nullptr) {
        for (AutoPtr<Node> x = mFirst; x != nullptr; x = x->mNext) {
            if (x->mItem == nullptr) {
                Unlink(x);
                if (result != nullptr) {
                    *result = true;
                }
                return NOERROR;
            }
        }
    }
    else {
        for (AutoPtr<Node> x = mFirst; x != nullptr; x = x->mNext) {
            if (Object::Equals(obj, x->mItem)) {
                Unlink(x);
                if (result != nullptr) {
                    *result = true;
                }
                return NOERROR;
            }
        }
    }
    if (result != nullptr) {
        *result = false;
    }
    return NOERROR;
}

ECode LinkedList::AddAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* changed)
{
    return AddAll(mSize, c, changed);
}

ECode LinkedList::AddAll(
    /* [in] */ Integer index,
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* result)
{
    FAIL_RETURN(CheckPositionIndex(index));

    Array<IInterface*> a;
    c->ToArray(&a);
    Integer numNew = a.GetLength();
    if (numNew == 0) {
        if (result != nullptr) {
            *result = false;
        }
        return NOERROR;
    }

    AutoPtr<Node> pred, succ;
    if (index == mSize) {
        pred = mLast;
    }
    else {
        succ = GetNode(index);
        pred = succ->mPrev;
    }

    for (IInterface* o : a) {
        AutoPtr<Node> newNode = new Node(pred, o, nullptr);
        if (pred == nullptr) {
            mFirst = newNode;
        }
        else {
            pred->mNext = newNode;
        }
        pred = std::move(newNode);
    }

    if (succ == nullptr) {
        mLast = pred;
    }
    else {
        pred->mNext = succ;
        succ->mPrev = pred;
    }

    mSize += numNew;
    mModCount++;
    if (result != nullptr) {
        *result = true;
    }
    return NOERROR;
}

ECode LinkedList::Clear()
{
    for (AutoPtr<Node> x = mFirst; x != nullptr; ) {
        AutoPtr<Node> next = std::move(x->mNext);
        x->mItem = nullptr;
        x->mNext = nullptr;
        x->mPrev = nullptr;
        x = std::move(next);
    }
    mFirst = nullptr;
    mLast = nullptr;
    mSize = 0;
    mModCount++;
    return NOERROR;
}

ECode LinkedList::Get(
    /* [in] */ Integer index,
    /* [out] */ AutoPtr<IInterface>& obj)
{
    FAIL_RETURN(CheckElementIndex(index));
    obj = GetNode(index)->mItem;
    return NOERROR;
}

ECode LinkedList::Set(
    /* [in] */ Integer index,
    /* [in] */ IInterface* obj,
    /* [out] */ IInterface** prevObj)
{
    FAIL_RETURN(CheckElementIndex(index));

    AutoPtr<Node> x = GetNode(index);
    if (prevObj != nullptr) {
        x->mItem.MoveTo(prevObj);
    }
    x->mItem = obj;
    return NOERROR;
}

ECode LinkedList::Add(
    /* [in] */ Integer index,
    /* [in] */ IInterface* obj)
{
    FAIL_RETURN(CheckPositionIndex(index));

    if (index == mSize) {
        LinkLast(obj);
    }
    else {
        LinkBefore(obj, GetNode(index));
    }
    return NOERROR;
}

ECode LinkedList::Remove(
    /* [in] */ Integer index,
    /* [out] */ IInterface** obj)
{
    FAIL_RETURN(CheckElementIndex(index));
    AutoPtr<IInterface> element = Unlink(GetNode(index));
    if (obj != nullptr) {
        element.MoveTo(obj);
    }
    return NOERROR;
}

Boolean LinkedList::IsElementIndex(
    /* [in] */ Integer index)
{
    return index >= 0 && index < mSize;
}

Boolean LinkedList::IsPositionIndex(
    /* [in] */ Integer index)
{
    return index >= 0 && index <= mSize;
}

String LinkedList::OutOfBoundsMsg(
    /* [in] */ Integer index)
{
    return String::Format("Index: %d, Size: %d\n", index, mSize);
}

ECode LinkedList::CheckElementIndex(
    /* [in] */ Integer index)
{
    if (!IsElementIndex(index)) {
        Logger::E("LinkedList", OutOfBoundsMsg(index).string());
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    return NOERROR;
}

ECode LinkedList::CheckPositionIndex(
    /* [in] */ Integer index)
{
    if (!IsPositionIndex(index)) {
        Logger::E("LinkedList", OutOfBoundsMsg(index).string());
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    return NOERROR;
}

AutoPtr<LinkedList::Node> LinkedList::GetNode(
    /* [in] */ Integer index)
{
    if (index < (mSize >> 1)) {
        AutoPtr<Node> x = mFirst;
        for (Integer i = 0; i < index; i++) {
            x = x->mNext;
        }
        return x;
    }
    else {
        AutoPtr<Node> x = mLast;
        for (Integer i = mSize - 1; i > index; i--) {
            x = x->mPrev;
        }
        return x;
    }
}

ECode LinkedList::IndexOf(
    /* [in] */ IInterface* obj,
    /* [out] */ Integer& index)
{
    Integer idx = 0;
    if (obj == nullptr) {
        for (AutoPtr<Node> x = mFirst; x != nullptr; x = x->mNext) {
            if (x->mItem == nullptr) {
                index = idx;
                return NOERROR;
            }
            idx++;
        }
    }
    else {
        for (AutoPtr<Node> x = mFirst; x != nullptr; x = x->mNext) {
            if (Object::Equals(obj, x->mItem)) {
                index = idx;
                return NOERROR;
            }
            idx++;
        }
    }
    index = -1;
    return NOERROR;
}

ECode LinkedList::LastIndexOf(
    /* [in] */ IInterface* obj,
    /* [out] */ Integer& index)
{
    Integer idx = mSize;
    if (obj == nullptr) {
        for (AutoPtr<Node> x = mLast; x != nullptr; x = x->mPrev) {
            idx--;
            if (x->mItem == nullptr) {
                index = idx;
                return NOERROR;
            }
        }
    }
    else {
        for (AutoPtr<Node> x = mLast; x != nullptr; x = x->mPrev) {
            idx--;
            if (Object::Equals(obj, x->mItem)) {
                index = idx;
                return NOERROR;
            }
        }
    }
    index = -1;
    return NOERROR;
}

ECode LinkedList::Peek(
    /* [out] */ AutoPtr<IInterface>& e)
{
    e = (mFirst == nullptr) ? nullptr : mFirst->mItem;
    return NOERROR;
}

ECode LinkedList::Element(
    /* [out] */ AutoPtr<IInterface>& e)
{
    return GetFirst(e);
}

ECode LinkedList::Poll(
    /* [out] */ AutoPtr<IInterface>& e)
{
    if (mFirst == nullptr) {
        e = nullptr;
    }
    else {
        e = UnlinkFirst(mFirst);
    }
    return NOERROR;
}

ECode LinkedList::Remove(
    /* [out] */ IInterface** e)
{
    return RemoveFirst(e);
}

ECode LinkedList::Offer(
    /* [in] */ IInterface* e,
    /* [out] */ Boolean* changed)
{
    return Add(e, changed);
}

ECode LinkedList::OfferFirst(
    /* [in] */ IInterface* e,
    /* [out] */ Boolean* changed)
{
    AddFirst(e);
    if (changed != nullptr) {
        *changed = true;
    }
    return NOERROR;
}

ECode LinkedList::OfferLast(
    /* [in] */ IInterface* e,
    /* [out] */ Boolean* changed)
{
    AddLast(e);
    if (changed != nullptr) {
        *changed = true;
    }
    return NOERROR;
}

ECode LinkedList::PeekFirst(
    /* [out] */ AutoPtr<IInterface>& e)
{
    e = (mFirst == nullptr) ? nullptr : mFirst->mItem;
    return NOERROR;
}

ECode LinkedList::PeekLast(
    /* [out] */ AutoPtr<IInterface>& e)
{
    e = (mLast == nullptr) ? nullptr : mLast->mItem;
    return NOERROR;
}

ECode LinkedList::PollFirst(
    /* [out] */ AutoPtr<IInterface>& e)
{
    if (mFirst == nullptr) {
        e = nullptr;
    }
    else {
        e = UnlinkFirst(mFirst);
    }
    return NOERROR;
}

ECode LinkedList::PollLast(
    /* [out] */ AutoPtr<IInterface>& e)
{
    if (mLast == nullptr) {
        e = nullptr;
    }
    else {
        e = UnlinkLast(mLast);
    }
    return NOERROR;
}

ECode LinkedList::Push(
    /* [in] */ IInterface* e)
{
    return AddFirst(e);
}

ECode LinkedList::Pop(
    /* [out] */ AutoPtr<IInterface>& e)
{
    return RemoveFirst(&e);
}

ECode LinkedList::RemoveFirstOccurrence(
    /* [in] */ IInterface* e,
    /* [out] */ Boolean* changed)
{
    return Remove(e, changed);
}

ECode LinkedList::RemoveLastOccurrence(
    /* [in] */ IInterface* e,
    /* [out] */ Boolean* changed)
{
    if (e == nullptr) {
        for (AutoPtr<Node> x = mLast; x != nullptr; x = x->mPrev) {
            if (x->mItem == nullptr) {
                Unlink(x);
                if (changed != nullptr) {
                    *changed = true;
                }
                return NOERROR;
            }
        }
    }
    else {
        for (AutoPtr<Node> x = mLast; x != nullptr; x = x->mPrev) {
            if (Object::Equals(e, x->mItem)) {
                Unlink(x);
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

ECode LinkedList::GetListIterator(
    /* [in] */ Integer index,
    /* [out] */ AutoPtr<IListIterator>& it)
{
    FAIL_RETURN(CheckPositionIndex(index));
    it = new ListItr(this, index);
    return NOERROR;
}

ECode LinkedList::GetDescendingIterator(
    /* [out] */ AutoPtr<IIterator>& it)
{
    it = new DescendingIterator(this);
    return NOERROR;
}

ECode LinkedList::CloneImpl(
    /* [in] */ ILinkedList* newObj)
{
    LinkedList* clone = (LinkedList*)newObj;

    clone->mFirst = nullptr;
    clone->mLast = nullptr;
    clone->mSize = 0;
    clone->mModCount = 0;

    for (AutoPtr<Node> x = mFirst; x != nullptr; x = x->mNext) {
        clone->Add(x->mItem);
    }
    return NOERROR;
}

ECode LinkedList::ToArray(
    /* [out, callee] */ Array<IInterface*>* objs)
{
    VALIDATE_NOT_NULL(objs);

    Array<IInterface*> result(mSize);
    Integer i = 0;
    for (AutoPtr<Node> x = mFirst; x != nullptr; x = x->mNext) {
        result.Set(i++, x->mItem);
    }
    *objs = result;
    return NOERROR;
}

ECode LinkedList::ToArray(
    /* [in] */ const InterfaceID& iid,
    /* [out, callee] */ Array<IInterface*>* objs)
{
    VALIDATE_NOT_NULL(objs);

    Array<IInterface*> result(mSize);
    Integer i = 0;
    for (AutoPtr<Node> x = mFirst; x != nullptr; x = x->mNext) {
        result.Set(i++, x->mItem->Probe(iid));
    }
    *objs = result;
    return NOERROR;
}

ECode LinkedList::GetIterator(
    /* [out] */ AutoPtr<IIterator>& it)
{
    return AbstractSequentialList::GetIterator(it);
}

//-------------------------------------------------------------------------------------

COMO_INTERFACE_IMPL_2(LinkedList::ListItr, SyncObject, IListIterator, IIterator);

LinkedList::ListItr::ListItr(
    /* [in] */ LinkedList* owner,
    /* [in] */ Integer index)
    : mNextIndex(index)
    , mExpectedModCount(owner->mModCount)
    , mOwner(owner)
{
    if (index != mOwner->mSize) {
        mNext = mOwner->GetNode(index);
    }
}

ECode LinkedList::ListItr::HasNext(
    /* [out] */ Boolean& result)
{
    result = mNextIndex < mOwner->mSize;
    return NOERROR;
}

ECode LinkedList::ListItr::Next(
    /* [out] */ AutoPtr<IInterface>& object)
{
    FAIL_RETURN(CheckForComodification());
    Boolean hasNext;
    if (HasNext(hasNext), !hasNext) {
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }

    mLastReturned = mNext;
    mNext = mNext->mNext;
    mNextIndex++;
    object = mLastReturned->mItem;
    return NOERROR;
}

ECode LinkedList::ListItr::HasPrevious(
    /* [out] */ Boolean& result)
{
    result = mNextIndex > 0;
    return NOERROR;
}

ECode LinkedList::ListItr::Previous(
    /* [out] */ AutoPtr<IInterface>& object)
{
    FAIL_RETURN(CheckForComodification());
    Boolean hasPrevious;
    if (HasPrevious(hasPrevious), !hasPrevious) {
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }

    mNext = (mNext == nullptr) ? mOwner->mLast.Get() : mNext->mPrev;
    mLastReturned = mNext;
    mNextIndex--;
    object = mLastReturned->mItem;
    return NOERROR;
}

ECode LinkedList::ListItr::GetNextIndex(
    /* [out] */ Integer& index)
{
    index = mNextIndex;
    return NOERROR;
}

ECode LinkedList::ListItr::GetPreviousIndex(
    /* [out] */ Integer& index)
{
    index = mNextIndex - 1;
    return NOERROR;
}

ECode LinkedList::ListItr::Remove()
{
    FAIL_RETURN(CheckForComodification());
    if (mLastReturned == nullptr) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    AutoPtr<Node> lastNext = mLastReturned->mNext;
    mOwner->Unlink(mLastReturned);
    if (mNext == mLastReturned) {
        mNext = lastNext;
    }
    else {
        mNextIndex--;
    }
    mLastReturned = nullptr;
    mExpectedModCount++;
    return NOERROR;
}

ECode LinkedList::ListItr::Set(
    /* [in] */ IInterface* object)
{
    if (mLastReturned == nullptr) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    FAIL_RETURN(CheckForComodification());
    mLastReturned->mItem = object;
    return NOERROR;
}

ECode LinkedList::ListItr::Add(
    /* [in] */ IInterface* object)
{
    FAIL_RETURN(CheckForComodification());
    mLastReturned = nullptr;
    if (mNext == nullptr) {
        mOwner->LinkLast(object);
    }
    else {
        mOwner->LinkBefore(object, mNext);
    }
    mNextIndex++;
    mExpectedModCount++;
    return NOERROR;
}

ECode LinkedList::ListItr::CheckForComodification()
{
    if (mOwner->mModCount != mExpectedModCount) {
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }
    return NOERROR;
}

//-------------------------------------------------------------------------------------

COMO_INTERFACE_IMPL_1(LinkedList::DescendingIterator, SyncObject, IIterator);

LinkedList::DescendingIterator::DescendingIterator(
    /* [in] */ LinkedList* owner)
{
    Integer index;
    owner->GetSize(index);
    mItr = new ListItr(owner, index);
}

ECode LinkedList::DescendingIterator::HasNext(
    /* [out] */ Boolean& result)
{
    return mItr->HasPrevious(result);
}

ECode LinkedList::DescendingIterator::Next(
    /* [out] */ AutoPtr<IInterface>& object)
{
    return mItr->Previous(object);
}

ECode LinkedList::DescendingIterator::Remove()
{
    return mItr->Remove();
}

}
}
