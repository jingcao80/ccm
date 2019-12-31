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

#ifndef __CCM_IO_BITS_H__
#define __CCM_IO_BITS_H__

#include "ccm/io/ByteBuffer.h"
#include "ccm.io.IByteOrder.h"
#include <ccmautoptr.h>

namespace ccm {
namespace io {

class Bits
{
public:
    static Char GetCharL(
        /* [in] */ ByteBuffer* bb,
        /* [in] */ Integer bi);

    static Char GetCharB(
        /* [in] */ ByteBuffer* bb,
        /* [in] */ Integer bi);

    static Char GetChar(
        /* [in] */ ByteBuffer* bb,
        /* [in] */ Integer bi,
        /* [in] */ Boolean bigEndian);

    static void PutCharL(
        /* [in] */ ByteBuffer* bb,
        /* [in] */ Integer bi,
        /* [in] */ Char x);

    static void PutCharB(
        /* [in] */ ByteBuffer* bb,
        /* [in] */ Integer bi,
        /* [in] */ Char x);

    static void PutChar(
        /* [in] */ ByteBuffer* bb,
        /* [in] */ Integer bi,
        /* [in] */ Char x,
        /* [in] */ Boolean bigEndian);

    static Short GetShortL(
        /* [in] */ ByteBuffer* bb,
        /* [in] */ Integer bi);

    static Short GetShortB(
        /* [in] */ ByteBuffer* bb,
        /* [in] */ Integer bi);

    static Short GetShort(
        /* [in] */ ByteBuffer* bb,
        /* [in] */ Integer bi,
        /* [in] */ Boolean bigEndian);

    static void PutShortL(
        /* [in] */ ByteBuffer* bb,
        /* [in] */ Integer bi,
        /* [in] */ Short x);

    static void PutShortB(
        /* [in] */ ByteBuffer* bb,
        /* [in] */ Integer bi,
        /* [in] */ Short x);

    static void PutShort(
        /* [in] */ ByteBuffer* bb,
        /* [in] */ Integer bi,
        /* [in] */ Short x,
        /* [in] */ Boolean bigEndian);

    static Integer GetIntegerL(
        /* [in] */ ByteBuffer* bb,
        /* [in] */ Integer bi);

    static Integer GetIntegerB(
        /* [in] */ ByteBuffer* bb,
        /* [in] */ Integer bi);

    static Integer GetInteger(
        /* [in] */ ByteBuffer* bb,
        /* [in] */ Integer bi,
        /* [in] */ Boolean bigEndian);

    static void PutIntegerL(
        /* [in] */ ByteBuffer* bb,
        /* [in] */ Integer bi,
        /* [in] */ Integer x);

    static void PutIntegerB(
        /* [in] */ ByteBuffer* bb,
        /* [in] */ Integer bi,
        /* [in] */ Integer x);

    static void PutInteger(
        /* [in] */ ByteBuffer* bb,
        /* [in] */ Integer bi,
        /* [in] */ Integer x,
        /* [in] */ Boolean bigEndian);

    static Long GetLongL(
        /* [in] */ ByteBuffer* bb,
        /* [in] */ Integer bi);

    static Long GetLongB(
        /* [in] */ ByteBuffer* bb,
        /* [in] */ Integer bi);

    static Long GetLong(
        /* [in] */ ByteBuffer* bb,
        /* [in] */ Integer bi,
        /* [in] */ Boolean bigEndian);

    static void PutLongL(
        /* [in] */ ByteBuffer* bb,
        /* [in] */ Integer bi,
        /* [in] */ Long x);

    static void PutLongB(
        /* [in] */ ByteBuffer* bb,
        /* [in] */ Integer bi,
        /* [in] */ Long x);

    static void PutLong(
        /* [in] */ ByteBuffer* bb,
        /* [in] */ Integer bi,
        /* [in] */ Long x,
        /* [in] */ Boolean bigEndian);

    static Float GetFloatL(
        /* [in] */ ByteBuffer* bb,
        /* [in] */ Integer bi);

    static Float GetFloatB(
        /* [in] */ ByteBuffer* bb,
        /* [in] */ Integer bi);

    static Float GetFloat(
        /* [in] */ ByteBuffer* bb,
        /* [in] */ Integer bi,
        /* [in] */ Boolean bigEndian);

    static void PutFloatL(
        /* [in] */ ByteBuffer* bb,
        /* [in] */ Integer bi,
        /* [in] */ Float x);

    static void PutFloatB(
        /* [in] */ ByteBuffer* bb,
        /* [in] */ Integer bi,
        /* [in] */ Float x);

    static void PutFloat(
        /* [in] */ ByteBuffer* bb,
        /* [in] */ Integer bi,
        /* [in] */ Float x,
        /* [in] */ Boolean bigEndian);

    static Double GetDoubleL(
        /* [in] */ ByteBuffer* bb,
        /* [in] */ Integer bi);

    static Double GetDoubleB(
        /* [in] */ ByteBuffer* bb,
        /* [in] */ Integer bi);

    static Double GetDouble(
        /* [in] */ ByteBuffer* bb,
        /* [in] */ Integer bi,
        /* [in] */ Boolean bigEndian);

    static void PutDoubleL(
        /* [in] */ ByteBuffer* bb,
        /* [in] */ Integer bi,
        /* [in] */ Double x);

    static void PutDoubleB(
        /* [in] */ ByteBuffer* bb,
        /* [in] */ Integer bi,
        /* [in] */ Double x);

    static void PutDouble(
        /* [in] */ ByteBuffer* bb,
        /* [in] */ Integer bi,
        /* [in] */ Double x,
        /* [in] */ Boolean bigEndian);

    static AutoPtr<IByteOrder> ByteOrder();

    static Integer PageSize();

    static Integer PageCount(
        /* [in] */ Long size);

private:
    Bits();

    static Char MakeChar(
        /* [in] */ Byte b3,
        /* [in] */ Byte b2,
        /* [in] */ Byte b1,
        /* [in] */ Byte b0);

    static Byte Char3(
        /* [in] */ Char x);

    static Byte Char2(
        /* [in] */ Char x);

    static Byte Char1(
        /* [in] */ Char x);

    static Byte Char0(
        /* [in] */ Char x);

    static Short MakeShort(
        /* [in] */ Byte b1,
        /* [in] */ Byte b0);

    static Byte Short1(
        /* [in] */ Short x);

    static Byte Short0(
        /* [in] */ Short x);

    static Integer MakeInteger(
        /* [in] */ Byte b3,
        /* [in] */ Byte b2,
        /* [in] */ Byte b1,
        /* [in] */ Byte b0);

    static Byte Integer3(
        /* [in] */ Integer x);

    static Byte Integer2(
        /* [in] */ Integer x);

    static Byte Integer1(
        /* [in] */ Integer x);

    static Byte Integer0(
        /* [in] */ Integer x);

    static Long MakeLong(
        /* [in] */ Byte b7,
        /* [in] */ Byte b6,
        /* [in] */ Byte b5,
        /* [in] */ Byte b4,
        /* [in] */ Byte b3,
        /* [in] */ Byte b2,
        /* [in] */ Byte b1,
        /* [in] */ Byte b0);

    static Byte Long7(
        /* [in] */ Long x);

    static Byte Long6(
        /* [in] */ Long x);

    static Byte Long5(
        /* [in] */ Long x);

    static Byte Long4(
        /* [in] */ Long x);

    static Byte Long3(
        /* [in] */ Long x);

    static Byte Long2(
        /* [in] */ Long x);

    static Byte Long1(
        /* [in] */ Long x);

    static Byte Long0(
        /* [in] */ Long x);

    static AutoPtr<IByteOrder> GetByteOrder();

private:
    static Integer sPageSize;
};

inline Char Bits::MakeChar(
    /* [in] */ Byte b3,
    /* [in] */ Byte b2,
    /* [in] */ Byte b1,
    /* [in] */ Byte b0)
{
    return (((b3) << 24) |
            ((b2 & 0xff) << 16) |
            ((b1 & 0xff) << 8) |
            ((b0 & 0xff)));
}

inline Byte Bits::Char3(
    /* [in] */ Char x)
{
    return (Byte)(x >> 24);
}

inline Byte Bits::Char2(
    /* [in] */ Char x)
{
    return (Byte)(x >> 16);
}

inline Byte Bits::Char1(
    /* [in] */ Char x)
{
    return (Byte)(x >> 8);
}

inline Byte Bits::Char0(
    /* [in] */ Char x)
{
    return (Byte)(x);
}

inline Short Bits::MakeShort(
    /* [in] */ Byte b1,
    /* [in] */ Byte b0)
{
    return (Short)((b1 << 8) | (b0 & 0xff));
}

inline Byte Bits::Short1(
    /* [in] */ Short x)
{
    return (Byte)(x >> 8);
}

inline Byte Bits::Short0(
    /* [in] */ Short x)
{
    return (Byte)(x);
}

inline Integer Bits::MakeInteger(
    /* [in] */ Byte b3,
    /* [in] */ Byte b2,
    /* [in] */ Byte b1,
    /* [in] */ Byte b0)
{
    return (((b3) << 24) |
            ((b2 & 0xff) << 16) |
            ((b1 & 0xff) << 8) |
            ((b0 & 0xff)));
}

inline Byte Bits::Integer3(
    /* [in] */ Integer x)
{
    return (Byte)(x >> 24);
}

inline Byte Bits::Integer2(
    /* [in] */ Integer x)
{
    return (Byte)(x >> 16);
}

inline Byte Bits::Integer1(
    /* [in] */ Integer x)
{
    return (Byte)(x >> 8);
}

inline Byte Bits::Integer0(
    /* [in] */ Integer x)
{
    return (Byte)(x);
}

inline Long Bits::MakeLong(
    /* [in] */ Byte b7,
    /* [in] */ Byte b6,
    /* [in] */ Byte b5,
    /* [in] */ Byte b4,
    /* [in] */ Byte b3,
    /* [in] */ Byte b2,
    /* [in] */ Byte b1,
    /* [in] */ Byte b0)
{
    return ((((Long) b7) << 56) |
            (((Long) b6 & 0xff) << 48) |
            (((Long) b5 & 0xff) << 40) |
            (((Long) b4 & 0xff) << 32) |
            (((Long) b3 & 0xff) << 24) |
            (((Long) b2 & 0xff) << 16) |
            (((Long) b1 & 0xff) << 8) |
            (((Long) b0 & 0xff)));
}

inline Byte Bits::Long7(
    /* [in] */ Long x)
{
    return (Byte)(x >> 56);
}

inline Byte Bits::Long6(
    /* [in] */ Long x)
{
    return (Byte)(x >> 48);
}

inline Byte Bits::Long5(
    /* [in] */ Long x)
{
    return (Byte)(x >> 40);
}

inline Byte Bits::Long4(
    /* [in] */ Long x)
{
    return (Byte)(x >> 32);
}

inline Byte Bits::Long3(
    /* [in] */ Long x)
{
    return (Byte)(x >> 24);
}

inline Byte Bits::Long2(
    /* [in] */ Long x)
{
    return (Byte)(x >> 16);
}

inline Byte Bits::Long1(
    /* [in] */ Long x)
{
    return (Byte)(x >> 8);
}

inline Byte Bits::Long0(
    /* [in] */ Long x)
{
    return (Byte)(x);
}

inline Integer Bits::PageCount(
    /* [in] */ Long size)
{
    return (Integer)(size + (Long)PageSize() - 1LL) / PageSize();
}

}
}

#endif // __CCM_IO_BITS_H__
