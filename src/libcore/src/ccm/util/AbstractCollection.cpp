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

#include "ccm/core/CStringBuilder.h"
#include "ccm/util/AbstractCollection.h"
#include "ccm/util/Arrays.h"
#include "ccm.core.IStringBuilder.h"
#include "ccm.util.IIterator.h"
#include <ccmautoptr.h>
#include <ccmlogger.h>

using ccm::core::CStringBuilder;
using ccm::core::IStringBuilder;
using ccm::core::IID_IIterable;
using ccm::core::IID_IStringBuilder;

namespace ccm {
namespace util {

CCM_INTERFACE_IMPL_2(AbstractCollection, SyncObject, ICollection, IIterable);

ECode AbstractCollection::IsEmpty(
    /* [out] */ Boolean* empty)
{
    VALIDATE_NOT_NULL(empty);

    Integer size;
    GetSize(&size);
    *empty = size == 0;
    return NOERROR;
}

ECode AbstractCollection::Contains(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IIterator> it;
    GetIterator(&it);
    if (obj == nullptr) {
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> e;
            it->Next(&e);
            if (e == nullptr) {
                *result = true;
                return NOERROR;
            }
        }
    }
    else {
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> e;
            it->Next(&e);
            if (Object::Equals(obj, e)) {
                *result = true;
                return NOERROR;
            }
        }
    }
    *result = false;
    return NOERROR;
}

ECode AbstractCollection::ToArray(
    /* [out, callee] */ Array<IInterface*>* objs)
{
    VALIDATE_NOT_NULL(objs);

    // Estimate size of array; be prepared to see more or fewer elements
    Integer size;
    GetSize(&size);
    Array<IInterface*> r = Array<IInterface*>(size);
    AutoPtr<IIterator> it;
    GetIterator(&it);
    for (Integer i = 0; i < r.GetLength(); i++) {
        Boolean hasNext;
        if (it->HasNext(&hasNext), !hasNext) {
            return Arrays::CopyOf(r, i, objs);
        }
        AutoPtr<IInterface> e;
        it->Next(&e);
        r.Set(i, e);
    }
    Boolean hasNext;
    it->HasNext(&hasNext);
    if (hasNext) {
        return FinishToArray(r, it, objs);
    }
    else {
        *objs = r;
        return NOERROR;
    }
}

ECode AbstractCollection::ToArray(
    /* [in] */ const InterfaceID& iid,
    /* [out, callee] */ Array<IInterface*>* objs)
{
    VALIDATE_NOT_NULL(objs);

    // Estimate size of array; be prepared to see more or fewer elements
    Integer size;
    GetSize(&size);
    Array<IInterface*> r = Array<IInterface*>(size);
    AutoPtr<IIterator> it;
    GetIterator(&it);
    for (Integer i = 0; i < r.GetLength(); i++) {
        Boolean hasNext;
        if (it->HasNext(&hasNext), !hasNext) {
            return Arrays::CopyOf(r, i, objs);
        }
        AutoPtr<IInterface> e;
        it->Next(&e);
        r.Set(i, e != nullptr ? e->Probe(iid) : nullptr);
    }
    Boolean hasNext;
    it->HasNext(&hasNext);
    if (hasNext) {
        return FinishToArray(r, it, iid, objs);
    }
    else {
        *objs = r;
        return NOERROR;
    }
}

ECode AbstractCollection::FinishToArray(
    /* [in] */ Array<IInterface*>& r,
    /* [in] */ IIterator* it,
    /* [out, callee] */ Array<IInterface*>* objs)
{
    Integer i = r.GetLength();
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        Integer cap = r.GetLength();
        if (i == cap) {
            Integer newCap = cap + (cap >> 1) + 1;
            // overflow-conscious code
            if (newCap - MAX_ARRAY_SIZE > 0) {
                FAIL_RETURN(HugeCapacity(cap + 1, &newCap));
            }
            Array<IInterface*> newArray;
            Arrays::CopyOf(r, newCap, &newArray);
            r = newArray;
        }
        AutoPtr<IInterface> obj;
        it->Next(&obj);
        r.Set(i++, obj);
    }
    // trim if overallocated
    if (i == r.GetLength()) {
        *objs = r;
        return NOERROR;
    }
    else {
        return Arrays::CopyOf(r, i, objs);
    }
}

ECode AbstractCollection::FinishToArray(
    /* [in] */ Array<IInterface*>& r,
    /* [in] */ IIterator* it,
    /* [in] */ const InterfaceID& iid,
    /* [out, callee] */ Array<IInterface*>* objs)
{
    Integer i = r.GetLength();
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        Integer cap = r.GetLength();
        if (i == cap) {
            Integer newCap = cap + (cap >> 1) + 1;
            // overflow-conscious code
            if (newCap - MAX_ARRAY_SIZE > 0) {
                FAIL_RETURN(HugeCapacity(cap + 1, &newCap));
            }
            Array<IInterface*> newArray;
            Arrays::CopyOf(r, newCap, &newArray);
            r = newArray;
        }
        AutoPtr<IInterface> obj;
        it->Next(&obj);
        r.Set(i++, obj->Probe(iid));
    }
    // trim if overallocated
    if (i == r.GetLength()) {
        *objs = r;
        return NOERROR;
    }
    else {
        return Arrays::CopyOf(r, i, objs);
    }
}

ECode AbstractCollection::HugeCapacity(
    /* [in] */ Integer minCapacity,
    /* [out] */ Integer* newCapacity)
{
    if (minCapacity < 0) {
        Logger::E("AbstractCollection", "Required array size too large");
        return E_OUT_OF_MEMORY_ERROR;
    }
    *newCapacity = minCapacity > MAX_ARRAY_SIZE ? IInteger::MAX_VALUE : MAX_ARRAY_SIZE;
    return NOERROR;
}

ECode AbstractCollection::Add(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* changed)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode AbstractCollection::Remove(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* changed)
{
    AutoPtr<IIterator> it;
    GetIterator(&it);
    if (obj == nullptr) {
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> e;
            it->Next(&e);
            if (e == nullptr) {
                it->Remove();
                if (changed != nullptr) *changed = true;
                return NOERROR;
            }
        }
    }
    else {
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> e;
            it->Next(&e);
            if (Object::Equals(obj, e)) {
                it->Remove();
                if (changed != nullptr) *changed = true;
                return NOERROR;
            }
        }
    }
    if (changed != nullptr) *changed = false;
    return NOERROR;
}

ECode AbstractCollection::ContainsAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IIterator> it;
    c->GetIterator(&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> e;
        it->Next(&e);
        Boolean contains;
        if (Contains(e, &contains), !contains) {
            *result = false;
            return NOERROR;
        }
    }
    *result = true;
    return NOERROR;
}

ECode AbstractCollection::AddAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* changed)
{
    Boolean modified = false;
    AutoPtr<IIterator> it;
    c->GetIterator(&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> e;
        it->Next(&e);
        Boolean result;
        if (Add(e, &result), result) {
            modified = true;
        }
    }
    if (changed != nullptr) *changed = modified;
    return NOERROR;
}

ECode AbstractCollection::RemoveAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* changed)
{
    Boolean modified = false;
    AutoPtr<IIterator> it;
    GetIterator(&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> e;
        it->Next(&e);
        Boolean result;
        if (c->Contains(e, &result), result) {
            it->Remove();
            modified = true;
        }
    }
    if (changed != nullptr) *changed = modified;
    return NOERROR;
}

ECode AbstractCollection::RetainAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* changed)
{
    Boolean modified = false;
    AutoPtr<IIterator> it;
    GetIterator(&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> e;
        it->Next(&e);
        Boolean result;
        if (c->Contains(e, &result), !result) {
            it->Remove();
            modified = true;
        }
    }
    if (changed != nullptr) *changed = modified;
    return NOERROR;
}

ECode AbstractCollection::Clear()
{
    AutoPtr<IIterator> it;
    GetIterator(&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        it->Next();
        it->Remove();
    }
    return NOERROR;
}

ECode AbstractCollection::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    AutoPtr<IIterator> it;
    GetIterator(&it);
    Boolean hasNext;
    if (it->HasNext(&hasNext), !hasNext) {
        *str = "[]";
        return NOERROR;
    }

    AutoPtr<IStringBuilder> sb;
    CStringBuilder::New(IID_IStringBuilder, (IInterface**)&sb);
    sb->Append(U'[');
    for (;;) {
        AutoPtr<IInterface> e;
        it->Next(&e);
        sb->Append(IInterface::Equals(e, (ICollection*)this) ?
                String("(this Collection)") : Object::ToString(e));
        if (it->HasNext(&hasNext), !hasNext) {
            sb->Append(U']');
            sb->ToString(str);
            return NOERROR;
        }
        sb->Append(U',');
        sb->Append(U' ');
    }
}

ECode AbstractCollection::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    return SyncObject::Equals(obj, result);
}

ECode AbstractCollection::GetHashCode(
    /* [out] */ Integer* hash)
{
    return SyncObject::GetHashCode(hash);
}

}
}
