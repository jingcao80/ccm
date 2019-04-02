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

#ifndef __LIBCORE_IO_NIOBUFFERITERATOR_H__
#define __LIBCORE_IO_NIOBUFFERITERATOR_H__

#include "ccm/core/SyncObject.h"
#include "libcore.io.IBufferIterator.h"
#include "libcore.io.IMemoryMappedFile.h"

using ccm::core::SyncObject;

namespace libcore {
namespace io {

class NioBufferIterator
    : public SyncObject
    , public IBufferIterator
{
public:
    CCM_INTERFACE_DECL();

    ECode Constructor(
        /* [in] */ IMemoryMappedFile* file,
        /* [in] */ HANDLE address,
        /* [in] */ Integer length,
        /* [in] */ Boolean swap);

    ECode Seek(
        /* [in] */ Integer offset) override;

    ECode Skip(
        /* [in] */ Integer byteCount) override;

    ECode Pos(
        /* [out] */ Integer* offset) override;

    ECode ReadByteArray(
        /* [out] */ Array<Byte>& dst,
        /* [in] */ Integer dstOffset,
        /* [in] */ Integer byteCount) override;

    ECode ReadByte(
        /* [out] */ Byte* result) override;

    ECode ReadInteger(
        /* [out] */ Integer* result) override;

    ECode ReadIntegerArray(
        /* [out] */ Array<Integer>& dst,
        /* [in] */ Integer dstOffset,
        /* [in] */ Integer intCount) override;

    ECode ReadShort(
        /* [out] */ Short* result) override;

private:
    static ECode CheckReadBounds(
        /* [in] */ Integer position,
        /* [in] */ Integer length,
        /* [in] */ Integer byteCount);

    static ECode CheckDstBounds(
        /* [in] */ Integer dstOffset,
        /* [in] */ Integer dstLength,
        /* [in] */ Integer count);

private:
    AutoPtr<IMemoryMappedFile> mFile;
    HANDLE mAddress = 0;
    Integer mLength = 0;
    Boolean mSwap = false;

    Integer mPosition = 0;
};

}
}

#endif // __LIBCORE_IO_NIOBUFFERITERATOR_H__
