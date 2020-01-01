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
#include "como/io/HeapDoubleBuffer.h"

namespace como {
namespace io {

extern const CoclassID CID_HeapDoubleBuffer =
        {{0x4f9fbfc5,0x7abe,0x4756,0xbc71,{0x7,0x4,0xb,0xb,0x2,0x3,0x5,0xd,0xa,0xf,0x9,0xa}}, &CID_libcore};

ECode HeapDoubleBuffer::Constructor(
    /* [in] */ Integer cap,
    /* [in] */ Integer lim)
{
    return Constructor(cap, lim, false);
}

ECode HeapDoubleBuffer::Constructor(
    /* [in] */ const Array<Double>& buf,
    /* [in] */ Integer off,
    /* [in] */ Integer len)
{
    return Constructor(buf, off, len ,false);
}

ECode HeapDoubleBuffer::Constructor(
    /* [in] */ const Array<Double>& buf,
    /* [in] */ Integer mark,
    /* [in] */ Integer pos,
    /* [in] */ Integer lim,
    /* [in] */ Integer cap,
    /* [in] */ Integer off)
{
    return Constructor(buf, mark, pos, lim, cap, off, false);
}

ECode HeapDoubleBuffer::Constructor(
    /* [in] */ Integer cap,
    /* [in] */ Integer lim,
    /* [in] */ Boolean isReadOnly)
{
    Array<Double> hb(cap);
    FAIL_RETURN(DoubleBuffer::Constructor(-1, 0, lim, cap, hb, 0));
    mIsReadOnly = isReadOnly;
    return NOERROR;
}

ECode HeapDoubleBuffer::Constructor(
    /* [in] */ const Array<Double>& buf,
    /* [in] */ Integer off,
    /* [in] */ Integer len,
    /* [in] */ Boolean isReadOnly)
{
    FAIL_RETURN(DoubleBuffer::Constructor(-1, off, off + len, buf.GetLength(), buf, 0));
    mIsReadOnly = isReadOnly;
    return NOERROR;
}

ECode HeapDoubleBuffer::Constructor(
    /* [in] */ const Array<Double>& buf,
    /* [in] */ Integer mark,
    /* [in] */ Integer pos,
    /* [in] */ Integer lim,
    /* [in] */ Integer cap,
    /* [in] */ Integer off,
    /* [in] */ Boolean isReadOnly)
{
    FAIL_RETURN(DoubleBuffer::Constructor(mark, pos, lim, cap, buf, off));
    mIsReadOnly = isReadOnly;
    return NOERROR;
}

ECode HeapDoubleBuffer::Slice(
    /* [out] */ IDoubleBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer);

    Integer remaining, pos;
    Remaining(&remaining);
    GetPosition(&pos);
    AutoPtr<HeapDoubleBuffer> hdb = new HeapDoubleBuffer();
    FAIL_RETURN(hdb->Constructor(
            mHb, -1, 0, remaining, remaining, pos + mOffset, mIsReadOnly));
    *buffer = (IDoubleBuffer*)hdb.Get();
    REFCOUNT_ADD(*buffer);
    return NOERROR;
}

ECode HeapDoubleBuffer::Duplicate(
    /* [out] */ IDoubleBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer);

    Integer pos, lim, cap;
    GetPosition(&pos);
    GetLimit(&lim);
    GetCapacity(&cap);
    AutoPtr<HeapDoubleBuffer> hdb = new HeapDoubleBuffer();
    FAIL_RETURN(hdb->Constructor(
            mHb, MarkValue(), pos, lim, cap, mOffset, mIsReadOnly));
    *buffer = (IDoubleBuffer*)hdb.Get();
    REFCOUNT_ADD(*buffer);
    return NOERROR;
}

ECode HeapDoubleBuffer::AsReadOnlyBuffer(
    /* [out] */ IDoubleBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer);

    Integer pos, lim, cap;
    GetPosition(&pos);
    GetLimit(&lim);
    GetCapacity(&cap);
    AutoPtr<HeapDoubleBuffer> hdb = new HeapDoubleBuffer();
    FAIL_RETURN(hdb->Constructor(
            mHb, MarkValue(), pos, lim, cap, mOffset, true));
    *buffer = (IDoubleBuffer*)hdb.Get();
    REFCOUNT_ADD(*buffer);
    return NOERROR;
}

ECode HeapDoubleBuffer::Get(
    /* [out] */ Double* d)
{
    VALIDATE_NOT_NULL(d);

    Integer index;
    NextGetIndex(&index);
    *d = mHb[Ix(index)];
    return NOERROR;
}

ECode HeapDoubleBuffer::Get(
    /* [in] */ Integer index,
    /* [out] */ Double* d)
{
    VALIDATE_NOT_NULL(d);

    FAIL_RETURN(CheckIndex(index));
    *d = mHb[Ix(index)];
    return NOERROR;
}

ECode HeapDoubleBuffer::Get(
    /* [out] */ Array<Double>& dst,
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

ECode HeapDoubleBuffer::IsDirect(
    /* [out] */ Boolean* direct)
{
    VALIDATE_NOT_NULL(direct);

    *direct = false;
    return NOERROR;
}

ECode HeapDoubleBuffer::IsReadOnly(
    /* [out] */ Boolean* readOnly)
{
    VALIDATE_NOT_NULL(readOnly);

    *readOnly = mIsReadOnly;
    return NOERROR;
}

ECode HeapDoubleBuffer::Put(
    /* [in] */ Double d)
{
    if (mIsReadOnly) {
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    Integer index;
    NextPutIndex(&index);
    mHb[Ix(index)] = d;
    return NOERROR;
}

ECode HeapDoubleBuffer::Put(
    /* [in] */ Integer index,
    /* [in] */ Double d)
{
    if (mIsReadOnly) {
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    FAIL_RETURN(CheckIndex(index));
    mHb[Ix(index)] = d;
    return NOERROR;
}

ECode HeapDoubleBuffer::Put(
    /* [in] */ const Array<Double>& src,
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

ECode HeapDoubleBuffer::Put(
    /* [in] */ IDoubleBuffer* src)
{
    if (src == (IDoubleBuffer*)this) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mIsReadOnly) {
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    Boolean direct;
    if (Object::InstanceOf(src, CID_HeapDoubleBuffer)) {
        HeapDoubleBuffer* hdb = (HeapDoubleBuffer*)src;
        Integer n, remaining;
        hdb->Remaining(&n);
        if (Remaining(&remaining), n > remaining) {
            return E_BUFFER_OVERFLOW_EXCEPTION;
        }
        Integer thisPos, hsbPos;
        GetPosition(&thisPos);
        hdb->GetPosition(&hsbPos);
        mHb.Copy(Ix(thisPos), hdb->mHb, hdb->Ix(hsbPos), n);
        hdb->SetPosition(hsbPos + n);
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
        return DoubleBuffer::Put(src);
    }
}

ECode HeapDoubleBuffer::Compact()
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

ECode HeapDoubleBuffer::GetOrder(
    /* [out] */ IByteOrder** bo)
{
    VALIDATE_NOT_NULL(bo);

    AutoPtr<IByteOrder> order = ByteOrder::Order();
    order.MoveTo(bo);
    return NOERROR;
}

ECode HeapDoubleBuffer::GetCoclassID(
    /* [out] */ CoclassID* cid)
{
    VALIDATE_NOT_NULL(cid);

    *cid = CID_HeapDoubleBuffer;
    return NOERROR;
}

}
}
