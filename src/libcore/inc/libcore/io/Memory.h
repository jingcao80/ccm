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
    static void UnsafeBulkGet(
        /* [out] */ Array<Char>& dst,
        /* [in] */ Integer dstOffset,
        /* [in] */ Integer byteCount,
        /* [in] */ const Array<Byte>& src,
        /* [in] */ Integer srcOffset,
        /* [in] */ Integer sizeofElement,
        /* [in] */ Boolean swap);

    static void UnsafeBulkGet(
        /* [out] */ Array<Short>& dst,
        /* [in] */ Integer dstOffset,
        /* [in] */ Integer byteCount,
        /* [in] */ const Array<Byte>& src,
        /* [in] */ Integer srcOffset,
        /* [in] */ Integer sizeofElement,
        /* [in] */ Boolean swap);

    static void UnsafeBulkGet(
        /* [out] */ Array<Integer>& dst,
        /* [in] */ Integer dstOffset,
        /* [in] */ Integer byteCount,
        /* [in] */ const Array<Byte>& src,
        /* [in] */ Integer srcOffset,
        /* [in] */ Integer sizeofElement,
        /* [in] */ Boolean swap);

    static void UnsafeBulkGet(
        /* [out] */ Array<Long>& dst,
        /* [in] */ Integer dstOffset,
        /* [in] */ Integer byteCount,
        /* [in] */ const Array<Byte>& src,
        /* [in] */ Integer srcOffset,
        /* [in] */ Integer sizeofElement,
        /* [in] */ Boolean swap);

    static void UnsafeBulkGet(
        /* [out] */ Array<Float>& dst,
        /* [in] */ Integer dstOffset,
        /* [in] */ Integer byteCount,
        /* [in] */ const Array<Byte>& src,
        /* [in] */ Integer srcOffset,
        /* [in] */ Integer sizeofElement,
        /* [in] */ Boolean swap);

    static void UnsafeBulkGet(
        /* [out] */ Array<Double>& dst,
        /* [in] */ Integer dstOffset,
        /* [in] */ Integer byteCount,
        /* [in] */ const Array<Byte>& src,
        /* [in] */ Integer srcOffset,
        /* [in] */ Integer sizeofElement,
        /* [in] */ Boolean swap);

    static void UnsafeBulkPut(
        /* [out] */ Array<Byte>& dst,
        /* [in] */ Integer dstOffset,
        /* [in] */ Integer byteCount,
        /* [in] */ const Array<Char>& src,
        /* [in] */ Integer srcOffset,
        /* [in] */ Integer sizeofElement,
        /* [in] */ Boolean swap);

    static void UnsafeBulkPut(
        /* [out] */ Array<Byte>& dst,
        /* [in] */ Integer dstOffset,
        /* [in] */ Integer byteCount,
        /* [in] */ const Array<Short>& src,
        /* [in] */ Integer srcOffset,
        /* [in] */ Integer sizeofElement,
        /* [in] */ Boolean swap);

    static void UnsafeBulkPut(
        /* [out] */ Array<Byte>& dst,
        /* [in] */ Integer dstOffset,
        /* [in] */ Integer byteCount,
        /* [in] */ const Array<Integer>& src,
        /* [in] */ Integer srcOffset,
        /* [in] */ Integer sizeofElement,
        /* [in] */ Boolean swap);

    static void UnsafeBulkPut(
        /* [out] */ Array<Byte>& dst,
        /* [in] */ Integer dstOffset,
        /* [in] */ Integer byteCount,
        /* [in] */ const Array<Long>& src,
        /* [in] */ Integer srcOffset,
        /* [in] */ Integer sizeofElement,
        /* [in] */ Boolean swap);

    static void UnsafeBulkPut(
        /* [out] */ Array<Byte>& dst,
        /* [in] */ Integer dstOffset,
        /* [in] */ Integer byteCount,
        /* [in] */ const Array<Float>& src,
        /* [in] */ Integer srcOffset,
        /* [in] */ Integer sizeofElement,
        /* [in] */ Boolean swap);

    static void UnsafeBulkPut(
        /* [out] */ Array<Byte>& dst,
        /* [in] */ Integer dstOffset,
        /* [in] */ Integer byteCount,
        /* [in] */ const Array<Double>& src,
        /* [in] */ Integer srcOffset,
        /* [in] */ Integer sizeofElement,
        /* [in] */ Boolean swap);

    static Byte PeekByte(
        /* [in] */ HANDLE address);

    static Integer PeekInteger(
        /* [in] */ HANDLE address,
        /* [in] */ Boolean swap);

    static Short PeekShort(
        /* [in] */ HANDLE address,
        /* [in] */ Boolean swap);

    static ECode PeekByteArray(
        /* [in] */ HANDLE address,
        /* [out] */ Array<Byte>& dst,
        /* [in] */ Integer dstOffset,
        /* [in] */ Integer byteCount);

    static ECode PeekIntegerArray(
        /* [in] */ HANDLE address,
        /* [out] */ Array<Integer>& dst,
        /* [in] */ Integer dstOffset,
        /* [in] */ Integer intCount,
        /* [in] */ Boolean swap);

private:
    Memory();
};

}
}

#endif // __LIBCORE_IO_MEMORY_H__
