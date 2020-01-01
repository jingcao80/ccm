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

#ifndef __COMO_IO_CHARBUFFER_H__
#define __COMO_IO_CHARBUFFER_H__

#include "como/io/Buffer.h"
#include "como.core.IAppendable.h"
#include "como.core.ICharSequence.h"
#include "como.core.IComparable.h"
#include "como.core.IReadable.h"
#include "como.io.ICharBuffer.h"

using como::core::IAppendable;
using como::core::ICharSequence;
using como::core::IComparable;
using como::core::IReadable;

namespace como {
namespace io {

class CharBuffer
    : public Buffer
    , public ICharBuffer
    , public IComparable
    , public IAppendable
    , public ICharSequence
    , public IReadable
{
public:
    COMO_INTERFACE_DECL();

    ECode Constructor(
        /* [in] */ Integer mark,
        /* [in] */ Integer pos,
        /* [in] */ Integer lim,
        /* [in] */ Integer cap,
        /* [in] */ const Array<Char>& hb,
        /* [in] */ Integer offset);

    ECode Constructor(
        /* [in] */ Integer mark,
        /* [in] */ Integer pos,
        /* [in] */ Integer lim,
        /* [in] */ Integer cap);

    static ECode Allocate(
        /* [in] */ Integer capacity,
        /* [out] */ ICharBuffer** buffer);

    static ECode Wrap(
        /* [in] */ const Array<Char>& array,
        /* [in] */ Integer offset,
        /* [in] */ Integer length,
        /* [out] */ ICharBuffer** buffer);

    static ECode Wrap(
        /* [in] */ const Array<Char>& array,
        /* [out] */ ICharBuffer** buffer);

    ECode Read(
        /* [in] */ ICharBuffer* target,
        /* [out] */ Integer* number) override;

    static ECode Wrap(
        /* [in] */ ICharSequence* csq,
        /* [in] */ Integer start,
        /* [in] */ Integer end,
        /* [out] */ ICharBuffer** buffer);

    static ECode Wrap(
        /* [in] */ ICharSequence* csq,
        /* [out] */ ICharBuffer** buffer);

    virtual ECode GetUnchecked(
        /* [in] */ Integer index,
        /* [out] */ Char* c) = 0;

    ECode Get(
        /* [out] */ Array<Char>& dst,
        /* [in] */ Integer offset,
        /* [in] */ Integer length) override;

    ECode Get(
        /* [out] */ Array<Char>& dst) override;

    using ICharBuffer::Get;

    ECode Put(
        /* [in] */ ICharBuffer* src) override;

    ECode Put(
        /* [in] */ const Array<Char>& src,
        /* [in] */ Integer offset,
        /* [in] */ Integer length) override;

    ECode Put(
        /* [in] */ const Array<Char>& src) override final;

    ECode Put(
        /* [in]　*/ const String& src,
        /* [in] */ Integer start,
        /* [in]　*/ Integer end) override;

    ECode Put(
        /* [in] */ const String& src) override final;

    ECode HasArray(
        /* [out] */ Boolean* result) override final;

    ECode GetArray(
        /* [out] */ IInterface** array) override final;

    ECode GetArrayOffset(
        /* [out] */ Integer* offset) override final;

    ECode GetHashCode(
        /* [out] */ Integer* hash) override;

    ECode Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* same) override;

    ECode CompareTo(
        /* [in] */ IInterface* other,
        /* [out] */ Integer* result) override;

    ECode ToString(
        /* [out] */ String* desc) override;

    using ICharBuffer::ToString;

    ECode GetLength(
        /* [out] */ Integer* number) override;

    ECode GetCharAt(
        /* [in] */ Integer index,
        /* [out] */ Char* c) override final;

    ECode Append(
        /* [in] */ ICharSequence* csq) override;

    ECode Append(
        /* [in] */ ICharSequence* csq,
        /* [in] */ Integer start,
        /* [in] */ Integer end) override;

    ECode Append(
        /* [in] */ Char c) override;

protected:
    Array<Char> mHb;
    Integer mOffset = 0;
    Boolean mIsReadOnly = false;
};

}
}

#endif // __COMO_IO_CHARBUFFER_H__
