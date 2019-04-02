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

#include "libcore/io/Memory.h"
#include "libcore/io/MemoryMappedFile.h"
#include "libcore/io/NioBufferIterator.h"
#include <ccmlogger.h>

using ccm::core::E_INDEX_OUT_OF_BOUNDS_EXCEPTION;

namespace libcore {
namespace io {

CCM_INTERFACE_IMPL_1(NioBufferIterator, SyncObject, IBufferIterator);

ECode NioBufferIterator::Constructor(
    /* [in] */ IMemoryMappedFile* file,
    /* [in] */ HANDLE address,
    /* [in] */ Integer length,
    /* [in] */ Boolean swap)
{
    mFile = file;
    mAddress = address;

    if (length < 0) {
        Logger::E("NioBufferIterator", "length < 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    constexpr Long MAX_VALID_ADDRESS = -1;
    if (mAddress > (HANDLE)(MAX_VALID_ADDRESS - length)) {
        Logger::E("NioBufferIterator", "length %d would overflow 64-bit address space", length);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mLength = length;

    mSwap = swap;
    return NOERROR;
}

ECode NioBufferIterator::Seek(
    /* [in] */ Integer offset)
{
    mPosition = offset;
    return NOERROR;
}

ECode NioBufferIterator::Skip(
    /* [in] */ Integer byteCount)
{
    mPosition += byteCount;
    return NOERROR;
}

ECode NioBufferIterator::Pos(
    /* [out] */ Integer* offset)
{
    VALIDATE_NOT_NULL(offset);

    *offset = mPosition;
    return NOERROR;
}

ECode NioBufferIterator::ReadByteArray(
    /* [out] */ Array<Byte>& dst,
    /* [in] */ Integer dstOffset,
    /* [in] */ Integer byteCount)
{
    FAIL_RETURN(CheckDstBounds(dstOffset, dst.GetLength(), byteCount));
    FAIL_RETURN(MemoryMappedFile::From(mFile)->CheckNotClosed());
    FAIL_RETURN(CheckReadBounds(mPosition, mLength, byteCount));
    Memory::PeekByteArray(mAddress + mPosition, dst, dstOffset, byteCount);
    mPosition += byteCount;
    return NOERROR;
}

ECode NioBufferIterator::ReadByte(
    /* [out] */ Byte* result)
{
    VALIDATE_NOT_NULL(result);

    FAIL_RETURN(MemoryMappedFile::From(mFile)->CheckNotClosed());
    FAIL_RETURN(CheckReadBounds(mPosition, mLength, 1));
    *result = Memory::PeekByte(mAddress + mPosition);
    ++mPosition;
    return NOERROR;
}

ECode NioBufferIterator::ReadInteger(
    /* [out] */ Integer* result)
{
    VALIDATE_NOT_NULL(result);

    FAIL_RETURN(MemoryMappedFile::From(mFile)->CheckNotClosed());
    FAIL_RETURN(CheckReadBounds(mPosition, mLength, sizeof(Integer)));
    *result = Memory::PeekInteger(mAddress + mPosition, mSwap);
    mPosition += sizeof(Integer);
    return NOERROR;
}

ECode NioBufferIterator::ReadIntegerArray(
    /* [out] */ Array<Integer>& dst,
    /* [in] */ Integer dstOffset,
    /* [in] */ Integer intCount)
{
    FAIL_RETURN(CheckDstBounds(dstOffset, dst.GetLength(), intCount));
    FAIL_RETURN(MemoryMappedFile::From(mFile)->CheckNotClosed());
    const Integer byteCount = sizeof(Integer) * intCount;
    FAIL_RETURN(CheckReadBounds(mPosition, mLength, byteCount));
    Memory::PeekIntegerArray(mAddress + mPosition, dst, dstOffset, intCount, mSwap);
    mPosition += byteCount;
    return NOERROR;
}

ECode NioBufferIterator::ReadShort(
    /* [out] */ Short* result)
{
    VALIDATE_NOT_NULL(result);

    FAIL_RETURN(MemoryMappedFile::From(mFile)->CheckNotClosed());
    FAIL_RETURN(CheckReadBounds(mPosition, mLength, sizeof(Short)));
    *result = Memory::PeekShort(mAddress + mPosition, mSwap);
    mPosition += sizeof(Short);
    return NOERROR;
}

ECode NioBufferIterator::CheckReadBounds(
    /* [in] */ Integer position,
    /* [in] */ Integer length,
    /* [in] */ Integer byteCount)
{
    if (position < 0 || byteCount < 0) {
        Logger::E("NioBufferIterator", "Invalid read args: position=%d, byteCount=%d",
                position, byteCount);
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    const Integer finalReadPos = position + byteCount;
    if (finalReadPos < 0 || finalReadPos > length) {
        Logger::E("NioBufferIterator", "Read outside range: position=%d, byteCount=%d, length=%d",
                position, byteCount, length);
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    return NOERROR;
}

ECode NioBufferIterator::CheckDstBounds(
    /* [in] */ Integer dstOffset,
    /* [in] */ Integer dstLength,
    /* [in] */ Integer count)
{
    if (dstOffset < 0 || count < 0) {
        Logger::E("NioBufferIterator", "Invalid dst args: offset=%d, count=%d",
                dstLength, count);
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    const Integer targetPos = dstOffset + count;
    if (targetPos < 0 || targetPos > dstLength) {
        Logger::E("NioBufferIterator", "Write outside range: dst.length=%d, offset=%d, count=%d",
                dstLength, dstOffset, count);
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    return NOERROR;
}

}
}
