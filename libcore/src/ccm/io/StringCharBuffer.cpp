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

#include "ccm/io/ByteOrder.h"
#include "ccm/io/StringCharBuffer.h"

using ccm::core::E_INDEX_OUT_OF_BOUNDS_EXCEPTION;

namespace ccm {
namespace io {

ECode StringCharBuffer::Constructor(
    /* [in] */ ICharSequence* s,
    /* [in] */ Integer start,
    /* [in] */ Integer end)
{
    Integer len;
    s->GetLength(&len);
    FAIL_RETURN(CharBuffer::Constructor(-1, start, end, len));
    if ((start < 0) || (start > len) || (end < start) || (end > len)) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    mStr = s;
    return NOERROR;
}

ECode StringCharBuffer::Constructor(
    /* [in] */ ICharSequence* s,
    /* [in] */ Integer mark,
    /* [in] */ Integer pos,
    /* [in] */ Integer limit,
    /* [in] */ Integer cap,
    /* [in] */ Integer offset)
{
    Array<Char> nullHb;
    FAIL_RETURN(CharBuffer::Constructor(mark, pos, limit, cap, nullHb, offset));
    mStr = s;
    return NOERROR;
}

ECode StringCharBuffer::Slice(
    /* [out] */ ICharBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer);

    Integer remaining, pos;
    Remaining(&remaining);
    GetPosition(&pos);
    AutoPtr<StringCharBuffer> hsb = new StringCharBuffer();
    FAIL_RETURN(hsb->Constructor(
            mStr, -1, 0, remaining, remaining, pos + mOffset));
    *buffer = (ICharBuffer*)hsb.Get();
    REFCOUNT_ADD(*buffer);
    return NOERROR;
}

ECode StringCharBuffer::Duplicate(
    /* [out] */ ICharBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer);

    Integer pos, lim, cap;
    GetPosition(&pos);
    GetLimit(&lim);
    GetCapacity(&cap);
    AutoPtr<StringCharBuffer> hsb = new StringCharBuffer();
    FAIL_RETURN(hsb->Constructor(
            mStr, MarkValue(), pos, lim, cap, mOffset));
    *buffer = (ICharBuffer*)hsb.Get();
    REFCOUNT_ADD(*buffer);
    return NOERROR;
}

ECode StringCharBuffer::AsReadOnlyBuffer(
    /* [out] */ ICharBuffer** buffer)
{
    return Duplicate(buffer);
}

ECode StringCharBuffer::Get(
    /* [out] */ Char* c)
{
    VALIDATE_NOT_NULL(c);

    Integer index;
    NextGetIndex(&index);
    mStr->GetCharAt(index + mOffset, c);
    return NOERROR;
}

ECode StringCharBuffer::Get(
    /* [in] */ Integer index,
    /* [out] */ Char* c)
{
    VALIDATE_NOT_NULL(c);

    FAIL_RETURN(CheckIndex(index));
    mStr->GetCharAt(index + mOffset, c);
    return NOERROR;
}

ECode StringCharBuffer::GetUnchecked(
    /* [in] */ Integer index,
    /* [out] */ Char* c)
{
    VALIDATE_NOT_NULL(c);

    mStr->GetCharAt(index + mOffset, c);
    return NOERROR;
}

ECode StringCharBuffer::Put(
    /* [in] */ Char c)
{
    return E_READ_ONLY_BUFFER_EXCEPTION;
}

ECode StringCharBuffer::Put(
    /* [in] */ Integer index,
    /* [in] */ Char c)
{
    return E_READ_ONLY_BUFFER_EXCEPTION;
}

ECode StringCharBuffer::Compact()
{
    return E_READ_ONLY_BUFFER_EXCEPTION;
}

ECode StringCharBuffer::IsReadOnly(
    /* [out] */ Boolean* readOnly)
{
    VALIDATE_NOT_NULL(readOnly);

    *readOnly = true;
    return NOERROR;
}

ECode StringCharBuffer::ToString(
    /* [in] */ Integer start,
    /* [in] */ Integer end,
    /* [out] */ String* desc)
{
    VALIDATE_NOT_NULL(desc);

    String str;
    mStr->ToString(&str);
    *desc = str.Substring(start + mOffset, end + mOffset);
    return NOERROR;
}

ECode StringCharBuffer::SubSequence(
    /* [in] */ Integer start,
    /* [in] */ Integer end,
    /* [out] */ ICharSequence** subcsq)
{
    VALIDATE_NOT_NULL(subcsq);

    Integer pos, cap;
    GetPosition(&pos);
    FAIL_RETURN(CheckIndex(start, pos));
    FAIL_RETURN(CheckIndex(end, pos));
    GetCapacity(&cap);
    AutoPtr<StringCharBuffer> scb = new StringCharBuffer();
    FAIL_RETURN(scb->Constructor(mStr, -1, pos + start, pos + end, cap, mOffset));
    *subcsq = (ICharSequence*)scb.Get();
    return NOERROR;
}

ECode StringCharBuffer::IsDirect(
    /* [out] */ Boolean* direct)
{
    VALIDATE_NOT_NULL(direct);

    *direct = false;
    return NOERROR;
}

ECode StringCharBuffer::GetOrder(
    /* [out] */ IByteOrder** bo)
{
    VALIDATE_NOT_NULL(bo);

    AutoPtr<IByteOrder> order = ByteOrder::Order();
    order.MoveTo(bo);
    return NOERROR;
}

}
}
