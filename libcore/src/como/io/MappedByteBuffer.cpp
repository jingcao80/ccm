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

#include "como/io/Bits.h"
#include "como/io/MappedByteBuffer.h"
#include <sys/mman.h>

namespace como {
namespace io {

Byte MappedByteBuffer::sUnused = -1;

COMO_INTERFACE_IMPL_1(MappedByteBuffer, ByteBuffer, IMappedByteBuffer);

ECode MappedByteBuffer::Constructor(
    /* [in] */ Integer mark,
    /* [in] */ Integer pos,
    /* [in] */ Integer lim,
    /* [in] */ Integer cap,
    /* [in] */ IFileDescriptor* fd)
{
    FAIL_RETURN(ByteBuffer::Constructor(mark, pos, lim, cap));
    mFd = fd;
    return NOERROR;
}

ECode MappedByteBuffer::Constructor(
    /* [in] */ Integer mark,
    /* [in] */ Integer pos,
    /* [in] */ Integer lim,
    /* [in] */ Integer cap,
    /* [in] */ Array<Byte>& buf,
    /* [in] */ Integer offset)
{
    FAIL_RETURN(ByteBuffer::Constructor(mark, pos, lim, cap, buf, offset));
    return NOERROR;
}

ECode MappedByteBuffer::Constructor(
    /* [in] */ Integer mark,
    /* [in] */ Integer pos,
    /* [in] */ Integer lim,
    /* [in] */ Integer cap)
{
    FAIL_RETURN(ByteBuffer::Constructor(mark, pos, lim, cap));
    return NOERROR;
}

ECode MappedByteBuffer::CheckMapped()
{
    if (mFd == nullptr) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    return NOERROR;
}

HANDLE MappedByteBuffer::MappingOffset()
{
    Integer ps = Bits::PageSize();
    HANDLE offset = mAddress % ps;
    return (offset >= 0) ? offset : (ps + offset);
}

HANDLE MappedByteBuffer::MappingAddress(
    /* [in] */ HANDLE mappingOffset)
{
    return mAddress - mappingOffset;
}

Long MappedByteBuffer::MappingLength(
    /* [in] */ HANDLE mappingOffset)
{
    Integer cap;
    GetCapacity(&cap);
    return cap + mappingOffset;
}

ECode MappedByteBuffer::IsLoaded(
    /* [out] */ Boolean* loaded)
{
    VALIDATE_NOT_NULL(loaded);

    FAIL_RETURN(CheckMapped());
    if (mAddress == 0) {
        *loaded = true;
        return NOERROR;
    }
    Integer cap;
    GetCapacity(&cap);
    if (cap == 0) {
        *loaded = true;
        return NOERROR;
    }
    HANDLE offset = MappingOffset();
    Long length = MappingLength(offset);
    return IsLoaded0(MappingAddress(offset), length, Bits::PageCount(length), loaded);
}

ECode MappedByteBuffer::Load()
{
    FAIL_RETURN(CheckMapped());
    if (mAddress == 0) {
        return NOERROR;
    }
    Integer cap;
    GetCapacity(&cap);
    if (cap == 0) {
        return NOERROR;
    }
    HANDLE offset = MappingOffset();
    Long length = MappingLength(offset);
    FAIL_RETURN(Load0(MappingAddress(offset), length));

    // Read a byte from each page to bring it into memory. A checksum
    // is computed as we go along to prevent the compiler from otherwise
    // considering the loop as dead code.
    Integer ps = Bits::PageSize();
    Integer count = Bits::PageCount(length);
    HANDLE a = MappingAddress(offset);
    Byte x = 0;
    for (Integer i = 0; i < count; i++) {
        x ^= *reinterpret_cast<Byte*>(a);
        a += ps;
    }
    sUnused = x;
    return NOERROR;
}

ECode MappedByteBuffer::Force()
{
    FAIL_RETURN(CheckMapped());
    if (mAddress == 0) {
        return NOERROR;
    }
    Integer cap;
    GetCapacity(&cap);
    if (cap == 0) {
        return NOERROR;
    }
    HANDLE offset = MappingOffset();
    return Force0(mFd, MappingAddress(offset), MappingLength(offset));
}

ECode MappedByteBuffer::IsLoaded0(
    /* [in] */ HANDLE address,
    /* [in] */ Long length,
    /* [in] */ Integer pageCount,
    /* [out] */ Boolean* loaded)
{
    *loaded = true;
    unsigned char *vec = (unsigned char *)malloc(pageCount * sizeof(char));
    if (vec == NULL) {
        *loaded = false;
        return E_OUT_OF_MEMORY_ERROR;
    }

    int result = mincore(reinterpret_cast<void*>(address), (size_t)length, vec);
    if (result == -1) {
        *loaded = false;
        free(vec);
        Logger::E("MappedByteBuffer", "mincore failed");
        return E_IO_EXCEPTION;
    }

    for (Integer i = 0; i < pageCount; i++) {
        if (vec[i] == 0) {
            *loaded = false;
            break;
        }
    }
    free(vec);
    return NOERROR;
}

ECode MappedByteBuffer::Load0(
    /* [in] */ HANDLE address,
    /* [in] */ Long length)
{
    char* a = reinterpret_cast<char*>(address);
    int result = madvise((caddr_t)a, (size_t)length, MADV_WILLNEED);
    if (result == -1) {
        Logger::E("MappedByteBuffer", "madvise failed");
        return E_IO_EXCEPTION;
    }
    return NOERROR;
}

ECode MappedByteBuffer::Force0(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ HANDLE address,
    /* [in] */ Long length)
{
    int result = msync(reinterpret_cast<void*>(address), (size_t)length, MS_SYNC);
    if (result == -1) {
        Logger::E("MappedByteBuffer", "msync failed");
        return E_IO_EXCEPTION;
    }
    return NOERROR;
}

}
}
