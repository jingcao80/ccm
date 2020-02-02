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

#ifndef __COMO_IO_SHORTBUFFER_H__
#define __COMO_IO_SHORTBUFFER_H__

#include "como/io/Buffer.h"
#include "como.core.IComparable.h"
#include "como.io.IShortBuffer.h"

using como::core::IComparable;

namespace como {
namespace io {

class ShortBuffer
    : public Buffer
    , public IShortBuffer
    , public IComparable
{
public:
    COMO_INTERFACE_DECL();

    ECode Constructor(
        /* [in] */ Integer mark,
        /* [in] */ Integer pos,
        /* [in] */ Integer lim,
        /* [in] */ Integer cap,
        /* [in] */ const Array<Short>& hb,
        /* [in] */ Integer offset);

    ECode Constructor(
        /* [in] */ Integer mark,
        /* [in] */ Integer pos,
        /* [in] */ Integer lim,
        /* [in] */ Integer cap);

    static ECode Allocate(
        /* [in] */ Integer capacity,
        /* [out] */ IShortBuffer** buffer);

    static ECode Wrap(
        /* [in] */ const Array<Short>& array,
        /* [in] */ Integer offset,
        /* [in] */ Integer length,
        /* [out] */ IShortBuffer** buffer);

    static ECode Wrap(
        /* [in] */ const Array<Short>& array,
        /* [out] */ IShortBuffer** buffer);

    ECode Get(
        /* [out] */ Array<Short>& dst,
        /* [in] */ Integer offset,
        /* [in] */ Integer length) override;

    ECode Get(
        /* [out] */ Array<Short>& dst) override;

    using IShortBuffer::Get;

    ECode Put(
        /* [in] */ IShortBuffer* src) override;

    ECode Put(
        /* [in] */ const Array<Short>& src,
        /* [in] */ Integer offset,
        /* [in] */ Integer length) override;

    ECode Put(
        /* [in] */ const Array<Short>& src) override final;

    ECode HasArray(
        /* [out] */ Boolean* result) override final;

    ECode GetArray(
        /* [out] */ IInterface** array) override final;

    ECode GetArrayOffset(
        /* [out] */ Integer* offset) override final;

    ECode ToString(
        /* [out] */ String& desc) override;

    ECode GetHashCode(
        /* [out] */ Integer& hash) override;

    ECode Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean& same) override;

    ECode CompareTo(
        /* [in] */ IInterface* other,
        /* [out] */ Integer& result) override;

protected:
    Array<Short> mHb;
    Integer mOffset;
    Boolean mIsReadOnly;
};

}
}

#endif // __COMO_IO_SHORTBUFFER_H__
