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

#include "como/util/AbstractSet.h"
#include "como.util.ICollection.h"
#include "como.util.IIterator.h"
#include <comosp.h>

namespace como {
namespace util {

COMO_INTERFACE_IMPL_1(AbstractSet, AbstractCollection, ISet);

ECode AbstractSet::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean& result)
{
    if (IInterface::Equals(obj, (ISet*)this)) {
        result = true;
        return NOERROR;
    }

    if (ISet::Probe(obj) == nullptr) {
        result = false;
        return NOERROR;
    }
    ICollection* c = ICollection::Probe(obj);
    Integer othSize, thisSize;
    if (c->GetSize(&othSize), GetSize(&thisSize), othSize != thisSize) {
        result = false;
        return NOERROR;
    }
    return ContainsAll(c, &result);
}

ECode AbstractSet::GetHashCode(
    /* [out] */ Integer& hash)
{
    hash = 0;
    AutoPtr<IIterator> it;
    GetIterator(it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->Next(&obj);
        if (obj != nullptr) {
            hash += Object::GetHashCode(obj);
        }
    }
    return NOERROR;
}

ECode AbstractSet::RemoveAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* changed)
{
    Boolean modified = false;

    Integer othSize, thisSize;
    if (GetSize(&thisSize), c->GetSize(&othSize), thisSize > othSize) {
        AutoPtr<IIterator> it;
        c->GetIterator(it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->Next(&obj);
            Boolean changed;
            Remove(obj, &changed);
            modified |= changed;
        }
    }
    else {
        AutoPtr<IIterator> it;
        GetIterator(it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->Next(&obj);
            Boolean contains;
            if (c->Contains(obj, &contains), contains) {
                it->Remove();
                modified = true;
            }
        }
    }
    if (changed != nullptr) *changed = modified;
    return NOERROR;
}

ECode AbstractSet::Add(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* changed)
{
    return AbstractCollection::Add(obj, changed);
}

ECode AbstractSet::AddAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* changed)
{
    return AbstractCollection::Add(c, changed);
}

ECode AbstractSet::Clear()
{
    return AbstractCollection::Clear();
}

ECode AbstractSet::ContainsAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* result)
{
    return AbstractCollection::ContainsAll(c, result);
}

ECode AbstractSet::IsEmpty(
    /* [out] */ Boolean* empty)
{
    return AbstractCollection::IsEmpty(empty);
}

ECode AbstractSet::Remove(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* contained)
{
    return AbstractCollection::Remove(obj, contained);
}

ECode AbstractSet::RetainAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* changed)
{
    return AbstractCollection::RetainAll(c, changed);
}

ECode AbstractSet::ToArray(
    /* [out, callee] */ Array<IInterface*>* objs)
{
    return AbstractCollection::ToArray(objs);
}

ECode AbstractSet::ToArray(
    /* [in] */ const InterfaceID& iid,
    /* [out, callee] */ Array<IInterface*>* objs)
{
    return AbstractCollection::ToArray(iid, objs);
}

}
}
