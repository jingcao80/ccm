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

static Integer bswap_2x16(Integer v)
{
    // v is initially ABCD
    v = bswap_32(v);                        // v=DCBA
    v = (v << 16) | ((v >> 16) & 0xffff);   // v=BADC
    return v;
}

static void SwapShorts(
    /* [in] */ Short* dstShorts,
    /* [in] */ const Short* srcShorts,
    /* [in] */ size_t count)
{
    Integer* dst = reinterpret_cast<Integer*>(dstShorts);
    const Integer* src = reinterpret_cast<const Integer*>(srcShorts);
    for (size_t i = 0; i < count / 2; ++i) {
        Integer v = GetUnaligned<Integer>(src++);
        PutUnaligned<Integer>(dst++, bswap_2x16(v));
    }
    if ((count % 2) != 0) {
        Short v = GetUnaligned<Short>(reinterpret_cast<const Short*>(src));
        PutUnaligned<Short>(reinterpret_cast<Short*>(dst), bswap_16(v));
    }
}

static void SwapIntegers(
    /* [in] */ Integer* dstInts,
    /* [in] */ const Integer* srcInts,
    /* [in] */ size_t count)
{
    for (size_t i = 0; i < count; ++i) {
        Integer v = GetUnaligned<Integer>(srcInts++);
        PutUnaligned<Integer>(dstInts++, bswap_32(v));
    }
}

static void SwapLongs(
    /* [in] */ Long* dstLongs,
    /* [in] */ const Long* srcLongs,
    /* [in] */ size_t count)
{
    Integer* dst = reinterpret_cast<Integer*>(dstLongs);
    const Integer* src = reinterpret_cast<const Integer*>(srcLongs);
    for (size_t i = 0; i < count; ++i) {
        Integer v1 = GetUnaligned<Integer>(src++);
        Integer v2 = GetUnaligned<Integer>(src++);
        PutUnaligned<Integer>(dst++, bswap_32(v2));
        PutUnaligned<Integer>(dst++, bswap_32(v1));
    }
}

static void UnsafeBulkCopy(
    /* [in] */ Byte* dst,
    /* [in] */ const Byte* src,
    /* [in] */ Integer byteCount,
    /* [in] */ Integer sizeofElement,
    /* [in] */ Boolean swap)
{
    if (!swap) {
        memcpy(dst, src, byteCount);
        return;
    }

    if (sizeofElement == 2) {
        Short* dstShorts = reinterpret_cast<Short*>(dst);
        const Short* srcShorts = reinterpret_cast<const Short*>(src);
        SwapShorts(dstShorts, srcShorts, byteCount / 2);
    }
    else if (sizeofElement == 4) {
        Integer* dstInts = reinterpret_cast<Integer*>(dst);
        const Integer* srcInts = reinterpret_cast<const Integer*>(src);
        SwapIntegers(dstInts, srcInts, byteCount / 4);
    }
    else if (sizeofElement == 8) {
        Long* dstLongs = reinterpret_cast<Long*>(dst);
        const Long* srcLongs = reinterpret_cast<const Long*>(src);
        SwapLongs(dstLongs, srcLongs, byteCount / 8);
    }
}

void Memory::UnsafeBulkGet(
    /* [out] */ Array<Char>& dstArray,
    /* [in] */ Integer dstOffset,
    /* [in] */ Integer byteCount,
    /* [in] */ const Array<Byte>& srcArray,
    /* [in] */ Integer srcOffset,
    /* [in] */ Integer sizeofElement,
    /* [in] */ Boolean swap)
{
    if (srcArray.IsNull() || dstArray.IsNull()) {
        return;
    }
    Byte* dstBytes = reinterpret_cast<Byte*>(dstArray.GetPayload());
    Byte* dst = dstBytes + dstOffset * sizeofElement;
    const Byte* src = srcArray.GetPayload() + srcOffset;
    UnsafeBulkCopy(dst, src, byteCount, sizeofElement, swap);
}

void Memory::UnsafeBulkGet(
    /* [out] */ Array<Short>& dstArray,
    /* [in] */ Integer dstOffset,
    /* [in] */ Integer byteCount,
    /* [in] */ const Array<Byte>& srcArray,
    /* [in] */ Integer srcOffset,
    /* [in] */ Integer sizeofElement,
    /* [in] */ Boolean swap)
{
    if (srcArray.IsNull() || dstArray.IsNull()) {
        return;
    }
    Byte* dstBytes = reinterpret_cast<Byte*>(dstArray.GetPayload());
    Byte* dst = dstBytes + dstOffset * sizeofElement;
    const Byte* src = srcArray.GetPayload() + srcOffset;
    UnsafeBulkCopy(dst, src, byteCount, sizeofElement, swap);
}

void Memory::UnsafeBulkGet(
    /* [out] */ Array<Integer>& dstArray,
    /* [in] */ Integer dstOffset,
    /* [in] */ Integer byteCount,
    /* [in] */ const Array<Byte>& srcArray,
    /* [in] */ Integer srcOffset,
    /* [in] */ Integer sizeofElement,
    /* [in] */ Boolean swap)
{
    if (srcArray.IsNull() || dstArray.IsNull()) {
        return;
    }
    Byte* dstBytes = reinterpret_cast<Byte*>(dstArray.GetPayload());
    Byte* dst = dstBytes + dstOffset * sizeofElement;
    const Byte* src = srcArray.GetPayload() + srcOffset;
    UnsafeBulkCopy(dst, src, byteCount, sizeofElement, swap);
}

void Memory::UnsafeBulkGet(
    /* [out] */ Array<Long>& dstArray,
    /* [in] */ Integer dstOffset,
    /* [in] */ Integer byteCount,
    /* [in] */ const Array<Byte>& srcArray,
    /* [in] */ Integer srcOffset,
    /* [in] */ Integer sizeofElement,
    /* [in] */ Boolean swap)
{
    if (srcArray.IsNull() || dstArray.IsNull()) {
        return;
    }
    Byte* dstBytes = reinterpret_cast<Byte*>(dstArray.GetPayload());
    Byte* dst = dstBytes + dstOffset * sizeofElement;
    const Byte* src = srcArray.GetPayload() + srcOffset;
    UnsafeBulkCopy(dst, src, byteCount, sizeofElement, swap);
}

void Memory::UnsafeBulkGet(
    /* [out] */ Array<Float>& dstArray,
    /* [in] */ Integer dstOffset,
    /* [in] */ Integer byteCount,
    /* [in] */ const Array<Byte>& srcArray,
    /* [in] */ Integer srcOffset,
    /* [in] */ Integer sizeofElement,
    /* [in] */ Boolean swap)
{
    if (srcArray.IsNull() || dstArray.IsNull()) {
        return;
    }
    Byte* dstBytes = reinterpret_cast<Byte*>(dstArray.GetPayload());
    Byte* dst = dstBytes + dstOffset * sizeofElement;
    const Byte* src = srcArray.GetPayload() + srcOffset;
    UnsafeBulkCopy(dst, src, byteCount, sizeofElement, swap);
}

void Memory::UnsafeBulkGet(
    /* [out] */ Array<Double>& dstArray,
    /* [in] */ Integer dstOffset,
    /* [in] */ Integer byteCount,
    /* [in] */ const Array<Byte>& srcArray,
    /* [in] */ Integer srcOffset,
    /* [in] */ Integer sizeofElement,
    /* [in] */ Boolean swap)
{
    if (srcArray.IsNull() || dstArray.IsNull()) {
        return;
    }
    Byte* dstBytes = reinterpret_cast<Byte*>(dstArray.GetPayload());
    Byte* dst = dstBytes + dstOffset * sizeofElement;
    const Byte* src = srcArray.GetPayload() + srcOffset;
    UnsafeBulkCopy(dst, src, byteCount, sizeofElement, swap);
}

void Memory::UnsafeBulkPut(
    /* [out] */ Array<Byte>& dstArray,
    /* [in] */ Integer dstOffset,
    /* [in] */ Integer byteCount,
    /* [in] */ const Array<Char>& srcArray,
    /* [in] */ Integer srcOffset,
    /* [in] */ Integer sizeofElement,
    /* [in] */ Boolean swap)
{
    if (dstArray.IsNull() || srcArray.IsNull()) {
        return;
    }
    Byte* srcBytes = reinterpret_cast<Byte*>(srcArray.GetPayload());
    const Byte* src = srcBytes + srcOffset * sizeofElement;
    Byte* dst = dstArray.GetPayload() + dstOffset;
    UnsafeBulkCopy(dst, src, byteCount, sizeofElement, swap);
}

void Memory::UnsafeBulkPut(
    /* [out] */ Array<Byte>& dstArray,
    /* [in] */ Integer dstOffset,
    /* [in] */ Integer byteCount,
    /* [in] */ const Array<Short>& srcArray,
    /* [in] */ Integer srcOffset,
    /* [in] */ Integer sizeofElement,
    /* [in] */ Boolean swap)
{
    if (dstArray.IsNull() || srcArray.IsNull()) {
        return;
    }
    Byte* srcBytes = reinterpret_cast<Byte*>(srcArray.GetPayload());
    const Byte* src = srcBytes + srcOffset * sizeofElement;
    Byte* dst = dstArray.GetPayload() + dstOffset;
    UnsafeBulkCopy(dst, src, byteCount, sizeofElement, swap);
}

void Memory::UnsafeBulkPut(
    /* [out] */ Array<Byte>& dstArray,
    /* [in] */ Integer dstOffset,
    /* [in] */ Integer byteCount,
    /* [in] */ const Array<Integer>& srcArray,
    /* [in] */ Integer srcOffset,
    /* [in] */ Integer sizeofElement,
    /* [in] */ Boolean swap)
{
    if (dstArray.IsNull() || srcArray.IsNull()) {
        return;
    }
    Byte* srcBytes = reinterpret_cast<Byte*>(srcArray.GetPayload());
    const Byte* src = srcBytes + srcOffset * sizeofElement;
    Byte* dst = dstArray.GetPayload() + dstOffset;
    UnsafeBulkCopy(dst, src, byteCount, sizeofElement, swap);
}

void Memory::UnsafeBulkPut(
    /* [out] */ Array<Byte>& dstArray,
    /* [in] */ Integer dstOffset,
    /* [in] */ Integer byteCount,
    /* [in] */ const Array<Long>& srcArray,
    /* [in] */ Integer srcOffset,
    /* [in] */ Integer sizeofElement,
    /* [in] */ Boolean swap)
{
    if (dstArray.IsNull() || srcArray.IsNull()) {
        return;
    }
    Byte* srcBytes = reinterpret_cast<Byte*>(srcArray.GetPayload());
    const Byte* src = srcBytes + srcOffset * sizeofElement;
    Byte* dst = dstArray.GetPayload() + dstOffset;
    UnsafeBulkCopy(dst, src, byteCount, sizeofElement, swap);
}

void Memory::UnsafeBulkPut(
    /* [out] */ Array<Byte>& dstArray,
    /* [in] */ Integer dstOffset,
    /* [in] */ Integer byteCount,
    /* [in] */ const Array<Float>& srcArray,
    /* [in] */ Integer srcOffset,
    /* [in] */ Integer sizeofElement,
    /* [in] */ Boolean swap)
{
    if (dstArray.IsNull() || srcArray.IsNull()) {
        return;
    }
    Byte* srcBytes = reinterpret_cast<Byte*>(srcArray.GetPayload());
    const Byte* src = srcBytes + srcOffset * sizeofElement;
    Byte* dst = dstArray.GetPayload() + dstOffset;
    UnsafeBulkCopy(dst, src, byteCount, sizeofElement, swap);
}

void Memory::UnsafeBulkPut(
    /* [out] */ Array<Byte>& dstArray,
    /* [in] */ Integer dstOffset,
    /* [in] */ Integer byteCount,
    /* [in] */ const Array<Double>& srcArray,
    /* [in] */ Integer srcOffset,
    /* [in] */ Integer sizeofElement,
    /* [in] */ Boolean swap)
{
    if (dstArray.IsNull() || srcArray.IsNull()) {
        return;
    }
    Byte* srcBytes = reinterpret_cast<Byte*>(srcArray.GetPayload());
    const Byte* src = srcBytes + srcOffset * sizeofElement;
    Byte* dst = dstArray.GetPayload() + dstOffset;
    UnsafeBulkCopy(dst, src, byteCount, sizeofElement, swap);
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

Long Memory::PeekLong(
    /* [in] */ HANDLE address,
    /* [in] */ Boolean swap)
{
    Long result = GetUnaligned<Long>(reinterpret_cast<Long*>(address));
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

ECode Memory::PeekCharArray(
    /* [in] */ HANDLE address,
    /* [out] */ Array<Char>& dst,
    /* [in] */ Integer dstOffset,
    /* [in] */ Integer charCount,
    /* [in] */ Boolean swap)
{
    if (swap) {
        SwapIntegers(reinterpret_cast<Integer*>(dst.GetPayload()) + dstOffset,
                reinterpret_cast<Integer*>(address), charCount);
    }
    else {
        dst.Copy(dstOffset, reinterpret_cast<Char*>(address), charCount);
    }
    return NOERROR;
}

ECode Memory::PeekDoubleArray(
    /* [in] */ HANDLE address,
    /* [out] */ Array<Double>& dst,
    /* [in] */ Integer dstOffset,
    /* [in] */ Integer doubleCount,
    /* [in] */ Boolean swap)
{
    if (swap) {
        SwapLongs(reinterpret_cast<Long*>(dst.GetPayload()) + dstOffset,
                reinterpret_cast<Long*>(address), doubleCount);
    }
    else {
        dst.Copy(dstOffset, reinterpret_cast<Double*>(address), doubleCount);
    }
    return NOERROR;
}

ECode Memory::PeekFloatArray(
    /* [in] */ HANDLE address,
    /* [out] */ Array<Float>& dst,
    /* [in] */ Integer dstOffset,
    /* [in] */ Integer floatCount,
    /* [in] */ Boolean swap)
{
    if (swap) {
        SwapIntegers(reinterpret_cast<Integer*>(dst.GetPayload()) + dstOffset,
                reinterpret_cast<Integer*>(address), floatCount);
    }
    else {
        dst.Copy(dstOffset, reinterpret_cast<Float*>(address), floatCount);
    }
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

ECode Memory::PeekLongArray(
    /* [in] */ HANDLE address,
    /* [out] */ Array<Long>& dst,
    /* [in] */ Integer dstOffset,
    /* [in] */ Integer longCount,
    /* [in] */ Boolean swap)
{
    if (swap) {
        SwapLongs(dst.GetPayload() + dstOffset, reinterpret_cast<Long*>(address), longCount);
    }
    else {
        dst.Copy(dstOffset, reinterpret_cast<Long*>(address), longCount);
    }
    return NOERROR;
}

ECode Memory::PeekShortArray(
    /* [in] */ HANDLE address,
    /* [out] */ Array<Short>& dst,
    /* [in] */ Integer dstOffset,
    /* [in] */ Integer shortCount,
    /* [in] */ Boolean swap)
{
    if (swap) {
        SwapShorts(dst.GetPayload() + dstOffset, reinterpret_cast<Short*>(address), shortCount);
    }
    else {
        dst.Copy(dstOffset, reinterpret_cast<Short*>(address), shortCount);
    }
    return NOERROR;
}

ECode Memory::PokeByte(
    /* [in] */ HANDLE address,
    /* [in] */ Byte value)
{
    *reinterpret_cast<Byte*>(address) = value;
    return NOERROR;
}

ECode Memory::PokeInteger(
    /* [in] */ HANDLE address,
    /* [in] */ Integer value,
    /* [in] */ Boolean swap)
{
    if (swap) {
        value = Math::ReverseBytes(value);
    }
    PutUnaligned<Integer>(reinterpret_cast<Integer*>(address), value);
    return NOERROR;
}

ECode Memory::PokeLong(
    /* [in] */ HANDLE address,
    /* [in] */ Long value,
    /* [in] */ Boolean swap)
{
    if (swap) {
        value = Math::ReverseBytes(value);
    }
    PutUnaligned<Long>(reinterpret_cast<Long*>(address), value);
    return NOERROR;
}

ECode Memory::PokeShort(
    /* [in] */ HANDLE address,
    /* [in] */ Short value,
    /* [in] */ Boolean swap)
{
    if (swap) {
        value = Math::ReverseBytes(value);
    }
    PutUnaligned<Short>(reinterpret_cast<Short*>(address), value);
    return NOERROR;
}

ECode Memory::PokeByteArray(
    /* [in] */ HANDLE address,
    /* [in] */ const Array<Byte>& src,
    /* [in] */ Integer offset,
    /* [in] */ Integer count)
{
    memcpy(reinterpret_cast<Byte*>(address), (src.GetPayload() + offset), count);
    return NOERROR;
}

ECode Memory::PokeCharArray(
    /* [in] */ HANDLE address,
    /* [in] */ const Array<Char>& src,
    /* [in] */ Integer offset,
    /* [in] */ Integer count,
    /* [in] */ Boolean swap)
{
    if (swap) {
        SwapIntegers(reinterpret_cast<Integer*>(address),
                reinterpret_cast<Integer*>(src.GetPayload() + offset), count);
    }
    else {
        memcpy(reinterpret_cast<Integer*>(address),
                reinterpret_cast<Integer*>(src.GetPayload() + offset), count * sizeof(Integer));
    }
    return NOERROR;
}

ECode Memory::PokeDoubleArray(
    /* [in] */ HANDLE address,
    /* [in] */ const Array<Double>& src,
    /* [in] */ Integer offset,
    /* [in] */ Integer count,
    /* [in] */ Boolean swap)
{
    if (swap) {
        SwapLongs(reinterpret_cast<Long*>(address),
                reinterpret_cast<Long*>(src.GetPayload() + offset), count);
    }
    else {
        memcpy(reinterpret_cast<Long*>(address),
                reinterpret_cast<Long*>(src.GetPayload() + offset), count * sizeof(Long));
    }
    return NOERROR;
}

ECode Memory::PokeFloatArray(
    /* [in] */ HANDLE address,
    /* [in] */ const Array<Float>& src,
    /* [in] */ Integer offset,
    /* [in] */ Integer count,
    /* [in] */ Boolean swap)
{
    if (swap) {
        SwapIntegers(reinterpret_cast<Integer*>(address),
                reinterpret_cast<Integer*>(src.GetPayload() + offset), count);
    }
    else {
        memcpy(reinterpret_cast<Integer*>(address),
                reinterpret_cast<Integer*>(src.GetPayload() + offset), count * sizeof(Integer));
    }
    return NOERROR;
}

ECode Memory::PokeIntegerArray(
    /* [in] */ HANDLE address,
    /* [in] */ const Array<Integer>& src,
    /* [in] */ Integer offset,
    /* [in] */ Integer count,
    /* [in] */ Boolean swap)
{
    if (swap) {
        SwapIntegers(reinterpret_cast<Integer*>(address),
                reinterpret_cast<Integer*>(src.GetPayload() + offset), count);
    }
    else {
        memcpy(reinterpret_cast<Integer*>(address),
                reinterpret_cast<Integer*>(src.GetPayload() + offset), count * sizeof(Integer));
    }
    return NOERROR;
}

ECode Memory::PokeLongArray(
    /* [in] */ HANDLE address,
    /* [in] */ const Array<Long>& src,
    /* [in] */ Integer offset,
    /* [in] */ Integer count,
    /* [in] */ Boolean swap)
{
    if (swap) {
        SwapLongs(reinterpret_cast<Long*>(address),
                reinterpret_cast<Long*>(src.GetPayload() + offset), count);
    }
    else {
        memcpy(reinterpret_cast<Long*>(address),
                reinterpret_cast<Long*>(src.GetPayload() + offset), count * sizeof(Long));
    }
    return NOERROR;
}

ECode Memory::PokeShortArray(
    /* [in] */ HANDLE address,
    /* [in] */ const Array<Short>& src,
    /* [in] */ Integer offset,
    /* [in] */ Integer count,
    /* [in] */ Boolean swap)
{
    if (swap) {
        SwapShorts(reinterpret_cast<Short*>(address),
                reinterpret_cast<Short*>(src.GetPayload() + offset), count);
    }
    else {
        memcpy(reinterpret_cast<Short*>(address),
                reinterpret_cast<Short*>(src.GetPayload() + offset), count * sizeof(Short));
    }
    return NOERROR;
}

}
}
