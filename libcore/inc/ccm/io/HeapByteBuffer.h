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

#ifndef __CCM_IO_HEAPBYTEBUFFER_H__
#define __CCM_IO_HEAPBYTEBUFFER_H__

#include "ccm/io/ByteBuffer.h"

namespace ccm {
namespace io {

class HeapByteBuffer
    : public ByteBuffer
{
public:
    ECode Constructor(
        /* [in] */ Integer cap,
        /* [in] */ Integer lim);

    ECode Constructor(
        /* [in] */ const Array<Byte>& buf,
        /* [in] */ Integer off,
        /* [in] */ Integer len);

    ECode Slice(
        /* [out] */ IByteBuffer** buffer) override;

    ECode Duplicate(
        /* [out] */ IByteBuffer** buffer) override;

    ECode AsReadOnlyBuffer(
        /* [out] */ IByteBuffer** buffer) override;

    ECode Get(
        /* [out] */ Byte* b) override;

    ECode Get(
        /* [in] */ Integer index,
        /* [out] */ Byte* b) override;

    ECode Get(
        /* [out] */ Array<Byte>& dst,
        /* [in] */ Integer offset,
        /* [in] */ Integer length) override;

    ECode IsDirect(
        /* [out] */ Boolean* direct) override;

    ECode IsReadOnly(
        /* [out] */ Boolean* readOnly) override;

    ECode Put(
        /* [in] */ Byte b) override;

    ECode Put(
        /* [in] */ Integer index,
        /* [in] */ Byte b) override;

    ECode Put(
        /* [in] */ const Array<Byte>& src,
        /* [in] */ Integer offset,
        /* [in] */ Integer length) override;

    ECode Compact() override;

    Byte _Get(
        /* [in] */ Integer i) override;

    ECode _Put(
        /* [in] */ Integer i,
        /* [in] */ Byte b) override;

    ECode GetChar(
        /* [out] */ Char* c) override;

    ECode GetChar(
        /* [in] */ Integer index,
        /* [out] */ Char* c) override;

    ECode GetCharUnchecked(
        /* [in] */ Integer index,
        /* [out] */ Char* c) override;

    ECode GetUnchecked(
        /* [in] */ Integer pos,
        /* [out] */ Array<Char>& dst,
        /* [in] */ Integer dstOffset,
        /* [in] */ Integer length) override;

    ECode PutChar(
        /* [in] */ Char value) override;

    ECode PutChar(
        /* [in] */ Integer index,
        /* [in] */ Char value) override;

    ECode PutCharUnchecked(
        /* [in] */ Integer index,
        /* [in] */ Char value) override;

    ECode PutUnchecked(
        /* [in] */ Integer pos,
        /* [in] */ const Array<Char>& src,
        /* [in] */ Integer srcOffset,
        /* [in] */ Integer length) override;

    ECode AsCharBuffer(
        /* [out] */ ICharBuffer** buffer) override;

    ECode GetShort(
        /* [out] */ Short* s) override;

    ECode GetShort(
        /* [in] */ Integer index,
        /* [out] */ Short* s) override;

    ECode GetShortUnchecked(
        /* [in] */ Integer index,
        /* [in] */ Short* value) override;

    ECode GetUnchecked(
        /* [in] */ Integer pos,
        /* [out] */ Array<Short>& dst,
        /* [in] */ Integer dstOffset,
        /* [in] */ Integer length) override;

    ECode PutShort(
        /* [in] */ Short value) override;

    ECode PutShort(
        /* [in] */ Integer index,
        /* [in] */ Short value) override;

    ECode PutShortUnchecked(
        /* [in] */ Integer index,
        /* [in] */ Short value) override;

    ECode PutUnchecked(
        /* [in] */ Integer pos,
        /* [in] */ const Array<Short>& src,
        /* [in] */ Integer srcOffset,
        /* [in] */ Integer length) override;

    ECode AsShortBuffer(
        /* [out] */ IShortBuffer** buffer) override;

    ECode GetInteger(
        /* [out] */ Integer* i) override;

    ECode GetInteger(
        /* [in] */ Integer index,
        /* [out] */ Integer* i) override;

    ECode GetIntegerUnchecked(
        /* [in] */ Integer index,
        /* [out] */ Integer* value) override;

    ECode GetUnchecked(
        /* [in] */ Integer pos,
        /* [out] */ Array<Integer>& dst,
        /* [in] */ Integer dstOffset,
        /* [in] */ Integer length) override;

    ECode PutInteger(
        /* [in] */ Integer value) override;

    ECode PutInteger(
        /* [in] */ Integer index,
        /* [in] */ Integer value) override;

    ECode PutIntegerUnchecked(
        /* [in] */ Integer index,
        /* [in] */ Integer value) override;

    ECode PutUnchecked(
        /* [in] */ Integer pos,
        /* [in] */ const Array<Integer>& src,
        /* [in] */ Integer srcOffset,
        /* [in] */ Integer length) override;

    ECode AsIntegerBuffer(
        /* [out] */ IIntegerBuffer** buffer) override;

    ECode GetLong(
        /* [out] */ Long* l) override;

    ECode GetLong(
        /* [in] */ Integer index,
        /* [out] */ Long* l) override;

    ECode GetLongUnchecked(
        /* [in] */ Integer index,
        /* [out] */ Long* value) override;

    ECode GetUnchecked(
        /* [in] */ Integer pos,
        /* [out] */ Array<Long>& dst,
        /* [in] */ Integer dstOffset,
        /* [in] */ Integer length) override;

    ECode PutLong(
        /* [in] */ Long value) override;

    ECode PutLong(
        /* [in] */ Integer index,
        /* [in] */ Long value) override;

    ECode PutLongUnchecked(
        /* [in] */ Integer index,
        /* [in] */ Long value) override;

    ECode PutUnchecked(
        /* [in] */ Integer pos,
        /* [in] */ const Array<Long>& src,
        /* [in] */ Integer srcOffset,
        /* [in] */ Integer length) override;

    ECode AsLongBuffer(
        /* [out] */ ILongBuffer** buffer) override;

    ECode GetFloat(
        /* [out] */ Float* f) override;

    ECode GetFloat(
        /* [in] */ Integer index,
        /* [out] */ Float* f) override;

    ECode GetFloatUnchecked(
        /* [in] */ Integer index,
        /* [out] */ Float* value) override;

    ECode GetUnchecked(
        /* [in] */ Integer pos,
        /* [out] */ Array<Float>& dst,
        /* [in] */ Integer dstOffset,
        /* [in] */ Integer length) override;

    ECode PutFloat(
        /* [in] */ Float value) override;

    ECode PutFloat(
        /* [in] */ Integer index,
        /* [in] */ Float value) override;

    ECode PutFloatUnchecked(
        /* [in] */ Integer index,
        /* [in] */ Float value) override;

    ECode PutUnchecked(
        /* [in] */ Integer pos,
        /* [in] */ const Array<Float>& src,
        /* [in] */ Integer srcOffset,
        /* [in] */ Integer length) override;

    ECode AsFloatBuffer(
        /* [out] */ IFloatBuffer** buffer) override;

    ECode GetDouble(
        /* [out] */ Double* d) override;

    ECode GetDouble(
        /* [in] */ Integer index,
        /* [out] */ Double* d) override;

    ECode GetDoubleUnchecked(
        /* [in] */ Integer index,
        /* [out] */ Double* value) override;

    ECode GetUnchecked(
        /* [in] */ Integer pos,
        /* [out] */ Array<Double>& dst,
        /* [in] */ Integer dstOffset,
        /* [in] */ Integer length) override;

    ECode PutDouble(
        /* [in] */ Double value) override;

    ECode PutDouble(
        /* [in] */ Integer index,
        /* [in] */ Double value) override;

    ECode PutDoubleUnchecked(
        /* [in] */ Integer index,
        /* [in] */ Double value) override;

    ECode PutUnchecked(
        /* [in] */ Integer pos,
        /* [in] */ const Array<Double>& src,
        /* [in] */ Integer srcOffset,
        /* [in] */ Integer length) override;

    ECode AsDoubleBuffer(
        /* [out] */ IDoubleBuffer** buffer) override;

private:
    ECode Constructor(
        /* [in] */ Integer cap,
        /* [in] */ Integer lim,
        /* [in] */ Boolean isReadOnly);

    ECode Constructor(
        /* [in] */ const Array<Byte>& buf,
        /* [in] */ Integer off,
        /* [in] */ Integer len,
        /* [in] */ Boolean isReadOnly);

    ECode Constructor(
        /* [in] */ const Array<Byte>& buf,
        /* [in] */ Integer mark,
        /* [in] */ Integer pos,
        /* [in] */ Integer lim,
        /* [in] */ Integer cap,
        /* [in] */ Integer off,
        /* [in] */ Boolean isReadOnly);

    Integer Ix(
        /* [in] */ Integer i);
};

inline Integer HeapByteBuffer::Ix(
    /* [in] */ Integer i)
{
    return i + mOffset;
}

}
}

#endif // __CCM_IO_HEAPBYTEBUFFER_H__
