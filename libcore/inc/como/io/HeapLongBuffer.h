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

#ifndef __COMO_IO_HEAPLONGBUFFER_H__
#define __COMO_IO_HEAPLONGBUFFER_H__

#include "como/io/LongBuffer.h"

namespace como {
namespace io {

class HeapLongBuffer
    : public LongBuffer
{
public:
    ECode Constructor(
        /* [in] */ Integer cap,
        /* [in] */ Integer lim);

    ECode Constructor(
        /* [in] */ Integer cap,
        /* [in] */ Integer lim,
        /* [in] */ Boolean isReadOnly);

    ECode Constructor(
        /* [in] */ const Array<Long>& buf,
        /* [in] */ Integer off,
        /* [in] */ Integer len);

    ECode Constructor(
        /* [in] */ const Array<Long>& buf,
        /* [in] */ Integer off,
        /* [in] */ Integer len,
        /* [in] */ Boolean isReadOnly);

    ECode Slice(
        /* [out] */ AutoPtr<ILongBuffer>& buffer) override;

    ECode Duplicate(
        /* [out] */ AutoPtr<ILongBuffer>& buffer) override;

    ECode AsReadOnlyBuffer(
        /* [out] */ AutoPtr<ILongBuffer>& buffer) override;

    ECode Get(
        /* [out] */ Long& l) override;

    ECode Get(
        /* [in] */ Integer index,
        /* [out] */ Long& l) override;

    ECode Get(
        /* [out] */ Array<Long>& dst,
        /* [in] */ Integer offset,
        /* [in] */ Integer length) override;

    ECode IsDirect(
        /* [out] */ Boolean& direct) override;

    ECode IsReadOnly(
        /* [out] */ Boolean& readOnly) override;

    ECode Put(
        /* [in] */ Long l) override;

    ECode Put(
        /* [in] */ Integer index,
        /* [in] */ Long l) override;

    ECode Put(
        /* [in] */ const Array<Long>& src,
        /* [in] */ Integer offset,
        /* [in] */ Integer length) override;

    ECode Put(
        /* [in] */ ILongBuffer* src) override;

    ECode Compact() override;

    ECode GetOrder(
        /* [out] */ AutoPtr<IByteOrder>& bo) override;

    ECode GetCoclassID(
        /* [out] */ CoclassID& cid) override;

protected:
    ECode Constructor(
        /* [in] */ const Array<Long>& buf,
        /* [in] */ Integer mark,
        /* [in] */ Integer pos,
        /* [in] */ Integer lim,
        /* [in] */ Integer cap,
        /* [in] */ Integer off);

    ECode Constructor(
        /* [in] */ const Array<Long>& buf,
        /* [in] */ Integer mark,
        /* [in] */ Integer pos,
        /* [in] */ Integer lim,
        /* [in] */ Integer cap,
        /* [in] */ Integer off,
        /* [in] */ Boolean isReadOnly);

    Integer Ix(
        /* [in] */ Integer i);
};

inline Integer HeapLongBuffer::Ix(
    /* [in] */ Integer i)
{
    return i + mOffset;
}

}
}

#endif // __COMO_IO_HEAPLONGBUFFER_H__
