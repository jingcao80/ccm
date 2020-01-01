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

#include "como/io/ByteBufferAsLongBuffer.h"
#include "como/io/CDirectByteBuffer.h"
#include "como.core.IArrayHolder.h"

using como::core::IArrayHolder;

namespace como {
namespace io {

ECode ByteBufferAsLongBuffer::Constructor(
    /* [in] */ ByteBuffer* bb,
    /* [in] */ Integer mark,
    /* [in] */ Integer pos,
    /* [in] */ Integer lim,
    /* [in] */ Integer cap,
    /* [in] */ Integer off,
    /* [in] */ IByteOrder* order)
{
    FAIL_RETURN(LongBuffer::Constructor(mark, pos, lim, cap));
    AutoPtr<IByteBuffer> newBB;
    bb->Duplicate(&newBB);
    mBB = (ByteBuffer*)newBB.Get();
    bb->IsReadOnly(&mIsReadOnly);
    if (Object::InstanceOf(bb, CID_CDirectByteBuffer)) {
        mAddress = bb->mAddress + off;
    }
    mBB->SetOrder(order);
    mOrder = order;
    mOffset = off;
    return NOERROR;
}

ECode ByteBufferAsLongBuffer::Slice(
    /* [out] */ ILongBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer);

    Integer pos, lim;
    GetPosition(&pos);
    GetLimit(&lim);
    CHECK(pos <= lim);
    Integer rem = (pos <= lim ? lim - pos : 0);
    Integer off = (pos << 3) + mOffset;
    CHECK(off >= 0);
    AutoPtr<ByteBufferAsLongBuffer> bb = new ByteBufferAsLongBuffer();
    FAIL_RETURN(bb->Constructor(mBB, -1, 0, rem, rem, off, mOrder));
    *buffer = (ILongBuffer*)bb.Get();
    REFCOUNT_ADD(*buffer);
    return NOERROR;
}

ECode ByteBufferAsLongBuffer::Duplicate(
    /* [out] */ ILongBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer);

    Integer pos, lim, cap;
    GetPosition(&pos);
    GetLimit(&lim);
    GetCapacity(&cap);
    AutoPtr<ByteBufferAsLongBuffer> bb = new ByteBufferAsLongBuffer();
    FAIL_RETURN(bb->Constructor(
            mBB, MarkValue(), pos, lim, cap, mOffset, mOrder));
    *buffer = (ILongBuffer*)bb.Get();
    REFCOUNT_ADD(*buffer);
    return NOERROR;
}

ECode ByteBufferAsLongBuffer::AsReadOnlyBuffer(
    /* [out] */ ILongBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer);

    AutoPtr<IByteBuffer> rb;
    mBB->AsReadOnlyBuffer(&rb);
    Integer pos, lim, cap;
    GetPosition(&pos);
    GetLimit(&lim);
    GetCapacity(&cap);
    AutoPtr<ByteBufferAsLongBuffer> bb = new ByteBufferAsLongBuffer();
    FAIL_RETURN(bb->Constructor(
            (ByteBuffer*)rb.Get(), MarkValue(), pos, lim, cap, mOffset, mOrder));
    *buffer = (ILongBuffer*)bb.Get();
    REFCOUNT_ADD(*buffer);
    return NOERROR;
}

ECode ByteBufferAsLongBuffer::Get(
    /* [out] */ Long* l)
{
    VALIDATE_NOT_NULL(l);

    Integer index;
    NextGetIndex(&index);
    return Get(index, l);
}

ECode ByteBufferAsLongBuffer::Get(
    /* [in] */ Integer index,
    /* [out] */ Long* l)
{
    VALIDATE_NOT_NULL(l);

    FAIL_RETURN(CheckIndex(index));
    return mBB->GetLongUnchecked(Ix(index), l);
}

ECode ByteBufferAsLongBuffer::Get(
    /* [out] */ Array<Long>& dst,
    /* [in] */ Integer offset,
    /* [in] */ Integer length)
{
    FAIL_RETURN(CheckBounds(offset, length, dst.GetLength()));
    Integer remaining;
    Remaining(&remaining);
    if (length > remaining){
        return E_BUFFER_UNDERFLOW_EXCEPTION;
    }
    Integer pos;
    GetPosition(&pos);
    mBB->GetUnchecked(Ix(pos), dst, offset, length);
    SetPosition(pos + length);
    return NOERROR;
}

ECode ByteBufferAsLongBuffer::Put(
    /* [in] */ Long l)
{
    Integer index;
    NextPutIndex(&index);
    return Put(index, l);
}

ECode ByteBufferAsLongBuffer::Put(
    /* [in] */ Integer index,
    /* [in] */ Long l)
{
    if (mIsReadOnly) {
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    FAIL_RETURN(CheckIndex(index));
    return mBB->PutLongUnchecked(Ix(index), l);
}

ECode ByteBufferAsLongBuffer::Put(
    /* [in] */ const Array<Long>& src,
    /* [in] */ Integer offset,
    /* [in] */ Integer length)
{
    if (mIsReadOnly) {
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    FAIL_RETURN(CheckBounds(offset, length, src.GetLength()));
    Integer remaining;
    Remaining(&remaining);
    if (length > remaining) {
        return E_BUFFER_OVERFLOW_EXCEPTION;
    }
    Integer pos;
    GetPosition(&pos);
    mBB->PutUnchecked(Ix(pos), src, offset, length);
    SetPosition(pos + length);
    return NOERROR;
}

ECode ByteBufferAsLongBuffer::Compact()
{
    if (mIsReadOnly) {
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    Integer pos, lim, cap;
    GetPosition(&pos);
    GetLimit(&lim);
    CHECK(pos <= lim);
    Integer rem = (pos <= lim ? lim - pos : 0);
    if (!Object::InstanceOf(mBB, CID_CDirectByteBuffer)) {
        AutoPtr<IArrayHolder> holder;
        mBB->GetArray((IInterface**)&holder);
        Array<Byte> bytes;
        holder->GetArray(&bytes);
        bytes.Copy(Ix(0), bytes, Ix(pos), rem << 3);
    }
    else {
        void* dst = reinterpret_cast<void*>(mBB->mAddress + Ix(0));
        void* src = reinterpret_cast<void*>(mBB->mAddress + Ix(pos));
        memmove(dst, src, rem << 3);
    }
    SetPosition(rem);
    GetCapacity(&cap);
    SetLimit(cap);
    DiscardMark();
    return NOERROR;
}

ECode ByteBufferAsLongBuffer::IsDirect(
    /* [out] */ Boolean* direct)
{
    return mBB->IsDirect(direct);
}

ECode ByteBufferAsLongBuffer::IsReadOnly(
    /* [out] */ Boolean* readOnly)
{
    VALIDATE_NOT_NULL(readOnly);

    *readOnly = mIsReadOnly;
    return NOERROR;
}

ECode ByteBufferAsLongBuffer::GetOrder(
    /* [out] */ IByteOrder** bo)
{
    VALIDATE_NOT_NULL(bo);

    *bo = mOrder;
    REFCOUNT_ADD(*bo);
    return NOERROR;
}

}
}
