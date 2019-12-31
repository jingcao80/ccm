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
#include "ccm/io/Bits.h"
#include "ccm/io/ByteOrder.h"
#include <unistd.h>

using ccm::core::Math;

namespace ccm {
namespace io {

Integer Bits::sPageSize = -1;

Char Bits::GetCharL(
    /* [in] */ ByteBuffer* bb,
    /* [in] */ Integer bi)
{
    return MakeChar(bb->_Get(bi + 3),
            bb->_Get(bi + 2),
            bb->_Get(bi + 1),
            bb->_Get(bi));
}

Char Bits::GetCharB(
    /* [in] */ ByteBuffer* bb,
    /* [in] */ Integer bi)
{
    return MakeChar(bb->_Get(bi),
            bb->_Get(bi + 1),
            bb->_Get(bi + 2),
            bb->_Get(bi + 3));
}

Char Bits::GetChar(
    /* [in] */ ByteBuffer* bb,
    /* [in] */ Integer bi,
    /* [in] */ Boolean bigEndian)
{
    if (bigEndian) {
        return GetCharB(bb, bi);
    }
    else {
        return GetCharL(bb, bi);
    }
}

void Bits::PutCharL(
    /* [in] */ ByteBuffer* bb,
    /* [in] */ Integer bi,
    /* [in] */ Char x)
{
    bb->_Put(bi + 3, Char3(x));
    bb->_Put(bi + 2, Char2(x));
    bb->_Put(bi + 1, Char1(x));
    bb->_Put(bi, Char0(x));
}

void Bits::PutCharB(
    /* [in] */ ByteBuffer* bb,
    /* [in] */ Integer bi,
    /* [in] */ Char x)
{
    bb->_Put(bi, Char3(x));
    bb->_Put(bi + 1, Char2(x));
    bb->_Put(bi + 2, Char1(x));
    bb->_Put(bi + 3, Char0(x));
}

void Bits::PutChar(
    /* [in] */ ByteBuffer* bb,
    /* [in] */ Integer bi,
    /* [in] */ Char x,
    /* [in] */ Boolean bigEndian)
{
    if (bigEndian) {
        PutCharB(bb, bi, x);
    }
    else {
        PutCharL(bb, bi, x);
    }
}

Short Bits::GetShortL(
    /* [in] */ ByteBuffer* bb,
    /* [in] */ Integer bi)
{
    return MakeShort(bb->_Get(bi + 1),
            bb->_Get(bi));
}

Short Bits::GetShortB(
    /* [in] */ ByteBuffer* bb,
    /* [in] */ Integer bi)
{
    return MakeShort(bb->_Get(bi),
            bb->_Get(bi + 1));
}

Short Bits::GetShort(
    /* [in] */ ByteBuffer* bb,
    /* [in] */ Integer bi,
    /* [in] */ Boolean bigEndian)
{
    if (bigEndian) {
        return GetShortB(bb, bi);
    }
    else {
        return GetShortL(bb, bi);
    }
}

void Bits::PutShortL(
    /* [in] */ ByteBuffer* bb,
    /* [in] */ Integer bi,
    /* [in] */ Short x)
{
    bb->_Put(bi, Short0(x));
    bb->_Put(bi + 1, Short1(x));
}

void Bits::PutShortB(
    /* [in] */ ByteBuffer* bb,
    /* [in] */ Integer bi,
    /* [in] */ Short x)
{
    bb->_Put(bi, Short1(x));
    bb->_Put(bi + 1, Short0(x));
}

void Bits::PutShort(
    /* [in] */ ByteBuffer* bb,
    /* [in] */ Integer bi,
    /* [in] */ Short x,
    /* [in] */ Boolean bigEndian)
{
    if (bigEndian) {
        PutShortB(bb, bi, x);
    }
    else {
        PutShortL(bb, bi, x);
    }
}

Integer Bits::GetIntegerL(
    /* [in] */ ByteBuffer* bb,
    /* [in] */ Integer bi)
{
    return MakeInteger(bb->_Get(bi + 3),
            bb->_Get(bi + 2),
            bb->_Get(bi + 1),
            bb->_Get(bi));
}

Integer Bits::GetIntegerB(
    /* [in] */ ByteBuffer* bb,
    /* [in] */ Integer bi)
{
    return MakeInteger(bb->_Get(bi),
            bb->_Get(bi + 1),
            bb->_Get(bi + 2),
            bb->_Get(bi + 3));
}

Integer Bits::GetInteger(
    /* [in] */ ByteBuffer* bb,
    /* [in] */ Integer bi,
    /* [in] */ Boolean bigEndian)
{
    if (bigEndian) {
        return GetIntegerB(bb, bi);
    }
    else {
        return GetIntegerL(bb, bi);
    }
}

void Bits::PutIntegerL(
    /* [in] */ ByteBuffer* bb,
    /* [in] */ Integer bi,
    /* [in] */ Integer x)
{
    bb->_Put(bi + 3, Integer3(x));
    bb->_Put(bi + 2, Integer2(x));
    bb->_Put(bi + 1, Integer1(x));
    bb->_Put(bi, Integer0(x));
}

void Bits::PutIntegerB(
    /* [in] */ ByteBuffer* bb,
    /* [in] */ Integer bi,
    /* [in] */ Integer x)
{
    bb->_Put(bi, Integer3(x));
    bb->_Put(bi + 1, Integer2(x));
    bb->_Put(bi + 2, Integer1(x));
    bb->_Put(bi + 3, Integer0(x));
}

void Bits::PutInteger(
    /* [in] */ ByteBuffer* bb,
    /* [in] */ Integer bi,
    /* [in] */ Integer x,
    /* [in] */ Boolean bigEndian)
{
    if (bigEndian) {
        PutIntegerB(bb, bi, x);
    }
    else {
        PutIntegerL(bb, bi, x);
    }
}

Long Bits::GetLongL(
    /* [in] */ ByteBuffer* bb,
    /* [in] */ Integer bi)
{
    return MakeLong(bb->_Get(bi + 7),
            bb->_Get(bi + 6),
            bb->_Get(bi + 5),
            bb->_Get(bi + 4),
            bb->_Get(bi + 3),
            bb->_Get(bi + 2),
            bb->_Get(bi + 1),
            bb->_Get(bi));
}

Long Bits::GetLongB(
    /* [in] */ ByteBuffer* bb,
    /* [in] */ Integer bi)
{
    return MakeLong(bb->_Get(bi),
            bb->_Get(bi + 1),
            bb->_Get(bi + 2),
            bb->_Get(bi + 3),
            bb->_Get(bi + 4),
            bb->_Get(bi + 5),
            bb->_Get(bi + 6),
            bb->_Get(bi + 7));
}

Long Bits::GetLong(
    /* [in] */ ByteBuffer* bb,
    /* [in] */ Integer bi,
    /* [in] */ Boolean bigEndian)
{
    if (bigEndian) {
        return GetLongB(bb, bi);
    }
    else {
        return GetLongL(bb, bi);
    }
}

void Bits::PutLongL(
    /* [in] */ ByteBuffer* bb,
    /* [in] */ Integer bi,
    /* [in] */ Long x)
{
    bb->_Put(bi + 7, Long7(x));
    bb->_Put(bi + 6, Long6(x));
    bb->_Put(bi + 5, Long5(x));
    bb->_Put(bi + 4, Long4(x));
    bb->_Put(bi + 3, Long3(x));
    bb->_Put(bi + 2, Long2(x));
    bb->_Put(bi + 1, Long1(x));
    bb->_Put(bi, Long0(x));
}

void Bits::PutLongB(
    /* [in] */ ByteBuffer* bb,
    /* [in] */ Integer bi,
    /* [in] */ Long x)
{
    bb->_Put(bi, Long7(x));
    bb->_Put(bi + 1, Long6(x));
    bb->_Put(bi + 2, Long5(x));
    bb->_Put(bi + 3, Long4(x));
    bb->_Put(bi + 4, Long3(x));
    bb->_Put(bi + 5, Long2(x));
    bb->_Put(bi + 6, Long1(x));
    bb->_Put(bi + 7, Long0(x));
}

void Bits::PutLong(
    /* [in] */ ByteBuffer* bb,
    /* [in] */ Integer bi,
    /* [in] */ Long x,
    /* [in] */ Boolean bigEndian)
{
    if (bigEndian) {
        PutLongB(bb, bi, x);
    }
    else {
        PutLongL(bb, bi, x);
    }
}

Float Bits::GetFloatL(
    /* [in] */ ByteBuffer* bb,
    /* [in] */ Integer bi)
{
    return Math::IntegerBitsToFloat(GetIntegerL(bb, bi));
}

Float Bits::GetFloatB(
    /* [in] */ ByteBuffer* bb,
    /* [in] */ Integer bi)
{
    return Math::IntegerBitsToFloat(GetIntegerB(bb, bi));
}

Float Bits::GetFloat(
    /* [in] */ ByteBuffer* bb,
    /* [in] */ Integer bi,
    /* [in] */ Boolean bigEndian)
{
    if (bigEndian) {
        return GetFloatB(bb, bi);
    }
    else {
        return GetFloatL(bb, bi);
    }
}

void Bits::PutFloatL(
    /* [in] */ ByteBuffer* bb,
    /* [in] */ Integer bi,
    /* [in] */ Float x)
{
    PutIntegerL(bb, bi, Math::FloatToRawIntegerBits(x));
}

void Bits::PutFloatB(
    /* [in] */ ByteBuffer* bb,
    /* [in] */ Integer bi,
    /* [in] */ Float x)
{
    PutIntegerB(bb, bi, Math::FloatToRawIntegerBits(x));
}

void Bits::PutFloat(
    /* [in] */ ByteBuffer* bb,
    /* [in] */ Integer bi,
    /* [in] */ Float x,
    /* [in] */ Boolean bigEndian)
{
    if (bigEndian) {
        PutFloatB(bb, bi, x);
    }
    else {
        PutLongL(bb, bi, x);
    }
}

Double Bits::GetDoubleL(
    /* [in] */ ByteBuffer* bb,
    /* [in] */ Integer bi)
{
    return Math::LongBitsToDouble(GetLongL(bb, bi));
}

Double Bits::GetDoubleB(
    /* [in] */ ByteBuffer* bb,
    /* [in] */ Integer bi)
{
    return Math::LongBitsToDouble(GetLongB(bb, bi));
}

Double Bits::GetDouble(
    /* [in] */ ByteBuffer* bb,
    /* [in] */ Integer bi,
    /* [in] */ Boolean bigEndian)
{
    if (bigEndian) {
        return GetDoubleB(bb, bi);
    }
    else {
        return GetDoubleL(bb, bi);
    }
}

void Bits::PutDoubleL(
    /* [in] */ ByteBuffer* bb,
    /* [in] */ Integer bi,
    /* [in] */ Double x)
{
    PutLongL(bb, bi, Math::DoubleToRawLongBits(x));
}

void Bits::PutDoubleB(
    /* [in] */ ByteBuffer* bb,
    /* [in] */ Integer bi,
    /* [in] */ Double x)
{
    PutLongB(bb, bi, Math::DoubleToRawLongBits(x));
}

void Bits::PutDouble(
    /* [in] */ ByteBuffer* bb,
    /* [in] */ Integer bi,
    /* [in] */ Double x,
    /* [in] */ Boolean bigEndian)
{
    if (bigEndian) {
        PutDoubleB(bb, bi, x);
    }
    else {
        PutDoubleL(bb, bi, x);
    }
}

AutoPtr<IByteOrder> Bits::GetByteOrder()
{
    static const AutoPtr<IByteOrder> sByteOrder = ByteOrder::GetLITTLE_ENDIAN();
    return sByteOrder;
}

AutoPtr<IByteOrder> Bits::ByteOrder()
{
    return GetByteOrder();
}

Integer Bits::PageSize()
{
    if (sPageSize == -1) {
        sPageSize = sysconf(_SC_PAGESIZE);
    }
    return sPageSize;
}

}
}
