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

#include "como/core/CArrayHolder.h"
#include "como/core/CStringBuffer.h"
#include "como/core/Math.h"
#include "como/io/Bits.h"
#include "como/io/ByteBuffer.h"
#include "como/io/ByteOrder.h"
#include "como/io/CDirectByteBuffer.h"
#include "como/io/HeapByteBuffer.h"
#include <ccmlogger.h>

using como::core::CArrayHolder;
using como::core::CStringBuffer;
using como::core::E_ILLEGAL_STATE_EXCEPTION;
using como::core::IArrayHolder;
using como::core::IID_IArrayHolder;
using como::core::IID_IComparable;
using como::core::IID_IStringBuffer;
using como::core::IStringBuffer;
using como::core::Math;

namespace como {
namespace io {

COMO_INTERFACE_IMPL_2(ByteBuffer, Buffer, IByteBuffer, IComparable);

ByteBuffer::ByteBuffer()
{
    mNativeByteOrder = (Bits::ByteOrder() == ByteOrder::GetBIG_ENDIAN());
}

ECode ByteBuffer::Constructor(
    /* [in] */ Integer mark,
    /* [in] */ Integer pos,
    /* [in] */ Integer lim,
    /* [in] */ Integer cap,
    /* [in] */ const Array<Byte>& hb,
    /* [in] */ Integer offset)
{
    FAIL_RETURN(Buffer::Constructor(mark, pos, lim, cap, 0));
    mHb = hb;
    mOffset = offset;
    return NOERROR;
}

ECode ByteBuffer::Constructor(
    /* [in] */ Integer mark,
    /* [in] */ Integer pos,
    /* [in] */ Integer lim,
    /* [in] */ Integer cap)
{
    Array<Byte> hb = Array<Byte>::Null();
    return Constructor(mark, pos, lim, cap, hb, 0);
}

ECode ByteBuffer::AllocateDirect(
    /* [in] */ Integer capacity,
    /* [out] */ IByteBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer);

    if (capacity < 0) {
        Logger::E("ByteBuffer", "capacity < 0: %d", capacity);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<DirectByteBuffer::MemoryRef> memoryRef = new DirectByteBuffer::MemoryRef(capacity);
    return CDirectByteBuffer::New(capacity, memoryRef, IID_IByteBuffer, (IInterface**)buffer);
}

ECode ByteBuffer::Allocate(
    /* [in] */ Integer capacity,
    /* [out] */ IByteBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer);

    if (capacity < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<HeapByteBuffer> hbb = new HeapByteBuffer();
    FAIL_RETURN(hbb->Constructor(capacity, capacity));
    *buffer = hbb;
    REFCOUNT_ADD(*buffer);
    return NOERROR;
}

ECode ByteBuffer::Wrap(
    /* [in] */ const Array<Byte>& array,
    /* [in] */ Integer offset,
    /* [in] */ Integer length,
    /* [out] */ IByteBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer);

    AutoPtr<HeapByteBuffer> hbb = new HeapByteBuffer();
    FAIL_RETURN(hbb->Constructor(array, offset, length));
    *buffer = hbb;
    REFCOUNT_ADD(*buffer);
    return NOERROR;
}

ECode ByteBuffer::Wrap(
    /* [in] */ const Array<Byte>& array,
    /* [out] */ IByteBuffer** buffer)
{
    return Wrap(array, 0, array.GetLength(), buffer);
}

ECode ByteBuffer::Get(
    /* [out] */ Array<Byte>& dst,
    /* [in] */ Integer offset,
    /* [in] */ Integer length)
{
    FAIL_RETURN(CheckBounds(offset, length, dst.GetLength()));
    Integer remaining;
    if (Remaining(&remaining), length > remaining) {
        return E_BUFFER_UNDERFLOW_EXCEPTION;
    }
    Integer end = offset + length;
    for (Integer i = offset; i < end; i++) {
        Get(&dst[i]);
    }
    return NOERROR;
}

ECode ByteBuffer::Get(
    /* [out] */ Array<Byte>& dst)
{
    return Get(dst, 0, dst.GetLength());
}

ECode ByteBuffer::Put(
    /* [in] */ IByteBuffer* src)
{
    Boolean accessible;
    if (IsAccessible(&accessible), !accessible) {
        Logger::E("ByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (src == (IByteBuffer*)this) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mIsReadOnly) {
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }

    ByteBuffer* srcObj = (ByteBuffer*)src;

    Integer n, remaining;
    srcObj->Remaining(&n);
    if (Remaining(&remaining), n > remaining) {
        return E_BUFFER_OVERFLOW_EXCEPTION;
    }

    if (!mHb.IsNull() && !srcObj->mHb.IsNull()) {
        Integer srcPos, thisPos;
        srcObj->GetPosition(&srcPos);
        GetPosition(&thisPos);
        mHb.Copy(thisPos + mOffset, srcObj->mHb, srcPos + srcObj->mOffset, n);
    }
    else {
        HANDLE srcAddr, dstAddr;
        Boolean direct;
        if (srcObj->IsDirect(&direct), direct) {
            ((DirectByteBuffer*)srcObj)->GetAddress(&srcAddr);
        }
        else {
            srcAddr = reinterpret_cast<HANDLE>(srcObj->mHb.GetPayload());
        }
        Integer srcOffset;
        srcObj->GetPosition(&srcOffset);
        if (!direct) {
            srcOffset += srcObj->mOffset;
        }

        if (IsDirect(&direct), direct) {
            ((DirectByteBuffer*)this)->GetAddress(&dstAddr);
        }
        else {
            dstAddr = reinterpret_cast<HANDLE>(mHb.GetPayload());
        }
        Integer dstOffset;
        GetPosition(&dstOffset);
        if (!direct) {
            dstOffset += mOffset;
        }
        memmove(reinterpret_cast<void*>(dstAddr + dstOffset), reinterpret_cast<void*>(srcAddr + srcOffset), n);
    }
    Integer limit;
    srcObj->GetLimit(&limit);
    srcObj->SetPosition(limit);
    Integer pos;
    GetPosition(&pos);
    SetPosition(pos + n);
    return NOERROR;
}

ECode ByteBuffer::Put(
    /* [in] */ const Array<Byte>& src,
    /* [in] */ Integer offset,
    /* [in] */ Integer length)
{
    FAIL_RETURN(CheckBounds(offset, length, src.GetLength()));
    Integer remaining;
    if (Remaining(&remaining), length > remaining) {
        return E_BUFFER_OVERFLOW_EXCEPTION;
    }
    Integer end = offset + length;
    for (Integer i = offset; i < end; i++) {
        Put(src[i]);
    }
    return NOERROR;
}

ECode ByteBuffer::Put(
    /* [in] */ const Array<Byte>& src)
{
    return Put(src, 0, src.GetLength());
}

ECode ByteBuffer::HasArray(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (mHb.IsNull()) {
        *result = false;
        return NOERROR;
    }

    Boolean readOnly;
    IsReadOnly(&readOnly);
    *result = !readOnly;
    return NOERROR;
}

ECode ByteBuffer::GetArray(
    /* [out] */ IInterface** array)
{
    VALIDATE_NOT_NULL(array);

    if (mHb.IsNull()) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    if (mIsReadOnly) {
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    return CArrayHolder::New(mHb, IID_IArrayHolder, array);
}

ECode ByteBuffer::GetArrayOffset(
    /* [out] */ Integer* offset)
{
    VALIDATE_NOT_NULL(offset);

    if (mHb.IsNull()) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    if (mIsReadOnly) {
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    *offset = mOffset;
    return NOERROR;
}

ECode ByteBuffer::ToString(
    /* [out] */ String* desc)
{
    VALIDATE_NOT_NULL(desc);

    AutoPtr<IStringBuffer> sb;
    CStringBuffer::New(IID_IStringBuffer, (IInterface**)&sb);
    sb->Append(Object::GetCoclassName(this));
    sb->Append(String("[pos="));
    Integer value;
    GetPosition(&value);
    sb->Append(value);
    sb->Append(String(" lim="));
    GetLimit(&value);
    sb->Append(value);
    sb->Append(String(" cap="));
    GetCapacity(&value);
    sb->Append(value);
    sb->Append(String("]"));
    return sb->ToString(desc);
}

ECode ByteBuffer::GetHashCode(
    /* [out] */ Integer* hash)
{
    VALIDATE_NOT_NULL(hash);

    Integer h = 1;
    Integer p;
    GetPosition(&p);
    Integer i;
    GetLimit(&i);
    for (i = i - 1; i >= p; i--) {
        Byte b;
        Get(i, &b);
        h = 31 * h + b;
    }
    *hash = h;
    return NOERROR;
}

ECode ByteBuffer::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* same)
{
    VALIDATE_NOT_NULL(same);

    ByteBuffer* other = (ByteBuffer*)IByteBuffer::Probe(obj);
    if (other == nullptr) {
        *same = false;
        return NOERROR;
    }
    if (this == other) {
        *same = true;
        return NOERROR;
    }
    Integer thisRemaining, otherRemaining;
    Remaining(&thisRemaining);
    other->Remaining(&otherRemaining);
    if (thisRemaining != otherRemaining) {
        *same = false;
        return NOERROR;
    }
    Integer p;
    GetPosition(&p);
    Integer i, j;
    GetLimit(&i);
    other->GetLimit(&j);
    for (i = i - 1, j = j - 1; i >= p; i--, j--) {
        Byte thisb, otherb;
        Get(i, &thisb);
        other->Get(j, &otherb);
        if (thisb != otherb) {
            *same = false;
            return NOERROR;
        }
    }
    *same = true;
    return NOERROR;
}

ECode ByteBuffer::CompareTo(
    /* [in] */ IInterface* other,
    /* [out] */ Integer* result)
{
    VALIDATE_NOT_NULL(result);

    ByteBuffer* otherBB = (ByteBuffer*)IByteBuffer::Probe(other);
    if (otherBB == nullptr) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer thisRemaining, otherRemaining;
    Remaining(&thisRemaining);
    otherBB->Remaining(&otherRemaining);
    Integer thisPos, otherPos;
    GetPosition(&thisPos);
    otherBB->GetPosition(&otherPos);

    Integer n = thisPos + Math::Min(thisRemaining, otherRemaining);
    for (Integer i = thisPos, j = otherPos; i < n; i++, j++) {
        Byte thisb, otherb;
        Get(i, &thisb);
        otherBB->Get(j, &otherb);
        Integer cmp = thisb - otherb;
        if (cmp != 0) {
            *result = cmp;
            return NOERROR;
        }
    }
    *result = thisRemaining - otherRemaining;
    return NOERROR;
}

ECode ByteBuffer::GetOrder(
    /* [out] */ IByteOrder** bo)
{
    VALIDATE_NOT_NULL(bo);

    if (mBigEndian) {
        *bo = ByteOrder::GetBIG_ENDIAN();
        REFCOUNT_ADD(*bo);
        return NOERROR;
    }
    else {
        *bo = ByteOrder::GetLITTLE_ENDIAN();
        REFCOUNT_ADD(*bo);
        return NOERROR;
    }
}

ECode ByteBuffer::SetOrder(
    /* [in] */ IByteOrder* bo)
{
    mBigEndian = (bo == ByteOrder::GetBIG_ENDIAN());
    mNativeByteOrder = (mBigEndian ==
            (Bits::ByteOrder() == ByteOrder::GetBIG_ENDIAN()));
    return NOERROR;
}

ECode ByteBuffer::IsAccessible(
    /* [out] */ Boolean* accessible)
{
    VALIDATE_NOT_NULL(accessible);

    *accessible = true;
    return NOERROR;
}

ECode ByteBuffer::SetAccessible(
    /* [in] */ Boolean value)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

}
}