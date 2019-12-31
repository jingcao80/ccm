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

#include "ccm/core/CArrayHolder.h"
#include "ccm/core/CStringBuffer.h"
#include "ccm/core/Math.h"
#include "ccm/io/HeapLongBuffer.h"
#include "ccm/io/LongBuffer.h"

using ccm::core::CArrayHolder;
using ccm::core::CStringBuffer;
using ccm::core::IID_IArrayHolder;
using ccm::core::IID_IComparable;
using ccm::core::IID_IStringBuffer;
using ccm::core::IStringBuffer;
using ccm::core::Math;

namespace ccm {
namespace io {

CCM_INTERFACE_IMPL_2(LongBuffer, Buffer, ILongBuffer, IComparable);

ECode LongBuffer::Constructor(
    /* [in] */ Integer mark,
    /* [in] */ Integer pos,
    /* [in] */ Integer lim,
    /* [in] */ Integer cap,
    /* [in] */ const Array<Long>& hb,
    /* [in] */ Integer offset)
{
    FAIL_RETURN(Buffer::Constructor(mark, pos, lim, cap, 3));
    mHb = hb;
    mOffset = offset;
    return NOERROR;
}

ECode LongBuffer::Constructor(
    /* [in] */ Integer mark,
    /* [in] */ Integer pos,
    /* [in] */ Integer lim,
    /* [in] */ Integer cap)
{
    Array<Long> nullHb;
    return Constructor(mark, pos, lim, cap, nullHb, 0);
}

ECode LongBuffer::Allocate(
    /* [in] */ Integer capacity,
    /* [out] */ ILongBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer);

    if (capacity < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<HeapLongBuffer> hlb = new HeapLongBuffer();
    FAIL_RETURN(hlb->Constructor(capacity, capacity));
    *buffer = hlb;
    REFCOUNT_ADD(*buffer);
    return NOERROR;
}

ECode LongBuffer::Wrap(
    /* [in] */ const Array<Long>& array,
    /* [in] */ Integer offset,
    /* [in] */ Integer length,
    /* [out] */ ILongBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer);

    AutoPtr<HeapLongBuffer> hlb = new HeapLongBuffer();
    FAIL_RETURN(hlb->Constructor(array, offset, length));
    *buffer = hlb;
    REFCOUNT_ADD(*buffer);
    return NOERROR;
}

ECode LongBuffer::Wrap(
    /* [in] */ const Array<Long>& array,
    /* [out] */ ILongBuffer** buffer)
{
    return Wrap(array, 0, array.GetLength(), buffer);
}

ECode LongBuffer::Get(
    /* [out] */ Array<Long>& dst,
    /* [in] */ Integer offset,
    /* [in] */ Integer length)
{
    FAIL_RETURN(CheckBounds(offset, length, dst.GetLength()));
    Integer remaining;
    Remaining(&remaining);
    if (length > remaining) {
        return E_BUFFER_UNDERFLOW_EXCEPTION;
    }
    Integer end = offset + length;
    for (Integer i = offset; i < end; i++) {
        Get(&dst[i]);
    }
    return NOERROR;
}

ECode LongBuffer::Get(
    /* [out] */ Array<Long>& dst)
{
    return Get(dst, 0, dst.GetLength());
}

ECode LongBuffer::Put(
    /* [in] */ ILongBuffer* src)
{
    if (src == (ILongBuffer*)this) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    LongBuffer* srcObject = (LongBuffer*)src;

    Integer n, remaining;
    srcObject->Remaining(&n);
    if (Remaining(&remaining), n > remaining) {
        return E_BUFFER_OVERFLOW_EXCEPTION;
    }
    for (Integer i = 0; i < n; i++) {
        Long l;
        srcObject->Get(&l);
        Put(l);
    }
    return NOERROR;
}

ECode LongBuffer::Put(
    /* [in] */ const Array<Long>& src,
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

ECode LongBuffer::Put(
    /* [in] */ const Array<Long>& src)
{
    return Put(src, 0, src.GetLength());
}

ECode LongBuffer::HasArray(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = (!mHb.IsNull() && !mIsReadOnly);
    return NOERROR;
}

ECode LongBuffer::GetArray(
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

ECode LongBuffer::GetArrayOffset(
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

ECode LongBuffer::ToString(
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

ECode LongBuffer::GetHashCode(
    /* [out] */ Integer* hash)
{
    VALIDATE_NOT_NULL(hash);

    Integer h = 1;
    Integer p;
    GetPosition(&p);
    Integer i;
    GetLimit(&i);
    for (i = i - 1; i >= p; i--) {
        Long l;
        Get(i, &l);
        h = 31 * h + l;
    }
    *hash = h;
    return NOERROR;
}

ECode LongBuffer::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* same)
{
    VALIDATE_NOT_NULL(same);

    LongBuffer* other = (LongBuffer*)ILongBuffer::Probe(obj);
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
        Long thisl, otherl;
        Get(i, &thisl);
        other->Get(j, &otherl);
        if (thisl != otherl) {
            *same = false;
            return NOERROR;
        }
    }
    *same = true;
    return NOERROR;
}

ECode LongBuffer::CompareTo(
    /* [in] */ IInterface* other,
    /* [out] */ Integer* result)
{
    VALIDATE_NOT_NULL(result);

    LongBuffer* otherIB = (LongBuffer*)ILongBuffer::Probe(other);
    if (otherIB == nullptr) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer thisRemaining, otherRemaining;
    Remaining(&thisRemaining);
    otherIB->Remaining(&otherRemaining);
    Integer thisPos, otherPos;
    GetPosition(&thisPos);
    otherIB->GetPosition(&otherPos);

    Integer n = thisPos + Math::Min(thisRemaining, otherRemaining);
    for (Integer i = thisPos, j = otherPos; i < n; i++, j++) {
        Long thisl, otherl;
        Get(i, &thisl);
        otherIB->Get(j, &otherl);
        Integer cmp = thisl - otherl;
        if (cmp != 0) {
            *result = cmp;
            return NOERROR;
        }
    }
    *result = thisRemaining - otherRemaining;
    return NOERROR;
}

}
}
