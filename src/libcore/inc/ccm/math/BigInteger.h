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

#ifndef __CCM_MATH_BIGINTEGER_H__
#define __CCM_MATH_BIGINTEGER_H__

#include "ccm/core/SyncObject.h"
#include "ccm/math/BigInt.h"
#include "ccm.core.IComparable.h"
#include "ccm.core.INumber.h"
#include "ccm.io.ISerializable.h"
#include "ccm.math.IBigInteger.h"
#include "ccm.util.IRandom.h"

using ccm::core::IComparable;
using ccm::core::INumber;
using ccm::core::SyncObject;
using ccm::io::ISerializable;
using ccm::util::IRandom;

namespace ccm {
namespace math {

class BigInteger
    : public SyncObject
    , public IBigInteger
    , public INumber
    , public IComparable
    , public ISerializable
{
public:
    CCM_INTERFACE_DECL();

    ECode Constructor(
        /* [in] */ BigInt* bigInt);

    ECode Constructor(
        /* [in] */ Integer sign,
        /* [in] */ Long value);

    ECode Constructor(
        /* [in] */ Integer sign,
        /* [in] */ Integer numberLength,
        /* [in] */ const Array<Integer>& digits);

    ECode Constructor(
        /* [in] */ Integer numBits,
        /* [in] */ IRandom* random);

    ECode Constructor(
        /* [in] */ Integer bitLength,
        /* [in] */ Integer certainty,
        /* [in] */ IRandom* random);

    ECode Constructor(
        /* [in] */ const String& value);

    ECode Constructor(
        /* [in] */ const String& value,
        /* [in] */ Integer radix);

    ECode Constructor(
        /* [in] */ Integer signum,
        /* [in] */ const Array<Byte>& magnitude);

    ECode Constructor(
        /* [in] */ const Array<Byte>& value);

    AutoPtr<BigInt> GetBigInt();

    void PrepareRepresentation();

    static ECode ValueOf(
        /* [in] */ Long value,
        /* [out] */ IBigInteger** bi);

    ECode ToByteArray(
        /* [out, callee] */ Array<Byte>* array) override;

    ECode Abs(
        /* [out] */ IBigInteger** value) override;

    ECode Negate(
        /* [out] */ IBigInteger** value) override;

    ECode Add(
        /* [in] */ IBigInteger* value,
        /* [out] */ IBigInteger** result) override;

    ECode Subtract(
        /* [in] */ IBigInteger* value,
        /* [out] */ IBigInteger** result) override;

    ECode Signum(
        /* [out] */ Integer* sign) override;

    ECode ShiftRight(
        /* [in] */ Integer n,
        /* [out] */ IBigInteger** value) override;

    ECode ShiftLeft(
        /* [in] */ Integer n,
        /* [out] */ IBigInteger** value) override;

    AutoPtr<IBigInteger> ShiftLeftOneBit();

    ECode BitLength(
        /* [out] */ Integer* length) override;

    ECode TestBit(
        /* [in] */ Integer n,
        /* [out] */ Boolean* set) override;

    ECode SetBit(
        /* [in] */ Integer n,
        /* [out] */ IBigInteger** value) override;

    ECode ClearBit(
        /* [in] */ Integer n,
        /* [out] */ IBigInteger** value) override;

    ECode FlipBit(
        /* [in] */ Integer n,
        /* [out] */ IBigInteger** value) override;

    ECode GetLowestSetBit(
        /* [out] */ Integer* setBit) override;

    ECode BitCount(
        /* [out] */ Integer* count) override;

    ECode Not(
        /* [out] */ IBigInteger** value) override;

    ECode And(
        /* [in] */ IBigInteger* value,
        /* [out] */ IBigInteger** result) override;

    ECode Or(
        /* [in] */ IBigInteger* value,
        /* [out] */ IBigInteger** result) override;

    ECode Xor(
        /* [in] */ IBigInteger* value,
        /* [out] */ IBigInteger** result) override;

    ECode AndNot(
        /* [in] */ IBigInteger* value,
        /* [out] */ IBigInteger** result) override;

    ECode IntegerValue(
        /* [out] */ Integer* value) override;

    ECode LongValue(
        /* [out] */ Long* value) override;

    ECode FloatValue(
        /* [out] */ Float* value) override;

    ECode DoubleValue(
        /* [out] */ Double* value) override;

    ECode CompareTo(
        /* [in] */ IInterface* other,
        /* [out] */ Integer* result) override;

    ECode Min(
        /* [in] */ IBigInteger* value,
        /* [out] */ IBigInteger** result) override;

    ECode Max(
        /* [in] */ IBigInteger* value,
        /* [out] */ IBigInteger** result) override;

    ECode GetHashCode(
        /* [out] */ Integer* hash) override;

    ECode Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* same) override;

    ECode ToString(
        /* [out] */ String* desc) override;

    ECode ToString(
        /* [in] */ Integer radix,
        /* [out] */ String* value) override;

    ECode Gcd(
        /* [in] */ IBigInteger* value,
        /* [out] */ IBigInteger** result) override;

    ECode Multiply(
        /* [in] */ IBigInteger* value,
        /* [out] */ IBigInteger** result) override;

    ECode Pow(
        /* [in] */ Integer exp,
        /* [out] */ IBigInteger** value) override;

    ECode DivideAndRemainder(
        /* [in] */ IBigInteger* divisor,
        /* [out, callee] */ Array<IBigInteger*>* result) override;

    ECode Divide(
        /* [in] */ IBigInteger* divisor,
        /* [out] */ IBigInteger** result) override;

    ECode Remainder(
        /* [in] */ IBigInteger* divisor,
        /* [out] */ IBigInteger** result) override;

    ECode ModInverse(
        /* [in] */ IBigInteger* m,
        /* [out] */ IBigInteger** result) override;

    ECode ModPow(
        /* [in] */ IBigInteger* exponent,
        /* [in] */ IBigInteger* modulus,
        /* [out] */ IBigInteger** result) override;

    ECode Mod(
        /* [in] */ IBigInteger* m,
        /* [out] */ IBigInteger** result) override;

    ECode IsProbablePrime(
        /* [in] */ Integer certainty,
        /* [out] */ Boolean* prime) override;

    ECode NextProbablePrime(
        /* [out] */ IBigInteger** value) override;

    static ECode ProbablePrime(
        /* [in] */ Integer bitLength,
        /* [in] */ IRandom* random,
        /* [out] */ IBigInteger** value);

    static Integer MultiplyByInteger(
        /* [in] */ Array<Integer>& res,
        /* [in] */ Array<Integer>& a,
        /* [in] */ Integer aSize,
        /* [in] */ Integer factor);

    static Integer InplaceAdd(
        /* [in] */ Array<Integer>& a,
        /* [in] */ Integer aSize,
        /* [in] */ Integer addend);

    Integer GetFirstNonzeroDigit();

    ECode ByteValue(
        /* [out] */ Byte* value) override;

    ECode ShortValue(
        /* [out] */ Short* value) override;

    static AutoPtr<IBigInteger> GetZERO();

    static AutoPtr<IBigInteger> GetONE();

    static AutoPtr<IBigInteger> GetTEN();

    static AutoPtr<IBigInteger> GetMINUS_ONE();

    static Array<IBigInteger*> GetSMALL_VALUES();

private:
    static Boolean IsSmallPrime(
        /* [in] */ Integer x);

    void SetBigInt(
        /* [in] */ BigInt* bigInt);

    void SetRepresentation(
        /* [in] */ Integer sign,
        /* [in] */ Integer numberLength,
        /* [in] */ const Array<Integer>& digits);

    ECode TwosComplement(
        /* [out, callee] */ Array<Byte>* complement);

    static ECode ParseFromString(
        /* [in] */ BigInteger* bi,
        /* [in] */ const String& value,
        /* [in] */ Integer radix);

    static BigInteger* From(
        /* [in] */ IBigInteger* obj);

public:
    /** The magnitude of this in the little-endian representation. */
    Array<Integer> mDigits;

    /**
     * The length of this in measured in ints. Can be less than
     * digits.length().
     */
    Integer mNumberLength = 0;

    Integer mSign = 0;

private:
    AutoPtr<BigInt> mBigInt;

    Boolean mNativeIsValid = false;

    Boolean mIsValid = false;

    Integer mFirstNonzeroDigit = 0;

    Integer mHashCode = 0;
};

inline BigInteger* BigInteger::From(
    /* [in] */ IBigInteger* obj)
{
    return (BigInteger*)obj;
}

}
}

#endif // __CCM_MATH_BIGINTEGER_H__