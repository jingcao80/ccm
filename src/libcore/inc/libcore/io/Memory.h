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

#ifndef __LIBCORE_IO_MEMORY_H__
#define __LIBCORE_IO_MEMORY_H__

#include <ccmtypes.h>

using namespace ccm;

namespace libcore {
namespace io {

class Memory
{
public:
    static Byte PeekByte(
        /* [in] */ HANDLE address)
    {
        return 0;
    }

    static Integer PeekInteger(
        /* [in] */ HANDLE address,
        /* [in] */ Boolean swap)
    {
        return 0;
    }

    static Short PeekShort(
        /* [in] */ HANDLE address,
        /* [in] */ Boolean swap)
    {
        return 0;
    }

    static ECode PeekByteArray(
        /* [in] */ HANDLE address,
        /* [out] */ Array<Byte>& dst,
        /* [in] */ Integer dstOffset,
        /* [in] */ Integer byteCount)
    {
        return NOERROR;
    }

    static ECode PeekIntegerArray(
        /* [in] */ HANDLE address,
        /* [out] */ Array<Integer>& dst,
        /* [in] */ Integer dstOffset,
        /* [in] */ Integer intCount,
        /* [in] */ Boolean swap)
    {
        return NOERROR;
    }

private:
    Memory();
};

}
}

#endif // __LIBCORE_IO_MEMORY_H__
