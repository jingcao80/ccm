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

#include "como/core/StringFactory.h"
#include "como/io/charset/StandardCharsets.h"
#include "libcore/io/IoBridge.h"
#include "libcore/io/IoUtils.h"
#include "libcore/io/Libcore.h"
#include "jing/system/OsConstants.h"
#include "jing.system.IStructStat.h"

using como::core::StringFactory;
using como::io::E_IO_EXCEPTION;
using como::io::charset::StandardCharsets;
using jing::system::IStructStat;
using jing::system::OsConstants;

namespace libcore {
namespace io {

ECode IoUtils::Close(
    /* [in] */ IFileDescriptor* fd)
{
    ECode ec = NOERROR;
    Boolean valid;
    if (fd != nullptr && (fd->Valid(&valid), valid)) {
        ec = Libcore::GetOs()->Close(fd);
    }
    return SUCCEEDED(ec) ? ec : E_IO_EXCEPTION;
}

void IoUtils::CloseQuietly(
    /* [in] */ IFileDescriptor* fd)
{
    IoUtils::Close(fd);
}

ECode IoUtils::ReadFileAsString(
    /* [in] */ const String& absolutePath,
    /* [out] */ String* content)
{
    VALIDATE_NOT_NULL(content);

    AutoPtr<FileReader> fr = new FileReader();
    FAIL_RETURN(fr->Constructor(absolutePath));
    fr->ReadFully();
    *content = fr->ToString(StandardCharsets::GetUTF_8());
    return NOERROR;
}

//--------------------------------------------------------------------------

ECode IoUtils::FileReader::Constructor(
    /* [in] */ const String& absolutePath)
{
    FAIL_RETURN(IoBridge::Open(absolutePath, OsConstants::O_RDONLY_, &mFd));

    AutoPtr<IStructStat> stat;
    ECode ec = Libcore::GetOs()->Fstat(mFd, &stat);
    if (FAILED(ec)) {
        CloseQuietly(mFd);
        return E_IO_EXCEPTION;
    }

    Long capacity;
    stat->GetSize(&capacity);
    if (capacity == 0) {
        mUnknownLength = true;
        capacity = 8192;
    }

    mBytes = Array<Byte>(capacity);
    return NOERROR;
}

ECode IoUtils::FileReader::ReadFully()
{
    Integer read;
    Integer capacity = mBytes.GetLength();
    ECode ec = Libcore::GetOs()->Read(mFd, mBytes, mCount, capacity - mCount, &read);
    if (FAILED(ec)) {
        CloseQuietly(mFd);
        return E_IO_EXCEPTION;
    }

    while (read != 0) {
        mCount += read;
        if (mCount == capacity) {
            if (mUnknownLength) {
                Integer newCapacity = capacity * 2;
                Array<Byte> newBytes(newCapacity);
                newBytes.Copy(0, mBytes, 0, capacity);
                mBytes = newBytes;
                capacity = newCapacity;
            }
            else {
                break;
            }
        }
        ec = Libcore::GetOs()->Read(mFd, mBytes, mCount, capacity - mCount, &read);
        if (FAILED(ec)) {
            CloseQuietly(mFd);
            return E_IO_EXCEPTION;
        }
    }

    CloseQuietly(mFd);
    return NOERROR;
}

Array<Byte> IoUtils::FileReader::ToByteArray()
{
    if (mCount == mBytes.GetLength()) {
        return mBytes;
    }
    Array<Byte> result(mCount);
    result.Copy(0, mBytes, 0, mCount);
    return result;
}

String IoUtils::FileReader::ToString(
    /* [in] */ ICharset* cs)
{
    String str;
    StringFactory::NewStringFromBytes(mBytes, 0, mCount, cs, &str);
    return str;
}

}
}
