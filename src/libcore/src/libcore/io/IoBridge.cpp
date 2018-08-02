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

#include "ccm/util/Arrays.h"
#include "libcore/io/IoBridge.h"
#include "libcore/io/Libcore.h"

using ccm::util::Arrays;

namespace libcore {
namespace io {

ECode IoBridge::CloseAndSignalBlockedThreads(
    /* [in] */ IFileDescriptor* fd)
{
    return NOERROR;
}

ECode IoBridge::Read(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ Array<Byte>& bytes,
    /* [in] */ Integer byteOffset,
    /* [in] */ Integer byteCount,
    /* [out] */ Integer* number)
{
    VALIDATE_NOT_NULL(number);

    FAIL_RETURN(Arrays::CheckOffsetAndCount(bytes.GetLength(), byteOffset, byteCount));
    if (byteCount == 0) {
        *number = 0;
        return NOERROR;
    }
    Integer readCount;
    ECode ec = Libcore::GetOs()->Read(fd, bytes, byteOffset, byteCount, &readCount);
    if (FAILED(ec)) return ccm::io::E_IO_EXCEPTION;
    if (readCount == 0) {
        *number = -1;
        return NOERROR;
    }
    *number = readCount;
    return NOERROR;
}

ECode IoBridge::Write(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ const Array<Byte>& bytes,
    /* [in] */ Integer byteOffset,
    /* [in] */ Integer byteCount)
{
    FAIL_RETURN(Arrays::CheckOffsetAndCount(bytes.GetLength(), byteOffset, byteCount));
    if (byteCount == 0) {
        return NOERROR;
    }
    while (byteCount > 0) {
        Integer bytesWritten;
        ECode ec = Libcore::GetOs()->Write(fd, bytes, byteOffset, byteCount, &bytesWritten);
        if (FAILED(ec)) return ccm::io::E_IO_EXCEPTION;
        byteCount -= bytesWritten;
        byteOffset += bytesWritten;
    }
    return NOERROR;
}

}
}
