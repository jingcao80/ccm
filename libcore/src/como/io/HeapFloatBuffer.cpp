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
#include "como/io/HeapFloatBuffer.h"

namespace como {
namespace io {

extern const CoclassID CID_HeapFloatBuffer =
        {{0x178f36d9,0x7846,0x4a62,0x80c8,{0x0a,0x20,0x7e,0x12,0x30,0x9e}}, &CID_libcore};

ECode HeapFloatBuffer::Constructor(
    /* [in] */ Integer cap,
    /* [in] */ Integer lim)
{
    return Constructor(cap, lim, false);
}

ECode HeapFloatBuffer::Constructor(
    /* [in] */ Integer cap,
    /* [in] */ Integer lim,
    /* [in] */ Boolean isReadOnly)
{
    Array<Float> hb(cap);
    FAIL_RETURN(FloatBuffer::Constructor(-1, 0, lim, cap, hb, 0));
    mIsReadOnly = isReadOnly;
    return NOERROR;
}

ECode HeapFloatBuffer::Constructor(
    /* [in] */ const Array<Float>& buf,
    /* [in] */ Integer off,
    /* [in] */ Integer len)
{
    return Constructor(buf, off, len ,false);
}

ECode HeapFloatBuffer::Constructor(
    /* [in] */ const Array<Float>& buf,
    /* [in] */ Integer off,
    /* [in] */ Integer len,
    /* [in] */ Boolean isReadOnly)
{
    FAIL_RETURN(FloatBuffer::Constructor(-1, off, off + len, buf.GetLength(), buf, 0));
    mIsReadOnly = isReadOnly;
    return NOERROR;
}

ECode HeapFloatBuffer::Constructor(
    /* [in] */ const Array<Float>& buf,
    /* [in] */ Integer mark,
    /* [in] */ Integer pos,
    /* [in] */ Integer lim,
    /* [in] */ Integer cap,
    /* [in] */ Integer off)
{
    return Constructor(buf, mark, pos, lim, cap, off, false);
}

ECode HeapFloatBuffer::Constructor(
    /* [in] */ const Array<Float>& buf,
    /* [in] */ Integer mark,
    /* [in] */ Integer pos,
    /* [in] */ Integer lim,
    /* [in] */ Integer cap,
    /* [in] */ Integer off,
    /* [in] */ Boolean isReadOnly)
{
    FAIL_RETURN(FloatBuffer::Constructor(mark, pos, lim, cap, buf, off));
    mIsReadOnly = isReadOnly;
    return NOERROR;
}

ECode HeapFloatBuffer::Slice(
    /* [out] */ IFloatBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer);

    Integer remaining, pos;
    Remaining(&remaining);
    GetPosition(&pos);
    AutoPtr<HeapFloatBuffer> hfb = new HeapFloatBuffer();
    FAIL_RETURN(hfb->Constructor(
            mHb, -1, 0, remaining, remaining, pos + mOffset, mIsReadOnly));
    *buffer = (IFloatBuffer*)hfb.Get();
    REFCOUNT_ADD(*buffer);
    return NOERROR;
}

ECode HeapFloatBuffer::Duplicate(
    /* [out] */ IFloatBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer);

    Integer pos, lim, cap;
    GetPosition(&pos);
    GetLimit(&lim);
    GetCapacity(&cap);
    AutoPtr<HeapFloatBuffer> hfb = new HeapFloatBuffer();
    FAIL_RETURN(hfb->Constructor(
            mHb, MarkValue(), pos, lim, cap, mOffset, mIsReadOnly));
    *buffer = (IFloatBuffer*)hfb.Get();
    REFCOUNT_ADD(*buffer);
    return NOERROR;
}

ECode HeapFloatBuffer::AsReadOnlyBuffer(
    /* [out] */ IFloatBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer);

    Integer pos, lim, cap;
    GetPosition(&pos);
    GetLimit(&lim);
    GetCapacity(&cap);
    AutoPtr<HeapFloatBuffer> hfb = new HeapFloatBuffer();
    FAIL_RETURN(hfb->Constructor(
            mHb, MarkValue(), pos, lim, cap, mOffset, true));
    *buffer = (IFloatBuffer*)hfb.Get();
    REFCOUNT_ADD(*buffer);
    return NOERROR;
}

ECode HeapFloatBuffer::Get(
    /* [out] */ Float* f)
{
    VALIDATE_NOT_NULL(f);

    Integer index;
    NextGetIndex(&index);
    *f = mHb[Ix(index)];
    return NOERROR;
}

ECode HeapFloatBuffer::Get(
    /* [in] */ Integer index,
    /* [out] */ Float* f)
{
    VALIDATE_NOT_NULL(f);

    FAIL_RETURN(CheckIndex(index));
    *f = mHb[Ix(index)];
    return NOERROR;
}

ECode HeapFloatBuffer::Get(
    /* [out] */ Array<Float>& dst,
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

ECode HeapFloatBuffer::IsDirect(
    /* [out] */ Boolean* direct)
{
    VALIDATE_NOT_NULL(direct);

    *direct = false;
    return NOERROR;
}

ECode HeapFloatBuffer::IsReadOnly(
    /* [out] */ Boolean* readOnly)
{
    VALIDATE_NOT_NULL(readOnly);

    *readOnly = mIsReadOnly;
    return NOERROR;
}

ECode HeapFloatBuffer::Put(
    /* [in] */ Float f)
{
    if (mIsReadOnly) {
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    Integer index;
    NextPutIndex(&index);
    mHb[Ix(index)] = f;
    return NOERROR;
}

ECode HeapFloatBuffer::Put(
    /* [in] */ Integer index,
    /* [in] */ Float f)
{
    if (mIsReadOnly) {
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    FAIL_RETURN(CheckIndex(index));
    mHb[Ix(index)] = f;
    return NOERROR;
}

ECode HeapFloatBuffer::Put(
    /* [in] */ const Array<Float>& src,
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

ECode HeapFloatBuffer::Put(
    /* [in] */ IFloatBuffer* src)
{
    if (src == (IFloatBuffer*)this) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mIsReadOnly) {
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    Boolean direct;
    if (Object::InstanceOf(src, CID_HeapFloatBuffer)) {
        HeapFloatBuffer* hfb = (HeapFloatBuffer*)src;
        Integer n, remaining;
        hfb->Remaining(&n);
        if (Remaining(&remaining), n > remaining) {
            return E_BUFFER_OVERFLOW_EXCEPTION;
        }
        Integer thisPos, hsbPos;
        GetPosition(&thisPos);
        hfb->GetPosition(&hsbPos);
        mHb.Copy(Ix(thisPos), hfb->mHb, hfb->Ix(hsbPos), n);
        hfb->SetPosition(hsbPos + n);
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
        return FloatBuffer::Put(src);
    }
}

ECode HeapFloatBuffer::Compact()
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

ECode HeapFloatBuffer::GetOrder(
    /* [out] */ IByteOrder** bo)
{
    VALIDATE_NOT_NULL(bo);

    AutoPtr<IByteOrder> order = ByteOrder::Order();
    order.MoveTo(bo);
    return NOERROR;
}

ECode HeapFloatBuffer::GetCoclassID(
    /* [out] */ CoclassID& cid)
{
    cid = CID_HeapFloatBuffer;
    return NOERROR;
}

}
}
