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

#ifndef __COMO_MATH_BIGDECIMAL_H__
#define __COMO_MATH_BIGDECIMAL_H__

#include "como/core/SyncObject.h"
#include "como.core.IComparable.h"
#include "como.core.INumber.h"
#include "como.io.ISerializable.h"
#include "como.math.IBigDecimal.h"
#include "como.math.IBigInteger.h"
#include "como.math.IMathContext.h"

using como::core::IComparable;
using como::core::INumber;
using como::core::SyncObject;
using como::io::ISerializable;

namespace como {
namespace math {

class BigDecimal
    : public SyncObject
    , public IBigDecimal
    , public INumber
    , public IComparable
    , public ISerializable
{
public:
    COMO_INTERFACE_DECL();

    ECode Constructor(
        /* [in] */ const Array<Char>& in,
        /* [in] */ Integer offset,
        /* [in] */ Integer len);

    ECode Constructor(
        /* [in] */ const Array<Char>& in,
        /* [in] */ Integer offset,
        /* [in] */ Integer len,
        /* [in] */ IMathContext* mc);

    ECode Constructor(
        /* [in] */ const Array<Char>& in);

    ECode Constructor(
        /* [in] */ const Array<Char>& in,
        /* [in] */ IMathContext* mc);

    ECode Constructor(
        /* [in] */ const String& value);

    ECode Constructor(
        /* [in] */ const String& value,
        /* [in] */ IMathContext* mc);

    ECode Constructor(
        /* [in] */ Double value);

    ECode Constructor(
        /* [in] */ Double value,
        /* [in] */ IMathContext* mc);

    ECode Constructor(
        /* [in] */ IBigInteger* value);

    ECode Constructor(
        /* [in] */ IBigInteger* value,
        /* [in] */ IMathContext* mc);

    ECode Constructor(
        /* [in] */ IBigInteger* unscaledValue,
        /* [in] */ Integer scale);

    ECode Constructor(
        /* [in] */ IBigInteger* unscaledValue,
        /* [in] */ Integer scale,
        /* [in] */ IMathContext* mc);

    ECode Constructor(
        /* [in] */ Integer value);

    ECode Constructor(
        /* [in] */ Integer value,
        /* [in] */ IMathContext* mc);

    ECode Constructor(
        /* [in] */ Long value);

    ECode Constructor(
        /* [in] */ Long value,
        /* [in] */ IMathContext* mc);

    static ECode ValueOf(
        /* [in] */ Long unscaledValue,
        /* [in] */ Integer scale,
        /* [out] */ IBigDecimal** result);

    static ECode ValueOf(
        /* [in] */ Long unscaledValue,
        /* [out] */ IBigDecimal** result);

    static ECode ValueOf(
        /* [in] */ Double value,
        /* [out] */ IBigDecimal** result);

    ECode Add(
        /* [in] */ IBigDecimal* augend,
        /* [out] */ IBigDecimal** result) override;

    ECode Add(
        /* [in] */ IBigDecimal* augend,
        /* [in] */ IMathContext* mc,
        /* [out] */ IBigDecimal** result) override;

    ECode Subtract(
        /* [in] */ IBigDecimal* subtrahend,
        /* [out] */ IBigDecimal** result) override;

    ECode Subtract(
        /* [in] */ IBigDecimal* subtrahend,
        /* [in] */ IMathContext* mc,
        /* [out] */ IBigDecimal** result) override;

    ECode Multiply(
        /* [in] */ IBigDecimal* multiplicand,
        /* [out] */ IBigDecimal** result) override;

    ECode Multiply(
        /* [in] */ IBigDecimal* multiplicand,
        /* [in] */ IMathContext* mc,
        /* [out] */ IBigDecimal** result) override;

    ECode Divide(
        /* [in] */ IBigDecimal* divisor,
        /* [in] */ Integer scale,
        /* [in] */ Integer roundingMode,
        /* [out] */ IBigDecimal** result) override;

    ECode Divide(
        /* [in] */ IBigDecimal* divisor,
        /* [in] */ Integer scale,
        /* [in] */ RoundingMode roundingMode,
        /* [out] */ IBigDecimal** result) override;

    ECode Divide(
        /* [in] */ IBigDecimal* divisor,
        /* [in] */ Integer roundingMode,
        /* [out] */ IBigDecimal** result) override;

    ECode Divide(
        /* [in] */ IBigDecimal* divisor,
        /* [in] */ RoundingMode roundingMode,
        /* [out] */ IBigDecimal** result) override;

    ECode Divide(
        /* [in] */ IBigDecimal* divisor,
        /* [out] */ IBigDecimal** result) override;

    ECode Divide(
        /* [in] */ IBigDecimal* divisor,
        /* [in] */ IMathContext* mc,
        /* [out] */ IBigDecimal** result) override;

    ECode DivideToIntegralValue(
        /* [in] */ IBigDecimal* divisor,
        /* [out] */ IBigDecimal** result) override;

    ECode DivideToIntegralValue(
        /* [in] */ IBigDecimal* divisor,
        /* [in] */ IMathContext* mc,
        /* [out] */ IBigDecimal** result) override;

    ECode Remainder(
        /* [in] */ IBigDecimal* divisor,
        /* [out] */ IBigDecimal** result) override;

    ECode Remainder(
        /* [in] */ IBigDecimal* divisor,
        /* [in] */ IMathContext* mc,
        /* [out] */ IBigDecimal** result) override;

    ECode DivideAndRemainder(
        /* [in] */ IBigDecimal* divisor,
        /* [out, callee] */ Array<IBigDecimal*>* quotAndRem) override;

    ECode DivideAndRemainder(
        /* [in] */ IBigDecimal* divisor,
        /* [in] */ IMathContext* mc,
        /* [out, callee] */ Array<IBigDecimal*>* quotAndRem) override;

    ECode Pow(
        /* [in] */ Integer n,
        /* [out] */ IBigDecimal** value) override;

    ECode Pow(
        /* [in] */ Integer n,
        /* [in] */ IMathContext* mc,
        /* [out] */ IBigDecimal** value) override;

    ECode Abs(
        /* [out] */ IBigDecimal** value) override;

    ECode Abs(
        /* [in] */ IMathContext* mc,
        /* [out] */ IBigDecimal** value) override;

    ECode Negate(
        /* [out] */ IBigDecimal** value) override;

    ECode Negate(
        /* [in] */ IMathContext* mc,
        /* [out] */ IBigDecimal** result) override;

    ECode Plus(
        /* [out] */ IBigDecimal** value) override;

    ECode Plus(
        /* [in] */ IMathContext* mc,
        /* [out] */ IBigDecimal** result) override;

    ECode Signum(
        /* [out] */ Integer* sign) override;

    ECode Scale(
        /* [out] */ Integer* scale) override;

    ECode Precision(
        /* [out] */ Integer* precision) override;

    ECode UnscaledValue(
        /* [out] */ IBigInteger** value) override;

    ECode Round(
        /* [in] */ IMathContext* mc,
        /* [out] */ IBigDecimal** result) override;

    ECode SetScale(
        /* [in] */ Integer newScale,
        /* [in] */ RoundingMode roundingMode,
        /* [out] */ IBigDecimal** result) override;

    ECode SetScale(
        /* [in] */ Integer newScale,
        /* [in] */ Integer roundingMode,
        /* [out] */ IBigDecimal** result) override;

    ECode SetScale(
        /* [in] */ Integer newScale,
        /* [out] */ IBigDecimal** value) override;

    ECode MovePointLeft(
        /* [in] */ Integer n,
        /* [out] */ IBigDecimal** value) override;

    ECode MovePointRight(
        /* [in] */ Integer n,
        /* [out] */ IBigDecimal** value) override;

    ECode ScaleByPowerOfTen(
        /* [in] */ Integer n,
        /* [out] */ IBigDecimal** value) override;

    ECode StripTrailingZeros(
        /* [out] */ IBigDecimal** value) override;

    ECode CompareTo(
        /* [in] */ IInterface* other,
        /* [out] */ Integer& result) override;

    ECode Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean& same) override;

    ECode Min(
        /* [in] */ IBigDecimal* value,
        /* [out] */ IBigDecimal** result) override;

    ECode Max(
        /* [in] */ IBigDecimal* value,
        /* [out] */ IBigDecimal** result) override;

    ECode GetHashCode(
        /* [out] */ Integer& hash) override;

    ECode ToString(
        /* [out] */ String& desc) override;

    ECode ToEngineeringString(
        /* [out] */ String* desc) override;

    ECode ToPlainString(
        /* [out] */ String* desc) override;

    ECode ToBigInteger(
        /* [out] */ IBigInteger** value) override;

    ECode ToBigIntegerExact(
        /* [out] */ IBigInteger** value) override;

    ECode LongValue(
        /* [out] */ Long& value) override;

    ECode LongValueExact(
        /* [out] */ Long* value) override;

    ECode IntegerValue(
        /* [out] */ Integer& value) override;

    ECode IntegerValueExact(
        /* [out] */ Integer* value) override;

    ECode ShortValueExact(
        /* [out] */ Short* value) override;

    ECode ByteValueExact(
        /* [out] */ Byte* value) override;

    ECode FloatValue(
        /* [out] */ Float& value) override;

    ECode DoubleValue(
        /* [out] */ Double& value) override;

    ECode Ulp(
        /* [out] */ IBigDecimal** value) override;

    ECode ByteValue(
        /* [out] */ Byte& value) override;

    ECode ShortValue(
        /* [out] */ Short& value) override;

    static AutoPtr<IBigDecimal> GetZERO();

    static AutoPtr<IBigDecimal> GetONE();

    static AutoPtr<IBigDecimal> GetTEN();

protected:
    ECode Constructor(
        /* [in] */ Long smallValue,
        /* [in] */ Integer scale);

    ECode Constructor(
        /* [in] */ Integer smallValue,
        /* [in] */ Integer scale);

private:
    static ECode AddAndMult10(
        /* [in] */ BigDecimal* thisValue,
        /* [in] */ BigDecimal* augend,
        /* [in] */ Integer diffScale,
        /* [out] */ IBigDecimal** result);

    static ECode DivideBigIntegers(
        /* [in] */ IBigInteger* scaledDividend,
        /* [in] */ IBigInteger* scaledDivisor,
        /* [in] */ Integer scale,
        /* [in] */ RoundingMode roundingMode,
        /* [out] */ IBigDecimal** result);

    static ECode DividePrimitiveLongs(
        /* [in] */ Long scaledDividend,
        /* [in] */ Long scaledDivisor,
        /* [in] */ Integer scale,
        /* [in] */ RoundingMode roundingMode,
        /* [out] */ IBigDecimal** result);

    Boolean IsZero();

    Integer DecimalDigitsInLong(
        /* [in] */ Long value);

    ECode MovePoint(
        /* [in] */ Long newScale,
        /* [out] */ IBigDecimal** value);

    ECode InplaceRound(
        /* [in] */ IMathContext* mc);

    static Integer CompareAbsoluteValues(
        /* [in] */ Long value1,
        /* [in] */ Long value2);

    static Integer CompareForRounding(
        /* [in] */ Long n,
        /* [in] */ Long d);

    ECode SmallRound(
        /* [in] */ IMathContext* mc,
        /* [in] */ Integer discardedPrecision);

    static ECode RoundingBehavior(
        /* [in] */ Integer parityBit,
        /* [in] */ Integer fraction,
        /* [in] */ RoundingMode roundingMode,
        /* [out] */ Integer* carry);

    ECode ValueExact(
        /* [in] */ Integer bitLengthOfType,
        /* [out] */ Long* value);

    Integer ApproxPrecision();

    static ECode SafeLongToInteger(
        /* [in] */ Long longValue,
        /* [out] */ Integer* integerValue);

    static ECode ZeroScaledBy(
        /* [in] */ Long longScale,
        /* [out] */ IBigDecimal** result);

    AutoPtr<IBigInteger> GetUnscaledValue();

    void SetUnscaledValue(
        /* [in] */ IBigInteger* unscaledValue);

    static Integer BitLength(
        /* [in] */ Long smallValue);

    static Integer BitLength(
        /* [in] */ Integer smallValue);

    static const Array<IBigInteger*>& GetFIVE_POW();

    static const Array<IBigInteger*>& GetTEN_POW();

    static const Array<Long>& GetLONG_FIVE_POW();

    static const Array<Integer>& GetLONG_FIVE_POW_BIT_LENGTH();

    static const Array<Integer>& GetLONG_POWERS_OF_TEN_BIT_LENGTH();

    static const Array<IBigDecimal*>& GetBI_SCALED_BY_ZERO();

    static const Array<IBigDecimal*>& GetZERO_SCALED_BY();

    static const Array<Char>& GetCH_ZEROS();

    static BigDecimal* From(
        /* [in] */ IBigDecimal* obj);

private:
    static constexpr Double LOG10_2 = 0.3010299956639812;

    String mToStringImage;

    Integer mHashCode = 0;

    static constexpr Integer BI_SCALED_BY_ZERO_LENGTH = 11;

    AutoPtr<IBigInteger> mIntegerValue;

    Integer mBitLength = 0;

    Long mSmallValue = 0;

    Integer mScale = 0;

    Integer mPrecision = 0;
};

inline BigDecimal* BigDecimal::From(
    /* [in] */ IBigDecimal* obj)
{
    return (BigDecimal*)obj;
}

}
}

#endif // __COMO_MATH_BIGDECIMAL_H__
