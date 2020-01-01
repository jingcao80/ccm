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
#include "como/core/CoreUtils.h"
#include "como/core/CString.h"
#include "como/core/Math.h"
#include "como/io/CharBuffer.h"
#include "como/io/HeapCharBuffer.h"
#include "como/io/StringCharBuffer.h"

using como::core::CArrayHolder;
using como::core::CoreUtils;
using como::core::CString;
using como::core::IID_IAppendable;
using como::core::IID_IArrayHolder;
using como::core::IID_ICharSequence;
using como::core::IID_IComparable;
using como::core::IID_IReadable;
using como::core::Math;

namespace como {
namespace io {

COMO_INTERFACE_IMPL_5(CharBuffer, Buffer, ICharBuffer, IComparable, IAppendable, ICharSequence, IReadable);

ECode CharBuffer::Constructor(
    /* [in] */ Integer mark,
    /* [in] */ Integer pos,
    /* [in] */ Integer lim,
    /* [in] */ Integer cap,
    /* [in] */ const Array<Char>& hb,
    /* [in] */ Integer offset)
{
    FAIL_RETURN(Buffer::Constructor(mark, pos, lim, cap, 1));
    mHb = hb;
    mOffset = offset;
    return NOERROR;
}

ECode CharBuffer::Constructor(
    /* [in] */ Integer mark,
    /* [in] */ Integer pos,
    /* [in] */ Integer lim,
    /* [in] */ Integer cap)
{
    Array<Char> nullHb;
    return Constructor(mark, pos, lim, cap, nullHb, 0);
}

ECode CharBuffer::Allocate(
    /* [in] */ Integer capacity,
    /* [out] */ ICharBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer);

    if (capacity < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<HeapCharBuffer> hcb = new HeapCharBuffer();
    FAIL_RETURN(hcb->Constructor(capacity, capacity));
    *buffer = (ICharBuffer*)hcb.Get();
    REFCOUNT_ADD(*buffer);
    return NOERROR;
}

ECode CharBuffer::Wrap(
    /* [in] */ const Array<Char>& array,
    /* [in] */ Integer offset,
    /* [in] */ Integer length,
    /* [out] */ ICharBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer);

    AutoPtr<HeapCharBuffer> hcb = new HeapCharBuffer();
    FAIL_RETURN(hcb->Constructor(array, offset, length));
    *buffer = (ICharBuffer*)hcb.Get();
    REFCOUNT_ADD(*buffer);
    return NOERROR;
}

ECode CharBuffer::Wrap(
    /* [in] */ const Array<Char>& array,
    /* [out] */ ICharBuffer** buffer)
{
    return Wrap(array, 0, array.GetLength(), buffer);
}

ECode CharBuffer::Read(
    /* [in] */ ICharBuffer* target,
    /* [out] */ Integer* number)
{
    VALIDATE_NOT_NULL(number);

    Integer remaining;
    Remaining(&remaining);
    if (remaining == 0) {
        *number = -1;
        return NOERROR;
    }
    Integer targetRemaining;
    IBuffer::Probe(target)->Remaining(&targetRemaining);
    Integer n = Math::Min(remaining, targetRemaining);
    Integer limit;
    GetLimit(&limit);
    if (targetRemaining < remaining) {
        Integer pos;
        GetPosition(&pos);
        SetLimit(pos + n);
    }
    if (n > 0) {
        target->Put(this);
    }
    SetLimit(limit);
    *number = n;
    return NOERROR;
}

ECode CharBuffer::Wrap(
    /* [in] */ ICharSequence* csq,
    /* [in] */ Integer start,
    /* [in] */ Integer end,
    /* [out] */ ICharBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer);

    AutoPtr<StringCharBuffer> scb = new StringCharBuffer();
    FAIL_RETURN(scb->Constructor(csq, start, end));
    *buffer = (ICharBuffer*)scb.Get();
    REFCOUNT_ADD(*buffer);
    return NOERROR;
}

ECode CharBuffer::Wrap(
    /* [in] */ ICharSequence* csq,
    /* [out] */ ICharBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer);

    Integer number;
    csq->GetLength(&number);
    return Wrap(csq, 0, number, buffer);
}

ECode CharBuffer::Get(
    /* [out] */ Array<Char>& dst,
    /* [in] */ Integer offset,
    /* [in] */ Integer length)
{
    FAIL_RETURN(CheckBounds(offset, length, dst.GetLength()));
    Integer remaining;
    if (Remaining(&remaining), length > remaining) {
        return E_BUFFER_UNDERFLOW_EXCEPTION;
    }
    Integer end = offset + length;
    for (Integer i = offset; i < end; i++) {
        Get(&dst[i]);
    }
    return NOERROR;
}

ECode CharBuffer::Get(
    /* [out] */ Array<Char>& dst)
{
    return Get(dst, 0, dst.GetLength());
}

ECode CharBuffer::Put(
    /* [in] */ ICharBuffer* src)
{
    if (src == (ICharBuffer*)this) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    CharBuffer* srcObject = (CharBuffer*)src;

    Integer n, remaining;
    srcObject->Remaining(&n);
    if (Remaining(&remaining), n > remaining) {
        return E_BUFFER_OVERFLOW_EXCEPTION;
    }
    for (Integer i = 0; i < n; i++) {
        Char c;
        srcObject->Get(&c);
        Put(c);
    }
    return NOERROR;
}

ECode CharBuffer::Put(
    /* [in] */ const Array<Char>& src,
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

ECode CharBuffer::Put(
    /* [in] */ const Array<Char>& src)
{
    return Put(src, 0, src.GetLength());
}

ECode CharBuffer::Put(
    /* [in]　*/ const String& src,
    /* [in] */ Integer start,
    /* [in]　*/ Integer end)
{
    FAIL_RETURN(CheckBounds(start, end - start, src.GetLength()));

    if (start == end) {
        return NOERROR;
    }

    if (mIsReadOnly) {
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }

    Integer remaining;
    if (Remaining(&remaining), (end - start) > remaining) {
        return E_BUFFER_OVERFLOW_EXCEPTION;
    }

    for (Integer i = start; i < end; i++) {
        Put(src.GetChar(i));
    }
    return NOERROR;
}

ECode CharBuffer::Put(
    /* [in] */ const String& src)
{
    return Put(src, 0, src.GetLength());
}

ECode CharBuffer::HasArray(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = (!mHb.IsNull() && !mIsReadOnly);
    return NOERROR;
}

ECode CharBuffer::GetArray(
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

ECode CharBuffer::GetArrayOffset(
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

ECode CharBuffer::GetHashCode(
    /* [out] */ Integer* hash)
{
    VALIDATE_NOT_NULL(hash);

    Integer h = 1;
    Integer p;
    GetPosition(&p);
    Integer i;
    GetLimit(&i);
    for (i = i - 1; i >= p; i--) {
        Char c;
        Get(i, &c);
        h = 31 * h + c;
    }
    *hash = h;
    return NOERROR;
}

ECode CharBuffer::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* same)
{
    VALIDATE_NOT_NULL(same);

    CharBuffer* other = (CharBuffer*)ICharBuffer::Probe(obj);
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
        Char thisC, otherC;
        Get(i, &thisC);
        other->Get(j, &otherC);
        if (thisC != otherC) {
            *same = false;
            return NOERROR;
        }
    }
    *same = true;
    return NOERROR;
}

ECode CharBuffer::CompareTo(
    /* [in] */ IInterface* other,
    /* [out] */ Integer* result)
{
    VALIDATE_NOT_NULL(result);

    CharBuffer* otherCB = (CharBuffer*)ICharBuffer::Probe(other);
    if (otherCB == nullptr) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer thisRemaining, otherRemaining;
    Remaining(&thisRemaining);
    otherCB->Remaining(&otherRemaining);
    Integer thisPos, otherPos;
    GetPosition(&thisPos);
    otherCB->GetPosition(&otherPos);

    Integer n = thisPos + Math::Min(thisRemaining, otherRemaining);
    for (Integer i = thisPos, j = otherPos; i < n; i++, j++) {
        Char thisC, otherC;
        Get(i, &thisC);
        otherCB->Get(j, &otherC);
        Integer cmp = thisC - otherC;
        if (cmp != 0) {
            *result = cmp;
            return NOERROR;
        }
    }
    *result = thisRemaining - otherRemaining;
    return NOERROR;
}

ECode CharBuffer::ToString(
    /* [out] */ String* desc)
{
    VALIDATE_NOT_NULL(desc);

    Integer pos, lim;
    GetPosition(&pos);
    GetLimit(&lim);
    return ToString(pos, lim, desc);
}

ECode CharBuffer::GetLength(
    /* [out] */ Integer* number)
{
    return Remaining(number);
}

ECode CharBuffer::GetCharAt(
    /* [in] */ Integer index,
    /* [out] */ Char* c)
{
    VALIDATE_NOT_NULL(c);

    FAIL_RETURN(CheckIndex(index, 1));
    Integer pos;
    GetPosition(&pos);
    return Get(pos + index, c);
}

ECode CharBuffer::Append(
    /* [in] */ ICharSequence* csq)
{
    if (csq == nullptr) {
        return Put(String("null"));
    }
    else {
        return Put(CoreUtils::Unbox(csq));
    }
}

ECode CharBuffer::Append(
    /* [in] */ ICharSequence* csq,
    /* [in] */ Integer start,
    /* [in] */ Integer end)
{
    AutoPtr<ICharSequence> cs = csq;
    if (cs == nullptr) {
        CString::New(String("null"), IID_ICharSequence, (IInterface**)&cs);
    }
    AutoPtr<ICharSequence> subcsq;
    cs->SubSequence(start, end, &subcsq);
    return Put(CoreUtils::Unbox(subcsq));
}

ECode CharBuffer::Append(
    /* [in] */ Char c)
{
    return Put(c);
}

}
}
