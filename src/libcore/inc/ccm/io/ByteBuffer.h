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

#ifndef __CCM_IO_BYTEBUFFER_H__
#define __CCM_IO_BYTEBUFFER_H__

#include "ccm/io/Buffer.h"
#include "ccm.core.IComparable.h"
#include "ccm.io.IByteBuffer.h"

using ccm::core::IComparable;

namespace ccm {
namespace io {

class ByteBuffer
    : public Buffer
    , public IByteBuffer
    , public IComparable
{
public:
    ByteBuffer();

    CCM_INTERFACE_DECL();

    ECode Constructor(
        /* [in] */ Integer mark,
        /* [in] */ Integer pos,
        /* [in] */ Integer lim,
        /* [in] */ Integer cap,
        /* [in] */ Array<Byte>& hb,
        /* [in] */ Integer offset);

    ECode Constructor(
        /* [in] */ Integer mark,
        /* [in] */ Integer pos,
        /* [in] */ Integer lim,
        /* [in] */ Integer cap);

    static ECode AllocateDirect(
        /* [in] */ Integer capacity,
        /* [out] */ IByteBuffer** buffer);

    static ECode Allocate(
        /* [in] */ Integer capacity,
        /* [out] */ IByteBuffer** buffer);

    static ECode Wrap(
        /* [in] */ Array<Byte>& array,
        /* [in] */ Integer offset,
        /* [in] */ Integer length,
        /* [out] */ IByteBuffer** buffer);

    static ECode Wrap(
        /* [in] */ Array<Byte>& array,
        /* [out] */ IByteBuffer** buffer);

    ECode Get(
        /* [out] */ Array<Byte>& dst,
        /* [in] */ Integer offset,
        /* [in] */ Integer length) override;

    ECode Get(
        /* [out] */ Array<Byte>& dst) override;

    using IByteBuffer::Get;

    ECode Put(
        /* [in] */ IByteBuffer* src) override;

    ECode Put(
        /* [in] */ const Array<Byte>& src,
        /* [in] */ Integer offset,
        /* [in] */ Integer length) override;

    ECode Put(
        /* [in] */ const Array<Byte>& src) override final;

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

    ECode GetOrder(
        /* [out] */ IByteOrder** bo) override final;

    ECode SetOrder(
        /* [in] */ IByteOrder* bo) override final;

    virtual Byte _Get(
        /* [in] */ Integer i) = 0;

    virtual ECode _Put(
        /* [in] */ Integer i,
        /* [in] */ Byte b) = 0;

    ECode IsAccessible(
        /* [out] */ Boolean* accessible) override;

    ECode SetAccessible(
        /* [in] */ Boolean value) override;

protected:
    virtual Char GetCharUnchecked(
        /* [in] */ Integer index) = 0;

    virtual void GetUnchecked(
        /* [in] */ Integer pos,
        /* [out] */ Array<Char>& dst,
        /* [in] */ Integer dstOffset,
        /* [in] */ Integer length) = 0;

    virtual void PutCharUnchecked(
        /* [in] */ Integer index,
        /* [in] */ Char value) = 0;

    virtual void PutUnchecked(
        /* [in] */ Integer pos,
        /* [in] */ const Array<Char>& src,
        /* [in] */ Integer srcOffset,
        /* [in] */ Integer length) = 0;

    virtual Short GetShortUnchecked(
        /* [in] */ Integer index) = 0;

    virtual void GetUnchecked(
        /* [in] */ Integer pos,
        /* [out] */ Array<Short>& dst,
        /* [in] */ Integer dstOffset,
        /* [in] */ Integer length) = 0;

    virtual void PutShortUnchecked(
        /* [in] */ Integer index,
        /* [in] */ Short value) = 0;

    virtual void PutUnchecked(
        /* [in] */ Integer pos,
        /* [in] */ const Array<Short>& src,
        /* [in] */ Integer srcOffset,
        /* [in] */ Integer length) = 0;

    virtual Integer GetIntegerUnchecked(
        /* [in] */ Integer index) = 0;

    virtual void GetUnchecked(
        /* [in] */ Integer pos,
        /* [out] */ Array<Integer>& dst,
        /* [in] */ Integer dstOffset,
        /* [in] */ Integer length) = 0;

    virtual void PutIntegerUnchecked(
        /* [in] */ Integer index,
        /* [in] */ Integer value) = 0;

    virtual void PutUnchecked(
        /* [in] */ Integer pos,
        /* [in] */ const Array<Integer>& src,
        /* [in] */ Integer srcOffset,
        /* [in] */ Integer length) = 0;

    virtual Long GetLongUnchecked(
        /* [in] */ Integer index) = 0;

    virtual void GetUnchecked(
        /* [in] */ Integer pos,
        /* [out] */ Array<Long>& dst,
        /* [in] */ Integer dstOffset,
        /* [in] */ Integer length) = 0;

    virtual void PutLongUnchecked(
        /* [in] */ Integer index,
        /* [in] */ Long value) = 0;

    virtual void PutUnchecked(
        /* [in] */ Integer pos,
        /* [in] */ const Array<Long>& src,
        /* [in] */ Integer srcOffset,
        /* [in] */ Integer length) = 0;

    virtual Float GetFloatUnchecked(
        /* [in] */ Integer index) = 0;

    virtual void GetUnchecked(
        /* [in] */ Integer pos,
        /* [out] */ Array<Float>& dst,
        /* [in] */ Integer dstOffset,
        /* [in] */ Integer length) = 0;

    virtual void PutFloatUnchecked(
        /* [in] */ Integer index,
        /* [in] */ Float value) = 0;

    virtual void PutUnchecked(
        /* [in] */ Integer pos,
        /* [in] */ const Array<Float>& src,
        /* [in] */ Integer srcOffset,
        /* [in] */ Integer length) = 0;

    virtual Double GetDoubleUnchecked(
        /* [in] */ Integer index) = 0;

    virtual void GetUnchecked(
        /* [in] */ Integer pos,
        /* [out] */ Array<Double>& dst,
        /* [in] */ Integer dstOffset,
        /* [in] */ Integer length) = 0;

    virtual void PutDoubleUnchecked(
        /* [in] */ Integer index,
        /* [in] */ Double value) = 0;

    virtual void PutUnchecked(
        /* [in] */ Integer pos,
        /* [in] */ const Array<Double>& src,
        /* [in] */ Integer srcOffset,
        /* [in] */ Integer length) = 0;

protected:
    Array<Byte> mHb;
    Integer mOffset = 0;
    Boolean mIsReadOnly = false;

    Boolean mBigEndian = true;
    Boolean mNativeByteOrder;
};

}
}

#endif // __CCM_IO_BYTEBUFFER_H__
