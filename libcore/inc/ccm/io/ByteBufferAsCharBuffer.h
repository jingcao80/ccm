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

#ifndef __CCM_IO_BYTEBUFFERASCHARBUFFER_H__
#define __CCM_IO_BYTEBUFFERASCHARBUFFER_H__

#include "ccm/io/ByteBuffer.h"
#include "ccm/io/CharBuffer.h"
#include "ccm.io.IByteOrder.h"

namespace ccm {
namespace io {

class ByteBufferAsCharBuffer
    : public CharBuffer
{
public:
    ECode Constructor(
        /* [in] */ ByteBuffer* bb,
        /* [in] */ Integer mark,
        /* [in] */ Integer pos,
        /* [in] */ Integer lim,
        /* [in] */ Integer cap,
        /* [in] */ Integer off,
        /* [in] */ IByteOrder* order);

    ECode Slice(
        /* [out] */ ICharBuffer** buffer) override;

    ECode Duplicate(
        /* [out] */ ICharBuffer** buffer) override;

    ECode AsReadOnlyBuffer(
        /* [out] */ ICharBuffer** buffer) override;


    ECode Get(
        /* [out] */ Char* c) override;

    ECode Get(
        /* [in] */ Integer index,
        /* [out] */ Char* c) override;

    ECode Get(
        /* [out] */ Array<Char>& dst,
        /* [in] */ Integer offset,
        /* [in] */ Integer length) override;

    ECode GetUnchecked(
        /* [in] */ Integer index,
        /* [out] */ Char* c) override;

    ECode Put(
        /* [in] */ Char c) override;

    ECode Put(
        /* [in] */ Integer index,
        /* [in] */ Char c) override;

    ECode Put(
        /* [in] */ const Array<Char>& src,
        /* [in] */ Integer offset,
        /* [in] */ Integer length) override;

    ECode Compact() override;

    ECode IsDirect(
        /* [out] */ Boolean* direct) override;

    ECode IsReadOnly(
        /* [out] */ Boolean* readOnly) override;

    ECode ToString(
        /* [in] */ Integer start,
        /* [in] */ Integer end,
        /* [out] */ String* desc) override;

    ECode SubSequence(
        /* [in] */ Integer start,
        /* [in] */ Integer end,
        /* [out] */ ICharSequence** subcsq) override;

    ECode GetOrder(
        /* [out] */ IByteOrder** bo) override;

protected:
    Integer Ix(
        /* [in] */ Integer i);

protected:
    AutoPtr<ByteBuffer> mBB;
    Integer mOffset;

private:
    AutoPtr<IByteOrder> mOrder;
};

inline Integer ByteBufferAsCharBuffer::Ix(
    /* [in] */ Integer i)
{
    return (i << 2) + mOffset;
}

}
}

#endif // __CCM_IO_BYTEBUFFERASCHARBUFFER_H__
