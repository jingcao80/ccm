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

#include "como/core/AutoLock.h"
#include "como/util/Arrays.h"
#include "como/util/Collections.h"
#include "como/util/Vector.h"
#include <comolog.h>

using como::core::AutoLock;
using como::core::E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
using como::core::E_ILLEGAL_STATE_EXCEPTION;
using como::core::E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
using como::core::IID_ICloneable;
using como::io::IID_ISerializable;

namespace como {
namespace util {

COMO_INTERFACE_IMPL_4(Vector, AbstractList, IVector, IRandomAccess, ICloneable, ISerializable);

ECode Vector::Constructor(
    /* [in] */ Integer initialCapacity,
    /* [in] */ Integer capacityIncrement)
{
    if (initialCapacity < 0) {
        Logger::E("Vector", "Illegal Capacity: %d", initialCapacity);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mElementData = Array<IInterface*>(initialCapacity);
    mCapacityIncrement = capacityIncrement;
    return NOERROR;
}

ECode Vector::Constructor(
    /* [in] */ Integer initialCapacity)
{
    return Constructor(initialCapacity, 0);
}

ECode Vector::Constructor()
{
    return Constructor(10);
}

ECode Vector::Constructor(
    /* [in] */ ICollection* c)
{
    c->ToArray(&mElementData);
    mElementCount = mElementData.GetLength();
    return NOERROR;
}

ECode Vector::CopyInto(
    /* [out] */ Array<IInterface*>& anArray)
{
    AutoLock lock(this);

    anArray.Copy(0, mElementData, 0, mElementCount);
    return NOERROR;
}

ECode Vector::TrimToSize()
{
    AutoLock lock(this);

    mModCount++;
    Integer oldCapacity = mElementData.GetLength();
    if (mElementCount < oldCapacity) {
        Arrays::CopyOf(mElementData, mElementCount, &mElementData);
    }
    return NOERROR;
}

ECode Vector::EnsureCapacity(
    /* [in] */ Integer minCapacity)
{
    AutoLock lock(this);

    if (minCapacity > 0) {
        mModCount++;
        EnsureCapacityHelper(minCapacity);
    }
    return NOERROR;
}

void Vector::EnsureCapacityHelper(
    /* [in] */ Integer minCapacity)
{
    if (minCapacity - mElementData.GetLength() > 0) {
        Grow(minCapacity);
    }
}

void Vector::Grow(
    /* [in] */ Integer minCapacity)
{
    Integer oldCapacity = mElementData.GetLength();
    Integer newCapacity = oldCapacity + ((mCapacityIncrement > 0) ?
            mCapacityIncrement : oldCapacity);
    if (newCapacity - minCapacity < 0) {
        newCapacity = minCapacity;
    }
    if (newCapacity - MAX_ARRAY_SIZE > 0) {
        newCapacity = HugeCapacity(minCapacity);
    }
    Arrays::CopyOf(mElementData, newCapacity, &mElementData);
}

Integer Vector::HugeCapacity(
    /* [in] */ Integer minCapacity)
{
    return (minCapacity > MAX_ARRAY_SIZE) ? IInteger::MAX_VALUE : MAX_ARRAY_SIZE;
}

ECode Vector::SetSize(
    /* [in] */ Integer newSize)
{
    AutoLock lock(this);

    mModCount++;
    if (newSize > mElementCount) {
        EnsureCapacityHelper(newSize);
    }
    else {
        for (Integer i = newSize; i < mElementCount; i++) {
            mElementData.Set(i, nullptr);
        }
    }
    mElementCount = newSize;
    return NOERROR;
}

ECode Vector::GetCapacity(
    /* [out] */ Integer* capacity)
{
    VALIDATE_NOT_NULL(capacity);

    AutoLock lock(this);

    *capacity = mElementData.GetLength();
    return NOERROR;
}

ECode Vector::GetSize(
    /* [out] */ Integer& size)
{
    AutoLock lock(this);
    size = mElementCount;
    return NOERROR;
}

ECode Vector::IsEmpty(
    /* [out] */ Boolean& empty)
{
    AutoLock lock(this);
    empty = mElementCount == 0;
    return NOERROR;
}

ECode Vector::GetElements(
    /* [out] */ IEnumeration** elements)
{
    VALIDATE_NOT_NULL(elements);

    class _Enumeration
        : public SyncObject
        , public IEnumeration
    {
    public:
        _Enumeration(
            /* [in] */ Vector* owner)
            : mOwner(owner)
        {}

        Integer AddRef(
            /* [in] */ HANDLE id) override
        {
            return Object::AddRef(id);
        }

        Integer Release(
            /* [in] */ HANDLE id) override
        {
            return Object::Release(id);
        }

        IInterface* Probe(
            /* [in] */ const InterfaceID& iid) override
        {
            if (iid == IID_IInterface) {
                return (IInterface*)(IEnumeration*)this;
            }
            else if (iid == IID_IEnumeration) {
                return (IEnumeration*)this;
            }
            return SyncObject::Probe(iid);
        }

        ECode GetInterfaceID(
            /* [in] */ IInterface* object,
            /* [out] */ InterfaceID& iid) override
        {
            if (object == (IInterface*)(IEnumeration*)this) {
                iid = IID_IEnumeration;
                return NOERROR;
            }
            return SyncObject::GetInterfaceID(object, iid);
        }

        ECode HasMoreElements(
            /* [out] */ Boolean& result) override
        {
            result = mCount < mOwner->mElementCount;
            return NOERROR;
        }

        ECode NextElement(
            /* [out] */ IInterface** object = nullptr) override
        {
            AutoLock lock(mOwner);

            if (mCount < mOwner->mElementCount) {
                mCount++;
                if (object != nullptr) {
                    *object = mOwner->ElementData(mCount);
                    REFCOUNT_ADD(*object)
                }
                return NOERROR;
            }
            Logger::E("Vector", "Vector Enumeration");
            return E_NO_SUCH_ELEMENT_EXCEPTION;
        }

    private:
        Integer mCount = 0;
        AutoPtr<Vector> mOwner;
    };

    AutoLock lock(this);

    *elements = new _Enumeration(this);
    REFCOUNT_ADD(*elements);
    return NOERROR;
}

ECode Vector::Contains(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean& result)
{
    Integer idx;
    IndexOf(obj, 0, &idx);
    result = idx >= 0;
    return NOERROR;
}

ECode Vector::IndexOf(
    /* [in] */ IInterface* obj,
    /* [out] */ Integer* index)
{
    return IndexOf(obj, 0, index);
}

ECode Vector::IndexOf(
    /* [in] */ IInterface* obj,
    /* [in] */ Integer fromIndex,
    /* [out] */ Integer* index)
{
    VALIDATE_NOT_NULL(index);

    AutoLock lock(this);

    if (obj == nullptr) {
        for (Integer i = fromIndex; i < mElementCount; i++) {
            if (mElementData[i] == nullptr) {
                *index = i;
                return NOERROR;
            }
        }
    }
    else {
        for (Integer i = fromIndex; i < mElementCount; i++) {
            if (Object::Equals(obj, mElementData[i])) {
                *index = i;
                return NOERROR;
            }
        }
    }
    *index = -1;
    return NOERROR;
}

ECode Vector::LastIndexOf(
    /* [in] */ IInterface* obj,
    /* [out] */ Integer* index)
{
    return LastIndexOf(obj, mElementCount - 1, index);
}

ECode Vector::LastIndexOf(
    /* [in] */ IInterface* obj,
    /* [in] */ Integer fromIndex,
    /* [out] */ Integer* index)
{
    VALIDATE_NOT_NULL(index);

    AutoLock lock(this);

    if (fromIndex >= mElementCount) {
        Logger::E("Vector", "%d >= %d", fromIndex, mElementCount);
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    if (obj == nullptr) {
        for (Integer i = fromIndex; i >= 0; i--) {
            if (mElementData[i] == nullptr) {
                *index = i;
                return NOERROR;
            }
        }
    }
    else {
        for (Integer i = fromIndex; i >= 0; i--) {
            if (Object::Equals(obj, mElementData[i])) {
                *index = i;
                return NOERROR;
            }
        }
    }
    *index = -1;
    return NOERROR;
}

ECode Vector::GetElementAt(
    /* [in] */ Integer index,
    /* [out] */ IInterface** element)
{
    VALIDATE_NOT_NULL(element);

    AutoLock lock(this);

    if (index >= mElementCount) {
        Logger::E("Vector", "%s >= %s", index, mElementCount);
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    *element = ElementData(index);
    REFCOUNT_ADD(*element);
    return NOERROR;
}

ECode Vector::GetFirstElement(
    /* [out] */ IInterface** element)
{
    VALIDATE_NOT_NULL(element);

    AutoLock lock(this);

    if (mElementCount == 0) {
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }
    *element = ElementData(0);
    REFCOUNT_ADD(*element);
    return NOERROR;
}

ECode Vector::GetLastElement(
    /* [out] */ IInterface** element)
{
    VALIDATE_NOT_NULL(element);

    AutoLock lock(this);

    if (mElementCount == 0) {
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }
    *element = ElementData(mElementCount - 1);
    REFCOUNT_ADD(*element);
    return NOERROR;
}

ECode Vector::SetElementAt(
    /* [in] */ IInterface* obj,
    /* [in] */ Integer index)
{
    AutoLock lock(this);

    if (index >= mElementCount) {
        Logger::E("Vector", "%d >= %d", index, mElementCount);
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    mElementData.Set(index, obj);
    return NOERROR;
}

ECode Vector::RemoveElementAt(
    /* [in] */ Integer index)
{
    AutoLock lock(this);

    mModCount++;
    if (index >= mElementCount) {
        Logger::E("Vector", "%d >= %d", index, mElementCount);
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    else if (index < 0) {
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    Integer j = mElementCount - index - 1;
    if (j > 0) {
        mElementData.Copy(index, mElementData, index + 1, j);
    }
    mElementCount--;
    mElementData.Set(mElementCount, nullptr);
    return NOERROR;
}

ECode Vector::InsertElementAt(
    /* [in] */ IInterface* obj,
    /* [in] */ Integer index)
{
    AutoLock lock(this);

    mModCount++;
    if (index > mElementCount) {
        Logger::E("Vector", "%d >= %d", index, mElementCount);
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    EnsureCapacityHelper(mElementCount + 1);
    mElementData.Copy(index + 1, mElementData, index, mElementCount - index);
    mElementData.Set(index, obj);
    mElementCount++;
    return NOERROR;
}

ECode Vector::AddElement(
    /* [in] */ IInterface* e)
{
    AutoLock lock(this);

    mModCount++;
    EnsureCapacityHelper(mElementCount + 1);
    mElementData.Set(mElementCount++, e);
    return NOERROR;
}

ECode Vector::RemoveElement(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* changed)
{
    AutoLock lock(this);

    mModCount++;
    Integer i;
    IndexOf(obj, &i);
    if (i >= 0) {
        RemoveElementAt(i);
        if (changed != nullptr) {
            *changed = true;
        }
        return NOERROR;
    }
    if (changed != nullptr) {
        *changed = false;
    }
    return NOERROR;
}

ECode Vector::RemoveAllElements()
{
    AutoLock lock(this);

    mModCount++;
    for (Integer i = 0; i < mElementCount; i++) {
        mElementData.Set(i, nullptr);
    }

    mElementCount = 0;
    return NOERROR;
}

ECode Vector::CloneImpl(
    /* [in] */ IVector* newObj)
{
    AutoLock lock(this);

    Vector* v = (Vector*)newObj;
    Arrays::CopyOf(mElementData, mElementCount, &v->mElementData);
    v->mModCount = 0;
    return NOERROR;
}

ECode Vector::ToArray(
    /* [out, callee] */ Array<IInterface*>* objs)
{
    AutoLock lock(this);

    return Arrays::CopyOf(mElementData, mElementCount, objs);
}

ECode Vector::ToArray(
    /* [in] */ const InterfaceID& iid,
    /* [out, callee] */ Array<IInterface*>* objs)
{
    AutoLock lock(this);

    Array<IInterface*> array(mElementCount);
    for (Integer i = 0; i < mElementCount; i++) {
        array.Set(i, mElementData[i]->Probe(iid));
    }
    *objs = std::move(array);
    return NOERROR;
}

AutoPtr<IInterface> Vector::ElementData(
    /* [in] */ Integer index)
{
    return mElementData[index];
}

ECode Vector::Get(
    /* [in] */ Integer index,
    /* [out] */ AutoPtr<IInterface>& obj)
{
    AutoLock lock(this);

    if (index >= mElementCount) {
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    obj = mElementData[index];
    return NOERROR;
}

ECode Vector::Set(
    /* [in] */ Integer index,
    /* [in] */ IInterface* obj,
    /* [out] */ IInterface** prevObj)
{
    AutoLock lock(this);

    if (index > mElementCount) {
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    if (prevObj != nullptr) {
        ElementData(index).MoveTo(prevObj);
    }
    mElementData.Set(index, obj);
    return NOERROR;
}

ECode Vector::Add(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* changed)
{
    AutoLock lock(this);

    mModCount++;
    EnsureCapacityHelper(mElementCount + 1);
    mElementData.Set(mElementCount++, obj);
    if (changed != nullptr) {
        *changed = true;
    }
    return NOERROR;
}

ECode Vector::Remove(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* changed)
{
    return RemoveElement(obj, changed);
}

ECode Vector::Add(
    /* [in] */ Integer index,
    /* [in] */ IInterface* obj)
{
    return InsertElementAt(obj, index);
}

ECode Vector::Remove(
    /* [in] */ Integer index,
    /* [out] */ IInterface** obj)
{
    AutoLock lock(this);

    mModCount++;
    if (index >= mElementCount) {
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    if (obj != nullptr) {
        *obj = ElementData(index);
        REFCOUNT_ADD(*obj);
    }

    Integer numMoved = mElementCount - index - 1;
    if (numMoved > 0) {
        mElementData.Copy(index, mElementData, index + 1, numMoved);
    }
    mElementData.Set(--mElementCount, nullptr);

    return NOERROR;
}

ECode Vector::Clear()
{
    return RemoveAllElements();
}

ECode Vector::ContainsAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean& result)
{
    AutoLock lock(this);
    return AbstractList::ContainsAll(c, result);
}

ECode Vector::AddAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* changed)
{
    AutoLock lock(this);

    Array<IInterface*> a;
    c->ToArray(&a);
    Integer numNew = a.GetLength();
    EnsureCapacityHelper(mElementCount + numNew);
    mElementData.Copy(mElementCount, a, 0, numNew);
    mElementCount += numNew;
    if (changed != nullptr) {
        *changed = numNew != 0;
    }
    return NOERROR;
}

ECode Vector::RemoveAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* changed)
{
    AutoLock lock(this);

    return AbstractList::RemoveAll(c, changed);
}

ECode Vector::RetainAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* changed)
{
    AutoLock lock(this);

    return AbstractList::RetainAll(c, changed);
}

ECode Vector::AddAll(
    /* [in] */ Integer index,
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* changed)
{
    AutoLock lock(this);

    mModCount++;
    if (index < 0 || index > mElementCount) {
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    Array<IInterface*> a;
    c->ToArray(&a);
    Integer numNew = a.GetLength();
    EnsureCapacityHelper(mElementCount + numNew);

    Integer numMoved = mElementCount - index;
    if (numMoved > 0) {
        mElementData.Copy(index + numNew, mElementData, index, numMoved);
    }
    mElementData.Copy(index, a, 0, numNew);
    mElementCount += numNew;
    if (changed != nullptr) {
        *changed = numNew != 0;
    }
    return NOERROR;
}

ECode Vector::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean& same)
{
    AutoLock lock(this);

    return AbstractList::Equals(obj, same);
}

ECode Vector::GetHashCode(
    /* [out] */ Integer& hash)
{
    AutoLock lock(this);

    return AbstractList::GetHashCode(hash);
}

ECode Vector::ToString(
    /* [out] */ String& desc)
{
    AutoLock lock(this);

    return AbstractList::ToString(desc);
}

ECode Vector::SubList(
    /* [in] */ Integer fromIndex,
    /* [in] */ Integer toIndex,
    /* [out] */ IList** subList)
{
    VALIDATE_NOT_NULL(subList);

    AutoLock lock(this);

    AutoPtr<IList> list;
    FAIL_RETURN(AbstractList::SubList(fromIndex, toIndex, list));
    Collections::CreateSynchronizedList(list, this).MoveTo(subList);
    return NOERROR;
}

ECode Vector::RemoveRange(
    /* [in] */ Integer fromIndex,
    /* [in] */ Integer toIndex)
{
    AutoLock lock(this);

    mModCount++;
    Integer numMoved = mElementCount - toIndex;
    mElementData.Copy(fromIndex, mElementData, toIndex, numMoved);

    Integer newElementCount = mElementCount - (toIndex - fromIndex);
    while (mElementCount != newElementCount) {
        mElementData.Set(--mElementCount, nullptr);
    }
    return NOERROR;
}

ECode Vector::GetListIterator(
    /* [in] */ Integer index,
    /* [out] */ AutoPtr<IListIterator>& it)
{
    AutoLock lock(this);

    if (index < 0|| index > mElementCount) {
        Logger::E("Vector", "Index: %d", index);
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    it = new ListItr(this, index);
    return NOERROR;
}

ECode Vector::GetListIterator(
    /* [out] */ AutoPtr<IListIterator>& it)
{
    AutoLock lock(this);
    it = new ListItr(this, 0);
    return NOERROR;
}

ECode Vector::GetIterator(
    /* [out] */ AutoPtr<IIterator>& it)
{
    AutoLock lock(this);
    it = new Itr(this);
    return NOERROR;
}

//-------------------------------------------------------------------------------

COMO_INTERFACE_IMPL_1(Vector::Itr, SyncObject, IIterator);

Vector::Itr::Itr(
    /* [in] */ Vector* owner)
    : mCursor(0)
    , mExpectedModCount(owner->mModCount)
    , mLimit(owner->mElementCount)
    , mOwner(owner)
{}

ECode Vector::Itr::HasNext(
    /* [out] */ Boolean& result)
{
    result = mCursor < mLimit;
    return NOERROR;
}

ECode Vector::Itr::Next(
    /* [out] */ AutoPtr<IInterface>& object)
{
    AutoLock lock(mOwner);

    FAIL_RETURN(CheckForComodification());
    Integer i = mCursor;
    if (i >= mLimit) {
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }
    mCursor = i + 1;
    mLastRet = i;
    object = mOwner->ElementData(mLastRet);
    return NOERROR;
}

ECode Vector::Itr::Remove()
{
    if (mLastRet == -1) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    {
        AutoLock lock(mOwner);

        FAIL_RETURN(CheckForComodification());
        mOwner->Remove(mLastRet);
        mExpectedModCount = mOwner->mModCount;
        mLimit--;
    }
    mCursor = mLastRet;
    mLastRet = -1;
    return NOERROR;
}

ECode Vector::Itr::CheckForComodification()
{
    if (mOwner->mModCount != mExpectedModCount) {
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }
    return NOERROR;
}


//-------------------------------------------------------------------------------

COMO_INTERFACE_IMPL_1(Vector::ListItr, Vector::Itr, IListIterator);

Vector::ListItr::ListItr(
    /* [in] */ Vector* owner,
    /* [in] */ Integer index)
    : Itr(owner)
{
    mCursor = index;
}

ECode Vector::ListItr::HasPrevious(
    /* [out] */ Boolean& result)
{
    result = mCursor != 0;
    return NOERROR;
}

ECode Vector::ListItr::GetNextIndex(
    /* [out] */ Integer& index)
{
    index = mCursor;
    return NOERROR;
}

ECode Vector::ListItr::GetPreviousIndex(
    /* [out] */ Integer& index)
{
    index = mCursor - 1;
    return NOERROR;
}

ECode Vector::ListItr::Previous(
    /* [out] */ AutoPtr<IInterface>& object)
{
    AutoLock lock(mOwner);

    FAIL_RETURN(CheckForComodification());
    Integer i = mCursor - 1;
    if (i < 0) {
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }
    mCursor = i;
    mLastRet = i;
    object = mOwner->ElementData(mLastRet);
    return NOERROR;
}

ECode Vector::ListItr::Set(
    /* [in] */ IInterface* object)
{
    if (mLastRet == -1) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    AutoLock lock(mOwner);
    FAIL_RETURN(CheckForComodification());
    mOwner->Set(mLastRet, object, nullptr);
    return NOERROR;
}

ECode Vector::ListItr::Add(
    /* [in] */ IInterface* object)
{
    Integer i = mCursor;
    {
        AutoLock lock(mOwner);

        FAIL_RETURN(CheckForComodification());
        mOwner->Add(i, object);
        mExpectedModCount = mOwner->mModCount;
        mLimit++;
    }
    mCursor = i + 1;
    mLastRet = -1;
    return NOERROR;
}

ECode Vector::ListItr::Next(
    /* [out] */ AutoPtr<IInterface>& object)
{
    return Itr::Next(object);
}

ECode Vector::ListItr::HasNext(
    /* [out] */ Boolean& result)
{
    return Itr::HasNext(result);
}

ECode Vector::ListItr::Remove()
{
    return Itr::Remove();
}

}
}
