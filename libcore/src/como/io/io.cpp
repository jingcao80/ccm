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

#include "como/io/CBufferedWriter.h"
#include "como/io/CByteArrayOutputStream.h"
#include "como/io/CDirectByteBuffer.h"
#include "como/io/CFile.h"
#include "como/io/CFileDescriptor.h"
#include "como/io/CFileInputStream.h"
#include "como/io/CFileOutputStream.h"
#include "como/io/COutputStreamWriter.h"
#include "como/io/CPrintWriter.h"
#include "como/io/CStringWriter.h"
#include <comoapi.h>
#include <new>

namespace como {
namespace io {

COMO_OBJECT_IMPL(CBufferedWriter);
COMO_OBJECT_IMPL(CByteArrayOutputStream);

COMO_OBJECT_IMPL(CDirectByteBuffer);
ECode CDirectByteBuffer::New(
    /* [in] */ Integer capacity,
    /* [in] */ MemoryRef* memoryRef,
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CDirectByteBuffer, nullptr, clsObject);
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
    clsObject->GetMetadate(comp);
    _obj->AttachMetadata(comp, String("como::io::CDirectByteBuffer"));
    *object = _obj->Probe(iid);
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode CDirectByteBuffer::New(
    /* [in] */ HANDLE address,
    /* [in] */ Integer cap,
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CDirectByteBuffer, nullptr, clsObject);
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
    clsObject->GetMetadate(comp);
    _obj->AttachMetadata(comp, String("como::io::CDirectByteBuffer"));
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
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CDirectByteBuffer, nullptr, clsObject);
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
    clsObject->GetMetadate(comp);
    _obj->AttachMetadata(comp, String("como::io::CDirectByteBuffer"));
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
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CDirectByteBuffer, nullptr, clsObject);
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
    clsObject->GetMetadate(comp);
    _obj->AttachMetadata(comp, String("como::io::CDirectByteBuffer"));
    *object = _obj->Probe(iid);
    REFCOUNT_ADD(*object);
    return NOERROR;
}

COMO_OBJECT_IMPL(CFile);
COMO_OBJECT_IMPL(CFileDescriptor);
COMO_OBJECT_IMPL(CFileInputStream);
COMO_OBJECT_IMPL(CFileOutputStream);
COMO_OBJECT_IMPL(COutputStreamWriter);
COMO_OBJECT_IMPL(CPrintWriter);
COMO_OBJECT_IMPL(CStringWriter);

}
}
