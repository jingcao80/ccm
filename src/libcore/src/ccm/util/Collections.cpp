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

AutoPtr<IIterator> Collections::GetEmptyIterator()
{
    return EmptyIterator::Get_EMPTY_ITERATOR();
}

AutoPtr<IEnumeration> Collections::GetEmptyEnumeration()
{
    return EmptyEnumeration::Get_EMPTY_ENUMERATION();
}

AutoPtr<IList> Collections::GetEmptyList()
{
    return Get_EMPTY_LIST();
}

AutoPtr<IList> Collections::Get_EMPTY_LIST()
{
    static AutoPtr<IList> EMPTY_LIST = new EmptyList();
    return EMPTY_LIST;
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

CCM_INTERFACE_IMPL_LIGHT_1(Collections::EmptyIterator, IIterator);

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

CCM_INTERFACE_IMPL_LIGHT_1(Collections::EmptyEnumeration, IEnumeration);

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

CCM_INTERFACE_IMPL_1(Collections::EmptyListIterator, EmptyIterator, IListIterator);

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

}
}
