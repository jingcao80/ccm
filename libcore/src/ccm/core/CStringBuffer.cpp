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

#include "ccm/core/AutoLock.h"
#include "ccm/core/CStringBuffer.h"

using ccm::io::IID_ISerializable;

namespace ccm {
namespace core {

CCM_INTERFACE_IMPL_2(CStringBuffer, AbstractStringBuilder, ISerializable, IStringBuffer);

CCM_OBJECT_IMPL(CStringBuffer);

ECode CStringBuffer::Constructor()
{
    return Constructor(16);
}

ECode CStringBuffer::Constructor(
    /* [in] */ Integer capacity)
{
    return AbstractStringBuilder::Constructor(16);
}

ECode CStringBuffer::Constructor(
    /* [in] */ const String& str)
{
    Constructor(str.GetByteLength() + 16);
    return Append(str);
}

ECode CStringBuffer::Constructor(
    /* [in] */ ICharSequence* seq)
{
    String str;
    seq->ToString(&str);
    return Constructor(str);
}

ECode CStringBuffer::GetLength(
    /* [out] */ Integer* number)
{
    AutoLock lock(this);
    return AbstractStringBuilder::GetLength(number);
}

ECode CStringBuffer::GetCapacity(
    /* [out] */ Integer* capacity)
{
    AutoLock lock(this);
    return AbstractStringBuilder::GetCapacity(capacity);
}

ECode CStringBuffer::EnsureCapacity(
    /* [in] */ Integer minimumCapacity)
{
    AutoLock lock(this);
    return AbstractStringBuilder::EnsureCapacity(minimumCapacity);
}

ECode CStringBuffer::TrimToSize()
{
    AutoLock lock(this);
    return AbstractStringBuilder::TrimToSize();
}

ECode CStringBuffer::SetLength(
    /* [in] */ Integer newLength)
{
    AutoLock lock(this);
    return AbstractStringBuilder::SetLength(newLength);
}

ECode CStringBuffer::GetCharAt(
    /* [in] */ Integer index,
    /* [out] */ Char* c)
{
    AutoLock lock(this);
    return AbstractStringBuilder::GetCharAt(index, c);
}

ECode CStringBuffer::GetChars(
    /* [in] */ Integer start,
    /* [in] */ Integer end,
    /* [out] */ Array<Char>& dst,
    /* [in] */ Integer dstStart)
{
    AutoLock lock(this);
    return AbstractStringBuilder::GetChars(start, end, dst, dstStart);
}

ECode CStringBuffer::SetCharAt(
    /* [in] */ Integer index,
    /* [in] */ Char ch)
{
    AutoLock lock(this);
    return AbstractStringBuilder::SetCharAt(index, ch);
}

ECode CStringBuffer::Append(
    /* [in] */ IInterface* obj)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Append(Object::ToString(obj));
}

ECode CStringBuffer::Append(
    /* [in] */ const String& str)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Append(str);
}

ECode CStringBuffer::Append(
    /* [in] */ IStringBuffer* sb)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Append(sb);
}

ECode CStringBuffer::Append(
    /* [in] */ ICharSequence* s)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Append(s);
}

ECode CStringBuffer::Append(
    /* [in] */ ICharSequence* s,
    /* [in] */ Integer start,
    /* [in] */ Integer end)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Append(s, start, end);
}

ECode CStringBuffer::Append(
    /* [in] */ const Array<Char>& str)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Append(str);
}

ECode CStringBuffer::Append(
    /* [in] */ const Array<Char>& str,
    /* [in] */ Integer offset,
    /* [in] */ Integer len)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Append(str, offset, len);
}

ECode CStringBuffer::Append(
    /* [in] */ Boolean b)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Append(b);
}

ECode CStringBuffer::Append(
    /* [in] */ Char c)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Append(c);
}

ECode CStringBuffer::Append(
    /* [in] */ Integer i)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Append(i);
}

ECode CStringBuffer::Append(
    /* [in] */ Long l)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Append(l);
}

ECode CStringBuffer::Append(
    /* [in] */ Float f)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Append(f);
}

ECode CStringBuffer::Append(
    /* [in] */ Double d)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Append(d);
}

ECode CStringBuffer::Delete(
    /* [in] */ Integer start,
    /* [in] */ Integer end)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Delete(start, end);
}

ECode CStringBuffer::DeleteCharAt(
    /* [in] */ Integer index)
{
    AutoLock lock(this);
    return AbstractStringBuilder::DeleteCharAt(index);
}

ECode CStringBuffer::Replace(
    /* [in] */ Integer start,
    /* [in] */ Integer end,
    /* [in] */ const String& str)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Replace(start, end, str);
}

ECode CStringBuffer::Substring(
    /* [in] */ Integer start,
    /* [out] */ String* str)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Substring(start, str);
}

ECode CStringBuffer::SubSequence(
    /* [in] */ Integer start,
    /* [in] */ Integer end,
    /* [out] */ ICharSequence** subcsq)
{
    AutoLock lock(this);
    return AbstractStringBuilder::SubSequence(start, end, subcsq);
}

ECode CStringBuffer::Substring(
    /* [in] */ Integer start,
    /* [in] */ Integer end,
    /* [out] */ String* str)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Substring(start, end, str);
}

ECode CStringBuffer::Insert(
    /* [in] */ Integer index,
    /* [in] */ const Array<Char>& str,
    /* [in] */ Integer offset,
    /* [in] */ Integer len)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Insert(index, str, offset, len);
}

ECode CStringBuffer::Insert(
    /* [in] */ Integer offset,
    /* [in] */ IInterface* obj)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Insert(offset, obj);
}

ECode CStringBuffer::Insert(
    /* [in] */ Integer offset,
    /* [in] */ const String& str)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Insert(offset, str);
}

ECode CStringBuffer::Insert(
    /* [in] */ Integer offset,
    /* [in] */ const Array<Char>& str)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Insert(offset, str);
}

ECode CStringBuffer::Insert(
    /* [in] */ Integer dstOffset,
    /* [in] */ ICharSequence* s)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Insert(dstOffset, s);
}

ECode CStringBuffer::Insert(
    /* [in] */ Integer dstOffset,
    /* [in] */ ICharSequence* s,
    /* [in] */ Integer start,
    /* [in] */ Integer end)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Insert(dstOffset, s, start, end);
}

ECode CStringBuffer::Insert(
    /* [in] */ Integer offset,
    /* [in] */ Boolean b)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Insert(offset, b);
}

ECode CStringBuffer::Insert(
    /* [in] */ Integer offset,
    /* [in] */ Char c)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Insert(offset, c);
}

ECode CStringBuffer::Insert(
    /* [in] */ Integer offset,
    /* [in] */ Integer i)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Insert(offset, i);
}

ECode CStringBuffer::Insert(
    /* [in] */ Integer offset,
    /* [in] */ Long l)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Insert(offset, l);
}

ECode CStringBuffer::Insert(
    /* [in] */ Integer offset,
    /* [in] */ Float f)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Insert(offset, f);
}

ECode CStringBuffer::Insert(
    /* [in] */ Integer offset,
    /* [in] */ Double d)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Insert(offset, d);
}

ECode CStringBuffer::IndexOf(
    /* [in] */ const String& str,
    /* [out] */ Integer* idx)
{
    AutoLock lock(this);
    return AbstractStringBuilder::IndexOf(str, idx);
}

ECode CStringBuffer::IndexOf(
    /* [in] */ const String& str,
    /* [in] */ Integer fromIndex,
    /* [out] */ Integer* idx)
{
    AutoLock lock(this);
    return AbstractStringBuilder::IndexOf(str, fromIndex, idx);
}

ECode CStringBuffer::LastIndexOf(
    /* [in] */ const String& str,
    /* [out] */ Integer* idx)
{
    AutoLock lock(this);
    return AbstractStringBuilder::LastIndexOf(str, idx);
}

ECode CStringBuffer::LastIndexOf(
    /* [in] */ const String& str,
    /* [in] */ Integer fromIndex,
    /* [out] */ Integer* idx)
{
    AutoLock lock(this);
    return AbstractStringBuilder::LastIndexOf(str, fromIndex, idx);
}

ECode CStringBuffer::Reverse()
{
    AutoLock lock(this);
    return AbstractStringBuilder::Reverse();
}

ECode CStringBuffer::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    AutoLock lock(this);
    if (mCount == 0) {
        *str = "";
        return NOERROR;
    }
    *str = String(mValue, mByteCount);
    return NOERROR;
}

}
}
