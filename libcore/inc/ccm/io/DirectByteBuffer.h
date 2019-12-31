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

#ifndef __CCM_IO_DIRECTBYTEBUFFER_H__
#define __CCM_IO_DIRECTBYTEBUFFER_H__

#include "ccm/io/MappedByteBuffer.h"
#include "ccm.io.channels.IDirectBuffer.h"

using ccm::io::channels::IDirectBuffer;

namespace ccm {
namespace io {

class DirectByteBuffer
    : public MappedByteBuffer
    , public IDirectBuffer
{
public:
    class MemoryRef
        : public Object
    {
    public:
        MemoryRef(
            /* [in] */ Integer capacity);

        MemoryRef(
            /* [in] */ HANDLE allocatedAddress);

        void Free();

    public:
        Array<Byte> mBuffer;
        HANDLE mAllocatedAddress;
        Integer mOffset;
        Boolean mIsAccessible;
        Boolean mIsFreed;
    };

public:
    CCM_INTERFACE_DECL();

    ECode Constructor(
        /* [in] */ Integer cap,
        /* [in] */ MemoryRef* memoryRef);

    ECode Constructor(
        /* [in] */ Integer cap,
        /* [in] */ HANDLE addr,
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Boolean isReadOnly);

    ECode Constructor(
        /* [in] */ MemoryRef* memoryRef,
        /* [in] */ Integer mark,
        /* [in] */ Integer pos,
        /* [in] */ Integer lim,
        /* [in] */ Integer cap,
        /* [in] */ Integer off);

    ECode Constructor(
        /* [in] */ MemoryRef* memoryRef,
        /* [in] */ Integer mark,
        /* [in] */ Integer pos,
        /* [in] */ Integer lim,
        /* [in] */ Integer cap,
        /* [in] */ Integer off,
        /* [in] */ Boolean isReadOnly);

    ECode GetAttachment(
        /* [out] */ IInterface** attachment) override final;

    ECode Slice(
        /* [out] */ IByteBuffer** buffer) override final;

    ECode Duplicate(
        /* [out] */ IByteBuffer** buffer) override final;

    ECode AsReadOnlyBuffer(
        /* [out] */ IByteBuffer** buffer) override final;

    ECode GetAddress(
        /* [out] */ HANDLE* addr) override final;

    ECode Get(
        /* [out] */ Byte* b) override final;

    ECode Get(
        /* [in] */ Integer index,
        /* [out] */ Byte* b) override final;

    ECode Get(
        /* [out] */ Array<Byte>& dst,
        /* [in] */ Integer offset,
        /* [in] */ Integer length) override final;

    ECode Put(
        /* [in] */ Byte b) override final;

    ECode Put(
        /* [in] */ Integer index,
        /* [in] */ Byte b) override final;

    ECode Put(
        /* [in] */ const Array<Byte>& src,
        /* [in] */ Integer offset,
        /* [in] */ Integer length) override final;

    ECode Compact() override final;

    ECode IsDirect(
        /* [out] */ Boolean* direct) override final;

    ECode IsReadOnly(
        /* [out] */ Boolean* readOnly) override final;

    Byte _Get(
        /* [in] */ Integer i) override final;

    ECode _Put(
        /* [in] */ Integer i,
        /* [in] */ Byte b) override final;

    ECode GetChar(
        /* [out] */ Char* c) override final;

    ECode GetChar(
        /* [in] */ Integer index,
        /* [out] */ Char* c) override final;

    ECode GetCharUnchecked(
        /* [in] */ Integer index,
        /* [out] */ Char* c) override final;

    ECode GetUnchecked(
        /* [in] */ Integer pos,
        /* [out] */ Array<Char>& dst,
        /* [in] */ Integer dstOffset,
        /* [in] */ Integer length) override final;

    ECode PutChar(
        /* [in] */ Char value) override final;

    ECode PutChar(
        /* [in] */ Integer index,
        /* [in] */ Char value) override final;

    ECode PutCharUnchecked(
        /* [in] */ Integer index,
        /* [in] */ Char value) override final;

    ECode PutUnchecked(
        /* [in] */ Integer pos,
        /* [in] */ const Array<Char>& src,
        /* [in] */ Integer srcOffset,
        /* [in] */ Integer length) override final;

    ECode AsCharBuffer(
        /* [out] */ ICharBuffer** buffer) override final;

    ECode GetShort(
        /* [out] */ Short* s) override final;

    ECode GetShort(
        /* [in] */ Integer index,
        /* [out] */ Short* s) override final;

    ECode GetShortUnchecked(
        /* [in] */ Integer index,
        /* [out] */ Short* value) override final;

    ECode GetUnchecked(
        /* [in] */ Integer pos,
        /* [out] */ Array<Short>& dst,
        /* [in] */ Integer dstOffset,
        /* [in] */ Integer length) override final;

    ECode PutShort(
        /* in] */ Short value) override final;

    ECode PutShort(
        /* [in] */ Integer index,
        /* [in] */ Short value) override final;

    ECode PutShortUnchecked(
        /* [in] */ Integer index,
        /* [in] */ Short value) override final;

    ECode PutUnchecked(
        /* [in] */ Integer pos,
        /* [in] */ const Array<Short>& src,
        /* [in] */ Integer srcOffset,
        /* [in] */ Integer length) override final;

    ECode AsShortBuffer(
        /* [out] */ IShortBuffer** buffer) override final;

    ECode GetInteger(
        /* [out] */ Integer* i) override final;

    ECode GetInteger(
        /* [in] */ Integer index,
        /* [out] */ Integer* i) override final;

    ECode GetIntegerUnchecked(
        /* [in] */ Integer index,
        /* [out] */ Integer* value) override final;

    ECode GetUnchecked(
        /* [in] */ Integer pos,
        /* [out] */ Array<Integer>& dst,
        /* [in] */ Integer dstOffset,
        /* [in] */ Integer length) override final;

    ECode PutInteger(
        /* [in] */ Integer value) override final;

    ECode PutInteger(
        /* [in] */ Integer index,
        /* [in] */ Integer value) override final;

    ECode PutIntegerUnchecked(
        /* [in] */ Integer index,
        /* [in] */ Integer value) override final;

    ECode PutUnchecked(
        /* [in] */ Integer pos,
        /* [in] */ const Array<Integer>& src,
        /* [in] */ Integer srcOffset,
        /* [in] */ Integer length) override final;

    ECode AsIntegerBuffer(
        /* [out] */ IIntegerBuffer** buffer) override final;

    ECode GetLong(
        /* [out] */ Long* l) override final;

    ECode GetLong(
        /* [in] */ Integer index,
        /* [out] */ Long* l) override final;

    ECode GetLongUnchecked(
        /* [in] */ Integer index,
        /* [out] */ Long* value) override final;

    ECode GetUnchecked(
        /* [in] */ Integer pos,
        /* [out] */ Array<Long>& dst,
        /* [in] */ Integer dstOffset,
        /* [in] */ Integer length) override final;

    ECode PutLong(
        /* [in] */ Long value) override final;

    ECode PutLong(
        /* [in] */ Integer index,
        /* [in] */ Long value) override final;

    ECode PutLongUnchecked(
        /* [in] */ Integer index,
        /* [in] */ Long value) override final;

    ECode PutUnchecked(
        /* [in] */ Integer pos,
        /* [in] */ const Array<Long>& src,
        /* [in] */ Integer srcOffset,
        /* [in] */ Integer length) override final;

    ECode AsLongBuffer(
        /* [out] */ ILongBuffer** buffer) override final;

    ECode GetFloat(
        /* [out] */ Float* f) override final;

    ECode GetFloat(
        /* [in] */ Integer index,
        /* [out] */ Float* f) override final;

    ECode GetFloatUnchecked(
        /* [in] */ Integer index,
        /* [out] */ Float* value) override final;

    ECode GetUnchecked(
        /* [in] */ Integer pos,
        /* [out] */ Array<Float>& dst,
        /* [in] */ Integer dstOffset,
        /* [in] */ Integer length) override final;

    ECode PutFloat(
        /* [in] */ Float value) override final;

    ECode PutFloat(
        /* [in] */ Integer index,
        /* [in] */ Float value) override final;

    ECode PutFloatUnchecked(
        /* [in] */ Integer index,
        /* [in] */ Float value) override final;

    ECode PutUnchecked(
        /* [in] */ Integer pos,
        /* [in] */ const Array<Float>& src,
        /* [in] */ Integer srcOffset,
        /* [in] */ Integer length) override final;

    ECode AsFloatBuffer(
        /* [out] */ IFloatBuffer** buffer) override final;

    ECode GetDouble(
        /* [out] */ Double* d) override final;

    ECode GetDouble(
        /* [in] */ Integer index,
        /* [out] */ Double* d) override final;

    ECode GetDoubleUnchecked(
        /* [in] */ Integer index,
        /* [out] */ Double* value) override final;

    ECode GetUnchecked(
        /* [in] */ Integer pos,
        /* [out] */ Array<Double>& dst,
        /* [in] */ Integer dstOffset,
        /* [in] */ Integer length) override final;

    ECode PutDouble(
        /* [in] */ Double value) override final;

    ECode PutDouble(
        /* [in] */ Integer index,
        /* [in] */ Double value) override final;

    ECode PutDoubleUnchecked(
        /* [in] */ Integer index,
        /* [in] */ Double value) override final;

    ECode PutUnchecked(
        /* [in] */ Integer pos,
        /* [in] */ const Array<Double>& src,
        /* [in] */ Integer srcOffset,
        /* [in] */ Integer length) override final;

    ECode AsDoubleBuffer(
        /* [out] */ IDoubleBuffer** buffer) override final;

    ECode IsAccessible(
        /* [out] */ Boolean* accessible) override final;

    ECode SetAccessible(
        /* [in] */ Boolean value) override final;

protected:
    ECode Constructor(
        /* [in] */ HANDLE addr,
        /* [in] */ Integer cap);

private:
    HANDLE Ix(
        /* [in] */ Integer i);

    Byte Get(
        /* [in] */ HANDLE addr);

    void Put(
        /* [in] */ HANDLE addr,
        /* [in] */ Byte x);

    void PutChar(
        /* [in] */ HANDLE addr,
        /* [in] */ Char x);

    Short GetShort(
        /* [in] */ HANDLE addr);

    void PutShort(
        /* [in] */ HANDLE addr,
        /* [in] */ Short x);

    Integer GetInteger(
        /* [in] */ HANDLE addr);

    void PutInteger(
        /* [in] */ HANDLE addr,
        /* [in] */ Integer x);

    Long GetLong(
        /* [in] */ HANDLE addr);

    void PutLong(
        /* [in] */ HANDLE addr,
        /* [in] */ Long x);

    Float GetFloat(
        /* [in] */ HANDLE addr);

    void PutFloat(
        /* [in] */ HANDLE addr,
        /* [in] */ Float x);

    Double GetDouble(
        /* [in] */ HANDLE addr);

    void PutDouble(
        /* [in] */ HANDLE addr,
        /* [in] */ Double x);

private:
    AutoPtr<MemoryRef> mMemoryRef;
};

}
}

#endif // __CCM_IO_DIRECTBYTEBUFFER_H__
