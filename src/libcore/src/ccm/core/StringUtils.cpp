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

#include "ccm/core/Character.h"
#include "ccm/core/CoreUtils.h"
#include "ccm/core/RealToString.h"
#include "ccm/core/StringToReal.h"
#include "ccm/core/StringUtils.h"
#include "ccm/util/regex/Pattern.h"
#include "ccm.core.IByte.h"
#include "ccm.core.IInteger.h"
#include "ccm.core.ILong.h"
#include "ccm.core.IShort.h"
#include "ccm.util.regex.IMatcher.h"
#include "ccm.util.regex.IPattern.h"
#include <ccmlogger.h>

using ccm::util::regex::IMatcher;
using ccm::util::regex::IPattern;
using ccm::util::regex::Pattern;

namespace ccm {
namespace core {

ECode StringUtils::ParseByte(
    /* [in] */ const String& s,
    /* [in] */ Integer radix,
    /* [out] */ Byte* value)
{
    VALIDATE_NOT_NULL(value);

    Integer i;
    FAIL_RETURN(ParseInt(s, radix, &i));
    if (i < IByte::MIN_VALUE || i > IByte::MAX_VALUE) {
        Logger::E("StringUtils", "Value out of range. Value:\"%s\" Radix:%d",
                s.string(), radix);
        return E_NUMBER_FORMAT_EXCEPTION;
    }
    *value = (Byte)i;
    return NOERROR;
}

ECode StringUtils::ParseShort(
    /* [in] */ const String& s,
    /* [in] */ Integer radix,
    /* [out] */ Short* value)
{
    VALIDATE_NOT_NULL(value);

    Integer i;
    FAIL_RETURN(ParseInt(s, radix, &i));
    if (i < IShort::MIN_VALUE || i > IShort::MAX_VALUE) {
        Logger::E("StringUtils", "Value out of range. Value:\"%s\" Radix:%d",
                s.string(), radix);
        return E_NUMBER_FORMAT_EXCEPTION;
    }
    *value = (Short)i;
    return NOERROR;
}

ECode StringUtils::ParseInt(
    /* [in] */ const String& s,
    /* [in] */ Integer radix,
    /* [out] */ Integer* value)
{
    VALIDATE_NOT_NULL(value);

    if (s.IsNull()) {
        Logger::E("StringUtils", "s == null");
        return E_NUMBER_FORMAT_EXCEPTION;
    }

    if (radix < Character::MIN_RADIX) {
        Logger::E("StringUtils", "radix %d less than Character::MIN_RADIX", radix);
        return E_NUMBER_FORMAT_EXCEPTION;
    }

    if (radix > Character::MAX_RADIX) {
        Logger::E("StringUtils", "radix %d greater than Character::MAX_RADIX", radix);
        return E_NUMBER_FORMAT_EXCEPTION;
    }

    Integer result = 0;
    Boolean negative = false;
    Integer i = 0, len = s.GetLength();
    Integer limit = -IInteger::MAX_VALUE;
    Integer multmin;
    Integer digit;

    if (len > 0) {
        Char firstChar = s.GetChar(0);
        if (firstChar < '0') { // Possible leading "+" or "-"
            if (firstChar == '-') {
                negative = true;
                limit = IInteger::MIN_VALUE;
            }
            else if (firstChar != '+') {
                Logger::E("StringUtils", "s == %s", s.string());
                return E_NUMBER_FORMAT_EXCEPTION;
            }

            if (len == 1) { // Cannot have lone "+" or "-"
                Logger::E("StringUtils", "s == %s", s.string());
                return E_NUMBER_FORMAT_EXCEPTION;
            }
            i++;
        }
        multmin = limit / radix;
        while (i < len) {
            // Accumulating negatively avoids surprises near MAX_VALUE
            digit = Character::Digit(s.GetChar(i++), radix);
            if (digit < 0) {
                Logger::E("StringUtils", "s == %s", s.string());
                return E_NUMBER_FORMAT_EXCEPTION;
            }
            if (result < multmin) {
                Logger::E("StringUtils", "s == %s", s.string());
                return E_NUMBER_FORMAT_EXCEPTION;
            }
            result *= radix;
            if (result < limit + digit) {
                Logger::E("StringUtils", "s == %s", s.string());
                return E_NUMBER_FORMAT_EXCEPTION;
            }
            result -= digit;
        }
    }
    else {
        Logger::E("StringUtils", "s == %s", s.string());
        return E_NUMBER_FORMAT_EXCEPTION;
    }
    *value = negative ? result : -result;
    return NOERROR;
}

ECode StringUtils::ParseLong(
    /* [in] */ const String& s,
    /* [in] */ Integer radix,
    /* [out] */ Long* value)
{
    VALIDATE_NOT_NULL(value);

    if (s.IsNull()) {
        Logger::E("StringUtils", "s == null");
        return E_NUMBER_FORMAT_EXCEPTION;
    }

    if (radix < Character::MIN_RADIX) {
        Logger::E("StringUtils", "radix %d less than Character::MIN_RADIX", radix);
        return E_NUMBER_FORMAT_EXCEPTION;
    }

    if (radix > Character::MAX_RADIX) {
        Logger::E("StringUtils", "radix %d greater than Character::MAX_RADIX", radix);
        return E_NUMBER_FORMAT_EXCEPTION;
    }

    Long result = 0;
    Boolean negative = false;
    Integer i = 0, len = s.GetLength();
    Long limit = -ILong::MAX_VALUE;
    Long multmin;
    Integer digit;

    if (len > 0) {
        Char firstChar = s.GetChar(0);
        if (firstChar < '0') { // Possible leading "+" or "-"
            if (firstChar == '-') {
                negative = true;
                limit = ILong::MIN_VALUE;
            }
            else if (firstChar != '+') {
                Logger::E("StringUtils", "s == %s", s.string());
                return E_NUMBER_FORMAT_EXCEPTION;
            }

            if (len == 1) { // Cannot have lone "+" or "-"
                Logger::E("StringUtils", "s == %s", s.string());
                return E_NUMBER_FORMAT_EXCEPTION;
            }
            i++;
        }
        multmin = limit / radix;
        while (i < len) {
            // Accumulating negatively avoids surprises near MAX_VALUE
            digit = Character::Digit(s.GetChar(i++), radix);
            if (digit < 0) {
                Logger::E("StringUtils", "s == %s", s.string());
                return E_NUMBER_FORMAT_EXCEPTION;
            }
            if (result < multmin) {
                Logger::E("StringUtils", "s == %s", s.string());
                return E_NUMBER_FORMAT_EXCEPTION;
            }
            result *= radix;
            if (result < limit + digit) {
                Logger::E("StringUtils", "s == %s", s.string());
                return E_NUMBER_FORMAT_EXCEPTION;
            }
            result -= digit;
        }
    }
    else {
        Logger::E("StringUtils", "s == %s", s.string());
        return E_NUMBER_FORMAT_EXCEPTION;
    }
    *value = negative ? result : -result;
    return NOERROR;
}

ECode StringUtils::ParseFloat(
    /* [in] */ const String& s,
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);

    return StringToReal::ParseFloat(s, value);
}

ECode StringUtils::ParseDouble(
    /* [in] */ const String& s,
    /* [out] */ Double* value)
{
    VALIDATE_NOT_NULL(value);

    return StringToReal::ParseDouble(s, value);
}

static Char digits[] = {
    '0' , '1' , '2' , '3' , '4' , '5' ,
    '6' , '7' , '8' , '9' , 'a' , 'b' ,
    'c' , 'd' , 'e' , 'f' , 'g' , 'h' ,
    'i' , 'j' , 'k' , 'l' , 'm' , 'n' ,
    'o' , 'p' , 'q' , 'r' , 's' , 't' ,
    'u' , 'v' , 'w' , 'x' , 'y' , 'z'
};

static Char upperCaseDigits[] = {
    '0' , '1' , '2' , '3' , '4' , '5' ,
    '6' , '7' , '8' , '9' , 'A' , 'B' ,
    'C' , 'D' , 'E' , 'F' , 'G' , 'H' ,
    'I' , 'J' , 'K' , 'L' , 'M' , 'N' ,
    'O' , 'P' , 'Q' , 'R' , 'S' , 'T' ,
    'U' , 'V' , 'W' , 'X' , 'Y' , 'Z'
};

static Char digitTens[] = {
    '0', '0', '0', '0', '0', '0', '0', '0', '0', '0',
    '1', '1', '1', '1', '1', '1', '1', '1', '1', '1',
    '2', '2', '2', '2', '2', '2', '2', '2', '2', '2',
    '3', '3', '3', '3', '3', '3', '3', '3', '3', '3',
    '4', '4', '4', '4', '4', '4', '4', '4', '4', '4',
    '5', '5', '5', '5', '5', '5', '5', '5', '5', '5',
    '6', '6', '6', '6', '6', '6', '6', '6', '6', '6',
    '7', '7', '7', '7', '7', '7', '7', '7', '7', '7',
    '8', '8', '8', '8', '8', '8', '8', '8', '8', '8',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
};

static Char digitOnes[] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
};

static void GetChars(
    /* [in] */ Integer i,
    /* [in] */ Integer index,
    /* [out] */ Array<Char>& buf)
{
    Integer q, r;
    Integer charPos = index;
    Char sign = 0;

    if (i < 0) {
        sign = '-';
        i = -i;
    }

    // Generate two digits per iteration
    while (i >= 65536) {
        q = i / 100;
        // really: r = i - (q * 100);
        r = i - ((q << 6) + (q << 5) + (q << 2));
        i = q;
        buf[--charPos] = digitOnes[r];
        buf[--charPos] = digitTens[r];
    }

    // Fall thru to fast mode for smaller numbers
    // assert(i <= 65536, i);
    for (;;) {
        q = (unsigned int)(i * 52429) >> (16 + 3);
        r = i - ((q << 3) + (q << 1));  // r = i-(q*10) ...
        buf [--charPos] = digits[r];
        i = q;
        if (i == 0) break;
    }
    if (sign != 0) {
        buf[--charPos] = sign;
    }
}

static Integer sizeTable[] = {
    9, 99, 999, 9999, 99999, 999999, 9999999,
    99999999, 999999999, IInteger::MAX_VALUE };

static Integer StringSize(
    /* [in] */ Integer x)
{
    for (Integer i = 0; ; i++) {
        if (x <= sizeTable[i]) {
            return i + 1;
        }
    }
}

String StringUtils::ToString(
    /* [in] */ Integer i)
{
    if (i == IInteger::MIN_VALUE) {
        return String("-2147483648");
    }

    Boolean negative = i < 0;
    Integer size = negative ? StringSize(-i) + 1 : StringSize(i);
    Array<Char> buf(size);
    GetChars(i, size, buf);
    return String(buf);
}

String StringUtils::ToString(
    /* [in] */ Integer i,
    /* [in] */ Integer radix)
{
    if (radix < Character::MIN_RADIX || radix > Character::MAX_RADIX) {
        radix = 10;
    }

    if (radix == 10) {
        return ToString(i);
    }

    Array<Char> buf(33);
    Boolean negative = (i < 0);
    Integer charPos = 32;

    if (!negative) {
        i = -i;
    }

    while (i <= -radix) {
        buf[charPos--] = digits[-(i % radix)];
        i = i / radix;
    }
    buf[charPos] = digits[-i];

    if (negative) {
        buf[--charPos] = '-';
    }

    return String(buf, charPos, (33 - charPos));
}

String StringUtils::ToString(
    /* [in] */ Long i)
{
    if (i == ILong::MIN_VALUE) {
        return String("-9223372036854775808");
    }
    Integer size = (i < 0) ? StringSize(-i) + 1 : StringSize(i);
    Array<Char> buf(size);
    GetChars(i, size, buf);
    return String(buf);
}

String StringUtils::ToString(
    /* [in] */ Long i,
    /* [in] */ Integer radix)
{
    if (radix < Character::MIN_RADIX || radix > Character::MAX_RADIX) {
        radix = 10;
    }
    if (radix == 10) {
        return ToString(i);
    }
    Array<Char> buf(65);
    Integer charPos = 64;
    Boolean negative = (i < 0);

    if (!negative) {
        i = -i;
    }

    while (i <= -radix) {
        buf[charPos--] = digits[-(i % radix)];
        i = i / radix;
    }
    buf[charPos] = digits[-i];

    if (negative) {
        buf[--charPos] = '-';
    }

    return String(buf, charPos, (65 - charPos));
}

String StringUtils::ToString(
    /* [in] */ Float f)
{
    return RealToString::FloatToString(f);
}

String StringUtils::ToString(
    /* [in] */ Double d)
{
    return RealToString::DoubleToString(d);
}

String StringUtils::ToOctalString(
    /* [in] */ Integer i)
{
    Integer bufLen = 11;  // Max number of octal digits in an Int32
    char buf[11];
    Integer cursor = bufLen;

    do {
        buf[--cursor] = digits[i & 7];
    } while ((i = (((unsigned Integer)i) >> 3) ) != 0);

    return String(buf + cursor, bufLen - cursor);
}

String StringUtils::ToOctalString(
    /* [in] */ Long i)
{
    Integer v = (Integer)i;
    if (i >= 0 && v == i) {
        return ToOctalString(v);
    }

    Integer bufLen = 22;  // Max number of octal digits in a long
    char buf[22];
    Integer cursor = bufLen;

    do {
        buf[--cursor] = digits[((Integer)i) & 7];
    } while ((i = (((unsigned Long)i) >> 3)) != 0);

    return String(buf + cursor, bufLen - cursor);
}

String StringUtils::ToHexString(
    /* [in] */ Integer i,
    /* [in] */ Boolean upperCase,
    /* [in] */ Integer minWidth)
{
    Integer bufLen = 8;  // Max number of hex digits in an int
    char buf[8];
    Integer cursor = bufLen;

    const Char* DIGITS = upperCase ? upperCaseDigits : digits;
    do {
        buf[--cursor] = DIGITS[i & 0xf];
    } while ((i = (((unsigned Integer)i) >> 4)) != 0 || (bufLen - cursor < minWidth));

    return String(buf + cursor, bufLen - cursor);
}

String StringUtils::ToHexString(
    /* [in] */ Long i,
    /* [in] */ Boolean upperCase)
{
    Integer v = (Integer)i;
    if (i >= 0 && v == i) {
        return ToHexString(v, upperCase, 0);
    }

    Integer bufLen = 16;  // Max number of hex digits in a long
    char buf[16];
    Integer cursor = bufLen;

    const Char* DIGITS = upperCase ? upperCaseDigits : digits;
    do {
        buf[--cursor] = DIGITS[((Integer)i) & 0xF];
    } while ((i = (((unsigned Long)i) >> 4)) != 0);

    return String(buf + cursor, bufLen - cursor);
}

ECode StringUtils::ReplaceFirst(
    /* [in] */ const String& input,
    /* [in] */ const String& regex,
    /* [in] */ const String& replacement,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IPattern> p;
    Pattern::Compile(regex, &p);
    AutoPtr<IMatcher> m;
    p->Matcher(CoreUtils::Box(input), &m);
    return m->ReplaceFirst(replacement, result);
}

ECode StringUtils::ReplaceAll(
    /* [in] */ const String& input,
    /* [in] */ const String& regex,
    /* [in] */ const String& replacement,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IPattern> p;
    Pattern::Compile(regex, &p);
    AutoPtr<IMatcher> m;
    p->Matcher(CoreUtils::Box(input), &m);
    return m->ReplaceAll(replacement, result);
}

Array<String> StringUtils::Split(
    /* [in] */ const String& input,
    /* [in] */ const String& regex,
    /* [in] */ Integer limit)
{
    // Try fast splitting without allocating Pattern object
    Array<String> strArr;
    Pattern::FastSplit(regex, input, limit, &strArr);
    if (!strArr.IsNull()) {
        return strArr;
    }

    AutoPtr<IPattern> p;
    Pattern::Compile(regex, &p);
    p->Split(CoreUtils::Box(input), limit, &strArr);
    return strArr;
}

}
}
