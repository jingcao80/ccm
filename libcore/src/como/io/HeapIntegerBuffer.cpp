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
#include "como/io/HeapIntegerBuffer.h"

namespace como {
namespace io {

ECode HeapIntegerBuffer::Constructor(
    /* [in] */ Integer cap,
    /* [in] */ Integer lim)
{
    return Constructor(cap, lim, false);
}

ECode HeapIntegerBuffer::Constructor(
    /* [in] */ Integer cap,
    /* [in] */ Integer lim,
    /* [in] */ Boolean isReadOnly)
{
    Array<Integer> hb(cap);
    FAIL_RETURN(IntegerBuffer::Constructor(-1, 0, lim, cap, hb, 0));
    mIsReadOnly = isReadOnly;
    return NOERROR;
}

ECode HeapIntegerBuffer::Constructor(
    /* [in] */ const Array<Integer>& buf,
    /* [in] */ Integer off,
    /* [in] */ Integer len)
{
    return Constructor(buf, off, len ,false);
}

ECode HeapIntegerBuffer::Constructor(
    /* [in] */ const Array<Integer>& buf,
    /* [in] */ Integer off,
    /* [in] */ Integer len,
    /* [in] */ Boolean isReadOnly)
{
    FAIL_RETURN(IntegerBuffer::Constructor(-1, off, off + len, buf.GetLength(), buf, 0));
    mIsReadOnly = isReadOnly;
    return NOERROR;
}

ECode HeapIntegerBuffer::Constructor(
    /* [in] */ const Array<Integer>& buf,
    /* [in] */ Integer mark,
    /* [in] */ Integer pos,
    /* [in] */ Integer lim,
    /* [in] */ Integer cap,
    /* [in] */ Integer off)
{
    return Constructor(buf, mark, pos, lim, cap, off, false);
}

ECode HeapIntegerBuffer::Constructor(
    /* [in] */ const Array<Integer>& buf,
    /* [in] */ Integer mark,
    /* [in] */ Integer pos,
    /* [in] */ Integer lim,
    /* [in] */ Integer cap,
    /* [in] */ Integer off,
    /* [in] */ Boolean isReadOnly)
{
    FAIL_RETURN(IntegerBuffer::Constructor(mark, pos, lim, cap, buf, off));
    mIsReadOnly = isReadOnly;
    return NOERROR;
}

ECode HeapIntegerBuffer::Slice(
    /* [out] */ IIntegerBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer);

    Integer remaining, pos;
    Remaining(&remaining);
    GetPosition(&pos);
    AutoPtr<HeapIntegerBuffer> hib = new HeapIntegerBuffer();
    FAIL_RETURN(hib->Constructor(
            mHb, -1, 0, remaining, remaining, pos + mOffset, mIsReadOnly));
    *buffer = (IIntegerBuffer*)hib.Get();
    REFCOUNT_ADD(*buffer);
    return NOERROR;
}

ECode HeapIntegerBuffer::Duplicate(
    /* [out] */ IIntegerBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer);

    Integer pos, lim, cap;
    GetPosition(&pos);
    GetLimit(&lim);
    GetCapacity(&cap);
    AutoPtr<HeapIntegerBuffer> hib = new HeapIntegerBuffer();
    FAIL_RETURN(hib->Constructor(
            mHb, MarkValue(), pos, lim, cap, mOffset, mIsReadOnly));
    *buffer = (IIntegerBuffer*)hib.Get();
    REFCOUNT_ADD(*buffer);
    return NOERROR;
}

ECode HeapIntegerBuffer::AsReadOnlyBuffer(
    /* [out] */ IIntegerBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer);

    Integer pos, lim, cap;
    GetPosition(&pos);
    GetLimit(&lim);
    GetCapacity(&cap);
    AutoPtr<HeapIntegerBuffer> hib = new HeapIntegerBuffer();
    FAIL_RETURN(hib->Constructor(
            mHb, MarkValue(), pos, lim, cap, mOffset, true));
    *buffer = (IIntegerBuffer*)hib.Get();
    REFCOUNT_ADD(*buffer);
    return NOERROR;
}

ECode HeapIntegerBuffer::Get(
    /* [out] */ Integer* i)
{
    VALIDATE_NOT_NULL(i);

    Integer index;
    NextGetIndex(&index);
    *i = mHb[Ix(index)];
    return NOERROR;
}

ECode HeapIntegerBuffer::Get(
    /* [in] */ Integer index,
    /* [out] */ Integer* i)
{
    VALIDATE_NOT_NULL(i);

    FAIL_RETURN(CheckIndex(index));
    *i = mHb[Ix(index)];
    return NOERROR;
}

ECode HeapIntegerBuffer::Get(
    /* [out] */ Array<Integer>& dst,
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

ECode HeapIntegerBuffer::IsDirect(
    /* [out] */ Boolean* direct)
{
    VALIDATE_NOT_NULL(direct);

    *direct = false;
    return NOERROR;
}

ECode HeapIntegerBuffer::IsReadOnly(
    /* [out] */ Boolean* readOnly)
{
    VALIDATE_NOT_NULL(readOnly);

    *readOnly = mIsReadOnly;
    return NOERROR;
}

ECode HeapIntegerBuffer::Put(
    /* [in] */ Integer i)
{
    if (mIsReadOnly) {
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    Integer index;
    NextPutIndex(&index);
    mHb[Ix(index)] = i;
    return NOERROR;
}

ECode HeapIntegerBuffer::Put(
    /* [in] */ Integer index,
    /* [in] */ Integer i)
{
    if (mIsReadOnly) {
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    FAIL_RETURN(CheckIndex(index));
    mHb[Ix(index)] = i;
    return NOERROR;
}

ECode HeapIntegerBuffer::Put(
    /* [in] */ const Array<Integer>& src,
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

ECode HeapIntegerBuffer::Compact()
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

ECode HeapIntegerBuffer::GetOrder(
    /* [out] */ IByteOrder** bo)
{
    VALIDATE_NOT_NULL(bo);

    AutoPtr<IByteOrder> order = ByteOrder::Order();
    order.MoveTo(bo);
    return NOERROR;
}

}
}
