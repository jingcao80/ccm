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

using ccm::core::AutoLock;
using ccm::io::IID_ISerializable;

namespace ccm {
namespace util {

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

ECode Collections::EmptyIterator::GetNext(
    /* [out] */ IInterface** object)
{
    return E_NO_SUCH_ELEMENT_EXCEPTION;
}

ECode Collections::EmptyIterator::Remove()
{
    return ccm::core::E_ILLEGAL_STATE_EXCEPTION;
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

ECode Collections::EmptyEnumeration::GetNextElement(
    /* [out] */ IInterface** object)
{
    return E_NO_SUCH_ELEMENT_EXCEPTION;
}

AutoPtr<IEnumeration> Collections::EmptyEnumeration::Get_EMPTY_ENUMERATION()
{
    static AutoPtr<IEnumeration> EMPTY_ENUMERATION = new EmptyEnumeration();
    return EMPTY_ENUMERATION;
}

}
}
