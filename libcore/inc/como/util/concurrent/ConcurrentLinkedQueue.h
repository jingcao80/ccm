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

#ifndef __COMO_UTIL_CONCURRENT_CONCURRENTLINKEDQUEUE_H__
#define __COMO_UTIL_CONCURRENT_CONCURRENTLINKEDQUEUE_H__

#include "como/core/volatile.h"
#include "como/util/AbstractQueue.h"
#include "como.io.ISerializable.h"
#include "como.util.IIterator.h"
#include "como.util.concurrent.IConcurrentLinkedQueue.h"
#include <comosp.h>
#include <comoobj.h>
#include <comoref.h>

using como::io::ISerializable;

namespace como {
namespace util {
namespace concurrent {

class ConcurrentLinkedQueue
    : public AbstractQueue
    , public IConcurrentLinkedQueue
    , public ISerializable
{
private:
    class Node
        : public Object
    {
    public:
        VOLATILE AutoPtr<IInterface> mItem;
        VOLATILE AutoPtr<Node> mNext;
        VOLATILE Boolean mIsOff = false;
    };

    class Itr
        : public LightRefBase
        , public IIterator
    {
    public:
        Itr(
            /* [in] */ ConcurrentLinkedQueue* owner);

        COMO_INTERFACE_DECL();

        ECode HasNext(
            /* [out] */ Boolean* result) override;

        ECode Next(
            /* [out] */ IInterface** object = nullptr) override;

        ECode Remove() override;

    private:
        ConcurrentLinkedQueue* mOwner;
        AutoPtr<Node> mNextNode;

        AutoPtr<IInterface> mNextItem;

        AutoPtr<Node> mLastRet;
    };

public:
    COMO_INTERFACE_DECL();

    static AutoPtr<Node> NewNode(
        /* [in] */ IInterface* item);

    static Boolean CasItem(
        /* [in] */ Node* node,
        /* [in] */ IInterface* cmp,
        /* [in] */ IInterface* val);

    static void LazySetNext(
        /* [in] */ Node* node,
        /* [in] */ Node* val);

    static void LazySetOff(
        /* [in] */ Node* node,
        /* [in] */ Boolean val);

    static Boolean CasNext(
        /* [in] */ Node* node,
        /* [in] */ Node* cmp,
        /* [in] */ Node* val);

    ECode Constructor();

    ECode Constructor(
        /* [in] */ ICollection* c);

    ECode Add(
        /* [in] */ IInterface* e,
        /* [out] */ Boolean* changed = nullptr) override;

    void UpdateHead(
        /* [in] */ Node* h,
        /* [in] */ Node* p);

    AutoPtr<Node> Succ(
        /* [in] */ Node* p);

    ECode Offer(
        /* [in] */ IInterface* e,
        /* [out] */ Boolean* changed = nullptr) override;

    ECode Poll(
        /* [out] */ IInterface** head = nullptr) override;

    ECode Peek(
        /* [out] */ AutoPtr<IInterface>& head) override;

    virtual AutoPtr<Node> First();

    ECode IsEmpty(
        /* [out] */ Boolean& empty) override;

    ECode GetSize(
        /* [out] */ Integer& size) override;

    ECode Contains(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean& result) override;

    ECode Remove(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* changed = nullptr) override;

    ECode AddAll(
        /* [in] */ ICollection* c,
        /* [out] */ Boolean* changed = nullptr) override;

    ECode ToString(
        /* [out] */ String& desc) override;

    ECode ToArray(
        /* [out, callee] */ Array<IInterface*>* objs) override;

    ECode ToArray(
        /* [in] */ const InterfaceID& iid,
        /* [out, callee] */ Array<IInterface*>* objs) override;

    ECode GetIterator(
        /* [out] */ AutoPtr<IIterator>& it) override;

    ECode Clear() override;

    ECode ContainsAll(
        /* [in] */ ICollection* c,
        /* [out] */ Boolean& result) override;

    ECode Element(
        /* [out] */ AutoPtr<IInterface>& head) override;

    ECode Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean& result) override;

    ECode GetHashCode(
        /* [out] */ Integer& hash) override;

    ECode Remove(
        /* [out] */ IInterface** head = nullptr) override;

    ECode RemoveAll(
        /* [in] */ ICollection* c,
        /* [out] */ Boolean* changed = nullptr) override;

    ECode RetainAll(
        /* [in] */ ICollection* c,
        /* [out] */ Boolean* changed = nullptr) override;

private:
    Array<IInterface*> ToArrayInternal();

    Array<IInterface*> ToArrayInternal(
        /* [in] */ const InterfaceID& iid);

    Boolean CasTail(
        /* [in] */ Node* cmp,
        /* [in] */ Node* val);

    Boolean CasHead(
        /* [in] */ Node* cmp,
        /* [in] */ Node* val);

private:
    VOLATILE AutoPtr<Node> mHead;
    VOLATILE AutoPtr<Node> mTail;
};

}
}
}

#endif // __COMO_UTIL_CONCURRENT_CONCURRENTLINKEDQUEUE_H__
