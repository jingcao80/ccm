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

#ifndef __CCM_IO_MAPPEDBYTEBUFFER_H__
#define __CCM_IO_MAPPEDBYTEBUFFER_H__

#include "ccm/io/ByteBuffer.h"
#include "ccm.io.IFileDescriptor.h"
#include "ccm.io.IMappedByteBuffer.h"

namespace ccm {
namespace io {

class MappedByteBuffer
    : public ByteBuffer
    , public IMappedByteBuffer
{
public:
    ECode Constructor(
        /* [in] */ Integer mark,
        /* [in] */ Integer pos,
        /* [in] */ Integer lim,
        /* [in] */ Integer cap,
        /* [in] */ IFileDescriptor* fd);

    ECode Constructor(
        /* [in] */ Integer mark,
        /* [in] */ Integer pos,
        /* [in] */ Integer lim,
        /* [in] */ Integer cap,
        /* [in] */ Array<Byte>& buf,
        /* [in] */ Integer offset);

    ECode Constructor(
        /* [in] */ Integer mark,
        /* [in] */ Integer pos,
        /* [in] */ Integer lim,
        /* [in] */ Integer cap);

    ECode IsLoaded(
        /* [out] */ Boolean* loaded) override final;

    ECode Load() override final;

    ECode Force() override final;

private:
    ECode CheckMapped();

    Long MappingOffset();

    Long MappingAddress(
        /* [in] */ Long mappingOffset);

    Long MappingLength(
        /* [in] */ Long mappingOffset);

    ECode IsLoaded0(
        /* [in] */ Long address,
        /* [in] */ Long length,
        /* [in] */ Integer pageCount,
        /* [out] */ Boolean* loaded);

    ECode Load0(
        /* [in] */ Long address,
        /* [in] */ Long length);

    ECode Force0(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Long address,
        /* [in] */ Long length);

private:
    AutoPtr<IFileDescriptor> mFd;

    static Byte sUnused;
};

}
}

#endif // __CCM_IO_MAPPEDBYTEBUFFER_H__
