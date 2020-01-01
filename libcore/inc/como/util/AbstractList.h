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

#ifndef __COMO_UTIL_ABSTRACTLIST_H__
#define __COMO_UTIL_ABSTRACTLIST_H__

#include "como/util/AbstractCollection.h"
#include "como.util.IIterator.h"
#include "como.util.IList.h"
#include "como.util.IListIterator.h"
#include <ccmrefbase.h>

namespace como {
namespace util {

class AbstractList
    : public AbstractCollection
    , public IList
{
private:
    class Itr
        : public LightRefBase
        , public IIterator
    {
    public:
        Itr(
            /* [in] */ AbstractList* owner)
            : mOwner(owner)
            , mExpectedModCount(mOwner->mModCount)
        {}

        COMO_INTERFACE_DECL();

        ECode HasNext(
            /* [out] */ Boolean* result) override;

        ECode Next(
            /* [out] */ IInterface** object = nullptr) override;

        ECode Remove() override;

    protected:
        ECode CheckForComodification();

    protected:
        AbstractList* mOwner;

        /**
         * Index of element to be returned by subsequent call to next.
         */
        Integer mCursor = 0;

        /**
         * Index of element returned by most recent call to next or
         * previous.  Reset to -1 if this element is deleted by a call
         * to remove.
         */
        Integer mLastRet = -1;

        /**
         * The modCount value that the iterator believes that the backing
         * List should have.  If this expectation is violated, the iterator
         * has detected concurrent modification.
         */
        Integer mExpectedModCount;
    };

    class ListItr
        : public Itr
        , public IListIterator
    {
    public:
        ListItr(
            /* [in] */ AbstractList* owner,
            /* [in] */ Integer index)
            : Itr(owner)
        {
            mCursor = index;
        }

        COMO_INTERFACE_DECL();

        ECode HasPrevious(
            /* [out] */ Boolean* result) override;

        ECode Previous(
            /* [out] */ IInterface** object = nullptr) override;

        ECode GetNextIndex(
            /* [out] */ Integer* index) override;

        ECode GetPreviousIndex(
            /* [out] */ Integer* index) override;

        ECode Set(
            /* [in] */ IInterface* object) override;

        ECode Add(
            /* [in] */ IInterface* object) override;

        ECode HasNext(
            /* [out] */ Boolean* result) override;

        ECode Next(
            /* [out] */ IInterface** object = nullptr) override;

        ECode Remove() override;
    };

public:
    COMO_INTERFACE_DECL();

    ECode Add(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result = nullptr) override;

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

    ECode Clear() override;

    ECode AddAll(
        /* [in] */ Integer index,
        /* [in] */ ICollection* c,
        /* [out] */ Boolean* result = nullptr) override;

    ECode GetIterator(
        /* [out] */ IIterator** it) override;

    ECode GetListIterator(
        /* [out] */ IListIterator** it) override;

    ECode GetListIterator(
        /* [in] */ Integer index,
        /* [out] */ IListIterator** it) override;

    ECode SubList(
        /* [in] */ Integer fromIndex,
        /* [in] */ Integer toIndex,
        /* [out] */ IList** subList) override;

    ECode Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result) override;

    ECode GetHashCode(
        /* [out] */ Integer* hash) override;

    virtual ECode RemoveRange(
        /* [in] */ Integer fromIndex,
        /* [in] */ Integer toIndex);

    ECode AddAll(
        /* [in] */ ICollection* c,
        /* [out] */ Boolean* result = nullptr) override;

    ECode Contains(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result) override;

    ECode ContainsAll(
        /* [in] */ ICollection* c,
        /* [out] */ Boolean* result) override;

    ECode IsEmpty(
        /* [out] */ Boolean* empty) override;

    ECode Remove(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* changed = nullptr) override;

    ECode RemoveAll(
        /* [in] */ ICollection* c,
        /* [out] */ Boolean* changed = nullptr) override;

    ECode RetainAll(
        /* [in] */ ICollection* c,
        /* [out] */ Boolean* changed = nullptr) override;

    ECode ToArray(
        /* [out, callee] */ Array<IInterface*>* objs) override;

    ECode ToArray(
        /* [in] */ const InterfaceID& iid,
        /* [out, callee] */ Array<IInterface*>* objs) override;

    using IList::GetSize;

protected:
    ECode Constructor();

private:
    ECode RangeCheckForAdd(
        /* [in] */ Integer index);

    String OutOfBoundsMsg(
        /* [in] */ Integer index);

public:
    Integer mModCount = 0;
};

}
}

#endif // __COMO_UTIL_ABSTRACTLIST_H__
