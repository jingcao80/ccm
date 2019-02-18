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

#include "ccm/util/AbstractList.h"
#include "ccm.util.IRandomAccess.h"
#include <ccmautoptr.h>
#include <ccmlogger.h>

namespace ccm {
namespace util {

class Sublist
    : public AbstractList
{
public:
    ECode Constructor(
        /* [in] */ AbstractList* list,
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
    ECode RangeCheck(
        /* [in] */ Integer index);

    ECode RangeCheckForAdd(
        /* [in] */ Integer index);

    String OutOfBoundsMsg(
        /* [in] */ Integer index);

    ECode CheckForComodification();

private:
    AutoPtr<AbstractList> mL;
    Integer mOffset;
    Integer mSize;
};

//-------------------------------------------------------------------------

class RandomAccessSubList
    : public Sublist
    , public IRandomAccess
{
public:
    CCM_INTERFACE_DECL();

    ECode Constructor(
        /* [in] */ AbstractList* list,
        /* [in] */ Integer fromIndex,
        /* [in] */ Integer toIndex);

    ECode SubList(
        /* [in] */ Integer fromIndex,
        /* [in] */ Integer toIndex,
        /* [out] */ IList** subList) override;
};

//-------------------------------------------------------------------------

CCM_INTERFACE_IMPL_1(AbstractList, AbstractCollection, IList);

ECode AbstractList::Constructor()
{
    return NOERROR;
}

ECode AbstractList::Add(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    Integer size;
    GetSize(&size);
    FAIL_RETURN(Add(size, obj));
    if (result != nullptr) *result = true;
    return NOERROR;
}

ECode AbstractList::Set(
    /* [in] */ Integer index,
    /* [in] */ IInterface* obj,
    /* [out] */ IInterface** prevObj)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode AbstractList::Add(
    /* [in] */ Integer index,
    /* [in] */ IInterface* obj)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode AbstractList::Remove(
    /* [in] */ Integer index,
    /* [out] */ IInterface** obj)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode AbstractList::IndexOf(
    /* [in] */ IInterface* obj,
    /* [out] */ Integer* index)
{
    VALIDATE_NOT_NULL(index);

    AutoPtr<IListIterator> it;
    GetListIterator(&it);
    if (obj == nullptr) {
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> next;
            if (it->Next(&next), next == nullptr) {
                return it->GetPreviousIndex(index);
            }
        }
    }
    else {
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> next;
            if (it->Next(&next), Object::Equals(obj, next)) {
                return it->GetPreviousIndex(index);
            }
        }
    }
    *index = -1;
    return NOERROR;
}

ECode AbstractList::LastIndexOf(
    /* [in] */ IInterface* obj,
    /* [out] */ Integer* index)
{
    VALIDATE_NOT_NULL(index);

    Integer size;
    GetSize(&size);
    AutoPtr<IListIterator> it;
    GetListIterator(size, &it);
    if (obj == nullptr) {
        Boolean hasPrev;
        while (it->HasPrevious(&hasPrev), hasPrev) {
            AutoPtr<IInterface> prev;
            if (it->Previous(&prev), prev == nullptr) {
                return it->GetNextIndex(index);
            }
        }
    }
    else {
        Boolean hasPrev;
        while (it->HasPrevious(&hasPrev), hasPrev) {
            AutoPtr<IInterface> prev;
            if (it->Previous(&prev), Object::Equals(obj, prev)) {
                return it->GetNextIndex(index);
            }
        }
    }
    *index = -1;
    return NOERROR;
}

ECode AbstractList::Clear()
{
    Integer size;
    GetSize(&size);
    RemoveRange(0, size);
    return NOERROR;
}

ECode AbstractList::AddAll(
    /* [in] */ Integer index,
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* result)
{
    FAIL_RETURN(RangeCheckForAdd(index));
    Boolean modified = false;
    AutoPtr<IIterator> it;
    c->GetIterator(&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> e;
        it->Next(&e);
        Add(index++, e);
        modified = true;
    }
    if (result != nullptr) *result = modified;
    return NOERROR;
}

ECode AbstractList::GetIterator(
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it);

    *it = new Itr(this);
    REFCOUNT_ADD(*it);
    return NOERROR;
}

ECode AbstractList::GetListIterator(
    /* [out] */ IListIterator** it)
{
    return GetListIterator(0, it);
}

ECode AbstractList::GetListIterator(
    /* [in] */ Integer index,
    /* [out] */ IListIterator** it)
{
    VALIDATE_NOT_NULL(it);

    FAIL_RETURN(RangeCheckForAdd(index));
    *it = new ListItr(this, index);
    REFCOUNT_ADD(*it);
    return NOERROR;
}

ECode AbstractList::SubList(
    /* [in] */ Integer fromIndex,
    /* [in] */ Integer toIndex,
    /* [out] */ IList** subList)
{
    VALIDATE_NOT_NULL(subList);

    if (Probe(IID_IRandomAccess) != nullptr) {
        AutoPtr<RandomAccessSubList> list = new RandomAccessSubList();
        FAIL_RETURN(list->Constructor(this, fromIndex, toIndex));
        *subList = list.Get();
        REFCOUNT_ADD(*subList);
        return NOERROR;
    }
    else {
        AutoPtr<Sublist> list = new Sublist();
        FAIL_RETURN(list->Constructor(this, fromIndex, toIndex));
        *subList = list.Get();
        REFCOUNT_ADD(*subList);
        return NOERROR;
    }
}

ECode AbstractList::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (IInterface::Equals(obj, (IList*)this)) {
        *result = true;
        return NOERROR;
    }
    if (IList::Probe(obj) == nullptr) {
        *result = false;
        return NOERROR;
    }

    AutoPtr<IListIterator> e1, e2;
    GetListIterator(&e1);
    IList::Probe(obj)->GetListIterator(&e2);
    Boolean hasNext1, hasNext2;
    while (e1->HasNext(&hasNext1), e2->HasNext(&hasNext2), hasNext1 && hasNext2) {
        AutoPtr<IInterface> o1, o2;
        e1->Next(&o1);
        e2->Next(&o2);
        if (!(o1 == nullptr ? o2 == nullptr : Object::Equals(o1, o2))) {
            *result = false;
            return NOERROR;
        }
    }
    *result = (e1->HasNext(&hasNext1), e2->HasNext(&hasNext2), !(hasNext1 || hasNext2));
    return NOERROR;
}

ECode AbstractList::GetHashCode(
    /* [out] */ Integer* hash)
{
    VALIDATE_NOT_NULL(hash);

    Integer h = 1;
    AutoPtr<IIterator> it;
    GetIterator(&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> e;
        it->Next(&e);
        h = 31 * h + (e == nullptr ? 0 : Object::GetHashCode(e));
    }
    *hash = h;
    return NOERROR;
}

ECode AbstractList::RemoveRange(
    /* [in] */ Integer fromIndex,
    /* [in] */ Integer toIndex)
{
    AutoPtr<IListIterator> it;
    GetListIterator(fromIndex, &it);
    for (Integer i = 0, n = toIndex - fromIndex; i < n; i++) {
        it->Next();
        it->Remove();
    }
    return NOERROR;
}

ECode AbstractList::RangeCheckForAdd(
    /* [in] */ Integer index)
{
    Integer size;
    if (index < 0 || (GetSize(&size), index > size)) {
        Logger::E("AbstractList", "%s", OutOfBoundsMsg(index).string());
        return ccm::core::E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    return NOERROR;
}

String AbstractList::OutOfBoundsMsg(
    /* [in] */ Integer index)
{
    Integer size;
    GetSize(&size);
    return String::Format("Index: %d, Size: %d", index, size);
}

ECode AbstractList::Contains(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    return AbstractCollection::Contains(obj, result);
}

ECode AbstractList::ContainsAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* result)
{
    return AbstractCollection::ContainsAll(c, result);
}

ECode AbstractList::IsEmpty(
    /* [out] */ Boolean* empty)
{
    return AbstractCollection::IsEmpty(empty);
}

ECode AbstractList::Remove(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* changed)
{
    return AbstractCollection::Remove(obj, changed);
}

ECode AbstractList::RemoveAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* changed)
{
    return AbstractCollection::RemoveAll(c, changed);
}

ECode AbstractList::RetainAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* changed)
{
    return AbstractCollection::RetainAll(c, changed);
}

ECode AbstractList::ToArray(
    /* [out, callee] */ Array<IInterface*>* objs)
{
    return AbstractCollection::ToArray(objs);
}

ECode AbstractList::ToArray(
    /* [in] */ const InterfaceID& iid,
    /* [out, callee] */ Array<IInterface*>* objs)
{
    return AbstractCollection::ToArray(iid, objs);
}

//-------------------------------------------------------------------------

CCM_INTERFACE_IMPL_LIGHT_1(AbstractList::Itr, IIterator);

ECode AbstractList::Itr::HasNext(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Integer size;
    mOwner->GetSize(&size);
    *result = mCursor != size;
    return NOERROR;
}

ECode AbstractList::Itr::Next(
    /* [out] */ IInterface** object)
{
    FAIL_RETURN(CheckForComodification());
    Integer i = mCursor;
    AutoPtr<IInterface> next;
    ECode ec = mOwner->Get(i, &next);
    if (FAILED(ec)) {
        FAIL_RETURN(CheckForComodification());
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }
    mLastRet = i;
    mCursor = i + 1;
    if (object != nullptr) {
        next.MoveTo(object);
    }
    return NOERROR;
}

ECode AbstractList::Itr::Remove()
{
    if (mLastRet < 0) {
        return ccm::core::E_ILLEGAL_STATE_EXCEPTION;
    }
    FAIL_RETURN(CheckForComodification());

    ECode ec = mOwner->Remove(mLastRet);
    if (FAILED(ec)) {
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }
    if (mLastRet < mCursor) {
        mCursor--;
    }
    mLastRet = -1;
    mExpectedModCount = mOwner->mModCount;
    return NOERROR;
}

ECode AbstractList::Itr::CheckForComodification()
{
    if (mOwner->mModCount != mExpectedModCount) {
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }
    return NOERROR;
}

//-------------------------------------------------------------------------

CCM_INTERFACE_IMPL_1(AbstractList::ListItr, Itr, IListIterator);

ECode AbstractList::ListItr::HasPrevious(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mCursor != 0;
    return NOERROR;
}

ECode AbstractList::ListItr::Previous(
    /* [out] */ IInterface** object)
{
    FAIL_RETURN(CheckForComodification());
    Integer i = mCursor - 1;
    AutoPtr<IInterface> prev;
    ECode ec = mOwner->Get(i, &prev);
    if (FAILED(ec)) {
        FAIL_RETURN(CheckForComodification());
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }
    mLastRet = mCursor = i;
    if (object != nullptr) {
        prev.MoveTo(object);
    }
    return NOERROR;
}

ECode AbstractList::ListItr::GetNextIndex(
    /* [out] */ Integer* index)
{
    VALIDATE_NOT_NULL(index);

    *index = mCursor;
    return NOERROR;
}

ECode AbstractList::ListItr::GetPreviousIndex(
    /* [out] */ Integer* index)
{
    VALIDATE_NOT_NULL(index);

    *index = mCursor - 1;
    return NOERROR;
}

ECode AbstractList::ListItr::Set(
    /* [in] */ IInterface* object)
{
    if (mLastRet < 0) {
        return ccm::core::E_ILLEGAL_STATE_EXCEPTION;
    }
    FAIL_RETURN(CheckForComodification());

    ECode ec = mOwner->Set(mLastRet, object);
    if (FAILED(ec)) {
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }
    mExpectedModCount = mOwner->mModCount;
    return NOERROR;
}

ECode AbstractList::ListItr::Add(
    /* [in] */ IInterface* object)
{
    FAIL_RETURN(CheckForComodification());

    Integer i = mCursor;
    ECode ec = mOwner->Add(i, object);
    if (FAILED(ec)) {
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }
    mLastRet = -1;
    mCursor = i + 1;
    mExpectedModCount = mOwner->mModCount;
    return NOERROR;
}

ECode AbstractList::ListItr::HasNext(
    /* [out] */ Boolean* result)
{
    return Itr::HasNext(result);
}

ECode AbstractList::ListItr::Next(
    /* [out] */ IInterface** object)
{
    return Itr::Next(object);
}

ECode AbstractList::ListItr::Remove()
{
    return Itr::Remove();
}

//-------------------------------------------------------------------------

ECode Sublist::Constructor(
    /* [in] */ AbstractList* list,
    /* [in] */ Integer fromIndex,
    /* [in] */ Integer toIndex)
{
    Integer size;
    if (fromIndex < 0 || (list->GetSize(&size), toIndex > size) ||
            (fromIndex > toIndex)) {
        Logger::E("SubList", "fromIndex: %d, toIndex: %d", fromIndex, toIndex);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mL = list;
    mOffset = fromIndex;
    mSize = toIndex - fromIndex;
    mModCount = mL->mModCount;
    return NOERROR;
}

ECode Sublist::Set(
    /* [in] */ Integer index,
    /* [in] */ IInterface* obj,
    /* [out] */ IInterface** prevObj)
{
    FAIL_RETURN(RangeCheck(index));
    FAIL_RETURN(CheckForComodification());
    return mL->Set(index + mOffset, obj, prevObj);
}

ECode Sublist::Get(
    /* [in] */ Integer index,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    FAIL_RETURN(RangeCheck(index));
    FAIL_RETURN(CheckForComodification());
    return mL->Get(index + mOffset, obj);
}

ECode Sublist::GetSize(
    /* [out] */ Integer* size)
{
    VALIDATE_NOT_NULL(size);

    FAIL_RETURN(CheckForComodification());
    *size = mSize;
    return NOERROR;
}

ECode Sublist::Add(
    /* [in] */ Integer index,
    /* [in] */ IInterface* obj)
{
    VALIDATE_NOT_NULL(obj);

    FAIL_RETURN(RangeCheckForAdd(index));
    FAIL_RETURN(CheckForComodification());
    mL->Add(index + mOffset, obj);
    mModCount = mL->mModCount;
    mSize++;
    return NOERROR;
}

ECode Sublist::Remove(
    /* [in] */ Integer index,
    /* [out] */ IInterface** obj)
{
    FAIL_RETURN(RangeCheck(index));
    FAIL_RETURN(CheckForComodification());
    FAIL_RETURN(mL->Remove(index + mOffset, obj));
    mModCount = mL->mModCount;
    mSize--;
    return NOERROR;
}

ECode Sublist::RemoveRange(
    /* [in] */ Integer fromIndex,
    /* [in] */ Integer toIndex)
{
    FAIL_RETURN(CheckForComodification());
    mL->RemoveRange(fromIndex + mOffset, toIndex + mOffset);
    mModCount = mL->mModCount;
    mSize -= (toIndex-fromIndex);
    return NOERROR;
}

ECode Sublist::AddAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* result)
{
    return AddAll(mSize, c);
}

ECode Sublist::AddAll(
    /* [in] */ Integer index,
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* result)
{
    FAIL_RETURN(RangeCheckForAdd(index));
    Integer cSize;
    c->GetSize(&cSize);
    if (cSize == 0) {
        if (result != nullptr) *result = false;
        return NOERROR;
    }

    FAIL_RETURN(CheckForComodification());
    mL->AddAll(index + mOffset, c, result);
    mModCount = mL->mModCount;
    mSize += cSize;
    if (result != nullptr) *result = true;
    return NOERROR;
}

ECode Sublist::GetIterator(
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it);

    AutoPtr<IListIterator> lit;
    GetListIterator(&lit);
    *it = IIterator::Probe(lit);
    REFCOUNT_ADD(*it);
    return NOERROR;
}

ECode Sublist::GetListIterator(
    /* [in] */ Integer index,
    /* [out] */ IListIterator** it)
{
    VALIDATE_NOT_NULL(it);

    FAIL_RETURN(CheckForComodification());
    FAIL_RETURN(RangeCheckForAdd(index));

    class _ListIterator
        : public LightRefBase
        , public IListIterator
        , public IIterator
    {
    public:
        _ListIterator(
            /* [in] */ Sublist* owner,
            /* [in] */ Integer index)
            : mOwner(owner)
        {
            mOwner->mL->GetListIterator(index + mOwner->mOffset, &mIt);
        }

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
            /* [out] */ InterfaceID* iid) override
        {
            VALIDATE_NOT_NULL(iid);

            if (object == (IInterface*)(IListIterator*)this) {
                *iid = IID_IListIterator;
                return NOERROR;
            }
            else if (object == (IInterface*)(IIterator*)this) {
                *iid = IID_IIterator;
                return NOERROR;
            }
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        ECode HasNext(
            /* [out] */ Boolean* result) override
        {
            VALIDATE_NOT_NULL(result);

            Integer nidx;
            GetNextIndex(&nidx);
            *result = nidx < mOwner->mSize;
            return NOERROR;
        }

        ECode Next(
            /* [out] */ IInterface** object = nullptr) override
        {
            Boolean hasNext;
            if (HasNext(&hasNext), hasNext) {
                return mIt->Next(object);
            }
            return E_NO_SUCH_ELEMENT_EXCEPTION;
        }

        ECode HasPrevious(
            /* [out] */ Boolean* result) override
        {
            VALIDATE_NOT_NULL(result);

            Integer pidx;
            GetPreviousIndex(&pidx);
            *result = pidx >= 0;
            return NOERROR;
        }

        ECode Previous(
            /* [out] */ IInterface** object = nullptr) override
        {
            Boolean hasPrev;
            if (HasPrevious(&hasPrev), hasPrev) {
                return mIt->Previous(object);
            }
            return E_NO_SUCH_ELEMENT_EXCEPTION;
        }

        ECode GetNextIndex(
            /* [out] */ Integer* index) override
        {
            VALIDATE_NOT_NULL(index);

            Integer idx;
            mIt->GetNextIndex(&idx);
            *index = idx - mOwner->mOffset;
            return NOERROR;
        }

        ECode GetPreviousIndex(
            /* [out] */ Integer* index) override
        {
            VALIDATE_NOT_NULL(index);

            Integer idx;
            mIt->GetPreviousIndex(&idx);
            *index = idx - mOwner->mOffset;
            return NOERROR;
        }

        ECode Remove() override
        {
            mIt->Remove();
            mOwner->mModCount = mOwner->mL->mModCount;
            mOwner->mSize--;
            return NOERROR;
        }

        ECode Set(
            /* [in] */ IInterface* object) override
        {
            return mIt->Set(object);
        }

        ECode Add(
            /* [in] */ IInterface* object) override
        {
            mIt->Add(object);
            mOwner->mModCount = mOwner->mL->mModCount;
            mOwner->mSize++;
            return NOERROR;
        }

    private:
        Sublist* mOwner;
        AutoPtr<IListIterator> mIt;
    };

    *it = new _ListIterator(this, index);
    REFCOUNT_ADD(*it);
    return NOERROR;
}

ECode Sublist::SubList(
    /* [in] */ Integer fromIndex,
    /* [in] */ Integer toIndex,
    /* [out] */ IList** subList)
{
    VALIDATE_NOT_NULL(subList);

    AutoPtr<Sublist> list = new Sublist();
    FAIL_RETURN(list->Constructor(this, fromIndex, toIndex));
    *subList = list.Get();
    REFCOUNT_ADD(*subList);
    return NOERROR;
}

ECode Sublist::RangeCheck(
    /* [in] */ Integer index)
{
    if (index < 0 || index >= mSize) {
        Logger::E("SubList", "%s", OutOfBoundsMsg(index).string());
        return ccm::core::E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    return NOERROR;
}

ECode Sublist::RangeCheckForAdd(
    /* [in] */ Integer index)
{
    if (index < 0 || index > mSize) {
        Logger::E("SubList", "%s", OutOfBoundsMsg(index).string());
        return ccm::core::E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    return NOERROR;
}

String Sublist::OutOfBoundsMsg(
    /* [in] */ Integer index)
{
    return String::Format("Index: %d, Size: %d", index, mSize);
}

ECode Sublist::CheckForComodification()
{
    if (mModCount != mL->mModCount) {
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }
    return NOERROR;
}

//-------------------------------------------------------------------------

CCM_INTERFACE_IMPL_1(RandomAccessSubList, Sublist, IRandomAccess);

ECode RandomAccessSubList::Constructor(
    /* [in] */ AbstractList* list,
    /* [in] */ Integer fromIndex,
    /* [in] */ Integer toIndex)
{
    return Sublist::Constructor(list, fromIndex, toIndex);
}

ECode RandomAccessSubList::SubList(
    /* [in] */ Integer fromIndex,
    /* [in] */ Integer toIndex,
    /* [out] */ IList** subList)
{
    VALIDATE_NOT_NULL(subList);

    AutoPtr<RandomAccessSubList> list = new RandomAccessSubList();
    FAIL_RETURN(list->Constructor(this, fromIndex, toIndex));
    *subList = list.Get();
    REFCOUNT_ADD(*subList);
    return NOERROR;
}

}
}
