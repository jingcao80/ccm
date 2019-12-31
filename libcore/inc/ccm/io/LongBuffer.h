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

#ifndef __CCM_IO_LONGBUFFER_H__
#define __CCM_IO_LONGBUFFER_H__

#include "ccm/io/Buffer.h"
#include "ccm.core.IComparable.h"
#include "ccm.io.ILongBuffer.h"

using ccm::core::IComparable;

namespace ccm {
namespace io {

class LongBuffer
    : public Buffer
    , public ILongBuffer
    , public IComparable
{
public:
    CCM_INTERFACE_DECL();

    ECode Constructor(
        /* [in] */ Integer mark,
        /* [in] */ Integer pos,
        /* [in] */ Integer lim,
        /* [in] */ Integer cap,
        /* [in] */ const Array<Long>& hb,
        /* [in] */ Integer offset);

    ECode Constructor(
        /* [in] */ Integer mark,
        /* [in] */ Integer pos,
        /* [in] */ Integer lim,
        /* [in] */ Integer cap);

    static ECode Allocate(
        /* [in] */ Integer capacity,
        /* [out] */ ILongBuffer** buffer);

    static ECode Wrap(
        /* [in] */ const Array<Long>& array,
        /* [in] */ Integer offset,
        /* [in] */ Integer length,
        /* [out] */ ILongBuffer** buffer);

    static ECode Wrap(
        /* [in] */ const Array<Long>& array,
        /* [out] */ ILongBuffer** buffer);

    ECode Get(
        /* [out] */ Array<Long>& dst,
        /* [in] */ Integer offset,
        /* [in] */ Integer length) override;

    ECode Get(
        /* [out] */ Array<Long>& dst) override;

    using ILongBuffer::Get;

    ECode Put(
        /* [in] */ ILongBuffer* src) override;

    ECode Put(
        /* [in] */ const Array<Long>& src,
        /* [in] */ Integer offset,
        /* [in] */ Integer length) override;

    ECode Put(
        /* [in] */ const Array<Long>& src) override final;

    ECode HasArray(
        /* [out] */ Boolean* result) override final;

    ECode GetArray(
        /* [out] */ IInterface** array) override final;

    ECode GetArrayOffset(
        /* [out] */ Integer* offset) override final;

    ECode ToString(
        /* [out] */ String* desc) override;

    ECode GetHashCode(
        /* [out] */ Integer* hash) override;

    ECode Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* same) override;

    ECode CompareTo(
        /* [in] */ IInterface* other,
        /* [out] */ Integer* result) override;

protected:
    Array<Long> mHb;
    Integer mOffset = 0;
    Boolean mIsReadOnly = false;
};

}
}

#endif // __CCM_IO_LONGBUFFER_H__
