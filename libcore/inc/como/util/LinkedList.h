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

#ifndef __COMO_UTIL_LINKEDLIST_H__
#define __COMO_UTIL_LINKEDLIST_H__

#include "como/core/SyncObject.h"
#include "como/util/AbstractSequentialList.h"
#include "como.core.ICloneable.h"
#include "como.io.ISerializable.h"
#include "como.util.IDeque.h"
#include "como.util.IIterator.h"
#include "como.util.ILinkedList.h"
#include "como.util.IListIterator.h"
#include "como.util.IQueue.h"

using como::core::ICloneable;
using como::core::SyncObject;
using como::io::ISerializable;

namespace como {
namespace util {

class LinkedList
    : public AbstractSequentialList
    , public ILinkedList
    , public IDeque
    , public IQueue
    , public ICloneable
    , public ISerializable
{
private:
    class Node
        : public SyncObject
    {
    public:
        Node(
            /* [in] */ Node* prev,
            /* [in] */ IInterface* element,
            /* [in] */ Node* next)
            : mItem(element)
            , mNext(next)
            , mPrev(prev)
        {}

    public:
        AutoPtr<IInterface> mItem;
        AutoPtr<Node> mNext;
        Node* mPrev = nullptr;
    };

    class ListItr
        : public SyncObject
        , public IListIterator
        , public IIterator
    {
    public:
        ListItr(
            /* [in] */ LinkedList* owner,
            /* [in] */ Integer index);

        COMO_INTERFACE_DECL();

        ECode HasNext(
            /* [out] */ Boolean& result) override;

        ECode Next(
            /* [out] */ IInterface** object = nullptr) override;

        ECode HasPrevious(
            /* [out] */ Boolean* result) override;

        ECode Previous(
            /* [out] */ IInterface** object = nullptr) override;

        ECode GetNextIndex(
            /* [out] */ Integer* index) override;

        ECode GetPreviousIndex(
            /* [out] */ Integer* index) override;

        ECode Remove() override;

        ECode Set(
            /* [in] */ IInterface* object) override;

        ECode Add(
            /* [in] */ IInterface* object) override;

        ECode CheckForComodification();

    private:
        AutoPtr<Node> mLastReturned;
        AutoPtr<Node> mNext;
        Integer mNextIndex = 0;
        Integer mExpectedModCount;
        AutoPtr<LinkedList> mOwner;
    };

    class DescendingIterator
        : public SyncObject
        , public IIterator
    {
    public:
        DescendingIterator(
            /* [in] */ LinkedList* owner);

        COMO_INTERFACE_DECL();

        ECode HasNext(
            /* [out] */ Boolean& result) override;

        ECode Next(
            /* [out] */ IInterface** object = nullptr) override;

        ECode Remove() override;

    private:
        AutoPtr<ListItr> mItr;
    };

public:
    COMO_INTERFACE_DECL();

    ECode Constructor();

    ECode Constructor(
        /* [in] */ ICollection* c);

    ECode GetFirst(
        /* [out] */ AutoPtr<IInterface>& e) override;

    ECode GetLast(
        /* [out] */ AutoPtr<IInterface>& e) override;

    ECode RemoveFirst(
        /* [out] */ IInterface** e = nullptr) override;

    ECode RemoveLast(
        /* [out] */ IInterface** e = nullptr) override;

    ECode AddFirst(
        /* [in] */ IInterface* e) override;

    ECode AddLast(
        /* [in] */ IInterface* e) override;

    ECode Contains(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean& result) override;

    ECode GetSize(
        /* [out] */ Integer& size) override;

    ECode Add(
        /* [in] */ IInterface* e,
        /* [out] */ Boolean* changed = nullptr) override;

    ECode Remove(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result = nullptr) override;

    ECode AddAll(
        /* [in] */ ICollection* c,
        /* [out] */ Boolean* changed = nullptr) override;

    ECode AddAll(
        /* [in] */ Integer index,
        /* [in] */ ICollection* c,
        /* [out] */ Boolean* result = nullptr) override;

    ECode Clear() override;

    ECode Get(
        /* [in] */ Integer index,
        /* [out] */ IInterface** obj) override;

    ECode Set(
        /* [in] */ Integer index,
        /* [in] */ IInterface* obj,
        /* [out] */ IInterface** prevObj = nullptr) override;

    ECode Add(
        /* [in] */ Integer index,
        /* [in] */ IInterface* obj) override;

    ECode Remove(
        /* [in] */ Integer index,
        /* [out] */ IInterface** obj = nullptr) override;

    ECode IndexOf(
        /* [in] */ IInterface* obj,
        /* [out] */ Integer* index) override;

    ECode LastIndexOf(
        /* [in] */ IInterface* obj,
        /* [out] */ Integer* index) override;

    ECode Peek(
        /* [out] */ AutoPtr<IInterface>& e) override;

    ECode Element(
        /* [out] */ AutoPtr<IInterface>& e) override;

    ECode Poll(
        /* [out] */ AutoPtr<IInterface>& e) override;

    ECode Remove(
        /* [out] */ IInterface** e = nullptr) override;

    ECode Offer(
        /* [in] */ IInterface* e,
        /* [out] */ Boolean* changed = nullptr) override;

    ECode OfferFirst(
        /* [in] */ IInterface* e,
        /* [out] */ Boolean* changed = nullptr) override;

    ECode OfferLast(
        /* [in] */ IInterface* e,
        /* [out] */ Boolean* changed = nullptr) override;

    ECode PeekFirst(
        /* [out] */ AutoPtr<IInterface>& e) override;

    ECode PeekLast(
        /* [out] */ AutoPtr<IInterface>& e) override;

    ECode PollFirst(
        /* [out] */ AutoPtr<IInterface>& e) override;

    ECode PollLast(
        /* [out] */ AutoPtr<IInterface>& e) override;

    ECode Push(
        /* [in] */ IInterface* e) override;

    ECode Pop(
        /* [out] */ AutoPtr<IInterface>& e) override;

    ECode RemoveFirstOccurrence(
        /* [in] */ IInterface* e,
        /* [out] */ Boolean* changed = nullptr) override;

    ECode RemoveLastOccurrence(
        /* [in] */ IInterface* e,
        /* [out] */ Boolean* changed = nullptr) override;

    ECode GetListIterator(
        /* [in] */ Integer index,
        /* [out] */ IListIterator** it) override;

    ECode GetDescendingIterator(
        /* [out] */ AutoPtr<IIterator>& it) override;

    ECode ToArray(
        /* [out, callee] */ Array<IInterface*>* objs) override;

    ECode ToArray(
        /* [in] */ const InterfaceID& iid,
        /* [out, callee] */ Array<IInterface*>* objs) override;

    ECode GetIterator(
        /* [out] */ AutoPtr<IIterator>& it) override;

protected:
    virtual void LinkLast(
        /* [in] */ IInterface* e);

    virtual void LinkBefore(
        /* [in] */ IInterface* e,
        /* [in] */ Node* succ);

    virtual AutoPtr<IInterface> Unlink(
        /* [in] */ Node* x);

    virtual AutoPtr<Node> GetNode(
        /* [in] */ Integer index);

    ECode CloneImpl(
        /* [in] */ ILinkedList* newObj);

private:
    void LinkFirst(
        /* [in] */ IInterface* e);

    AutoPtr<IInterface> UnlinkFirst(
        /* [in] */ Node* f);

    AutoPtr<IInterface> UnlinkLast(
        /* [in] */ Node* l);

    Boolean IsElementIndex(
        /* [in] */ Integer index);

    Boolean IsPositionIndex(
        /* [in] */ Integer index);

    String OutOfBoundsMsg(
        /* [in] */ Integer index);

    ECode CheckElementIndex(
        /* [in] */ Integer index);

    ECode CheckPositionIndex(
        /* [in] */ Integer index);

protected:
    Integer mSize = 0;

    AutoPtr<Node> mFirst;

    AutoPtr<Node> mLast;
};

}
}

#endif // __COMO_UTIL_LINKEDLIST_H__
