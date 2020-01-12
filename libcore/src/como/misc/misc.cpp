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

#include "como/misc/CFDBigInteger.h"
#include <comoapi.h>
#include <new>

namespace como {
namespace misc {

COMO_OBJECT_IMPL(CFDBigInteger);
ECode CFDBigInteger::New(
    /* [in] */ const Array<Integer>& data,
    /* [in] */ Integer offset,
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CFDBigInteger, nullptr, clsObject);
    if (FAILED(ec)) return ec;

    void* addr = calloc(sizeof(CFDBigInteger), 1);
    if (addr == nullptr) return E_OUT_OF_MEMORY_ERROR;

    CFDBigInteger* _obj = new(addr) CFDBigInteger();
    ec = _obj->Constructor(data, offset);
    if (FAILED(ec)) {
        free(addr);
        return ec;
    }
    AutoPtr<IMetaComponent> comp;
    clsObject->GetMetadate(comp);
    _obj->AttachMetadata(comp, String("como::misc::CFDBigInteger"));
    *object = _obj->Probe(iid);
    REFCOUNT_ADD(*object);
    return NOERROR;
}

}
}