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

#include "como/math/CBigDecimal.h"
#include "como/math/CBigInteger.h"
#include "como/math/CMathContext.h"
#include <comoapi.h>
#include <new>

namespace como {
namespace math {

COMO_OBJECT_IMPL(CBigDecimal);
ECode CBigDecimal::New(
    /* [in] */ Long smallValue,
    /* [in] */ Integer scale,
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** object)
{
    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CBigDecimal, nullptr, clsObject);
    if (FAILED(ec)) return ec;

    void* addr = calloc(sizeof(CBigDecimal), 1);
    if (addr == nullptr) return E_OUT_OF_MEMORY_ERROR;

    CBigDecimal* _obj = new(addr) CBigDecimal();
    ec = _obj->Constructor(smallValue, scale);
    if (FAILED(ec)) {
        free(addr);
        return ec;
    }
    AutoPtr<IMetaComponent> comp;
    clsObject->GetMetadate(comp);
    _obj->AttachMetadata(comp, String("como::math::CBigDecimal"));
    *object = _obj->Probe(iid);
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode CBigDecimal::New(
    /* [in] */ Integer smallValue,
    /* [in] */ Integer scale,
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** object)
{
    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CBigDecimal, nullptr, clsObject);
    if (FAILED(ec)) return ec;

    void* addr = calloc(sizeof(CBigDecimal), 1);
    if (addr == nullptr) return E_OUT_OF_MEMORY_ERROR;

    CBigDecimal* _obj = new(addr) CBigDecimal();
    ec = _obj->Constructor(smallValue, scale);
    if (FAILED(ec)) {
        free(addr);
        return ec;
    }
    AutoPtr<IMetaComponent> comp;
    clsObject->GetMetadate(comp);
    _obj->AttachMetadata(comp, String("como::math::CBigDecimal"));
    *object = _obj->Probe(iid);
    REFCOUNT_ADD(*object);
    return NOERROR;
}

COMO_OBJECT_IMPL(CBigInteger);
ECode CBigInteger::New(
    /* [in] */ BigInt* bigInt,
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** object)
{
    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CBigInteger, nullptr, clsObject);
    if (FAILED(ec)) return ec;

    void* addr = calloc(sizeof(CBigInteger), 1);
    if (addr == nullptr) return E_OUT_OF_MEMORY_ERROR;

    CBigInteger* _obj = new(addr) CBigInteger();
    ec = _obj->Constructor(bigInt);
    if (FAILED(ec)) {
        free(addr);
        return ec;
    }
    AutoPtr<IMetaComponent> comp;
    clsObject->GetMetadate(comp);
    _obj->AttachMetadata(comp, String("como::math::CBigInteger"));
    *object = _obj->Probe(iid);
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode CBigInteger::New(
    /* [in] */ Integer sign,
    /* [in] */ Long value,
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** object)
{
    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CBigInteger, nullptr, clsObject);
    if (FAILED(ec)) return ec;

    void* addr = calloc(sizeof(CBigInteger), 1);
    if (addr == nullptr) return E_OUT_OF_MEMORY_ERROR;

    CBigInteger* _obj = new(addr) CBigInteger();
    ec = _obj->Constructor(sign, value);
    if (FAILED(ec)) {
        free(addr);
        return ec;
    }
    AutoPtr<IMetaComponent> comp;
    clsObject->GetMetadate(comp);
    _obj->AttachMetadata(comp, String("como::math::CBigInteger"));
    *object = _obj->Probe(iid);
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode CBigInteger::New(
    /* [in] */ Integer sign,
    /* [in] */ Integer numberLength,
    /* [in] */ const Array<Integer>& digits,
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** object)
{
    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CBigInteger, nullptr, clsObject);
    if (FAILED(ec)) return ec;

    void* addr = calloc(sizeof(CBigInteger), 1);
    if (addr == nullptr) return E_OUT_OF_MEMORY_ERROR;

    CBigInteger* _obj = new(addr) CBigInteger();
    ec = _obj->Constructor(sign, numberLength, digits);
    if (FAILED(ec)) {
        free(addr);
        return ec;
    }
    AutoPtr<IMetaComponent> comp;
    clsObject->GetMetadate(comp);
    _obj->AttachMetadata(comp, String("como::math::CBigInteger"));
    *object = _obj->Probe(iid);
    REFCOUNT_ADD(*object);
    return NOERROR;
}

COMO_OBJECT_IMPL(CMathContext);

}
}
