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

#include "como/io/ByteBuffer.h"
#include "como/io/ByteBufferFactory.h"

namespace como {
namespace io {

ECode ByteBufferFactory::AllocateDirect(
    /* [in] */ Integer capacity,
    /* [out] */ IByteBuffer** buffer)
{
    return ByteBuffer::AllocateDirect(capacity, buffer);
}

ECode ByteBufferFactory::Allocate(
    /* [in] */ Integer capacity,
    /* [out] */ IByteBuffer** buffer)
{
    return ByteBuffer::Allocate(capacity, buffer);
}

ECode ByteBufferFactory::Wrap(
    /* [in] */ const Array<Byte>& array,
    /* [in] */ Integer offset,
    /* [in] */ Integer length,
    /* [out] */ IByteBuffer** buffer)
{
    return ByteBuffer::Wrap(array, offset, length, buffer);
}

ECode ByteBufferFactory::Wrap(
    /* [in] */ const Array<Byte>& array,
    /* [out] */ IByteBuffer** buffer)
{
    return ByteBuffer::Wrap(array, buffer);
}

}
}
