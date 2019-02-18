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

#include "ccm/core/CStringBuilder.h"

using ccm::io::IID_ISerializable;

namespace ccm {
namespace core {

CCM_INTERFACE_IMPL_2(CStringBuilder, AbstractStringBuilder, ISerializable, IStringBuilder);

CCM_OBJECT_IMPL(CStringBuilder);

ECode CStringBuilder::Constructor()
{
    return Constructor(16);
}

ECode CStringBuilder::Constructor(
    /* [in] */ Integer capacity)
{
    return AbstractStringBuilder::Constructor(16);
}

ECode CStringBuilder::Constructor(
    /* [in] */ const String& str)
{
    Constructor(str.GetByteLength() + 16);
    return Append(str);
}

ECode CStringBuilder::Constructor(
    /* [in] */ ICharSequence* seq)
{
    String str;
    seq->ToString(&str);
    return Constructor(str);
}

ECode CStringBuilder::Append(
    /* [in] */ IInterface* obj)
{
    return Append(Object::ToString(obj));
}

ECode CStringBuilder::Append(
    /* [in] */ const String& str)
{
    return AbstractStringBuilder::Append(str);
}

ECode CStringBuilder::Append(
    /* [in] */ IStringBuffer* sb)
{
    return AbstractStringBuilder::Append(sb);
}

ECode CStringBuilder::Append(
    /* [in] */ ICharSequence* s)
{
    return AbstractStringBuilder::Append(s);
}

ECode CStringBuilder::Append(
    /* [in] */ ICharSequence* s,
    /* [in] */ Integer start,
    /* [in] */ Integer end)
{
    return AbstractStringBuilder::Append(s, start, end);
}

ECode CStringBuilder::Append(
    /* [in] */ const Array<Char>& str)
{
    return AbstractStringBuilder::Append(str);
}

ECode CStringBuilder::Append(
    /* [in] */ const Array<Char>& str,
    /* [in] */ Integer offset,
    /* [in] */ Integer len)
{
    return AbstractStringBuilder::Append(str, offset, len);
}

ECode CStringBuilder::Append(
    /* [in] */ Boolean b)
{
    return AbstractStringBuilder::Append(b);
}

ECode CStringBuilder::Append(
    /* [in] */ Char c)
{
    return AbstractStringBuilder::Append(c);
}

ECode CStringBuilder::Append(
    /* [in] */ Integer i)
{
    return AbstractStringBuilder::Append(i);
}

ECode CStringBuilder::Append(
    /* [in] */ Long l)
{
    return AbstractStringBuilder::Append(l);
}

ECode CStringBuilder::Append(
    /* [in] */ Float f)
{
    return AbstractStringBuilder::Append(f);
}

ECode CStringBuilder::Append(
    /* [in] */ Double d)
{
    return AbstractStringBuilder::Append(d);
}

ECode CStringBuilder::Delete(
    /* [in] */ Integer start,
    /* [in] */ Integer end)
{
    return AbstractStringBuilder::Delete(start, end);
}

ECode CStringBuilder::DeleteCharAt(
    /* [in] */ Integer index)
{
    return AbstractStringBuilder::DeleteCharAt(index);
}

ECode CStringBuilder::Replace(
    /* [in] */ Integer start,
    /* [in] */ Integer end,
    /* [in] */ const String& str)
{
    return AbstractStringBuilder::Replace(start, end, str);
}

ECode CStringBuilder::Insert(
    /* [in] */ Integer index,
    /* [in] */ const Array<Char>& str,
    /* [in] */ Integer offset,
    /* [in] */ Integer len)
{
    return AbstractStringBuilder::Insert(index, str, offset, len);
}

ECode CStringBuilder::Insert(
    /* [in] */ Integer offset,
    /* [in] */ IInterface* obj)
{
    return AbstractStringBuilder::Insert(offset, obj);
}

ECode CStringBuilder::Insert(
    /* [in] */ Integer offset,
    /* [in] */ const String& str)
{
    return AbstractStringBuilder::Insert(offset, str);
}

ECode CStringBuilder::Insert(
    /* [in] */ Integer offset,
    /* [in] */ const Array<Char>& str)
{
    return AbstractStringBuilder::Insert(offset, str);
}

ECode CStringBuilder::Insert(
    /* [in] */ Integer dstOffset,
    /* [in] */ ICharSequence* s)
{
    return AbstractStringBuilder::Insert(dstOffset, s);
}

ECode CStringBuilder::Insert(
    /* [in] */ Integer dstOffset,
    /* [in] */ ICharSequence* s,
    /* [in] */ Integer start,
    /* [in] */ Integer end)
{
    return AbstractStringBuilder::Insert(dstOffset, s, start, end);
}

ECode CStringBuilder::Insert(
    /* [in] */ Integer offset,
    /* [in] */ Boolean b)
{
    return AbstractStringBuilder::Insert(offset, b);
}

ECode CStringBuilder::Insert(
    /* [in] */ Integer offset,
    /* [in] */ Char c)
{
    return AbstractStringBuilder::Insert(offset, c);
}

ECode CStringBuilder::Insert(
    /* [in] */ Integer offset,
    /* [in] */ Integer i)
{
    return AbstractStringBuilder::Insert(offset, i);
}

ECode CStringBuilder::Insert(
    /* [in] */ Integer offset,
    /* [in] */ Long l)
{
    return AbstractStringBuilder::Insert(offset, l);
}

ECode CStringBuilder::Insert(
    /* [in] */ Integer offset,
    /* [in] */ Float f)
{
    return AbstractStringBuilder::Insert(offset, f);
}

ECode CStringBuilder::Insert(
    /* [in] */ Integer offset,
    /* [in] */ Double d)
{
    return AbstractStringBuilder::Insert(offset, d);
}

ECode CStringBuilder::IndexOf(
    /* [in] */ const String& str,
    /* [out] */ Integer* idx)
{
    return AbstractStringBuilder::IndexOf(str, idx);
}

ECode CStringBuilder::IndexOf(
    /* [in] */ const String& str,
    /* [in] */ Integer fromIndex,
    /* [out] */ Integer* idx)
{
    return AbstractStringBuilder::IndexOf(str, fromIndex, idx);
}

ECode CStringBuilder::LastIndexOf(
    /* [in] */ const String& str,
    /* [out] */ Integer* idx)
{
    return AbstractStringBuilder::LastIndexOf(str, idx);
}

ECode CStringBuilder::LastIndexOf(
    /* [in] */ const String& str,
    /* [in] */ Integer fromIndex,
    /* [out] */ Integer* idx)
{
    return AbstractStringBuilder::LastIndexOf(str, fromIndex, idx);
}

ECode CStringBuilder::Reverse()
{
    return AbstractStringBuilder::Reverse();
}

ECode CStringBuilder::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    if (mCount == 0) {
        *str = "";
        return NOERROR;
    }
    *str = String(mValue, mByteCount);
    return NOERROR;
}

ECode CStringBuilder::EnsureCapacity(
    /* [in] */ Integer minimumCapacity)
{
    return AbstractStringBuilder::EnsureCapacity(minimumCapacity);
}

ECode CStringBuilder::GetCapacity(
    /* [out] */ Integer* capacity)
{
    return AbstractStringBuilder::GetCapacity(capacity);
}

ECode CStringBuilder::GetChars(
    /* [in] */ Integer start,
    /* [in] */ Integer end,
    /* [out] */ Array<Char>& dst,
    /* [in] */ Integer dstStart)
{
    return AbstractStringBuilder::GetChars(start, end, dst, dstStart);
}

ECode CStringBuilder::SetCharAt(
    /* [in] */ Integer index,
    /* [in] */ Char ch)
{
    return AbstractStringBuilder::SetCharAt(index, ch);
}

ECode CStringBuilder::SetLength(
    /* [in] */ Integer newLength)
{
    return AbstractStringBuilder::SetLength(newLength);
}

ECode CStringBuilder::Substring(
    /* [in] */ Integer start,
    /* [out] */ String* str)
{
    return AbstractStringBuilder::Substring(start, str);
}

ECode CStringBuilder::Substring(
    /* [in] */ Integer start,
    /* [in] */ Integer end,
    /* [out] */ String* str)
{
    return AbstractStringBuilder::Substring(start, end, str);
}

ECode CStringBuilder::TrimToSize()
{
    return AbstractStringBuilder::TrimToSize();
}

ECode CStringBuilder::GetCharAt(
    /* [in] */ Integer index,
    /* [out] */ Char* c)
{
    return AbstractStringBuilder::GetCharAt(index, c);
}

ECode CStringBuilder::GetLength(
    /* [out] */ Integer* number)
{
    return AbstractStringBuilder::GetLength(number);
}

ECode CStringBuilder::SubSequence(
    /* [in] */ Integer start,
    /* [in] */ Integer end,
    /* [out] */ ICharSequence** subcsq)
{
    return AbstractStringBuilder::SubSequence(start, end, subcsq);
}

}
}
