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

#include "como/core/Math.h"
#include "como/util/Arrays.h"
#include "como/util/ArrayList.h"
#include <comolog.h>

using como::core::IID_ICloneable;
using como::core::Math;
using como::io::IID_ISerializable;

namespace como {
namespace util {

COMO_INTERFACE_IMPL_4(ArrayList, AbstractList, IArrayList, IRandomAccess, ICloneable, ISerializable);

Array<IInterface*> ArrayList::GetEMPTY_ELEMENTDATA()
{
    static const Array<IInterface*> EMPTY_ELEMENTDATA(0);
    return EMPTY_ELEMENTDATA;
}

Array<IInterface*> ArrayList::GetDEFAULTCAPACITY_EMPTY_ELEMENTDATA()
{
    static const Array<IInterface*> DEFAULTCAPACITY_EMPTY_ELEMENTDATA(0);
    return DEFAULTCAPACITY_EMPTY_ELEMENTDATA;
}

ECode ArrayList::Constructor(
    /* [in] */ Integer initialCapacity)
{
    if (initialCapacity > 0) {
        mElementData = Array<IInterface*>(initialCapacity);
    }
    else if (initialCapacity == 0) {
        mElementData = GetEMPTY_ELEMENTDATA();
    }
    Logger::E("ArrayList", "Illegal Capacity: %d", initialCapacity);
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode ArrayList::Constructor()
{
    mElementData = GetDEFAULTCAPACITY_EMPTY_ELEMENTDATA();
    return NOERROR;
}

ECode ArrayList::Constructor(
    /* [in] */ ICollection* c)
{
    c->ToArray(&mElementData);
    if (mSize = mElementData.GetLength(), mSize == 0) {
        mElementData = GetEMPTY_ELEMENTDATA();
    }
    return NOERROR;
}

ECode ArrayList::TrimToSize()
{
    mModCount++;
    if (mSize < mElementData.GetLength()) {
        if (mSize != 0) {
            Arrays::CopyOf(mElementData, mSize, &mElementData);
        }
        else {
            mElementData = GetEMPTY_ELEMENTDATA();
        }
    }
    return NOERROR;
}

ECode ArrayList::EnsureCapacity(
    /* [in] */ Integer minCapacity)
{
    Integer minExpand = (!mElementData.Equals(GetDEFAULTCAPACITY_EMPTY_ELEMENTDATA()))
            // any size if not default element table
            ? 0
            // larger than default for default empty table. It's already
            // supposed to be at default size.
            : DEFAULT_CAPACITY;

    if (minCapacity > minExpand) {
        EnsureExplicitCapacity(minCapacity);
    }
    return NOERROR;
}

void ArrayList::EnsureCapacityInternal(
    /* [in] */ Integer minCapacity)
{
    if (mElementData.Equals(GetDEFAULTCAPACITY_EMPTY_ELEMENTDATA())) {
        minCapacity = Math::Max(DEFAULT_CAPACITY, minCapacity);
    }

    EnsureExplicitCapacity(minCapacity);
}

void ArrayList::EnsureExplicitCapacity(
    /* [in] */ Integer minCapacity)
{
    mModCount++;

    if (minCapacity - mElementData.GetLength() > 0) {
        Grow(minCapacity);
    }
}

void ArrayList::Grow(
    /* [in] */ Integer minCapacity)
{
    Integer oldCapacity = mElementData.GetLength();
    Integer newCapacity = oldCapacity + (oldCapacity >> 1);
    if (newCapacity - minCapacity < 0) {
        newCapacity = minCapacity;
    }
    if (newCapacity - MAX_ARRAY_SIZE > 0) {
        newCapacity = HugeCapacity(minCapacity);
    }
    Arrays::CopyOf(mElementData, newCapacity, &mElementData);
}

Integer ArrayList::HugeCapacity(
    /* [in] */ Integer minCapacity)
{
    return (minCapacity > MAX_ARRAY_SIZE) ?
            IInteger::MAX_VALUE : MAX_ARRAY_SIZE;
}

ECode ArrayList::GetSize(
    /* [out] */ Integer* size)
{
    VALIDATE_NOT_NULL(size);

    *size = mSize;
    return NOERROR;
}

ECode ArrayList::IsEmpty(
    /* [out] */ Boolean* empty)
{
    VALIDATE_NOT_NULL(empty);

    *empty = mSize == 0;
    return NOERROR;
}

ECode ArrayList::Contains(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Integer index;
    IndexOf(obj, &index);
    *result = index >= 0;
    return NOERROR;
}

ECode ArrayList::IndexOf(
    /* [in] */ IInterface* obj,
    /* [out] */ Integer* index)
{
    VALIDATE_NOT_NULL(index);

    if (obj == nullptr) {
        for (Integer i = 0; i < mSize; i++) {
            if (mElementData[i] == nullptr) {
                *index = i;
                return NOERROR;
            }
        }
    }
    else {
        for (Integer i = 0; i < mSize; i++) {
            if (Object::Equals(obj, mElementData[i])) {
                *index = i;
                return NOERROR;
            }
        }
    }
    *index = -1;
    return NOERROR;
}

ECode ArrayList::LastIndexOf(
    /* [in] */ IInterface* obj,
    /* [out] */ Integer* index)
{
    VALIDATE_NOT_NULL(index);

    if (obj == nullptr) {
        for (Integer i = mSize - 1; i >= 0; i--) {
            if (mElementData[i] == nullptr) {
                *index = i;
                return NOERROR;
            }
        }
    }
    else {
        for (Integer i = mSize - 1; i >= 0; i--) {
            if (Object::Equals(obj, mElementData[i])) {
                *index = i;
                return NOERROR;
            }
        }
    }
    *index = -1;
    return NOERROR;
}

ECode ArrayList::CloneImpl(
    /* [in] */ IArrayList* newObj)
{
    ArrayList* v = (ArrayList*)newObj;
    v->mSize = mSize;
    Arrays::CopyOf(mElementData, mSize, &v->mElementData);
    v->mModCount = 0;
    return NOERROR;
}

ECode ArrayList::ToArray(
    /* [out, callee] */ Array<IInterface*>* objs)
{
    return Arrays::CopyOf(mElementData, mSize, objs);
}

ECode ArrayList::ToArray(
    /* [in] */ const InterfaceID& iid,
    /* [out, callee] */ Array<IInterface*>* objs)
{
    VALIDATE_NOT_NULL(objs);

    Array<IInterface*> newArray(mSize);
    for (Integer i = 0; i < mSize; i++) {
        newArray.Set(i, mElementData[i] != nullptr ?
                mElementData[i]->Probe(iid) : nullptr);
    }
    *objs = newArray;
    return NOERROR;
}

ECode ArrayList::Get(
    /* [in] */ Integer index,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    if (index > mSize) {
        Logger::E("ArrayList", "%s", OutOfBoundsMsg(index).string());
        return como::core::E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    *obj = mElementData[index];
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

ECode ArrayList::Set(
    /* [in] */ Integer index,
    /* [in] */ IInterface* obj,
    /* [out] */ IInterface** prevObj)
{
    if (index > mSize) {
        Logger::E("ArrayList", "%s", OutOfBoundsMsg(index).string());
        return como::core::E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    if (prevObj != nullptr) {
        *prevObj = mElementData[index];
        REFCOUNT_ADD(*prevObj);
    }
    mElementData.Set(index, obj);
    return NOERROR;
}

ECode ArrayList::Add(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    EnsureCapacityInternal(mSize + 1);
    mElementData.Set(mSize++, obj);
    if (result != nullptr) *result = true;
    return NOERROR;
}

ECode ArrayList::Add(
    /* [in] */ Integer index,
    /* [in] */ IInterface* obj)
{
    if (index > mSize || index < 0) {
        Logger::E("ArrayList", "%s", OutOfBoundsMsg(index).string());
        return como::core::E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    EnsureCapacityInternal(mSize + 1);
    for (Integer i = mSize; i > index; i--) {
        mElementData.Set(i, mElementData[i - 1]);
    }
    mElementData.Set(index, obj);
    mSize++;
    return NOERROR;
}

ECode ArrayList::Remove(
    /* [in] */ Integer index,
    /* [out] */ IInterface** obj)
{
    if (index > mSize) {
        Logger::E("ArrayList", "%s", OutOfBoundsMsg(index).string());
        return como::core::E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    mModCount++;
    if (obj != nullptr) {
        *obj = mElementData[index];
        REFCOUNT_ADD(*obj);
    }

    Integer numMoved = mSize - index - 1;
    if (numMoved > 0) {
        for (Integer i = index; i < mSize - 1; i++) {
            mElementData.Set(i, mElementData[i + 1]);
        }
    }
    mElementData.Set(--mSize, nullptr);
    return NOERROR;
}

ECode ArrayList::Remove(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    if (obj == nullptr) {
        for (Integer index = 0; index < mSize; index++) {
            if (mElementData[index] == nullptr) {
                FastRemove(index);
                if (result != nullptr) *result = true;
                return NOERROR;
            }
        }
    }
    else {
        for (Integer index = 0; index < mSize; index++) {
            if (Object::Equals(obj, mElementData[index])) {
                FastRemove(index);
                if (result != nullptr) *result = true;
                return NOERROR;
            }
        }
    }
    if (result != nullptr) *result = false;
    return NOERROR;
}

void ArrayList::FastRemove(
    /* [in] */ Integer index)
{
    mModCount++;
    Integer numMoved = mSize - index - 1;
    if (numMoved > 0) {
        for (Integer i = index; i < mSize - 1; i++) {
            mElementData.Set(i, mElementData[i + 1]);
        }
    }
    mElementData.Set(--mSize, nullptr);
}

ECode ArrayList::Clear()
{
    mModCount++;

    for (Integer i = 0; i < mSize; i++) {
        mElementData.Set(i, nullptr);
    }

    mSize = 0;
    return NOERROR;
}

ECode ArrayList::AddAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* result)
{
    Array<IInterface*> a;
    c->ToArray(&a);
    Integer numNew = a.GetLength();
    EnsureCapacityInternal(mSize + numNew);
    mElementData.Copy(mSize, a, 0, numNew);
    mSize += numNew;
    if (result != nullptr) *result = numNew != 0;
    return NOERROR;
}

ECode ArrayList::AddAll(
    /* [in] */ Integer index,
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* result)
{
    if (index > mSize || index < 0) {
        Logger::E("ArrayList", "%s", OutOfBoundsMsg(index).string());
        return como::core::E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    Array<IInterface*> a;
    c->ToArray(&a);
    Integer numNew = a.GetLength();
    EnsureCapacityInternal(mSize + numNew);

    if (numNew > 0) {
        for (Integer i = mSize - 1; i > mSize - numNew - 1; i--) {
            mElementData.Set(i, mElementData[i - numNew]);
        }
    }

    mElementData.Copy(index, a, 0, numNew);
    mSize += numNew;
    if (result != nullptr) *result = numNew != 0;
    return NOERROR;
}

ECode ArrayList::RemoveRange(
    /* [in] */ Integer fromIndex,
    /* [in] */ Integer toIndex)
{
    if (toIndex < fromIndex) {
        Logger::E("ArrayList", "toIndex < fromIndex");
        return como::core::E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    mModCount++;
    Integer numMoved = mSize - toIndex;
    for (Integer i = 0; i <= numMoved; i++) {
        mElementData.Set(fromIndex + i, mElementData[toIndex + i]);
    }

    Integer newSize = mSize - (toIndex - fromIndex);
    for (Integer i = newSize; i < mSize; i++) {
        mElementData.Set(i, nullptr);
    }
    mSize = newSize;
    return NOERROR;
}

String ArrayList::OutOfBoundsMsg(
    /* [in] */ Integer index)
{
    return String::Format("Index: %d, Size: %d", index, mSize);
}

ECode ArrayList::RemoveAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* result)
{
    Boolean ret = BatchRemove(c, false);
    if (result != nullptr) *result = ret;
    return NOERROR;
}

ECode ArrayList::RetainAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* result)
{
    Boolean ret = BatchRemove(c, true);
    if (result != nullptr) *result = ret;
    return NOERROR;
}

Boolean ArrayList::BatchRemove(
    /* [in] */ ICollection* c,
    /* [in] */ Boolean complement)
{
    Integer r = 0, w = 0;
    Boolean modified = false;
    for (; r < mSize; r++) {
        Boolean contained;
        if (c->Contains(mElementData[r], &contained), contained == complement) {
            mElementData.Set(w++, mElementData[r]);
        }
    }
    if (w != mSize) {
        for (Integer i = w; i < mSize; i++) {
            mElementData.Set(i, nullptr);
        }
        mModCount += mSize - w;
        mSize = w;
        modified = true;
    }
    return modified;
}

ECode ArrayList::GetListIterator(
    /* [in] */ Integer index,
    /* [out] */ IListIterator** it)
{
    VALIDATE_NOT_NULL(it);

    if (index < 0 || index > mSize) {
        Logger::E("ArrayList", "%s", OutOfBoundsMsg(index).string());
        return como::core::E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    *it = new ListItr(this, index);
    REFCOUNT_ADD(*it);
    return NOERROR;
}

ECode ArrayList::GetListIterator(
    /* [out] */ IListIterator** it)
{
    VALIDATE_NOT_NULL(it);

    *it = new ListItr(this, 0);
    REFCOUNT_ADD(*it);
    return NOERROR;
}

ECode ArrayList::GetIterator(
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it);

    *it = new Itr(this);
    REFCOUNT_ADD(*it);
    return NOERROR;
}

ECode ArrayList::SubList(
    /* [in] */ Integer fromIndex,
    /* [in] */ Integer toIndex,
    /* [out] */ IList** subList)
{
    VALIDATE_NOT_NULL(subList);

    FAIL_RETURN(SubListRangeCheck(fromIndex, toIndex, mSize));
    AutoPtr<Sublist> l = new Sublist();
    l->Constructor(this, this, 0, fromIndex, toIndex);
    *subList = l;
    REFCOUNT_ADD(*subList);
    return NOERROR;
}

ECode ArrayList::SubListRangeCheck(
    /* [in] */ Integer fromIndex,
    /* [in] */ Integer toIndex,
    /* [in] */ Integer size)
{
    if (fromIndex < 0 || toIndex > size || fromIndex > toIndex) {
        Logger::E("ArrayList", "fromIndex: %d, toIndex: %d", fromIndex, toIndex);
        return como::core::E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    return NOERROR;
}

ECode ArrayList::ContainsAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* result)
{
    return AbstractList::ContainsAll(c, result);
}

ECode ArrayList::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean& result)
{
    return AbstractList::Equals(obj, result);
}

ECode ArrayList::GetHashCode(
    /* [out] */ Integer& hash)
{
    return AbstractList::GetHashCode(hash);
}

//---------------------------------------------------------------------------------

COMO_INTERFACE_IMPL_LIGHT_1(ArrayList::Itr, LightRefBase, IIterator);

ECode ArrayList::Itr::HasNext(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mCursor < mLimit;
    return NOERROR;
}

ECode ArrayList::Itr::Next(
    /* [out] */ IInterface** object)
{
    if (mOwner->mModCount != mExpectedModCount) {
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }
    Integer i = mCursor;
    if (i >= mLimit) {
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }
    Array<IInterface*>& elementData = mOwner->mElementData;
    if (i > elementData.GetLength()) {
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }
    mCursor = i + 1;
    if (*object != nullptr) {
        *object = elementData[mLastRet = i];
        REFCOUNT_ADD(*object);
    }
    return NOERROR;
}

ECode ArrayList::Itr::Remove()
{
    if (mLastRet < 0) {
        return como::core::E_ILLEGAL_STATE_EXCEPTION;
    }
    if (mOwner->mModCount != mExpectedModCount) {
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }

    ECode ec = mOwner->Remove(mLastRet);
    if (FAILED(ec)) {
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }
    mCursor = mLastRet;
    mLastRet = -1;
    mExpectedModCount = mOwner->mModCount;
    mLimit--;
    return NOERROR;
}

//---------------------------------------------------------------------------------

COMO_INTERFACE_IMPL_LIGHT_1(ArrayList::ListItr, Itr, IListIterator);

ECode ArrayList::ListItr::HasPrevious(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mCursor != 0;
    return NOERROR;
}

ECode ArrayList::ListItr::GetNextIndex(
    /* [out] */ Integer* index)
{
    VALIDATE_NOT_NULL(index);

    *index = mCursor;
    return NOERROR;
}

ECode ArrayList::ListItr::GetPreviousIndex(
    /* [out] */ Integer* index)
{
    VALIDATE_NOT_NULL(index);

    *index = mCursor - 1;
    return NOERROR;
}

ECode ArrayList::ListItr::Previous(
    /* [out] */ IInterface** object)
{
    if (mOwner->mModCount != mExpectedModCount) {
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }
    Integer i = mCursor - 1;
    if (i < 0) {
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }
    Array<IInterface*>& elementData = mOwner->mElementData;
    if (i > elementData.GetLength()) {
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }
    mCursor = i;
    if (object != nullptr) {
        *object = elementData[mLastRet = i];
        REFCOUNT_ADD(*object);
    }
    return NOERROR;
}

ECode ArrayList::ListItr::Set(
    /* [in] */ IInterface* object)
{
    if (mLastRet < 0) {
        return como::core::E_ILLEGAL_STATE_EXCEPTION;
    }
    if (mOwner->mModCount != mExpectedModCount) {
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }

    ECode ec = mOwner->Set(mLastRet, object);
    if (FAILED(ec)) {
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }
    return NOERROR;
}

ECode ArrayList::ListItr::Add(
    /* [in] */ IInterface* object)
{
    if (mOwner->mModCount != mExpectedModCount) {
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }

    Integer i = mCursor;
    ECode ec = mOwner->Add(i, object);
    if (FAILED(ec)) {
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }
    mCursor = i + 1;
    mLastRet = -1;
    mExpectedModCount = mOwner->mModCount;
    mLimit++;
    return NOERROR;
}

ECode ArrayList::ListItr::HasNext(
    /* [out] */ Boolean* result)
{
    return Itr::HasNext(result);
}

ECode ArrayList::ListItr::Next(
    /* [out] */ IInterface** object)
{
    return Itr::Next(object);
}

ECode ArrayList::ListItr::Remove()
{
    return Itr::Remove();
}

//---------------------------------------------------------------------------------

COMO_INTERFACE_IMPL_1(ArrayList::Sublist, AbstractList, IRandomAccess);

ECode ArrayList::Sublist::Constructor(
    /* [in] */ ArrayList* owner,
    /* [in] */ AbstractList* parent,
    /* [in] */ Integer offset,
    /* [in] */ Integer fromIndex,
    /* [in] */ Integer toIndex)
{
    mOwner = owner;
    mParent = parent;
    mParentOffset = fromIndex;
    mOffset = offset + fromIndex;
    mSize = toIndex - fromIndex;
    mModCount = owner->mModCount;
    return NOERROR;
}

ECode ArrayList::Sublist::Set(
    /* [in] */ Integer index,
    /* [in] */ IInterface* obj,
    /* [out] */ IInterface** prevObj)
{
    if (index < 0 || index > mSize) {
        Logger::E("ArrayList::Sublist", "%s", OutOfBoundsMsg(index).string());
        return como::core::E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    if (mOwner->mModCount != mModCount) {
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }
    Array<IInterface*>& elementData = mOwner->mElementData;
    if (prevObj != nullptr) {
        *prevObj = elementData[mOffset + index];
        REFCOUNT_ADD(*prevObj);
    }
    elementData.Set(mOffset + index, obj);
    return NOERROR;
}

ECode ArrayList::Sublist::Get(
    /* [in] */ Integer index,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    if (index < 0 || index > mSize) {
        Logger::E("ArrayList::Sublist", "%s", OutOfBoundsMsg(index).string());
        return como::core::E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    if (mOwner->mModCount != mModCount) {
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }
    *obj = mOwner->mElementData[mOffset + index];
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

ECode ArrayList::Sublist::GetSize(
    /* [out] */ Integer* size)
{
    VALIDATE_NOT_NULL(size);

    if (mOwner->mModCount != mModCount) {
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }
    *size = mSize;
    return NOERROR;
}

ECode ArrayList::Sublist::Add(
    /* [in] */ Integer index,
    /* [in] */ IInterface* obj)
{
    if (index < 0 || index > mSize) {
        Logger::E("ArrayList::Sublist", "%s", OutOfBoundsMsg(index).string());
        return como::core::E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    if (mOwner->mModCount != mModCount) {
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }
    mParent->Add(mParentOffset + index, obj);
    mModCount = mParent->mModCount;
    mSize++;
    return NOERROR;
}

ECode ArrayList::Sublist::Remove(
    /* [in] */ Integer index,
    /* [out] */ IInterface** obj)
{
    if (index < 0 || index > mSize) {
        Logger::E("ArrayList::Sublist", "%s", OutOfBoundsMsg(index).string());
        return como::core::E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    if (mOwner->mModCount != mModCount) {
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }
    mParent->Remove(mParentOffset + index, obj);
    mModCount = mParent->mModCount;
    mSize--;
    return NOERROR;
}

ECode ArrayList::Sublist::RemoveRange(
    /* [in] */ Integer fromIndex,
    /* [in] */ Integer toIndex)
{
    if (mOwner->mModCount != mModCount) {
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }
    mParent->RemoveRange(mParentOffset + fromIndex,
            mParentOffset + toIndex);
    mModCount = mParent->mModCount;
    mSize -= toIndex - fromIndex;
    return NOERROR;
}

ECode ArrayList::Sublist::AddAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* result)
{
    return AddAll(mSize, c);
}

ECode ArrayList::Sublist::AddAll(
    /* [in] */ Integer index,
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* result)
{
    if (index < 0 || index > mSize) {
        Logger::E("ArrayList::Sublist", "%s", OutOfBoundsMsg(index).string());
        return como::core::E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    Integer cSize;
    c->GetSize(&cSize);
    if (cSize == 0) {
        if (result != nullptr) *result = false;
        return NOERROR;
    }

    if (mOwner->mModCount != mModCount) {
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }
    mParent->AddAll(mParentOffset + index, c);
    mModCount = mParent->mModCount;
    mSize += cSize;
    if (result != nullptr) *result = true;
    return NOERROR;
}

ECode ArrayList::Sublist::GetIterator(
    /* [out] */ AutoPtr<IIterator>& it)
{
    AutoPtr<IListIterator> lit;
    GetListIterator(&lit);
    it = std::move(lit);
    return NOERROR;
}

ECode ArrayList::Sublist::GetListIterator(
    /* [in] */ Integer index,
    /* [out] */ IListIterator** it)
{
    VALIDATE_NOT_NULL(it);

    if (index < 0 || index > mSize) {
        Logger::E("ArrayList::Sublist", "%s", OutOfBoundsMsg(index).string());
        return como::core::E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    if (mOwner->mModCount != mModCount) {
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }
    Integer offset = mOffset;

    class _ListIterator
        : public LightRefBase
        , public IListIterator
        , public IIterator
    {
    public:
        _ListIterator(
            /* [in] */ Sublist* owner,
            /* [in] */ Integer index,
            /* [in] */ Integer offset)
            : mOwner(owner)
            , mCursor(index)
            , mExpectedModCount(owner->mOwner->mModCount)
            , mOffset(offset)
        {}

        Integer AddRef(
            /* [in] */ HANDLE id = 0) override
        {
            return LightRefBase::AddRef(id);
        }

        Integer Release(
            /* [in] */ HANDLE id = 0) override
        {
            return LightRefBase::Release(id);
        }

        IInterface* Probe(
            /* [in] */ const InterfaceID& iid) override
        {
            if (iid == IID_IInterface) {
                return (IInterface*)(IIterator*)this;
            }
            else if (iid == IID_IListIterator) {
                return (IListIterator*)this;
            }
            else if (iid == IID_IIterator) {
                return (IIterator*)this;
            }
            return nullptr;
        }

        ECode GetInterfaceID(
            /* [in] */ IInterface* object,
            /* [out] */ InterfaceID& iid) override
        {
            if (object == (IInterface*)(IListIterator*)this) {
                iid = IID_IListIterator;
                return NOERROR;
            }
            else if (object == (IInterface*)(IIterator*)this) {
                iid = IID_IIterator;
                return NOERROR;
            }
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        ECode HasNext(
            /* [out] */ Boolean* result) override
        {
            VALIDATE_NOT_NULL(result);

            *result = mCursor != mOwner->mSize;
            return NOERROR;
        }

        ECode Next(
            /* [out] */ IInterface** object = nullptr) override
        {
            if (mExpectedModCount != mOwner->mOwner->mModCount) {
                return E_CONCURRENT_MODIFICATION_EXCEPTION;
            }
            Integer i = mCursor;
            if (i >= mOwner->mSize) {
                return E_NO_SUCH_ELEMENT_EXCEPTION;
            }
            Array<IInterface*>& elementData = mOwner->mOwner->mElementData;
            if (mOffset + i >= elementData.GetLength()) {
                return E_CONCURRENT_MODIFICATION_EXCEPTION;
            }
            mCursor = i + 1;
            if (object != nullptr) {
                *object = elementData[mOffset + (mLastRet = i)];
                REFCOUNT_ADD(*object);
            }
            return NOERROR;
        }

        ECode HasPrevious(
            /* [out] */ Boolean* result) override
        {
            VALIDATE_NOT_NULL(result);

            *result = mCursor != 0;
            return NOERROR;
        }

        ECode Previous(
            /* [out] */ IInterface** object = nullptr) override
        {
            if (mExpectedModCount != mOwner->mOwner->mModCount) {
                return E_CONCURRENT_MODIFICATION_EXCEPTION;
            }
            Integer i = mCursor - 1;
            if (i < 0) {
                return E_NO_SUCH_ELEMENT_EXCEPTION;
            }
            Array<IInterface*>& elementData = mOwner->mOwner->mElementData;
            if (mOffset + i >= elementData.GetLength()) {
                return E_CONCURRENT_MODIFICATION_EXCEPTION;
            }
            mCursor = i;
            if (object != nullptr) {
                *object = elementData[mOffset + (mLastRet = i)];
                REFCOUNT_ADD(*object);
            }
            return NOERROR;
        }

        ECode GetNextIndex(
            /* [out] */ Integer* index) override
        {
            VALIDATE_NOT_NULL(index);

            *index = mCursor;
            return NOERROR;
        }

        ECode GetPreviousIndex(
            /* [out] */ Integer* index) override
        {
            VALIDATE_NOT_NULL(index);

            *index = mCursor - 1;
            return NOERROR;
        }

        ECode Remove() override
        {
            if (mLastRet < 0) {
                return como::core::E_ILLEGAL_STATE_EXCEPTION;
            }
            if (mExpectedModCount != mOwner->mOwner->mModCount) {
                return E_CONCURRENT_MODIFICATION_EXCEPTION;
            }

            ECode ec = mOwner->Remove(mLastRet);
            if (FAILED(ec)) {
                return E_CONCURRENT_MODIFICATION_EXCEPTION;
            }
            mCursor = mLastRet;
            mLastRet = -1;
            mExpectedModCount = mOwner->mOwner->mModCount;
            return NOERROR;
        }

        ECode Set(
            /* [in] */ IInterface* object) override
        {
            if (mLastRet < 0) {
                return como::core::E_ILLEGAL_STATE_EXCEPTION;
            }
            if (mExpectedModCount != mOwner->mOwner->mModCount) {
                return E_CONCURRENT_MODIFICATION_EXCEPTION;
            }

            ECode ec = mOwner->mOwner->Set(mOffset + mLastRet, object);
            if (FAILED(ec)) {
                return E_CONCURRENT_MODIFICATION_EXCEPTION;
            }
            return NOERROR;
        }

        ECode Add(
            /* [in] */ IInterface* object) override
        {
            if (mExpectedModCount != mOwner->mOwner->mModCount) {
                return E_CONCURRENT_MODIFICATION_EXCEPTION;
            }

            Integer i = mCursor;
            ECode ec = mOwner->Add(i, object);
            if (FAILED(ec)) {
                return E_CONCURRENT_MODIFICATION_EXCEPTION;
            }
            mCursor = i + 1;
            mLastRet = -1;
            mExpectedModCount = mOwner->mOwner->mModCount;
            return NOERROR;
        }

    private:
        Sublist* mOwner;
        Integer mCursor;
        Integer mLastRet = -1;
        Integer mExpectedModCount;
        Integer mOffset;
    };

    *it = new _ListIterator(this, index, offset);
    REFCOUNT_ADD(*it);
    return NOERROR;
}

ECode ArrayList::Sublist::SubList(
    /* [in] */ Integer fromIndex,
    /* [in] */ Integer toIndex,
    /* [out] */ IList** subList)
{
    VALIDATE_NOT_NULL(subList);

    FAIL_RETURN(SubListRangeCheck(fromIndex, toIndex, mSize));
    AutoPtr<Sublist> l = new Sublist();
    l->Constructor(mOwner, this, mOffset, fromIndex, toIndex);
    *subList = l.Get();
    REFCOUNT_ADD(*subList);
    return NOERROR;
}

String ArrayList::Sublist::OutOfBoundsMsg(
    /* [in] */ Integer index)
{
    return String::Format("Index: %d, Size: %d", index, mSize);
}

}
}
