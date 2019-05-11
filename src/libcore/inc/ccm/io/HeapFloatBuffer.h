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

#ifndef __CCM_IO_HEAPFLOATBUFFER_H__
#define __CCM_IO_HEAPFLOATBUFFER_H__

#include "ccm/io/FloatBuffer.h"

namespace ccm {
namespace io {

class HeapFloatBuffer
    : public FloatBuffer
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
        /* [in] */ const Array<Float>& buf,
        /* [in] */ Integer off,
        /* [in] */ Integer len);

    ECode Constructor(
        /* [in] */ const Array<Float>& buf,
        /* [in] */ Integer off,
        /* [in] */ Integer len,
        /* [in] */ Boolean isReadOnly);

    ECode Slice(
        /* [out] */ IFloatBuffer** buffer) override;

    ECode Duplicate(
        /* [out] */ IFloatBuffer** buffer) override;

    ECode AsReadOnlyBuffer(
        /* [out] */ IFloatBuffer** buffer) override;

    ECode Get(
        /* [out] */ Float* f) override;

    ECode Get(
        /* [in] */ Integer index,
        /* [out] */ Float* f) override;

    ECode Get(
        /* [out] */ Array<Float>& dst,
        /* [in] */ Integer offset,
        /* [in] */ Integer length) override;

    ECode IsDirect(
        /* [out] */ Boolean* direct) override;

    ECode IsReadOnly(
        /* [out] */ Boolean* readOnly) override;

    ECode Put(
        /* [in] */ Float f) override;

    ECode Put(
        /* [in] */ Integer index,
        /* [in] */ Float f) override;

    ECode Put(
        /* [in] */ const Array<Float>& src,
        /* [in] */ Integer offset,
        /* [in] */ Integer length) override;

    ECode Put(
        /* [in] */ IFloatBuffer* src) override;

    ECode Compact() override;

    ECode GetOrder(
        /* [out] */ IByteOrder** bo) override;

    ECode GetCoclassID(
        /* [out] */ CoclassID* cid) override;

protected:
    ECode Constructor(
        /* [in] */ const Array<Float>& buf,
        /* [in] */ Integer mark,
        /* [in] */ Integer pos,
        /* [in] */ Integer lim,
        /* [in] */ Integer cap,
        /* [in] */ Integer off);

    ECode Constructor(
        /* [in] */ const Array<Float>& buf,
        /* [in] */ Integer mark,
        /* [in] */ Integer pos,
        /* [in] */ Integer lim,
        /* [in] */ Integer cap,
        /* [in] */ Integer off,
        /* [in] */ Boolean isReadOnly);

    Integer Ix(
        /* [in] */ Integer i);
};

inline Integer HeapFloatBuffer::Ix(
    /* [in] */ Integer i)
{
    return i + mOffset;
}

}
}

#endif // __CCM_IO_HEAPFLOATBUFFER_H__
