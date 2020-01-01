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

#ifndef __COMO_CORE_CSTRINGBUFFER_H__
#define __COMO_CORE_CSTRINGBUFFER_H__

#include "como/core/AbstractStringBuilder.h"
#include "como.core.ICharSequence.h"
#include "como.core.IStringBuilder.h"
#include "como.io.ISerializable.h"
#include "_como_core_CStringBuffer.h"

using como::io::ISerializable;

namespace como {
namespace core {

Coclass(CStringBuffer)
    , public AbstractStringBuilder
    , public ISerializable
    , public IStringBuffer
{
public:
    COMO_INTERFACE_DECL();

    COMO_OBJECT_DECL();

    ECode Constructor();

    ECode Constructor(
        /* [in] */ Integer capacity);

    ECode Constructor(
        /* [in] */ const String& str);

    ECode Constructor(
        /* [in] */ ICharSequence* seq);

    ECode GetLength(
        /* [out] */ Integer* number) override;

    ECode GetCapacity(
        /* [out] */ Integer* capacity) override;

    ECode EnsureCapacity(
        /* [in] */ Integer minimumCapacity) override;

    ECode TrimToSize() override;

    ECode SetLength(
        /* [in] */ Integer newLength) override;

    ECode GetCharAt(
        /* [in] */ Integer index,
        /* [out] */ Char* c) override;

    ECode GetChars(
        /* [in] */ Integer start,
        /* [in] */ Integer end,
        /* [out] */ Array<Char>& dst,
        /* [in] */ Integer dstStart) override;

    ECode SetCharAt(
        /* [in] */ Integer index,
        /* [in] */ Char ch) override;

    ECode Append(
        /* [in] */ IInterface* obj) override;

    ECode Append(
        /* [in] */ const String& str) override;

    ECode Append(
        /* [in] */ IStringBuffer* sb) override;

    ECode Append(
        /* [in] */ ICharSequence* s) override;

    ECode Append(
        /* [in] */ ICharSequence* s,
        /* [in] */ Integer start,
        /* [in] */ Integer end) override;

    ECode Append(
        /* [in] */ const Array<Char>& str) override;

    ECode Append(
        /* [in] */ const Array<Char>& str,
        /* [in] */ Integer offset,
        /* [in] */ Integer len) override;

    ECode Append(
        /* [in] */ Boolean b) override;

    ECode Append(
        /* [in] */ Char c) override;

    ECode Append(
        /* [in] */ Integer i) override;

    ECode Append(
        /* [in] */ Long l) override;

    ECode Append(
        /* [in] */ Float f) override;

    ECode Append(
        /* [in] */ Double d) override;

    ECode Delete(
        /* [in] */ Integer start,
        /* [in] */ Integer end) override;

    ECode DeleteCharAt(
        /* [in] */ Integer index) override;

    ECode Replace(
        /* [in] */ Integer start,
        /* [in] */ Integer end,
        /* [in] */ const String& str) override;

    ECode Substring(
        /* [in] */ Integer start,
        /* [out] */ String* str) override;

    ECode SubSequence(
        /* [in] */ Integer start,
        /* [in] */ Integer end,
        /* [out] */ ICharSequence** subcsq) override;

    ECode Substring(
        /* [in] */ Integer start,
        /* [in] */ Integer end,
        /* [out] */ String* str) override;

    ECode Insert(
        /* [in] */ Integer index,
        /* [in] */ const Array<Char>& str,
        /* [in] */ Integer offset,
        /* [in] */ Integer len) override;

    ECode Insert(
        /* [in] */ Integer offset,
        /* [in] */ IInterface* obj) override;

    ECode Insert(
        /* [in] */ Integer offset,
        /* [in] */ const String& str) override;

    ECode Insert(
        /* [in] */ Integer offset,
        /* [in] */ const Array<Char>& str) override;

    ECode Insert(
        /* [in] */ Integer dstOffset,
        /* [in] */ ICharSequence* s) override;

    ECode Insert(
        /* [in] */ Integer dstOffset,
        /* [in] */ ICharSequence* s,
        /* [in] */ Integer start,
        /* [in] */ Integer end) override;

    ECode Insert(
        /* [in] */ Integer offset,
        /* [in] */ Boolean b) override;

    ECode Insert(
        /* [in] */ Integer offset,
        /* [in] */ Char c) override;

    ECode Insert(
        /* [in] */ Integer offset,
        /* [in] */ Integer i) override;

    ECode Insert(
        /* [in] */ Integer offset,
        /* [in] */ Long l) override;

    ECode Insert(
        /* [in] */ Integer offset,
        /* [in] */ Float f) override;

    ECode Insert(
        /* [in] */ Integer offset,
        /* [in] */ Double d) override;

    ECode IndexOf(
        /* [in] */ const String& str,
        /* [out] */ Integer* idx) override;

    ECode IndexOf(
        /* [in] */ const String& str,
        /* [in] */ Integer fromIndex,
        /* [out] */ Integer* idx) override;

    ECode LastIndexOf(
        /* [in] */ const String& str,
        /* [out] */ Integer* idx) override;

    ECode LastIndexOf(
        /* [in] */ const String& str,
        /* [in] */ Integer fromIndex,
        /* [out] */ Integer* idx) override;

    ECode Reverse() override;

    ECode ToString(
        /* [out] */ String* str) override;
};

}
}

#endif // __COMO_CORE_CSTRINGBUFFER_H__
