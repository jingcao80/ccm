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

#include "ccm/core/Math.h"
#include "ccm/io/CDirectByteBuffer.h"
#include "libcore/io/Memory.h"
#include <ccmlogger.h>

using ccm::core::E_ILLEGAL_STATE_EXCEPTION;
using ccm::core::Math;
using ccm::io::channels::IID_IDirectBuffer;
using libcore::io::Memory;

namespace ccm {
namespace io {

DirectByteBuffer::MemoryRef::MemoryRef(
    /* [in] */ Integer capacity)
    : mIsAccessible(true)
    , mIsFreed(false)
{
    mBuffer = Array<Byte>::Allocate(capacity + 7);
    mAllocatedAddress = reinterpret_cast<HANDLE>(mBuffer.GetPayload());
    mOffset = (Integer) (((mAllocatedAddress + 7) & ~(HANDLE)7) - mAllocatedAddress);
}

DirectByteBuffer::MemoryRef::MemoryRef(
    /* [in] */ HANDLE allocatedAddress)
    : mAllocatedAddress(allocatedAddress)
    , mOffset(0)
    , mIsAccessible(true)
    , mIsFreed(false)
{}

void DirectByteBuffer::MemoryRef::Free()
{
    mBuffer.Clear();
    mAllocatedAddress = 0;
    mIsAccessible = false;
    mIsFreed = true;
}

//-------------------------------------------------------------------------

CCM_INTERFACE_IMPL_1(DirectByteBuffer, MappedByteBuffer, IDirectBuffer);

ECode DirectByteBuffer::Constructor(
    /* [in] */ Integer cap,
    /* [in] */ MemoryRef* memoryRef)
{
    FAIL_RETURN(MappedByteBuffer::Constructor(-1, 0, cap, cap, memoryRef->mBuffer, memoryRef->mOffset));
    mMemoryRef = memoryRef;
    mAddress = memoryRef->mAllocatedAddress + memoryRef->mOffset;
    mIsReadOnly = false;
    return NOERROR;
}

ECode DirectByteBuffer::Constructor(
    /* [in] */ HANDLE addr,
    /* [in] */ Integer cap)
{
    FAIL_RETURN(MappedByteBuffer::Constructor(-1, 0, cap, cap));
    mMemoryRef = new MemoryRef(addr);
    mAddress = addr;
    return NOERROR;
}

ECode DirectByteBuffer::Constructor(
    /* [in] */ Integer cap,
    /* [in] */ HANDLE addr,
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Boolean isReadOnly)
{
    FAIL_RETURN(MappedByteBuffer::Constructor(-1, 0, cap, cap, fd));
    mIsReadOnly = isReadOnly;
    mMemoryRef = new MemoryRef(addr);
    mAddress = addr;
    return NOERROR;
}

ECode DirectByteBuffer::Constructor(
    /* [in] */ MemoryRef* memoryRef,
    /* [in] */ Integer mark,
    /* [in] */ Integer pos,
    /* [in] */ Integer lim,
    /* [in] */ Integer cap,
    /* [in] */ Integer off)
{
    return Constructor(memoryRef, mark, pos, lim, cap, off, false);
}

ECode DirectByteBuffer::Constructor(
    /* [in] */ MemoryRef* memoryRef,
    /* [in] */ Integer mark,
    /* [in] */ Integer pos,
    /* [in] */ Integer lim,
    /* [in] */ Integer cap,
    /* [in] */ Integer off,
    /* [in] */ Boolean isReadOnly)
{
    FAIL_RETURN(MappedByteBuffer::Constructor(mark, pos, lim, cap, memoryRef->mBuffer, off));
    mIsReadOnly = isReadOnly;
    mMemoryRef = memoryRef;
    mAddress = memoryRef->mAllocatedAddress + off;
    return NOERROR;
}

ECode DirectByteBuffer::GetAttachment(
    /* [out] */ IInterface** attachment)
{
    VALIDATE_NOT_NULL(attachment);

    *attachment = (IObject*)mMemoryRef;
    REFCOUNT_ADD(*attachment);
    return NOERROR;
}

ECode DirectByteBuffer::Slice(
    /* [out] */ IByteBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer);

    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    Integer pos;
    GetPosition(&pos);
    Integer lim;
    GetLimit(&lim);
    CHECK(pos <= lim);
    Integer rem = (pos <= lim ? lim - pos : 0);
    Integer off = pos + mOffset;
    CHECK(off >= 0);
    return CDirectByteBuffer::New(mMemoryRef, -1, 0, rem, rem, off, mIsReadOnly,
            IID_IByteBuffer, (IInterface**)buffer);
}

ECode DirectByteBuffer::Duplicate(
    /* [out] */ IByteBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer);

    if (mMemoryRef->mIsFreed) {
        Logger::E("DirectByteBuffer", "buffer has been freed");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    Integer pos;
    GetPosition(&pos);
    Integer lim;
    GetLimit(&lim);
    Integer cap;
    GetCapacity(&cap);
    return CDirectByteBuffer::New(mMemoryRef, MarkValue(), pos, lim, cap, mOffset, mIsReadOnly,
            IID_IByteBuffer, (IInterface**)buffer);
}

ECode DirectByteBuffer::AsReadOnlyBuffer(
    /* [out] */ IByteBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer);

    if (mMemoryRef->mIsFreed) {
        Logger::E("DirectByteBuffer", "buffer has been freed");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    Integer pos;
    GetPosition(&pos);
    Integer lim;
    GetLimit(&lim);
    Integer cap;
    GetCapacity(&cap);
    return CDirectByteBuffer::New(mMemoryRef, MarkValue(), pos, lim, cap, mOffset, true,
            IID_IByteBuffer, (IInterface**)buffer);
}

ECode DirectByteBuffer::GetAddress(
    /* [out] */ HANDLE* addr)
{
    VALIDATE_NOT_NULL(addr);

    *addr = mAddress;
    return NOERROR;
}

HANDLE DirectByteBuffer::Ix(
    /* [in] */ Integer i)
{
    return mAddress + i;
}

Byte DirectByteBuffer::Get(
    /* [in] */ HANDLE addr)
{
    return Memory::PeekByte(addr);
}

ECode DirectByteBuffer::Get(
    /* [out] */ Byte* b)
{
    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    Integer idx;
    NextGetIndex(&idx);
    *b = Get(Ix(idx));
    return NOERROR;
}

ECode DirectByteBuffer::Get(
    /* [in] */ Integer i,
    /* [out] */ Byte* b)
{
    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    Integer idx;
    NextGetIndex(i, &idx);
    *b = Get(Ix(idx));
    return NOERROR;
}

ECode DirectByteBuffer::Get(
    /* [out] */ Array<Byte>& dst,
    /* [in] */ Integer offset,
    /* [in] */ Integer length)
{
    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    FAIL_RETURN(CheckBounds(offset, length, dst.GetLength()));
    Integer pos;
    GetPosition(&pos);
    Integer lim;
    GetLimit(&lim);
    CHECK(pos <=  lim);
    Integer rem = (pos <= lim ? lim - pos : 0);
    if (length > rem) {
        return E_BUFFER_UNDERFLOW_EXCEPTION;
    }
    Memory::PeekByteArray(Ix(pos), dst, offset, length);
    mPosition = pos + length;
    return NOERROR;
}

void DirectByteBuffer::Put(
    /* [in] */ HANDLE addr,
    /* [in] */ Byte x)
{
    Memory::PokeByte(addr, x);
}

ECode DirectByteBuffer::Put(
    /* [in] */ Byte b)
{
    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (mIsReadOnly) {
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    Integer idx;
    NextPutIndex(&idx);
    Put(Ix(idx), b);
    return NOERROR;
}

ECode DirectByteBuffer::Put(
    /* [in] */ Integer i,
    /* [in] */ Byte b)
{
    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (mIsReadOnly) {
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    FAIL_RETURN(CheckIndex(i));
    Put(Ix(i), b);
    return NOERROR;
}

ECode DirectByteBuffer::Put(
    /* [in] */ const Array<Byte>& src,
    /* [in] */ Integer offset,
    /* [in] */ Integer length)
{
    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (mIsReadOnly) {
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    FAIL_RETURN(CheckBounds(offset, length, src.GetLength()));
    Integer pos;
    GetPosition(&pos);
    Integer lim;
    GetLimit(&lim);
    CHECK(pos <= lim);
    Integer rem = (pos <= lim ? lim - pos : 0);
    if (length > rem) {
        return E_BUFFER_OVERFLOW_EXCEPTION;
    }
    Memory::PokeByteArray(Ix(pos), src, offset, length);
    mPosition = pos + length;
    return NOERROR;
}

ECode DirectByteBuffer::Compact()
{
    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (mIsReadOnly) {
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    Integer pos;
    GetPosition(&pos);
    Integer lim;
    GetLimit(&lim);
    CHECK(pos <= lim);
    Integer rem = (pos <= lim ? lim - pos : 0);
    Integer remaining;
    Remaining(&remaining);
    mHb.Copy(mOffset, mHb, mPosition + mOffset, remaining);
    SetPosition(rem);
    Integer cap;
    GetCapacity(&cap);
    SetLimit(cap);
    DiscardMark();
    return NOERROR;
}

ECode DirectByteBuffer::IsDirect(
    /* [out] */ Boolean* direct)
{
    VALIDATE_NOT_NULL(direct);

    *direct = true;
    return NOERROR;
}

ECode DirectByteBuffer::IsReadOnly(
    /* [out] */ Boolean* readOnly)
{
    VALIDATE_NOT_NULL(readOnly);

    *readOnly = mIsReadOnly;
    return NOERROR;
}

Byte DirectByteBuffer::_Get(
    /* [in] */ Integer i)
{
    Byte b;
    Get(i, &b);
    return b;
}

ECode DirectByteBuffer::_Put(
    /* [in] */ Integer i,
    /* [in] */ Byte b)
{
    return Put(i, b);
}

ECode DirectByteBuffer::GetChar(
    /* [out] */ Char* c)
{
    VALIDATE_NOT_NULL(c);

    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    Integer newPosition = mPosition + sizeof(Char);
    Integer lim;
    GetLimit(&lim);
    if (newPosition > lim) {
        return E_BUFFER_UNDERFLOW_EXCEPTION;
    }
    *c = (Char)Memory::PeekInteger(Ix(mPosition), !mNativeByteOrder);
    mPosition = newPosition;
    return NOERROR;
}

ECode DirectByteBuffer::GetChar(
    /* [in] */ Integer index,
    /* [out] */ Char* c)
{
    VALIDATE_NOT_NULL(c);

    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    FAIL_RETURN(CheckIndex(index, sizeof(Char)));
    *c = (Char)Memory::PeekInteger(Ix(index), !mNativeByteOrder);
    return NOERROR;
}

ECode DirectByteBuffer::GetCharUnchecked(
    /* [in] */ Integer index,
    /* [out] */ Char* c)
{
    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    *c = (Char)Memory::PeekInteger(Ix(index), !mNativeByteOrder);
    return NOERROR;
}

ECode DirectByteBuffer::GetUnchecked(
    /* [in] */ Integer pos,
    /* [out] */ Array<Char>& dst,
    /* [in] */ Integer dstOffset,
    /* [in] */ Integer length)
{
    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    Memory::PeekCharArray(Ix(pos), dst, dstOffset, length, !mNativeByteOrder);
    return NOERROR;
}

void DirectByteBuffer::PutChar(
    /* [in] */ HANDLE addr,
    /* [in] */ Char x)
{
    Memory::PokeInteger(addr, (Integer)x, !mNativeByteOrder);
}

ECode DirectByteBuffer::PutChar(
    /* [in] */ Char value)
{
    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (mIsReadOnly) {
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    Integer idx;
    NextPutIndex(sizeof(Char), &idx);
    PutChar(Ix(idx), value);
    return NOERROR;
}

ECode DirectByteBuffer::PutChar(
    /* [in] */ Integer index,
    /* [in] */ Char value)
{
    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (mIsReadOnly) {
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    FAIL_RETURN(CheckIndex(index, sizeof(Char)));
    PutChar(Ix(index), value);
    return NOERROR;
}

ECode DirectByteBuffer::PutCharUnchecked(
    /* [in] */ Integer index,
    /* [in] */ Char value)
{
    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    PutChar(Ix(index), value);
    return NOERROR;
}

ECode DirectByteBuffer::PutUnchecked(
    /* [in] */ Integer pos,
    /* [in] */ const Array<Char>& src,
    /* [in] */ Integer srcOffset,
    /* [in] */ Integer length)
{
    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    Memory::PokeCharArray(Ix(pos), src, srcOffset, length, !mNativeByteOrder);
    return NOERROR;
}

Short DirectByteBuffer::GetShort(
    /* [in] */ HANDLE addr)
{
    return Memory::PeekShort(addr, !mNativeByteOrder);
}

ECode DirectByteBuffer::GetShort(
    /* [out] */ Short* s)
{
    VALIDATE_NOT_NULL(s);

    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    Integer idx;
    NextGetIndex(sizeof(Short), &idx);
    *s = GetShort(Ix(idx));
    return NOERROR;
}

ECode DirectByteBuffer::GetShort(
    /* [in] */ Integer index,
    /* [out] */ Short* s)
{
    VALIDATE_NOT_NULL(s);

    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    FAIL_RETURN(CheckIndex(index, sizeof(Short)));
    *s = GetShort(Ix(index));
    return NOERROR;
}

ECode DirectByteBuffer::GetShortUnchecked(
    /* [in] */ Integer index,
    /* [out] */ Short* value)
{
    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    *value = GetShort(Ix(index));
    return NOERROR;
}

ECode DirectByteBuffer::GetUnchecked(
    /* [in] */ Integer pos,
    /* [out] */ Array<Short>& dst,
    /* [in] */ Integer dstOffset,
    /* [in] */ Integer length)
{
    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    Memory::PeekShortArray(Ix(pos), dst, dstOffset, length, !mNativeByteOrder);
    return NOERROR;
}

void DirectByteBuffer::PutShort(
    /* [in] */ HANDLE addr,
    /* [in] */ Short x)
{
    Memory::PokeShort(addr, x, !mNativeByteOrder);
}

ECode DirectByteBuffer::PutShort(
    /* in] */ Short value)
{
    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (mIsReadOnly) {
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    Integer idx;
    NextPutIndex(sizeof(Short), &idx);
    PutShort(Ix(idx), value);
    return NOERROR;
}

ECode DirectByteBuffer::PutShort(
    /* [in] */ Integer index,
    /* [in] */ Short value)
{
    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (mIsReadOnly) {
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    FAIL_RETURN(CheckIndex(index, sizeof(Short)));
    PutShort(Ix(index), value);
    return NOERROR;
}

ECode DirectByteBuffer::PutShortUnchecked(
    /* [in] */ Integer index,
    /* [in] */ Short value)
{
    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    PutShort(Ix(index), value);
    return NOERROR;
}

ECode DirectByteBuffer::PutUnchecked(
    /* [in] */ Integer pos,
    /* [in] */ const Array<Short>& src,
    /* [in] */ Integer srcOffset,
    /* [in] */ Integer length)
{
    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    Memory::PokeShortArray(Ix(pos), src, srcOffset, length, !mNativeByteOrder);
    return NOERROR;
}

Integer DirectByteBuffer::GetInteger(
    /* [in] */ HANDLE addr)
{
    return Memory::PeekInteger(addr, !mNativeByteOrder);
}

ECode DirectByteBuffer::GetInteger(
    /* [out] */ Integer* i)
{
    VALIDATE_NOT_NULL(i);

    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    Integer idx;
    NextGetIndex(sizeof(Integer), &idx);
    *i = GetInteger(Ix(idx));
    return NOERROR;
}

ECode DirectByteBuffer::GetInteger(
    /* [in] */ Integer index,
    /* [out] */ Integer* i)
{
    VALIDATE_NOT_NULL(i);

    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    FAIL_RETURN(CheckIndex(index, sizeof(Integer)));
    *i = GetInteger(Ix(index));
    return NOERROR;
}

ECode DirectByteBuffer::GetIntegerUnchecked(
    /* [in] */ Integer index,
    /* [out] */ Integer* value)
{
    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    *value = GetInteger(Ix(index));
    return NOERROR;
}

ECode DirectByteBuffer::GetUnchecked(
    /* [in] */ Integer pos,
    /* [out] */ Array<Integer>& dst,
    /* [in] */ Integer dstOffset,
    /* [in] */ Integer length)
{
    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    Memory::PeekIntegerArray(Ix(pos), dst, dstOffset, length, !mNativeByteOrder);
    return NOERROR;
}

void DirectByteBuffer::PutInteger(
    /* [in] */ HANDLE addr,
    /* [in] */ Integer x)
{
    Memory::PokeInteger(addr, x, !mNativeByteOrder);
}

ECode DirectByteBuffer::PutInteger(
    /* [in] */ Integer value)
{
    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (mIsReadOnly) {
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    Integer idx;
    NextPutIndex(sizeof(Integer), &idx);
    PutInteger(Ix(idx), value);
    return NOERROR;
}

ECode DirectByteBuffer::PutInteger(
    /* [in] */ Integer index,
    /* [in] */ Integer value)
{
    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (mIsReadOnly) {
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    FAIL_RETURN(CheckIndex(index, sizeof(Integer)));
    PutInteger(Ix(index), value);
    return NOERROR;
}

ECode DirectByteBuffer::PutIntegerUnchecked(
    /* [in] */ Integer index,
    /* [in] */ Integer value)
{
    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    PutInteger(Ix(index), value);
    return NOERROR;
}

ECode DirectByteBuffer::PutUnchecked(
    /* [in] */ Integer pos,
    /* [in] */ const Array<Integer>& src,
    /* [in] */ Integer srcOffset,
    /* [in] */ Integer length)
{
    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    Memory::PokeIntegerArray(Ix(pos), src, srcOffset, length, !mNativeByteOrder);
    return NOERROR;
}

Long DirectByteBuffer::GetLong(
    /* [in] */ HANDLE addr)
{
    return Memory::PeekLong(addr, !mNativeByteOrder);
}

ECode DirectByteBuffer::GetLong(
    /* [out] */ Long* l)
{
    VALIDATE_NOT_NULL(l);

    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    Integer idx;
    NextGetIndex(sizeof(Long), &idx);
    *l = GetLong(Ix(idx));
    return NOERROR;
}

ECode DirectByteBuffer::GetLong(
    /* [in] */ Integer index,
    /* [out] */ Long* l)
{
    VALIDATE_NOT_NULL(l);

    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    FAIL_RETURN(CheckIndex(index, sizeof(Long)));
    *l = GetLong(Ix(index));
    return NOERROR;
}

ECode DirectByteBuffer::GetLongUnchecked(
    /* [in] */ Integer index,
    /* [out] */ Long* value)
{
    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    *value = GetLong(Ix(index));
    return NOERROR;
}

ECode DirectByteBuffer::GetUnchecked(
    /* [in] */ Integer pos,
    /* [out] */ Array<Long>& dst,
    /* [in] */ Integer dstOffset,
    /* [in] */ Integer length)
{
    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    Memory::PeekLongArray(Ix(pos), dst, dstOffset, length, !mNativeByteOrder);
    return NOERROR;
}

void DirectByteBuffer::PutLong(
    /* [in] */ HANDLE addr,
    /* [in] */ Long x)
{
    Memory::PokeLong(addr, x, !mNativeByteOrder);
}

ECode DirectByteBuffer::PutLong(
    /* [in] */ Long value)
{
    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (mIsReadOnly) {
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    Integer idx;
    NextPutIndex(sizeof(Long), &idx);
    PutLong(Ix(idx), value);
    return NOERROR;
}

ECode DirectByteBuffer::PutLong(
    /* [in] */ Integer index,
    /* [in] */ Long value)
{
    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (mIsReadOnly) {
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    FAIL_RETURN(CheckIndex(index, sizeof(Long)));
    PutLong(Ix(index), value);
    return NOERROR;
}

ECode DirectByteBuffer::PutLongUnchecked(
    /* [in] */ Integer index,
    /* [in] */ Long value)
{
    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    PutLong(Ix(index), value);
    return NOERROR;
}

ECode DirectByteBuffer::PutUnchecked(
    /* [in] */ Integer pos,
    /* [in] */ const Array<Long>& src,
    /* [in] */ Integer srcOffset,
    /* [in] */ Integer length)
{
    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    Memory::PokeLongArray(Ix(pos), src, srcOffset, length, !mNativeByteOrder);
    return NOERROR;
}

Float DirectByteBuffer::GetFloat(
    /* [in] */ HANDLE addr)
{
    Integer x = Memory::PeekInteger(addr, !mNativeByteOrder);
    return Math::IntegerBitsToFloat(x);
}

ECode DirectByteBuffer::GetFloat(
    /* [out] */ Float* f)
{
    VALIDATE_NOT_NULL(f);

    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    Integer idx;
    NextGetIndex(sizeof(Float), &idx);
    *f = GetFloat(Ix(idx));
    return NOERROR;
}

ECode DirectByteBuffer::GetFloat(
    /* [in] */ Integer index,
    /* [out] */ Float* f)
{
    VALIDATE_NOT_NULL(f);

    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    FAIL_RETURN(CheckIndex(index, sizeof(Float)));
    *f = GetFloat(Ix(index));
    return NOERROR;
}

ECode DirectByteBuffer::GetFloatUnchecked(
    /* [in] */ Integer index,
    /* [out] */ Float* value)
{
    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    *value = GetFloat(Ix(index));
    return NOERROR;
}

ECode DirectByteBuffer::GetUnchecked(
    /* [in] */ Integer pos,
    /* [out] */ Array<Float>& dst,
    /* [in] */ Integer dstOffset,
    /* [in] */ Integer length)
{
    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    Memory::PeekFloatArray(Ix(pos), dst, dstOffset, length, !mNativeByteOrder);
    return NOERROR;
}

void DirectByteBuffer::PutFloat(
    /* [in] */ HANDLE addr,
    /* [in] */ Float x)
{
    Integer y = Math::FloatToRawIntegerBits(x);
    Memory::PokeInteger(addr, y, !mNativeByteOrder);
}

ECode DirectByteBuffer::PutFloat(
    /* [in] */ Float value)
{
    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (mIsReadOnly) {
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    Integer idx;
    NextPutIndex(sizeof(Float), &idx);
    PutFloat(Ix(idx), value);
    return NOERROR;
}

ECode DirectByteBuffer::PutFloat(
    /* [in] */ Integer index,
    /* [in] */ Float value)
{
    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (mIsReadOnly) {
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    FAIL_RETURN(CheckIndex(index, sizeof(Float)));
    PutFloat(Ix(index), value);
    return NOERROR;
}

ECode DirectByteBuffer::PutFloatUnchecked(
    /* [in] */ Integer index,
    /* [in] */ Float value)
{
    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    PutFloat(Ix(index), value);
    return NOERROR;
}

ECode DirectByteBuffer::PutUnchecked(
    /* [in] */ Integer pos,
    /* [in] */ const Array<Float>& src,
    /* [in] */ Integer srcOffset,
    /* [in] */ Integer length)
{
    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    Memory::PokeFloatArray(Ix(pos), src, srcOffset, length, !mNativeByteOrder);
    return NOERROR;
}

Double DirectByteBuffer::GetDouble(
    /* [in] */ HANDLE addr)
{
    Long x = Memory::PeekLong(addr, !mNativeByteOrder);
    return Math::LongBitsToDouble(x);
}

ECode DirectByteBuffer::GetDouble(
    /* [out] */ Double* d)
{
    VALIDATE_NOT_NULL(d);

    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    Integer idx;
    NextGetIndex(sizeof(Double), &idx);
    *d = GetDouble(Ix(idx));
    return NOERROR;
}

ECode DirectByteBuffer::GetDouble(
    /* [in] */ Integer index,
    /* [out] */ Double* d)
{
    VALIDATE_NOT_NULL(d);

    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    FAIL_RETURN(CheckIndex(index, sizeof(Double)));
    *d = GetDouble(Ix(index));
    return NOERROR;
}

ECode DirectByteBuffer::GetDoubleUnchecked(
    /* [in] */ Integer index,
    /* [out] */ Double* value)
{
    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    *value = GetDouble(Ix(index));
    return NOERROR;
}

ECode DirectByteBuffer::GetUnchecked(
    /* [in] */ Integer pos,
    /* [out] */ Array<Double>& dst,
    /* [in] */ Integer dstOffset,
    /* [in] */ Integer length)
{
    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    Memory::PeekDoubleArray(Ix(pos), dst, dstOffset, length, !mNativeByteOrder);
    return NOERROR;
}

void DirectByteBuffer::PutDouble(
    /* [in] */ HANDLE addr,
    /* [in] */ Double x)
{
    Long y = Math::DoubleToRawLongBits(x);
    Memory::PokeLong(addr, y, !mNativeByteOrder);
}

ECode DirectByteBuffer::PutDouble(
    /* [in] */ Double value)
{
    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (mIsReadOnly) {
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    Integer idx;
    NextPutIndex(sizeof(Double), &idx);
    PutDouble(Ix(idx), value);
    return NOERROR;
}

ECode DirectByteBuffer::PutDouble(
    /* [in] */ Integer index,
    /* [in] */ Double value)
{
    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (mIsReadOnly) {
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    FAIL_RETURN(CheckIndex(index, sizeof(Double)));
    PutDouble(Ix(index), value);
    return NOERROR;
}

ECode DirectByteBuffer::PutDoubleUnchecked(
    /* [in] */ Integer index,
    /* [in] */ Double value)
{
    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    PutDouble(Ix(index), value);
    return NOERROR;
}

ECode DirectByteBuffer::PutUnchecked(
    /* [in] */ Integer pos,
    /* [in] */ const Array<Double>& src,
    /* [in] */ Integer srcOffset,
    /* [in] */ Integer length)
{
    if (!mMemoryRef->mIsAccessible) {
        Logger::E("DirectByteBuffer", "buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    Memory::PokeDoubleArray(Ix(pos), src, srcOffset, length, !mNativeByteOrder);
    return NOERROR;
}

ECode DirectByteBuffer::IsAccessible(
    /* [out] */ Boolean* accessible)
{
    VALIDATE_NOT_NULL(accessible);

    *accessible = mMemoryRef->mIsAccessible;
    return NOERROR;
}

ECode DirectByteBuffer::SetAccessible(
    /* [in] */ Boolean value)
{
    mMemoryRef->mIsAccessible = value;
    return NOERROR;
}

}
}
