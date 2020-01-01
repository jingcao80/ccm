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
#include "como/io/FloatBuffer.h"
#include "como/io/HeapFloatBuffer.h"

using como::core::CArrayHolder;
using como::core::CStringBuffer;
using como::core::IID_IArrayHolder;
using como::core::IID_IComparable;
using como::core::IID_IStringBuffer;
using como::core::IStringBuffer;
using como::core::Math;

namespace como {
namespace io {

COMO_INTERFACE_IMPL_2(FloatBuffer, Buffer, IFloatBuffer, IComparable);

ECode FloatBuffer::Constructor(
    /* [in] */ Integer mark,
    /* [in] */ Integer pos,
    /* [in] */ Integer lim,
    /* [in] */ Integer cap,
    /* [in] */ const Array<Float>& hb,
    /* [in] */ Integer offset)
{
    FAIL_RETURN(Buffer::Constructor(mark, pos, lim, cap, 2));
    mHb = hb;
    mOffset = offset;
    return NOERROR;
}

ECode FloatBuffer::Constructor(
    /* [in] */ Integer mark,
    /* [in] */ Integer pos,
    /* [in] */ Integer lim,
    /* [in] */ Integer cap)
{
    Array<Float> nullHb;
    return Constructor(mark, pos, lim, cap, nullHb, 0);
}

ECode FloatBuffer::Allocate(
    /* [in] */ Integer capacity,
    /* [out] */ IFloatBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer);

    if (capacity < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<HeapFloatBuffer> hfb = new HeapFloatBuffer();
    FAIL_RETURN(hfb->Constructor(capacity, capacity));
    *buffer = hfb;
    REFCOUNT_ADD(*buffer);
    return NOERROR;
}

ECode FloatBuffer::Wrap(
    /* [in] */ const Array<Float>& array,
    /* [in] */ Integer offset,
    /* [in] */ Integer length,
    /* [out] */ IFloatBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer);

    AutoPtr<HeapFloatBuffer> hfb = new HeapFloatBuffer();
    FAIL_RETURN(hfb->Constructor(array, offset, length));
    *buffer = hfb;
    REFCOUNT_ADD(*buffer);
    return NOERROR;
}

ECode FloatBuffer::Wrap(
    /* [in] */ const Array<Float>& array,
    /* [out] */ IFloatBuffer** buffer)
{
    return Wrap(array, 0, array.GetLength(), buffer);
}

ECode FloatBuffer::Get(
    /* [out] */ Array<Float>& dst,
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

ECode FloatBuffer::Get(
    /* [out] */ Array<Float>& dst)
{
    return Get(dst, 0, dst.GetLength());
}

ECode FloatBuffer::Put(
    /* [in] */ IFloatBuffer* src)
{
    if (src == (IFloatBuffer*)this) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    FloatBuffer* srcObject = (FloatBuffer*)src;

    Integer n, remaining;
    srcObject->Remaining(&n);
    if (Remaining(&remaining), n > remaining) {
        return E_BUFFER_OVERFLOW_EXCEPTION;
    }
    for (Integer i = 0; i < n; i++) {
        Float f;
        srcObject->Get(&f);
        Put(f);
    }
    return NOERROR;
}

ECode FloatBuffer::Put(
    /* [in] */ const Array<Float>& src,
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

ECode FloatBuffer::Put(
    /* [in] */ const Array<Float>& src)
{
    return Put(src, 0, src.GetLength());
}

ECode FloatBuffer::HasArray(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = (!mHb.IsNull() && !mIsReadOnly);
    return NOERROR;
}

ECode FloatBuffer::GetArray(
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

ECode FloatBuffer::GetArrayOffset(
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

ECode FloatBuffer::ToString(
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

ECode FloatBuffer::GetHashCode(
    /* [out] */ Integer* hash)
{
    VALIDATE_NOT_NULL(hash);

    Integer h = 1;
    Integer p;
    GetPosition(&p);
    Integer i;
    GetLimit(&i);
    for (i = i - 1; i >= p; i--) {
        Float f;
        Get(i, &f);
        h = 31 * h + (Integer)f;
    }
    *hash = h;
    return NOERROR;
}

ECode FloatBuffer::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* same)
{
    VALIDATE_NOT_NULL(same);

    FloatBuffer* other = (FloatBuffer*)IFloatBuffer::Probe(obj);
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
        Float thisf, otherf;
        Get(i, &thisf);
        other->Get(j, &otherf);
        if (!Equals(thisf, otherf)) {
            *same = false;
            return NOERROR;
        }
    }
    *same = true;
    return NOERROR;
}

Boolean FloatBuffer::Equals(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    return (x == y) || (Math::IsNaN(x) && Math::IsNaN(y));
}

ECode FloatBuffer::CompareTo(
    /* [in] */ IInterface* other,
    /* [out] */ Integer* result)
{
    VALIDATE_NOT_NULL(result);

    FloatBuffer* otherIB = (FloatBuffer*)IFloatBuffer::Probe(other);
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
        Float thisf, otherf;
        Get(i, &thisf);
        otherIB->Get(j, &otherf);
        Integer cmp = Compare(thisf, otherf);
        if (cmp != 0) {
            *result = cmp;
            return NOERROR;
        }
    }
    *result = thisRemaining - otherRemaining;
    return NOERROR;
}

Integer FloatBuffer::Compare(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    return ((x < y) ? -1 : (x > y) ? +1 : (x == y) ? 0 :
            Math::IsNaN(x) ? (Math::IsNaN(y) ? 0 : +1) : -1);
}

}
}
