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

#ifndef __CCM_IO_INTEGERBUFFER_H__
#define __CCM_IO_INTEGERBUFFER_H__

#include "ccm/io/Buffer.h"
#include "ccm.core.IComparable.h"
#include "ccm.io.IIntegerBuffer.h"

using ccm::core::IComparable;

namespace ccm {
namespace io {

class IntegerBuffer
    : public Buffer
    , public IIntegerBuffer
    , public IComparable
{
public:
    CCM_INTERFACE_DECL();

    ECode Constructor(
        /* [in] */ Integer mark,
        /* [in] */ Integer pos,
        /* [in] */ Integer lim,
        /* [in] */ Integer cap,
        /* [in] */ const Array<Integer>& hb,
        /* [in] */ Integer offset);

    ECode Constructor(
        /* [in] */ Integer mark,
        /* [in] */ Integer pos,
        /* [in] */ Integer lim,
        /* [in] */ Integer cap);

    static ECode Allocate(
        /* [in] */ Integer capacity,
        /* [out] */ IIntegerBuffer** buffer);

    static ECode Wrap(
        /* [in] */ const Array<Integer>& array,
        /* [in] */ Integer offset,
        /* [in] */ Integer length,
        /* [out] */ IIntegerBuffer** buffer);

    static ECode Wrap(
        /* [in] */ const Array<Integer>& array,
        /* [out] */ IIntegerBuffer** buffer);

    ECode Get(
        /* [out] */ Array<Integer>& dst,
        /* [in] */ Integer offset,
        /* [in] */ Integer length) override;

    ECode Get(
        /* [out] */ Array<Integer>& dst) override;

    using IIntegerBuffer::Get;

    ECode Put(
        /* [in] */ IIntegerBuffer* src) override;

    ECode Put(
        /* [in] */ const Array<Integer>& src,
        /* [in] */ Integer offset,
        /* [in] */ Integer length) override;

    ECode Put(
        /* [in] */ const Array<Integer>& src) override final;

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
    Array<Integer> mHb;
    Integer mOffset = 0;
    Boolean mIsReadOnly = false;
};

}
}

#endif // __CCM_IO_INTEGERBUFFER_H__
