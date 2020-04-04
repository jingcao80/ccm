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
#include "como/io/StringCharBuffer.h"

using como::core::E_INDEX_OUT_OF_BOUNDS_EXCEPTION;

namespace como {
namespace io {

ECode StringCharBuffer::Constructor(
    /* [in] */ ICharSequence* s,
    /* [in] */ Integer start,
    /* [in] */ Integer end)
{
    Integer len;
    s->GetLength(len);
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
    /* [out] */ AutoPtr<ICharBuffer>& buffer)
{
    Integer remaining, pos;
    Remaining(remaining);
    GetPosition(pos);
    AutoPtr<StringCharBuffer> hsb = new StringCharBuffer();
    FAIL_RETURN(hsb->Constructor(
            mStr, -1, 0, remaining, remaining, pos + mOffset));
    buffer = (ICharBuffer*)hsb.Get();
    return NOERROR;
}

ECode StringCharBuffer::Duplicate(
    /* [out] */ AutoPtr<ICharBuffer>& buffer)
{
    Integer pos, lim, cap;
    GetPosition(pos);
    GetLimit(lim);
    GetCapacity(cap);
    AutoPtr<StringCharBuffer> hsb = new StringCharBuffer();
    FAIL_RETURN(hsb->Constructor(
            mStr, MarkValue(), pos, lim, cap, mOffset));
    buffer = (ICharBuffer*)hsb.Get();
    return NOERROR;
}

ECode StringCharBuffer::AsReadOnlyBuffer(
    /* [out] */ AutoPtr<ICharBuffer>& buffer)
{
    return Duplicate(buffer);
}

ECode StringCharBuffer::Get(
    /* [out] */ Char& c)
{
    Integer index;
    NextGetIndex(&index);
    mStr->GetCharAt(index + mOffset, c);
    return NOERROR;
}

ECode StringCharBuffer::Get(
    /* [in] */ Integer index,
    /* [out] */ Char& c)
{
    FAIL_RETURN(CheckIndex(index));
    mStr->GetCharAt(index + mOffset, c);
    return NOERROR;
}

ECode StringCharBuffer::GetUnchecked(
    /* [in] */ Integer index,
    /* [out] */ Char& c)
{
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
    /* [out] */ Boolean& readOnly)
{
    readOnly = true;
    return NOERROR;
}

ECode StringCharBuffer::ToString(
    /* [in] */ Integer start,
    /* [in] */ Integer end,
    /* [out] */ String& desc)
{
    String str;
    mStr->ToString(str);
    desc = str.Substring(start + mOffset, end + mOffset);
    return NOERROR;
}

ECode StringCharBuffer::SubSequence(
    /* [in] */ Integer start,
    /* [in] */ Integer end,
    /* [out] */ AutoPtr<ICharSequence>& subcsq)
{
    Integer pos, cap;
    GetPosition(pos);
    FAIL_RETURN(CheckIndex(start, pos));
    FAIL_RETURN(CheckIndex(end, pos));
    GetCapacity(cap);
    AutoPtr<StringCharBuffer> scb = new StringCharBuffer();
    FAIL_RETURN(scb->Constructor(mStr, -1, pos + start, pos + end, cap, mOffset));
    subcsq = (ICharSequence*)scb.Get();
    return NOERROR;
}

ECode StringCharBuffer::IsDirect(
    /* [out] */ Boolean& direct)
{
    direct = false;
    return NOERROR;
}

ECode StringCharBuffer::GetOrder(
    /* [out] */ AutoPtr<IByteOrder>& bo)
{
    bo = ByteOrder::Order();
    return NOERROR;
}

}
}
