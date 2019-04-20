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
        /* [out, callee] */ Array<Byte>* array) override final;

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

    ECode IsAccessible(
        /* [out] */ Boolean* accessible) override;

    ECode SetAccessible(
        /* [in] */ Boolean value) override;

protected:
    virtual ECode GetCharUnchecked(
        /* [in] */ Integer index,
        /* [out] */ Char* c);

    virtual ECode GetUnchecked(
        /* [in] */ Integer pos,
        /* [out] */ Array<Char>& dst,
        /* [in] */ Integer dstOffset,
        /* [in] */ Integer length);

    virtual ECode PutCharUnchecked(
        /* [in] */ Integer index,
        /* [in] */ Char value);

    virtual ECode PutUnchecked(
        /* [in] */ Integer pos,
        /* [in] */ const Array<Char>& src,
        /* [in] */ Integer srcOffset,
        /* [in] */ Integer length);

    virtual ECode GetShortUnchecked(
        /* [in] */ Integer index,
        /* [out] */ Short* s);

    virtual ECode GetUnchecked(
        /* [in] */ Integer pos,
        /* [out] */ Array<Short>& dst,
        /* [in] */ Integer dstOffset,
        /* [in] */ Integer length);

    virtual ECode PutShortUnchecked(
        /* [in] */ Integer index,
        /* [in] */ Short value);

    virtual ECode PutUnchecked(
        /* [in] */ Integer pos,
        /* [in] */ const Array<Short>& src,
        /* [in] */ Integer srcOffset,
        /* [in] */ Integer length);

    virtual ECode GetIntegerUnchecked(
        /* [in] */ Integer index,
        /* [out] */ Integer* i);

    virtual ECode GetUnchecked(
        /* [in] */ Integer pos,
        /* [out] */ Array<Integer>& dst,
        /* [in] */ Integer dstOffset,
        /* [in] */ Integer length);

    virtual ECode PutIntegerUnchecked(
        /* [in] */ Integer index,
        /* [in] */ Integer value);

    virtual ECode PutUnchecked(
        /* [in] */ Integer pos,
        /* [in] */ const Array<Integer>& src,
        /* [in] */ Integer srcOffset,
        /* [in] */ Integer length);

    virtual ECode GetLongUnchecked(
        /* [in] */ Integer index,
        /* [out] */ Long* l);

    virtual ECode GetUnchecked(
        /* [in] */ Integer pos,
        /* [out] */ Array<Long>& dst,
        /* [in] */ Integer dstOffset,
        /* [in] */ Integer length);

    virtual ECode PutLongUnchecked(
        /* [in] */ Integer index,
        /* [in] */ Long value);

    virtual ECode PutUnchecked(
        /* [in] */ Integer pos,
        /* [in] */ const Array<Long>& src,
        /* [in] */ Integer srcOffset,
        /* [in] */ Integer length);

    virtual ECode GetFloatUnchecked(
        /* [in] */ Integer index,
        /* [out] */ Float* f);

    virtual ECode GetUnchecked(
        /* [in] */ Integer pos,
        /* [out] */ Array<Float>& dst,
        /* [in] */ Integer dstOffset,
        /* [in] */ Integer length);

    virtual ECode PutFloatUnchecked(
        /* [in] */ Integer index,
        /* [in] */ Float value);

    virtual ECode PutUnchecked(
        /* [in] */ Integer pos,
        /* [in] */ const Array<Float>& src,
        /* [in] */ Integer srcOffset,
        /* [in] */ Integer length);

    virtual ECode GetDoubleUnchecked(
        /* [in] */ Integer index,
        /* [out] */ Double* d);

    virtual ECode GetUnchecked(
        /* [in] */ Integer pos,
        /* [out] */ Array<Double>& dst,
        /* [in] */ Integer dstOffset,
        /* [in] */ Integer length);

    virtual ECode PutDoubleUnchecked(
        /* [in] */ Integer index,
        /* [in] */ Double value);

    virtual ECode PutUnchecked(
        /* [in] */ Integer pos,
        /* [in] */ const Array<Double>& dst,
        /* [in] */ Integer srcOffset,
        /* [in] */ Integer length);

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
