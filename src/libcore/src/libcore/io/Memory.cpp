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

#include "ccm/core/Math.h"
#include "libcore/io/Memory.h"
#include <byteswap.h>

using ccm::core::Math;

namespace libcore {
namespace io {

// Use packed structures for access to unaligned data on targets with alignment restrictions.
// The compiler will generate appropriate code to access these structures without
// generating alignment exceptions.
template <typename T> static inline T GetUnaligned(const T* address)
{
    struct unaligned { T v; } __attribute__ ((packed));
    const unaligned* p = reinterpret_cast<const unaligned*>(address);
    return p->v;
}

template <typename T> static inline void PutUnaligned(T* address, T v)
{
    struct unaligned { T v; } __attribute__ ((packed));
    unaligned* p = reinterpret_cast<unaligned*>(address);
    p->v = v;
}

static inline void SwapIntegers(Integer* dstInts, const Integer* srcInts, size_t count)
{
    for (size_t i = 0; i < count; ++i) {
        Integer v = GetUnaligned<Integer>(srcInts++);
        PutUnaligned<Integer>(dstInts++, bswap_32(v));
    }
}

Byte Memory::PeekByte(
    /* [in] */ HANDLE address)
{
    return *reinterpret_cast<Byte*>(address);
}

Integer Memory::PeekInteger(
    /* [in] */ HANDLE address,
    /* [in] */ Boolean swap)
{
    Integer result = GetUnaligned<Integer>(reinterpret_cast<Integer*>(address));
    if (swap) {
        result = Math::ReverseBytes(result);
    }
    return result;
}

Short Memory::PeekShort(
    /* [in] */ HANDLE address,
    /* [in] */ Boolean swap)
{
    Short result = GetUnaligned<Short>(reinterpret_cast<Short*>(address));
    if (swap) {
        result = Math::ReverseBytes(result);
    }
    return result;
}

ECode Memory::PeekByteArray(
    /* [in] */ HANDLE address,
    /* [out] */ Array<Byte>& dst,
    /* [in] */ Integer dstOffset,
    /* [in] */ Integer byteCount)
{
    dst.Copy(dstOffset, reinterpret_cast<Byte*>(address), byteCount);
    return NOERROR;
}

ECode Memory::PeekIntegerArray(
    /* [in] */ HANDLE address,
    /* [out] */ Array<Integer>& dst,
    /* [in] */ Integer dstOffset,
    /* [in] */ Integer intCount,
    /* [in] */ Boolean swap)
{
    if (swap) {
        SwapIntegers(dst.GetPayload() + dstOffset, reinterpret_cast<Integer*>(address), intCount);
    }
    else {
        dst.Copy(dstOffset, reinterpret_cast<Integer*>(address), intCount);
    }
    return NOERROR;
}

}
}
