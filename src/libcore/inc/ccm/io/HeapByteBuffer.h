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

#ifndef __CCM_IO_HEAPBYTEBUFFER_H__
#define __CCM_IO_HEAPBYTEBUFFER_H__

#include "ccm/io/ByteBuffer.h"

namespace ccm {
namespace io {

class HeapByteBuffer
    : public ByteBuffer
{
public:
    ECode Constructor(
        /* [in] */ Integer cap,
        /* [in] */ Integer lim);

    ECode Constructor(
        /* [in] */ Array<Byte>& buf,
        /* [in] */ Integer off,
        /* [in] */ Integer len);
};

}
}

#endif // __CCM_IO_HEAPBYTEBUFFER_H__
