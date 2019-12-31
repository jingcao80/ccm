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

#ifndef __LIBCORE_IO_MEMORYMAPPEDFILE_H__
#define __LIBCORE_IO_MEMORYMAPPEDFILE_H__

#include "ccm/core/SyncObject.h"
#include "ccm.core.IAutoCloseable.h"
#include "libcore.io.IMemoryMappedFile.h"

using ccm::core::IAutoCloseable;
using ccm::core::SyncObject;
using libcore::io::IMemoryMappedFile;

namespace libcore {
namespace io {

class MemoryMappedFile
    : public SyncObject
    , public IMemoryMappedFile
    , public IAutoCloseable
{
public:
    CCM_INTERFACE_DECL();

    ECode Constructor(
        /* [in] */ HANDLE address,
        /* [in] */ Long size);

    static ECode MmapRO(
        /* [in] */ const String& path,
        /* [out]) */ IMemoryMappedFile** mappedFile);

    ECode Close() override;

    ECode IsClosed(
        /* [out] */ Boolean* closed) override;

    ECode BigEndianIterator(
        /* [out] */ IBufferIterator** it) override;

    ECode LittleEndianIterator(
        /* [out] */ IBufferIterator** it) override;

    ECode GetSize(
        /* [out] */ Integer* size) override;

    ECode CheckNotClosed();

    static MemoryMappedFile* From(
        /* [in] */ IMemoryMappedFile* obj);

private:
    Boolean mClosed = false;
    HANDLE mAddress = 0;
    Integer mSize = 0;
};

inline MemoryMappedFile* MemoryMappedFile::From(
    /* [in] */ IMemoryMappedFile* obj)
{
    return (MemoryMappedFile*)obj;
}

}
}

#endif // __LIBCORE_IO_MEMORYMAPPEDFILE_H__
