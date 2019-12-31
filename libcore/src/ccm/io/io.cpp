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

#include "ccm/io/CBufferedWriter.h"
#include "ccm/io/CByteArrayOutputStream.h"
#include "ccm/io/CDirectByteBuffer.h"
#include "ccm/io/CFile.h"
#include "ccm/io/CFileDescriptor.h"
#include "ccm/io/CFileInputStream.h"
#include "ccm/io/CFileOutputStream.h"
#include "ccm/io/COutputStreamWriter.h"
#include "ccm/io/CPrintWriter.h"
#include "ccm/io/CStringWriter.h"
#include <ccmapi.h>
#include <new>

namespace ccm {
namespace io {

CCM_OBJECT_IMPL(CBufferedWriter);
CCM_OBJECT_IMPL(CByteArrayOutputStream);

CCM_OBJECT_IMPL(CDirectByteBuffer);
ECode CDirectByteBuffer::New(
    /* [in] */ Integer capacity,
    /* [in] */ MemoryRef* memoryRef,
    /* [in] */ const InterfaceID& iid,
    /* [out] */ ccm::IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CDirectByteBuffer, nullptr, &clsObject);
    if (FAILED(ec)) return ec;

    void* addr = calloc(sizeof(CDirectByteBuffer), 1);
    if (addr == nullptr) return E_OUT_OF_MEMORY_ERROR;

    CDirectByteBuffer* _obj = new(addr) CDirectByteBuffer();
    ec = _obj->Constructor(capacity, memoryRef);
    if (FAILED(ec)) {
        free(addr);
        return ec;
    }
    AutoPtr<IMetaComponent> comp;
    clsObject->GetMetadate(&comp);
    _obj->AttachMetadata(comp, String("ccm::io::CDirectByteBuffer"));
    *object = _obj->Probe(iid);
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode CDirectByteBuffer::New(
    /* [in] */ HANDLE address,
    /* [in] */ Integer cap,
    /* [in] */ const InterfaceID& iid,
    /* [out] */ ccm::IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CDirectByteBuffer, nullptr, &clsObject);
    if (FAILED(ec)) return ec;

    void* addr = calloc(sizeof(CDirectByteBuffer), 1);
    if (addr == nullptr) return E_OUT_OF_MEMORY_ERROR;

    CDirectByteBuffer* _obj = new(addr) CDirectByteBuffer();
    ec = _obj->Constructor(address, cap);
    if (FAILED(ec)) {
        free(addr);
        return ec;
    }
    AutoPtr<IMetaComponent> comp;
    clsObject->GetMetadate(&comp);
    _obj->AttachMetadata(comp, String("ccm::io::CDirectByteBuffer"));
    *object = _obj->Probe(iid);
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode CDirectByteBuffer::New(
    /* [in] */ MemoryRef* memoryRef,
    /* [in] */ Integer mark,
    /* [in] */ Integer pos,
    /* [in] */ Integer lim,
    /* [in] */ Integer cap,
    /* [in] */ Integer off,
    /* [in] */ const InterfaceID& iid,
    /* [out] */ ccm::IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CDirectByteBuffer, nullptr, &clsObject);
    if (FAILED(ec)) return ec;

    void* addr = calloc(sizeof(CDirectByteBuffer), 1);
    if (addr == nullptr) return E_OUT_OF_MEMORY_ERROR;

    CDirectByteBuffer* _obj = new(addr) CDirectByteBuffer();
    ec = _obj->Constructor(memoryRef, mark, pos, lim, cap, off);
    if (FAILED(ec)) {
        free(addr);
        return ec;
    }
    AutoPtr<IMetaComponent> comp;
    clsObject->GetMetadate(&comp);
    _obj->AttachMetadata(comp, String("ccm::io::CDirectByteBuffer"));
    *object = _obj->Probe(iid);
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode CDirectByteBuffer::New(
    /* [in] */ MemoryRef* memoryRef,
    /* [in] */ Integer mark,
    /* [in] */ Integer pos,
    /* [in] */ Integer lim,
    /* [in] */ Integer cap,
    /* [in] */ Integer off,
    /* [in] */ Boolean isReadOnly,
    /* [in] */ const InterfaceID& iid,
    /* [out] */ ccm::IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CDirectByteBuffer, nullptr, &clsObject);
    if (FAILED(ec)) return ec;

    void* addr = calloc(sizeof(CDirectByteBuffer), 1);
    if (addr == nullptr) return E_OUT_OF_MEMORY_ERROR;

    CDirectByteBuffer* _obj = new(addr) CDirectByteBuffer();
    ec = _obj->Constructor(memoryRef, mark, pos, lim, cap, off, isReadOnly);
    if (FAILED(ec)) {
        free(addr);
        return ec;
    }
    AutoPtr<IMetaComponent> comp;
    clsObject->GetMetadate(&comp);
    _obj->AttachMetadata(comp, String("ccm::io::CDirectByteBuffer"));
    *object = _obj->Probe(iid);
    REFCOUNT_ADD(*object);
    return NOERROR;
}

CCM_OBJECT_IMPL(CFile);
CCM_OBJECT_IMPL(CFileDescriptor);
CCM_OBJECT_IMPL(CFileInputStream);
CCM_OBJECT_IMPL(CFileOutputStream);
CCM_OBJECT_IMPL(COutputStreamWriter);
CCM_OBJECT_IMPL(CPrintWriter);
CCM_OBJECT_IMPL(CStringWriter);

}
}
