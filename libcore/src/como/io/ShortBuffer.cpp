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
#include "como/io/HeapShortBuffer.h"
#include "como/io/ShortBuffer.h"

using como::core::CArrayHolder;
using como::core::CStringBuffer;
using como::core::IID_IArrayHolder;
using como::core::IID_IComparable;
using como::core::IID_IStringBuffer;
using como::core::IStringBuffer;
using como::core::Math;

namespace como {
namespace io {

COMO_INTERFACE_IMPL_2(ShortBuffer, Buffer, IShortBuffer, IComparable)

ECode ShortBuffer::Constructor(
    /* [in] */ Integer mark,
    /* [in] */ Integer pos,
    /* [in] */ Integer lim,
    /* [in] */ Integer cap,
    /* [in] */ const Array<Short>& hb,
    /* [in] */ Integer offset)
{
    FAIL_RETURN(Buffer::Constructor(mark, pos, lim, cap, 1));
    mHb = hb;
    mOffset = offset;
    return NOERROR;
}

ECode ShortBuffer::Constructor(
    /* [in] */ Integer mark,
    /* [in] */ Integer pos,
    /* [in] */ Integer lim,
    /* [in] */ Integer cap)
{
    Array<Short> nullHb;
    return Constructor(mark, pos, lim, cap, nullHb, 0);
}

ECode ShortBuffer::Allocate(
    /* [in] */ Integer capacity,
    /* [out] */ IShortBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer);

    if (capacity < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<HeapShortBuffer> hsb = new HeapShortBuffer();
    FAIL_RETURN(hsb->Constructor(capacity, capacity));
    *buffer = hsb;
    REFCOUNT_ADD(*buffer);
    return NOERROR;
}

ECode ShortBuffer::Wrap(
    /* [in] */ const Array<Short>& array,
    /* [in] */ Integer offset,
    /* [in] */ Integer length,
    /* [out] */ IShortBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer);

    AutoPtr<HeapShortBuffer> hsb = new HeapShortBuffer();
    FAIL_RETURN(hsb->Constructor(array, offset, length));
    *buffer = hsb;
    REFCOUNT_ADD(*buffer);
    return NOERROR;
}

ECode ShortBuffer::Wrap(
    /* [in] */ const Array<Short>& array,
    /* [out] */ IShortBuffer** buffer)
{
    return Wrap(array, 0, array.GetLength(), buffer);
}

ECode ShortBuffer::Get(
    /* [out] */ Array<Short>& dst,
    /* [in] */ Integer offset,
    /* [in] */ Integer length)
{
    FAIL_RETURN(CheckBounds(offset, length, dst.GetLength()));
    Integer remaining;
    Remaining(remaining);
    if (length > remaining) {
        return E_BUFFER_UNDERFLOW_EXCEPTION;
    }
    Integer end = offset + length;
    for (Integer i = offset; i < end; i++) {
        Get(dst[i]);
    }
    return NOERROR;
}

ECode ShortBuffer::Get(
    /* [out] */ Array<Short>& dst)
{
    return Get(dst, 0, dst.GetLength());
}

ECode ShortBuffer::Put(
    /* [in] */ IShortBuffer* src)
{
    if (src == (IShortBuffer*)this) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    ShortBuffer* srcObject = (ShortBuffer*)src;

    Integer n, remaining;
    srcObject->Remaining(n);
    if (Remaining(remaining), n > remaining) {
        return E_BUFFER_OVERFLOW_EXCEPTION;
    }
    for (Integer i = 0; i < n; i++) {
        Short s;
        srcObject->Get(s);
        Put(s);
    }
    return NOERROR;
}

ECode ShortBuffer::Put(
    /* [in] */ const Array<Short>& src,
    /* [in] */ Integer offset,
    /* [in] */ Integer length)
{
    FAIL_RETURN(CheckBounds(offset, length, src.GetLength()));
    Integer remaining;
    if (Remaining(remaining), length > remaining) {
        return E_BUFFER_OVERFLOW_EXCEPTION;
    }
    Integer end = offset + length;
    for (Integer i = offset; i < end; i++) {
        Put(src[i]);
    }
    return NOERROR;
}

ECode ShortBuffer::Put(
    /* [in] */ const Array<Short>& src)
{
    return Put(src, 0, src.GetLength());
}

ECode ShortBuffer::HasArray(
    /* [out] */ Boolean& result)
{
    result = (!mHb.IsNull() && !mIsReadOnly);
    return NOERROR;
}

ECode ShortBuffer::GetArray(
    /* [out] */ AutoPtr<IArrayHolder>& array)
{
    if (mHb.IsNull()) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    if (mIsReadOnly) {
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    array = nullptr;
    return CArrayHolder::New(mHb, IID_IArrayHolder, (IInterface**)&array);
}

ECode ShortBuffer::GetArrayOffset(
    /* [out] */ Integer& offset)
{
    if (mHb.IsNull()) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    if (mIsReadOnly) {
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    offset = mOffset;
    return NOERROR;
}

ECode ShortBuffer::ToString(
    /* [out] */ String& desc)
{
    AutoPtr<IStringBuffer> sb;
    CStringBuffer::New(IID_IStringBuffer, (IInterface**)&sb);
    sb->Append(Object::GetCoclassName(this));
    sb->Append(String("[pos="));
    Integer value;
    GetPosition(value);
    sb->Append(value);
    sb->Append(String(" lim="));
    GetLimit(value);
    sb->Append(value);
    sb->Append(String(" cap="));
    GetCapacity(value);
    sb->Append(value);
    sb->Append(String("]"));
    return sb->ToString(desc);
}

ECode ShortBuffer::GetHashCode(
    /* [out] */ Integer& hash)
{
    hash = 1;
    Integer p;
    GetPosition(p);
    Integer i;
    GetLimit(i);
    for (i = i - 1; i >= p; i--) {
        Short s;
        Get(i, s);
        hash = 31 * hash + s;
    }
    return NOERROR;
}

ECode ShortBuffer::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean& same)
{
    ShortBuffer* other = (ShortBuffer*)IShortBuffer::Probe(obj);
    if (other == nullptr) {
        same = false;
        return NOERROR;
    }
    if (this == other) {
        same = true;
        return NOERROR;
    }
    Integer thisRemaining, otherRemaining;
    Remaining(thisRemaining);
    other->Remaining(otherRemaining);
    if (thisRemaining != otherRemaining) {
        same = false;
        return NOERROR;
    }
    Integer p;
    GetPosition(p);
    Integer i, j;
    GetLimit(i);
    other->GetLimit(j);
    for (i = i - 1, j = j - 1; i >= p; i--, j--) {
        Short thiss, others;
        Get(i, thiss);
        other->Get(j, others);
        if (thiss != others) {
            same = false;
            return NOERROR;
        }
    }
    same = true;
    return NOERROR;
}

ECode ShortBuffer::CompareTo(
    /* [in] */ IInterface* other,
    /* [out] */ Integer& result)
{
    ShortBuffer* otherSB = (ShortBuffer*)IShortBuffer::Probe(other);
    if (otherSB == nullptr) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer thisRemaining, otherRemaining;
    Remaining(thisRemaining);
    otherSB->Remaining(otherRemaining);
    Integer thisPos, otherPos;
    GetPosition(thisPos);
    otherSB->GetPosition(otherPos);

    Integer n = thisPos + Math::Min(thisRemaining, otherRemaining);
    for (Integer i = thisPos, j = otherPos; i < n; i++, j++) {
        Short thiss, others;
        Get(i, thiss);
        otherSB->Get(j, others);
        Integer cmp = thiss - others;
        if (cmp != 0) {
            result = cmp;
            return NOERROR;
        }
    }
    result = thisRemaining - otherRemaining;
    return NOERROR;
}


}
}