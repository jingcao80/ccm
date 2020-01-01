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

#include "como/core/Character.h"
#include "como/core/CoreUtils.h"
#include "como/core/Math.h"
#include "como/core/StringUtils.h"
#include "como/core/ThreadLocal.h"
#include "como/misc/CFDBigInteger.h"
#include "como/misc/FloatingDecimal.h"
#include "como/util/Arrays.h"
#include "como/util/regex/Pattern.h"
#include "como.core.IDouble.h"
#include "como.core.IFloat.h"
#include "como.core.IInteger.h"
#include "como.core.IStringBuffer.h"
#include "como.core.IStringBuilder.h"
#include "como.util.regex.IMatcher.h"
#include <ccmlogger.h>

using como::core::Character;
using como::core::CoreUtils;
using como::core::E_ASSERTION_ERROR;
using como::core::E_NUMBER_FORMAT_EXCEPTION;
using como::core::IDouble;
using como::core::IFloat;
using como::core::IInteger;
using como::core::IStringBuffer;
using como::core::IStringBuilder;
using como::core::Math;
using como::core::StringUtils;
using como::core::ThreadLocal;
using como::util::Arrays;
using como::util::regex::IMatcher;
using como::util::regex::Pattern;

namespace como {
namespace misc {

FloatingDecimal::ExceptionalBinaryToASCIIBuffer::ExceptionalBinaryToASCIIBuffer(
    /* [in] */ const String& image,
    /* [in] */ Boolean isNegative)
    : mImage(image)
    , mIsNegative(isNegative)
{}

COMO_INTERFACE_IMPL_1(FloatingDecimal::ExceptionalBinaryToASCIIBuffer, Object, IFloatingDecimalBinaryToASCIIConverter);

ECode FloatingDecimal::ExceptionalBinaryToASCIIBuffer::ToFormatString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    *str = mImage;
    return NOERROR;
}

ECode FloatingDecimal::ExceptionalBinaryToASCIIBuffer::AppendTo(
    /* [in] */ IAppendable* buf)
{
    VALIDATE_NOT_NULL(buf);

    if (IStringBuilder::Probe(buf) != nullptr) {
        return IStringBuilder::Probe(buf)->Append(mImage);
    }
    else if (IStringBuffer::Probe(buf) != nullptr) {
        return IStringBuffer::Probe(buf)->Append(mImage);
    }
    else {
        CHECK(0);
        return NOERROR;
    }
}

ECode FloatingDecimal::ExceptionalBinaryToASCIIBuffer::GetDecimalExponent(
    /* [out] */ Integer* exponent)
{
    Logger::E("FloatingDecimal", "Exceptional value does not have an exponent");
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode FloatingDecimal::ExceptionalBinaryToASCIIBuffer::GetDigits(
    /* [out] */ Array<Char>& digits,
    /* [out] */ Integer* number)
{
    Logger::E("FloatingDecimal", "Exceptional value does not have digits");
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode FloatingDecimal::ExceptionalBinaryToASCIIBuffer::IsNegative(
    /* [out] */ Boolean* neg)
{
    VALIDATE_NOT_NULL(neg);

    *neg = mIsNegative;
    return NOERROR;
}

ECode FloatingDecimal::ExceptionalBinaryToASCIIBuffer::IsExceptional(
    /* [out] */ Boolean* exceptional)
{
    VALIDATE_NOT_NULL(exceptional);

    *exceptional = true;
    return NOERROR;
}

ECode FloatingDecimal::ExceptionalBinaryToASCIIBuffer::DigitsRoundedUp(
    /* [out] */ Boolean* roundedUp)
{
    Logger::E("FloatingDecimal", "Exceptional value is not rounded");
    return NOERROR;
}

ECode FloatingDecimal::ExceptionalBinaryToASCIIBuffer::DecimalDigitsExact(
    /* [out] */ Boolean* exact)
{
    Logger::E("FloatingDecimal", "Exceptional value is not exact");
    return NOERROR;
}

//-------------------------------------------------------------------------------------

constexpr Integer FloatingDecimal::BinaryToASCIIBuffer::sInsignificantDigitsNumber[];
constexpr Integer FloatingDecimal::BinaryToASCIIBuffer::N_5_BITS[];

FloatingDecimal::BinaryToASCIIBuffer::BinaryToASCIIBuffer()
    : mDigits(20)
    , mBuffer(26)
{}

FloatingDecimal::BinaryToASCIIBuffer::BinaryToASCIIBuffer(
    /* [in] */ Boolean isNegative,
    /* [in] */ Array<Char>& digits)
    : mIsNegative(isNegative)
    , mDigits(digits)
{
    mNDigits = digits.GetLength();
}

COMO_INTERFACE_IMPL_1(FloatingDecimal::BinaryToASCIIBuffer, Object, IFloatingDecimalBinaryToASCIIConverter);

ECode FloatingDecimal::BinaryToASCIIBuffer::ToFormatString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    Integer len = GetChars(mBuffer);
    *str = String(mBuffer, 0, len);
    return NOERROR;
}

ECode FloatingDecimal::BinaryToASCIIBuffer::AppendTo(
    /* [in] */ IAppendable* buf)
{
    Integer len = GetChars(mBuffer);
    if (IStringBuilder::Probe(buf) != nullptr) {
        IStringBuilder::Probe(buf)->Append(mBuffer, 0, len);
    }
    else if (IStringBuffer::Probe(buf) != nullptr) {
        IStringBuffer::Probe(buf)->Append(mBuffer, 0, len);
    }
    else {
        CHECK(0);
    }
    return NOERROR;
}

ECode FloatingDecimal::BinaryToASCIIBuffer::GetDecimalExponent(
    /* [out] */ Integer* exponent)
{
    VALIDATE_NOT_NULL(exponent);

    *exponent = mDecExponent;
    return NOERROR;
}

ECode FloatingDecimal::BinaryToASCIIBuffer::GetDigits(
    /* [out] */ Array<Char>& digits,
    /* [out] */ Integer* number)
{
    VALIDATE_NOT_NULL(number);

    digits.Copy(0, mDigits, mFirstDigitIndex, mNDigits);
    *number = mNDigits;
    return NOERROR;
}

ECode FloatingDecimal::BinaryToASCIIBuffer::IsNegative(
    /* [out] */ Boolean* neg)
{
    VALIDATE_NOT_NULL(neg);

    *neg = mIsNegative;
    return NOERROR;
}

ECode FloatingDecimal::BinaryToASCIIBuffer::IsExceptional(
    /* [out] */ Boolean* exceptional)
{
    VALIDATE_NOT_NULL(exceptional);

    *exceptional = false;
    return NOERROR;
}

ECode FloatingDecimal::BinaryToASCIIBuffer::DigitsRoundedUp(
    /* [out] */ Boolean* roundedUp)
{
    VALIDATE_NOT_NULL(roundedUp);

    *roundedUp = mDecimalDigitsRoundedUp;
    return NOERROR;
}

ECode FloatingDecimal::BinaryToASCIIBuffer::DecimalDigitsExact(
    /* [out] */ Boolean* exact)
{
    VALIDATE_NOT_NULL(exact);

    *exact = mExactDecimalConversion;
    return NOERROR;
}

void FloatingDecimal::BinaryToASCIIBuffer::DevelopLongDigits(
    /* [in] */ Integer decExponent,
    /* [in] */ Long lvalue,
    /* [in] */ Integer insignificantDigits)
{
    if (insignificantDigits != 0) {
        // Discard non-significant low-order bits, while rounding,
        // up to insignificant value.
        Long pow10 = CFDBigInteger::LONG_5_POW[insignificantDigits] << insignificantDigits; // 10^i == 5^i * 2^i;
        Long residue = lvalue % pow10;
        lvalue /= pow10;
        decExponent += insignificantDigits;
        if (residue >= (pow10 >> 1)){
            // round up based on the low-order bits we're discarding
            lvalue++;
        }
    }
    Integer digitno = mDigits.GetLength() - 1;
    Integer c;
    if (lvalue <= IInteger::MAX_VALUE) {
        CHECK(lvalue > 0LL);
        // even easier subcase!
        // can do int arithmetic rather than long!
        Integer ivalue = (Integer)lvalue;
        c = ivalue % 10;
        ivalue /= 10;
        while (c == 0) {
            decExponent++;
            c = ivalue % 10;
            ivalue /= 10;
        }
        while (ivalue != 0) {
            mDigits[digitno--] = (Char)(c + U'0');
            decExponent++;
            c = ivalue % 10;
            ivalue /= 10;
        }
        mDigits[digitno] = (Char)(c + U'0');
    }
    else {
        // same algorithm as above (same bugs, too )
        // but using long arithmetic.
        c = (Integer)(lvalue % 10LL);
        lvalue /= 10LL;
        while (c == 0) {
            decExponent++;
            c = (Integer)(lvalue % 10LL);
            lvalue /= 10LL;
        }
        while (lvalue != 0LL) {
            mDigits[digitno--] = (Char)(c + U'0');
            decExponent++;
            c = (Integer)(lvalue % 10LL);
            lvalue /= 10;
        }
        mDigits[digitno] = (Char)(c + U'0');
    }
    mDecExponent = decExponent + 1;
    mFirstDigitIndex = digitno;
    mNDigits = mDigits.GetLength() - digitno;
}

void FloatingDecimal::BinaryToASCIIBuffer::Dtoa(
    /* [in] */ Integer binExp,
    /* [in] */ Long fractBits,
    /* [in] */ Integer nSignificantBits,
    /* [in] */ Boolean isCompatibleFormat)
{
    CHECK(fractBits > 0);
    CHECK((fractBits & FRACT_HOB) != 0);
    // Examine number. Determine if it is an easy case,
    // which we can do pretty trivially using float/long conversion,
    // or whether we must do real work.
    const Integer tailZeros = Math::NumberOfTrailingZeros(fractBits);

    // number of significant bits of fractBits;
    const Integer nFractBits = EXP_SHIFT + 1 - tailZeros;

    // reset flags to default values as dtoa() does not always set these
    // flags and a prior call to dtoa() might have set them to incorrect
    // values with respect to the current state.
    mDecimalDigitsRoundedUp = false;
    mExactDecimalConversion = false;

    // number of significant bits to the right of the point.
    Integer nTinyBits = Math::Max(0, nFractBits - binExp - 1);
    if (binExp <= MAX_SMALL_BIN_EXP && binExp >= MIN_SMALL_BIN_EXP) {
        // Look more closely at the number to decide if,
        // with scaling by 10^nTinyBits, the result will fit in
        // a long.
        if ((nTinyBits < ArrayLength(CFDBigInteger::LONG_5_POW)) && ((nFractBits + N_5_BITS[nTinyBits]) < 64)) {
            //
            // We can do this:
            // take the fraction bits, which are normalized.
            // (a) nTinyBits == 0: Shift left or right appropriately
            //     to align the binary point at the extreme right, i.e.
            //     where a long int point is expected to be. The integer
            //     result is easily converted to a string.
            // (b) nTinyBits > 0: Shift right by EXP_SHIFT-nFractBits,
            //     which effectively converts to long and scales by
            //     2^nTinyBits. Then multiply by 5^nTinyBits to
            //     complete the scaling. We know this won't overflow
            //     because we just counted the number of bits necessary
            //     in the result. The integer you get from this can
            //     then be converted to a string pretty easily.
            //
            if (nTinyBits == 0) {
                Integer insignificant;
                if (binExp > nSignificantBits) {
                    insignificant = InsignificantDigitsForPow2(binExp - nSignificantBits - 1);
                }
                else {
                    insignificant = 0;
                }
                if (binExp >= EXP_SHIFT) {
                    fractBits <<= (binExp - EXP_SHIFT);
                }
                else {
                    fractBits = ((unsigned Long)fractBits)>> (EXP_SHIFT - binExp);
                }
                DevelopLongDigits(0, fractBits, insignificant);
                return;
            }
            //
            // The following causes excess digits to be printed
            // out in the single-float case. Our manipulation of
            // halfULP here is apparently not correct. If we
            // better understand how this works, perhaps we can
            // use this special case again. But for the time being,
            // we do not.
            // else {
            //     fractBits >>>= EXP_SHIFT+1-nFractBits;
            //     fractBits//= long5pow[ nTinyBits ];
            //     halfULP = long5pow[ nTinyBits ] >> (1+nSignificantBits-nFractBits);
            //     developLongDigits( -nTinyBits, fractBits, insignificantDigits(halfULP) );
            //     return;
            // }
            //
        }
    }
    //
    // This is the hard case. We are going to compute large positive
    // integers B and S and integer decExp, s.t.
    //      d = ( B / S )// 10^decExp
    //      1 <= B / S < 10
    // Obvious choices are:
    //      decExp = floor( log10(d) )
    //      B      = d// 2^nTinyBits// 10^max( 0, -decExp )
    //      S      = 10^max( 0, decExp)// 2^nTinyBits
    // (noting that nTinyBits has already been forced to non-negative)
    // I am also going to compute a large positive integer
    //      M      = (1/2^nSignificantBits)// 2^nTinyBits// 10^max( 0, -decExp )
    // i.e. M is (1/2) of the ULP of d, scaled like B.
    // When we iterate through dividing B/S and picking off the
    // quotient bits, we will know when to stop when the remainder
    // is <= M.
    //
    // We keep track of powers of 2 and powers of 5.
    //
    Integer decExp = EstimateDecExp(fractBits, binExp);
    int B2, B5; // powers of 2 and powers of 5, respectively, in B
    int S2, S5; // powers of 2 and powers of 5, respectively, in S
    int M2, M5; // powers of 2 and powers of 5, respectively, in M

    B5 = Math::Max(0, -decExp);
    B2 = B5 + nTinyBits + binExp;

    S5 = Math::Max(0, decExp);
    S2 = S5 + nTinyBits;

    M5 = B5;
    M2 = B2 - nSignificantBits;

    //
    // the long integer fractBits contains the (nFractBits) interesting
    // bits from the mantissa of d ( hidden 1 added if necessary) followed
    // by (EXP_SHIFT+1-nFractBits) zeros. In the interest of compactness,
    // I will shift out those zeros before turning fractBits into a
    // FDBigInteger. The resulting whole number will be
    //      d * 2^(nFractBits-1-binExp).
    //
    fractBits = ((unsigned Long)fractBits) >> tailZeros;
    B2 -= nFractBits - 1;
    Integer common2factor = Math::Min(B2, S2);
    B2 -= common2factor;
    S2 -= common2factor;
    M2 -= common2factor;

    //
    // HACK!! For exact powers of two, the next smallest number
    // is only half as far away as we think (because the meaning of
    // ULP changes at power-of-two bounds) for this reason, we
    // hack M2. Hope this works.
    //
    if (nFractBits == 1) {
        M2 -= 1;
    }

    if (M2 < 0) {
        // oops.
        // since we cannot scale M down far enough,
        // we must scale the other values up.
        B2 -= M2;
        S2 -= M2;
        M2 =  0;
    }
    //
    // Construct, Scale, iterate.
    // Some day, we'll write a stopping test that takes
    // account of the asymmetry of the spacing of floating-point
    // numbers below perfect powers of 2
    // 26 Sept 96 is not that day.
    // So we use a symmetric test.
    //
    Integer ndigit = 0;
    Boolean low, high;
    Long lowDigitDifference;
    Integer q;

    //
    // Detect the special cases where all the numbers we are about
    // to compute will fit in int or long integers.
    // In these cases, we will avoid doing FDBigInteger arithmetic.
    // We use the same algorithms, except that we "normalize"
    // our FDBigIntegers before iterating. This is to make division easier,
    // as it makes our fist guess (quotient of high-order words)
    // more accurate!
    //
    // Some day, we'll write a stopping test that takes
    // account of the asymmetry of the spacing of floating-point
    // numbers below perfect powers of 2
    // 26 Sept 96 is not that day.
    // So we use a symmetric test.
    //
    // binary digits needed to represent B, approx.
    Integer Bbits = nFractBits + B2 + ((B5 < ArrayLength(N_5_BITS)) ? N_5_BITS[B5] : (B5 * 3));

    // binary digits needed to represent 10*S, approx.
    Integer tenSbits = S2 + 1 + (((S5 + 1) < ArrayLength(N_5_BITS)) ? N_5_BITS[(S5 + 1)] : ((S5 + 1) * 3));
    if (Bbits < 64 && tenSbits < 64) {
        if (Bbits < 32 && tenSbits < 32) {
            // wa-hoo! They're all ints!
            Integer b = ((Integer)fractBits * CFDBigInteger::SMALL_5_POW[B5]) << B2;
            Integer s = CFDBigInteger::SMALL_5_POW[S5] << S2;
            Integer m = CFDBigInteger::SMALL_5_POW[M5] << M2;
            Integer tens = s * 10;
            //
            // Unroll the first iteration. If our decExp estimate
            // was too high, our first quotient will be zero. In this
            // case, we discard it and decrement decExp.
            //
            ndigit = 0;
            q = b / s;
            b = 10 * (b % s);
            m *= 10;
            low = (b < m);
            high = (b + m > tens);
            CHECK(q < 10);
            if ((q == 0) && !high) {
                // oops. Usually ignore leading zero.
                decExp--;
            }
            else {
                mDigits[ndigit++] = (Char)(U'0' + q);
            }
            //
            // HACK! Java spec sez that we always have at least
            // one digit after the . in either F- or E-form output.
            // Thus we will need more than one digit if we're using
            // E-form
            //
            if (!isCompatibleFormat ||decExp < -3 || decExp >= 8) {
                high = low = false;
            }
            while(!low && !high) {
                q = b / s;
                b = 10 * (b % s);
                m *= 10;
                CHECK(q < 10);
                if (m > 0LL){
                    low = (b < m);
                    high = (b + m > tens);
                }
                else {
                    // hack -- m might overflow!
                    // in this case, it is certainly > b,
                    // which won't
                    // and b+m > tens, too, since that has overflowed
                    // either!
                    low = true;
                    high = true;
                }
                mDigits[ndigit++] = (Char)(U'0' + q);
            }
            lowDigitDifference = (b << 1) - tens;
            mExactDecimalConversion = (b == 0);
        }
        else {
            // still good! they're all longs!
            Long b = (fractBits * CFDBigInteger::LONG_5_POW[B5]) << B2;
            Long s = CFDBigInteger::LONG_5_POW[S5] << S2;
            Long m = CFDBigInteger::LONG_5_POW[M5] << M2;
            Long tens = s * 10LL;
            //
            // Unroll the first iteration. If our decExp estimate
            // was too high, our first quotient will be zero. In this
            // case, we discard it and decrement decExp.
            //
            ndigit = 0;
            q = (Integer) (b / s);
            b = 10LL * (b % s);
            m *= 10LL;
            low = (b < m);
            high = (b + m > tens);
            CHECK(q < 10);
            if ((q == 0) && !high){
                // oops. Usually ignore leading zero.
                decExp--;
            }
            else {
                mDigits[ndigit++] = (Char)(U'0' + q);
            }
            //
            // HACK! Java spec sez that we always have at least
            // one digit after the . in either F- or E-form output.
            // Thus we will need more than one digit if we're using
            // E-form
            //
            if (!isCompatibleFormat || decExp < -3 || decExp >= 8) {
                high = low = false;
            }
            while(!low && !high) {
                q = (Integer) (b / s);
                b = 10 * (b % s);
                m *= 10;
                CHECK(q < 10);
                if (m > 0LL) {
                    low = (b < m);
                    high = (b + m > tens);
                }
                else {
                    // hack -- m might overflow!
                    // in this case, it is certainly > b,
                    // which won't
                    // and b+m > tens, too, since that has overflowed
                    // either!
                    low = true;
                    high = true;
                }
                mDigits[ndigit++] = (Char)(U'0' + q);
            }
            lowDigitDifference = (b << 1) - tens;
            mExactDecimalConversion = (b == 0);
        }
    }
    else {
        //
        // We really must do FDBigInteger arithmetic.
        // Fist, construct our FDBigInteger initial values.
        //
        AutoPtr<IFDBigInteger> sval = CFDBigInteger::ValueOfPow52(S5, S2);
        Integer shiftBias;
        sval->GetNormalizationBias(&shiftBias);
        AutoPtr<IFDBigInteger> tempVal;
        sval->LeftShift(shiftBias, &tempVal);
        sval = std::move(tempVal);

        AutoPtr<IFDBigInteger> bval = CFDBigInteger::ValueOfMulPow52(fractBits, B5, B2 + shiftBias);
        AutoPtr<IFDBigInteger> mval = CFDBigInteger::ValueOfPow52(M5 + 1, M2 + shiftBias + 1);

        AutoPtr<IFDBigInteger> tenSval = CFDBigInteger::ValueOfPow52(S5 + 1, S2 + shiftBias + 1); //Sval.mult( 10 );
        //
        // Unroll the first iteration. If our decExp estimate
        // was too high, our first quotient will be zero. In this
        // case, we discard it and decrement decExp.
        //
        ndigit = 0;
        bval->QuoRemIteration(sval, &q);
        Integer comRes;
        bval->Cmp(mval, &comRes);
        low = (comRes < 0);
        tenSval->AddAndCmp(bval, mval, &comRes);
        high = (comRes <= 0);

        CHECK(q < 10);
        if ((q == 0) && !high) {
            // oops. Usually ignore leading zero.
            decExp--;
        }
        else {
            mDigits[ndigit++] = (Char)(U'0' + q);
        }
        //
        // HACK! Java spec sez that we always have at least
        // one digit after the . in either F- or E-form output.
        // Thus we will need more than one digit if we're using
        // E-form
        //
        if (!isCompatibleFormat || decExp < -3 || decExp >= 8) {
            high = low = false;
        }
        while(!low && !high){
            bval->QuoRemIteration(sval, &q);
            CHECK(q < 10);
            mval->MultBy10(&tempVal); //Mval = Mval.mult( 10 );
            mval = std::move(tempVal);
            bval->Cmp(mval, &comRes);
            low = (comRes < 0);
            tenSval->AddAndCmp(bval, mval, &comRes);
            high = (comRes <= 0);
            mDigits[ndigit++] = (Char)(U'0' + q);
        }
        if (high && low) {
            bval->LeftShift(1, &tempVal);
            bval = std::move(tempVal);
            bval->Cmp(tenSval, &comRes);
            lowDigitDifference = comRes;
        }
        else {
            lowDigitDifference = 0LL; // this here only for flow analysis!
        }
        bval->Cmp(CFDBigInteger::GetZERO(), &comRes);
        mExactDecimalConversion = (comRes == 0);
    }
    mDecExponent = decExp + 1;
    mFirstDigitIndex = 0;
    mNDigits = ndigit;
    //
    // Last digit gets rounded based on stopping condition.
    //
    if (high) {
        if (low) {
            if (lowDigitDifference == 0LL) {
                // it's a tie!
                // choose based on which digits we like.
                if ((mDigits[mFirstDigitIndex + mNDigits - 1] & 1) != 0) {
                    Roundup();
                }
            }
            else if (lowDigitDifference > 0) {
                Roundup();
            }
        }
        else {
            Roundup();
        }
    }
}

void FloatingDecimal::BinaryToASCIIBuffer::Roundup()
{
    Integer i = (mFirstDigitIndex + mNDigits - 1);
    Integer q = mDigits[i];
    if (q == U'9') {
        while (q == U'9' && i > mFirstDigitIndex) {
            mDigits[i] = U'0';
            q = mDigits[--i];
        }
        if (q == U'9') {
            // carryout! High-order 1, rest 0s, larger exp.
            mDecExponent += 1;
            mDigits[mFirstDigitIndex] = U'1';
            return;
        }
    }
    mDigits[i] = (Char)(q + 1);
    mDecimalDigitsRoundedUp = true;
}

Integer FloatingDecimal::BinaryToASCIIBuffer::EstimateDecExp(
    /* [in] */ Long fractBits,
    /* [in] */ Integer binExp)
{
    Double d2 = Math::LongBitsToDouble(EXP_ONE | (fractBits & DoubleConsts::SIGNIF_BIT_MASK));
    Double d = (d2 - 1.5) * 0.289529654 + 0.176091259 + (Double)binExp * 0.301029995663981;
    Long dBits = Math::DoubleToRawLongBits(d);  //can't be NaN here so use raw
    Integer exponent = (Integer)((dBits & DoubleConsts::EXP_BIT_MASK) >> EXP_SHIFT) - DoubleConsts::EXP_BIAS;
    Boolean isNegative = (dBits & DoubleConsts::SIGN_BIT_MASK) != 0; // discover sign
    if (exponent >= 0 && exponent < 52) { // hot path
        Long mask = DoubleConsts::SIGNIF_BIT_MASK >> exponent;
        Integer r = (Integer)(((dBits & DoubleConsts::SIGNIF_BIT_MASK) | FRACT_HOB) >> (EXP_SHIFT - exponent));
        return isNegative ? (((mask & dBits) == 0LL) ? -r : -r - 1) : r;
    }
    else if (exponent < 0) {
        return (((dBits & ~DoubleConsts::SIGN_BIT_MASK) == 0) ? 0 :
                ((isNegative) ? -1 : 0));
    }
    else { //if (exponent >= 52)
        return (Integer)d;
    }
}

Integer FloatingDecimal::BinaryToASCIIBuffer::InsignificantDigits(
    /* [in] */ Integer insignificant)
{
    Integer i;
    for (i = 0; insignificant >= 10; i++) {
        insignificant /= 10;
    }
    return i;
}

Integer FloatingDecimal::BinaryToASCIIBuffer::InsignificantDigitsForPow2(
    /* [in] */ Integer p2)
{
    if (p2 > 1 && p2 < ArrayLength(sInsignificantDigitsNumber)) {
        return sInsignificantDigitsNumber[p2];
    }
    return 0;
}

Integer FloatingDecimal::BinaryToASCIIBuffer::GetChars(
    /* [out]*/ Array<Char>& result)
{
    CHECK(mNDigits <= 19);
    Integer i = 0;
    if (mIsNegative) {
        result[0] = U'-';
        i = 1;
    }
    if (mDecExponent > 0 && mDecExponent < 8) {
        Integer charLength = Math::Min(mNDigits, mDecExponent);
        result.Copy(i, mDigits, mFirstDigitIndex, charLength);
        i += charLength;
        if (charLength < mDecExponent) {
            charLength = mDecExponent - charLength;
            Arrays::Fill(result, i, i + charLength, U'0');
            i += charLength;
            result[i++] = U'.';
            result[i++] = U'0';
        }
        else {
            result[i++] = U'.';
            if (charLength < mNDigits) {
                Integer t = mNDigits - charLength;
                result.Copy(i, mDigits, mFirstDigitIndex + charLength, t);
                i += t;
            }
            else {
                result[i++] = U'0';
            }
        }
    }
    else if (mDecExponent <= 0 && mDecExponent > -3) {
        result[i++] = U'0';
        result[i++] = U'.';
        if (mDecExponent != 0) {
            Arrays::Fill(result, i, i - mDecExponent, U'0');
            i -= mDecExponent;
        }
        result.Copy(i, mDigits, mFirstDigitIndex, mNDigits);
        i += mNDigits;
    }
    else {
        result[i++] = mDigits[mFirstDigitIndex];
        result[i++] = '.';
        if (mNDigits > 1) {
            result.Copy(i, mDigits, mFirstDigitIndex + 1, mNDigits - 1);
            i += mNDigits - 1;
        }
        else {
            result[i++] = U'0';
        }
        result[i++] = U'E';
        Integer e;
        if (mDecExponent <= 0) {
            result[i++] = U'-';
            e = -mDecExponent + 1;
        }
        else {
            e = mDecExponent - 1;
        }
        // decExponent has 1, 2, or 3, digits
        if (e <= 9) {
            result[i++] = (Char) (e + U'0');
        }
        else if (e <= 99) {
            result[i++] = (Char) (e / 10 + U'0');
            result[i++] = (Char) (e % 10 + U'0');
        }
        else {
            result[i++] = (Char) (e / 100 + U'0');
            e %= 100;
            result[i++] = (Char) (e / 10 + U'0');
            result[i++] = (Char) (e % 10 + U'0');
        }
    }
    return i;
}

//-------------------------------------------------------------------------------------

FloatingDecimal::PreparedASCIIToBinaryBuffer::PreparedASCIIToBinaryBuffer(
    /* [in] */ Double doubleVal,
    /* [in] */ Float floatVal)
    : mDoubleVal(doubleVal)
    , mFloatVal(floatVal)
{}

COMO_INTERFACE_IMPL_1(FloatingDecimal::PreparedASCIIToBinaryBuffer, Object, IFloatingDecimalASCIIToBinaryConverter);

ECode FloatingDecimal::PreparedASCIIToBinaryBuffer::DoubleValue(
    /* [out] */ Double* value)
{
    VALIDATE_NOT_NULL(value);

    *value = mDoubleVal;
    return NOERROR;
}

ECode FloatingDecimal::PreparedASCIIToBinaryBuffer::FloatValue(
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);

    *value = mFloatVal;
    return NOERROR;
}

//-------------------------------------------------------------------------------------

constexpr Double FloatingDecimal::ASCIIToBinaryBuffer::SMALL_10_POW[];
constexpr Float FloatingDecimal::ASCIIToBinaryBuffer::SINGLE_SMALL_10_POW[];
constexpr Double FloatingDecimal::ASCIIToBinaryBuffer::BIG_10_POW[];
constexpr Double FloatingDecimal::ASCIIToBinaryBuffer::TINY_10_POW[];
const Integer FloatingDecimal::ASCIIToBinaryBuffer::MAX_SMALL_TEN = ArrayLength(SMALL_10_POW) - 1;
const Integer FloatingDecimal::ASCIIToBinaryBuffer::SINGLE_MAX_SMALL_TEN = ArrayLength(SINGLE_SMALL_10_POW) - 1;

FloatingDecimal::ASCIIToBinaryBuffer::ASCIIToBinaryBuffer(
    /* [in] */ Boolean negSign,
    /* [in] */ Integer decExponent,
    /* [in] */ Array<Char>& digits,
    /* [in] */ Integer n)
    : mIsNegative(negSign)
    , mDecExponent(decExponent)
    , mDigits(digits)
    , mNDigits(n)
{}

COMO_INTERFACE_IMPL_1(FloatingDecimal::ASCIIToBinaryBuffer, Object, IFloatingDecimalASCIIToBinaryConverter);

ECode FloatingDecimal::ASCIIToBinaryBuffer::DoubleValue(
    /* [out] */ Double* value)
{
    VALIDATE_NOT_NULL(value);

    Integer kDigits = Math::Min(mNDigits, MAX_DECIMAL_DIGITS + 1);
    //
    // convert the lead kDigits to a long integer.
    //
    // (special performance hack: start to do it using int)
    Integer iValue = (Integer)mDigits[0] - (Integer)U'0';
    Integer iDigits = Math::Min(kDigits, INT_DECIMAL_DIGITS);
    for (Integer i = 1; i < iDigits; i++) {
        iValue = iValue * 10 + (Integer)mDigits[i] - (Integer)U'0';
    }
    Long lValue = (Long)iValue;
    for (Integer i = iDigits; i < kDigits; i++) {
        lValue = lValue * 10LL + (Long)((Integer)mDigits[i] - (Integer)U'0');
    }
    Double dValue = (Double)lValue;
    Integer exp = mDecExponent - kDigits;
    //
    // lValue now contains a long integer with the value of
    // the first kDigits digits of the number.
    // dValue contains the (double) of the same.
    //

    if (mNDigits <= MAX_DECIMAL_DIGITS) {
        //
        // possibly an easy case.
        // We know that the digits can be represented
        // exactly. And if the exponent isn't too outrageous,
        // the whole thing can be done with one operation,
        // thus one rounding error.
        // Note that all our constructors trim all leading and
        // trailing zeros, so simple values (including zero)
        // will always end up here
        //
        if (exp == 0 || dValue == 0.0) {
            return (mIsNegative) ? -dValue : dValue; // small floating integer
        }
        else if (exp >= 0) {
            if (exp <= MAX_SMALL_TEN) {
                //
                // Can get the answer with one operation,
                // thus one roundoff.
                //
                Double rValue = dValue * SMALL_10_POW[exp];
                return (mIsNegative) ? -rValue : rValue;
            }
            Integer slop = MAX_DECIMAL_DIGITS - kDigits;
            if (exp <= MAX_SMALL_TEN + slop) {
                //
                // We can multiply dValue by 10^(slop)
                // and it is still "small" and exact.
                // Then we can multiply by 10^(exp-slop)
                // with one rounding.
                //
                dValue *= SMALL_10_POW[slop];
                Double rValue = dValue * SMALL_10_POW[exp - slop];
                return (mIsNegative) ? -rValue : rValue;
            }
            //
            // Else we have a hard case with a positive exp.
            //
        }
        else {
            if (exp >= -MAX_SMALL_TEN) {
                //
                // Can get the answer in one division.
                //
                Double rValue = dValue / SMALL_10_POW[-exp];
                return (mIsNegative) ? -rValue : rValue;
            }
            //
            // Else we have a hard case with a negative exp.
            //
        }
    }

    //
    // Harder cases:
    // The sum of digits plus exponent is greater than
    // what we think we can do with one error.
    //
    // Start by approximating the right answer by,
    // naively, scaling by powers of 10.
    //
    if (exp > 0) {
        if (mDecExponent > MAX_DECIMAL_EXPONENT + 1) {
            //
            // Lets face it. This is going to be
            // Infinity. Cut to the chase.
            //
            return (mIsNegative) ? IDouble::NEGATIVE_INFINITY : IDouble::POSITIVE_INFINITY;
        }
        if ((exp & 15) != 0) {
            dValue *= SMALL_10_POW[exp & 15];
        }
        if ((exp >>= 4) != 0) {
            Integer j;
            for (j = 0; exp > 1; j++, exp >>= 1) {
                if ((exp & 1) != 0) {
                    dValue *= BIG_10_POW[j];
                }
            }
            //
            // The reason for the weird exp > 1 condition
            // in the above loop was so that the last multiply
            // would get unrolled. We handle it here.
            // It could overflow.
            //
            Double t = dValue * BIG_10_POW[j];
            if (Math::IsInfinite(t)) {
                //
                // It did overflow.
                // Look more closely at the result.
                // If the exponent is just one too large,
                // then use the maximum finite as our estimate
                // value. Else call the result infinity
                // and punt it.
                // ( I presume this could happen because
                // rounding forces the result here to be
                // an ULP or two larger than
                // Double.MAX_VALUE ).
                //
                t = dValue / 2.0;
                t *= BIG_10_POW[j];
                if (Math::IsInfinite(t)) {
                    return (mIsNegative) ? IDouble::NEGATIVE_INFINITY : IDouble::POSITIVE_INFINITY;
                }
                t = IDouble::MAX_VALUE;
            }
            dValue = t;
        }
    }
    else if (exp < 0) {
        exp = -exp;
        if (mDecExponent < MIN_DECIMAL_EXPONENT - 1) {
            //
            // Lets face it. This is going to be
            // zero. Cut to the chase.
            //
            return (mIsNegative) ? -0.0 : 0.0;
        }
        if ((exp & 15) != 0) {
            dValue /= SMALL_10_POW[exp & 15];
        }
        if ((exp >>= 4) != 0) {
            Integer j;
            for (j = 0; exp > 1; j++, exp >>= 1) {
                if ((exp & 1) != 0) {
                    dValue *= TINY_10_POW[j];
                }
            }
            //
            // The reason for the weird exp > 1 condition
            // in the above loop was so that the last multiply
            // would get unrolled. We handle it here.
            // It could underflow.
            //
            Double t = dValue * TINY_10_POW[j];
            if (t == 0.0) {
                //
                // It did underflow.
                // Look more closely at the result.
                // If the exponent is just one too small,
                // then use the minimum finite as our estimate
                // value. Else call the result 0.0
                // and punt it.
                // ( I presume this could happen because
                // rounding forces the result here to be
                // an ULP or two less than
                // Double.MIN_VALUE ).
                //
                t = dValue * 2.0;
                t *= TINY_10_POW[j];
                if (t == 0.0) {
                    return (mIsNegative) ? -0.0 : 0.0;
                }
                t = IDouble::MIN_VALUE;
            }
            dValue = t;
        }
    }

    //
    // dValue is now approximately the result.
    // The hard part is adjusting it, by comparison
    // with FDBigInteger arithmetic.
    // Formulate the EXACT big-number result as
    // bigD0 * 10^exp
    //
    if (mNDigits > MAX_NDIGITS) {
        mNDigits = MAX_NDIGITS + 1;
        mDigits[MAX_NDIGITS] = U'1';
    }
    AutoPtr<IFDBigInteger> bigD0;
    CFDBigInteger::New(lValue, mDigits, kDigits, mNDigits, IID_IFDBigInteger, (IInterface**)&bigD0);
    exp = mDecExponent - mNDigits;

    Long ieeeBits = Math::DoubleToRawLongBits(dValue); // IEEE-754 bits of double candidate
    const Integer B5 = Math::Max(0, -exp); // powers of 5 in bigB, value is not modified inside correctionLoop
    const Integer D5 = Math::Max(0, exp); // powers of 5 in bigD, value is not modified inside correctionLoop
    AutoPtr<IFDBigInteger> tempVal;
    bigD0->MultByPow52(D5, 0, &tempVal);
    bigD0 = std::move(tempVal);
    bigD0->MakeImmutable();   // prevent bigD0 modification inside correctionLoop
    AutoPtr<IFDBigInteger> bigD;
    Integer prevD2 = 0;

    while (true) {
        // here ieeeBits can't be NaN, Infinity or zero
        Integer binexp = (Integer) (((unsigned Long)ieeeBits) >> EXP_SHIFT);
        Long bigBbits = ieeeBits & DoubleConsts::SIGNIF_BIT_MASK;
        if (binexp > 0) {
            bigBbits |= FRACT_HOB;
        }
        else { // Normalize denormalized numbers.
            CHECK(bigBbits != 0LL);
            Integer leadingZeros = Math::NumberOfLeadingZeros(bigBbits);
            Integer shift = leadingZeros - (63 - EXP_SHIFT);
            bigBbits <<= shift;
            binexp = 1 - shift;
        }
        binexp -= DoubleConsts::EXP_BIAS;
        Integer lowOrderZeros = Math::NumberOfTrailingZeros(bigBbits);
        bigBbits = (((unsigned Long)bigBbits) >> lowOrderZeros);
        const Integer bigIntExp = binexp - EXP_SHIFT + lowOrderZeros;
        const Integer bigIntNBits = EXP_SHIFT + 1 - lowOrderZeros;

        //
        // Scale bigD, bigB appropriately for
        // big-integer operations.
        // Naively, we multiply by powers of ten
        // and powers of two. What we actually do
        // is keep track of the powers of 5 and
        // powers of 2 we would use, then factor out
        // common divisors before doing the work.
        //
        Integer B2 = B5; // powers of 2 in bigB
        Integer D2 = D5; // powers of 2 in bigD
        Integer Ulp2;   // powers of 2 in halfUlp.
        if (bigIntExp >= 0) {
            B2 += bigIntExp;
        }
        else {
            D2 -= bigIntExp;
        }
        Ulp2 = B2;
        // shift bigB and bigD left by a number s. t.
        // halfUlp is still an integer.
        Integer hulpbias;
        if (binexp <= -DoubleConsts::EXP_BIAS) {
            // This is going to be a denormalized number
            // (if not actually zero).
            // half an ULP is at 2^-(DoubleConsts.EXP_BIAS+EXP_SHIFT+1)
            hulpbias = binexp + lowOrderZeros + DoubleConsts::EXP_BIAS;
        }
        else {
            hulpbias = 1 + lowOrderZeros;
        }
        B2 += hulpbias;
        D2 += hulpbias;
        // if there are common factors of 2, we might just as well
        // factor them out, as they add nothing useful.
        Integer common2 = Math::Min(B2, Math::Min(D2, Ulp2));
        B2 -= common2;
        D2 -= common2;
        Ulp2 -= common2;
        // do multiplications by powers of 5 and 2
        AutoPtr<IFDBigInteger> bigB = CFDBigInteger::ValueOfMulPow52(bigBbits, B5, B2);
        if (bigD == nullptr || prevD2 != D2) {
            bigD0->LeftShift(D2, &tempVal);
            bigD = std::move(tempVal);
            prevD2 = D2;
        }
        //
        // to recap:
        // bigB is the scaled-big-int version of our floating-point
        // candidate.
        // bigD is the scaled-big-int version of the exact value
        // as we understand it.
        // halfUlp is 1/2 an ulp of bigB, except for special cases
        // of exact powers of 2
        //
        // the plan is to compare bigB with bigD, and if the difference
        // is less than halfUlp, then we're satisfied. Otherwise,
        // use the ratio of difference to halfUlp to calculate a fudge
        // factor to add to the floating value, then go 'round again.
        //
        AutoPtr<IFDBigInteger> diff;
        Integer cmpResult;
        Boolean overvalue;
        if (bigB->Cmp(bigD, &cmpResult), cmpResult > 0) {
            overvalue = true; // our candidate is too big.
            bigB->LeftInplaceSub(bigD, &diff); // bigB is not user further - reuse
            if ((bigIntNBits == 1) && (bigIntExp > -DoubleConsts::EXP_BIAS + 1)) {
                // candidate is a normalized exact power of 2 and
                // is too big (larger than Double.MIN_NORMAL). We will be subtracting.
                // For our purposes, ulp is the ulp of the
                // next smaller range.
                Ulp2 -= 1;
                if (Ulp2 < 0) {
                    // rats. Cannot de-scale ulp this far.
                    // must scale diff in other direction.
                    Ulp2 = 0;
                    diff->LeftShift(1, &tempVal);
                    diff = std::move(tempVal);
                }
            }
        }
        else if (cmpResult < 0) {
            overvalue = false; // our candidate is too small.
            bigD->RightInplaceSub(bigB, &diff); // bigB is not user further - reuse
        }
        else {
            // the candidate is exactly right!
            // this happens with surprising frequency
            break;
        }
        diff->CmpPow52(B5, Ulp2, &cmpResult);
        if (cmpResult < 0) {
            // difference is small.
            // this is close enough
            break;
        }
        else if (cmpResult == 0) {
            // difference is exactly half an ULP
            // round to some other value maybe, then finish
            if ((ieeeBits & 1) != 0) { // half ties to even
                ieeeBits += overvalue ? -1 : 1; // nextDown or nextUp
            }
            break;
        }
        else {
            // difference is non-trivial.
            // could scale addend by ratio of difference to
            // halfUlp here, if we bothered to compute that difference.
            // Most of the time ( I hope ) it is about 1 anyway.
            ieeeBits += overvalue ? -1 : 1; // nextDown or nextUp
            if (ieeeBits == 0 || ieeeBits == DoubleConsts::EXP_BIT_MASK) { // 0.0 or Double.POSITIVE_INFINITY
                break; // oops. Fell off end of range.
            }
            continue; // try again.
        }
    }
    if (mIsNegative) {
        ieeeBits |= DoubleConsts::SIGN_BIT_MASK;
    }
    *value = Math::LongBitsToDouble(ieeeBits);
    return NOERROR;
}

ECode FloatingDecimal::ASCIIToBinaryBuffer::FloatValue(
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);

    Integer kDigits = Math::Min(mNDigits, SINGLE_MAX_DECIMAL_DIGITS + 1);
    //
    // convert the lead kDigits to an integer.
    //
    Integer iValue = (Integer)mDigits[0] - (Integer)U'0';
    for (Integer i = 1; i < kDigits; i++) {
        iValue = iValue * 10 + (Integer)mDigits[i] - (Integer)U'0';
    }
    Float fValue = (Float)iValue;
    Integer exp = mDecExponent - kDigits;
    //
    // iValue now contains an integer with the value of
    // the first kDigits digits of the number.
    // fValue contains the (float) of the same.
    //

    if (mNDigits <= SINGLE_MAX_DECIMAL_DIGITS) {
        //
        // possibly an easy case.
        // We know that the digits can be represented
        // exactly. And if the exponent isn't too outrageous,
        // the whole thing can be done with one operation,
        // thus one rounding error.
        // Note that all our constructors trim all leading and
        // trailing zeros, so simple values (including zero)
        // will always end up here.
        //
        if (exp == 0 || fValue == 0.0f) {
            return (mIsNegative) ? -fValue : fValue; // small floating integer
        }
        else if (exp >= 0) {
            if (exp <= SINGLE_MAX_SMALL_TEN) {
                //
                // Can get the answer with one operation,
                // thus one roundoff.
                //
                fValue *= SINGLE_SMALL_10_POW[exp];
                return (mIsNegative) ? -fValue : fValue;
            }
            Integer slop = SINGLE_MAX_DECIMAL_DIGITS - kDigits;
            if (exp <= SINGLE_MAX_SMALL_TEN + slop) {
                //
                // We can multiply fValue by 10^(slop)
                // and it is still "small" and exact.
                // Then we can multiply by 10^(exp-slop)
                // with one rounding.
                //
                fValue *= SINGLE_SMALL_10_POW[slop];
                fValue *= SINGLE_SMALL_10_POW[exp - slop];
                return (mIsNegative) ? -fValue : fValue;
            }
            //
            // Else we have a hard case with a positive exp.
            //
        }
        else {
            if (exp >= -SINGLE_MAX_SMALL_TEN) {
                //
                // Can get the answer in one division.
                //
                fValue /= SINGLE_SMALL_10_POW[-exp];
                return (mIsNegative) ? -fValue : fValue;
            }
            //
            // Else we have a hard case with a negative exp.
            //
        }
    }
    else if ((mDecExponent >= mNDigits) && (mNDigits + mDecExponent <= MAX_DECIMAL_DIGITS)) {
        //
        // In double-precision, this is an exact floating integer.
        // So we can compute to double, then shorten to float
        // with one round, and get the right answer.
        //
        // First, finish accumulating digits.
        // Then convert that integer to a double, multiply
        // by the appropriate power of ten, and convert to float.
        //
        Long lValue = (Long)iValue;
        for (Integer i = kDigits; i < mNDigits; i++) {
            lValue = lValue * 10LL + (Long)((Integer)mDigits[i] - (Integer)U'0');
        }
        Double dValue = (Double)lValue;
        exp = mDecExponent - mNDigits;
        dValue *= SMALL_10_POW[exp];
        fValue = (Float)dValue;
        return (mIsNegative) ? -fValue : fValue;
    }
    //
    // Harder cases:
    // The sum of digits plus exponent is greater than
    // what we think we can do with one error.
    //
    // Start by approximating the right answer by,
    // naively, scaling by powers of 10.
    // Scaling uses doubles to avoid overflow/underflow.
    //
    Double dValue = fValue;
    if (exp > 0) {
        if (mDecExponent > SINGLE_MAX_DECIMAL_EXPONENT + 1) {
            //
            // Lets face it. This is going to be
            // Infinity. Cut to the chase.
            //
            return (mIsNegative) ? IFloat::NEGATIVE_INFINITY : IFloat::POSITIVE_INFINITY;
        }
        if ((exp & 15) != 0) {
            dValue *= SMALL_10_POW[exp & 15];
        }
        if ((exp >>= 4) != 0) {
            Integer j;
            for (j = 0; exp > 0; j++, exp >>= 1) {
                if ((exp & 1) != 0) {
                    dValue *= BIG_10_POW[j];
                }
            }
        }
    }
    else if (exp < 0) {
        exp = -exp;
        if (mDecExponent < SINGLE_MIN_DECIMAL_EXPONENT - 1) {
            //
            // Lets face it. This is going to be
            // zero. Cut to the chase.
            //
            return (mIsNegative) ? -0.0f : 0.0f;
        }
        if ((exp & 15) != 0) {
            dValue /= SMALL_10_POW[exp & 15];
        }
        if ((exp >>= 4) != 0) {
            Integer j;
            for (j = 0; exp > 0; j++, exp >>= 1) {
                if ((exp & 1) != 0) {
                    dValue *= TINY_10_POW[j];
                }
            }
        }
    }
    fValue = Math::Max(IFloat::MIN_VALUE, Math::Min(IFloat::MAX_VALUE, (Float)dValue));

    //
    // fValue is now approximately the result.
    // The hard part is adjusting it, by comparison
    // with FDBigInteger arithmetic.
    // Formulate the EXACT big-number result as
    // bigD0 * 10^exp
    //
    if (mNDigits > SINGLE_MAX_NDIGITS) {
        mNDigits = SINGLE_MAX_NDIGITS + 1;
        mDigits[SINGLE_MAX_NDIGITS] = U'1';
    }
    AutoPtr<IFDBigInteger> bigD0;
    CFDBigInteger::New(iValue, mDigits, kDigits, mNDigits, IID_IFDBigInteger, (IInterface**)&bigD0);
    exp = mDecExponent - mNDigits;

    Integer ieeeBits = Math::FloatToRawIntegerBits(fValue); // IEEE-754 bits of float candidate
    const Integer B5 = Math::Max(0, -exp); // powers of 5 in bigB, value is not modified inside correctionLoop
    const Integer D5 = Math::Max(0, exp); // powers of 5 in bigD, value is not modified inside correctionLoop
    AutoPtr<IFDBigInteger> tempVal;
    bigD0->MultByPow52(D5, 0, &tempVal);
    bigD0 = std::move(tempVal);
    bigD0->MakeImmutable();   // prevent bigD0 modification inside correctionLoop
    AutoPtr<IFDBigInteger> bigD;
    Integer prevD2 = 0;

    while (true) {
        // here ieeeBits can't be NaN, Infinity or zero
        Integer binexp = ((unsigned Integer)ieeeBits) >> SINGLE_EXP_SHIFT;
        Integer bigBbits = ieeeBits & FloatConsts::SIGNIF_BIT_MASK;
        if (binexp > 0) {
            bigBbits |= SINGLE_FRACT_HOB;
        }
        else { // Normalize denormalized numbers.
            CHECK(bigBbits != 0)
            Integer leadingZeros = Math::NumberOfLeadingZeros(bigBbits);
            Integer shift = leadingZeros - (31 - SINGLE_EXP_SHIFT);
            bigBbits <<= shift;
            binexp = 1 - shift;
        }
        binexp -= FloatConsts::EXP_BIAS;
        Integer lowOrderZeros = Math::NumberOfTrailingZeros(bigBbits);
        bigBbits = ((unsigned Integer)bigBbits) >> lowOrderZeros;
        const Integer bigIntExp = binexp - SINGLE_EXP_SHIFT + lowOrderZeros;
        const Integer bigIntNBits = SINGLE_EXP_SHIFT + 1 - lowOrderZeros;

        //
        // Scale bigD, bigB appropriately for
        // big-integer operations.
        // Naively, we multiply by powers of ten
        // and powers of two. What we actually do
        // is keep track of the powers of 5 and
        // powers of 2 we would use, then factor out
        // common divisors before doing the work.
        //
        Integer B2 = B5; // powers of 2 in bigB
        Integer D2 = D5; // powers of 2 in bigD
        Integer Ulp2;   // powers of 2 in halfUlp.
        if (bigIntExp >= 0) {
            B2 += bigIntExp;
        }
        else {
            D2 -= bigIntExp;
        }
        Ulp2 = B2;
        // shift bigB and bigD left by a number s. t.
        // halfUlp is still an integer.
        Integer hulpbias;
        if (binexp <= -FloatConsts::EXP_BIAS) {
            // This is going to be a denormalized number
            // (if not actually zero).
            // half an ULP is at 2^-(FloatConsts.EXP_BIAS+SINGLE_EXP_SHIFT+1)
            hulpbias = binexp + lowOrderZeros + FloatConsts::EXP_BIAS;
        }
        else {
            hulpbias = 1 + lowOrderZeros;
        }
        B2 += hulpbias;
        D2 += hulpbias;
        // if there are common factors of 2, we might just as well
        // factor them out, as they add nothing useful.
        Integer common2 = Math::Min(B2, Math::Min(D2, Ulp2));
        B2 -= common2;
        D2 -= common2;
        Ulp2 -= common2;
        // do multiplications by powers of 5 and 2
        AutoPtr<IFDBigInteger> bigB = FDBigInteger::ValueOfMulPow52(bigBbits, B5, B2);
        if (bigD == nullptr || prevD2 != D2) {
            bigD0->LeftShift(D2, &tempVal);
            bigD = std::move(tempVal);
            prevD2 = D2;
        }
        //
        // to recap:
        // bigB is the scaled-big-int version of our floating-point
        // candidate.
        // bigD is the scaled-big-int version of the exact value
        // as we understand it.
        // halfUlp is 1/2 an ulp of bigB, except for special cases
        // of exact powers of 2
        //
        // the plan is to compare bigB with bigD, and if the difference
        // is less than halfUlp, then we're satisfied. Otherwise,
        // use the ratio of difference to halfUlp to calculate a fudge
        // factor to add to the floating value, then go 'round again.
        //
        AutoPtr<IFDBigInteger> diff;
        Integer cmpResult;
        Boolean overvalue;
        if (bigB->Cmp(bigD, &cmpResult), cmpResult > 0) {
            overvalue = true; // our candidate is too big.
            bigB->LeftInplaceSub(bigD, &diff); // bigB is not user further - reuse
            if ((bigIntNBits == 1) && (bigIntExp > -FloatConsts::EXP_BIAS + 1)) {
                // candidate is a normalized exact power of 2 and
                // is too big (larger than Float.MIN_NORMAL). We will be subtracting.
                // For our purposes, ulp is the ulp of the
                // next smaller range.
                Ulp2 -= 1;
                if (Ulp2 < 0) {
                    // rats. Cannot de-scale ulp this far.
                    // must scale diff in other direction.
                    Ulp2 = 0;
                    diff->LeftShift(1, &tempVal);
                    diff = std::move(tempVal);
                }
            }
        }
        else if (cmpResult < 0) {
            overvalue = false; // our candidate is too small.
            bigD->RightInplaceSub(bigB, &diff); // bigB is not user further - reuse
        }
        else {
            // the candidate is exactly right!
            // this happens with surprising frequency
            break;
        }
        diff->CmpPow52(B5, Ulp2, &cmpResult);
        if ((cmpResult) < 0) {
            // difference is small.
            // this is close enough
            break;
        }
        else if (cmpResult == 0) {
            // difference is exactly half an ULP
            // round to some other value maybe, then finish
            if ((ieeeBits & 1) != 0) { // half ties to even
                ieeeBits += overvalue ? -1 : 1; // nextDown or nextUp
            }
            break;
        }
        else {
            // difference is non-trivial.
            // could scale addend by ratio of difference to
            // halfUlp here, if we bothered to compute that difference.
            // Most of the time ( I hope ) it is about 1 anyway.
            ieeeBits += overvalue ? -1 : 1; // nextDown or nextUp
            if (ieeeBits == 0 || ieeeBits == FloatConsts::EXP_BIT_MASK) { // 0.0 or Float.POSITIVE_INFINITY
                break; // oops. Fell off end of range.
            }
            continue; // try again.
        }
    }
    if (mIsNegative) {
        ieeeBits |= FloatConsts::SIGN_BIT_MASK;
    }
    *value = Math::IntegerBitsToFloat(ieeeBits);
    return NOERROR;
}

//-------------------------------------------------------------------------------------

const String FloatingDecimal::INFINITY_REP("Infinity");
const Integer FloatingDecimal::INFINITY_LENGTH = FloatingDecimal::INFINITY_REP.GetLength();
const String FloatingDecimal::NAN_REP("NaN");
const Integer FloatingDecimal::NAN_LENGTH = FloatingDecimal::NAN_REP.GetLength();

AutoPtr<IFloatingDecimalBinaryToASCIIConverter> FloatingDecimal::Get_B2AC_POSITIVE_INFINITY()
{
    static const AutoPtr<IFloatingDecimalBinaryToASCIIConverter> B2AC_POSITIVE_INFINITY =
            new ExceptionalBinaryToASCIIBuffer(INFINITY_REP, false);
    return B2AC_POSITIVE_INFINITY;
}

AutoPtr<IFloatingDecimalBinaryToASCIIConverter> FloatingDecimal::Get_B2AC_NEGATIVE_INFINITY()
{
    static const AutoPtr<IFloatingDecimalBinaryToASCIIConverter> B2AC_NEGATIVE_INFINITY =
            new ExceptionalBinaryToASCIIBuffer(String("-") + INFINITY_REP, true);
    return B2AC_NEGATIVE_INFINITY;
}

AutoPtr<IFloatingDecimalBinaryToASCIIConverter> FloatingDecimal::Get_B2AC_NOT_A_NUMBER()
{
    static const AutoPtr<IFloatingDecimalBinaryToASCIIConverter> B2AC_NOT_A_NUMBER =
            new ExceptionalBinaryToASCIIBuffer(NAN_REP, false);
    return B2AC_NOT_A_NUMBER;
}

AutoPtr<IFloatingDecimalBinaryToASCIIConverter> FloatingDecimal::Get_B2AC_POSITIVE_ZERO()
{
    static Array<Char> digits{ U'0' };
    static const AutoPtr<IFloatingDecimalBinaryToASCIIConverter> B2AC_POSITIVE_ZERO =
            new BinaryToASCIIBuffer(false, digits);
    return B2AC_POSITIVE_ZERO;
}

AutoPtr<IFloatingDecimalBinaryToASCIIConverter> FloatingDecimal::Get_B2AC_NEGATIVE_ZERO()
{
    static Array<Char> digits{ U'0' };
    static const AutoPtr<IFloatingDecimalBinaryToASCIIConverter> B2AC_NEGATIVE_ZERO =
            new BinaryToASCIIBuffer(true, digits);
    return B2AC_NEGATIVE_ZERO;
}

AutoPtr<IFloatingDecimalASCIIToBinaryConverter> FloatingDecimal::Get_A2BC_POSITIVE_INFINITY()
{
    static AutoPtr<IFloatingDecimalASCIIToBinaryConverter> A2BC_POSITIVE_INFINITY =
            new PreparedASCIIToBinaryBuffer(IDouble::POSITIVE_INFINITY, IFloat::POSITIVE_INFINITY);
    return A2BC_POSITIVE_INFINITY;
}

AutoPtr<IFloatingDecimalASCIIToBinaryConverter> FloatingDecimal::Get_A2BC_NEGATIVE_INFINITY()
{
    static AutoPtr<IFloatingDecimalASCIIToBinaryConverter> A2BC_NEGATIVE_INFINITY =
            new PreparedASCIIToBinaryBuffer(IDouble::NEGATIVE_INFINITY, IFloat::NEGATIVE_INFINITY);
    return A2BC_NEGATIVE_INFINITY;
}

AutoPtr<IFloatingDecimalASCIIToBinaryConverter> FloatingDecimal::Get_A2BC_NOT_A_NUMBER()
{
    static AutoPtr<IFloatingDecimalASCIIToBinaryConverter> A2BC_NOT_A_NUMBER =
            new PreparedASCIIToBinaryBuffer(IDouble::NaN, IFloat::NaN);
    return A2BC_NOT_A_NUMBER;
}

AutoPtr<IFloatingDecimalASCIIToBinaryConverter> FloatingDecimal::Get_A2BC_POSITIVE_ZERO()
{
    static AutoPtr<IFloatingDecimalASCIIToBinaryConverter> A2BC_POSITIVE_ZERO =
            new PreparedASCIIToBinaryBuffer(0.0, 0.0f);
    return A2BC_POSITIVE_ZERO;
}

AutoPtr<IFloatingDecimalASCIIToBinaryConverter> FloatingDecimal::Get_A2BC_NEGATIVE_ZERO()
{
    static AutoPtr<IFloatingDecimalASCIIToBinaryConverter> A2BC_NEGATIVE_ZERO =
            new PreparedASCIIToBinaryBuffer(-0.0, -0.0f);
    return A2BC_NEGATIVE_ZERO;
}

static AutoPtr<IPattern> CreatePattern(
    /* [in] */ const String& patternStr)
{
    AutoPtr<IPattern> pattern;
    Pattern::Compile(patternStr, &pattern);
    return pattern;
}

AutoPtr<IPattern> FloatingDecimal::GET_HEX_FLOAT_PATTERN()
{
    static const AutoPtr<IPattern> HEX_FLOAT_PATTERN = CreatePattern(
                  //1           234                   56                7                   8      9
            String("([-+])?0[xX](((\\p{XDigit}+)\\.?)|((\\p{XDigit}*)\\.(\\p{XDigit}+)))[pP]([-+])?(\\p{Digit}+)[fFdD]?"));
    return HEX_FLOAT_PATTERN;
}

String FloatingDecimal::ToFormatString(
    /* [in]*/ Double d)
{
    String formatStr;
    GetBinaryToASCIIConverter(d)->ToFormatString(&formatStr);
    return formatStr;
}

String FloatingDecimal::ToFormatString(
    /* [in] */ Float f)
{
    String formatStr;
    GetBinaryToASCIIConverter(f)->ToFormatString(&formatStr);
    return formatStr;
}

ECode FloatingDecimal::AppendTo(
    /* [in] */ Double d,
    /* [in] */ IAppendable* buf)
{
    return GetBinaryToASCIIConverter(d)->AppendTo(buf);
}

ECode FloatingDecimal::AppendTo(
    /* [in] */ Float f,
    /* [in] */ IAppendable* buf)
{
    return GetBinaryToASCIIConverter(f)->AppendTo(buf);
}

ECode FloatingDecimal::ParseDouble(
    /* [in] */ const String& s,
    /* [out] */ Double* value)
{
    VALIDATE_NOT_NULL(value);

    AutoPtr<IFloatingDecimalASCIIToBinaryConverter> converter;
    FAIL_RETURN(ReadFormatString(s, &converter));
    return converter->DoubleValue(value);
}

ECode FloatingDecimal::ParseFloat(
    /* [in] */ const String& s,
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);

    AutoPtr<IFloatingDecimalASCIIToBinaryConverter> converter;
    FAIL_RETURN(ReadFormatString(s, &converter));
    return converter->FloatValue(value);
}

AutoPtr<IThreadLocal> FloatingDecimal::GetThreadLocalBinaryToASCIIBuffer()
{
    class _ThreadLocal
        : public ThreadLocal
    {
    protected:
        AutoPtr<IInterface> InitialValue() override
        {
            AutoPtr<IFloatingDecimalBinaryToASCIIConverter> buffer =
                    new BinaryToASCIIBuffer();
            return buffer.Get();
        }
    };

    static AutoPtr<IThreadLocal> sThreadLocalBinaryToASCIIBuffer =
            new _ThreadLocal();
    return sThreadLocalBinaryToASCIIBuffer;
}

AutoPtr<FloatingDecimal::BinaryToASCIIBuffer> FloatingDecimal::GetBinaryToASCIIBuffer()
{
    AutoPtr<IFloatingDecimalBinaryToASCIIConverter> buffer;
    GetThreadLocalBinaryToASCIIBuffer()->Get((IInterface**)&buffer);
    return (BinaryToASCIIBuffer*)buffer.Get();
}

AutoPtr<IFloatingDecimalBinaryToASCIIConverter> FloatingDecimal::GetBinaryToASCIIConverter(
    /* [in] */ Double d,
    /* [in] */ Boolean isCompatibleFormat)
{
    Long dBits = Math::DoubleToRawLongBits(d);
    Boolean isNegative = (dBits & DoubleConsts::SIGN_BIT_MASK) != 0;
    Long fractBits = dBits & DoubleConsts::SIGNIF_BIT_MASK;
    Integer binExp = (Integer)((dBits & DoubleConsts::EXP_BIT_MASK) >> EXP_SHIFT);
    // Discover obvious special cases of NaN and Infinity.
    if (binExp == (Integer)(DoubleConsts::EXP_BIT_MASK >> EXP_SHIFT)) {
        if (fractBits == 0LL) {
            return isNegative ? Get_B2AC_NEGATIVE_INFINITY() : Get_B2AC_POSITIVE_INFINITY();
        }
        else {
            return Get_B2AC_NOT_A_NUMBER();
        }
    }
    // Finish unpacking
    // Normalize denormalized numbers.
    // Insert assumed high-order bit for normalized numbers.
    // Subtract exponent bias.
    Integer nSignificantBits;
    if (binExp == 0) {
        if (fractBits == 0LL) {
            // not a denorm, just a 0!
            return isNegative ? Get_B2AC_NEGATIVE_ZERO() : Get_B2AC_POSITIVE_ZERO();
        }
        Integer leadingZeros = Math::NumberOfLeadingZeros(fractBits);
        Integer shift = leadingZeros - (63 - EXP_SHIFT);
        fractBits <<= shift;
        binExp = 1 - shift;
        nSignificantBits = 64 - leadingZeros; // recall binExp is  - shift count.
    }
    else {
        fractBits |= FRACT_HOB;
        nSignificantBits = EXP_SHIFT + 1;
    }
    binExp -= DoubleConsts::EXP_BIAS;
    AutoPtr<BinaryToASCIIBuffer> buf = GetBinaryToASCIIBuffer();
    buf->SetSign(isNegative);
    // call the routine that actually does all the hard work.
    buf->Dtoa(binExp, fractBits, nSignificantBits, isCompatibleFormat);
    return buf.Get();
}

AutoPtr<IFloatingDecimalBinaryToASCIIConverter> FloatingDecimal::GetBinaryToASCIIConverter(
    /* [in] */ Float f)
{
    Integer fBits = Math::FloatToRawIntegerBits(f);
    Boolean isNegative = (fBits & FloatConsts::SIGN_BIT_MASK) != 0;
    Integer fractBits = fBits & FloatConsts::SIGNIF_BIT_MASK;
    Integer binExp = (fBits & FloatConsts::EXP_BIT_MASK) >> SINGLE_EXP_SHIFT;
    // Discover obvious special cases of NaN and Infinity.
    if (binExp == (FloatConsts::EXP_BIT_MASK >> SINGLE_EXP_SHIFT)) {
        if (fractBits == 0) {
            return isNegative ? Get_B2AC_NEGATIVE_INFINITY() : Get_B2AC_POSITIVE_INFINITY();
        }
        else {
            return Get_B2AC_NOT_A_NUMBER();
        }
    }
    // Finish unpacking
    // Normalize denormalized numbers.
    // Insert assumed high-order bit for normalized numbers.
    // Subtract exponent bias.
    Integer nSignificantBits;
    if (binExp == 0) {
        if (fractBits == 0) {
            // not a denorm, just a 0!
            return isNegative ? Get_B2AC_NEGATIVE_ZERO() : Get_B2AC_POSITIVE_ZERO();
        }
        Integer leadingZeros = Math::NumberOfLeadingZeros(fractBits);
        Integer shift = leadingZeros - (31 - SINGLE_EXP_SHIFT);
        fractBits <<= shift;
        binExp = 1 - shift;
        nSignificantBits =  32 - leadingZeros; // recall binExp is  - shift count.
    }
    else {
        fractBits |= SINGLE_FRACT_HOB;
        nSignificantBits = SINGLE_EXP_SHIFT + 1;
    }
    binExp -= FloatConsts::EXP_BIAS;
    AutoPtr<BinaryToASCIIBuffer> buf = GetBinaryToASCIIBuffer();
    buf->SetSign(isNegative);
    // call the routine that actually does all the hard work.
    buf->Dtoa(binExp, ((Long)fractBits) << (EXP_SHIFT - SINGLE_EXP_SHIFT), nSignificantBits, true);
    return buf.Get();
}

ECode FloatingDecimal::ReadFormatString(
    /* [in] */ const String& _in,
    /* [out] */ IFloatingDecimalASCIIToBinaryConverter** converter)
{
    Boolean isNegative = false;
    Boolean signSeen = false;
    Integer decExp;
    Char c;
    String in = _in;

    in = in.Trim();
    Integer len = in.GetLength();
    if (len == 0) {
        Logger::E("FloatingDecimal", "empty String");
        return E_NUMBER_FORMAT_EXCEPTION;
    }
    Integer i = 0;
    switch (in.GetChar(i)) {
        case U'-':
            isNegative = true;
            // fall through
        case U'+':
            i++;
            signSeen = true;
    }
    c = in.GetChar(i);
    if (c == U'N') {
        // Check for NaN
        if ((len - i) == NAN_LENGTH && in.IndexOf(NAN_REP, i) == i) {
            AutoPtr<IFloatingDecimalASCIIToBinaryConverter> conv =
                    Get_A2BC_NOT_A_NUMBER();
            conv.MoveTo(converter);
            return NOERROR;
        }
        Logger::E("FloatingDecimal", "For input string: \"%s\"", in.string());
        return E_NUMBER_FORMAT_EXCEPTION;
    }
    else if (c == U'I') {
        // Check for Infinity strings
        if ((len - i) == INFINITY_LENGTH && in.IndexOf(INFINITY_REP, i) == i) {
            AutoPtr<IFloatingDecimalASCIIToBinaryConverter> conv = isNegative ?
                    Get_A2BC_NEGATIVE_INFINITY() : Get_A2BC_POSITIVE_INFINITY();
            conv.MoveTo(converter);
            return NOERROR;
        }
        Logger::E("FloatingDecimal", "For input string: \"%s\"", in.string());
        return E_NUMBER_FORMAT_EXCEPTION;
    }
    else if (c == U'0') {
        // check for hexadecimal floating-point number
        if (len > i + 1) {
            Char ch = in.GetChar(i + 1);
            if (ch == U'x' || ch == U'X') {
                return ParseHexString(in, converter);
            }
        }
    }

    // look for and process decimal floating-point string
    Array<Char> digits(len);
    Integer nDigits = 0;
    Boolean decSeen = false;
    Integer decPt = 0;
    Integer nLeadZero = 0;
    Integer nTrailZero = 0;

    while (i < len) {
        c = in.GetChar(i);
        if (c == U'0') {
            nLeadZero++;
        }
        else if (c == U'.') {
            if (decSeen) {
                // already saw one ., this is the 2nd.
                Logger::E("FloatingDecimal", "multiple points");
                return E_NUMBER_FORMAT_EXCEPTION;
            }
            decPt = i;
            if (signSeen) {
                decPt -= 1;
            }
            decSeen = true;
        }
        else {
            break;
        }
        i++;
    }
    while(i < len) {
        c = in.GetChar(i);
        if (c >= U'1' && c <= U'9') {
            digits[nDigits++] = c;
            nTrailZero = 0;
        }
        else if (c == U'0') {
            digits[nDigits++] = c;
            nTrailZero++;
        }
        else if (c == U'.') {
            if (decSeen) {
                // already saw one ., this is the 2nd.
                Logger::E("FloatingDecimal", "multiple points");
                return E_NUMBER_FORMAT_EXCEPTION;
            }
            decPt = i;
            if (signSeen) {
                decPt -= 1;
            }
            decSeen = true;
        }
        else {
            break;
        }
        i++;
    }
    nDigits -= nTrailZero;
    //
    // At this point, we've scanned all the digits and decimal
    // point we're going to see. Trim off leading and trailing
    // zeros, which will just confuse us later, and adjust
    // our initial decimal exponent accordingly.
    // To review:
    // we have seen i total characters.
    // nLeadZero of them were zeros before any other digits.
    // nTrailZero of them were zeros after any other digits.
    // if ( decSeen ), then a . was seen after decPt characters
    // ( including leading zeros which have been discarded )
    // nDigits characters were neither lead nor trailing
    // zeros, nor point
    //
    //
    // special hack: if we saw no non-zero digits, then the
    // answer is zero!
    // Unfortunately, we feel honor-bound to keep parsing!
    //
    Boolean isZero = (nDigits == 0);
    if (isZero && nLeadZero == 0) {
        // we saw NO DIGITS AT ALL,
        // not even a crummy 0!
        // this is not allowed.
        Logger::E("FloatingDecimal", "For input string: \"%s\"", in.string());
        return E_NUMBER_FORMAT_EXCEPTION;
    }
    //
    // Our initial exponent is decPt, adjusted by the number of
    // discarded zeros. Or, if there was no decPt,
    // then its just nDigits adjusted by discarded trailing zeros.
    //
    if (decSeen) {
        decExp = decPt - nLeadZero;
    }
    else {
        decExp = nDigits + nTrailZero;
    }

    //
    // Look for 'e' or 'E' and an optionally signed integer.
    //
    if ((i < len) && (((c = in.GetChar(i)) == U'e') || (c == U'E'))) {
        Integer expSign = 1;
        Integer expVal = 0;
        Integer reallyBig = IInteger::MAX_VALUE / 10;
        Boolean expOverflow = false;
        switch (in.GetChar(++i)) {
            case U'-':
                expSign = -1;
                // fall through
            case U'+':
                i++;
        }
        Integer expAt = i;
        while (i < len) {
            if (expVal >= reallyBig) {
                // the next character will cause integer
                // overflow.
                expOverflow = true;
            }
            c = in.GetChar(i++);
            if (c >= U'0' && c <= U'9') {
                expVal = expVal * 10 + (c - U'0');
            }
            else {
                i--;
                break;
            }
        }
        Integer expLimit = BIG_DECIMAL_EXPONENT + nDigits + nTrailZero;
        if (expOverflow || (expVal > expLimit)) {
            //
            // The intent here is to end up with
            // infinity or zero, as appropriate.
            // The reason for yielding such a small decExponent,
            // rather than something intuitive such as
            // expSign*Integer.MAX_VALUE, is that this value
            // is subject to further manipulation in
            // doubleValue() and floatValue(), and I don't want
            // it to be able to cause overflow there!
            // (The only way we can get into trouble here is for
            // really outrageous nDigits+nTrailZero, such as 2 billion. )
            //
            decExp = expSign * expLimit;
        }
        else {
            // this should not overflow, since we tested
            // for expVal > (MAX+N), where N >= abs(decExp)
            decExp = decExp + expSign * expVal;
        }

        // if we saw something not a digit ( or end of string )
        // after the [Ee][+-], without seeing any digits at all
        // this is certainly an error. If we saw some digits,
        // but then some trailing garbage, that might be ok.
        // so we just fall through in that case.
        // HUMBUG
        if (i == expAt) {
            Logger::E("FloatingDecimal", "For input string: \"%s\"", in.string());
            return E_NUMBER_FORMAT_EXCEPTION;
        }
    }
    //
    // We parsed everything we could.
    // If there are leftovers, then this is not good input!
    //
    if (i < len && ((i != len - 1) ||
            (in.GetChar(i) != U'f' &&
             in.GetChar(i) != U'F' &&
             in.GetChar(i) != U'd' &&
             in.GetChar(i) != U'D'))) {
        Logger::E("FloatingDecimal", "For input string: \"%s\"", in.string());
        return E_NUMBER_FORMAT_EXCEPTION;
    }
    if (isZero) {
        AutoPtr<IFloatingDecimalASCIIToBinaryConverter> conv = isNegative ?
                Get_A2BC_NEGATIVE_ZERO() : Get_A2BC_POSITIVE_ZERO();
        conv.MoveTo(converter);
        return NOERROR;
    }
    *converter = new ASCIIToBinaryBuffer(isNegative, decExp, digits, nDigits);
    REFCOUNT_ADD(*converter);
    return NOERROR;
}

ECode FloatingDecimal::ParseHexString(
    /* [in] */ const String& in,
    /* [out] */ IFloatingDecimalASCIIToBinaryConverter** converter)
{
    // Verify string is a member of the hexadecimal floating-point
    // string language.
    AutoPtr<IMatcher> m;
    GET_HEX_FLOAT_PATTERN()->Matcher(CoreUtils::Box(in), &m);
    Boolean validInput;
    m->Matches(&validInput);
    if (!validInput) {
        Logger::E("FloatingDecimal", "For input string: \"%s\"", in.string());
        return E_NUMBER_FORMAT_EXCEPTION;
    }
    else {
        //
        // We must isolate the sign, significand, and exponent
        // fields.  The sign value is straightforward.  Since
        // floating-point numbers are stored with a normalized
        // representation, the significand and exponent are
        // interrelated.
        //
        // After extracting the sign, we normalized the
        // significand as a hexadecimal value, calculating an
        // exponent adjust for any shifts made during
        // normalization.  If the significand is zero, the
        // exponent doesn't need to be examined since the output
        // will be zero.
        //
        // Next the exponent in the input string is extracted.
        // Afterwards, the significand is normalized as a *binary*
        // value and the input value's normalized exponent can be
        // computed.  The significand bits are copied into a
        // double significand; if the string has more logical bits
        // than can fit in a double, the extra bits affect the
        // round and sticky bits which are used to round the final
        // value.
        //
        //  Extract significand sign
        String group1;
        m->Group(1, &group1);
        Boolean isNegative = ((!group1.IsNull()) && group1.Equals("-"));

        //  Extract Significand magnitude
        //
        // Based on the form of the significand, calculate how the
        // binary exponent needs to be adjusted to create a
        // normalized//hexadecimal* floating-point number; that
        // is, a number where there is one nonzero hex digit to
        // the left of the (hexa)decimal point.  Since we are
        // adjusting a binary, not hexadecimal exponent, the
        // exponent is adjusted by a multiple of 4.
        //
        // There are a number of significand scenarios to consider;
        // letters are used in indicate nonzero digits:
        //
        // 1. 000xxxx       =>      x.xxx   normalized
        //    increase exponent by (number of x's - 1)*4
        //
        // 2. 000xxx.yyyy =>        x.xxyyyy        normalized
        //    increase exponent by (number of x's - 1)*4
        //
        // 3. .000yyy  =>   y.yy    normalized
        //    decrease exponent by (number of zeros + 1)*4
        //
        // 4. 000.00000yyy => y.yy normalized
        //    decrease exponent by (number of zeros to right of point + 1)*4
        //
        // If the significand is exactly zero, return a properly
        // signed zero.
        //

        String significandString;
        Integer signifLength = 0;
        Integer exponentAdjust = 0;
        {
            Integer leftDigits = 0; // number of meaningful digits to
            // left of "decimal" point
            // (leading zeros stripped)
            Integer rightDigits = 0; // number of digits to right of
            // "decimal" point; leading zeros
            // must always be accounted for
            //
            // The significand is made up of either
            //
            // 1. group 4 entirely (integer portion only)
            //
            // OR
            //
            // 2. the fractional portion from group 7 plus any
            // (optional) integer portions from group 6.
            //
            String group4;
            m->Group(4, &group4);
            if (!group4.IsNull()) {
                // Integer-only significand
                // Leading zeros never matter on the integer portion
                significandString = StripLeadingZeros(group4);
                leftDigits = significandString.GetLength();
            }
            else {
                // Group 6 is the optional integer; leading zeros
                // never matter on the integer portion
                String group6;
                m->Group(6, &group6);
                group6 = StripLeadingZeros(group6);
                leftDigits = group6.GetLength();

                // fraction
                String group7;
                m->Group(7, &group7);
                rightDigits = group7.GetLength();

                // Turn "integer.fraction" into "integer"+"fraction"
                significandString = (group6.IsNull() ? String("") : group6) + group7;
            }

            significandString = StripLeadingZeros(significandString);
            signifLength = significandString.GetLength();

            //
            // Adjust exponent as described above
            //
            if (leftDigits >= 1) {  // Cases 1 and 2
                exponentAdjust = 4 * (leftDigits - 1);
            }
            else {                // Cases 3 and 4
                exponentAdjust = -4 * (rightDigits - signifLength + 1);
            }

            // If the significand is zero, the exponent doesn't
            // matter; return a properly signed zero.

            if (signifLength == 0) { // Only zeros in input
                AutoPtr<IFloatingDecimalASCIIToBinaryConverter> conv = isNegative ?
                        Get_A2BC_NEGATIVE_ZERO() : Get_A2BC_POSITIVE_ZERO();
                conv.MoveTo(converter);
                return NOERROR;
            }
        }

        //  Extract Exponent
        //
        // Use an int to read in the exponent value; this should
        // provide more than sufficient range for non-contrived
        // inputs.  If reading the exponent in as an int does
        // overflow, examine the sign of the exponent and
        // significand to determine what to do.
        //
        String group8;
        m->Group(8, &group8);
        Boolean positiveExponent = (group8.IsNull() || group8.Equals("+"));
        String group9;
        m->Group(9, &group9);
        Integer value;
        ECode ec = StringUtils::ParseInteger(group9, &value);
        if (FAILED(ec)) {
            // At this point, we know the exponent is
            // syntactically well-formed as a sequence of
            // digits.  Therefore, if an NumberFormatException
            // is thrown, it must be due to overflowing int's
            // range.  Also, at this point, we have already
            // checked for a zero significand.  Thus the signs
            // of the exponent and significand determine the
            // final result:
            //
            //                      significand
            //                      +               -
            // exponent     +       +infinity       -infinity
            //              -       +0.0            -0.0
            AutoPtr<IFloatingDecimalASCIIToBinaryConverter> conv = isNegative ?
                      (positiveExponent ? Get_A2BC_NEGATIVE_INFINITY() : Get_A2BC_NEGATIVE_ZERO())
                    : (positiveExponent ? Get_A2BC_POSITIVE_INFINITY() : Get_A2BC_POSITIVE_ZERO());
            conv.MoveTo(converter);
            return NOERROR;
        }
        Long unsignedRawExponent = value;

        Long rawExponent =
                (positiveExponent ? 1LL : -1LL) * // exponent sign
                        unsignedRawExponent;            // exponent magnitude

        // Calculate partially adjusted exponent
        Long exponent = rawExponent + exponentAdjust;

        // Starting copying non-zero bits into proper position in
        // a long; copy explicit bit too; this will be masked
        // later for normal values.

        Boolean round = false;
        Boolean sticky = false;
        Integer nextShift = 0;
        Long significand = 0;
        // First iteration is different, since we only copy
        // from the leading significand bit; one more exponent
        // adjust will be needed...

        // IMPORTANT: make leadingDigit a long to avoid
        // surprising shift semantics!
        FAIL_RETURN(GetHexDigit(significandString, 0, &value));
        Long leadingDigit = value;

        //
        // Left shift the leading digit (53 - (bit position of
        // leading 1 in digit)); this sets the top bit of the
        // significand to 1.  The nextShift value is adjusted
        // to take into account the number of bit positions of
        // the leadingDigit actually used.  Finally, the
        // exponent is adjusted to normalize the significand
        // as a binary value, not just a hex value.
        //
        if (leadingDigit == 1) {
            significand |= leadingDigit << 52;
            nextShift = 52 - 4;
            // exponent += 0
        }
        else if (leadingDigit <= 3) { // [2, 3]
            significand |= leadingDigit << 51;
            nextShift = 52 - 5;
            exponent += 1;
        }
        else if (leadingDigit <= 7) { // [4, 7]
            significand |= leadingDigit << 50;
            nextShift = 52 - 6;
            exponent += 2;
        }
        else if (leadingDigit <= 15) { // [8, f]
            significand |= leadingDigit << 49;
            nextShift = 52 - 7;
            exponent += 3;
        }
        else {
            Logger::E("FloatingDecimal", "Result from digit conversion too large!");
            return E_ASSERTION_ERROR;
        }
        // The preceding if-else could be replaced by a single
        // code block based on the high-order bit set in
        // leadingDigit.  Given leadingOnePosition,

        // significand |= leadingDigit << (SIGNIFICAND_WIDTH - leadingOnePosition);
        // nextShift = 52 - (3 + leadingOnePosition);
        // exponent += (leadingOnePosition-1);

        //
        // Now the exponent variable is equal to the normalized
        // binary exponent.  Code below will make representation
        // adjustments if the exponent is incremented after
        // rounding (includes overflows to infinity) or if the
        // result is subnormal.
        //

        // Copy digit into significand until the significand can't
        // hold another full hex digit or there are no more input
        // hex digits.
        Integer i = 0;
        for (i = 1; i < signifLength && nextShift >= 0; i++) {
            FAIL_RETURN(GetHexDigit(significandString, i, &value))
            Long currentDigit = value;
            significand |= (currentDigit << nextShift);
            nextShift -= 4;
        }

        // After the above loop, the bulk of the string is copied.
        // Now, we must copy any partial hex digits into the
        // significand AND compute the round bit and start computing
        // sticky bit.

        if (i < signifLength) { // at least one hex input digit exists
            FAIL_RETURN(GetHexDigit(significandString, i, &value));
            Long currentDigit = value;

            // from nextShift, figure out how many bits need
            // to be copied, if any
            switch (nextShift) { // must be negative
                case -1:
                    // three bits need to be copied in; can
                    // set round bit
                    significand |= ((currentDigit & 0xELL) >> 1);
                    round = (currentDigit & 0x1LL) != 0LL;
                    break;

                case -2:
                    // two bits need to be copied in; can
                    // set round and start sticky
                    significand |= ((currentDigit & 0xCLL) >> 2);
                    round = (currentDigit & 0x2LL) != 0LL;
                    sticky = (currentDigit & 0x1LL) != 0;
                    break;

                case -3:
                    // one bit needs to be copied in
                    significand |= ((currentDigit & 0x8LL) >> 3);
                    // Now set round and start sticky, if possible
                    round = (currentDigit & 0x4LL) != 0LL;
                    sticky = (currentDigit & 0x3LL) != 0;
                    break;

                case -4:
                    // all bits copied into significand; set
                    // round and start sticky
                    round = ((currentDigit & 0x8LL) != 0);  // is top bit set?
                    // nonzeros in three low order bits?
                    sticky = (currentDigit & 0x7LL) != 0;
                    break;

                default:
                    Logger::E("FloatingDecimal", "Unexpected shift distance remainder.");
                    return E_ASSERTION_ERROR;
            }

            // Round is set; sticky might be set.

            // For the sticky bit, it suffices to check the
            // current digit and test for any nonzero digits in
            // the remaining unprocessed input.
            i++;
            while (i < signifLength && !sticky) {
                FAIL_RETURN(GetHexDigit(significandString, i, &value));
                currentDigit = value;
                sticky = sticky || (currentDigit != 0);
                i++;
            }
        }
        // else all of string was seen, round and sticky are
        // correct as false.

        // Float calculations
        Integer floatBits = isNegative ? FloatConsts::SIGN_BIT_MASK : 0;
        if (exponent >= FloatConsts::MIN_EXPONENT) {
            if (exponent > FloatConsts::MAX_EXPONENT) {
                // Float.POSITIVE_INFINITY
                floatBits |= FloatConsts::EXP_BIT_MASK;
            }
            else {
                Integer threshShift = DoubleConsts::SIGNIFICAND_WIDTH - FloatConsts::SIGNIFICAND_WIDTH - 1;
                Boolean floatSticky = (significand & ((1LL << threshShift) - 1)) != 0 || round || sticky;
                Integer iValue = (Integer)(((unsigned Long)significand) >> threshShift);
                if ((iValue & 3) != 1 || floatSticky) {
                    iValue++;
                }
                floatBits |= (((((Integer)exponent) + (FloatConsts::EXP_BIAS - 1))) << SINGLE_EXP_SHIFT) + (iValue >> 1);
            }
        }
        else {
            if (exponent < FloatConsts::MIN_SUB_EXPONENT - 1) {
                // 0
            }
            else {
                // exponent == -127 ==> threshShift = 53 - 2 + (-149) - (-127) = 53 - 24
                Integer threshShift = (Integer)((DoubleConsts::SIGNIFICAND_WIDTH - 2 + FloatConsts::MIN_SUB_EXPONENT) - exponent);
                CHECK(threshShift >= DoubleConsts::SIGNIFICAND_WIDTH - FloatConsts::SIGNIFICAND_WIDTH);
                CHECK(threshShift < DoubleConsts::SIGNIFICAND_WIDTH);
                Boolean floatSticky = (significand & ((1LL << threshShift) - 1)) != 0 || round || sticky;
                Integer iValue = (Integer)(((unsigned Long)significand) >> threshShift);
                if ((iValue & 3) != 1 || floatSticky) {
                    iValue++;
                }
                floatBits |= iValue >> 1;
            }
        }
        Float fValue = Math::IntegerBitsToFloat(floatBits);

        // Check for overflow and update exponent accordingly.
        if (exponent > DoubleConsts::MAX_EXPONENT) {         // Infinite result
            // overflow to properly signed infinity
            AutoPtr<IFloatingDecimalASCIIToBinaryConverter> conv = isNegative ?
                    Get_A2BC_NEGATIVE_INFINITY() : Get_A2BC_POSITIVE_INFINITY();
            conv.MoveTo(converter);
            return NOERROR;
        }
        else {  // Finite return value
            if (exponent <= DoubleConsts::MAX_EXPONENT && // (Usually) normal result
                    exponent >= DoubleConsts::MIN_EXPONENT) {

                // The result returned in this block cannot be a
                // zero or subnormal; however after the
                // significand is adjusted from rounding, we could
                // still overflow in infinity.

                // AND exponent bits into significand; if the
                // significand is incremented and overflows from
                // rounding, this combination will update the
                // exponent correctly, even in the case of
                // Double.MAX_VALUE overflowing to infinity.

                significand = ((( exponent +
                        (Long)DoubleConsts::EXP_BIAS) <<
                        (DoubleConsts::SIGNIFICAND_WIDTH - 1))
                        & DoubleConsts::EXP_BIT_MASK) |
                        (DoubleConsts::SIGNIF_BIT_MASK & significand);

            }
            else {  // Subnormal or zero
                // (exponent < DoubleConsts.MIN_EXPONENT)

                if (exponent < (DoubleConsts::MIN_SUB_EXPONENT - 1)) {
                    // No way to round back to nonzero value
                    // regardless of significand if the exponent is
                    // less than -1075.
                    AutoPtr<IFloatingDecimalASCIIToBinaryConverter> conv = isNegative ?
                            Get_A2BC_NEGATIVE_ZERO() : Get_A2BC_POSITIVE_ZERO();
                    conv.MoveTo(converter);
                    return NOERROR;
                }
                else { //  -1075 <= exponent <= MIN_EXPONENT -1 = -1023
                    //
                    // Find bit position to round to; recompute
                    // round and sticky bits, and shift
                    // significand right appropriately.
                    //

                    sticky = sticky || round;
                    round = false;

                    // Number of bits of significand to preserve is
                    // exponent - abs_min_exp +1
                    // check:
                    // -1075 +1074 + 1 = 0
                    // -1023 +1074 + 1 = 52

                    Integer bitsDiscarded = 53 -
                            ((Integer)exponent - DoubleConsts::MIN_SUB_EXPONENT + 1);
                    CHECK(bitsDiscarded >= 1 && bitsDiscarded <= 53);

                    // What to do here:
                    // First, isolate the new round bit
                    round = (significand & (1LL << (bitsDiscarded - 1))) != 0LL;
                    if (bitsDiscarded > 1) {
                        // create mask to update sticky bits; low
                        // order bitsDiscarded bits should be 1
                        Long mask = ~((~0LL) << (bitsDiscarded - 1));
                        sticky = sticky || ((significand & mask) != 0LL);
                    }

                    // Now, discard the bits
                    significand = significand >> bitsDiscarded;

                    significand = ((((Long)(DoubleConsts::MIN_EXPONENT - 1) + // subnorm exp.
                            (Long)DoubleConsts::EXP_BIAS) <<
                            (DoubleConsts::SIGNIFICAND_WIDTH - 1))
                            & DoubleConsts::EXP_BIT_MASK) |
                            (DoubleConsts::SIGNIF_BIT_MASK & significand);
                }
            }

            // The significand variable now contains the currently
            // appropriate exponent bits too.

            //
            // Determine if significand should be incremented;
            // making this determination depends on the least
            // significant bit and the round and sticky bits.
            //
            // Round to nearest even rounding table, adapted from
            // table 4.7 in "Computer Arithmetic" by IsraelKoren.
            // The digit to the left of the "decimal" point is the
            // least significant bit, the digits to the right of
            // the point are the round and sticky bits
            //
            // Number       Round(x)
            // x0.00        x0.
            // x0.01        x0.
            // x0.10        x0.
            // x0.11        x1. = x0. +1
            // x1.00        x1.
            // x1.01        x1.
            // x1.10        x1. + 1
            // x1.11        x1. + 1
            //
            Boolean leastZero = ((significand & 1LL) == 0LL);
            if ((leastZero && round && sticky) ||
                    ((!leastZero) && round)) {
                significand++;
            }

            Double value = isNegative ?
                    Math::LongBitsToDouble(significand | DoubleConsts::SIGN_BIT_MASK) :
                    Math::LongBitsToDouble(significand );

            *converter = new PreparedASCIIToBinaryBuffer(value, fValue);
            REFCOUNT_ADD(*converter);
            return NOERROR;
        }
    }
}

String FloatingDecimal::StripLeadingZeros(
    /* [in] */ const String& s)
{
    if (!s.IsEmpty() && s.GetChar(0) == U'0') {
        for (Integer i = 1; i < s.GetLength(); i++) {
            if (s.GetChar(i) != U'0') {
                return s.Substring(i);
            }
        }
        return String("");
    }
    return s;
}

ECode FloatingDecimal::GetHexDigit(
    /* [in] */ const String& s,
    /* [in] */ Integer position,
    /* [out] */ Integer* value)
{
    *value = Character::Digit(s.GetChar(position), 16);
    if (*value <= -1 || *value >= 16) {
        Logger::E("FloatingDecimal", "Unexpected failure of digit conversion of %c",
                s.GetChar(position));
        return E_ASSERTION_ERROR;
    }
    return NOERROR;
}

}
}
