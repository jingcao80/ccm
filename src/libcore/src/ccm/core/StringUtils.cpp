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
#include "ccm/core/CStringBuilder.h"
#include "ccm/core/Math.h"
#include "ccm/core/RealToString.h"
#include "ccm/core/StringToReal.h"
#include "ccm/core/StringUtils.h"
#include "ccm/misc/DoubleConsts.h"
#include "ccm/util/CFormatter.h"
#include "ccm/util/regex/Pattern.h"
#include "ccm.core.IByte.h"
#include "ccm.core.IInteger.h"
#include "ccm.core.ILong.h"
#include "ccm.core.IShort.h"
#include "ccm.util.regex.IMatcher.h"
#include "ccm.util.regex.IPattern.h"
#include <ccmlogger.h>

using ccm::misc::DoubleConsts;
using ccm::util::CFormatter;
using ccm::util::IFormatter;
using ccm::util::IID_IComparator;
using ccm::util::IID_IFormatter;
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
    FAIL_RETURN(ParseInteger(s, radix, &i));
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
    FAIL_RETURN(ParseInteger(s, radix, &i));
    if (i < IShort::MIN_VALUE || i > IShort::MAX_VALUE) {
        Logger::E("StringUtils", "Value out of range. Value:\"%s\" Radix:%d",
                s.string(), radix);
        return E_NUMBER_FORMAT_EXCEPTION;
    }
    *value = (Short)i;
    return NOERROR;
}

ECode StringUtils::ParseInteger(
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
        if (firstChar < U'0') { // Possible leading "+" or "-"
            if (firstChar == U'-') {
                negative = true;
                limit = IInteger::MIN_VALUE;
            }
            else if (firstChar != U'+') {
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
        if (firstChar < U'0') { // Possible leading "+" or "-"
            if (firstChar == U'-') {
                negative = true;
                limit = ILong::MIN_VALUE;
            }
            else if (firstChar != U'+') {
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
    U'0' , U'1' , U'2' , U'3' , U'4' , U'5' ,
    U'6' , U'7' , U'8' , U'9' , U'a' , U'b' ,
    U'c' , U'd' , U'e' , U'f' , U'g' , U'h' ,
    U'i' , U'j' , U'k' , U'l' , U'm' , U'n' ,
    U'o' , U'p' , U'q' , U'r' , U's' , U't' ,
    U'u' , U'v' , U'w' , U'x' , U'y' , U'z'
};

static Char upperCaseDigits[] = {
    U'0' , U'1' , U'2' , U'3' , U'4' , U'5' ,
    U'6' , U'7' , U'8' , U'9' , U'A' , U'B' ,
    U'C' , U'D' , U'E' , U'F' , U'G' , U'H' ,
    U'I' , U'J' , U'K' , U'L' , U'M' , U'N' ,
    U'O' , U'P' , U'Q' , U'R' , U'S' , U'T' ,
    U'U' , U'V' , U'W' , U'X' , U'Y' , U'Z'
};

static Char digitTens[] = {
    U'0', U'0', U'0', U'0', U'0', U'0', U'0', U'0', U'0', U'0',
    U'1', U'1', U'1', U'1', U'1', U'1', U'1', U'1', U'1', U'1',
    U'2', U'2', U'2', U'2', U'2', U'2', U'2', U'2', U'2', U'2',
    U'3', U'3', U'3', U'3', U'3', U'3', U'3', U'3', U'3', U'3',
    U'4', U'4', U'4', U'4', U'4', U'4', U'4', U'4', U'4', U'4',
    U'5', U'5', U'5', U'5', U'5', U'5', U'5', U'5', U'5', U'5',
    U'6', U'6', U'6', U'6', U'6', U'6', U'6', U'6', U'6', U'6',
    U'7', U'7', U'7', U'7', U'7', U'7', U'7', U'7', U'7', U'7',
    U'8', U'8', U'8', U'8', U'8', U'8', U'8', U'8', U'8', U'8',
    U'9', U'9', U'9', U'9', U'9', U'9', U'9', U'9', U'9', U'9',
};

static Char digitOnes[] = {
    U'0', U'1', U'2', U'3', U'4', U'5', U'6', U'7', U'8', U'9',
    U'0', U'1', U'2', U'3', U'4', U'5', U'6', U'7', U'8', U'9',
    U'0', U'1', U'2', U'3', U'4', U'5', U'6', U'7', U'8', U'9',
    U'0', U'1', U'2', U'3', U'4', U'5', U'6', U'7', U'8', U'9',
    U'0', U'1', U'2', U'3', U'4', U'5', U'6', U'7', U'8', U'9',
    U'0', U'1', U'2', U'3', U'4', U'5', U'6', U'7', U'8', U'9',
    U'0', U'1', U'2', U'3', U'4', U'5', U'6', U'7', U'8', U'9',
    U'0', U'1', U'2', U'3', U'4', U'5', U'6', U'7', U'8', U'9',
    U'0', U'1', U'2', U'3', U'4', U'5', U'6', U'7', U'8', U'9',
    U'0', U'1', U'2', U'3', U'4', U'5', U'6', U'7', U'8', U'9',
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
        sign = U'-';
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
        buf[--charPos] = U'-';
    }

    return String(buf, charPos, (33 - charPos));
}

static void GetChars(
    /* [in] */ Long i,
    /* [in] */ Integer index,
    /* [out] */ Array<Char>& buf)
{
    Long q;
    Integer r;
    Integer charPos = index;
    Char sign = 0;

    if (i < 0) {
        sign = U'-';
        i = -i;
    }

    // Get 2 digits/iteration using longs until quotient fits into an int
    while (i > IInteger::MAX_VALUE) {
        q = i / 100;
        // really: r = i - (q * 100);
        r = (Integer)(i - ((q << 6) + (q << 5) + (q << 2)));
        i = q;
        buf[--charPos] = digitOnes[r];
        buf[--charPos] = digitTens[r];
    }

    // Get 2 digits/iteration using ints
    Integer q2;
    Integer i2 = (Integer)i;
    while (i2 >= 65536) {
        q2 = i2 / 100;
        // really: r = i2 - (q * 100);
        r = i2 - ((q2 << 6) + (q2 << 5) + (q2 << 2));
        i2 = q2;
        buf[--charPos] = digitOnes[r];
        buf[--charPos] = digitTens[r];
    }

    // Fall thru to fast mode for smaller numbers
    // assert(i2 <= 65536, i2);
    for (;;) {
        q2 = ((unsigned Integer)(i2 * 52429)) >> (16+3);
        r = i2 - ((q2 << 3) + (q2 << 1));  // r = i2-(q2*10) ...
        buf[--charPos] = digits[r];
        i2 = q2;
        if (i2 == 0) break;
    }
    if (sign != 0) {
        buf[--charPos] = sign;
    }
}

static Integer StringSize(
    /* [in] */ Long x)
{
    Long p = 10;
    for (Integer i = 1; i < 19; i++) {
        if (x < p) {
            return i;
        }
        p = 10 * p;
    }
    return 19;
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
        buf[--charPos] = U'-';
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

String StringUtils::ToHexString(
    /* [in] */ Double d)
{
    if (!Math::IsFinite(d)) {
        // For infinity and NaN, use the decimal output.
        return ToString(d);
    }
    else {
        // Initialized to maximum size of output.
        AutoPtr<IStringBuilder> answer;
        CStringBuilder::New(24, IID_IStringBuilder, (IInterface**)&answer);

        if (Math::CopySign(1.0, d) == -1.0){
            answer->Append(U'-');
        }

        answer->Append(String("0x"));

        d = Math::Abs(d);

        if (d == 0.0) {
            answer->Append(String("0.0p0"));
        }
        else {
            Boolean subnormal = (d < DoubleConsts::MIN_NORMAL);

            // Isolate significand bits and OR in a high-order bit
            // so that the string representation has a known
            // length.
            Long signifBits = (Math::DoubleToLongBits(d) &
                    DoubleConsts::SIGNIF_BIT_MASK) |
                    0x1000000000000000LL;

            // Subnormal values have a 0 implicit bit; normal
            // values have a 1 implicit bit.
            answer->Append(subnormal ? String("0.") : String("1."));

            // Isolate the low-order 13 digits of the hex
            // representation.  If all the digits are zero,
            // replace with a single 0; otherwise, remove all
            // trailing zeros.
            String signif = StringUtils::ToHexString(signifBits).Substring(3, 16);
            if (signif.Equals("0000000000000")) { // 13 zeros
                answer->Append(U'0');
            }
            else {
                String replacedStr;
                StringUtils::ReplaceFirst(signif, String("0{1,12}$"), String(""), &replacedStr);
                answer->Append(replacedStr);
            }

            answer->Append(U'p');
            // If the value is subnormal, use the E_min exponent
            // value for double; otherwise, extract and report d's
            // exponent (the representation of a subnormal uses
            // E_min -1).
            answer->Append(subnormal ? DoubleConsts::MIN_EXPONENT :
                    Math::GetExponent(d));
        }
        String answerStr;
        answer->ToString(&answerStr);
        return answerStr;
    }
}

ECode StringUtils::ToString(
    /* [in] */ const String& str,
    /* [in] */ const String& charsetName,
    /* [out] */ String* decodedStr)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
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

String StringUtils::Format(
    /* [in] */ const String& format,
    /* [in] */ const Array<IInterface*>* args)
{
    AutoPtr<IFormatter> formatter;
    CFormatter::New(IID_IFormatter, (IInterface**)&formatter);
    formatter->Format(format, args);
    return Object::ToString(formatter);
}

String StringUtils::Format(
    /* [in] */ ILocale* l,
    /* [in] */ const String& format,
    /* [in] */ const Array<IInterface*>* args)
{
    AutoPtr<IFormatter> formatter;
    CFormatter::New(l, IID_IFormatter, (IInterface**)&formatter);
    formatter->Format(format, args);
    return Object::ToString(formatter);
}

class CaseInsensitiveComparator
    : public Object
    , public IComparator
{
public:
    CCM_INTERFACE_DECL();

    ECode Compare(
        /* [in] */ IInterface* c1,
        /* [in] */ IInterface* c2,
        /* [out] */ Integer* cmp) override;

    ECode Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* isEqual) override;
};

CCM_INTERFACE_IMPL_1(CaseInsensitiveComparator, Object, IComparator);

ECode CaseInsensitiveComparator::Compare(
    /* [in] */ IInterface* c1,
    /* [in] */ IInterface* c2,
    /* [out] */ Integer* cmp)
{
    VALIDATE_NOT_NULL(cmp);

    String s1, s2;
    ICharSequence::Probe(c1)->ToString(&s1);
    ICharSequence::Probe(c2)->ToString(&s2);

    Integer n1 = s1.GetLength();
    Integer n2 = s2.GetLength();
    Integer min = Math::Min(n1, n2);
    for (Integer i = 0; i < min; i++) {
        Char c1 = s1.GetChar(i);
        Char c2 = s2.GetChar(i);
        if (c1 != c2) {
            c1 = Character::ToUpperCase(c1);
            c2 = Character::ToUpperCase(c2);
            if (c1 != c2) {
                c1 = Character::ToLowerCase(c1);
                c2 = Character::ToLowerCase(c2);
                if (c1 != c2) {
                    *cmp = c1 - c2;
                    return NOERROR;
                }
            }
        }
    }
    *cmp = n1 - n2;
    return NOERROR;
}

ECode CaseInsensitiveComparator::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* isEqual)
{
    VALIDATE_NOT_NULL(isEqual);

    *isEqual = IComparator::Probe(obj) == (IComparator*)this;
    return NOERROR;
}

AutoPtr<IComparator> StringUtils::GetStringCASE_INSENSITIVE_ORDER()
{
    static AutoPtr<IComparator> CASE_INSENSITIVE_ORDER = new CaseInsensitiveComparator();
    return CASE_INSENSITIVE_ORDER;
}

}
}
