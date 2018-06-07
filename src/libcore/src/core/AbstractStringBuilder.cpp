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

#include "core/AbstractStringBuilder.h"

namespace ccm {
namespace core {

CCM_INTERFACE_IMPL_2(AbstractStringBuilder, SyncObject, IAppendable, ICharSequence);

ECode AbstractStringBuilder::constructor(
    /* [in] */ Integer capacity)
{
    return NOERROR;
}

ECode AbstractStringBuilder::GetLength(
    /* [out] */ Integer* number)
{
    return NOERROR;
}

ECode AbstractStringBuilder::GetCharAt(
    /* [in] */ Integer index,
    /* [out] */ Char* c)
{
    return NOERROR;
}

ECode AbstractStringBuilder::Append(
    /* [in] */ const String& str)
{
    return NOERROR;
}

ECode AbstractStringBuilder::Append(
    /* [in] */ IStringBuffer* sb)
{
    return NOERROR;
}

ECode AbstractStringBuilder::Append(
    /* [in] */ ICharSequence* s)
{
    return NOERROR;
}

ECode AbstractStringBuilder::Append(
    /* [in] */ ICharSequence* s,
    /* [in] */ Integer start,
    /* [in] */ Integer end)
{
    return NOERROR;
}

ECode AbstractStringBuilder::Append(
    /* [in] */ const Array<Char>& str)
{
    return NOERROR;
}

ECode AbstractStringBuilder::Append(
    /* [in] */ const Array<Char>& str,
    /* [in] */ Integer offset,
    /* [in] */ Integer len)
{
    return NOERROR;
}

ECode AbstractStringBuilder::Append(
    /* [in] */ Boolean b)
{
    return NOERROR;
}

ECode AbstractStringBuilder::Append(
    /* [in] */ Char c)
{
    return NOERROR;
}

ECode AbstractStringBuilder::Append(
    /* [in] */ Integer i)
{
    return NOERROR;
}

ECode AbstractStringBuilder::Append(
    /* [in] */ Long l)
{
    return NOERROR;
}

ECode AbstractStringBuilder::Append(
    /* [in] */ Float f)
{
    return NOERROR;
}

ECode AbstractStringBuilder::Append(
    /* [in] */ Double d)
{
    return NOERROR;
}

ECode AbstractStringBuilder::Delete(
    /* [in] */ Integer start,
    /* [in] */ Integer end)
{
    return NOERROR;
}

ECode AbstractStringBuilder::DeleteCharAt(
    /* [in] */ Integer index)
{
    return NOERROR;
}

ECode AbstractStringBuilder::Replace(
    /* [in] */ Integer start,
    /* [in] */ Integer end,
    /* [in] */ const String& str)
{
    return NOERROR;
}

ECode AbstractStringBuilder::Insert(
    /* [in] */ Integer index,
    /* [in] */ const Array<Char>& str,
    /* [in] */ Integer offset,
    /* [in] */ Integer len)
{
    return NOERROR;
}

ECode AbstractStringBuilder::Insert(
    /* [in] */ Integer offset,
    /* [in] */ IInterface* obj)
{
    return NOERROR;
}

ECode AbstractStringBuilder::Insert(
    /* [in] */ Integer offset,
    /* [in] */ const String& str)
{
    return NOERROR;
}

ECode AbstractStringBuilder::Insert(
    /* [in] */ Integer offset,
    /* [in] */ const Array<Char>& str)
{
    return NOERROR;
}

ECode AbstractStringBuilder::Insert(
    /* [in] */ Integer dstOffset,
    /* [in] */ ICharSequence* s)
{
    return NOERROR;
}

ECode AbstractStringBuilder::Insert(
    /* [in] */ Integer dstOffset,
    /* [in] */ ICharSequence* s,
    /* [in] */ Integer start,
    /* [in] */ Integer end)
{
    return NOERROR;
}

ECode AbstractStringBuilder::Insert(
    /* [in] */ Integer offset,
    /* [in] */ Boolean b)
{
    return NOERROR;
}

ECode AbstractStringBuilder::Insert(
    /* [in] */ Integer offset,
    /* [in] */ Char c)
{
    return NOERROR;
}

ECode AbstractStringBuilder::Insert(
    /* [in] */ Integer offset,
    /* [in] */ Integer i)
{
    return NOERROR;
}

ECode AbstractStringBuilder::Insert(
    /* [in] */ Integer offset,
    /* [in] */ Long l)
{
    return NOERROR;
}

ECode AbstractStringBuilder::Insert(
    /* [in] */ Integer offset,
    /* [in] */ Float f)
{
    return NOERROR;
}

ECode AbstractStringBuilder::Insert(
    /* [in] */ Integer offset,
    /* [in] */ Double d)
{
    return NOERROR;
}

ECode AbstractStringBuilder::IndexOf(
    /* [in] */ const String& str,
    /* [out] */ Integer* idx)
{
    return NOERROR;
}

ECode AbstractStringBuilder::IndexOf(
    /* [in] */ const String& str,
    /* [in] */ Integer fromIndex,
    /* [out] */ Integer* idx)
{
    return NOERROR;
}

ECode AbstractStringBuilder::LastIndexOf(
    /* [in] */ const String& str,
    /* [out] */ Integer* idx)
{
    return NOERROR;
}

ECode AbstractStringBuilder::LastIndexOf(
    /* [in] */ const String& str,
    /* [in] */ Integer fromIndex,
    /* [out] */ Integer* idx)
{
    return NOERROR;
}

ECode AbstractStringBuilder::Reverse()
{
    return NOERROR;
}

ECode AbstractStringBuilder::SubSequence(
    /* [in] */ Integer start,
    /* [in] */ Integer end,
    /* [out] */ ICharSequence** subcsq)
{
    return NOERROR;
}

ECode AbstractStringBuilder::ToString(
    /* [out] */ String* str)
{
    return NOERROR;
}

}
}
