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

#include "ccm/util/CArrayList.h"

using ccm::core::IID_ICloneable;
using ccm::core::IID_IIterable;
using ccm::io::IID_ISerializable;

namespace ccm {
namespace util {

CCM_INTERFACE_IMPL_7(CArrayList, SyncObject, IArrayList, IList, ICollection,
        IIterable, IRandomAccess, ICloneable, ISerializable);

CCM_OBJECT_IMPL(CArrayList);

ECode CArrayList::Constructor()
{
    return NOERROR;
}

ECode CArrayList::Add(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    return NOERROR;
}

ECode CArrayList::Add(
    /* [in] */ Integer index,
    /* [in] */ IInterface* obj)
{
    return NOERROR;
}

ECode CArrayList::AddAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* result)
{
    return NOERROR;
}

ECode CArrayList::AddAll(
    /* [in] */ Integer index,
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* result)
{
    return NOERROR;
}

ECode CArrayList::Clear()
{
    return NOERROR;
}

ECode CArrayList::Contains(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    return NOERROR;
}

ECode CArrayList::ContainsAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* result)
{
    return NOERROR;
}

ECode CArrayList::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    return NOERROR;
}

ECode CArrayList::Get(
    /* [in] */ Integer index,
    /* [out] */ IInterface** obj)
{
    return NOERROR;
}

ECode CArrayList::GetHashCode(
    /* [out] */ Integer* hash)
{
    return NOERROR;
}

ECode CArrayList::GetIterator(
    /* [out] */ IIterator** it)
{
    return NOERROR;
}

ECode CArrayList::GetListIterator(
    /* [out] */ IListIterator** it)
{
    return NOERROR;
}

ECode CArrayList::GetListIterator(
    /* [in] */ Integer index,
    /* [out] */ IListIterator** it)
{
    return NOERROR;
}

ECode CArrayList::GetSize(
    /* [out] */ Integer* size)
{
    return NOERROR;
}

ECode CArrayList::IndexOf(
    /* [in] */ IInterface* obj,
    /* [out] */ Integer* index)
{
    return NOERROR;
}

ECode CArrayList::IsEmpty(
    /* [out] */ Boolean* empty)
{
    return NOERROR;
}

ECode CArrayList::LastIndexOf(
    /* [in] */ IInterface* obj,
    /* [out] */ Integer* index)
{
    return NOERROR;
}

ECode CArrayList::Remove(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    return NOERROR;
}

ECode CArrayList::Remove(
    /* [in] */ Integer index,
    /* [out] */ IInterface** obj)
{
    return NOERROR;
}

ECode CArrayList::RemoveAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* result)
{
    return NOERROR;
}

ECode CArrayList::RetainAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* result)
{
    return NOERROR;
}

ECode CArrayList::Set(
    /* [in] */ Integer index,
    /* [in] */ IInterface* obj,
    /* [out] */ IInterface** prevObj)
{
    return NOERROR;
}

ECode CArrayList::SubList(
    /* [in] */ Integer fromIndex,
    /* [in] */ Integer toIndex,
    /* [out] */ IList** subList)
{
    return NOERROR;
}

ECode CArrayList::ToArray(
    /* [out, callee] */ Array<IInterface*>* objs)
{
    return NOERROR;
}

ECode CArrayList::Clone(
    /* [out] */ IInterface** obj)
{
    return NOERROR;
}

}
}
