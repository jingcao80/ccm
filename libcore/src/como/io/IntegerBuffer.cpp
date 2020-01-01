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
#include "como/io/HeapIntegerBuffer.h"
#include "como/io/IntegerBuffer.h"

using como::core::CArrayHolder;
using como::core::CStringBuffer;
using como::core::Math;
using como::core::IID_IArrayHolder;
using como::core::IID_IComparable;
using como::core::IID_IStringBuffer;
using como::core::IStringBuffer;

namespace como {
namespace io {

COMO_INTERFACE_IMPL_2(IntegerBuffer, Buffer, IIntegerBuffer, IComparable);

ECode IntegerBuffer::Constructor(
    /* [in] */ Integer mark,
    /* [in] */ Integer pos,
    /* [in] */ Integer lim,
    /* [in] */ Integer cap,
    /* [in] */ const Array<Integer>& hb,
    /* [in] */ Integer offset)
{
    FAIL_RETURN(Buffer::Constructor(mark, pos, lim, cap, 2));
    mHb = hb;
    mOffset = offset;
    return NOERROR;
}

ECode IntegerBuffer::Constructor(
    /* [in] */ Integer mark,
    /* [in] */ Integer pos,
    /* [in] */ Integer lim,
    /* [in] */ Integer cap)
{
    Array<Integer> nullHb;
    return Constructor(mark, pos, lim, cap, nullHb, 0);
}

ECode IntegerBuffer::Allocate(
    /* [in] */ Integer capacity,
    /* [out] */ IIntegerBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer);

    if (capacity < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<HeapIntegerBuffer> hib = new HeapIntegerBuffer();
    FAIL_RETURN(hib->Constructor(capacity, capacity));
    *buffer = hib;
    REFCOUNT_ADD(*buffer);
    return NOERROR;
}

ECode IntegerBuffer::Wrap(
    /* [in] */ const Array<Integer>& array,
    /* [in] */ Integer offset,
    /* [in] */ Integer length,
    /* [out] */ IIntegerBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer);

    AutoPtr<HeapIntegerBuffer> hib = new HeapIntegerBuffer();
    FAIL_RETURN(hib->Constructor(array, offset, length));
    *buffer = hib;
    REFCOUNT_ADD(*buffer);
    return NOERROR;
}

ECode IntegerBuffer::Wrap(
    /* [in] */ const Array<Integer>& array,
    /* [out] */ IIntegerBuffer** buffer)
{
    return Wrap(array, 0, array.GetLength(), buffer);
}

ECode IntegerBuffer::Get(
    /* [out] */ Array<Integer>& dst,
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

ECode IntegerBuffer::Get(
    /* [out] */ Array<Integer>& dst)
{
    return Get(dst, 0, dst.GetLength());
}

ECode IntegerBuffer::Put(
    /* [in] */ IIntegerBuffer* src)
{
    if (src == (IIntegerBuffer*)this) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    IntegerBuffer* srcObject = (IntegerBuffer*)src;

    Integer n, remaining;
    srcObject->Remaining(&n);
    if (Remaining(&remaining), n > remaining) {
        return E_BUFFER_OVERFLOW_EXCEPTION;
    }
    for (Integer i = 0; i < n; i++) {
        Integer iv;
        srcObject->Get(&iv);
        Put(iv);
    }
    return NOERROR;
}

ECode IntegerBuffer::Put(
    /* [in] */ const Array<Integer>& src,
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

ECode IntegerBuffer::Put(
    /* [in] */ const Array<Integer>& src)
{
    return Put(src, 0, src.GetLength());
}

ECode IntegerBuffer::HasArray(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = (!mHb.IsNull() && !mIsReadOnly);
    return NOERROR;
}

ECode IntegerBuffer::GetArray(
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

ECode IntegerBuffer::GetArrayOffset(
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

ECode IntegerBuffer::ToString(
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

ECode IntegerBuffer::GetHashCode(
    /* [out] */ Integer* hash)
{
    VALIDATE_NOT_NULL(hash);

    Integer h = 1;
    Integer p;
    GetPosition(&p);
    Integer i;
    GetLimit(&i);
    for (i = i - 1; i >= p; i--) {
        Integer iv;
        Get(i, &iv);
        h = 31 * h + iv;
    }
    *hash = h;
    return NOERROR;
}

ECode IntegerBuffer::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* same)
{
    VALIDATE_NOT_NULL(same);

    IntegerBuffer* other = (IntegerBuffer*)IIntegerBuffer::Probe(obj);
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
        Integer thisiv, otheriv;
        Get(i, &thisiv);
        other->Get(j, &otheriv);
        if (thisiv != otheriv) {
            *same = false;
            return NOERROR;
        }
    }
    *same = true;
    return NOERROR;
}

ECode IntegerBuffer::CompareTo(
    /* [in] */ IInterface* other,
    /* [out] */ Integer* result)
{
    VALIDATE_NOT_NULL(result);

    IntegerBuffer* otherIB = (IntegerBuffer*)IIntegerBuffer::Probe(other);
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
        Integer thisiv, otheriv;
        Get(i, &thisiv);
        otherIB->Get(j, &otheriv);
        Integer cmp = thisiv - otheriv;
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
