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

#include "como/io/ByteOrder.h"
#include "como/io/HeapLongBuffer.h"

namespace como {
namespace io {

extern const CoclassID CID_HeapLongBuffer =
        {{0x2bceaadf,0x64fd,0x445e,0xb537,{0xc3,0xa8,0x40,0x35,0x64,0x4f}}, &CID_libcore};

ECode HeapLongBuffer::Constructor(
    /* [in] */ Integer cap,
    /* [in] */ Integer lim)
{
    return Constructor(cap, lim, false);
}

ECode HeapLongBuffer::Constructor(
    /* [in] */ Integer cap,
    /* [in] */ Integer lim,
    /* [in] */ Boolean isReadOnly)
{
    Array<Long> hb(cap);
    FAIL_RETURN(LongBuffer::Constructor(-1, 0, lim, cap, hb, 0));
    mIsReadOnly = isReadOnly;
    return NOERROR;
}

ECode HeapLongBuffer::Constructor(
    /* [in] */ const Array<Long>& buf,
    /* [in] */ Integer off,
    /* [in] */ Integer len)
{
    return Constructor(buf, off, len ,false);
}

ECode HeapLongBuffer::Constructor(
    /* [in] */ const Array<Long>& buf,
    /* [in] */ Integer off,
    /* [in] */ Integer len,
    /* [in] */ Boolean isReadOnly)
{
    FAIL_RETURN(LongBuffer::Constructor(-1, off, off + len, buf.GetLength(), buf, 0));
    mIsReadOnly = isReadOnly;
    return NOERROR;
}

ECode HeapLongBuffer::Constructor(
    /* [in] */ const Array<Long>& buf,
    /* [in] */ Integer mark,
    /* [in] */ Integer pos,
    /* [in] */ Integer lim,
    /* [in] */ Integer cap,
    /* [in] */ Integer off)
{
    return Constructor(buf, mark, pos, lim, cap, off, false);
}

ECode HeapLongBuffer::Constructor(
    /* [in] */ const Array<Long>& buf,
    /* [in] */ Integer mark,
    /* [in] */ Integer pos,
    /* [in] */ Integer lim,
    /* [in] */ Integer cap,
    /* [in] */ Integer off,
    /* [in] */ Boolean isReadOnly)
{
    FAIL_RETURN(LongBuffer::Constructor(mark, pos, lim, cap, buf, off));
    mIsReadOnly = isReadOnly;
    return NOERROR;
}

ECode HeapLongBuffer::Slice(
    /* [out] */ ILongBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer);

    Integer remaining, pos;
    Remaining(&remaining);
    GetPosition(&pos);
    AutoPtr<HeapLongBuffer> hlb = new HeapLongBuffer();
    FAIL_RETURN(hlb->Constructor(
            mHb, -1, 0, remaining, remaining, pos + mOffset, mIsReadOnly));
    *buffer = (ILongBuffer*)hlb.Get();
    REFCOUNT_ADD(*buffer);
    return NOERROR;
}

ECode HeapLongBuffer::Duplicate(
    /* [out] */ ILongBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer);

    Integer pos, lim, cap;
    GetPosition(&pos);
    GetLimit(&lim);
    GetCapacity(&cap);
    AutoPtr<HeapLongBuffer> hlb = new HeapLongBuffer();
    FAIL_RETURN(hlb->Constructor(
            mHb, MarkValue(), pos, lim, cap, mOffset, mIsReadOnly));
    *buffer = (ILongBuffer*)hlb.Get();
    REFCOUNT_ADD(*buffer);
    return NOERROR;
}

ECode HeapLongBuffer::AsReadOnlyBuffer(
    /* [out] */ ILongBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer);

    Integer pos, lim, cap;
    GetPosition(&pos);
    GetLimit(&lim);
    GetCapacity(&cap);
    AutoPtr<HeapLongBuffer> hlb = new HeapLongBuffer();
    FAIL_RETURN(hlb->Constructor(
            mHb, MarkValue(), pos, lim, cap, mOffset, true));
    *buffer = (ILongBuffer*)hlb.Get();
    REFCOUNT_ADD(*buffer);
    return NOERROR;
}

ECode HeapLongBuffer::Get(
    /* [out] */ Long* l)
{
    VALIDATE_NOT_NULL(l);

    Integer index;
    NextGetIndex(&index);
    *l = mHb[Ix(index)];
    return NOERROR;
}

ECode HeapLongBuffer::Get(
    /* [in] */ Integer index,
    /* [out] */ Long* l)
{
    VALIDATE_NOT_NULL(l);

    FAIL_RETURN(CheckIndex(index));
    *l = mHb[Ix(index)];
    return NOERROR;
}

ECode HeapLongBuffer::Get(
    /* [out] */ Array<Long>& dst,
    /* [in] */ Integer offset,
    /* [in] */ Integer length)
{
    FAIL_RETURN(CheckBounds(mOffset, length, dst.GetLength()));
    Integer remaining;
    Remaining(&remaining);
    if (length > remaining){
        return E_BUFFER_UNDERFLOW_EXCEPTION;
    }
    Integer pos;
    GetPosition(&pos);
    dst.Copy(offset, mHb, Ix(pos), length);
    SetPosition(pos + length);
    return NOERROR;
}

ECode HeapLongBuffer::IsDirect(
    /* [out] */ Boolean* direct)
{
    VALIDATE_NOT_NULL(direct);

    *direct = false;
    return NOERROR;
}

ECode HeapLongBuffer::IsReadOnly(
    /* [out] */ Boolean* readOnly)
{
    VALIDATE_NOT_NULL(readOnly);

    *readOnly = mIsReadOnly;
    return NOERROR;
}

ECode HeapLongBuffer::Put(
    /* [in] */ Long l)
{
    if (mIsReadOnly) {
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    Integer index;
    NextPutIndex(&index);
    mHb[Ix(index)] = l;
    return NOERROR;
}

ECode HeapLongBuffer::Put(
    /* [in] */ Integer index,
    /* [in] */ Long l)
{
    if (mIsReadOnly) {
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    FAIL_RETURN(CheckIndex(index));
    mHb[Ix(index)] = l;
    return NOERROR;
}

ECode HeapLongBuffer::Put(
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
    mHb.Copy(Ix(pos), src, offset, length);
    SetPosition(pos + length);
    return NOERROR;
}

ECode HeapLongBuffer::Put(
    /* [in] */ ILongBuffer* src)
{
    if (src == (ILongBuffer*)this) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mIsReadOnly) {
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    Boolean direct;
    if (Object::InstanceOf(src, CID_HeapLongBuffer)) {
        HeapLongBuffer* hlb = (HeapLongBuffer*)src;
        Integer n, remaining;
        hlb->Remaining(&n);
        if (Remaining(&remaining), n > remaining) {
            return E_BUFFER_OVERFLOW_EXCEPTION;
        }
        Integer thisPos, hlbPos;
        GetPosition(&thisPos);
        hlb->GetPosition(&hlbPos);
        mHb.Copy(Ix(thisPos), hlb->mHb, hlb->Ix(hlbPos), n);
        hlb->SetPosition(hlbPos + n);
        SetPosition(thisPos + n);
        return NOERROR;
    }
    else if (IBuffer::Probe(src)->IsDirect(&direct), direct) {
        Integer n, remaining;
        IBuffer::Probe(src)->Remaining(&n);
        if (Remaining(&remaining), n > remaining) {
            return E_BUFFER_OVERFLOW_EXCEPTION;
        }
        Integer pos;
        GetPosition(&pos);
        src->Get(mHb, Ix(pos), n);
        SetPosition(pos + n);
        return NOERROR;
    }
    else {
        return LongBuffer::Put(src);
    }
}

ECode HeapLongBuffer::Compact()
{
    if (mIsReadOnly) {
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    Integer pos, remaining, cap;
    GetPosition(&pos);
    Remaining(&remaining);
    mHb.Copy(Ix(0), mHb, Ix(pos), remaining);
    SetPosition(remaining);
    GetCapacity(&cap);
    SetLimit(cap);
    DiscardMark();
    return NOERROR;
}

ECode HeapLongBuffer::GetOrder(
    /* [out] */ IByteOrder** bo)
{
    VALIDATE_NOT_NULL(bo);

    AutoPtr<IByteOrder> order = ByteOrder::Order();
    order.MoveTo(bo);
    return NOERROR;
}

ECode HeapLongBuffer::GetCoclassID(
    /* [out] */ CoclassID& cid)
{
    cid = CID_HeapLongBuffer;
    return NOERROR;
}

}
}
