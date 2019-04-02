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

#include "ccm/io/ByteOrder.h"
#include "ccm.core.IInteger.h"
#include "ccm.io.IFileDescriptor.h"
#include "libcore/io/CMemoryMappedFile.h"
#include "libcore/io/Libcore.h"
#include "libcore/io/MemoryMappedFile.h"
#include "libcore/io/NioBufferIterator.h"
#include "libcore.io.IOs.h"
#include "pisces/system/OsConstants.h"
#include "pisces.system.IStructStat.h"
#include <ccmlogger.h>

using ccm::core::E_ILLEGAL_STATE_EXCEPTION;
using ccm::core::IID_IAutoCloseable;
using ccm::core::IInteger;
using ccm::io::ByteOrder;
using ccm::io::IFileDescriptor;
using pisces::system::IStructStat;
using pisces::system::OsConstants;

namespace libcore {
namespace io {

CCM_INTERFACE_IMPL_2(MemoryMappedFile, SyncObject, IMemoryMappedFile, IAutoCloseable);

ECode MemoryMappedFile::Constructor(
    /* [in] */ HANDLE address,
    /* [in] */ Long size)
{
    mAddress = address;
    if (size < 0 || size > IInteger::MAX_VALUE) {
        Logger::E("MemoryMappedFile", "Unsupported file size=%lld", size);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mSize = size;
    return NOERROR;
}

ECode MemoryMappedFile::MmapRO(
    /* [in] */ const String& path,
    /* [out]) */ IMemoryMappedFile** mappedFile)
{
    VALIDATE_NOT_NULL(mappedFile);

    AutoPtr<IOs> os = Libcore::GetOs();

    AutoPtr<IFileDescriptor> fd;
    os->Open(path, OsConstants::O_RDONLY_, 0, &fd);
    AutoPtr<IStructStat> stat;
    os->Fstat(fd, &stat);
    Long size;
    stat->GetSize(&size);
    HANDLE address;
    ECode ec = os->Mmap(0, size, OsConstants::PROT_READ_, OsConstants::MAP_SHARED_, fd, 0, &address);
    if (SUCCEEDED(ec)) {
        ec = CMemoryMappedFile::New(address, size, IID_IMemoryMappedFile, (IInterface**)mappedFile);
    }
    os->Close(fd);
    return ec;
}

ECode MemoryMappedFile::Close()
{
    if (!mClosed) {
        mClosed = true;
        return Libcore::GetOs()->Munmap(mAddress, mSize);
    }
    return NOERROR;
}

ECode MemoryMappedFile::IsClosed(
    /* [out] */ Boolean* closed)
{
    VALIDATE_NOT_NULL(closed);

    *closed = mClosed;
    return NOERROR;
}

ECode MemoryMappedFile::BigEndianIterator(
    /* [out] */ IBufferIterator** it)
{
    VALIDATE_NOT_NULL(it);

    FAIL_RETURN(CheckNotClosed());
    AutoPtr<NioBufferIterator> bi = new NioBufferIterator();
    FAIL_RETURN(bi->Constructor(this, mAddress, mSize,
            ByteOrder::Order() != ByteOrder::GetBIG_ENDIAN()));
    *it = (IBufferIterator*)bi;
    REFCOUNT_ADD(*it);
    return NOERROR;
}

ECode MemoryMappedFile::LittleEndianIterator(
    /* [out] */ IBufferIterator** it)
{
    VALIDATE_NOT_NULL(it);

    FAIL_RETURN(CheckNotClosed());
    AutoPtr<NioBufferIterator> bi = new NioBufferIterator();
    FAIL_RETURN(bi->Constructor(this, mAddress, mSize,
            ByteOrder::Order() != ByteOrder::GetLITTLE_ENDIAN()));
    *it = (IBufferIterator*)bi;
    REFCOUNT_ADD(*it);
    return NOERROR;
}

ECode MemoryMappedFile::CheckNotClosed()
{
    if (mClosed) {
        Logger::E("MemoryMappedFile", "MemoryMappedFile is closed");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode MemoryMappedFile::GetSize(
    /* [out] */ Integer* size)
{
    VALIDATE_NOT_NULL(size);

    FAIL_RETURN(CheckNotClosed());
    *size = mSize;
    return NOERROR;
}

}
}