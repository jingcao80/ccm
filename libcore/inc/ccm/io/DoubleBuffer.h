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

#ifndef __CCM_IO_DOUBLEBUFFER_H__
#define __CCM_IO_DOUBLEBUFFER_H__

#include "ccm/io/Buffer.h"
#include "ccm.core.IComparable.h"
#include "ccm.io.IDoubleBuffer.h"

using ccm::core::IComparable;

namespace ccm {
namespace io {

class DoubleBuffer
    : public Buffer
    , public IDoubleBuffer
    , public IComparable
{
public:
    CCM_INTERFACE_DECL();

    ECode Constructor(
        /* [in] */ Integer mark,
        /* [in] */ Integer pos,
        /* [in] */ Integer lim,
        /* [in] */ Integer cap,
        /* [in] */ const Array<Double>& hb,
        /* [in] */ Integer offset);

    ECode Constructor(
        /* [in] */ Integer mark,
        /* [in] */ Integer pos,
        /* [in] */ Integer lim,
        /* [in] */ Integer cap);

    static ECode Allocate(
        /* [in] */ Integer capacity,
        /* [out] */ IDoubleBuffer** buffer);

    static ECode Wrap(
        /* [in] */ const Array<Double>& array,
        /* [in] */ Integer offset,
        /* [in] */ Integer length,
        /* [out] */ IDoubleBuffer** buffer);

    static ECode Wrap(
        /* [in] */ const Array<Double>& array,
        /* [out] */ IDoubleBuffer** buffer);

    ECode Get(
        /* [out] */ Array<Double>& dst,
        /* [in] */ Integer offset,
        /* [in] */ Integer length) override;

    ECode Get(
        /* [out] */ Array<Double>& dst) override;

    using IDoubleBuffer::Get;

    ECode Put(
        /* [in] */ IDoubleBuffer* src) override;

    ECode Put(
        /* [in] */ const Array<Double>& src,
        /* [in] */ Integer offset,
        /* [in] */ Integer length) override;

    ECode Put(
        /* [in] */ const Array<Double>& src) override final;

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

private:
    static Boolean Equals(
        /* [in] */ Double x,
        /* [in] */ Double y);

protected:
    Array<Double> mHb;
    Integer mOffset = 0;
    Boolean mIsReadOnly = false;
};

}
}


#endif // __CCM_IO_DOUBLEBUFFER_H__
