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

    static Long PeekLong(
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

    static ECode PeekCharArray(
        /* [in] */ HANDLE address,
        /* [out] */ Array<Char>& dst,
        /* [in] */ Integer dstOffset,
        /* [in] */ Integer charCount,
        /* [in] */ Boolean swap);

    static ECode PeekDoubleArray(
        /* [in] */ HANDLE address,
        /* [out] */ Array<Double>& dst,
        /* [in] */ Integer dstOffset,
        /* [in] */ Integer doubleCount,
        /* [in] */ Boolean swap);

    static ECode PeekFloatArray(
        /* [in] */ HANDLE address,
        /* [out] */ Array<Float>& dst,
        /* [in] */ Integer dstOffset,
        /* [in] */ Integer floatCount,
        /* [in] */ Boolean swap);

    static ECode PeekIntegerArray(
        /* [in] */ HANDLE address,
        /* [out] */ Array<Integer>& dst,
        /* [in] */ Integer dstOffset,
        /* [in] */ Integer intCount,
        /* [in] */ Boolean swap);

    static ECode PeekLongArray(
        /* [in] */ HANDLE address,
        /* [out] */ Array<Long>& dst,
        /* [in] */ Integer dstOffset,
        /* [in] */ Integer longCount,
        /* [in] */ Boolean swap);

    static ECode PeekShortArray(
        /* [in] */ HANDLE address,
        /* [out] */ Array<Short>& dst,
        /* [in] */ Integer dstOffset,
        /* [in] */ Integer shortCount,
        /* [in] */ Boolean swap);

    static ECode PokeByte(
        /* [in] */ HANDLE address,
        /* [in] */ Byte value);

    static ECode PokeInteger(
        /* [in] */ HANDLE address,
        /* [in] */ Integer value,
        /* [in] */ Boolean swap);

    static ECode PokeLong(
        /* [in] */ HANDLE address,
        /* [in] */ Long value,
        /* [in] */ Boolean swap);

    static ECode PokeShort(
        /* [in] */ HANDLE address,
        /* [in] */ Short value,
        /* [in] */ Boolean swap);

    static ECode PokeByteArray(
        /* [in] */ HANDLE address,
        /* [in] */ const Array<Byte>& src,
        /* [in] */ Integer offset,
        /* [in] */ Integer count);

    static ECode PokeCharArray(
        /* [in] */ HANDLE address,
        /* [in] */ const Array<Char>& src,
        /* [in] */ Integer offset,
        /* [in] */ Integer count,
        /* [in] */ Boolean swap);

    static ECode PokeDoubleArray(
        /* [in] */ HANDLE address,
        /* [in] */ const Array<Double>& src,
        /* [in] */ Integer offset,
        /* [in] */ Integer count,
        /* [in] */ Boolean swap);

    static ECode PokeFloatArray(
        /* [in] */ HANDLE address,
        /* [in] */ const Array<Float>& src,
        /* [in] */ Integer offset,
        /* [in] */ Integer count,
        /* [in] */ Boolean swap);

    static ECode PokeIntegerArray(
        /* [in] */ HANDLE address,
        /* [in] */ const Array<Integer>& src,
        /* [in] */ Integer offset,
        /* [in] */ Integer count,
        /* [in] */ Boolean swap);

    static ECode PokeLongArray(
        /* [in] */ HANDLE address,
        /* [in] */ const Array<Long>& src,
        /* [in] */ Integer offset,
        /* [in] */ Integer count,
        /* [in] */ Boolean swap);

    static ECode PokeShortArray(
        /* [in] */ HANDLE address,
        /* [in] */ const Array<Short>& src,
        /* [in] */ Integer offset,
        /* [in] */ Integer count,
        /* [in] */ Boolean swap);

private:
    Memory();
};

}
}

#endif // __LIBCORE_IO_MEMORY_H__
