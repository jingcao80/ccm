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

#ifndef __COMO_UTIL_ARRAYLIST_H__
#define __COMO_UTIL_ARRAYLIST_H__

#include "como/util/AbstractList.h"
#include "como.core.ICloneable.h"
#include "como.core.IInteger.h"
#include "como.io.ISerializable.h"
#include "como.util.IArrayList.h"
#include "como.util.IRandomAccess.h"
#include <ccmautoptr.h>

using como::core::ICloneable;
using como::core::IInteger;
using como::io::ISerializable;

namespace como {
namespace util {

class ArrayList
    : public AbstractList
    , public IArrayList
    , public IRandomAccess
    , public ICloneable
    , public ISerializable

{
private:
    class Itr
        : public LightRefBase
        , public IIterator
    {
    public:
        Itr(
            /* [in] */ ArrayList* owner)
            : mOwner(owner)
            , mLimit(owner->mSize)
            , mExpectedModCount(owner->mModCount)
        {}

        COMO_INTERFACE_DECL();

        ECode HasNext(
            /* [out] */ Boolean* result) override;

        ECode Next(
            /* [out] */ IInterface** object = nullptr) override;

        ECode Remove() override;

    protected:
        ArrayList* mOwner;
        Integer mLimit;
        Integer mCursor = 0;
        Integer mLastRet = -1;
        Integer mExpectedModCount;
    };

    class ListItr
        : public Itr
        , public IListIterator
    {
    public:
        ListItr(
            /* [in] */ ArrayList* owner,
            /* [in] */ Integer index)
            : Itr(owner)
        {
            mCursor = index;
        }

        COMO_INTERFACE_DECL();

        ECode HasPrevious(
            /* [out] */ Boolean* result) override;

        ECode GetNextIndex(
            /* [out] */ Integer* index) override;

        ECode GetPreviousIndex(
            /* [out] */ Integer* index) override;

        ECode Previous(
            /* [out] */ IInterface** object = nullptr) override;

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

    class Sublist
        : public AbstractList
        , public IRandomAccess
    {
    public:
        COMO_INTERFACE_DECL();

        ECode Constructor(
            /* [in] */ ArrayList* owner,
            /* [in] */ AbstractList* parent,
            /* [in] */ Integer offset,
            /* [in] */ Integer fromIndex,
            /* [in] */ Integer toIndex);

        ECode Set(
            /* [in] */ Integer index,
            /* [in] */ IInterface* obj,
            /* [out] */ IInterface** prevObj = nullptr) override;

        ECode Get(
            /* [in] */ Integer index,
            /* [out] */ IInterface** obj) override;

        ECode GetSize(
            /* [out] */ Integer* size) override;

        ECode Add(
            /* [in] */ Integer index,
            /* [in] */ IInterface* obj) override;

        ECode Remove(
            /* [in] */ Integer index,
            /* [out] */ IInterface** obj = nullptr) override;

        ECode AddAll(
            /* [in] */ ICollection* c,
            /* [out] */ Boolean* result = nullptr) override;

        ECode AddAll(
            /* [in] */ Integer index,
            /* [in] */ ICollection* c,
            /* [out] */ Boolean* result = nullptr) override;

        ECode GetIterator(
            /* [out] */ IIterator** it) override;

        ECode GetListIterator(
            /* [in] */ Integer index,
            /* [out] */ IListIterator** it) override;

        ECode SubList(
            /* [in] */ Integer fromIndex,
            /* [in] */ Integer toIndex,
            /* [out] */ IList** subList) override;

        using AbstractList::GetListIterator;

    protected:
        ECode RemoveRange(
            /* [in] */ Integer fromIndex,
            /* [in] */ Integer toIndex) override;

    private:
        String OutOfBoundsMsg(
            /* [in] */ Integer index);

    private:
        ArrayList* mOwner;
        AutoPtr<AbstractList> mParent;
        Integer mParentOffset;
        Integer mOffset;
        Integer mSize;
    };

public:
    COMO_INTERFACE_DECL();

    ECode Constructor(
        /* [in] */ Integer initialCapacity);

    ECode Constructor();

    ECode Constructor(
        /* [in] */ ICollection* c);

    ECode TrimToSize() override;

    ECode EnsureCapacity(
        /* [in] */ Integer minCapacity) override;

    ECode GetSize(
        /* [out] */ Integer* size) override;

    ECode IsEmpty(
        /* [out] */ Boolean* empty) override;

    ECode Contains(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result) override;

    ECode IndexOf(
        /* [in] */ IInterface* obj,
        /* [out] */ Integer* index) override;

    ECode LastIndexOf(
        /* [in] */ IInterface* obj,
        /* [out] */ Integer* index) override;

    ECode ToArray(
        /* [out, callee] */ Array<IInterface*>* objs) override;

    ECode ToArray(
        /* [in] */ const InterfaceID& iid,
        /* [out, callee] */ Array<IInterface*>* objs) override;

    ECode Get(
        /* [in] */ Integer index,
        /* [out] */ IInterface** obj) override;

    ECode Set(
        /* [in] */ Integer index,
        /* [in] */ IInterface* obj,
        /* [out] */ IInterface** prevObj = nullptr) override;

    ECode Add(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result = nullptr) override;

    ECode Add(
        /* [in] */ Integer index,
        /* [in] */ IInterface* obj) override;

    ECode Remove(
        /* [in] */ Integer index,
        /* [out] */ IInterface** obj = nullptr) override;

    ECode Remove(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result = nullptr) override;

    ECode Clear() override;

    ECode AddAll(
        /* [in] */ ICollection* c,
        /* [out] */ Boolean* result = nullptr) override;

    ECode AddAll(
        /* [in] */ Integer index,
        /* [in] */ ICollection* c,
        /* [out] */ Boolean* result = nullptr) override;

    ECode RemoveAll(
        /* [in] */ ICollection* c,
        /* [out] */ Boolean* result = nullptr) override;

    ECode RetainAll(
        /* [in] */ ICollection* c,
        /* [out] */ Boolean* result = nullptr) override;

    ECode GetListIterator(
        /* [in] */ Integer index,
        /* [out] */ IListIterator** it) override;

    ECode GetListIterator(
        /* [out] */ IListIterator** it) override;

    ECode GetIterator(
        /* [out] */ IIterator** it) override;

    ECode SubList(
        /* [in] */ Integer fromIndex,
        /* [in] */ Integer toIndex,
        /* [out] */ IList** subList) override;

    ECode ContainsAll(
        /* [in] */ ICollection* c,
        /* [out] */ Boolean* result) override;

    ECode Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result) override;

    ECode GetHashCode(
        /* [out] */ Integer* hash) override;

protected:
    ECode CloneImpl(
        /* [in] */ IArrayList* newObj);

    ECode RemoveRange(
        /* [in] */ Integer fromIndex,
        /* [in] */ Integer toIndex) override;

    static ECode SubListRangeCheck(
        /* [in] */ Integer fromIndex,
        /* [in] */ Integer toIndex,
        /* [in] */ Integer size);

private:
    void EnsureCapacityInternal(
        /* [in] */ Integer minCapacity);

    void EnsureExplicitCapacity(
        /* [in] */ Integer minCapacity);

    void Grow(
        /* [in] */ Integer minCapacity);

    static Integer HugeCapacity(
        /* [in] */ Integer minCapacity);

    void FastRemove(
        /* [in] */ Integer index);

    String OutOfBoundsMsg(
        /* [in] */ Integer index);

    Boolean BatchRemove(
        /* [in] */ ICollection* c,
        /* [in] */ Boolean complement);

    static Array<IInterface*> GetEMPTY_ELEMENTDATA();

    static Array<IInterface*> GetDEFAULTCAPACITY_EMPTY_ELEMENTDATA();

private:
    /**
     * Default initial capacity.
     */
    static constexpr Integer DEFAULT_CAPACITY = 10;

    /**
     * The array buffer into which the elements of the ArrayList are stored.
     * The capacity of the ArrayList is the length of this array buffer. Any
     * empty ArrayList with elementData == DEFAULTCAPACITY_EMPTY_ELEMENTDATA
     * will be expanded to DEFAULT_CAPACITY when the first element is added.
     */
    Array<IInterface*> mElementData;

    /**
     * The size of the ArrayList (the number of elements it contains).
     */
    Integer mSize;

    static constexpr Integer MAX_ARRAY_SIZE = IInteger::MAX_VALUE - 8;
};

}
}

#endif //__COMO_UTIL_ARRAYLIST_H__
