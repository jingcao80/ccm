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

#include "core/CArray.h"

namespace ccm {
namespace core {

CCM_INTERFACE_IMPL_1(CArray, SyncObject, IArray);

CCM_OBJECT_IMPL(CArray);

ECode CArray::constructor(
    /* [in] */ const InterfaceID& elemId,
    /* [in] */ Long size)
{
    if (size < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mElements = Array<IInterface*>(size);
    mElementTypeId = elemId;
    return NOERROR;
}

ECode CArray::GetLength(
    /* [out] */ Long* size)
{
    VALIDATE_NOT_NULL(size);

    *size = mElements.GetLength();
    return NOERROR;
}

ECode CArray::Get(
    /* [in] */ Long index,
    /* [out] */ IInterface** element)
{
    VALIDATE_NOT_NULL(element);

    if (index < 0 || index > mElements.GetLength()) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    *element = mElements[index];
    REFCOUNT_ADD(*element);
    return NOERROR;
}

ECode CArray::Set(
    /* [in] */ Long index,
    /* [in] */ IInterface* element)
{
    if (index < 0 || index > mElements.GetLength()) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    if (element != nullptr &&
            element->Probe(mElementTypeId) == nullptr) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mElements.Set(index, element);
    return NOERROR;
}

ECode CArray::GetTypeId(
    /* [out] */ InterfaceID* id)
{
    VALIDATE_NOT_NULL(id);

    *id = mElementTypeId;
    return NOERROR;
}

}
}
