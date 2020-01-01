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

#ifndef __COMO_IO_CDIRECTBYTEBUFFER_H__
#define __COMO_IO_CDIRECTBYTEBUFFER_H__

#include "como/io/DirectByteBuffer.h"
#include "_como_io_CDirectByteBuffer.h"

namespace como {
namespace io {

Coclass(CDirectByteBuffer)
    , public DirectByteBuffer
{
public:
    COMO_OBJECT_DECL();

    static ECode New(
        /* [in] */ Integer capacity,
        /* [in] */ MemoryRef* memoryRef,
        /* [in] */ const InterfaceID& iid,
        /* [out] */ como::IInterface** object);

    static ECode New(
        /* [in] */ HANDLE addr,
        /* [in] */ Integer cap,
        /* [in] */ const InterfaceID& iid,
        /* [out] */ como::IInterface** object);

    static ECode New(
        /* [in] */ MemoryRef* memoryRef,
        /* [in] */ Integer mark,
        /* [in] */ Integer pos,
        /* [in] */ Integer lim,
        /* [in] */ Integer cap,
        /* [in] */ Integer off,
        /* [in] */ const InterfaceID& iid,
        /* [out] */ como::IInterface** object);

    static ECode New(
        /* [in] */ MemoryRef* memoryRef,
        /* [in] */ Integer mark,
        /* [in] */ Integer pos,
        /* [in] */ Integer lim,
        /* [in] */ Integer cap,
        /* [in] */ Integer off,
        /* [in] */ Boolean isReadOnly,
        /* [in] */ const InterfaceID& iid,
        /* [out] */ como::IInterface** object);

    using _CDirectByteBuffer::New;
};

}
}

#endif // __COMO_IO_CDIRECTBYTEBUFFER_H__
