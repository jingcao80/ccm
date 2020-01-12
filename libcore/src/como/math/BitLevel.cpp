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
#include "como/core/Math.h"
#include "como/math/BitLevel.h"

using como::core::Math;

namespace como {
namespace math {

Integer BitLevel::BitLength(
    /* [in] */ BigInteger* value)
{
    value->PrepareRepresentation();
    if (value->mSign == 0) {
        return 0;
    }
    Integer bLength = (value->mNumberLength << 5);
    Integer highDigit = value->mDigits[value->mNumberLength - 1];

    if (value->mSign < 0) {
        Integer i = value->GetFirstNonzeroDigit();
        // We reduce the problem to the positive case.
        if (i == value->mNumberLength - 1) {
            highDigit--;
        }
    }
    // Subtracting all sign bits
    bLength -= Math::NumberOfLeadingZeros(highDigit);
    return bLength;
}

Integer BitLevel::BitCount(
    /* [in] */ BigInteger* value)
{
    value->PrepareRepresentation();
    Integer bCount = 0;

    if (value->mSign == 0) {
        return 0;
    }

    Integer i = value->GetFirstNonzeroDigit();
    if (value->mSign > 0) {
        for (; i < value->mNumberLength; i++) {
            bCount += Math::BitCount(value->mDigits[i]);
        }
    }
    else {
        // this digit absorbs the carry
        bCount += Math::BitCount(-value->mDigits[i]);
        for (i++; i < value->mNumberLength; i++) {
            bCount += Math::BitCount(~value->mDigits[i]);
        }
        // We take the complement sum:
        bCount = (value->mNumberLength << 5) - bCount;
    }
    return bCount;
}

Boolean BitLevel::NonZeroDroppedBits(
    /* [in] */ Integer numberOfBits,
    /* [in] */ Array<Integer>& digits)
{
    Integer intCount = numberOfBits >> 5;
    Integer bitCount = numberOfBits & 31;
    Integer i;

    for (i = 0; (i < intCount) && (digits[i] == 0); i++) {
        ;
    }
    return ((i != intCount) || (digits[i] << (32 - bitCount) != 0));
}

void BitLevel::ShiftLeftOneBit(
    /* [in] */ Array<Integer>& result,
    /* [in] */ Array<Integer>& source,
    /* [in] */ Integer srcLen)
{
    Integer carry = 0;
    for (Integer i = 0; i < srcLen; i++) {
        Integer value = source[i];
        result[i] = (value << 1) | carry;
        carry = ((UInteger)value) >> 31;
    }
    if (carry != 0) {
        result[srcLen] = carry;
    }
}

ECode BitLevel::ShiftLeftOneBit(
    /* [in] */ BigInteger* source,
    /* [out] */ IBigInteger** result)
{
    source->PrepareRepresentation();
    Integer srcLen = source->mNumberLength;
    Integer resLen = srcLen + 1;
    Array<Integer> resDigits(resLen);
    ShiftLeftOneBit(resDigits, source->mDigits, srcLen);
    return CBigInteger::New(source->mSign, resLen, resDigits, IID_IBigInteger, (IInterface**)result);
}

ECode BitLevel::ShiftRight(
    /* [in] */ BigInteger* source,
    /* [in] */ Integer count,
    /* [out] */ IBigInteger** result)
{
    source->PrepareRepresentation();
    Integer intCount = count >> 5; // count of integers
    count &= 31; // count of remaining bits
    if (intCount >= source->mNumberLength) {
        AutoPtr<IBigInteger> bi = ((source->mSign < 0) ? CBigInteger::GetMINUS_ONE() : CBigInteger::GetZERO());
        bi.MoveTo(result);
        return NOERROR;
    }
    Integer i;
    Integer resLength = source->mNumberLength - intCount;
    Array<Integer> resDigits(resLength + 1);

    ShiftRight(resDigits, resLength, source->mDigits, intCount, count);
    if (source->mSign < 0) {
        // Checking if the dropped bits are zeros (the remainder equals to
        // 0)
        for (i = 0; (i < intCount) && (source->mDigits[i] == 0); i++) {
            ;
        }
        // If the remainder is not zero, add 1 to the result
        if ((i < intCount) || ((count > 0) &&
                ((source->mDigits[i] << (32 - count)) != 0))) {
            for (i = 0; (i < resLength) && (resDigits[i] == -1); i++) {
                resDigits[i] = 0;
            }
            if (i == resLength) {
                resLength++;
            }
            resDigits[i]++;
        }
    }
    return CBigInteger::New(source->mSign, resLength, resDigits, IID_IBigInteger, (IInterface**)result);
}

Boolean BitLevel::ShiftRight(
    /* [in] */ Array<Integer>& result,
    /* [in] */ Integer resultLen,
    /* [in] */ Array<Integer>& source,
    /* [in] */ Integer intCount,
    /* [in] */ Integer count)
{
    Integer i;
    Boolean allZero = true;
    for (i = 0; i < intCount; i++) {
        allZero &= source[i] == 0;
    }
    if (count == 0) {
        result.Copy(0, source, intCount, resultLen);
        i = resultLen;
    }
    else {
        Integer leftShiftCount = 32 - count;

        allZero &= (source[i] << leftShiftCount) == 0;
        for (i = 0; i < resultLen - 1; i++) {
            result[i] = (((UInteger)source[i + intCount]) >> count) |
                (source[i + intCount + 1] << leftShiftCount);
        }
        result[i] = (((UInteger)source[i + intCount]) >> count);
        i++;
    }

    return allZero;
}

ECode BitLevel::FlipBit(
    /* [in] */ BigInteger* value,
    /* [in] */ Integer n,
    /* [out] */ IBigInteger** result)
{
    value->PrepareRepresentation();
    Integer resSign = (value->mSign == 0) ? 1 : value->mSign;
    Integer intCount = n >> 5;
    Integer bitN = n & 31;
    Integer resLength = Math::Max(intCount + 1, value->mNumberLength) + 1;
    Array<Integer> resDigits(resLength);
    Integer i;

    Integer bitNumber = 1 << bitN;
    resDigits.Copy(0, value->mDigits, 0, value->mNumberLength);

    if (value->mSign < 0) {
        if (intCount >= value->mNumberLength) {
            resDigits[intCount] = bitNumber;
        }
        else {
            Integer firstNonZeroDigit = value->GetFirstNonzeroDigit();
            if (intCount > firstNonZeroDigit) {
                resDigits[intCount] ^= bitNumber;
            }
            else if (intCount < firstNonZeroDigit) {
                resDigits[intCount] = -bitNumber;
                for (i = intCount + 1; i < firstNonZeroDigit; i++) {
                    resDigits[i] = -1;
                }
                resDigits[i] = resDigits[i]--;
            }
            else {
                i = intCount;
                resDigits[i] = -((-resDigits[intCount]) ^ bitNumber);
                if (resDigits[i] == 0) {
                    for (i++; resDigits[i] == -1; i++) {
                        resDigits[i] = 0;
                    }
                    resDigits[i]++;
                }
            }
        }
    }
    else {
        resDigits[intCount] ^= bitNumber;
    }
    return CBigInteger::New(resSign, resLength, resDigits, IID_IBigInteger, (IInterface**)result);
}

}
}
