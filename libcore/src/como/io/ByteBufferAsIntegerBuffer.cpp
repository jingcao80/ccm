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

#include "como/io/ByteBufferAsIntegerBuffer.h"
#include "como/io/CDirectByteBuffer.h"
#include "como.core.IArrayHolder.h"

using como::core::IArrayHolder;

namespace como {
namespace io {

ECode ByteBufferAsIntegerBuffer::Constructor(
    /* [in] */ ByteBuffer* bb,
    /* [in] */ Integer mark,
    /* [in] */ Integer pos,
    /* [in] */ Integer lim,
    /* [in] */ Integer cap,
    /* [in] */ Integer off,
    /* [in] */ IByteOrder* order)
{
    FAIL_RETURN(IntegerBuffer::Constructor(mark, pos, lim, cap));
    AutoPtr<IByteBuffer> newBB;
    bb->Duplicate(newBB);
    mBB = (ByteBuffer*)newBB.Get();
    bb->IsReadOnly(mIsReadOnly);
    if (Object::InstanceOf(bb, CID_CDirectByteBuffer)) {
        mAddress = bb->mAddress + off;
    }
    mBB->SetOrder(order);
    mOrder = order;
    mOffset = off;
    return NOERROR;
}

ECode ByteBufferAsIntegerBuffer::Slice(
    /* [out] */ AutoPtr<IIntegerBuffer>& buffer)
{
    Integer pos, lim;
    GetPosition(pos);
    GetLimit(lim);
    CHECK(pos <= lim);
    Integer rem = (pos <= lim ? lim - pos : 0);
    Integer off = (pos << 2) + mOffset;
    CHECK(off >= 0);
    AutoPtr<ByteBufferAsIntegerBuffer> bb = new ByteBufferAsIntegerBuffer();
    FAIL_RETURN(bb->Constructor(mBB, -1, 0, rem, rem, off, mOrder));
    buffer = (IIntegerBuffer*)bb.Get();
    return NOERROR;
}

ECode ByteBufferAsIntegerBuffer::Duplicate(
    /* [out] */ AutoPtr<IIntegerBuffer>& buffer)
{
    Integer pos, lim, cap;
    GetPosition(pos);
    GetLimit(lim);
    GetCapacity(cap);
    AutoPtr<ByteBufferAsIntegerBuffer> bb = new ByteBufferAsIntegerBuffer();
    FAIL_RETURN(bb->Constructor(
            mBB, MarkValue(), pos, lim, cap, mOffset, mOrder));
    buffer = (IIntegerBuffer*)bb.Get();
    return NOERROR;
}

ECode ByteBufferAsIntegerBuffer::AsReadOnlyBuffer(
    /* [out] */ AutoPtr<IIntegerBuffer>& buffer)
{
    AutoPtr<IByteBuffer> rb;
    mBB->AsReadOnlyBuffer(rb);
    Integer pos, lim, cap;
    GetPosition(pos);
    GetLimit(lim);
    GetCapacity(cap);
    AutoPtr<ByteBufferAsIntegerBuffer> bb = new ByteBufferAsIntegerBuffer();
    FAIL_RETURN(bb->Constructor(
            (ByteBuffer*)rb.Get(), MarkValue(), pos, lim, cap, mOffset, mOrder));
    buffer = (IIntegerBuffer*)bb.Get();
    return NOERROR;
}

ECode ByteBufferAsIntegerBuffer::Get(
    /* [out] */ Integer& i)
{
    Integer index;
    NextGetIndex(&index);
    return Get(index, i);
}

ECode ByteBufferAsIntegerBuffer::Get(
    /* [in] */ Integer index,
    /* [out] */ Integer& i)
{
    FAIL_RETURN(CheckIndex(index));
    return mBB->GetIntegerUnchecked(Ix(index), i);
}

ECode ByteBufferAsIntegerBuffer::Get(
    /* [out] */ Array<Integer>& dst,
    /* [in] */ Integer offset,
    /* [in] */ Integer length)
{
    FAIL_RETURN(CheckBounds(offset, length, dst.GetLength()));
    Integer remaining;
    Remaining(remaining);
    if (length > remaining){
        return E_BUFFER_UNDERFLOW_EXCEPTION;
    }
    Integer pos;
    GetPosition(pos);
    mBB->GetUnchecked(Ix(pos), dst, offset, length);
    SetPosition(pos + length);
    return NOERROR;
}

ECode ByteBufferAsIntegerBuffer::Put(
    /* [in] */ Integer i)
{
    Integer index;
    NextPutIndex(&index);
    return Put(index, i);
}

ECode ByteBufferAsIntegerBuffer::Put(
    /* [in] */ Integer index,
    /* [in] */ Integer i)
{
    if (mIsReadOnly) {
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    FAIL_RETURN(CheckIndex(index));
    return mBB->PutIntegerUnchecked(Ix(index), i);
}

ECode ByteBufferAsIntegerBuffer::Put(
    /* [in] */ const Array<Integer>& src,
    /* [in] */ Integer offset,
    /* [in] */ Integer length)
{
    if (mIsReadOnly) {
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    FAIL_RETURN(CheckBounds(offset, length, src.GetLength()));
    Integer remaining;
    Remaining(remaining);
    if (length > remaining) {
        return E_BUFFER_OVERFLOW_EXCEPTION;
    }
    Integer pos;
    GetPosition(pos);
    mBB->PutUnchecked(Ix(pos), src, offset, length);
    SetPosition(pos + length);
    return NOERROR;
}

ECode ByteBufferAsIntegerBuffer::Compact()
{
    if (mIsReadOnly) {
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    Integer pos, lim, cap;
    GetPosition(pos);
    GetLimit(lim);
    CHECK(pos <= lim);
    Integer rem = (pos <= lim ? lim - pos : 0);
    if (!Object::InstanceOf(mBB, CID_CDirectByteBuffer)) {
        AutoPtr<IArrayHolder> holder;
        mBB->GetArray(holder);
        Array<Byte> bytes;
        holder->GetArray(&bytes);
        bytes.Copy(Ix(0), bytes, Ix(pos), rem << 2);
    }
    else {
        void* dst = reinterpret_cast<void*>(mBB->mAddress + Ix(0));
        void* src = reinterpret_cast<void*>(mBB->mAddress + Ix(pos));
        memmove(dst, src, rem << 2);
    }
    SetPosition(rem);
    GetCapacity(cap);
    SetLimit(cap);
    DiscardMark();
    return NOERROR;
}

ECode ByteBufferAsIntegerBuffer::IsDirect(
    /* [out] */ Boolean& direct)
{
    return mBB->IsDirect(direct);
}

ECode ByteBufferAsIntegerBuffer::IsReadOnly(
    /* [out] */ Boolean& readOnly)
{
    readOnly = mIsReadOnly;
    return NOERROR;
}

ECode ByteBufferAsIntegerBuffer::GetOrder(
    /* [out] */ AutoPtr<IByteOrder>& bo)
{
    bo = mOrder;
    return NOERROR;
}

}
}
