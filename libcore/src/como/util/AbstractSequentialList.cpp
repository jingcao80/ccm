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

#include "como/util/AbstractSequentialList.h"
#include "como.util.IListIterator.h"

using como::core::E_INDEX_OUT_OF_BOUNDS_EXCEPTION;

namespace como {
namespace util {

ECode AbstractSequentialList::Constructor()
{
    return AbstractList::Constructor();
}

ECode AbstractSequentialList::Get(
    /* [in] */ Integer index,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<IListIterator> it;
    ECode ec = GetListIterator(index, &it);
    if (FAILED(ec)) {
        Logger::E("AbstractSequentialList", "Index: %d", index);
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    return it->Next(obj);
}

ECode AbstractSequentialList::Set(
    /* [in] */ Integer index,
    /* [in] */ IInterface* obj,
    /* [out] */ IInterface** prevObj)
{
    AutoPtr<IListIterator> it;
    ECode ec = GetListIterator(index, &it);
    if (FAILED(ec)) {
        Logger::E("AbstractSequentialList", "Index: %d", index);
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    if (prevObj != nullptr) {
        it->Next(prevObj);
    }
    it->Set(obj);
    return NOERROR;
}

ECode AbstractSequentialList::Add(
    /* [in] */ Integer index,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IListIterator> it;
    ECode ec = GetListIterator(index, &it);
    if (FAILED(ec)) {
        Logger::E("AbstractSequentialList", "Index: %d", index);
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    return it->Add(obj);
}

ECode AbstractSequentialList::Remove(
    /* [in] */ Integer index,
    /* [out] */ IInterface** obj)
{
    AutoPtr<IListIterator> it;
    ECode ec = GetListIterator(index, &it);
    if (FAILED(ec)) {
        Logger::E("AbstractSequentialList", "Index: %d", index);
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    if (obj != nullptr) {
        it->Next(obj);
    }
    it->Remove();
    return NOERROR;
}

ECode AbstractSequentialList::AddAll(
    /* [in] */ Integer index,
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* result)
{
    Boolean modified = false;
    AutoPtr<IListIterator> it1;
    ECode ec = GetListIterator(index, &it1);
    if (FAILED(ec)) {
        Logger::E("AbstractSequentialList", "Index: %d", index);
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    AutoPtr<IIterator> it2;
    c->GetIterator(it2);
    Boolean hasNext;
    while (it2->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it2->Next(&obj);
        it1->Add(obj);
        modified = true;
    }
    if (result != nullptr) {
        *result = modified;
    }
    return NOERROR;
}

ECode AbstractSequentialList::GetIterator(
    /* [out] */ AutoPtr<IIterator>& it)
{
    AutoPtr<IListIterator> lit;
    GetListIterator(&lit);
    it = std::move(lit);
    return NOERROR;
}

}
}
