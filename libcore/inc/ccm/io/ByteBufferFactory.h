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

#ifndef __CCM_IO_BYTEBUFFERFACTORY_H__
#define __CCM_IO_BYTEBUFFERFACTORY_H__

#include "ccm.io.IByteBuffer.h"

namespace ccm {
namespace io {

class COM_PUBLIC ByteBufferFactory
{
public:
    static ECode AllocateDirect(
        /* [in] */ Integer capacity,
        /* [out] */ IByteBuffer** buffer);

    static ECode Allocate(
        /* [in] */ Integer capacity,
        /* [out] */ IByteBuffer** buffer);

    static ECode Wrap(
        /* [in] */ const Array<Byte>& array,
        /* [in] */ Integer offset,
        /* [in] */ Integer length,
        /* [out] */ IByteBuffer** buffer);

    static ECode Wrap(
        /* [in] */ const Array<Byte>& array,
        /* [out] */ IByteBuffer** buffer);

private:
    ByteBufferFactory();
};

}
}

#endif // __CCM_IO_BYTEBUFFERFACTORY_H__
