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

#ifndef __COMO_IO_HEAPDOUBLEBUFFER_H__
#define __COMO_IO_HEAPDOUBLEBUFFER_H__

#include "como/io/DoubleBuffer.h"

namespace como {
namespace io {

class HeapDoubleBuffer
    : public DoubleBuffer
{
public:
    ECode Constructor(
        /* [in] */ Integer cap,
        /* [in] */ Integer lim);

    ECode Constructor(
        /* [in] */ const Array<Double>& buf,
        /* [in] */ Integer off,
        /* [in] */ Integer len);

    ECode Constructor(
        /* [in] */ Integer cap,
        /* [in] */ Integer lim,
        /* [in] */ Boolean isReadOnly);

    ECode Constructor(
        /* [in] */ const Array<Double>& buf,
        /* [in] */ Integer off,
        /* [in] */ Integer len,
        /* [in] */ Boolean isReadOnly);

    ECode Slice(
        /* [out] */ AutoPtr<IDoubleBuffer>& buffer) override;

    ECode Duplicate(
        /* [out] */ AutoPtr<IDoubleBuffer>& buffer) override;

    ECode AsReadOnlyBuffer(
        /* [out] */ AutoPtr<IDoubleBuffer>& buffer) override;

    ECode Get(
        /* [out] */ Double& d) override;

    ECode Get(
        /* [in] */ Integer index,
        /* [out] */ Double& d) override;

    ECode Get(
        /* [out] */ Array<Double>& dst,
        /* [in] */ Integer offset,
        /* [in] */ Integer length) override;

    ECode IsDirect(
        /* [out] */ Boolean& direct) override;

    ECode IsReadOnly(
        /* [out] */ Boolean& readOnly) override;

    ECode Put(
        /* [in] */ Double d) override;

    ECode Put(
        /* [in] */ Integer index,
        /* [in] */ Double d) override;

    ECode Put(
        /* [in] */ const Array<Double>& src,
        /* [in] */ Integer offset,
        /* [in] */ Integer length) override;

    ECode Put(
        /* [in] */ IDoubleBuffer* src) override;

    ECode Compact() override;

    ECode GetOrder(
        /* [out] */ AutoPtr<IByteOrder>& bo) override;

    ECode GetCoclassID(
        /* [out] */ CoclassID& cid) override;

protected:
    ECode Constructor(
        /* [in] */ const Array<Double>& buf,
        /* [in] */ Integer mark,
        /* [in] */ Integer pos,
        /* [in] */ Integer lim,
        /* [in] */ Integer cap,
        /* [in] */ Integer off);

    ECode Constructor(
        /* [in] */ const Array<Double>& buf,
        /* [in] */ Integer mark,
        /* [in] */ Integer pos,
        /* [in] */ Integer lim,
        /* [in] */ Integer cap,
        /* [in] */ Integer off,
        /* [in] */ Boolean isReadOnly);

    Integer Ix(
        /* [in] */ Integer i);
};

inline Integer HeapDoubleBuffer::Ix(
    /* [in] */ Integer i)
{
    return i + mOffset;
}

}
}

#endif // __COMO_IO_HEAPDOUBLEBUFFER_H__
