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
#include "como/io/HeapShortBuffer.h"

namespace como {
namespace io {

extern const CoclassID CID_HeapShortBuffer =
        {{0x8a24eb0b,0xef94,0x4356,0x83a3,{0x3e,0x0c,0xbd,0x8f,0x5e,0x50}}, &CID_libcore};

ECode HeapShortBuffer::Constructor(
    /* [in] */ Integer cap,
    /* [in] */ Integer lim)
{
    return Constructor(cap, lim, false);
}

ECode HeapShortBuffer::Constructor(
    /* [in] */ Integer cap,
    /* [in] */ Integer lim,
    /* [in] */ Boolean isReadOnly)
{
    Array<Short> hb(cap);
    FAIL_RETURN(ShortBuffer::Constructor(-1, 0, lim, cap, hb, 0));
    mIsReadOnly = isReadOnly;
    return NOERROR;
}

ECode HeapShortBuffer::Constructor(
    /* [in] */ const Array<Short>& buf,
    /* [in] */ Integer off,
    /* [in] */ Integer len)
{
    return Constructor(buf, off, len, false);
}

ECode HeapShortBuffer::Constructor(
    /* [in] */ const Array<Short>& buf,
    /* [in] */ Integer off,
    /* [in] */ Integer len,
    /* [in] */ Boolean isReadOnly)
{
    FAIL_RETURN(ShortBuffer::Constructor(-1, off, off + len, buf.GetLength(), buf, 0));
    mIsReadOnly = isReadOnly;
    return NOERROR;
}

ECode HeapShortBuffer::Constructor(
    /* [in] */ const Array<Short>& buf,
    /* [in] */ Integer mark,
    /* [in] */ Integer pos,
    /* [in] */ Integer lim,
    /* [in] */ Integer cap,
    /* [in] */ Integer off)
{
    return Constructor(buf, mark, pos, lim, cap, off, false);
}

ECode HeapShortBuffer::Constructor(
    /* [in] */ const Array<Short>& buf,
    /* [in] */ Integer mark,
    /* [in] */ Integer pos,
    /* [in] */ Integer lim,
    /* [in] */ Integer cap,
    /* [in] */ Integer off,
    /* [in] */ Boolean isReadOnly)
{
    FAIL_RETURN(ShortBuffer::Constructor(mark, pos, lim, cap, buf, off));
    mIsReadOnly = isReadOnly;
    return NOERROR;
}

ECode HeapShortBuffer::Slice(
    /* [out] */ AutoPtr<IShortBuffer>& buffer)
{
    Integer remaining, pos;
    Remaining(remaining);
    GetPosition(pos);
    AutoPtr<HeapShortBuffer> hsb = new HeapShortBuffer();
    FAIL_RETURN(hsb->Constructor(
            mHb, -1, 0, remaining, remaining, pos + mOffset, mIsReadOnly));
    buffer = (IShortBuffer*)hsb.Get();
    return NOERROR;
}

ECode HeapShortBuffer::Duplicate(
    /* [out] */ AutoPtr<IShortBuffer>& buffer)
{
    Integer pos, lim, cap;
    GetPosition(pos);
    GetLimit(lim);
    GetCapacity(cap);
    AutoPtr<HeapShortBuffer> hsb = new HeapShortBuffer();
    FAIL_RETURN(hsb->Constructor(
            mHb, MarkValue(), pos, lim, cap, mOffset, mIsReadOnly));
    buffer = (IShortBuffer*)hsb.Get();
    return NOERROR;
}

ECode HeapShortBuffer::AsReadOnlyBuffer(
    /* [out] */ AutoPtr<IShortBuffer>& buffer)
{
    Integer pos, lim, cap;
    GetPosition(pos);
    GetLimit(lim);
    GetCapacity(cap);
    AutoPtr<HeapShortBuffer> hsb = new HeapShortBuffer();
    FAIL_RETURN(hsb->Constructor(
            mHb, MarkValue(), pos, lim, cap, mOffset, true));
    buffer = (IShortBuffer*)hsb.Get();
    return NOERROR;
}

ECode HeapShortBuffer::Get(
    /* [out] */ Short& s)
{
    Integer index;
    NextGetIndex(&index);
    s = mHb[Ix(index)];
    return NOERROR;
}

ECode HeapShortBuffer::Get(
    /* [in] */ Integer index,
    /* [out] */ Short& s)
{
    FAIL_RETURN(CheckIndex(index));
    s = mHb[Ix(index)];
    return NOERROR;
}

ECode HeapShortBuffer::Get(
    /* [out] */ Array<Short>& dst,
    /* [in] */ Integer offset,
    /* [in] */ Integer length)
{
    FAIL_RETURN(CheckBounds(mOffset, length, dst.GetLength()));
    Integer remaining;
    Remaining(remaining);
    if (length > remaining){
        return E_BUFFER_UNDERFLOW_EXCEPTION;
    }
    Integer pos;
    GetPosition(pos);
    dst.Copy(offset, mHb, Ix(pos), length);
    SetPosition(pos + length);
    return NOERROR;
}

ECode HeapShortBuffer::IsDirect(
    /* [out] */ Boolean& direct)
{
    direct = false;
    return NOERROR;
}

ECode HeapShortBuffer::IsReadOnly(
    /* [out] */ Boolean& readOnly)
{
    readOnly = mIsReadOnly;
    return NOERROR;
}

ECode HeapShortBuffer::Put(
    /* [in] */ Short s)
{
    if (mIsReadOnly) {
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    Integer index;
    NextPutIndex(&index);
    mHb[Ix(index)] = s;
    return NOERROR;
}

ECode HeapShortBuffer::Put(
    /* [in] */ Integer index,
    /* [in] */ Short s)
{
    if (mIsReadOnly) {
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    FAIL_RETURN(CheckIndex(index));
    mHb[Ix(index)] = s;
    return NOERROR;
}

ECode HeapShortBuffer::Put(
    /* [in] */ const Array<Short>& src,
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
    mHb.Copy(Ix(pos), src, offset, length);
    SetPosition(pos + length);
    return NOERROR;
}

ECode HeapShortBuffer::Put(
    /* [in] */ IShortBuffer* src)
{
    if (src == (IShortBuffer*)this) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mIsReadOnly) {
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    Boolean direct;
    if (Object::InstanceOf(src, CID_HeapShortBuffer)) {
        HeapShortBuffer* hsb = (HeapShortBuffer*)src;
        Integer n, remaining;
        hsb->Remaining(n);
        if (Remaining(remaining), n > remaining) {
            return E_BUFFER_OVERFLOW_EXCEPTION;
        }
        Integer thisPos, hsbPos;
        GetPosition(thisPos);
        hsb->GetPosition(hsbPos);
        mHb.Copy(Ix(thisPos), hsb->mHb, hsb->Ix(hsbPos), n);
        hsb->SetPosition(hsbPos + n);
        SetPosition(thisPos + n);
        return NOERROR;
    }
    else if (IBuffer::Probe(src)->IsDirect(direct), direct) {
        Integer n, remaining;
        IBuffer::Probe(src)->Remaining(n);
        if (Remaining(remaining), n > remaining) {
            return E_BUFFER_OVERFLOW_EXCEPTION;
        }
        Integer pos;
        GetPosition(pos);
        src->Get(mHb, Ix(pos), n);
        SetPosition(pos + n);
        return NOERROR;
    }
    else {
        return ShortBuffer::Put(src);
    }
}

ECode HeapShortBuffer::Compact()
{
    if (mIsReadOnly) {
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    Integer pos, remaining, cap;
    GetPosition(pos);
    Remaining(remaining);
    mHb.Copy(Ix(0), mHb, Ix(pos), remaining);
    SetPosition(remaining);
    GetCapacity(cap);
    SetLimit(cap);
    DiscardMark();
    return NOERROR;
}

ECode HeapShortBuffer::GetOrder(
    /* [out] */ AutoPtr<IByteOrder>& bo)
{
    bo = ByteOrder::Order();
    return NOERROR;
}

ECode HeapShortBuffer::GetCoclassID(
    /* [out] */ CoclassID& cid)
{
    cid = CID_HeapShortBuffer;
    return NOERROR;
}

}
}
