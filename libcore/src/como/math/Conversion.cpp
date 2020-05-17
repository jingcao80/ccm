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

#include "innerdef.h"
#include "como/core/Character.h"
#include "como/core/CStringBuilder.h"
#include "como/core/Math.h"
#include "como/core/StringUtils.h"
#include "como/math/BitLevel.h"
#include "como/math/Conversion.h"
#include "como/math/Division.h"
#include "como.core.IDouble.h"
#include "como.core.IInteger.h"
#include "como.core.INumber.h"

using como::core::Character;
using como::core::CStringBuilder;
using como::core::IDouble;
using como::core::IID_IStringBuilder;
using como::core::IInteger;
using como::core::INumber;
using como::core::IStringBuilder;
using como::core::Math;
using como::core::StringUtils;

namespace como {
namespace math {

constexpr Integer Conversion::sDigitFitInInt[];
constexpr Integer Conversion::sBigRadices[];

String Conversion::BigInteger2String(
    /* [in] */ BigInteger* value,
    /* [in] */ Integer radix)
{
    value->PrepareRepresentation();
    Integer sign = value->mSign;
    Integer numberLength = value->mNumberLength;
    Array<Integer> digits = value->mDigits;

    if (sign == 0) {
        return String("0");
    }
    if (numberLength == 1) {
        Integer highDigit = digits[numberLength - 1];
        Long v = highDigit & 0xFFFFFFFFLL;
        if (sign < 0) {
            v = -v;
        }
        return StringUtils::ToString(v, radix);
    }
    if ((radix == 10) || (radix < Character::MIN_RADIX) ||
            (radix > Character::MAX_RADIX)) {
        String str;
        value->ToString(str);
        return str;
    }
    Double bitsForRadixDigit = Math::Log(radix) / Math::Log(2);
    AutoPtr<IBigInteger> absBI;
    value->Abs(absBI);
    Integer lenth;
    absBI->BitLength(lenth);
    Integer resLengthInChars = (Integer)(lenth / bitsForRadixDigit + ((sign < 0) ? 1 : 0)) + 1;

    Array<Char> result(resLengthInChars);
    Integer currentChar = resLengthInChars;
    Integer resDigit;
    if (radix != 16) {
        Array<Integer> temp(numberLength);
        temp.Copy(digits, 0, numberLength);
        Integer tempLen = numberLength;
        Integer charsPerInt = sDigitFitInInt[radix];
        Integer i;
        // get the maximal power of radix that fits in int
        Integer bigRadix = sBigRadices[radix - 2];
        while (true) {
            // divide the array of digits by bigRadix and convert remainders
            // to characters collecting them in the char array
            resDigit = Division::DivideArrayByInteger(temp, temp, tempLen,
                    bigRadix);
            Integer previous = currentChar;
            do {
                result[--currentChar] = Character::ForDigit(
                        resDigit % radix, radix);
            } while (((resDigit /= radix) != 0) && (currentChar != 0));
            Integer delta = charsPerInt - previous + currentChar;
            for (i = 0; i < delta && currentChar > 0; i++) {
                result[--currentChar] = U'0';
            }
            for (i = tempLen - 1; (i > 0) && (temp[i] == 0); i--) {
                ;
            }
            tempLen = i + 1;
            if ((tempLen == 1) && (temp[0] == 0)) { // the quotient is 0
                break;
            }
        }
    }
    else {
        // radix == 16
        for (Integer i = 0; i < numberLength; i++) {
            for (Integer j = 0; (j < 8) && (currentChar > 0); j++) {
                resDigit = digits[i] >> (j << 2) & 0xf;
                result[--currentChar] = Character::ForDigit(resDigit, 16);
            }
        }
    }
    while (result[currentChar] == U'0') {
        currentChar++;
    }
    if (sign == -1) {
        result[--currentChar] = U'-';
    }
    return String(result, currentChar, resLengthInChars - currentChar);
}

String Conversion::ToDecimalScaledString(
    /* [in] */ BigInteger* value,
    /* [in] */ Integer scale)
{
    value->PrepareRepresentation();
    Integer sign = value->mSign;
    Integer numberLength = value->mNumberLength;
    Array<Integer> digits = value->mDigits;
    Integer resLengthInChars;
    Integer currentChar;
    Array<Char> result;

    if (sign == 0) {
        switch (scale) {
            case 0:
                return String("0");
            case 1:
                return String("0.0");
            case 2:
                return String("0.00");
            case 3:
                return String("0.000");
            case 4:
                return String("0.0000");
            case 5:
                return String("0.00000");
            case 6:
                return String("0.000000");
            default: {
                AutoPtr<IStringBuilder> result1;
                CStringBuilder::New(IID_IStringBuilder, (IInterface**)&result1);
                if (scale < 0) {
                    result1->Append(String("0E+"));
                }
                else {
                    result1->Append(String("0E"));
                }
                result1->Append(-scale);
                String str;
                result1->ToString(str);
                return str;
            }
        }
    }
    // one 32-bit unsigned value may contains 10 decimal digits
    resLengthInChars = numberLength * 10 + 1 + 7;
    // Explanation why +1+7:
    // +1 - one char for sign if needed.
    // +7 - For "special case 2" (see below) we have 7 free chars for
    // inserting necessary scaled digits.
    result = Array<Char>(resLengthInChars + 1);
    // allocated [resLengthInChars+1] characters.
    // a free latest character may be used for "special case 1" (see
    // below)
    currentChar = resLengthInChars;
    if (numberLength == 1) {
        Integer highDigit = digits[0];
        if (highDigit < 0) {
            Long v = highDigit & 0xFFFFFFFFLL;
            do {
                Long prev = v;
                v /= 10;
                result[--currentChar] = (Char)(0x0030 + ((Integer)(prev - v * 10)));
            } while (v != 0);
        }
        else {
            Integer v = highDigit;
            do {
                Integer prev = v;
                v /= 10;
                result[--currentChar] = (Char)(0x0030 + (prev - v * 10));
            } while (v != 0);
        }
    }
    else {
        Array<Integer> temp(numberLength);
        Integer tempLen = numberLength;
        temp.Copy(digits, 0, tempLen);
        while (true) {
            // divide the array of digits by bigRadix and convert
            // remainders
            // to characters collecting them in the char array
            Long result11 = 0;
            for (Integer i1 = tempLen - 1; i1 >= 0; i1--) {
                Long temp1 = (result11 << 32) +
                        (temp[i1] & 0xFFFFFFFFLL);
                Long res = DivideLongByBillion(temp1);
                temp[i1] = (Integer)res;
                result11 = (Integer)(res >> 32);
            }
            Integer resDigit = (Integer)result11;
            Integer previous = currentChar;
            do {
                result[--currentChar] = (Char)(0x0030 + (resDigit % 10));
            } while (((resDigit /= 10) != 0) && (currentChar != 0));
            Integer delta = 9 - previous + currentChar;
            for (Integer i = 0; (i < delta) && (currentChar > 0); i++) {
                result[--currentChar] = U'0';
            }
            Integer j = tempLen - 1;
            for (; temp[j] == 0; j--) {
                if (j == 0) {
                    // means temp[0] == 0
                    goto BIG_LOOP;
                }
            }
            tempLen = j + 1;
        }
BIG_LOOP:
        while (result[currentChar] == U'0') {
            currentChar++;
        }
    }
    Boolean negNumber = (sign < 0);
    Integer exponent = resLengthInChars - currentChar - scale - 1;
    if (scale == 0) {
        if (negNumber) {
            result[--currentChar] = U'-';
        }
        return String(result, currentChar, resLengthInChars - currentChar);
    }
    if ((scale > 0) && (exponent >= -6)) {
        if (exponent >= 0) {
            // special case 1
            Integer insertPoint = currentChar + exponent;
            for (Integer j = resLengthInChars - 1; j >= insertPoint; j--) {
                result[j + 1] = result[j];
            }
            result[++insertPoint] = U'.';
            if (negNumber) {
                result[--currentChar] = U'-';
            }
            return String(result, currentChar, resLengthInChars - currentChar + 1);
        }
        // special case 2
        for (Integer j = 2; j < -exponent + 1; j++) {
            result[--currentChar] = U'0';
        }
        result[--currentChar] = U'.';
        result[--currentChar] = U'0';
        if (negNumber) {
            result[--currentChar] = U'-';
        }
        return String(result, currentChar, resLengthInChars - currentChar);
    }
    Integer startPoint = currentChar + 1;
    Integer endPoint = resLengthInChars;
    AutoPtr<IStringBuilder> result1;
    CStringBuilder::New(16 + endPoint - startPoint, IID_IStringBuilder, (IInterface**)&result1);
    if (negNumber) {
        result1->Append(U'-');
    }
    if (endPoint - startPoint >= 1) {
        result1->Append(result[currentChar]);
        result1->Append(U'.');
        result1->Append(result, currentChar + 1, resLengthInChars -
                currentChar - 1);
    }
    else {
        result1->Append(result, currentChar, resLengthInChars - currentChar);
    }
    result1->Append(U'E');
    if (exponent > 0) {
        result1->Append(U'+');
    }
    result1->Append(StringUtils::ToString(exponent));
    String str;
    result1->ToString(str);
    return str;
}

String Conversion::ToDecimalScaledString(
    /* [in] */ Long value,
    /* [in] */ Integer scale)
{
    Integer resLengthInChars;
    Integer currentChar;
    Array<Char> result;
    Boolean negNumber = value < 0;
    if (negNumber) {
        value = -value;
    }
    if (value == 0) {
        switch (scale) {
            case 0: return String("0");
            case 1: return String("0.0");
            case 2: return String("0.00");
            case 3: return String("0.000");
            case 4: return String("0.0000");
            case 5: return String("0.00000");
            case 6: return String("0.000000");
            default: {
                AutoPtr<IStringBuilder> result1;
                CStringBuilder::New(IID_IStringBuilder, (IInterface**)&result1);
                if (scale < 0) {
                    result1->Append(String("0E+"));
                }
                else {
                    result1->Append(String("0E"));
                }
                result1->Append((scale == IInteger::MIN_VALUE) ? String("2147483648") : StringUtils::ToString(-scale));
                String str;
                result1->ToString(str);
                return str;
            }
        }
    }
    // one 32-bit unsigned value may contains 10 decimal digits
    resLengthInChars = 18;
    // Explanation why +1+7:
    // +1 - one char for sign if needed.
    // +7 - For "special case 2" (see below) we have 7 free chars for
    //  inserting necessary scaled digits.
    result = Array<Char>(resLengthInChars + 1);
    //  Allocated [resLengthInChars+1] characters.
    // a free latest character may be used for "special case 1" (see below)
    currentChar = resLengthInChars;
    Long v = value;
    do {
        Long prev = v;
        v /= 10;
        result[--currentChar] = (Char)(0x0030 + (prev - v * 10));
    } while(v != 0);

    Long exponent = (Long)resLengthInChars - (Long)currentChar - scale - 1;
    if (scale == 0) {
        if (negNumber) {
            result[--currentChar] = U'-';
        }
        return String(result, currentChar, resLengthInChars - currentChar);
    }
    if (scale > 0 && exponent >= -6) {
        if (exponent >= 0) {
            // special case 1
            Integer insertPoint = currentChar + (Integer)exponent;
            for (Integer j = resLengthInChars - 1; j >= insertPoint; j--) {
                result[j + 1] = result[j];
            }
            result[++insertPoint] = U'.';
            if (negNumber) {
                result[--currentChar] = U'-';
            }
            return String(result, currentChar, resLengthInChars - currentChar + 1);
        }
        // special case 2
        for (Integer j = 2; j < -exponent + 1; j++) {
            result[--currentChar] = U'0';
        }
        result[--currentChar] = U'.';
        result[--currentChar] = U'0';
        if (negNumber) {
            result[--currentChar] = U'-';
        }
        return String(result, currentChar, resLengthInChars - currentChar);
    }
    Integer startPoint = currentChar + 1;
    Integer endPoint = resLengthInChars;
    AutoPtr<IStringBuilder> result1;
    CStringBuilder::New(16 + endPoint - startPoint, IID_IStringBuilder, (IInterface**)&result1);
    if (negNumber) {
        result1->Append(U'-');
    }
    if (endPoint - startPoint >= 1) {
        result1->Append(result[currentChar]);
        result1->Append(U'.');
        result1->Append(result, currentChar + 1, resLengthInChars - currentChar - 1);
    }
    else {
        result1->Append(result, currentChar, resLengthInChars - currentChar);
    }
    result1->Append(U'E');
    if (exponent > 0) {
        result1->Append(U'+');
    }
    result1->Append(StringUtils::ToString(exponent));
    String str;
    result1->ToString(str);
    return str;
}

Long Conversion::DivideLongByBillion(
    /* [in] */ Long a)
{
    Long quot;
    Long rem;

    if (a >= 0) {
        Long bLong = 1000000000LL;
        quot = (a / bLong);
        rem = (a % bLong);
    }
    else {
        /*
         * Make the dividend positive shifting it right by 1 bit then get
         * the quotient an remainder and correct them properly
         */
        Long aPos = ((ULong)a) >> 1;
        Long bPos = ((ULong)1000000000LL) >> 1;
        quot = aPos / bPos;
        rem = aPos % bPos;
        // double the remainder and add 1 if 'a' is odd
        rem = (rem << 1) + (a & 1);
    }
    return ((rem << 32) | (quot & 0xFFFFFFFFLL));
}

Double Conversion::BigInteger2Double(
    /* [in] */ BigInteger* value)
{
    value->PrepareRepresentation();
    // val.bitLength() < 64
    if ((value->mNumberLength < 2) ||
            ((value->mNumberLength == 2) && (value->mDigits[1] > 0))) {
        Long lVal;
        value->LongValue(lVal);
        return lVal;
    }
    // val.bitLength() >= 33 * 32 > 1024
    if (value->mNumberLength > 32) {
        return ((value->mSign > 0) ? IDouble::POSITIVE_INFINITY :
                IDouble::NEGATIVE_INFINITY);
    }
    AutoPtr<IBigInteger> absBI;
    value->Abs(absBI);
    Integer bitLen;
    absBI->BitLength(bitLen);
    Long exponent = bitLen - 1;
    Integer delta = bitLen - 54;
    // We need 54 top bits from this, the 53th bit is always 1 in lVal.
    AutoPtr<IBigInteger> shiftBI;
    absBI->ShiftRight(delta, shiftBI);
    Long lVal;
    INumber::Probe(shiftBI)->LongValue(lVal);
    /*
     * Take 53 bits from lVal to mantissa. The least significant bit is
     * needed for rounding.
     */
    Long mantissa = lVal & 0x1FFFFFFFFFFFFFLL;
    if (exponent == 1023) {
        if (mantissa == 0X1FFFFFFFFFFFFFLL) {
            return ((value->mSign > 0) ? IDouble::POSITIVE_INFINITY :
                    IDouble::NEGATIVE_INFINITY);
        }
        if (mantissa == 0x1FFFFFFFFFFFFELL) {
            return ((value->mSign > 0) ? IDouble::MAX_VALUE : -IDouble::MAX_VALUE);
        }
    }
    // Round the mantissa
    if (((mantissa & 1) == 1) && (((mantissa & 2) == 2) ||
            BitLevel::NonZeroDroppedBits(delta, value->mDigits))) {
        mantissa += 2;
    }
    mantissa >>= 1; // drop the rounding bit
    Long resSign = (value->mSign < 0) ? 0x8000000000000000LL : 0;
    exponent = ((1023 + exponent) << 52) & 0x7FF0000000000000LL;
    Long result = resSign | exponent | mantissa;
    return Math::LongBitsToDouble(result);
}

}
}
