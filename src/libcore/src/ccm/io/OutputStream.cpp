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

#include "ccm/io/OutputStream.h"

using ccm::core::IID_IAutoCloseable;

namespace ccm {
namespace io {

CCM_INTERFACE_IMPL_4(OutputStream, SyncObject, IOutputStream, IFlushable, ICloseable, IAutoCloseable);

ECode OutputStream::Write(
    /* [in] */ const Array<Byte>& buffer)
{
    return Write(buffer, 0, buffer.GetLength());
}

ECode OutputStream::Write(
    /* [in] */ const Array<Byte>& buffer,
    /* [in] */ Integer offset,
    /* [in] */ Integer size)
{
    if ((offset < 0) || (offset > buffer.GetLength()) || (size < 0) ||
            ((offset + size) > buffer.GetLength()) || ((offset + size) < 0)) {
        return ccm::core::E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    else if (size == 0) {
        return NOERROR;
    }
    for (Integer i = 0; i < size; i++) {
        FAIL_RETURN(Write(buffer[offset + i]));
    }
    return NOERROR;
}

ECode OutputStream::Flush()
{
    return NOERROR;
}

ECode OutputStream::Close()
{
    return NOERROR;
}

}
}
