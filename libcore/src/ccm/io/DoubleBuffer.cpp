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
#include "ccm/io/DoubleBuffer.h"
#include "ccm/io/HeapDoubleBuffer.h"

using ccm::core::CArrayHolder;
using ccm::core::CStringBuffer;
using ccm::core::IID_IArrayHolder;
using ccm::core::IID_IComparable;
using ccm::core::IID_IStringBuffer;
using ccm::core::IStringBuffer;
using ccm::core::Math;

namespace ccm {
namespace io {

CCM_INTERFACE_IMPL_2(DoubleBuffer, Buffer, IDoubleBuffer, IComparable);

ECode DoubleBuffer::Constructor(
    /* [in] */ Integer mark,
    /* [in] */ Integer pos,
    /* [in] */ Integer lim,
    /* [in] */ Integer cap,
    /* [in] */ const Array<Double>& hb,
    /* [in] */ Integer offset)
{
    FAIL_RETURN(Buffer::Constructor(mark, pos, lim, cap, 3));
    mHb = hb;
    mOffset = offset;
    return NOERROR;
}

ECode DoubleBuffer::Constructor(
    /* [in] */ Integer mark,
    /* [in] */ Integer pos,
    /* [in] */ Integer lim,
    /* [in] */ Integer cap)
{
    Array<Double> nullHb;
    return Constructor(mark, pos, lim, cap, nullHb, 0);
}

ECode DoubleBuffer::Allocate(
    /* [in] */ Integer capacity,
    /* [out] */ IDoubleBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer);

    if (capacity < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<HeapDoubleBuffer> hdb = new HeapDoubleBuffer();
    FAIL_RETURN(hdb->Constructor(capacity, capacity));
    *buffer = hdb;
    REFCOUNT_ADD(*buffer);
    return NOERROR;
}

ECode DoubleBuffer::Wrap(
    /* [in] */ const Array<Double>& array,
    /* [in] */ Integer offset,
    /* [in] */ Integer length,
    /* [out] */ IDoubleBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer);

    AutoPtr<HeapDoubleBuffer> hdb = new HeapDoubleBuffer();
    FAIL_RETURN(hdb->Constructor(array, offset, length));
    *buffer = hdb;
    REFCOUNT_ADD(*buffer);
    return NOERROR;
}

ECode DoubleBuffer::Wrap(
    /* [in] */ const Array<Double>& array,
    /* [out] */ IDoubleBuffer** buffer)
{
    return Wrap(array, 0, array.GetLength(), buffer);
}

ECode DoubleBuffer::Get(
    /* [out] */ Array<Double>& dst,
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

ECode DoubleBuffer::Get(
    /* [out] */ Array<Double>& dst)
{
    return Get(dst, 0, dst.GetLength());
}

ECode DoubleBuffer::Put(
    /* [in] */ IDoubleBuffer* src)
{
    if (src == (IDoubleBuffer*)this) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    DoubleBuffer* srcObject = (DoubleBuffer*)src;

    Integer n, remaining;
    srcObject->Remaining(&n);
    if (Remaining(&remaining), n > remaining) {
        return E_BUFFER_OVERFLOW_EXCEPTION;
    }
    for (Integer i = 0; i < n; i++) {
        Double d;
        srcObject->Get(&d);
        Put(d);
    }
    return NOERROR;
}

ECode DoubleBuffer::Put(
    /* [in] */ const Array<Double>& src,
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

ECode DoubleBuffer::Put(
    /* [in] */ const Array<Double>& src)
{
    return Put(src, 0, src.GetLength());
}

ECode DoubleBuffer::HasArray(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = (!mHb.IsNull() && !mIsReadOnly);
    return NOERROR;
}

ECode DoubleBuffer::GetArray(
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

ECode DoubleBuffer::GetArrayOffset(
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

ECode DoubleBuffer::ToString(
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

ECode DoubleBuffer::GetHashCode(
    /* [out] */ Integer* hash)
{
    VALIDATE_NOT_NULL(hash);

    Integer h = 1;
    Integer p;
    GetPosition(&p);
    Integer i;
    GetLimit(&i);
    for (i = i - 1; i >= p; i--) {
        Double d;
        Get(i, &d);
        h = 31 * h + (Integer)d;
    }
    *hash = h;
    return NOERROR;
}

ECode DoubleBuffer::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* same)
{
    VALIDATE_NOT_NULL(same);

    DoubleBuffer* other = (DoubleBuffer*)IDoubleBuffer::Probe(obj);
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
        Double thisd, otherd;
        Get(i, &thisd);
        other->Get(j, &otherd);
        if (!Equals(thisd, otherd)) {
            *same = false;
            return NOERROR;
        }
    }
    *same = true;
    return NOERROR;
}

Boolean DoubleBuffer::Equals(
    /* [in] */ Double x,
    /* [in] */ Double y)
{
    return (x == y) || (Math::IsNaN(x) && Math::IsNaN(y));
}

ECode DoubleBuffer::CompareTo(
    /* [in] */ IInterface* other,
    /* [out] */ Integer* result)
{
    VALIDATE_NOT_NULL(result);

    DoubleBuffer* otherIB = (DoubleBuffer*)IDoubleBuffer::Probe(other);
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
        Double thisd, otherd;
        Get(i, &thisd);
        otherIB->Get(j, &otherd);
        Integer cmp = Math::Compare(thisd, otherd);
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
