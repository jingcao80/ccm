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

#include "ccm/core/AutoLock.h"
#include "ccm/util/Collections.h"
#include "ccm.util.IListIterator.h"
#include "ccm.util.IRandomAccess.h"

using ccm::core::AutoLock;
using ccm::core::E_ILLEGAL_STATE_EXCEPTION;
using ccm::core::E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
using ccm::io::IID_ISerializable;

namespace ccm {
namespace util {

static const InterfaceID IID_ReverseComparator2 =
        {{0xe78c4caa,0x878f,0x42e1,0x8394,{0x2,0xa,0x9,0x1,0x4,0xd,0x6,0xd,0x5,0x3,0xb,0x5}}, &CID_libcore};

void Collections::Reverse(
    /* [in] */ IList* list)
{
    Integer size;
    list->GetSize(&size);
    if (size < REVERSE_THRESHOLD || IRandomAccess::Probe(list) != nullptr) {
        for (Integer i = 0, mid = size >> 1, j = size - 1; i < mid; i++, j--) {
            Swap(list, i, j);
        }
    }
    else {
        AutoPtr<IListIterator> fwd, rev;
        list->GetListIterator(&fwd);
        list->GetListIterator(size, &rev);
        for (Integer i = 0, mid = size >> 1; i < mid; i++) {
            AutoPtr<IInterface> e1, e2;
            rev->Previous(&e1);
            fwd->Next(&e2);
            fwd->Set(e1);
            rev->Set(e2);
        }
    }
}

void Collections::Swap(
    /* [in] */ IList* list,
    /* [in] */ Integer i,
    /* [in] */ Integer j)
{
    AutoPtr<IInterface> e1, e2;
    list->Get(i, &e1);
    list->Set(j, e1, &e2);
    list->Set(i, e2);
}

AutoPtr<ISet> Collections::CreateUnmodifiableSet(
    /* [in] */ ISet* set)
{
    return new UnmodifiableSet(set);
}

AutoPtr<IList> Collections::CreateUnmodifiableList(
    /* [in] */ IList* list)
{
    return (IRandomAccess::Probe(list) != nullptr) ?
            new UnmodifiableRandomAccessList(list) :
            new UnmodifiableList(list);
}

AutoPtr<ICollection> Collections::CreateSynchronizedCollection(
    /* [in] */ ICollection* c,
    /* [in] */ ISynchronize* mutex)
{
    return new SynchronizedCollection(c, mutex);
}

AutoPtr<ISet> Collections::CreateSynchronizedSet(
    /* [in] */ ISet* s,
    /* [in] */ ISynchronize* mutex)
{
    return new SynchronizedSet(s, mutex);
}

AutoPtr<IList> Collections::CreateSynchronizedList(
    /* [in] */ IList* l,
    /* [in] */ ISynchronize* mutex)
{
    if (IRandomAccess::Probe(l) != nullptr) {
        return new SynchronizedRandomAccessList(l, mutex);
    }
    else {
        return new SynchronizedList(l, mutex);
    }
}

AutoPtr<IIterator> Collections::GetEmptyIterator()
{
    return EmptyIterator::Get_EMPTY_ITERATOR();
}

AutoPtr<IEnumeration> Collections::GetEmptyEnumeration()
{
    return EmptyEnumeration::Get_EMPTY_ENUMERATION();
}

AutoPtr<ISet> Collections::GetEMPTY_SET()
{
    static AutoPtr<ISet> EMPTY_SET = new EmptySet();
    return EMPTY_SET;
}

AutoPtr<IList> Collections::GetEmptyList()
{
    return GetEMPTY_LIST();
}

AutoPtr<IList> Collections::GetEMPTY_LIST()
{
    static AutoPtr<IList> EMPTY_LIST = new EmptyList();
    return EMPTY_LIST;
}

AutoPtr<IMap> Collections::GetEMPTY_MAP()
{
    static AutoPtr<IMap> EMPTY_MAP = new EmptyMap();
    return EMPTY_MAP;
}

AutoPtr<IComparator> Collections::ReverseOrder()
{
    return (IComparator*)ReverseComparator::GetREVERSE_ORDER().Get();
}

AutoPtr<IComparator> Collections::ReverseOrder(
    /* [in] */ IComparator* cmp)
{
    if (cmp == nullptr) {
        return ReverseOrder();
    }

    if (cmp->Probe(IID_ReverseComparator2) != nullptr) {
        return ((ReverseComparator2*)cmp)->mCmp;
    }

    return new ReverseComparator2(cmp);
}

//----------------------------------------------------------------

CCM_INTERFACE_IMPL_2(Collections::UnmodifiableCollection, Object, ICollection, ISerializable);

ECode Collections::UnmodifiableCollection::GetSize(
    /* [out] */ Integer* size)
{
    return mC->GetSize(size);
}

ECode Collections::UnmodifiableCollection::IsEmpty(
    /* [out] */ Boolean* empty)
{
    return mC->IsEmpty(empty);
}

ECode Collections::UnmodifiableCollection::Contains(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    return mC->Contains(obj, result);
}

ECode Collections::UnmodifiableCollection::ToArray(
    /* [out, callee] */ Array<IInterface*>* objs)
{
    return mC->ToArray(objs);
}

ECode Collections::UnmodifiableCollection::ToArray(
    /* [in] */ const InterfaceID& iid,
    /* [out, callee] */ Array<IInterface*>* objs)
{
    return mC->ToArray(iid, objs);
}

ECode Collections::UnmodifiableCollection::ToString(
    /* [out] */ String* desc)
{
    return IObject::Probe(mC)->ToString(desc);
}

ECode Collections::UnmodifiableCollection::GetIterator(
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it);

    class _Iterator
        : public LightRefBase
        , public IIterator
    {
    public:
        _Iterator(
            /* [in] */ IIterator* it)
            : mI(it)
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

            if (object == (IInterface*)(IIterator*)this) {
                *iid = IID_IIterator;
                return NOERROR;
            }
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        ECode HasNext(
            /* [out] */ Boolean* result) override
        {
            return mI->HasNext(result);
        }

        ECode Next(
            /* [out] */ IInterface** object = nullptr) override
        {
            return mI->Next(object);
        }

        ECode Remove() override
        {
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
        }

    private:
        AutoPtr<IIterator> mI;
    };

    AutoPtr<IIterator> i;
    mC->GetIterator(&i);
    *it = new _Iterator(i);
    REFCOUNT_ADD(*it);
    return NOERROR;
}

ECode Collections::UnmodifiableCollection::Add(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* changed)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Collections::UnmodifiableCollection::Remove(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* changed)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Collections::UnmodifiableCollection::ContainsAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* result)
{
    return mC->ContainsAll(c, result);
}

ECode Collections::UnmodifiableCollection::AddAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* changed)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Collections::UnmodifiableCollection::RemoveAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* changed)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Collections::UnmodifiableCollection::RetainAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* changed)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Collections::UnmodifiableCollection::Clear()
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

//----------------------------------------------------------------

CCM_INTERFACE_IMPL_1(Collections::UnmodifiableSet, UnmodifiableCollection, ISet);

ECode Collections::UnmodifiableSet::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if ((ISet*)this == ISet::Probe(obj)) {
        *result = true;
        return NOERROR;
    }
    return IObject::Probe(mC)->Equals(obj, result);
}

ECode Collections::UnmodifiableSet::GetHashCode(
    /* [out] */ Integer* hash)
{
    return IObject::Probe(mC)->GetHashCode(hash);
}

ECode Collections::UnmodifiableSet::Add(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* modified)
{
    return UnmodifiableCollection::Add(obj, modified);
}

ECode Collections::UnmodifiableSet::AddAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* changed)
{
    return UnmodifiableCollection::AddAll(c, changed);
}

ECode Collections::UnmodifiableSet::Clear()
{
    return UnmodifiableCollection::Clear();
}

ECode Collections::UnmodifiableSet::Contains(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    return UnmodifiableCollection::Contains(obj, result);
}

ECode Collections::UnmodifiableSet::ContainsAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* result)
{
    return UnmodifiableCollection::ContainsAll(c, result);
}

ECode Collections::UnmodifiableSet::GetIterator(
    /* [out] */ IIterator** it)
{
    return UnmodifiableCollection::GetIterator(it);
}

ECode Collections::UnmodifiableSet::GetSize(
    /* [out] */ Integer* size)
{
    return UnmodifiableCollection::GetSize(size);
}

ECode Collections::UnmodifiableSet::IsEmpty(
    /* [out] */ Boolean* result)
{
    return UnmodifiableCollection::IsEmpty(result);
}

ECode Collections::UnmodifiableSet::Remove(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* contained)
{
    return UnmodifiableCollection::Remove(obj, contained);
}

ECode Collections::UnmodifiableSet::RemoveAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* changed)
{
    return UnmodifiableCollection::RemoveAll(c, changed);
}

ECode Collections::UnmodifiableSet::RetainAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* changed)
{
    return UnmodifiableCollection::RetainAll(c, changed);
}

ECode Collections::UnmodifiableSet::ToArray(
    /* [out, callee] */ Array<IInterface*>* objs)
{
    return UnmodifiableCollection::ToArray(objs);
}

ECode Collections::UnmodifiableSet::ToArray(
    /* [in] */ const InterfaceID& iid,
    /* [out, callee] */ Array<IInterface*>* objs)
{
    return UnmodifiableCollection::ToArray(iid, objs);
}

//----------------------------------------------------------------

CCM_INTERFACE_IMPL_1(Collections::UnmodifiableList, UnmodifiableCollection, IList);

ECode Collections::UnmodifiableList::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if ((IList*)this == IList::Probe(obj)) {
        *result = true;
        return NOERROR;
    }
    return IObject::Probe(mList)->Equals(obj, result);
}

ECode Collections::UnmodifiableList::GetHashCode(
    /* [out] */ Integer* hash)
{
    return IObject::Probe(mList)->GetHashCode(hash);
}

ECode Collections::UnmodifiableList::Get(
    /* [in] */ Integer index,
    /* [out] */ IInterface** obj)
{
    return mList->Get(index, obj);
}

ECode Collections::UnmodifiableList::Set(
    /* [in] */ Integer index,
    /* [in] */ IInterface* obj,
    /* [out] */ IInterface** prevObj)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Collections::UnmodifiableList::Add(
    /* [in] */ Integer index,
    /* [in] */ IInterface* obj)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Collections::UnmodifiableList::Remove(
    /* [in] */ Integer index,
    /* [out] */ IInterface** obj)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Collections::UnmodifiableList::IndexOf(
    /* [in] */ IInterface* obj,
    /* [out] */ Integer* index)
{
    return mList->IndexOf(obj, index);
}

ECode Collections::UnmodifiableList::LastIndexOf(
    /* [in] */ IInterface* obj,
    /* [out] */ Integer* index)
{
    return mList->LastIndexOf(obj, index);
}

ECode Collections::UnmodifiableList::AddAll(
    /* [in] */ Integer index,
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* result)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Collections::UnmodifiableList::GetListIterator(
    /* [out] */ IListIterator** it)
{
    return GetListIterator(0, it);
}

ECode Collections::UnmodifiableList::GetListIterator(
    /* [in] */ Integer index,
    /* [out] */ IListIterator** it)
{
    VALIDATE_NOT_NULL(it);

    class _ListIterator
        : public LightRefBase
        , public IListIterator
        , public IIterator
    {
    public:
        _ListIterator(
            /* [in] */ IListIterator* it)
            : mI(it)
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
            return mI->HasNext(result);
        }

        ECode Next(
            /* [out] */ IInterface** object = nullptr) override
        {
            return mI->Next(object);
        }

        ECode HasPrevious(
            /* [out] */ Boolean* result) override
        {
            return mI->HasPrevious(result);
        }

        ECode Previous(
            /* [out] */ IInterface** object = nullptr) override
        {
            return mI->Previous(object);
        }

        ECode GetNextIndex(
            /* [out] */ Integer* index) override
        {
            return mI->GetNextIndex(index);
        }

        ECode GetPreviousIndex(
            /* [out] */ Integer* index) override
        {
            return mI->GetPreviousIndex(index);
        }

        ECode Remove() override
        {
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
        }

        ECode Set(
            /* [in] */ IInterface* object) override
        {
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
        }

        ECode Add(
            /* [in] */ IInterface* object) override
        {
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
        }

    private:
        AutoPtr<IListIterator> mI;
    };

    AutoPtr<IListIterator> i;
    mList->GetListIterator(index, &i);
    *it = new _ListIterator(i);
    REFCOUNT_ADD(*it);
    return NOERROR;
}

ECode Collections::UnmodifiableList::SubList(
    /* [in] */ Integer fromIndex,
    /* [in] */ Integer toIndex,
    /* [out] */ IList** subList)
{
    VALIDATE_NOT_NULL(subList);

    AutoPtr<IList> sub;
    mList->SubList(fromIndex, toIndex, &sub);
    *subList = new UnmodifiableList(sub);
    REFCOUNT_ADD(*subList);
    return NOERROR;
}

ECode Collections::UnmodifiableList::Add(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* changed)
{
    return UnmodifiableCollection::Add(obj, changed);
}

ECode Collections::UnmodifiableList::AddAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* changed)
{
    return UnmodifiableCollection::AddAll(c, changed);
}

ECode Collections::UnmodifiableList::Clear()
{
    return UnmodifiableCollection::Clear();
}

ECode Collections::UnmodifiableList::Contains(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    return UnmodifiableCollection::Contains(obj, result);
}

ECode Collections::UnmodifiableList::ContainsAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* result)
{
    return UnmodifiableCollection::ContainsAll(c, result);
}

ECode Collections::UnmodifiableList::GetIterator(
    /* [out] */ IIterator** it)
{
    return UnmodifiableCollection::GetIterator(it);
}

ECode Collections::UnmodifiableList::GetSize(
    /* [out] */ Integer* size)
{
    return UnmodifiableCollection::GetSize(size);
}

ECode Collections::UnmodifiableList::IsEmpty(
    /* [out] */ Boolean* empty)
{
    return UnmodifiableCollection::IsEmpty(empty);
}

ECode Collections::UnmodifiableList::Remove(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* changed)
{
    return UnmodifiableCollection::Remove(obj, changed);
}

ECode Collections::UnmodifiableList::RemoveAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* changed)
{
    return UnmodifiableCollection::RemoveAll(c, changed);
}

ECode Collections::UnmodifiableList::RetainAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* changed)
{
    return UnmodifiableCollection::RetainAll(c, changed);
}

ECode Collections::UnmodifiableList::ToArray(
    /* [out, callee] */ Array<IInterface*>* objs)
{
    return UnmodifiableCollection::ToArray(objs);
}

ECode Collections::UnmodifiableList::ToArray(
    /* [in] */ const InterfaceID& iid,
    /* [out, callee] */ Array<IInterface*>* objs)
{
    return UnmodifiableCollection::ToArray(iid, objs);
}

//----------------------------------------------------------------

CCM_INTERFACE_IMPL_1(Collections::UnmodifiableRandomAccessList, UnmodifiableList, IRandomAccess);

ECode Collections::UnmodifiableRandomAccessList::SubList(
    /* [in] */ Integer fromIndex,
    /* [in] */ Integer toIndex,
    /* [out] */ IList** subList)
{
    VALIDATE_NOT_NULL(subList);

    AutoPtr<IList> sub;
    mList->SubList(fromIndex, toIndex, &sub);
    *subList = new UnmodifiableRandomAccessList(sub);
    REFCOUNT_ADD(*subList);
    return NOERROR;
}

//----------------------------------------------------------------

CCM_INTERFACE_IMPL_2(Collections::SynchronizedCollection, Object, ICollection, ISerializable);

ECode Collections::SynchronizedCollection::GetSize(
    /* [out] */ Integer* size)
{
    VALIDATE_NOT_NULL(size);

    AutoLock lock(mMutex);
    return mC->GetSize(size);
}

ECode Collections::SynchronizedCollection::IsEmpty(
    /* [out] */ Boolean* empty)
{
    VALIDATE_NOT_NULL(empty);

    AutoLock lock(mMutex);
    return mC->IsEmpty(empty);
}

ECode Collections::SynchronizedCollection::Contains(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoLock lock(mMutex);
    return mC->Contains(obj, result);
}

ECode Collections::SynchronizedCollection::ToArray(
    /* [out, callee] */ Array<IInterface*>* objs)
{
    VALIDATE_NOT_NULL(objs);

    AutoLock lock(mMutex);
    return mC->ToArray(objs);
}

ECode Collections::SynchronizedCollection::ToArray(
    /* [in] */ const InterfaceID& iid,
    /* [out, callee] */ Array<IInterface*>* objs)
{
    VALIDATE_NOT_NULL(objs);

    AutoLock lock(mMutex);
    return mC->ToArray(iid, objs);
}

ECode Collections::SynchronizedCollection::GetIterator(
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it);

    return mC->GetIterator(it);
}

ECode Collections::SynchronizedCollection::Add(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* changed)
{
    VALIDATE_NOT_NULL(changed);

    AutoLock lock(mMutex);
    return mC->Add(obj, changed);
}

ECode Collections::SynchronizedCollection::Remove(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* changed)
{
    VALIDATE_NOT_NULL(changed);

    AutoLock lock(mMutex);
    return mC->Remove(obj, changed);
}

ECode Collections::SynchronizedCollection::ContainsAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoLock lock(mMutex);
    return mC->ContainsAll(c, result);
}

ECode Collections::SynchronizedCollection::AddAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* changed)
{
    VALIDATE_NOT_NULL(changed);

    AutoLock lock(mMutex);
    return mC->AddAll(c, changed);
}

ECode Collections::SynchronizedCollection::RemoveAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* changed)
{
    VALIDATE_NOT_NULL(changed);

    AutoLock lock(mMutex);
    return mC->RemoveAll(c, changed);
}

ECode Collections::SynchronizedCollection::RetainAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* changed)
{
    VALIDATE_NOT_NULL(changed);

    AutoLock lock(mMutex);
    return mC->RetainAll(c, changed);
}

ECode Collections::SynchronizedCollection::Clear()
{
    AutoLock lock(mMutex);
    return mC->Clear();
}

ECode Collections::SynchronizedCollection::ToString(
    /* [out] */ String* desc)
{
    AutoLock lock(mMutex);
    *desc = Object::ToString(mC);
    return NOERROR;
}

ECode Collections::SynchronizedCollection::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    return SyncObject::Equals(obj, result);
}

ECode Collections::SynchronizedCollection::GetHashCode(
    /* [out] */ Integer* hash)
{
    return SyncObject::GetHashCode(hash);
}

//----------------------------------------------------------------

CCM_INTERFACE_IMPL_1(Collections::SynchronizedSet, SynchronizedCollection, ISet);

ECode Collections::SynchronizedSet::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (IInterface::Equals(obj, (ISet*)this)) {
        *result = true;
        return NOERROR;
    }
    AutoLock lock(mMutex);
    *result = Object::Equals(mC, obj);
    return NOERROR;
}

ECode Collections::SynchronizedSet::GetHashCode(
    /* [out] */ Integer* hash)
{
    VALIDATE_NOT_NULL(hash);

    AutoLock lock(mMutex);
    *hash = Object::GetHashCode(mC);
    return NOERROR;
}

ECode Collections::SynchronizedSet::Add(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* changed)
{
    return SynchronizedCollection::Add(obj, changed);
}

ECode Collections::SynchronizedSet::AddAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* changed)
{
    return SynchronizedCollection::AddAll(c, changed);
}

ECode Collections::SynchronizedSet::Clear()
{
    return SynchronizedCollection::Clear();
}

ECode Collections::SynchronizedSet::Contains(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    return SynchronizedCollection::Contains(obj, result);
}

ECode Collections::SynchronizedSet::ContainsAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* result)
{
    return SynchronizedCollection::ContainsAll(c, result);
}

ECode Collections::SynchronizedSet::GetIterator(
    /* [out] */ IIterator** it)
{
    return SynchronizedCollection::GetIterator(it);
}

ECode Collections::SynchronizedSet::GetSize(
    /* [out] */ Integer* size)
{
    return SynchronizedCollection::GetSize(size);
}

ECode Collections::SynchronizedSet::IsEmpty(
    /* [out] */ Boolean* empty)
{
    return SynchronizedCollection::IsEmpty(empty);
}

ECode Collections::SynchronizedSet::Remove(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* changed)
{
    return SynchronizedCollection::Remove(obj, changed);
}

ECode Collections::SynchronizedSet::RemoveAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* changed)
{
    return SynchronizedCollection::RemoveAll(c, changed);
}

ECode Collections::SynchronizedSet::RetainAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* changed)
{
    return SynchronizedCollection::RetainAll(c, changed);
}

ECode Collections::SynchronizedSet::ToArray(
    /* [out, callee] */ Array<IInterface*>* objs)
{
    return SynchronizedCollection::ToArray(objs);
}

ECode Collections::SynchronizedSet::ToArray(
    /* [in] */ const InterfaceID& iid,
    /* [out, callee] */ Array<IInterface*>* objs)
{
    return SynchronizedCollection::ToArray(iid, objs);
}

//----------------------------------------------------------------

CCM_INTERFACE_IMPL_1(Collections::SynchronizedList, Collections::SynchronizedCollection, IList);

ECode Collections::SynchronizedList::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if ((IList*)this == IList::Probe(obj)) {
        *result = true;
        return NOERROR;
    }
    AutoLock lock(mMutex);
    *result = Object::Equals(mList, obj);
    return NOERROR;
}

ECode Collections::SynchronizedList::GetHashCode(
    /* [out] */ Integer* hash)
{
    VALIDATE_NOT_NULL(hash);

    AutoLock lock(mMutex);
    *hash = Object::GetHashCode(mList);
    return NOERROR;
}

ECode Collections::SynchronizedList::Get(
    /* [in] */ Integer index,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoLock lock(mMutex);
    return mList->Get(index, obj);
}

ECode Collections::SynchronizedList::Set(
    /* [in] */ Integer index,
    /* [in] */ IInterface* obj,
    /* [out] */ IInterface** prevObj)
{
    AutoLock lock(mMutex);
    return mList->Set(index, obj, prevObj);
}

ECode Collections::SynchronizedList::Add(
    /* [in] */ Integer index,
    /* [in] */ IInterface* obj)
{
    AutoLock lock(mMutex);
    return mList->Add(index, obj);
}

ECode Collections::SynchronizedList::Remove(
    /* [in] */ Integer index,
    /* [out] */ IInterface** obj)
{
    AutoLock lock(mMutex);
    return mList->Remove(index, obj);
}

ECode Collections::SynchronizedList::IndexOf(
    /* [in] */ IInterface* obj,
    /* [out] */ Integer* index)
{
    VALIDATE_NOT_NULL(index);

    AutoLock lock(mMutex);
    return mList->IndexOf(obj, index);
}

ECode Collections::SynchronizedList::LastIndexOf(
    /* [in] */ IInterface* obj,
    /* [out] */ Integer* index)
{
    VALIDATE_NOT_NULL(index);

    AutoLock lock(mMutex);
    return mList->LastIndexOf(obj, index);
}

ECode Collections::SynchronizedList::AddAll(
    /* [in] */ Integer index,
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* result)
{
    AutoLock lock(mMutex);
    return mList->AddAll(index, c, result);
}

ECode Collections::SynchronizedList::GetListIterator(
    /* [out] */ IListIterator** it)
{
    // Must be manually synched by user
    return mList->GetListIterator(it);
}

ECode Collections::SynchronizedList::GetListIterator(
    /* [in] */ Integer index,
    /* [out] */ IListIterator** it)
{
    // Must be manually synched by user
    return mList->GetListIterator(index, it);
}

ECode Collections::SynchronizedList::SubList(
    /* [in] */ Integer fromIndex,
    /* [in] */ Integer toIndex,
    /* [out] */ IList** subList)
{
    VALIDATE_NOT_NULL(subList);

    AutoLock lock(this);

    AutoPtr<IList> list;
    FAIL_RETURN(mList->SubList(fromIndex, toIndex, &list));
    *subList = new SynchronizedList(list, mMutex);
    REFCOUNT_ADD(*subList);
    return NOERROR;
}

ECode Collections::SynchronizedList::Add(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* changed)
{
    return SynchronizedCollection::Add(obj, changed);
}

ECode Collections::SynchronizedList::AddAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* changed)
{
    return SynchronizedCollection::AddAll(c, changed);
}

ECode Collections::SynchronizedList::Clear()
{
    return SynchronizedCollection::Clear();
}

ECode Collections::SynchronizedList::Contains(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    return SynchronizedCollection::Contains(obj, result);
}

ECode Collections::SynchronizedList::ContainsAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* result)
{
    return SynchronizedCollection::ContainsAll(c, result);
}

ECode Collections::SynchronizedList::GetIterator(
    /* [out] */ IIterator** it)
{
    return SynchronizedCollection::GetIterator(it);
}

ECode Collections::SynchronizedList::GetSize(
    /* [out] */ Integer* size)
{
    return SynchronizedCollection::GetSize(size);
}

ECode Collections::SynchronizedList::IsEmpty(
    /* [out] */ Boolean* empty)
{
    return SynchronizedCollection::IsEmpty(empty);
}

ECode Collections::SynchronizedList::Remove(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* changed)
{
    return SynchronizedCollection::Remove(obj, changed);
}

ECode Collections::SynchronizedList::RemoveAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* changed)
{
    return SynchronizedCollection::RemoveAll(c, changed);
}

ECode Collections::SynchronizedList::RetainAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* changed)
{
    return SynchronizedCollection::RetainAll(c, changed);
}

ECode Collections::SynchronizedList::ToArray(
    /* [out, callee] */ Array<IInterface*>* objs)
{
    return SynchronizedCollection::ToArray(objs);
}

ECode Collections::SynchronizedList::ToArray(
    /* [in] */ const InterfaceID& iid,
    /* [out, callee] */ Array<IInterface*>* objs)
{
    return SynchronizedCollection::ToArray(iid, objs);
}

//----------------------------------------------------------------

CCM_INTERFACE_IMPL_1(Collections::SynchronizedRandomAccessList, Collections::SynchronizedList, IRandomAccess);

ECode Collections::SynchronizedRandomAccessList::SubList(
    /* [in] */ Integer fromIndex,
    /* [in] */ Integer toIndex,
    /* [out] */ IList** subList)
{
    VALIDATE_NOT_NULL(subList);

    AutoLock lock(this);

    AutoPtr<IList> list;
    FAIL_RETURN(mList->SubList(fromIndex, toIndex, &list));
    *subList = new SynchronizedRandomAccessList(list, mMutex);
    REFCOUNT_ADD(*subList);
    return NOERROR;
}

//----------------------------------------------------------------

CCM_INTERFACE_IMPL_LIGHT_1(Collections::EmptyIterator, LightRefBase, IIterator);

ECode Collections::EmptyIterator::HasNext(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = false;
    return NOERROR;
}

ECode Collections::EmptyIterator::Next(
    /* [out] */ IInterface** object)
{
    return E_NO_SUCH_ELEMENT_EXCEPTION;
}

ECode Collections::EmptyIterator::Remove()
{
    return E_ILLEGAL_STATE_EXCEPTION;
}

AutoPtr<IIterator> Collections::EmptyIterator::Get_EMPTY_ITERATOR()
{
    static AutoPtr<IIterator> EMPTY_ITERATOR = new EmptyIterator();
    return EMPTY_ITERATOR;
}

//-----------------------------------------------------------------

CCM_INTERFACE_IMPL_LIGHT_1(Collections::EmptyEnumeration, LightRefBase, IEnumeration);

ECode Collections::EmptyEnumeration::HasMoreElements(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = false;
    return NOERROR;
}

ECode Collections::EmptyEnumeration::NextElement(
    /* [out] */ IInterface** object)
{
    return E_NO_SUCH_ELEMENT_EXCEPTION;
}

AutoPtr<IEnumeration> Collections::EmptyEnumeration::Get_EMPTY_ENUMERATION()
{
    static AutoPtr<IEnumeration> EMPTY_ENUMERATION = new EmptyEnumeration();
    return EMPTY_ENUMERATION;
}

//----------------------------------------------------------------

CCM_INTERFACE_IMPL_LIGHT_1(Collections::EmptyListIterator, EmptyIterator, IListIterator);

ECode Collections::EmptyListIterator::HasPrevious(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = false;
    return NOERROR;
}

ECode Collections::EmptyListIterator::Previous(
    /* [out] */ IInterface** object)
{
    return E_NO_SUCH_ELEMENT_EXCEPTION;
}

ECode Collections::EmptyListIterator::GetNextIndex(
    /* [out] */ Integer* index)
{
    VALIDATE_NOT_NULL(index);

    *index = 0;
    return NOERROR;
}

ECode Collections::EmptyListIterator::GetPreviousIndex(
    /* [out] */ Integer* index)
{
    VALIDATE_NOT_NULL(index);

    *index = -1;
    return NOERROR;
}

ECode Collections::EmptyListIterator::Set(
    /* [in] */ IInterface* object)
{
    return E_ILLEGAL_STATE_EXCEPTION;
}

ECode Collections::EmptyListIterator::Add(
    /* [in] */ IInterface* object)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Collections::EmptyListIterator::Next(
    /* [out] */ IInterface** object)
{
    return EmptyIterator::Next(object);
}

ECode Collections::EmptyListIterator::HasNext(
    /* [out] */ Boolean* result)
{
    return EmptyIterator::HasNext(result);
}

ECode Collections::EmptyListIterator::Remove()
{
    return EmptyIterator::Remove();
}

AutoPtr<IListIterator> Collections::EmptyListIterator::Get_EMPTY_ITERATOR()
{
    static AutoPtr<IListIterator> EMPTY_ITERATOR = new EmptyListIterator();
    return EMPTY_ITERATOR;
}

//----------------------------------------------------------------

CCM_INTERFACE_IMPL_1(Collections::EmptySet, AbstractSet, ISerializable);

ECode Collections::EmptySet::GetIterator(
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it);

    AutoPtr<IIterator> iterator = GetEmptyIterator();
    iterator.MoveTo(it);
    return NOERROR;
}

ECode Collections::EmptySet::GetSize(
    /* [out] */ Integer* size)
{
    VALIDATE_NOT_NULL(size);

    *size = 0;
    return NOERROR;
}

ECode Collections::EmptySet::IsEmpty(
    /* [out] */ Boolean* empty)
{
    VALIDATE_NOT_NULL(empty);

    *empty = true;
    return NOERROR;
}

ECode Collections::EmptySet::Contains(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result= false;
    return NOERROR;
}

ECode Collections::EmptySet::ContainsAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    return c->IsEmpty(result);
}

ECode Collections::EmptySet::ToArray(
    /* [out, callee] */ Array<IInterface*>* objs)
{
    VALIDATE_NOT_NULL(objs);

    *objs = Array<IInterface*>(0);
    return NOERROR;
}

ECode Collections::EmptySet::ToArray(
    /* [in] */ const InterfaceID& iid,
    /* [out, callee] */ Array<IInterface*>* objs)
{
    VALIDATE_NOT_NULL(objs);

    *objs = Array<IInterface*>(0);
    return NOERROR;
}

//----------------------------------------------------------------

CCM_INTERFACE_IMPL_2(Collections::EmptyList, AbstractList, IRandomAccess, ISerializable);

ECode Collections::EmptyList::GetIterator(
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it);

    *it = new EmptyIterator();
    REFCOUNT_ADD(*it);
    return NOERROR;
}

ECode Collections::EmptyList::GetListIterator(
    /* [out] */ IListIterator** it)
{
    VALIDATE_NOT_NULL(it);

    *it = new EmptyListIterator();
    REFCOUNT_ADD(*it);
    return NOERROR;
}

ECode Collections::EmptyList::GetSize(
    /* [out] */ Integer* size)
{
    VALIDATE_NOT_NULL(size);

    *size = 0;
    return NOERROR;
}

ECode Collections::EmptyList::IsEmpty(
    /* [out] */ Boolean* empty)
{
    VALIDATE_NOT_NULL(empty);

    *empty = true;
    return NOERROR;
}

ECode Collections::EmptyList::Contains(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = false;
    return NOERROR;
}

ECode Collections::EmptyList::ContainsAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* result)
{
    return c->IsEmpty(result);
}

ECode Collections::EmptyList::ToArray(
    /* [out, callee] */ Array<IInterface*>* objs)
{
    VALIDATE_NOT_NULL(objs);

    *objs = Array<IInterface*>(0);
    return NOERROR;
}

ECode Collections::EmptyList::ToArray(
    /* [in] */ const InterfaceID& iid,
    /* [out, callee] */ Array<IInterface*>* objs)
{
    VALIDATE_NOT_NULL(objs);

    *objs = Array<IInterface*>(0);
    return NOERROR;
}

ECode Collections::EmptyList::Get(
    /* [in] */ Integer index,
    /* [out] */ IInterface** obj)
{
    return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
}

ECode Collections::EmptyList::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    IList* other = IList::Probe(obj);
    if (other == nullptr) {
        *result = false;
        return NOERROR;
    }
    return other->IsEmpty(result);
}

ECode Collections::EmptyList::GetHashCode(
    /* [out] */ Integer* hash)
{
    VALIDATE_NOT_NULL(hash);

    *hash = 1;
    return NOERROR;
}

ECode Collections::EmptyList::AddAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* result)
{
    return AbstractCollection::AddAll(c, result);
}

//----------------------------------------------------------------

CCM_INTERFACE_IMPL_1(Collections::EmptyMap, AbstractMap, ISerializable);

ECode Collections::EmptyMap::GetSize(
    /* [out] */ Integer* size)
{
    VALIDATE_NOT_NULL(size);

    *size = 0;
    return NOERROR;
}

ECode Collections::EmptyMap::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = true;
    return NOERROR;
}

ECode Collections::EmptyMap::ContainsKey(
    /* [in] */ IInterface* key,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = false;
    return NOERROR;
}

ECode Collections::EmptyMap::ContainsValue(
    /* [in] */ IInterface* value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = false;
    return NOERROR;
}

ECode Collections::EmptyMap::Get(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value);

    *value = nullptr;
    return NOERROR;
}

ECode Collections::EmptyMap::GetKeySet(
    /* [out] */ ISet** keys)
{
    VALIDATE_NOT_NULL(keys);

    AutoPtr<ISet> emptySet = GetEmptySet();
    emptySet.MoveTo(keys);
    return NOERROR;
}

ECode Collections::EmptyMap::GetValues(
    /* [out] */ ICollection** values)
{
    VALIDATE_NOT_NULL(values);

    AutoPtr<ISet> emptySet = GetEmptySet();
    emptySet.MoveTo(values);
    return NOERROR;
}

ECode Collections::EmptyMap::GetEntrySet(
    /* [out] */ ISet** entries)
{
    VALIDATE_NOT_NULL(entries);

    AutoPtr<ISet> emptySet = GetEmptySet();
    emptySet.MoveTo(entries);
    return NOERROR;
}

ECode Collections::EmptyMap::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    IMap* other = IMap::Probe(obj);
    if (other == nullptr) {
        *result = false;
        return NOERROR;
    }
    return other->IsEmpty(result);
}

ECode Collections::EmptyMap::GetHashCode(
    /* [out] */ Integer* hash)
{
    VALIDATE_NOT_NULL(hash);

    *hash = 0;
    return NOERROR;
}

ECode Collections::EmptyMap::PutIfAbsent(
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value,
    /* [out] */ IInterface** prevValue)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

//----------------------------------------------------------------

CCM_INTERFACE_IMPL_1(Collections::ReverseComparator, Object, IComparator);

ECode Collections::ReverseComparator::Compare(
    /* [in] */ IInterface* c1,
    /* [in] */ IInterface* c2,
    /* [out] */ Integer* cmp)
{
    return IComparable::Probe(c2)->CompareTo(c1, cmp);
}

ECode Collections::ReverseComparator::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* isEqual)
{
    VALIDATE_NOT_NULL(isEqual);

    *isEqual = (IComparator*)this == IComparator::Probe(obj);
    return NOERROR;
}

AutoPtr<Collections::ReverseComparator> Collections::ReverseComparator::GetREVERSE_ORDER()
{
    static const AutoPtr<ReverseComparator> REVERSE_ORDER = new ReverseComparator();
    return REVERSE_ORDER;
}

//----------------------------------------------------------------

CCM_INTERFACE_IMPL_1(Collections::ReverseComparator2, Object, IComparator);

ECode Collections::ReverseComparator2::Compare(
    /* [in] */ IInterface* c1,
    /* [in] */ IInterface* c2,
    /* [out] */ Integer* cmp)
{
    return mCmp->Compare(c2, c1, cmp);
}

ECode Collections::ReverseComparator2::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* isEqual)
{
    VALIDATE_NOT_NULL(isEqual);

    if (obj->Probe(IID_ReverseComparator2) == nullptr) {
        *isEqual = false;
        return NOERROR;
    }
    ReverseComparator2* other = (ReverseComparator2*)IComparator::Probe(obj);
    if (other != this) {
        *isEqual = false;
        return NOERROR;
    }
    *isEqual = Object::Equals(mCmp, other->mCmp);
    return NOERROR;
}

ECode Collections::ReverseComparator2::GetHashCode(
    /* [out] */ Integer* hash)
{
    VALIDATE_NOT_NULL(hash);

    *hash = Object::GetHashCode(mCmp) ^ IInteger::MIN_VALUE;
    return NOERROR;
}

}
}
