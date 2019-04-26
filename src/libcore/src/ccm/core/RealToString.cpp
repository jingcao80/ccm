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

#include "ccm/core/CStringBuilder.h"
#include "ccm/core/Math.h"
#include "ccm/core/RealToString.h"
#include "ccm/core/StringUtils.h"
#include "ccm/core/cbigint.h"
#include <ccmautoptr.h>

namespace ccm {
namespace core {

Integer RealToString::sFirstK = 0;
Integer RealToString::sDigits[64];
Integer RealToString::sDigitCount = 0;

String RealToString::ResultOrSideEffect(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ const String& s)
{
    if (sb != nullptr) {
        sb->Append(s);
        return String();
    }
    return s;
}

String RealToString::FloatToString(
    /* [in] */ Float f)
{
    return ConvertFloat(nullptr, f);
}

String RealToString::ConvertFloat(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ Float inputNumber)
{
    Integer inputNumberBits = Math::FloatToRawIntegerBits(inputNumber);
    Boolean positive = (inputNumberBits & FLOAT_SIGN_MASK) == 0;
    Integer e = (inputNumberBits & FLOAT_EXPONENT_MASK) >> FLOAT_MANTISSA_BITS;
    Integer f = inputNumberBits & FLOAT_MANTISSA_MASK;
    Boolean mantissaIsZero = f == 0;

    String quickResult;
    if (e == 255) {
        if (mantissaIsZero) {
            quickResult = positive ? "Infinity" : "-Infinity";
        }
        else {
            quickResult = "NaN";
        }
    }
    else if (e == 0 && mantissaIsZero) {
        quickResult = positive ? "0.0" : "-0.0";
    }
    if (!quickResult.IsNullOrEmpty()) {
        return ResultOrSideEffect(sb, quickResult);
    }

    Integer p = FLOAT_EXPONENT_BIAS + FLOAT_MANTISSA_BITS; // the power offset (precision)
    Integer pow;
    Integer numBits = FLOAT_MANTISSA_BITS;
    if (e == 0) {
        pow = 1 - p; // a denormalized number
        if (f < 8) { // want more precision with smallest values
            f = f << 2;
            pow -= 2;
        }
        Integer ff = f;
        while ((ff & 0x00800000) == 0) {
            ff = ff << 1;
            numBits--;
        }
    }
    else {
        // 0 < e < 255
        // a "normalized" number
        f = f | 0x00800000;
        pow = e - p;
    }

    sFirstK = sDigitCount = 0;
    if (-59 < pow && pow < 35 || (pow == -59 && !mantissaIsZero)) {
        LongDigitGenerator(f, pow, e == 0, mantissaIsZero, numBits);
    }
    else {
        BigIntDigitGenerator(f, pow, e == 0, numBits);
    }
    AutoPtr<IStringBuilder> dst = sb;
    if (dst == nullptr) {
        CStringBuilder::New(26, IID_IStringBuilder, (IInterface**)&dst);
    }
    if (inputNumber >= 1e7f || inputNumber <= -1e7f
            || (inputNumber > -1e-3f && inputNumber < 1e-3f)) {
        FreeFormatExponential(dst, positive);
    }
    else {
        FreeFormat(dst, positive);
    }
    String s;
    if (sb == nullptr) {
        dst->ToString(&s);
    }
    return s;
}

String RealToString::DoubleToString(
    /* [in] */ Double d)
{
    return ConvertDouble(nullptr, d);
}

String RealToString::ConvertDouble(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ Double inputNumber)
{
    Long inputNumberBits = Math::DoubleToRawLongBits(inputNumber);
    Boolean positive = (inputNumberBits & DOUBLE_SIGN_MASK) == 0;
    Integer e = (Integer) ((inputNumberBits & DOUBLE_EXPONENT_MASK) >> DOUBLE_MANTISSA_BITS);
    Long f = inputNumberBits & DOUBLE_MANTISSA_MASK;
    Boolean mantissaIsZero = f == 0;

    String quickResult;
    if (e == 2047) {
        if (mantissaIsZero) {
            quickResult = positive ? "Infinity" : "-Infinity";
        }
        else {
            quickResult = "NaN";
        }
    }
    else if (e == 0) {
        if (mantissaIsZero) {
            quickResult = positive ? "0.0" : "-0.0";
        }
        else if (f == 1) {
            // special case to increase precision even though 2 * Double.MIN_VALUE is 1.0e-323
            quickResult = positive ? "4.9E-324" : "-4.9E-324";
        }
    }
    if (!quickResult.IsNullOrEmpty()) {
        return ResultOrSideEffect(sb, quickResult);
    }

    Integer p = DOUBLE_EXPONENT_BIAS + DOUBLE_MANTISSA_BITS; // the power offset (precision)
    Integer pow;
    Integer numBits = DOUBLE_MANTISSA_BITS;
    if (e == 0) {
        pow = 1 - p; // a denormalized number
        Long ff = f;
        while ((ff & 0x0010000000000000ll) == 0) {
            ff = ff << 1;
            numBits--;
        }
    }
    else {
        // 0 < e < 2047
        // a "normalized" number
        f = f | 0x0010000000000000ll;
        pow = e - p;
    }

    sFirstK = sDigitCount = 0;
    if (-59 < pow && pow < 6 || (pow == -59 && !mantissaIsZero)) {
        LongDigitGenerator(f, pow, e == 0, mantissaIsZero, numBits);
    }
    else {
        BigIntDigitGenerator(f, pow, e == 0, numBits);
    }
    AutoPtr<IStringBuilder> dst = sb;
    if (dst == nullptr) {
        CStringBuilder::New(26, IID_IStringBuilder, (IInterface**)&dst);
    }
    if (inputNumber >= 1e7D || inputNumber <= -1e7D
            || (inputNumber > -1e-3D && inputNumber < 1e-3D)) {
        FreeFormatExponential(dst, positive);
    }
    else {
        FreeFormat(dst, positive);
    }
    String s;
    if (sb == nullptr) {
        dst->ToString(&s);
    }
    return s;
}

void RealToString::FreeFormatExponential(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ Boolean positive)
{
    Integer digitIndex = 0;
    if (!positive) {
        sb->Append(U'-');
    }
    sb->Append((Char)(U'0' + sDigits[digitIndex++]));
    sb->Append(U'.');

    Integer k = sFirstK;
    Integer exponent = k;
    while (true) {
        k--;
        if (digitIndex >= sDigitCount) {
            break;
        }
        sb->Append((Char)(U'0' + sDigits[digitIndex++]));
    }

    if (k == exponent - 1) {
        sb->Append(U'0');
    }
    sb->Append(U'E');
    sb->Append(StringUtils::ToString(exponent));
}

void RealToString::FreeFormat(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ Boolean positive)
{
    Integer digitIndex = 0;
    if (!positive) {
        sb->Append(U'-');
    }
    Integer k = sFirstK;
    if (k < 0) {
        sb->Append(U'0');
        sb->Append(U'.');
        for (Integer i = k + 1; i < 0; ++i) {
            sb->Append(U'0');
        }
    }
    Integer U = sDigits[digitIndex++];
    do {
        if (U != -1) {
            sb->Append((Char)(U'0' + U));
        }
        else if (k >= -1) {
            sb->Append(U'0');
        }
        if (k == 0) {
            sb->Append(U'.');
        }
        k--;
        U = digitIndex < sDigitCount ? sDigits[digitIndex++] : -1;
    } while (U != -1 || k >= -1);
}

#define INV_LOG_OF_TEN_BASE_2 (0.30102999566398114)
#define RM_SIZE 21
#define STemp_SIZE 22

/* The algorithm for this particular function can be found in:
 *
 *      Printing Floating-Point Numbers Quickly and Accurately, Robert
 *      G. Burger, and R. Kent Dybvig, Programming Language Design and
 *      Implementation (PLDI) 1996, pp.108-116.
 *
 * The previous implementation of this function combined m+ and m- into
 * one single M which caused some inaccuracy of the last digit. The
 * particular case below shows this inaccuracy:
 *
 *       System.out.println(new Double((1.234123412431233E107)).toString());
 *       System.out.println(new Double((1.2341234124312331E107)).toString());
 *       System.out.println(new Double((1.2341234124312332E107)).toString());
 *
 *       outputs the following:
 *
 *           1.234123412431233E107
 *           1.234123412431233E107
 *           1.234123412431233E107
 *
 *       instead of:
 *
 *           1.234123412431233E107
 *           1.2341234124312331E107
 *           1.2341234124312331E107
 *
 */
void RealToString::BigIntDigitGenerator(
    /* [in] */ Long f,
    /* [in] */ Integer e,
    /* [in] */ Boolean isDenormalized,
    /* [in] */ Integer p)
{
    int RLength, SLength, TempLength, mplus_Length, mminus_Length;
    int high, low, i;
    Integer k, firstK, U;

    uint64_t R[RM_SIZE], S[STemp_SIZE], mplus[RM_SIZE], mminus[RM_SIZE], Temp[STemp_SIZE];

    memset(R     , 0, RM_SIZE    * sizeof (uint64_t));
    memset(S     , 0, STemp_SIZE * sizeof (uint64_t));
    memset(mplus , 0, RM_SIZE    * sizeof (uint64_t));
    memset(mminus, 0, RM_SIZE    * sizeof (uint64_t));
    memset(Temp  , 0, STemp_SIZE * sizeof (uint64_t));

    if (e >= 0) {
        *R = f;
        *mplus = *mminus = 1;
        simpleShiftLeftHighPrecision(mminus, RM_SIZE, e);
        if (f != (2 << (p - 1))) {
            simpleShiftLeftHighPrecision(R, RM_SIZE, e + 1);
            *S = 2;
            /*
             * m+ = m+ << e results in 1.0e23 to be printed as
             * 0.9999999999999999E23
             * m+ = m+ << e+1 results in 1.0e23 to be printed as
             * 1.0e23 (caused too much rounding)
             *      470fffffffffffff = 2.0769187434139308E34
             *      4710000000000000 = 2.076918743413931E34
             */
            simpleShiftLeftHighPrecision(mplus, RM_SIZE, e);
        }
        else {
            simpleShiftLeftHighPrecision(R, RM_SIZE, e + 2);
            *S = 4;
            simpleShiftLeftHighPrecision(mplus, RM_SIZE, e + 1);
        }
    }
    else {
        if (isDenormalized || (f != (2 << (p - 1)))) {
            *R = f << 1;
            *S = 1;
            simpleShiftLeftHighPrecision(S, STemp_SIZE, 1 - e);
            *mplus = *mminus = 1;
        }
        else {
            *R = f << 2;
            *S = 1;
            simpleShiftLeftHighPrecision(S, STemp_SIZE, 2 - e);
            *mplus = 2;
            *mminus = 1;
        }
    }

    k = static_cast<int>(Math::Ceil((e + p - 1) * INV_LOG_OF_TEN_BASE_2 - 1e-10));

    if (k > 0) {
        timesTenToTheEHighPrecision(S, STemp_SIZE, k);
    }
    else {
        timesTenToTheEHighPrecision(R     , RM_SIZE, -k);
        timesTenToTheEHighPrecision(mplus , RM_SIZE, -k);
        timesTenToTheEHighPrecision(mminus, RM_SIZE, -k);
    }

    RLength = mplus_Length = mminus_Length = RM_SIZE;
    SLength = TempLength = STemp_SIZE;

    memset(Temp + RM_SIZE, 0, (STemp_SIZE - RM_SIZE) * sizeof(uint64_t));
    memcpy(Temp, R, RM_SIZE * sizeof(uint64_t));

    while (RLength > 1 && R[RLength - 1] == 0) {
        --RLength;
    }
    while (mplus_Length > 1 && mplus[mplus_Length - 1] == 0) {
        --mplus_Length;
    }
    while (mminus_Length > 1 && mminus[mminus_Length - 1] == 0) {
        --mminus_Length;
    }
    while (SLength > 1 && S[SLength - 1] == 0) {
        --SLength;
    }
    TempLength = (RLength > mplus_Length ? RLength : mplus_Length) + 1;
    addHighPrecision(Temp, TempLength, mplus, mplus_Length);

    if (compareHighPrecision(Temp, TempLength, S, SLength) >= 0) {
        firstK = k;
    }
    else {
        firstK = k - 1;
        simpleAppendDecimalDigitHighPrecision(R     , ++RLength      , 0);
        simpleAppendDecimalDigitHighPrecision(mplus , ++mplus_Length , 0);
        simpleAppendDecimalDigitHighPrecision(mminus, ++mminus_Length, 0);
        while (RLength > 1 && R[RLength - 1] == 0) {
            --RLength;
        }
        while (mplus_Length > 1 && mplus[mplus_Length - 1] == 0) {
            --mplus_Length;
        }
        while (mminus_Length > 1 && mminus[mminus_Length - 1] == 0) {
            --mminus_Length;
        }
    }

    Integer digitCount = 0;
    do {
        U = 0;
        for (i = 3; i >= 0; --i) {
            TempLength = SLength + 1;
            Temp[SLength] = 0;
            memcpy(Temp, S, SLength * sizeof(uint64_t));
            simpleShiftLeftHighPrecision(Temp, TempLength, i);
            if (compareHighPrecision(R, RLength, Temp, TempLength) >= 0) {
                subtractHighPrecision(R, RLength, Temp, TempLength);
                U += 1 << i;
            }
        }

        low = compareHighPrecision(R, RLength, mminus, mminus_Length) <= 0;

        memset(Temp + RLength, 0, (STemp_SIZE - RLength) * sizeof(uint64_t));
        memcpy(Temp, R, RLength * sizeof(uint64_t));
        TempLength = (RLength > mplus_Length ? RLength : mplus_Length) + 1;
        addHighPrecision(Temp, TempLength, mplus, mplus_Length);

        high = compareHighPrecision(Temp, TempLength, S, SLength) >= 0;

        if (low || high) {
            break;
        }

        simpleAppendDecimalDigitHighPrecision(R     , ++RLength      , 0);
        simpleAppendDecimalDigitHighPrecision(mplus , ++mplus_Length , 0);
        simpleAppendDecimalDigitHighPrecision(mminus, ++mminus_Length, 0);
        while (RLength > 1 && R[RLength - 1] == 0) {
            --RLength;
        }
        while (mplus_Length > 1 && mplus[mplus_Length - 1] == 0) {
            --mplus_Length;
        }
        while (mminus_Length > 1 && mminus[mminus_Length - 1] == 0) {
            --mminus_Length;
        }
        sDigits[digitCount++] = U;
    } while (1);

    simpleShiftLeftHighPrecision(R, ++RLength, 1);
    if (low && !high) {
        sDigits[digitCount++] = U;
    }
    else if (high && !low) {
        sDigits[digitCount++] = U + 1;
    }
    else if (compareHighPrecision(R, RLength, S, SLength) < 0) {
        sDigits[digitCount++] = U;
    }
    else {
        sDigits[digitCount++] = U + 1;
    }

    sDigitCount = digitCount;
    sFirstK = firstK;
}

void RealToString::LongDigitGenerator(
    /* [in] */ Long f,
    /* [in] */ Integer e,
    /* [in] */ Boolean isDenormalized,
    /* [in] */ Boolean mantissaIsZero,
    /* [in] */ Integer p)
{
    static const Double sInvLogOfTenBaseTwo = Math::Log(2.0) / Math::Log(10.0);

    Long R, S, M;
    if (e >= 0) {
        M = 1ll << e;
        if (!mantissaIsZero) {
            R = f << (e + 1);
            S = 2;
        }
        else {
            R = f << (e + 2);
            S = 4;
        }
    }
    else {
        M = 1;
        if (isDenormalized || !mantissaIsZero) {
            R = f << 1;
            S = 1ll << (1 - e);
        }
        else {
            R = f << 2;
            S = 1ll << (2 - e);
        }
    }

    Integer k = (Integer)Math::Ceil((e + p - 1) * sInvLogOfTenBaseTwo - 1e-10);

    if (k > 0) {
        S = S * Math::LONG_POWERS_OF_TEN[k];
    }
    else if (k < 0) {
        Long scale = Math::LONG_POWERS_OF_TEN[-k];
        R = R * scale;
        M = M == 1 ? scale : M * scale;
    }

    if (R + M > S) { // was M_plus
        sFirstK = k;
    }
    else {
        sFirstK = k - 1;
        R = R * 10;
        M = M * 10;
    }

    Boolean low, high;
    Integer U;
    while (true) {
        // Set U to floor(R/S) and R to the remainder, using *unsigned* 64-bit division
        U = 0;
        for (Integer i = 3; i >= 0; i--) {
            Long remainder = R - (S << i);
            if (remainder >= 0) {
                R = remainder;
                U += 1 << i;
            }
        }

        low = R < M; // was M_minus
        high = R + M > S; // was M_plus

        if (low || high) {
            break;
        }
        R = R * 10;
        M = M * 10;
        sDigits[sDigitCount++] = U;
    }
    if (low && !high) {
        sDigits[sDigitCount++] = U;
    }
    else if (high && !low) {
        sDigits[sDigitCount++] = U + 1;
    }
    else if ((R << 1) < S) {
        sDigits[sDigitCount++] = U;
    }
    else {
        sDigits[sDigitCount++] = U + 1;
    }
}

}
}
