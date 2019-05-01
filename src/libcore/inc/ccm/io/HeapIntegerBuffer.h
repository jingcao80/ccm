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

#ifndef __CCM_IO_HEAPINTEGERBUFFER_H__
#define __CCM_IO_HEAPINTEGERBUFFER_H__

#include "ccm/io/IntegerBuffer.h"

namespace ccm {
namespace io {

class HeapIntegerBuffer
    : public IntegerBuffer
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
        /* [in] */ const Array<Integer>& buf,
        /* [in] */ Integer off,
        /* [in] */ Integer len);

    ECode Constructor(
        /* [in] */ const Array<Integer>& buf,
        /* [in] */ Integer off,
        /* [in] */ Integer len,
        /* [in] */ Boolean isReadOnly);

    ECode Slice(
        /* [out] */ IIntegerBuffer** buffer) override;

    ECode Duplicate(
        /* [out] */ IIntegerBuffer** buffer) override;

    ECode AsReadOnlyBuffer(
        /* [out] */ IIntegerBuffer** buffer) override;

    ECode Get(
        /* [out] */ Integer* i) override;

    ECode Get(
        /* [in] */ Integer index,
        /* [out] */ Integer* i) override;

    ECode Get(
        /* [out] */ Array<Integer>& dst,
        /* [in] */ Integer offset,
        /* [in] */ Integer length) override;

    ECode IsDirect(
        /* [out] */ Boolean* direct) override;

    ECode IsReadOnly(
        /* [out] */ Boolean* readOnly) override;

    ECode Put(
        /* [in] */ Integer i) override;

    ECode Put(
        /* [in] */ Integer index,
        /* [in] */ Integer i) override;

    ECode Put(
        /* [in] */ const Array<Integer>& src,
        /* [in] */ Integer offset,
        /* [in] */ Integer length) override;

    ECode Compact() override;

    ECode GetOrder(
        /* [out] */ IByteOrder** bo) override;

protected:
    ECode Constructor(
        /* [in] */ const Array<Integer>& buf,
        /* [in] */ Integer mark,
        /* [in] */ Integer pos,
        /* [in] */ Integer lim,
        /* [in] */ Integer cap,
        /* [in] */ Integer off);

    ECode Constructor(
        /* [in] */ const Array<Integer>& buf,
        /* [in] */ Integer mark,
        /* [in] */ Integer pos,
        /* [in] */ Integer lim,
        /* [in] */ Integer cap,
        /* [in] */ Integer off,
        /* [in] */ Boolean isReadOnly);

    Integer Ix(
        /* [in] */ Integer i);
};

inline Integer HeapIntegerBuffer::Ix(
    /* [in] */ Integer i)
{
    return i + mOffset;
}

}
}

#endif // __CCM_IO_HEAPINTEGERBUFFER_H__
