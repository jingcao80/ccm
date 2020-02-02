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
#include "como/core/CStringBuilder.h"
#include "como/core/Math.h"
#include "como/core/StringUtils.h"
#include "como/math/BigDecimal.h"
#include "como/math/BigInt.h"
#include "como/math/CBigDecimal.h"
#include "como/math/CBigInteger.h"
#include "como/math/CMathContext.h"
#include "como/math/Conversion.h"
#include "como/math/Multiplication.h"
#include "como/util/Arrays.h"
#include "como.core.IDouble.h"
#include "como.core.IFloat.h"
#include "como.core.IInteger.h"
#include "como.core.ILong.h"
#include "libcore/math/MathUtils.h"
#include <comolog.h>

using como::core::CStringBuilder;
using como::core::E_ARITHMETIC_EXCEPTION;
using como::core::E_NULL_POINTER_EXCEPTION;
using como::core::E_NUMBER_FORMAT_EXCEPTION;
using como::core::IDouble;
using como::core::IFloat;
using como::core::IInteger;
using como::core::IID_IComparable;
using como::core::IID_INumber;
using como::core::IID_IStringBuilder;
using como::core::ILong;
using como::core::IStringBuilder;
using como::core::Math;
using como::core::StringUtils;
using como::io::IID_ISerializable;
using como::util::Arrays;
using libcore::math::MathUtils;

namespace como {
namespace math {

const Array<IBigInteger*>& BigDecimal::GetFIVE_POW()
{
    static const Array<IBigInteger*> FIVE_POW = Multiplication::GetBigFivePows();
    return FIVE_POW;
}

const Array<IBigInteger*>& BigDecimal::GetTEN_POW()
{
    static const Array<IBigInteger*> TEN_POW = Multiplication::GetBigTenPows();
    return TEN_POW;
}

const Array<Long>& BigDecimal::GetLONG_FIVE_POW()
{
    static const Array<Long> LONG_FIVE_POW{
        1LL,
        5LL,
        25LL,
        125LL,
        625LL,
        3125LL,
        15625LL,
        78125LL,
        390625LL,
        1953125LL,
        9765625LL,
        48828125LL,
        244140625LL,
        1220703125LL,
        6103515625LL,
        30517578125LL,
        152587890625LL,
        762939453125LL,
        3814697265625LL,
        19073486328125LL,
        95367431640625LL,
        476837158203125LL,
        2384185791015625LL,
        11920928955078125LL,
        59604644775390625LL,
        298023223876953125LL,
        1490116119384765625LL,
        7450580596923828125LL,
    };
    return LONG_FIVE_POW;
}

typedef Integer (*BitLengthFunc)(Integer);

static Array<Integer> CreateLONG_FIVE_POW_BIT_LENGTH(
    /* [in] */ const Array<Long>& long_five_pow,
    /* [in] */ BitLengthFunc func)
{
    Array<Integer> long_five_pow_bit_length(long_five_pow.GetLength());
    for (Integer i = 0; i < long_five_pow_bit_length.GetLength(); ++i) {
        long_five_pow_bit_length[i] = func(long_five_pow[i]);
    }
    return long_five_pow_bit_length;
}

const Array<Integer>& BigDecimal::GetLONG_FIVE_POW_BIT_LENGTH()
{
    static const Array<Integer> LONG_FIVE_POW_BIT_LENGTH =
            CreateLONG_FIVE_POW_BIT_LENGTH(GetLONG_FIVE_POW(), &BitLength);
    return LONG_FIVE_POW_BIT_LENGTH;
}

static Array<Integer> CreateLONG_POWERS_OF_TEN_BIT_LENGTH(
    /* [in] */ BitLengthFunc func)
{
    Array<Integer> long_powers_of_ten_bit_length(MathUtils::LONG_POWERS_OF_TEN.GetLength());
    for (Integer i = 0; i < long_powers_of_ten_bit_length.GetLength(); ++i) {
        long_powers_of_ten_bit_length[i] = func(MathUtils::LONG_POWERS_OF_TEN[i]);
    }
    return long_powers_of_ten_bit_length;
}

const Array<Integer>& BigDecimal::GetLONG_POWERS_OF_TEN_BIT_LENGTH()
{
    static const Array<Integer> LONG_POWERS_OF_TEN_BIT_LENGTH =
            CreateLONG_POWERS_OF_TEN_BIT_LENGTH(&BitLength);
    return LONG_POWERS_OF_TEN_BIT_LENGTH;
}

static Array<IBigDecimal*> CreateBI_SCALED_BY_ZERO(
    /* [in] */ Integer bi_scaled_by_zero_length)
{
    Array<IBigDecimal*> bi_scaled_by_zero(bi_scaled_by_zero_length);
    for (Integer i = 0; i < bi_scaled_by_zero_length; ++i) {
        AutoPtr<IBigDecimal> tempBD;
        CBigDecimal::New(i, 0, IID_IBigDecimal, (IInterface**)&tempBD);
        bi_scaled_by_zero.Set(i, tempBD);
    }
    return bi_scaled_by_zero;
}

const Array<IBigDecimal*>& BigDecimal::GetBI_SCALED_BY_ZERO()
{
    static const Array<IBigDecimal*> BI_SCALED_BY_ZERO =
            CreateBI_SCALED_BY_ZERO(BI_SCALED_BY_ZERO_LENGTH);
    return BI_SCALED_BY_ZERO;
}

static Array<IBigDecimal*> CreateZERO_SCALED_BY()
{
    Array<IBigDecimal*> zero_scaled_by(11);
    for (Integer i = 0; i < zero_scaled_by.GetLength(); ++i) {
        AutoPtr<IBigDecimal> tempBD;
        CBigDecimal::New(0, i, IID_IBigDecimal, (IInterface**)&tempBD);
        zero_scaled_by.Set(i, tempBD);
    }
    return zero_scaled_by;
}

const Array<IBigDecimal*>& BigDecimal::GetZERO_SCALED_BY()
{
    static const Array<IBigDecimal*> ZERO_SCALED_BY = CreateZERO_SCALED_BY();
    return ZERO_SCALED_BY;
}

static Array<Char> CreateCH_ZEROS()
{
    Array<Char> ch_zeros(100);
    Arrays::Fill(ch_zeros, U'0');
    return ch_zeros;
}

const Array<Char>& BigDecimal::GetCH_ZEROS()
{
    static const Array<Char> CH_ZEROS = CreateCH_ZEROS();
    return CH_ZEROS;
}

AutoPtr<IBigDecimal> CreateBigDecimal(
    /* [in] */ Integer smallValue,
    /* [in] */ Integer scale)
{
    AutoPtr<IBigDecimal> value;
    CBigDecimal::New(smallValue, scale, IID_IBigDecimal, (IInterface**)&value);
    return value;
}

AutoPtr<IBigDecimal> BigDecimal::GetZERO()
{
    static const AutoPtr<IBigDecimal> ZERO = CreateBigDecimal(0, 0);
    return ZERO;
}

AutoPtr<IBigDecimal> BigDecimal::GetONE()
{
    static const AutoPtr<IBigDecimal> ONE = CreateBigDecimal(1, 0);
    return ONE;
}

AutoPtr<IBigDecimal> BigDecimal::GetTEN()
{
    static const AutoPtr<IBigDecimal> TEN = CreateBigDecimal(10, 0);
    return TEN;
}

COMO_INTERFACE_IMPL_4(BigDecimal, SyncObject, IBigDecimal, INumber, IComparable, ISerializable);

ECode BigDecimal::Constructor(
    /* [in] */ Long smallValue,
    /* [in] */ Integer scale)
{
    mSmallValue = smallValue;
    mScale = scale;
    mBitLength = BitLength(smallValue);
    return NOERROR;
}

ECode BigDecimal::Constructor(
    /* [in] */ Integer smallValue,
    /* [in] */ Integer scale)
{
    mSmallValue = smallValue;
    mScale = scale;
    mBitLength = BitLength(smallValue);
    return NOERROR;
}

ECode BigDecimal::Constructor(
    /* [in] */ const Array<Char>& in,
    /* [in] */ Integer offset,
    /* [in] */ Integer len)
{
    Integer begin = offset;
    Integer last = offset + (len - 1);
    String scaleString;
    AutoPtr<IStringBuilder> unscaledBuffer;
    Long newScale;

    if (in.IsNull()) {
        Logger::E("BigDecimal", "in == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    if ((last >= in.GetLength()) || (offset < 0) || (len <= 0) || (last < 0)) {
        Logger::E("BigDecimal", "Bad offset/length: offset=%d len=%d in.GetLength()=%lld",
                offset, len, in.GetLength());
        return E_NUMBER_FORMAT_EXCEPTION;
    }
    CStringBuilder::New(len, IID_IStringBuilder, (IInterface**)&unscaledBuffer);
    Integer bufLength = 0;
    // To skip a possible '+' symbol
    if ((offset <= last) && (in[offset] == U'+')) {
        offset++;
        begin++;
    }
    Integer counter = 0;
    Boolean wasNonZero = false;
    // Accumulating all digits until a possible decimal point
    for (; (offset <= last) && (in[offset] != U'.') && (in[offset] != U'e') && (in[offset] != U'E'); offset++) {
        if (!wasNonZero) {
            if (in[offset] == U'0') {
                counter++;
            }
            else {
                wasNonZero = true;
            }
        }
    }
    unscaledBuffer->Append(in, begin, offset - begin);
    bufLength += offset - begin;
    // A decimal point was found
    if ((offset <= last) && (in[offset] == U'.')) {
        offset++;
        // Accumulating all digits until a possible exponent
        begin = offset;
        for (; (offset <= last) && (in[offset] != U'e') && (in[offset] != U'E'); offset++) {
            if (!wasNonZero) {
                if (in[offset] == U'0') {
                    counter++;
                }
                else {
                    wasNonZero = true;
                }
            }
        }
        mScale = offset - begin;
        bufLength += mScale;
        unscaledBuffer->Append(in, begin, mScale);
    }
    else {
        mScale = 0;
    }
    // An exponent was found
    if ((offset <= last) && ((in[offset] == U'e') || (in[offset] == U'E'))) {
        offset++;
        // Checking for a possible sign of scale
        begin = offset;
        if ((offset <= last) && (in[offset] == U'+')) {
            offset++;
            if ((offset <= last) && (in[offset] != U'-')) {
                begin++;
            }
        }
        // Accumulating all remaining digits
        scaleString = String(in, begin, last + 1 - begin);
        // Checking if the scale is defined
        Integer scaleInt;
        StringUtils::ParseInteger(scaleString, &scaleInt);
        newScale = (Long)mScale - scaleInt;
        mScale = (Integer)newScale;
        if (newScale != mScale) {
            Logger::E("BigDecimal", "Scale out of range");
            return E_NUMBER_FORMAT_EXCEPTION;
        }
    }
    // Parsing the unscaled value
    if (bufLength < 19) {
        String str;
        unscaledBuffer->ToString(str);
        FAIL_RETURN(StringUtils::ParseLong(str, &mSmallValue));
        mBitLength = BitLength(mSmallValue);
    }
    else {
        String str;
        unscaledBuffer->ToString(str);
        AutoPtr<IBigInteger> bi;
        FAIL_RETURN(CBigInteger::New(str, IID_IBigInteger, (IInterface**)&bi));
        SetUnscaledValue(bi);
    }
    return NOERROR;
}

ECode BigDecimal::Constructor(
    /* [in] */ const Array<Char>& in,
    /* [in] */ Integer offset,
    /* [in] */ Integer len,
    /* [in] */ IMathContext* mc)
{
    FAIL_RETURN(Constructor(in, offset, len));
    return InplaceRound(mc);
}

ECode BigDecimal::Constructor(
    /* [in] */ const Array<Char>& in)
{
    return Constructor(in, 0, in.GetLength());
}

ECode BigDecimal::Constructor(
    /* [in] */ const Array<Char>& in,
    /* [in] */ IMathContext* mc)
{
    FAIL_RETURN(Constructor(in, 0, in.GetLength()));
    return InplaceRound(mc);
}

ECode BigDecimal::Constructor(
    /* [in] */ const String& value)
{
    return Constructor(value.GetChars(), 0, value.GetLength());
}

ECode BigDecimal::Constructor(
    /* [in] */ const String& value,
    /* [in] */ IMathContext* mc)
{
    FAIL_RETURN(Constructor(value.GetChars(), 0, value.GetLength()));
    return InplaceRound(mc);
}

ECode BigDecimal::Constructor(
    /* [in] */ Double value)
{
    if (Math::IsInfinite(value) || Math::IsNaN(value)) {
        Logger::E("BigDecimal", "IsInfinite or NaN: %f", value);
        return E_NUMBER_FORMAT_EXCEPTION;
    }
    Long bits = Math::DoubleToLongBits(value); // IEEE-754
    Long mantissa;
    Integer trailingZeros;
    // Extracting the exponent, note that the bias is 1023
    mScale = 1075 - (Integer)((bits >> 52) & 0x7FFLL);
    // Extracting the 52 bits of the mantissa.
    mantissa = (mScale == 1075) ? (bits & 0xFFFFFFFFFFFFFLL) << 1
            : (bits & 0xFFFFFFFFFFFFFLL) | 0x10000000000000LL;
    if (mantissa == 0) {
        mScale = 0;
        mPrecision = 1;
    }
    // To simplify all factors '2' in the mantissa
    if (mScale > 0) {
        trailingZeros = Math::Min(mScale, Math::NumberOfTrailingZeros(mantissa));
        mantissa = ((ULong)mantissa) >> trailingZeros;
        mScale -= trailingZeros;
    }
    // Calculating the new unscaled value and the new scale
    if ((bits >> 63) != 0) {
        mantissa = -mantissa;
    }
    Integer mantissaBits = BitLength(mantissa);
    if (mScale < 0) {
        mBitLength = mantissaBits == 0 ? 0 : mantissaBits - mScale;
        if (mBitLength < 64) {
            mSmallValue = mantissa << (-mScale);
        }
        else {
            AutoPtr<BigInt> bi = new BigInt();
            bi->PutLongInt(mantissa);
            bi->Shift(-mScale);
            CBigInteger::New(bi, IID_IBigInteger, (IInterface**)&mIntegerValue);
        }
        mScale = 0;
    }
    else if (mScale > 0) {
        if (mScale < GetLONG_FIVE_POW().GetLength() &&
                mantissaBits + GetLONG_FIVE_POW_BIT_LENGTH()[mScale] < 64) {
            mSmallValue = mantissa * GetLONG_FIVE_POW()[mScale];
            mBitLength = BitLength(mSmallValue);
        }
        else {
            AutoPtr<IBigInteger> bi;
            CBigInteger::ValueOf(mantissa, &bi);
            AutoPtr<IBigInteger> resBi = Multiplication::MultiplyByFivePow(
                    CBigInteger::From(bi), mScale);
            SetUnscaledValue(resBi);
        }
    }
    else {
        mSmallValue = mantissa;
        mBitLength = mantissaBits;
    }
    return NOERROR;
}

ECode BigDecimal::Constructor(
    /* [in] */ Double value,
    /* [in] */ IMathContext* mc)
{
    FAIL_RETURN(Constructor(value));
    return InplaceRound(mc);
}

ECode BigDecimal::Constructor(
    /* [in] */ IBigInteger* value)
{
    return Constructor(value, 0);
}

ECode BigDecimal::Constructor(
    /* [in] */ IBigInteger* value,
    /* [in] */ IMathContext* mc)
{
    FAIL_RETURN(Constructor(value));
    return InplaceRound(mc);
}

ECode BigDecimal::Constructor(
    /* [in] */ IBigInteger* unscaledValue,
    /* [in] */ Integer scale)
{
    if (unscaledValue == nullptr) {
        Logger::E("BigDecimal", "unscaledValue == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    mScale = scale;
    SetUnscaledValue(unscaledValue);
    return NOERROR;
}

ECode BigDecimal::Constructor(
    /* [in] */ IBigInteger* unscaledValue,
    /* [in] */ Integer scale,
    /* [in] */ IMathContext* mc)
{
    FAIL_RETURN(Constructor(unscaledValue, scale));
    return InplaceRound(mc);
}

ECode BigDecimal::Constructor(
    /* [in] */ Integer value)
{
    return Constructor(value, 0);
}

ECode BigDecimal::Constructor(
    /* [in] */ Integer value,
    /* [in] */ IMathContext* mc)
{
    FAIL_RETURN(Constructor(value, 0));
    return InplaceRound(mc);
}

ECode BigDecimal::Constructor(
    /* [in] */ Long value)
{
    return Constructor(value, 0);
}

ECode BigDecimal::Constructor(
    /* [in] */ Long value,
    /* [in] */ IMathContext* mc)
{
    FAIL_RETURN(Constructor(value));
    return InplaceRound(mc);
}

ECode BigDecimal::ValueOf(
    /* [in] */ Long unscaledValue,
    /* [in] */ Integer scale,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);

    if (scale == 0) {
        return ValueOf(unscaledValue, result);
    }
    if ((unscaledValue == 0) && (scale >= 0) &&
            (scale < GetZERO_SCALED_BY().GetLength())) {
        *result = GetZERO_SCALED_BY()[scale];
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    return CBigDecimal::New(unscaledValue, scale, IID_IBigDecimal, (IInterface**)result);
}

ECode BigDecimal::ValueOf(
    /* [in] */ Long unscaledValue,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);

    if ((unscaledValue >= 0) && (unscaledValue < BI_SCALED_BY_ZERO_LENGTH)) {
        *result = GetBI_SCALED_BY_ZERO()[(Integer)unscaledValue];
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    return CBigDecimal::New(unscaledValue, 0, IID_IBigDecimal, (IInterface**)result);
}

ECode BigDecimal::ValueOf(
    /* [in] */ Double value,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);

    if (Math::IsInfinite(value) || Math::IsNaN(value)) {
        Logger::E("BigDecimal", "Infinity or NaN: %f", value);
        return E_NUMBER_FORMAT_EXCEPTION;
    }
    return CBigDecimal::New(StringUtils::ToString(value), IID_IBigDecimal, (IInterface**)result);
}

ECode BigDecimal::Add(
    /* [in] */ IBigDecimal* augend,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);

    BigDecimal* augendObj = From(augend);
    Integer diffScale = mScale - augendObj->mScale;
    // Fast return when some operand is zero
    if (IsZero()) {
        if (diffScale <= 0) {
            *result = augend;
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
        if (augendObj->IsZero()) {
            *result = this;
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
    }
    else if (augendObj->IsZero()) {
        if (diffScale >= 0) {
            *result = this;
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
    }
    // Let be:  this = [u1,s1]  and  augend = [u2,s2]
    if (diffScale == 0) {
        if (Math::Max(mBitLength, augendObj->mBitLength) + 1 < 64) {
            return ValueOf(mSmallValue + augendObj->mSmallValue, mScale, result);
        }
        AutoPtr<IBigInteger> bi;
        GetUnscaledValue()->Add(augendObj->GetUnscaledValue(), &bi);
        return CBigDecimal::New(bi, mScale, IID_IBigDecimal, (IInterface**)result);
    }
    else if (diffScale > 0) {
        return AddAndMult10(this, augendObj, diffScale, result);
    }
    else {
        return AddAndMult10(augendObj, this, -diffScale, result);
    }
}

ECode BigDecimal::AddAndMult10(
    /* [in] */ BigDecimal* thisValue,
    /* [in] */ BigDecimal* augend,
    /* [in] */ Integer diffScale,
    /* [out] */ IBigDecimal** result)
{
    if (diffScale < MathUtils::LONG_POWERS_OF_TEN.GetLength() &&
            Math::Max(thisValue->mBitLength, augend->mBitLength + GetLONG_POWERS_OF_TEN_BIT_LENGTH()[diffScale]) + 1 < 64) {
        return ValueOf(thisValue->mSmallValue + augend->mSmallValue * MathUtils::LONG_POWERS_OF_TEN[diffScale], thisValue->mScale, result);
    }
    else {
        AutoPtr<BigInt> b = CBigInteger::From(Multiplication::MultiplyByTenPow(
                CBigInteger::From(augend->GetUnscaledValue()), diffScale))->GetBigInt();
        b->Add(CBigInteger::From(thisValue->GetUnscaledValue())->GetBigInt());
        AutoPtr<IBigInteger> bi;
        CBigInteger::New(b, IID_IBigInteger, (IInterface**)&bi);
        return CBigDecimal::New(bi, thisValue->mScale, IID_IBigDecimal, (IInterface**)result);
    }
}

ECode BigDecimal::Add(
    /* [in] */ IBigDecimal* augend,
    /* [in] */ IMathContext* mc,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);
    BigDecimal* augendObj = From(augend);

    AutoPtr<BigDecimal> larger;
    AutoPtr<BigDecimal> smaller;
    AutoPtr<IBigInteger> tempBI;
    Long diffScale = (Long)mScale - augendObj->mScale;
    Integer largerSignum, smallerSignum;
    // Some operand is zero or the precision is infinity
    Integer precision;
    if ((augendObj->IsZero()) || (IsZero()) ||
            (mc->GetPrecision(&precision), precision == 0)) {
        AutoPtr<IBigDecimal> addedBD;
        Add(augend, &addedBD);
        return addedBD->Round(mc, result);
    }
    // Cases where there is room for optimizations
    if (ApproxPrecision() < diffScale - 1) {
        larger = augendObj;
        smaller = this;
    }
    else if (augendObj->ApproxPrecision() < -diffScale - 1) {
        larger = this;
        smaller = augendObj;
    }
    else {
        AutoPtr<IBigDecimal> addedBD;
        Add(augend, &addedBD);
        return addedBD->Round(mc, result);
    }
    if (mc->GetPrecision(&precision), precision >= larger->ApproxPrecision()) {
        AutoPtr<IBigDecimal> addedBD;
        Add(augend, &addedBD);
        return addedBD->Round(mc, result);
    }
    // Cases where it's unnecessary to add two numbers with very different scales
    larger->Signum(&largerSignum);
    smaller->Signum(&smallerSignum);
    if (largerSignum == smallerSignum) {
        AutoPtr<IBigInteger> bi1;
        CBigInteger::ValueOf(largerSignum, &bi1);
        Multiplication::MultiplyByPositiveInteger(
                CBigInteger::From(larger->GetUnscaledValue()), 10)->Add(bi1, &tempBI);
    }
    else {
        AutoPtr<IBigInteger> bi1, bi2, bi3;
        CBigInteger::ValueOf(largerSignum, &bi1);
        larger->GetUnscaledValue()->Subtract(bi1, &bi2);
        CBigInteger::ValueOf(largerSignum * 9, &bi3);
        Multiplication::MultiplyByPositiveInteger(
                CBigInteger::From(bi2), 10)->Add(bi3, &tempBI);
    }
    // Rounding the improved adding
    AutoPtr<IBigDecimal> tempBD;
    CBigDecimal::New(tempBI, larger->mScale + 1, IID_IBigDecimal, (IInterface**)&tempBD);
    return tempBD->Round(mc, result);
}

ECode BigDecimal::Subtract(
    /* [in] */ IBigDecimal* subtrahend,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);
    BigDecimal* subtrahendObj = From(subtrahend);

    Integer diffScale = mScale - subtrahendObj->mScale;
    // Fast return when some operand is zero
    if (IsZero()) {
        if (diffScale <= 0) {
            return subtrahend->Negate(result);
        }
        if (subtrahendObj->IsZero()) {
            *result = this;
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
    }
    else if (subtrahendObj->IsZero()) {
        if (diffScale >= 0) {
            *result = this;
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
    }
    // Let be: this = [u1,s1] and subtrahend = [u2,s2] so:
    if (diffScale == 0) {
        // case s1 = s2 : [u1 - u2 , s1]
        if (Math::Max(mBitLength, subtrahendObj->mBitLength) + 1 < 64) {
            return ValueOf(mSmallValue - subtrahendObj->mSmallValue, mScale, result);
        }
        AutoPtr<IBigInteger> bi;
        GetUnscaledValue()->Subtract(subtrahendObj->GetUnscaledValue(), &bi);
        return CBigDecimal::New(bi, mScale, IID_IBigDecimal, (IInterface**)result);
    }
    else if (diffScale > 0) {
        // case s1 > s2 : [ u1 - u2 * 10 ^ (s1 - s2) , s1 ]
        if (diffScale < MathUtils::LONG_POWERS_OF_TEN.GetLength() &&
                Math::Max(mBitLength, subtrahendObj->mBitLength + GetLONG_POWERS_OF_TEN_BIT_LENGTH()[diffScale]) + 1 < 64) {
            return ValueOf(mSmallValue + subtrahendObj->mSmallValue * MathUtils::LONG_POWERS_OF_TEN[diffScale], mScale, result);
        }
        AutoPtr<IBigInteger> bi1;
        GetUnscaledValue()->Subtract(Multiplication::MultiplyByTenPow(
                CBigInteger::From(subtrahendObj->GetUnscaledValue()), diffScale), &bi1);
        return CBigDecimal::New(bi1, mScale, IID_IBigDecimal, (IInterface**)result);
    }
    else {
        // case s2 > s1 : [ u1 * 10 ^ (s2 - s1) - u2 , s2 ]
        diffScale = -diffScale;
        if (diffScale < MathUtils::LONG_POWERS_OF_TEN.GetLength() &&
                Math::Max(mBitLength + GetLONG_POWERS_OF_TEN_BIT_LENGTH()[diffScale], subtrahendObj->mBitLength) + 1 < 64) {
            return ValueOf(mSmallValue * MathUtils::LONG_POWERS_OF_TEN[diffScale] - subtrahendObj->mSmallValue, subtrahendObj->mScale, result);
        }
        AutoPtr<IBigInteger> bi1;
        Multiplication::MultiplyByTenPow(CBigInteger::From(GetUnscaledValue()),
                diffScale)->Subtract(subtrahendObj->GetUnscaledValue(), &bi1);
        return CBigDecimal::New(bi1, subtrahendObj->mScale, IID_IBigDecimal, (IInterface**)result);
    }
}

ECode BigDecimal::Subtract(
    /* [in] */ IBigDecimal* subtrahend,
    /* [in] */ IMathContext* mc,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);
    BigDecimal* subtrahendObj = From(subtrahend);

    Long diffScale = subtrahendObj->mScale - (Long)mScale;
    Integer thisSignum, subtrahendSignum;
    AutoPtr<IBigDecimal> leftOperand;
    AutoPtr<IBigInteger> tempBI;
    // Some operand is zero or the precision is infinity
    Integer precision;
    if (subtrahendObj->IsZero() || IsZero() ||
            (mc->GetPrecision(&precision), precision == 0)) {
        AutoPtr<IBigDecimal> bd;
        Subtract(subtrahend, &bd);
        return bd->Round(mc, result);
    }
    // Now:   this != 0   and   subtrahend != 0
    if (subtrahendObj->ApproxPrecision() < diffScale - 1) {
        // Cases where it is unnecessary to subtract two numbers with very different scales
        if (mc->GetPrecision(&precision), precision < ApproxPrecision()) {
            Signum(&thisSignum);
            subtrahend->Signum(&subtrahendSignum);
            if (thisSignum != subtrahendSignum) {
                AutoPtr<IBigInteger> bi1;
                CBigInteger::ValueOf(thisSignum, &bi1);
                Multiplication::MultiplyByPositiveInteger(
                        CBigInteger::From(GetUnscaledValue()), 10)->Add(bi1, &tempBI);
            }
            else {
                AutoPtr<IBigInteger> bi1, bi2, bi3;
                CBigInteger::ValueOf(thisSignum, &bi1);
                GetUnscaledValue()->Subtract(bi1, &bi2);
                CBigInteger::ValueOf(thisSignum * 9, &bi3);
                Multiplication::MultiplyByPositiveInteger(
                        CBigInteger::From(bi2), 10)->Add(bi3, &tempBI);
            }
            // Rounding the improved subtracting
            CBigDecimal::New(tempBI, mScale + 1, IID_IBigDecimal, (IInterface**)&leftOperand);
            return leftOperand->Round(mc, result);
        }
    }
    // No optimization is done
    AutoPtr<IBigDecimal> bd;
    Subtract(subtrahend, &bd);
    return bd->Round(mc, result);
}

ECode BigDecimal::Multiply(
    /* [in] */ IBigDecimal* multiplicand,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);
    BigDecimal* multiplicandObj = From(multiplicand);

    Long newScale = (Long)mScale + multiplicandObj->mScale;

    if (IsZero() || multiplicandObj->IsZero()) {
        return ZeroScaledBy(newScale, result);
    }
    /* Let be: this = [u1,s1] and multiplicand = [u2,s2] so:
     * this x multiplicand = [ s1 * s2 , s1 + s2 ] */
    if (mBitLength + multiplicandObj->mBitLength < 64) {
        Long unscaledValue = mSmallValue * multiplicandObj->mSmallValue;
        // b/19185440 Case where result should be +2^63 but unscaledValue overflowed to -2^63
        Boolean longMultiplicationOverflowed = (unscaledValue == ILong::MIN_VALUE) &&
                (Math::Signum(mSmallValue) * Math::Signum(multiplicandObj->mSmallValue) > 0);
        if (!longMultiplicationOverflowed) {
            Integer intScale;
            FAIL_RETURN(SafeLongToInteger(newScale, &intScale));
            return ValueOf(unscaledValue, intScale, result);
        }
    }
    Integer intScale;
    FAIL_RETURN(SafeLongToInteger(newScale, &intScale));
    AutoPtr<IBigInteger> bi;
    GetUnscaledValue()->Multiply(multiplicandObj->GetUnscaledValue(), &bi);
    return CBigDecimal::New(bi, intScale, IID_IBigDecimal, (IInterface**)result);
}

ECode BigDecimal::Multiply(
    /* [in] */ IBigDecimal* multiplicand,
    /* [in] */ IMathContext* mc,
    /* [out] */ IBigDecimal** result)
{
    FAIL_RETURN(Multiply(multiplicand, result));

    return From(*result)->InplaceRound(mc);
}

ECode BigDecimal::Divide(
    /* [in] */ IBigDecimal* divisor,
    /* [in] */ Integer scale,
    /* [in] */ Integer roundingMode,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);

    RoundingMode rm = static_cast<RoundingMode>(roundingMode);
    if (rm < RoundingMode::UP || rm > RoundingMode::UNNECESSARY) {
        Logger::E("BigDecimal", "Invalid rounding mode");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return Divide(divisor, scale, rm, result);
}

ECode BigDecimal::Divide(
    /* [in] */ IBigDecimal* divisor,
    /* [in] */ Integer scale,
    /* [in] */ RoundingMode roundingMode,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);

    BigDecimal* divisorObj = From(divisor);
    if (divisorObj->IsZero()) {
        Logger::E("BigDecimal", "Division by zero");
        return E_ARITHMETIC_EXCEPTION;
    }

    Long diffScale = ((Long)mScale - divisorObj->mScale) - scale;

    // Check whether the diffScale will fit into an int. See http://b/17393664.
    if (BitLength(diffScale) > 32) {
        Logger::E("BigDecimal", "Unable to perform divisor / dividend scaling:"
                " the difference in scale is too big (%lld)", diffScale);
        return E_ARITHMETIC_EXCEPTION;
    }

    if (mBitLength < 64 && divisorObj->mBitLength < 64) {
        if (diffScale == 0) {
            // http://b/26105053 - corner case: Long.MIN_VALUE / (-1) overflows a long
            if (mSmallValue != ILong::MIN_VALUE || divisorObj->mSmallValue != -1) {
                return DividePrimitiveLongs(mSmallValue, divisorObj->mSmallValue,
                        scale, roundingMode, result);
            }
        }
        else if (diffScale > 0) {
            if (diffScale < MathUtils::LONG_POWERS_OF_TEN.GetLength() &&
                    divisorObj->mBitLength + GetLONG_POWERS_OF_TEN_BIT_LENGTH()[(Integer)diffScale] < 64) {
                return DividePrimitiveLongs(mSmallValue,
                        divisorObj->mSmallValue * MathUtils::LONG_POWERS_OF_TEN[(Integer)diffScale],
                        scale, roundingMode, result);
            }
        }
        else {
            if (-diffScale < MathUtils::LONG_POWERS_OF_TEN.GetLength() &&
                    mBitLength + GetLONG_POWERS_OF_TEN_BIT_LENGTH()[(Integer)-diffScale] < 64) {
                return DividePrimitiveLongs(mSmallValue * MathUtils::LONG_POWERS_OF_TEN[(Integer)-diffScale],
                        divisorObj->mSmallValue, scale, roundingMode, result);
            }
        }
    }
    AutoPtr<IBigInteger> scaledDividend = GetUnscaledValue();
    AutoPtr<IBigInteger> scaledDivisor = divisorObj->GetUnscaledValue(); // for scaling of 'u2'

    if (diffScale > 0) {
        // Multiply 'u2'  by:  10^((s1 - s2) - scale)
        scaledDivisor = Multiplication::MultiplyByTenPow(CBigInteger::From(scaledDivisor),
                (Integer)diffScale);
    }
    else if (diffScale < 0) {
        // Multiply 'u1'  by:  10^(scale - (s1 - s2))
        scaledDividend = Multiplication::MultiplyByTenPow(CBigInteger::From(scaledDividend),
                (Integer)-diffScale);
    }
    return DivideBigIntegers(scaledDividend, scaledDivisor, scale, roundingMode, result);
}

ECode BigDecimal::DivideBigIntegers(
    /* [in] */ IBigInteger* scaledDividend,
    /* [in] */ IBigInteger* scaledDivisor,
    /* [in] */ Integer scale,
    /* [in] */ RoundingMode roundingMode,
    /* [out] */ IBigDecimal** result)
{
    Array<IBigInteger*> quotAndRem;
    scaledDividend->DivideAndRemainder(scaledDivisor, &quotAndRem);
    // If after division there is a remainder...
    AutoPtr<IBigInteger> quotient = quotAndRem[0];
    AutoPtr<IBigInteger> remainder = quotAndRem[1];
    Integer sign;
    if (remainder->Signum(&sign), sign == 0) {
        return CBigDecimal::New(quotient, scale, IID_IBigDecimal, (IInterface**)result);
    }
    Integer s1, s2;
    scaledDividend->Signum(&s1);
    scaledDivisor->Signum(&s2);
    sign = s1 * s2;
    Integer compRem;
    Integer bitLength;
    if (scaledDivisor->BitLength(&bitLength), bitLength < 63) { // 63 in order to avoid out of long after *2
        Long rem, divisor;
        INumber::Probe(remainder)->LongValue(rem);
        INumber::Probe(scaledDivisor)->LongValue(divisor);
        compRem = CompareForRounding(rem, divisor);
        // To look if there is a carry
        Boolean set;
        FAIL_RETURN(RoundingBehavior((quotient->TestBit(0, &set),
                set ? 1 : 0), sign * (5 + compRem), roundingMode, &compRem));
    }
    else {
        // Checking if:  remainder * 2 >= scaledDivisor
        AutoPtr<IBigInteger> bi1, bi2;
        remainder->Abs(&bi1);
        scaledDivisor->Abs(&bi2);
        IComparable::Probe(CBigInteger::From(bi1)->ShiftLeftOneBit())->CompareTo(bi2, compRem);
        Boolean set;
        FAIL_RETURN(RoundingBehavior((quotient->TestBit(0, &set),
                set ? 1 : 0), sign * (5 + compRem), roundingMode, &compRem));
    }
    if (compRem != 0) {
        if (quotient->BitLength(&bitLength), bitLength < 63) {
            Long quot;
            INumber::Probe(quotient)->LongValue(quot);
            return ValueOf(quot + compRem, scale, result);
        }
        AutoPtr<IBigInteger> bi1, bi2;
        CBigInteger::ValueOf(compRem, &bi1);
        quotient->Add(bi1, &bi2);
        return CBigDecimal::New(bi2, scale, IID_IBigDecimal, (IInterface**)result);
    }
    return CBigDecimal::New(quotient, scale, IID_IBigDecimal, (IInterface**)result);
}

ECode BigDecimal::DividePrimitiveLongs(
    /* [in] */ Long scaledDividend,
    /* [in] */ Long scaledDivisor,
    /* [in] */ Integer scale,
    /* [in] */ RoundingMode roundingMode,
    /* [out] */ IBigDecimal** result)
{
    Long quotient = scaledDividend / scaledDivisor;
    Long remainder = scaledDividend % scaledDivisor;
    Integer sign = Math::Signum(scaledDividend) * Math::Signum(scaledDivisor);
    if (remainder != 0) {
        // Checking if:  remainder * 2 >= scaledDivisor
        Integer compRem = CompareForRounding(remainder, scaledDivisor);
        // To look if there is a carry
        Integer carry;
        FAIL_RETURN(RoundingBehavior(((Integer)quotient) & 1, sign * (5 + compRem),
                roundingMode, &carry));
        quotient += carry;
    }
    return ValueOf(quotient, scale, result);
}

ECode BigDecimal::Divide(
    /* [in] */ IBigDecimal* divisor,
    /* [in] */ Integer roundingMode,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);

    RoundingMode rm = static_cast<RoundingMode>(roundingMode);
    if (rm < RoundingMode::UP || rm > RoundingMode::UNNECESSARY) {
        Logger::E("BigDecimal", "Invalid rounding mode");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return Divide(divisor, mScale, rm, result);
}

ECode BigDecimal::Divide(
    /* [in] */ IBigDecimal* divisor,
    /* [in] */ RoundingMode roundingMode,
    /* [out] */ IBigDecimal** result)
{
    return Divide(divisor, mScale, roundingMode, result);
}

ECode BigDecimal::Divide(
    /* [in] */ IBigDecimal* divisor,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);
    BigDecimal* divisorObj = From(divisor);

    AutoPtr<IBigInteger> p = GetUnscaledValue();
    AutoPtr<IBigInteger> q = divisorObj->GetUnscaledValue();
    AutoPtr<IBigInteger> gcd; // greatest common divisor between 'p' and 'q'
    Array<IBigInteger*> quotAndRem;
    Long diffScale = (Long)mScale - divisorObj->mScale;
    Integer newScale; // the new scale for final quotient
    Integer k; // number of factors "2" in 'q'
    Integer l = 0; // number of factors "5" in 'q'
    Integer i = 1;
    Integer lastPow = GetFIVE_POW().GetLength() - 1;

    if (divisorObj->IsZero()) {
        Logger::E("BigDecimal", "Division by zero");
        return E_ARITHMETIC_EXCEPTION;
    }
    Integer sign;
    if (p->Signum(&sign), sign == 0) {
        return ZeroScaledBy(diffScale, result);
    }
    AutoPtr<IBigInteger> tempBI;
    // To divide both by the GCD
    p->Gcd(q, &gcd);
    p->Divide(gcd, &tempBI);
    p = std::move(tempBI);
    q->Divide(gcd, &tempBI);
    q = std::move(tempBI);
    // To simplify all "2" factors of q, dividing by 2^k
    q->GetLowestSetBit(&k);
    q->ShiftRight(k, &tempBI);
    q = std::move(tempBI);
    // To simplify all "5" factors of q, dividing by 5^l
    do {
        q->DivideAndRemainder(GetFIVE_POW()[i], &quotAndRem);
        if (quotAndRem[1]->Signum(&sign), sign == 0) {
            l += i;
            if (i < lastPow) {
                i++;
            }
            q = quotAndRem[0];
        }
        else {
            if (i == 1) {
                break;
            }
            i = 1;
        }
    } while (true);
    // If  abs(q) != 1  then the quotient is periodic
    q->Abs(&tempBI);
    if (!Object::Equals(tempBI, CBigInteger::GetONE())) {
        Logger::E("BigDecimal", "Non-terminating decimal expansion; no exact representable decimal result");
        return E_ARITHMETIC_EXCEPTION;
    }
    tempBI = nullptr;
    // The sign of the is fixed and the quotient will be saved in 'p'
    if (q->Signum(&sign), sign < 0) {
        p->Negate(&tempBI);
        p = std::move(tempBI);
    }
    // Checking if the new scale is out of range
    FAIL_RETURN(SafeLongToInteger(diffScale + Math::Max(k, l), &newScale));
    // k >= 0  and  l >= 0  implies that  k - l  is in the 32-bit range
    i = k - l;

    if (i > 0) {
        tempBI = Multiplication::MultiplyByFivePow(CBigInteger::From(p), i);
    }
    else {
        p->ShiftLeft(-i, &tempBI);
    }
    return CBigDecimal::New(tempBI, newScale, IID_IBigDecimal, (IInterface**)result);
}

ECode BigDecimal::Divide(
    /* [in] */ IBigDecimal* divisor,
    /* [in] */ IMathContext* mc,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);
    BigDecimal* divisorObj = From(divisor);

    /* Calculating how many zeros must be append to 'dividend'
     * to obtain a  quotient with at least 'mc.precision()' digits */
    Integer precision;
    mc->GetPrecision(&precision);
    Long trailingZeros = precision + 2LL + divisorObj->ApproxPrecision() -
            ApproxPrecision();
    Long diffScale = (Long)mScale - divisorObj->mScale;
    Long newScale = diffScale; // scale of the final quotient
    Integer compRem; // to compare the remainder
    Integer i = 1; // index
    Integer lastPow = GetTEN_POW().GetLength() - 1; // last power of ten
    AutoPtr<IBigInteger> integerQuot; // for temporal results
    Array<IBigInteger*> quotAndRem{ GetUnscaledValue() };
    // In special cases it reduces the problem to call the dual method
    if ((precision == 0) || IsZero() || divisorObj->IsZero()) {
        return Divide(divisor, result);
    }
    if (trailingZeros > 0) {
        // To append trailing zeros at end of dividend
        AutoPtr<IBigInteger> bi1;
        GetUnscaledValue()->Multiply(Multiplication::PowerOf10(trailingZeros), &bi1);
        quotAndRem.Set(0, bi1);
        newScale += trailingZeros;
    }
    quotAndRem[0]->DivideAndRemainder(divisorObj->GetUnscaledValue(), &quotAndRem);
    integerQuot = quotAndRem[0];
    // Calculating the exact quotient with at least 'mc.precision()' digits
    Integer sign;
    if (quotAndRem[1]->Signum(&sign), sign != 0) {
        // Checking if:   2 * remainder >= divisor ?
        IComparable::Probe(CBigInteger::From(quotAndRem[1])->ShiftLeftOneBit())->CompareTo(
                divisorObj->GetUnscaledValue(), compRem);
        // quot := quot * 10 + r;     with 'r' in {-6,-5,-4, 0,+4,+5,+6}
        AutoPtr<IBigInteger> bi1, bi2, bi3;
        integerQuot->Multiply(CBigInteger::GetTEN(), &bi1);
        quotAndRem[0]->Signum(&sign);
        CBigInteger::ValueOf(sign * (5 + compRem), &bi2);
        bi1->Add(bi2, &bi3);
        integerQuot = std::move(bi3);
        newScale++;
    }
    else {
        // To strip trailing zeros until the preferred scale is reached
        Boolean set;
        while (integerQuot->TestBit(0, &set), !set) {
            integerQuot->DivideAndRemainder(GetTEN_POW()[i], &quotAndRem);
            Integer sign;
            if ((quotAndRem[1]->Signum(&sign), sign == 0) &&
                    (newScale - i >= diffScale)) {
                newScale -= i;
                if (i < lastPow) {
                    i++;
                }
                integerQuot = quotAndRem[0];
            }
            else {
                if (i == 1) {
                    break;
                }
                i = 1;
            }
        }
    }
    // To perform rounding
    Integer intScale;
    FAIL_RETURN(SafeLongToInteger(newScale, &intScale));
    return CBigDecimal::New(integerQuot, intScale, mc, IID_IBigDecimal, (IInterface**)result);
}

ECode BigDecimal::DivideToIntegralValue(
    /* [in] */ IBigDecimal* divisor,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);
    BigDecimal* divisorObj = From(divisor);

    AutoPtr<IBigInteger> integralValue; // the integer of result
    AutoPtr<IBigInteger> powerOfTen; // some power of ten

    Long newScale = (Long)mScale - divisorObj->mScale;
    Long tempScale = 0;
    Integer i = 1;
    Integer lastPow = GetTEN_POW().GetLength() - 1;

    if (divisorObj->IsZero()) {
        Logger::E("BigDecimal", "Division by zero");
        return E_ARITHMETIC_EXCEPTION;
    }
    if ((divisorObj->ApproxPrecision() + newScale > ApproxPrecision() + 1LL) || IsZero()) {
        /* If the divisor's integer part is greater than this's integer part,
         * the result must be zero with the appropriate scale */
        integralValue = CBigInteger::GetZERO();
    }
    else if (newScale == 0) {
        GetUnscaledValue()->Divide(
                divisorObj->GetUnscaledValue(), &integralValue);
    }
    else if (newScale > 0) {
        powerOfTen = Multiplication::PowerOf10(newScale);
        AutoPtr<IBigInteger> bi1, bi2;
        divisorObj->GetUnscaledValue()->Multiply(powerOfTen, &bi1);
        GetUnscaledValue()->Divide(bi1, &bi2);
        bi2->Multiply(powerOfTen, &integralValue);
    }
    else {
        powerOfTen = Multiplication::PowerOf10(-newScale);
        AutoPtr<IBigInteger> bi1;
        GetUnscaledValue()->Multiply(powerOfTen, &bi1);
        bi1->Divide(divisorObj->GetUnscaledValue(), &integralValue);
        // To strip trailing zeros approximating to the preferred scale
        Boolean set;
        while (integralValue->TestBit(0, &set), !set) {
            Array<IBigInteger*> quotAndRem;
            integralValue->DivideAndRemainder(GetTEN_POW()[i], &quotAndRem);
            Integer sign;
            if ((quotAndRem[1]->Signum(&sign), sign == 0) &&
                    (tempScale - i >= newScale)) {
                tempScale -= i;
                if (i < lastPow) {
                    i++;
                }
                integralValue = quotAndRem[0];
            }
            else {
                if (i == 1) {
                    break;
                }
                i = 1;
            }
        }
        newScale = tempScale;
    }
    Integer sign;
    if (integralValue->Signum(&sign), sign == 0) {
        return ZeroScaledBy(newScale, result);
    }
    else {
        Integer intScale;
        FAIL_RETURN(SafeLongToInteger(newScale, &intScale));
        return CBigDecimal::New(integralValue, intScale, IID_IBigDecimal, (IInterface**)result);
    }
}

ECode BigDecimal::DivideToIntegralValue(
    /* [in] */ IBigDecimal* divisor,
    /* [in] */ IMathContext* mc,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);
    BigDecimal* divisorObj = From(divisor);

    Integer mcPrecision;
    mc->GetPrecision(&mcPrecision);
    Integer thisPrecision, divisorPrecision;
    Precision(&thisPrecision);
    divisorObj->Precision(&divisorPrecision);
    Integer diffPrecision = thisPrecision - divisorPrecision;
    Integer lastPow = GetTEN_POW().GetLength() - 1;
    Long diffScale = (Long)mScale - divisorObj->mScale;
    Long newScale = diffScale;
    Long quotPrecision = diffPrecision - diffScale + 1;
    Array<IBigInteger*> quotAndRem(2);
    // In special cases it call the dual method
    if ((mcPrecision == 0) || IsZero() || divisorObj->IsZero()) {
        return DivideToIntegralValue(divisor, result);
    }
    // Let be:   this = [u1,s1]   and   divisor = [u2,s2]
    if (quotPrecision <= 0) {
        quotAndRem.Set(0, CBigInteger::GetZERO());
    }
    else if (diffScale == 0) {
        // CASE s1 == s2:  to calculate   u1 / u2
        AutoPtr<IBigInteger> bi;
        GetUnscaledValue()->Divide(divisorObj->GetUnscaledValue(), &bi);
        quotAndRem.Set(0, bi);
    }
    else if (diffScale > 0) {
        // CASE s1 >= s2:  to calculate   u1 / (u2 * 10^(s1-s2)
        AutoPtr<IBigInteger> bi1, bi2;
        divisorObj->GetUnscaledValue()->Multiply(Multiplication::PowerOf10(diffScale), &bi1);
        GetUnscaledValue()->Divide(bi1, &bi2);
        quotAndRem.Set(0, bi2);
        // To chose  10^newScale  to get a quotient with at least 'mc.precision()' digits
        newScale = Math::Min(diffScale, Math::Max(mcPrecision - quotPrecision + 1, 0LL));
        // To calculate: (u1 / (u2 * 10^(s1-s2)) * 10^newScale
        bi1 = nullptr;
        quotAndRem[0]->Multiply(Multiplication::PowerOf10(newScale), &bi1);
        quotAndRem.Set(0, bi1);
    }
    else {
        /* To calculate the minimum power of ten, such that the quotient
         *   (u1 * 10^exp) / u2   has at least 'mc.precision()' digits. */
        Long exp = Math::Min(-diffScale, Math::Max((Long)mcPrecision - diffPrecision, 0LL));
        Long compRemDiv;
        // Let be:   (u1 * 10^exp) / u2 = [q,r]
        AutoPtr<IBigInteger> bi1, bi2;
        GetUnscaledValue()->Multiply(Multiplication::PowerOf10(exp), &bi1);
        bi1->DivideAndRemainder(divisorObj->GetUnscaledValue(), &quotAndRem);
        newScale += exp; // To fix the scale
        exp = -newScale; // The remaining power of ten
        // If after division there is a remainder...
        Integer sign;
        if ((quotAndRem[1]->Signum(&sign), sign != 0) && (exp > 0)) {
            // Log10(r) + ((s2 - s1) - exp) > mc.precision ?
            AutoPtr<IBigDecimal> bd;
            CBigDecimal::New(quotAndRem[1], IID_IBigDecimal, (IInterface**)&bd);
            Integer quotAndRemPrecision, divisorPrecision;
            bd->Precision(&quotAndRemPrecision);
            divisorObj->Precision(&divisorPrecision);
            compRemDiv = quotAndRemPrecision + exp - divisorPrecision;
            if (compRemDiv == 0) {
                // To calculate:  (r * 10^exp2) / u2
                bi1 = bi2 = nullptr;
                quotAndRem[1]->Multiply(Multiplication::PowerOf10(exp), &bi1);
                bi1->Divide(divisorObj->GetUnscaledValue(), &bi2);
                quotAndRem.Set(1, bi2);
                quotAndRem[1]->Signum(&sign);
                compRemDiv = Math::Abs(sign);
            }
            if (compRemDiv > 0) {
                Logger::E("BigDecimal", "Division impossible");
                return E_ARITHMETIC_EXCEPTION;
            }
        }
    }
    // Fast return if the quotient is zero
    Integer sign;
    if (quotAndRem[0]->Signum(&sign), sign == 0) {
        return ZeroScaledBy(diffScale, result);
    }
    AutoPtr<IBigInteger> strippedBI = quotAndRem[0];
    AutoPtr<IBigDecimal> integralValue;
    CBigDecimal::New(quotAndRem[0], IID_IBigDecimal, (IInterface**)&integralValue);
    Integer precision;
    integralValue->Precision(&precision);
    Long resultPrecision = precision;
    Integer i = 1;
    // To strip trailing zeros until the specified precision is reached
    Boolean set;
    while (strippedBI->TestBit(0, &set), !set) {
        strippedBI->DivideAndRemainder(GetTEN_POW()[i], &quotAndRem);
        if ((quotAndRem[1]->Signum(&sign), sign == 0) &&
                ((resultPrecision - i >= mcPrecision) ||
                (newScale - i >= diffScale))) {
            resultPrecision -= i;
            newScale -= i;
            if (i < lastPow) {
                i++;
            }
            strippedBI = quotAndRem[0];
        }
        else {
            if (i == 1) {
                break;
            }
            i = 1;
        }
    }
    // To check if the result fit in 'mc.precision()' digits
    if (resultPrecision > mcPrecision) {
        Logger::E("BigDecimal", "Division impossible");
        return E_ARITHMETIC_EXCEPTION;
    }
    Integer intScale;
    FAIL_RETURN(SafeLongToInteger(newScale, &intScale));
    From(integralValue)->mScale = intScale;
    From(integralValue)->SetUnscaledValue(strippedBI);
    integralValue.MoveTo(result);
    return NOERROR;
}

ECode BigDecimal::Remainder(
    /* [in] */ IBigDecimal* divisor,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);

    Array<IBigDecimal*> quotAndRem;
    FAIL_RETURN(DivideAndRemainder(divisor, &quotAndRem));
    *result = quotAndRem[1];
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode BigDecimal::Remainder(
    /* [in] */ IBigDecimal* divisor,
    /* [in] */ IMathContext* mc,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);

    Array<IBigDecimal*> quotAndRem;
    FAIL_RETURN(DivideAndRemainder(divisor, mc, &quotAndRem));
    *result = quotAndRem[1];
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode BigDecimal::DivideAndRemainder(
    /* [in] */ IBigDecimal* divisor,
    /* [out, callee] */ Array<IBigDecimal*>* quotAndRem)
{
    VALIDATE_NOT_NULL(quotAndRem);

    *quotAndRem = Array<IBigDecimal*>(2);

    AutoPtr<IBigDecimal> quotient, remainder, tempBD;
    FAIL_RETURN(DivideToIntegralValue(divisor, &quotient));
    quotAndRem->Set(0, quotient);
    (*quotAndRem)[0]->Multiply(divisor, &tempBD);
    Subtract(tempBD, &remainder);
    quotAndRem->Set(1, remainder);
    return NOERROR;
}

ECode BigDecimal::DivideAndRemainder(
    /* [in] */ IBigDecimal* divisor,
    /* [in] */ IMathContext* mc,
    /* [out, callee] */ Array<IBigDecimal*>* quotAndRem)
{
    VALIDATE_NOT_NULL(quotAndRem);

    *quotAndRem = Array<IBigDecimal*>(2);

    AutoPtr<IBigDecimal> quotient, remainder, tempBD;
    FAIL_RETURN(DivideToIntegralValue(divisor, mc, &quotient));
    quotAndRem->Set(0, quotient);
    (*quotAndRem)[0]->Multiply(divisor, &tempBD);
    Subtract(tempBD, &remainder);
    quotAndRem->Set(1, remainder);
    return NOERROR;
}

ECode BigDecimal::Pow(
    /* [in] */ Integer n,
    /* [out] */ IBigDecimal** value)
{
    VALIDATE_NOT_NULL(value);

    if (n == 0) {
        GetONE().MoveTo(value);
        return NOERROR;
    }
    if ((n < 0) || (n > 999999999)) {
        Logger::E("BigDecimal", "Invalid operation");
        return E_ARITHMETIC_EXCEPTION;
    }
    Long newScale = mScale * (Long)n;
    // Let be: this = [u,s]   so:  this^n = [u^n, s*n]
    if (IsZero()) {
        return ZeroScaledBy(newScale, value);
    }
    else {
        AutoPtr<IBigInteger> bi;
        GetUnscaledValue()->Pow(n, &bi);
        Integer intScale;
        FAIL_RETURN(SafeLongToInteger(newScale, &intScale));
        return CBigDecimal::New(bi, intScale, IID_IBigDecimal, (IInterface**)value);
    }
}

ECode BigDecimal::Pow(
    /* [in] */ Integer n,
    /* [in] */ IMathContext* mc,
    /* [out] */ IBigDecimal** value)
{
    VALIDATE_NOT_NULL(value);

    // The ANSI standard X3.274-1996 algorithm
    Integer m = Math::Abs(n);
    Integer mcPrecision;
    mc->GetPrecision(&mcPrecision);
    Integer elength = (Integer)Math::Log10(m) + 1; // decimal digits in 'n'
    Integer oneBitMask; // mask of bits
    AutoPtr<IBigDecimal> accum; // the single accumulator
    AutoPtr<IMathContext> newPrecision = mc; // MathContext by default

    // In particular cases, it reduces the problem to call the other 'pow()'
    if ((n == 0) || (IsZero() && (n > 0))) {
        return Pow(n, value);
    }
    if ((m > 999999999)|| ((mcPrecision == 0) && (n < 0)) ||
            ((mcPrecision > 0) && (elength > mcPrecision))) {
        Logger::E("BigDecimal", "Invalid operation");
        return E_ARITHMETIC_EXCEPTION;
    }
    if (mcPrecision > 0) {
        RoundingMode roundingMode;
        mc->GetRoundingMode(&roundingMode);
        newPrecision = nullptr;
        CMathContext::New(mcPrecision + elength + 1, roundingMode,
                IID_IMathContext, (IInterface**)&newPrecision);
    }
    // The result is calculated as if 'n' were positive
    Round(newPrecision, &accum);
    oneBitMask = Math::HighestOneBit(m) >> 1;

    while (oneBitMask > 0) {
        AutoPtr<IBigDecimal> tempBD;
        accum->Multiply(accum, newPrecision, &tempBD);
        accum = std::move(tempBD);
        if ((m & oneBitMask) == oneBitMask) {
            accum->Multiply(this, newPrecision, &tempBD);
            accum = std::move(tempBD);
        }
        oneBitMask >>= 1;
    }
    // If 'n' is negative, the value is divided into 'ONE'
    if (n < 0) {
        AutoPtr<IBigDecimal> tempBD;
        GetONE()->Divide(accum, newPrecision, &tempBD);
        accum = std::move(tempBD);
    }
    // The final value is rounded to the destination precision
    From(accum)->InplaceRound(mc);
    accum.MoveTo(value);
    return NOERROR;
}

ECode BigDecimal::Abs(
    /* [out] */ IBigDecimal** value)
{
    VALIDATE_NOT_NULL(value);

    Integer sign;
    if (Signum(&sign), sign < 0) {
        return Negate(value);
    }
    else {
        *value = this;
        REFCOUNT_ADD(*value);
        return NOERROR;
    }
}

ECode BigDecimal::Abs(
    /* [in] */ IMathContext* mc,
    /* [out] */ IBigDecimal** value)
{
    VALIDATE_NOT_NULL(value);

    AutoPtr<IBigDecimal> result;
    Integer sign;
    if (Signum(&sign), sign < 0) {
        Negate(&result);
    }
    else {
        CBigDecimal::New(GetUnscaledValue(), mScale, IID_IBigDecimal, (IInterface**)&result);
    }
    From(result)->InplaceRound(mc);
    result.MoveTo(value);
    return NOERROR;
}

ECode BigDecimal::Negate(
    /* [out] */ IBigDecimal** value)
{
    VALIDATE_NOT_NULL(value);

    if (mBitLength < 63 || (mBitLength == 63 && mSmallValue != ILong::MIN_VALUE)) {
        return ValueOf(-mSmallValue, mScale, value);
    }
    AutoPtr<IBigInteger> tempBI;
    GetUnscaledValue()->Negate(&tempBI);
    return CBigDecimal::New(tempBI, mScale, IID_IBigDecimal, (IInterface**)value);
}

ECode BigDecimal::Negate(
    /* [in] */ IMathContext* mc,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);

    Negate(result);
    return From(*result)->InplaceRound(mc);
}

ECode BigDecimal::Plus(
    /* [out] */ IBigDecimal** value)
{
    VALIDATE_NOT_NULL(value);

    *value = this;
    REFCOUNT_ADD(*value);
    return NOERROR;
}

ECode BigDecimal::Plus(
    /* [in] */ IMathContext* mc,
    /* [out] */ IBigDecimal** result)
{
    return Round(mc, result);
}

ECode BigDecimal::Signum(
    /* [out] */ Integer* sign)
{
    VALIDATE_NOT_NULL(sign);

    if (mBitLength < 64) {
        *sign = Math::Signum(mSmallValue);
        return NOERROR;
    }
    return GetUnscaledValue()->Signum(sign);
}

Boolean BigDecimal::IsZero()
{
    return mBitLength == 0 && mSmallValue != -1;
}

ECode BigDecimal::Scale(
    /* [out] */ Integer* scale)
{
    VALIDATE_NOT_NULL(scale);

    *scale = mScale;
    return NOERROR;
}

ECode BigDecimal::Precision(
    /* [out] */ Integer* precision)
{
    VALIDATE_NOT_NULL(precision);

    if (mPrecision != 0) {
        *precision = mPrecision;
        return NOERROR;
    }

    if (mBitLength == 0) {
        mPrecision = 1;
    }
    else if (mBitLength < 64) {
        mPrecision = DecimalDigitsInLong(mSmallValue);
    }
    else {
        Integer decimalDigits = 1 + (Integer)((mBitLength - 1) * LOG10_2);
        // If after division the number isn't zero, there exists an additional digit
        AutoPtr<IBigInteger> tempBI;
        GetUnscaledValue()->Divide(Multiplication::PowerOf10(decimalDigits), &tempBI);
        Integer sign;
        if (tempBI->Signum(&sign), sign != 0) {
            decimalDigits++;
        }
        mPrecision = decimalDigits;
    }
    *precision = mPrecision;
    return NOERROR;
}

Integer BigDecimal::DecimalDigitsInLong(
    /* [in] */ Long value)
{
    if (value == ILong::MIN_VALUE) {
        return 19; // special case required because abs(MIN_VALUE) == MIN_VALUE
    }
    else {
        Integer index = Arrays::BinarySearch(MathUtils::LONG_POWERS_OF_TEN, Math::Abs(value));
        return (index < 0) ? (-index - 1) : (index + 1);
    }
}

ECode BigDecimal::UnscaledValue(
    /* [out] */ IBigInteger** value)
{
    VALIDATE_NOT_NULL(value);

    GetUnscaledValue().MoveTo(value);
    return NOERROR;
}

ECode BigDecimal::Round(
    /* [in] */ IMathContext* mc,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IBigDecimal> bd;
    CBigDecimal::New(GetUnscaledValue(), mScale, IID_IBigDecimal, (IInterface**)&bd);

    From(bd)->InplaceRound(mc);
    bd.MoveTo(result);
    return NOERROR;
}

ECode BigDecimal::SetScale(
    /* [in] */ Integer newScale,
    /* [in] */ RoundingMode roundingMode,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);

    Long diffScale = newScale - (Long)mScale;
    // Let be:  'this' = [u,s]
    if (diffScale == 0) {
        *result = this;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    if (diffScale > 0) {
        // return  [u * 10^(s2 - s), newScale]
        if (diffScale < MathUtils::LONG_POWERS_OF_TEN.GetLength() &&
                (mBitLength + GetLONG_POWERS_OF_TEN_BIT_LENGTH()[(Integer)diffScale]) < 64) {
            return ValueOf(mSmallValue * MathUtils::LONG_POWERS_OF_TEN[(Integer)diffScale], newScale, result);
        }
        return CBigDecimal::New(Multiplication::MultiplyByTenPow(CBigInteger::From(GetUnscaledValue()),
                (Integer)diffScale), newScale, IID_IBigDecimal, (IInterface**)result);
    }
    // return  [u,s] / [1,newScale]  with the appropriate scale and rounding
    if (mBitLength < 64 && -diffScale < MathUtils::LONG_POWERS_OF_TEN.GetLength()) {
        return DividePrimitiveLongs(mSmallValue, MathUtils::LONG_POWERS_OF_TEN[(Integer)-diffScale], newScale, roundingMode, result);
    }
    return DivideBigIntegers(GetUnscaledValue(), Multiplication::PowerOf10(-diffScale), newScale, roundingMode, result);
}

ECode BigDecimal::SetScale(
    /* [in] */ Integer newScale,
    /* [in] */ Integer roundingMode,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);

    RoundingMode rm = static_cast<RoundingMode>(roundingMode);
    if (rm < RoundingMode::UP || rm > RoundingMode::UNNECESSARY) {
        Logger::E("BigDecimal", "Invalid rounding mode");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return SetScale(newScale, rm, result);
}

ECode BigDecimal::SetScale(
    /* [in] */ Integer newScale,
    /* [out] */ IBigDecimal** value)
{
    return SetScale(newScale, RoundingMode::UNNECESSARY, value);
}

ECode BigDecimal::MovePointLeft(
    /* [in] */ Integer n,
    /* [out] */ IBigDecimal** value)
{
    VALIDATE_NOT_NULL(value);

    return MovePoint(mScale + (Long)n, value);
}

ECode BigDecimal::MovePoint(
    /* [in] */ Long newScale,
    /* [out] */ IBigDecimal** value)
{
    if (IsZero()) {
        return ZeroScaledBy(Math::Max(newScale, 0LL), value);
    }
    /*
     * When: 'n'== Integer.MIN_VALUE isn't possible to call to
     * movePointRight(-n) since -Integer.MIN_VALUE == Integer.MIN_VALUE
     */
    if (newScale >= 0) {
        Integer intScale;
        FAIL_RETURN(SafeLongToInteger(newScale, &intScale));
        if (mBitLength < 64) {
            return ValueOf(mSmallValue, intScale, value);
        }
        return CBigDecimal::New(GetUnscaledValue(), intScale, IID_IBigDecimal, (IInterface**)value);
    }
    if (-newScale < MathUtils::LONG_POWERS_OF_TEN.GetLength() &&
            mBitLength + GetLONG_POWERS_OF_TEN_BIT_LENGTH()[(Integer)-newScale] < 64) {
        return ValueOf(mSmallValue * MathUtils::LONG_POWERS_OF_TEN[(Integer)-newScale], 0, value);
    }
    Integer intScale;
    FAIL_RETURN(SafeLongToInteger(-newScale, &intScale));
    return CBigDecimal::New(Multiplication::MultiplyByTenPow(CBigInteger::From(GetUnscaledValue()), intScale),
            0, IID_IBigDecimal, (IInterface**)value);
}

ECode BigDecimal::MovePointRight(
    /* [in] */ Integer n,
    /* [out] */ IBigDecimal** value)
{
    VALIDATE_NOT_NULL(value);

    return MovePoint(mScale - (Long)n, value);
}

ECode BigDecimal::ScaleByPowerOfTen(
    /* [in] */ Integer n,
    /* [out] */ IBigDecimal** value)
{
    VALIDATE_NOT_NULL(value);

    Long newScale = mScale - (Long)n;
    if (mBitLength < 64) {
        //Taking care when a 0 is to be scaled
        if (mSmallValue == 0) {
            return ZeroScaledBy(newScale, value);
        }
        Integer intScale;
        FAIL_RETURN(SafeLongToInteger(newScale, &intScale));
        return ValueOf(mSmallValue, intScale, value);
    }
    Integer intScale;
    FAIL_RETURN(SafeLongToInteger(newScale, &intScale));
    return CBigDecimal::New(GetUnscaledValue(), intScale, IID_IBigDecimal, (IInterface**)value);
}

ECode BigDecimal::StripTrailingZeros(
    /* [out] */ IBigDecimal** value)
{
    VALIDATE_NOT_NULL(value);

    Integer i = 1; // 1 <= i <= 18
    Integer lastPow = GetTEN_POW().GetLength() - 1;
    Long newScale = mScale;

    if (IsZero()) {
        // Preserve RI compatibility, so BigDecimal::Equals (which checks
        // value *and* scale) continues to work.
        *value = this;
        REFCOUNT_ADD(*value);
        return NOERROR;
    }
    AutoPtr<IBigInteger> strippedBI = GetUnscaledValue();
    Array<IBigInteger*> quotAndRem;

    // while the number is even...
    Boolean set;
    while (strippedBI->TestBit(0, &set), !set) {
        // To divide by 10^i
        strippedBI->DivideAndRemainder(GetTEN_POW()[i], &quotAndRem);
        // To look the remainder
        Integer sign;
        if (quotAndRem[1]->Signum(&sign), sign == 0) {
            // To adjust the scale
            newScale -= i;
            if (i < lastPow) {
                // To set to the next power
                i++;
            }
            strippedBI = quotAndRem[0];
        }
        else {
            if (i == 1) {
                // 'this' has no more trailing zeros
                break;
            }
            // To set to the smallest power of ten
            i = 1;
        }
    }
    Integer intScale;
    FAIL_RETURN(SafeLongToInteger(newScale, &intScale));
    return CBigDecimal::New(strippedBI, intScale, IID_IBigDecimal, (IInterface**)value);
}

ECode BigDecimal::CompareTo(
    /* [in] */ IInterface* other,
    /* [out] */ Integer& result)
{
    BigDecimal* otherObj = From(IBigDecimal::Probe(other));
    if (otherObj == nullptr) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer thisSign, otherSign;
    Signum(&thisSign);
    otherObj->Signum(&otherSign);

    if (thisSign == otherSign) {
        if (mScale == otherObj->mScale && mBitLength < 64 && otherObj->mBitLength < 64) {
            result = (mSmallValue < otherObj->mSmallValue) ? -1 : (mSmallValue > otherObj->mSmallValue) ? 1 : 0;
            return NOERROR;
        }
        Long diffScale = (Long)mScale - otherObj->mScale;
        Integer diffPrecision = ApproxPrecision() - otherObj->ApproxPrecision();
        if (diffPrecision > diffScale + 1) {
            result = thisSign;
            return NOERROR;
        }
        else if (diffPrecision < diffScale - 1) {
            result = -thisSign;
            return NOERROR;
        }
        else {
            AutoPtr<IBigInteger> thisUnscaled = GetUnscaledValue();
            AutoPtr<IBigInteger> otherUnscaled = GetUnscaledValue();
            // If any of both precision is bigger, append zeros to the shorter one
            if (diffScale < 0) {
                AutoPtr<IBigInteger> tempBI;
                thisUnscaled->Multiply(Multiplication::PowerOf10(-diffScale), &tempBI);
                thisUnscaled = std::move(tempBI);
            }
            else if (diffScale > 0) {
                AutoPtr<IBigInteger> tempBI;
                otherUnscaled->Multiply(Multiplication::PowerOf10(diffScale), &tempBI);
                otherUnscaled = std::move(tempBI);
            }
            return IComparable::Probe(thisUnscaled)->CompareTo(otherUnscaled, result);
        }
    }
    else if (thisSign < otherSign) {
        result = -1;
        return NOERROR;
    }
    else {
        result = 1;
        return NOERROR;
    }
}

ECode BigDecimal::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean& same)
{
    BigDecimal* other = From(IBigDecimal::Probe(obj));
    if (other == nullptr) {
        same = false;
        return NOERROR;
    }

    if (other == this) {
        same = true;
        return NOERROR;
    }

    if (mScale != other->mScale || mBitLength != other->mBitLength) {
        same = false;
        return NOERROR;
    }
    if (mBitLength < 64) {
        same = mSmallValue == other->mSmallValue;
        return NOERROR;
    }
    else {
        return IObject::Probe(mIntegerValue)->Equals(other->mIntegerValue, same);
    }
}

ECode BigDecimal::Min(
    /* [in] */ IBigDecimal* value,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);

    Integer comp;
    if (CompareTo(value, comp), comp <= 0) {
        *result = this;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    else {
        *result = value;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
}

ECode BigDecimal::Max(
    /* [in] */ IBigDecimal* value,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);

    Integer comp;
    if (CompareTo(value, comp), comp >= 0) {
        *result = this;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    else {
        *result = value;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
}

ECode BigDecimal::GetHashCode(
    /* [out] */ Integer& hash)
{
    if (mHashCode != 0) {
        hash = mHashCode;
        return NOERROR;
    }
    if (mBitLength < 64) {
        mHashCode = (Integer)(mSmallValue & 0xffffffff);
        mHashCode = 33 * mHashCode + (Integer)((mSmallValue >> 32) & 0xffffffff);
        mHashCode = 17 * mHashCode + mScale;
        hash = mHashCode;
        return NOERROR;
    }
    mHashCode = 17 * Object::GetHashCode(mIntegerValue) + mScale;
    hash = mHashCode;
    return NOERROR;
}

ECode BigDecimal::ToString(
    /* [out] */ String& desc)
{
    if (!mToStringImage.IsNull()) {
        desc = mToStringImage;
        return NOERROR;
    }
    if (mBitLength < 32) {
        mToStringImage = Conversion::ToDecimalScaledString(mSmallValue, mScale);
        desc = mToStringImage;
        return NOERROR;
    }
    String intString = Object::ToString(GetUnscaledValue());
    if (mScale == 0) {
        desc = intString;
        return NOERROR;
    }
    Integer sign;
    GetUnscaledValue()->Signum(&sign);
    Integer begin = (sign < 0) ? 2 : 1;
    Integer end = intString.GetLength();
    Long exponent = -(Long)mScale + end - begin;
    AutoPtr<IStringBuilder> result;
    CStringBuilder::New(IID_IStringBuilder, (IInterface**)&result);

    result->Append(intString);
    if ((mScale > 0) && (exponent >= -6)) {
        if (exponent >= 0) {
            result->Insert(end - mScale, U'.');
        }
        else {
            result->Insert(begin - 1, String("0."));
            result->Insert(begin + 1, GetCH_ZEROS(), 0, -(Integer)exponent - 1);
        }
    }
    else {
        if (end - begin >= 1) {
            result->Insert(begin, U'.');
            end++;
        }
        result->Insert(end, U'E');
        if (exponent > 0) {
            result->Insert(++end, U'+');
        }
        result->Insert(++end, StringUtils::ToString(exponent));
    }
    result->ToString(mToStringImage);
    desc = mToStringImage;
    return NOERROR;
}

ECode BigDecimal::ToEngineeringString(
    /* [out] */ String* desc)
{
    VALIDATE_NOT_NULL(desc);

    String intString = Object::ToString(GetUnscaledValue());
    if (mScale == 0) {
        *desc = intString;
        return NOERROR;
    }
    Integer sign;
    GetUnscaledValue()->Signum(&sign);
    Integer begin = (sign < 0) ? 2 : 1;
    Integer end = intString.GetLength();
    Long exponent = -(Long)mScale + end - begin;
    AutoPtr<IStringBuilder> result;
    CStringBuilder::New(IID_IStringBuilder, (IInterface**)&result);

    if ((mScale > 0) && (exponent >= -6)) {
        if (exponent >= 0) {
            result->Insert(end - mScale, U'.');
        }
        else {
            result->Insert(begin - 1, String("0."));
            result->Insert(begin + 1, GetCH_ZEROS(), 0, -(Integer)exponent - 1);
        }
    }
    else {
        Integer delta = end - begin;
        Integer rem = (Integer)(exponent % 3);

        if (rem != 0) {
            // adjust exponent so it is a multiple of three
            Integer sign;
            GetUnscaledValue()->Signum(&sign);
            if (sign == 0) {
                // zero value
                rem = (rem < 0) ? -rem : 3 - rem;
                exponent += rem;
            }
            else {
                // nonzero value
                rem = (rem < 0) ? rem + 3 : rem;
                exponent -= rem;
                begin += rem;
            }
            if (delta < 3) {
                for (Integer i = rem - delta; i > 0; i--) {
                    result->Insert(end++, U'0');
                }
            }
        }
        if (end - begin >= 1) {
            result->Insert(begin, U'.');
            end++;
        }
        if (exponent != 0) {
            result->Insert(end, U'E');
            if (exponent > 0) {
                result->Insert(++end, U'+');
            }
            result->Insert(++end, StringUtils::ToString(exponent));
        }
    }
    return result->ToString(*desc);
}

ECode BigDecimal::ToPlainString(
    /* [out] */ String* desc)
{
    VALIDATE_NOT_NULL(desc);

    String intString = Object::ToString(GetUnscaledValue());
    if (((mScale == 0) || IsZero()) && (mScale < 0)) {
        *desc = intString;
        return NOERROR;
    }
    Integer sign;
    Signum(&sign);
    Integer begin = (sign < 0) ? 1 : 0;
    Integer delta = mScale;
    // We take space for all digits, plus a possible decimal point, plus 'scale'
    AutoPtr<IStringBuilder> result;
    CStringBuilder::New(intString.GetLength() + 1 + Math::Abs(mScale),
            IID_IStringBuilder, (IInterface**)&result);

    if (begin == 1) {
        // If the number is negative, we insert a '-' character at front
        result->Append(U'-');
    }
    if (mScale > 0) {
        delta -= (intString.GetLength() - begin);
        if (delta >= 0) {
            result->Append(String("0."));
            // To append zeros after the decimal point
            for (; delta > GetCH_ZEROS().GetLength(); delta -= GetCH_ZEROS().GetLength()) {
                result->Append(GetCH_ZEROS());
            }
            result->Append(GetCH_ZEROS(), 0, delta);
            result->Append(intString.Substring(begin));
        }
        else {
            delta = begin - delta;
            result->Append(intString.Substring(begin, delta));
            result->Append(U'.');
            result->Append(intString.Substring(delta));
        }
    }
    else {
        result->Append(intString.Substring(begin));
        // To append trailing zeros
        for (; delta < -GetCH_ZEROS().GetLength(); delta += GetCH_ZEROS().GetLength()) {
            result->Append(GetCH_ZEROS());
        }
        result->Append(GetCH_ZEROS(), 0, -delta);
    }
    return result->ToString(*desc);
}

ECode BigDecimal::ToBigInteger(
    /* [out] */ IBigInteger** value)
{
    VALIDATE_NOT_NULL(value);

    if ((mScale == 0) || IsZero()) {
        GetUnscaledValue().MoveTo(value);
        return NOERROR;
    }
    else if (mScale < 0) {
        return GetUnscaledValue()->Multiply(Multiplication::PowerOf10(-(Long)mScale), value);
    }
    else {
        return GetUnscaledValue()->Divide(Multiplication::PowerOf10(mScale), value);
    }
}

ECode BigDecimal::ToBigIntegerExact(
    /* [out] */ IBigInteger** value)
{
    VALIDATE_NOT_NULL(value);

    if ((mScale == 0) || IsZero()) {
        GetUnscaledValue().MoveTo(value);
        return NOERROR;
    }
    else if (mScale < 0) {
        return GetUnscaledValue()->Multiply(Multiplication::PowerOf10(-(Long)mScale), value);
    }
    else {
        Array<IBigInteger*> integerAndFraction;
        // An optimization before do a heavy division
        if (mScale > ApproxPrecision()) {
            Logger::E("BigDecimal", "Rounding necessary");
            return E_ARITHMETIC_EXCEPTION;
        }
        Integer setBit;
        GetUnscaledValue()->GetLowestSetBit(&setBit);
        if (mScale > setBit) {
            Logger::E("BigDecimal", "Rounding necessary");
            return E_ARITHMETIC_EXCEPTION;
        }
        GetUnscaledValue()->DivideAndRemainder(
                Multiplication::PowerOf10(mScale), &integerAndFraction);
        Integer sign;
        if (integerAndFraction[1]->Signum(&sign), sign != 0) {
            // It exists a non-zero fractional part
            Logger::E("BigDecimal", "Rounding necessary");
            return E_ARITHMETIC_EXCEPTION;
        }
        *value = integerAndFraction[0];
        REFCOUNT_ADD(*value);
        return NOERROR;
    }
}

ECode BigDecimal::LongValue(
    /* [out] */ Long& value)
{
    /*
     * If scale <= -64 there are at least 64 trailing bits zero in
     * 10^(-scale). If the scale is positive and very large the long value
     * could be zero.
     */
    if ((mScale <= -64) || (mScale > ApproxPrecision())) {
        value = 0;
        return NOERROR;
    }
    else {
        AutoPtr<IBigInteger> tempBI;
        ToBigInteger(&tempBI);
        return INumber::Probe(tempBI)->LongValue(value);
    }
}

ECode BigDecimal::LongValueExact(
    /* [out] */ Long* value)
{
    VALIDATE_NOT_NULL(value);

    return ValueExact(64, value);
}

ECode BigDecimal::IntegerValue(
    /* [out] */ Integer& value)
{
    /*
     * If scale <= -32 there are at least 32 trailing bits zero in
     * 10^(-scale). If the scale is positive and very large the long value
     * could be zero.
     */
    if ((mScale <= -32) || (mScale > ApproxPrecision())) {
        value = 0;
        return NOERROR;
    }
    else {
        AutoPtr<IBigInteger> tempBI;
        ToBigInteger(&tempBI);
        return INumber::Probe(tempBI)->IntegerValue(value);
    }
}

ECode BigDecimal::IntegerValueExact(
    /* [out] */ Integer* value)
{
    VALIDATE_NOT_NULL(value);

    Long lv;
    FAIL_RETURN(ValueExact(32, &lv));
    *value = (Integer)lv;
    return NOERROR;
}

ECode BigDecimal::ShortValueExact(
    /* [out] */ Short* value)
{
    VALIDATE_NOT_NULL(value);

    Long lv;
    FAIL_RETURN(ValueExact(16, &lv));
    *value = (Short)lv;
    return NOERROR;
}

ECode BigDecimal::ByteValueExact(
    /* [out] */ Byte* value)
{
    VALIDATE_NOT_NULL(value);

    Long lv;
    FAIL_RETURN(ValueExact(8, &lv));
    *value = (Byte)lv;
    return NOERROR;
}

ECode BigDecimal::FloatValue(
    /* [out] */ Float& value)
{
    Integer sign;
    Signum(&sign);
    Float floatResult = sign;
    Long powerOfTwo = mBitLength - (Long)(mScale / LOG10_2);
    if ((powerOfTwo < -149) || (floatResult == 0.0f)) {
        // Cases which 'this' is very small
        floatResult *= 0.0f;
    }
    else if (powerOfTwo > 129) {
        // Cases which 'this' is very large
        floatResult *= IFloat::POSITIVE_INFINITY;
    }
    else {
        Double dv;
        DoubleValue(dv);
        floatResult = (Float)dv;
    }
    value = floatResult;
    return NOERROR;
}

ECode BigDecimal::DoubleValue(
    /* [out] */ Double& value)
{
    Integer sign;
    Signum(&sign);
    Integer exponent = 1076; // bias + 53
    Integer lowestSetBit;
    Integer discardedSize;
    Long powerOfTwo = mBitLength - (Long)(mScale / LOG10_2);
    Long bits; // IEEE-754 Standard
    Long tempBits; // for temporal calculations
    AutoPtr<IBigInteger> mantissa;

    if ((powerOfTwo < -1074) || (sign == 0)) {
        // Cases which 'this' is very small
        value = sign * 0.0;
        return NOERROR;
    }
    else if (powerOfTwo > 1025) {
        // Cases which 'this' is very large
        value = sign * IDouble::POSITIVE_INFINITY;
        return NOERROR;
    }
    GetUnscaledValue()->Abs(&mantissa);
    // Let be:  this = [u,s], with s > 0
    if (mScale <= 0) {
        // mantissa = abs(u) * 10^s
        AutoPtr<IBigInteger> tempBI;
        mantissa->Multiply(Multiplication::PowerOf10(-mScale), &tempBI);
        mantissa = std::move(tempBI);
    }
    else {
        Array<IBigInteger*> quotAndRem;
        AutoPtr<IBigInteger> powerOfTen = Multiplication::PowerOf10(mScale);
        Integer k = 100 - (Integer)powerOfTwo;
        Integer compRem;
        AutoPtr<IBigInteger> tempBI, bi1, bi2;

        if (k > 0) {
            /* Computing (mantissa * 2^k) , where 'k' is a enough big
             * power of '2' to can divide by 10^s */
            mantissa->ShiftLeft(k, &tempBI);
            mantissa = std::move(tempBI);
            exponent -= k;
        }
        // Computing (mantissa * 2^k) / 10^s
        mantissa->DivideAndRemainder(powerOfTen, &quotAndRem);
        // To check if the fractional part >= 0.5
        IComparable::Probe(CBigInteger::From(quotAndRem[1])->ShiftLeftOneBit())->CompareTo(powerOfTen, compRem);
        // To add two rounded bits at end of mantissa
        quotAndRem[0]->ShiftLeft(2, &bi1);
        CBigInteger::ValueOf((compRem * (compRem + 3)) / 2 + 1, &bi2);
        bi1->Add(bi2, &tempBI);
        mantissa = std::move(tempBI);
        exponent -= 2;
    }
    mantissa->GetLowestSetBit(&lowestSetBit);
    Integer bitLength;
    mantissa->BitLength(&bitLength);
    discardedSize = bitLength - 54;
    if (discardedSize > 0) {
        // mantissa = (abs(u) * 10^s) >> (n - 54)
        AutoPtr<IBigInteger> tempBI;
        mantissa->ShiftRight(discardedSize, &tempBI);
        INumber::Probe(tempBI)->LongValue(bits);
        tempBits = bits;
        // #bits = 54, to check if the discarded fraction produces a carry
        if ((((bits & 1) == 1) && (lowestSetBit < discardedSize)) ||
                ((bits & 3) == 3)) {
            bits += 2;
        }
    }
    else {
        // mantissa = (abs(u) * 10^s) << (54 - n)
        INumber::Probe(mantissa)->LongValue(bits);
        bits = bits << -discardedSize;
        tempBits = bits;
        // #bits = 54, to check if the discarded fraction produces a carry:
        if ((bits & 3) == 3) {
            bits += 2;
        }
    }
    // Testing bit 54 to check if the carry creates a new binary digit
    if ((bits & 0x40000000000000LL) == 0) {
        // To drop the last bit of mantissa (first discarded)
        bits >>= 1;
        // exponent = 2^(s-n+53+bias)
        exponent += discardedSize;
    }
    else {
        bits >>= 2;
        exponent += discardedSize + 1;
    }
    // To test if the 53-bits number fits in 'double'
    if (exponent > 2046) {
        value = sign * IDouble::POSITIVE_INFINITY;
        return NOERROR;
    }
    else if (exponent <= 0) {
        // Denormalized numbers (having exponent == 0)
        if (exponent < -53) {
            value = sign * 0.0;
            return NOERROR;
        }
        // -1076 <= exponent - bias <= -1023
        // To discard '- exponent + 1' bits
        bits = tempBits >> 1;
        tempBits = bits & (((ULong)-1LL) >> (63 + exponent));
        bits >>= (-exponent);
        // To test if after discard bits, a new carry is generated
        if (((bits & 3) == 3) || (((bits & 1) == 1) && (tempBits != 0) &&
                (lowestSetBit < discardedSize))) {
            bits += 1;
        }
        exponent = 0;
        bits >>= 1;
    }
    // Construct the 64 double bits: [sign(1), exponent(11), mantissa(52)]
    bits = (sign & 0x8000000000000000LL) | ((long)exponent << 52)
            | (bits & 0xFFFFFFFFFFFFFLL);
    value = Math::LongBitsToDouble(bits);
    return NOERROR;
}

ECode BigDecimal::Ulp(
    /* [out] */ IBigDecimal** value)
{
    VALIDATE_NOT_NULL(value);

    return ValueOf(1, mScale, value);
}

ECode BigDecimal::InplaceRound(
    /* [in] */ IMathContext* mc)
{
    Integer mcPrecision;
    mc->GetPrecision(&mcPrecision);
    if (ApproxPrecision() < mcPrecision || mcPrecision == 0) {
        return NOERROR;
    }
    Integer precision;
    Precision(&precision);
    Integer discardedPrecision = precision - mcPrecision;
    // If no rounding is necessary it returns immediately
    if (discardedPrecision <= 0) {
        return NOERROR;
    }
    // When the number is small perform an efficient rounding
    if (mBitLength < 64) {
        return SmallRound(mc, discardedPrecision);
    }
    // Getting the integer part and the discarded fraction
    AutoPtr<IBigInteger> sizeOfFraction = Multiplication::PowerOf10(discardedPrecision);
    Array<IBigInteger*> integerAndFraction;
    GetUnscaledValue()->DivideAndRemainder(sizeOfFraction, &integerAndFraction);
    Long newScale = (Long)mScale - discardedPrecision;
    Integer compRem;
    AutoPtr<IBigDecimal> tempBD;
    // If the discarded fraction is non-zero, perform rounding
    Integer sign;
    if (integerAndFraction[1]->Signum(&sign), sign != 0) {
        // To check if the discarded fraction >= 0.5
        AutoPtr<IBigInteger> tempBI, bi1;
        integerAndFraction[1]->Abs(&tempBI);
        IComparable::Probe(CBigInteger::From(tempBI)->ShiftLeftOneBit())->CompareTo(sizeOfFraction, compRem);
        // To look if there is a carry
        Boolean set;
        RoundingMode rm;
        FAIL_RETURN(RoundingBehavior((integerAndFraction[0]->TestBit(0, &set), set) ? 1 : 0,
                (integerAndFraction[1]->Signum(&sign), sign * (5 + compRem)),
                (mc->GetRoundingMode(&rm), rm), &compRem));
        if (compRem != 0) {
            tempBI = nullptr;
            CBigInteger::ValueOf(compRem, &tempBI);
            integerAndFraction[0]->Add(tempBI, &bi1);
            integerAndFraction.Set(0, bi1);
        }
        CBigDecimal::New(integerAndFraction[0], IID_IBigDecimal, (IInterface**)&tempBD);
        // If after to add the increment the precision changed, we normalize the size
        if (tempBD->Precision(&precision), precision > mcPrecision) {
            bi1 = nullptr;
            integerAndFraction[0]->Divide(CBigInteger::GetTEN(), &bi1);
            integerAndFraction.Set(0, bi1);
            newScale--;
        }
    }
    // To update all internal fields
    FAIL_RETURN(SafeLongToInteger(newScale, &mScale));
    mPrecision = mcPrecision;
    SetUnscaledValue(integerAndFraction[0]);
    return NOERROR;
}

Integer BigDecimal::CompareAbsoluteValues(
    /* [in] */ Long value1,
    /* [in] */ Long value2)
{
    // Map long values to the range -1 .. ILong::MAX_VALUE so that comparison
    // of absolute magnitude can be done using regular long arithmetics.
    // This deals correctly with ILong::MIN_VALUE, whose absolute magnitude
    // is larger than any other long value, and which is mapped to
    // ILong::MAX_VALUE here.
    // Values that only differ by sign get mapped to the same value, for
    // example both +3 and -3 get mapped to +2.
    value1 = Math::Abs(value1) - 1;
    value2 = Math::Abs(value2) - 1;
    return value1 > value2 ? 1 : (value1 < value2 ? -1 : 0);
}

Integer BigDecimal::CompareForRounding(
    /* [in] */ Long n,
    /* [in] */ Long d)
{
    Long halfD = d / 2; //  rounds towards 0
    if (n == halfD || n == -halfD) {
        // In absolute terms: Because n == halfD, we know that 2 * n + lsb == d
        // for some lsb value 0 or 1. This means that n == d/2 (result 0) if
        // lsb is 0, or n < d/2 (result -1) if lsb is 1. In either case, the
        // result is -lsb.
        // Since we're calculating in absolute terms, we need the absolute lsb
        // (d & 1) as opposed to the signed lsb (d % 2) which would be -1 for
        // negative odd values of d.
        Integer lsb = (Integer) d & 1;
        return -lsb; // returns 0 or -1
    }
    else {
        // In absolute terms, either 2 * n + 1 < d (in the case of n < halfD),
        // or 2 * n > d (in the case of n > halfD).
        // In either case, comparing n against halfD gets the right result
        // -1 or +1, respectively.
        return CompareAbsoluteValues(n, halfD);
    }
}

ECode BigDecimal::SmallRound(
    /* [in] */ IMathContext* mc,
    /* [in] */ Integer discardedPrecision)
{
    Long sizeOfFraction = MathUtils::LONG_POWERS_OF_TEN[discardedPrecision];
    Long newScale = (Long)mScale - discardedPrecision;
    Long unscaledValue = mSmallValue;
    // Getting the integer part and the discarded fraction
    Long integer = unscaledValue / sizeOfFraction;
    Long fraction = unscaledValue % sizeOfFraction;
    Integer compRem;
    // If the discarded fraction is non-zero perform rounding
    if (fraction != 0) {
        // To check if the discarded fraction >= 0.5
        compRem = CompareForRounding(fraction, sizeOfFraction);
        // To look if there is a carry
        RoundingMode rm;
        Integer carry;
        FAIL_RETURN(RoundingBehavior(((Integer)integer) & 1,
                Math::Signum(fraction) * (5 + compRem),
                (mc->GetRoundingMode(&rm), rm), &carry));
        integer += carry;
        // If after to add the increment the precision changed, we normalize the size
        Integer precision;
        mc->GetPrecision(&precision);
        if (Math::Log10(Math::Abs(integer)) >= precision) {
            integer /= 10;
            newScale--;
        }
    }
    // To update all internal fields
    FAIL_RETURN(SafeLongToInteger(newScale, &mScale));
    mc->GetPrecision(&mPrecision);
    mSmallValue = integer;
    mBitLength = BitLength(integer);
    mIntegerValue = nullptr;
    return NOERROR;
}

ECode BigDecimal::RoundingBehavior(
    /* [in] */ Integer parityBit,
    /* [in] */ Integer fraction,
    /* [in] */ RoundingMode roundingMode,
    /* [out] */ Integer* carry)
{
    Integer increment = 0; // the carry after rounding

    switch (roundingMode) {
        case RoundingMode::UNNECESSARY:
            if (fraction != 0) {
                Logger::E("BigDecimal", "Rounding necessary");
                return E_ARITHMETIC_EXCEPTION;
            }
            break;
        case RoundingMode::UP:
            increment = Math::Signum(fraction);
            break;
        case RoundingMode::DOWN:
            break;
        case RoundingMode::CEILING:
            increment = Math::Max(Math::Signum(fraction), 0);
            break;
        case RoundingMode::FLOOR:
            increment = Math::Min(Math::Signum(fraction), 0);
            break;
        case RoundingMode::HALF_UP:
            if (Math::Abs(fraction) >= 5) {
                increment = Math::Signum(fraction);
            }
            break;
        case RoundingMode::HALF_DOWN:
            if (Math::Abs(fraction) > 5) {
                increment = Math::Signum(fraction);
            }
            break;
        case RoundingMode::HALF_EVEN:
            if (Math::Abs(fraction) + parityBit > 5) {
                increment = Math::Signum(fraction);
            }
            break;
    }
    *carry = increment;
    return NOERROR;
}

ECode BigDecimal::ValueExact(
    /* [in] */ Integer bitLengthOfType,
    /* [out] */ Long* value)
{
    AutoPtr<IBigInteger> bi;
    FAIL_RETURN(ToBigIntegerExact(&bi));

    Integer bitLength;
    bi->BitLength(&bitLength);
    if (bitLength < bitLengthOfType) {
        // It fits in the primitive type
        return INumber::Probe(bi)->LongValue(*value);
    }
    Logger::E("BigDecimal", "Rounding necessary");
    return E_ARITHMETIC_EXCEPTION;
}

Integer BigDecimal::ApproxPrecision()
{
    return mPrecision > 0 ? mPrecision :
            (Integer)((mBitLength - 1) * LOG10_2) + 1;
}

ECode BigDecimal::SafeLongToInteger(
    /* [in] */ Long longValue,
    /* [out] */ Integer* integerValue)
{
    if (longValue < IInteger::MIN_VALUE || longValue > IInteger::MAX_VALUE) {
        Logger::E("BigDecimal", "Out of integer range: %lld", longValue);
        return E_ARITHMETIC_EXCEPTION;
    }
    *integerValue = (Integer)longValue;
    return NOERROR;
}

ECode BigDecimal::ZeroScaledBy(
    /* [in] */ Long longScale,
    /* [out] */ IBigDecimal** result)
{
    if (longScale == (Integer)longScale) {
        return ValueOf(0, (Integer)longScale, result);
    }
    if (longScale >= 0) {
        return CBigDecimal::New(0, IInteger::MAX_VALUE, IID_IBigDecimal, (IInterface**)result);
    }
    return CBigDecimal::New(0, IInteger::MIN_VALUE, IID_IBigDecimal, (IInterface**)result);
}

AutoPtr<IBigInteger> BigDecimal::GetUnscaledValue()
{
    if (mIntegerValue == nullptr) {
        CBigInteger::ValueOf(mSmallValue, &mIntegerValue);
    }
    return mIntegerValue;
}

void BigDecimal::SetUnscaledValue(
    /* [in] */ IBigInteger* unscaledValue)
{
    mIntegerValue = unscaledValue;
    unscaledValue->BitLength(&mBitLength);
    if (mBitLength < 64) {
        INumber::Probe(unscaledValue)->LongValue(mSmallValue);
    }
}

Integer BigDecimal::BitLength(
    /* [in] */ Long smallValue)
{
    if (smallValue < 0) {
        smallValue = ~smallValue;
    }
    return 64 - Math::NumberOfLeadingZeros(smallValue);
}

Integer BigDecimal::BitLength(
    /* [in] */ Integer smallValue)
{
    if (smallValue < 0) {
        smallValue = ~smallValue;
    }
    return 32 - Math::NumberOfLeadingZeros(smallValue);
}

ECode BigDecimal::ByteValue(
    /* [out] */ Byte& value)
{
    Integer iv;
    IntegerValue(iv);
    value = (Byte)iv;
    return NOERROR;
}

ECode BigDecimal::ShortValue(
    /* [out] */ Short& value)
{
    Integer iv;
    IntegerValue(iv);
    value = (Short)iv;
    return NOERROR;
}

}
}
