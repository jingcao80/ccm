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

#include "ccm/io/ByteBufferAsShortBuffer.h"
#include "ccm/io/CDirectByteBuffer.h"
#include "ccm.core.IArrayHolder.h"

using ccm::core::IArrayHolder;

namespace ccm {
namespace io {

ECode ByteBufferAsShortBuffer::Constructor(
    /* [in] */ ByteBuffer* bb,
    /* [in] */ Integer mark,
    /* [in] */ Integer pos,
    /* [in] */ Integer lim,
    /* [in] */ Integer cap,
    /* [in] */ Integer off,
    /* [in] */ IByteOrder* order)
{
    FAIL_RETURN(ShortBuffer::Constructor(mark, pos, lim, cap));
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

ECode ByteBufferAsShortBuffer::Slice(
    /* [out] */ IShortBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer);

    Integer pos, lim;
    GetPosition(&pos);
    GetLimit(&lim);
    CHECK(pos <= lim);
    Integer rem = (pos <= lim ? lim - pos : 0);
    Integer off = (pos << 1) + mOffset;
    CHECK(off >= 0);
    AutoPtr<ByteBufferAsShortBuffer> bb = new ByteBufferAsShortBuffer();
    FAIL_RETURN(bb->Constructor(mBB, -1, 0, rem, rem, off, mOrder));
    *buffer = (IShortBuffer*)bb.Get();
    REFCOUNT_ADD(*buffer);
    return NOERROR;
}

ECode ByteBufferAsShortBuffer::Duplicate(
    /* [out] */ IShortBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer);

    Integer pos, lim, cap;
    GetPosition(&pos);
    GetLimit(&lim);
    GetCapacity(&cap);
    AutoPtr<ByteBufferAsShortBuffer> bb = new ByteBufferAsShortBuffer();
    FAIL_RETURN(bb->Constructor(
            mBB, MarkValue(), pos, lim, cap, mOffset, mOrder));
    *buffer = (IShortBuffer*)bb.Get();
    REFCOUNT_ADD(*buffer);
    return NOERROR;
}

ECode ByteBufferAsShortBuffer::AsReadOnlyBuffer(
    /* [out] */ IShortBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer);

    AutoPtr<IByteBuffer> rb;
    mBB->AsReadOnlyBuffer(&rb);
    Integer pos, lim, cap;
    GetPosition(&pos);
    GetLimit(&lim);
    GetCapacity(&cap);
    AutoPtr<ByteBufferAsShortBuffer> bb = new ByteBufferAsShortBuffer();
    FAIL_RETURN(bb->Constructor(
            (ByteBuffer*)rb.Get(), MarkValue(), pos, lim, cap, mOffset, mOrder));
    *buffer = (IShortBuffer*)bb.Get();
    REFCOUNT_ADD(*buffer);
    return NOERROR;
}

ECode ByteBufferAsShortBuffer::Get(
    /* [out] */ Short* s)
{
    VALIDATE_NOT_NULL(s);

    Integer index;
    NextGetIndex(&index);
    return Get(index, s);
}

ECode ByteBufferAsShortBuffer::Get(
    /* [in] */ Integer index,
    /* [out] */ Short* s)
{
    VALIDATE_NOT_NULL(s);

    FAIL_RETURN(CheckIndex(index));
    return mBB->GetShortUnchecked(Ix(index), s);
}

ECode ByteBufferAsShortBuffer::Get(
    /* [out] */ Array<Short>& dst,
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

ECode ByteBufferAsShortBuffer::Put(
    /* [in] */ Short s)
{
    Integer index;
    NextPutIndex(&index);
    return Put(index, s);
}

ECode ByteBufferAsShortBuffer::Put(
    /* [in] */ Integer index,
    /* [in] */ Short s)
{
    if (mIsReadOnly) {
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    FAIL_RETURN(CheckIndex(index));
    return mBB->PutIntegerUnchecked(Ix(index), s);
}

ECode ByteBufferAsShortBuffer::Put(
    /* [in] */ const Array<Short>& src,
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

ECode ByteBufferAsShortBuffer::Compact()
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
        bytes.Copy(Ix(0), bytes, Ix(pos), rem << 1);
    }
    else {
        void* dst = reinterpret_cast<void*>(mBB->mAddress + Ix(0));
        void* src = reinterpret_cast<void*>(mBB->mAddress + Ix(pos));
        memmove(dst, src, rem << 1);
    }
    SetPosition(rem);
    GetCapacity(&cap);
    SetLimit(cap);
    DiscardMark();
    return NOERROR;
}

ECode ByteBufferAsShortBuffer::IsDirect(
    /* [out] */ Boolean* direct)
{
    return mBB->IsDirect(direct);
}

ECode ByteBufferAsShortBuffer::IsReadOnly(
    /* [out] */ Boolean* readOnly)
{
    VALIDATE_NOT_NULL(readOnly);

    *readOnly = mIsReadOnly;
    return NOERROR;
}

ECode ByteBufferAsShortBuffer::GetOrder(
    /* [out] */ IByteOrder** bo)
{
    VALIDATE_NOT_NULL(bo);

    *bo = mOrder;
    REFCOUNT_ADD(*bo);
    return NOERROR;
}

}
}
