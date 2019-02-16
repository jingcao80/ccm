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

#include "ccm/core/AutoLock.h"
#include "ccm/core/Character.h"
#include "ccm/core/Math.h"
#include "ccm/core/StringUtils.h"
#include "ccm/math/BigInteger.h"
#include "ccm/math/BitLevel.h"
#include "ccm/math/CBigInteger.h"
#include "ccm/math/Conversion.h"
#include "ccm/math/Logical.h"
#include "ccm/math/Primality.h"
#include <ccmlogger.h>

using ccm::core::AutoLock;
using ccm::core::Character;
using ccm::core::E_ARITHMETIC_EXCEPTION;
using ccm::core::E_ASSERTION_ERROR;
using ccm::core::E_NULL_POINTER_EXCEPTION;
using ccm::core::E_NUMBER_FORMAT_EXCEPTION;
using ccm::core::IID_IComparable;
using ccm::core::IID_INumber;
using ccm::core::Math;
using ccm::core::StringUtils;
using ccm::io::IID_ISerializable;

namespace ccm {
namespace math {

static AutoPtr<IBigInteger> CreateBigInteger(
    /* [in] */ Integer sign,
    /* [in] */ Long value)
{
    AutoPtr<IBigInteger> bi;
    CBigInteger::New(sign, value, IID_IBigInteger, (IInterface**)&bi);
    return bi;
}

AutoPtr<IBigInteger> BigInteger::GetZERO()
{
    static const AutoPtr<IBigInteger> ZERO = CreateBigInteger(0, 0);
    return ZERO;
}

AutoPtr<IBigInteger> BigInteger::GetONE()
{
    static const AutoPtr<IBigInteger> ONE = CreateBigInteger(1, 1);
    return ONE;
}

AutoPtr<IBigInteger> BigInteger::GetTEN()
{
    static const AutoPtr<IBigInteger> TEN = CreateBigInteger(1, 10);
    return TEN;
}

AutoPtr<IBigInteger> BigInteger::GetMINUS_ONE()
{
    static const AutoPtr<IBigInteger> MINUS_ONE = CreateBigInteger(-1, 1);
    return MINUS_ONE;
}

static Array<IBigInteger*> CreateValues()
{
    Array<IBigInteger*> values {
        BigInteger::GetZERO(), BigInteger::GetONE(),
        CreateBigInteger(1, 2), CreateBigInteger(1, 3),
        CreateBigInteger(1, 4), CreateBigInteger(1, 5),
        CreateBigInteger(1, 6), CreateBigInteger(1, 7),
        CreateBigInteger(1, 8), CreateBigInteger(1, 9),
        BigInteger::GetTEN() };

    return values;
}

const Array<IBigInteger*>& BigInteger::GetSMALL_VALUES()
{
    static const Array<IBigInteger*> SMALL_VALUES = CreateValues();
    return SMALL_VALUES;
}

CCM_INTERFACE_IMPL_4(BigInteger, SyncObject, IBigInteger, INumber, IComparable, ISerializable);

ECode BigInteger::Constructor(
    /* [in] */ BigInt* bigInt)
{
    if (bigInt == nullptr || bigInt->GetNativeBIGNUM() == 0) {
        return E_ASSERTION_ERROR;
    }
    SetBigInt(bigInt);
    return NOERROR;
}

ECode BigInteger::Constructor(
    /* [in] */ Integer sign,
    /* [in] */ Long value)
{
    AutoPtr<BigInt> bigInt = new BigInt();
    bigInt->PutULongInt(value, (sign < 0));
    SetBigInt(bigInt);
    return NOERROR;
}

ECode BigInteger::Constructor(
    /* [in] */ Integer sign,
    /* [in] */ Integer numberLength,
    /* [in] */ const Array<Integer>& digits)
{
    SetRepresentation(sign, numberLength, digits);
    return NOERROR;
}

ECode BigInteger::Constructor(
    /* [in] */ Integer numBits,
    /* [in] */ IRandom* random)
{
    if (numBits < 0) {
        Logger::E("BigInteger", "numBits < 0: %d", numBits);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (numBits == 0) {
        Array<Integer> digits(1);
        digits[0] = 0;
        SetRepresentation(0, 1, digits);
    }
    else {
        Integer sign = 1;
        Integer numberLength = (numBits + 31) >> 5;
        Array<Integer> digits(numberLength);
        for (Integer i = 0; i < numberLength; i++) {
            Integer r;
            random->NextInteger(&r);
            digits[i] = r;
        }
        // Clear any extra bits.
        digits[numberLength - 1] = ((unsigned Integer)digits[numberLength - 1]) >> ((-numBits) & 31);
        SetRepresentation(sign, numberLength, digits);
    }
    mIsValid = true;
    return NOERROR;
}

ECode BigInteger::Constructor(
    /* [in] */ Integer bitLength,
    /* [in] */ Integer certainty,
    /* [in] */ IRandom* random)
{
    if (bitLength < 2) {
        Logger::E("BigInteger", "bitLength < 2: %d", bitLength);
        return E_ARITHMETIC_EXCEPTION;
    }
    if (bitLength < 16) {
        // We have to generate short primes ourselves, because OpenSSL bottoms out at 16 bits.
        Integer candidate;
        do {
            Integer r;
            random->NextInteger(&r);
            candidate = r & ((1 << bitLength) - 1);
            candidate |= (1 << (bitLength - 1)); // Set top bit.
            if (bitLength > 2) {
                candidate |= 1; // Any prime longer than 2 bits must have the bottom bit set.
            }
        } while (!IsSmallPrime(candidate));
        AutoPtr<BigInt> prime = new BigInt();
        prime->PutULongInt(candidate, false);
        SetBigInt(prime);
    }
    else {
        Integer bitLen;
        // We need a loop here to work around an OpenSSL bug; http://b/8588028.
        do {
            SetBigInt(BigInt::GeneratePrimeDefault(bitLength));
        } while (BitLength(&bitLen), bitLen != bitLength);
    }
    return NOERROR;
}

Boolean BigInteger::IsSmallPrime(
    /* [in] */ Integer x)
{
    if (x == 2) {
        return true;
    }
    if ((x % 2) == 0) {
        return false;
    }
    Integer max = (Integer)Math::Sqrt(x);
    for (Integer i = 3; i <= max; i += 2) {
        if ((x % i) == 0) {
            return false;
        }
    }
    return true;
}

ECode BigInteger::Constructor(
    /* [in] */ const String& value)
{
    AutoPtr<BigInt> bigInt = new BigInt();
    FAIL_RETURN(bigInt->PutDecString(value));
    SetBigInt(bigInt);
    return NOERROR;
}

ECode BigInteger::Constructor(
    /* [in] */ const String& value,
    /* [in] */ Integer radix)
{
    if (value.IsNull()) {
        Logger::E("BigInteger", "value is null");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (radix == 10) {
        AutoPtr<BigInt> bigInt = new BigInt();
        FAIL_RETURN(bigInt->PutDecString(value));
        SetBigInt(bigInt);
        return NOERROR;
    }
    else if (radix == 16) {
        AutoPtr<BigInt> bigInt = new BigInt();
        FAIL_RETURN(bigInt->PutHexString(value));
        SetBigInt(bigInt);
        return NOERROR;
    }
    else {
        if (radix < Character::MIN_RADIX || radix > Character::MAX_RADIX) {
            Logger::E("BigInteger", "Invalid radix: %d", radix);
            return E_NUMBER_FORMAT_EXCEPTION;
        }
        if (value.IsEmpty()) {
            Logger::E("BigInteger", "value.isEmpty()");
            return E_NUMBER_FORMAT_EXCEPTION;
        }
        return ParseFromString(this, value, radix);
    }
}

ECode BigInteger::Constructor(
    /* [in] */ Integer signum,
    /* [in] */ const Array<Byte>& magnitude)
{
    if (magnitude.IsNull()) {
        Logger::E("BigInteger", "magnitude == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (signum < -1 || signum > 1) {
        Logger::E("BigInteger", "Invalid signum: %d", signum);
        return E_NUMBER_FORMAT_EXCEPTION;
    }
    if (signum == 0) {
        for (Integer i = 0; i < magnitude.GetLength(); i++) {
            Byte element = magnitude[i];
            if (element != 0) {
                Logger::E("BigInteger", "signum-magnitude mismatch");
                return E_NUMBER_FORMAT_EXCEPTION;
            }
        }
    }
    AutoPtr<BigInt> bigInt = new BigInt();
    bigInt->PutBigEndian(magnitude, signum < 0);
    SetBigInt(bigInt);
    return NOERROR;
}

ECode BigInteger::Constructor(
    /* [in] */ const Array<Byte>& value)
{
    if (value.GetLength() == 0) {
        Logger::E("BigInteger", "value.length == 0");
        return E_NUMBER_FORMAT_EXCEPTION;
    }
    AutoPtr<BigInt> bigInt = new BigInt();
    bigInt->PutBigEndianTwosComplement(value);
    SetBigInt(bigInt);
    return NOERROR;
}

AutoPtr<BigInt> BigInteger::GetBigInt()
{
    if (mNativeIsValid) {
        return mBigInt;
    }

    {
        AutoLock lock(this);
        if (mNativeIsValid) {
            return mBigInt;
        }
        AutoPtr<BigInt> bigInt = new BigInt();
        bigInt->PutLittleEndianIntegers(mDigits, (mSign < 0));
        SetBigInt(bigInt);
        return bigInt;
    }
}

void BigInteger::SetBigInt(
    /* [in] */ BigInt* bigInt)
{
    mBigInt = bigInt;
    mNativeIsValid = true;
}

void BigInteger::SetRepresentation(
    /* [in] */ Integer sign,
    /* [in] */ Integer numberLength,
    /* [in] */ const Array<Integer>& digits)
{
    // decrement numberLength to drop leading zeroes...
    while (numberLength > 0 && digits[--numberLength] == 0);
    // ... and then increment it back because we always drop one too many
    if (digits[numberLength++] == 0) {
        sign = 0;
    }
    mSign = sign;
    mDigits = digits;
    mNumberLength = numberLength;
    mIsValid = true;
}

void BigInteger::PrepareRepresentation()
{
    if (mIsValid) {
        return;
    }

    AutoLock lock(this);
    if (mIsValid) {
        return;
    }
    Integer sign = mBigInt->Sign();
    Array<Integer> digits;
    if (sign != 0) {
        digits = mBigInt->LittleEndianIntegersMagnitude();
    }
    else {
        digits = Array<Integer>(1);
        digits[0] = 0;
    }
    SetRepresentation(sign, digits.GetLength(), digits);
}

ECode BigInteger::ValueOf(
    /* [in] */ Long value,
    /* [out] */ IBigInteger** bi)
{
    VALIDATE_NOT_NULL(bi);

    if (value < 0) {
        if (value != -1) {
            return CBigInteger::New(-1, -value, IID_IBigInteger, (IInterface**)bi);
        }
        *bi = GetMINUS_ONE();
        REFCOUNT_ADD(*bi);
        return NOERROR;
    }
    else if (value < GetSMALL_VALUES().GetLength()) {
        *bi = GetSMALL_VALUES()[value];
        REFCOUNT_ADD(*bi);
        return NOERROR;
    }
    else {
        // (value > 10)
        return CBigInteger::New(1, value, IID_IBigInteger, (IInterface**)bi);
    }
}

ECode BigInteger::ToByteArray(
    /* [out, callee] */ Array<Byte>* array)
{
    return TwosComplement(array);
}

ECode BigInteger::Abs(
    /* [out] */ IBigInteger** value)
{
    VALIDATE_NOT_NULL(value);

    AutoPtr<BigInt> bigInt = GetBigInt();
    if (bigInt->Sign() >= 0) {
        *value = this;
        REFCOUNT_ADD(*value);
        return NOERROR;
    }
    AutoPtr<BigInt> a = bigInt->Copy();
    a->SetSign(1);
    return CBigInteger::New(a, IID_IBigInteger, (IInterface**)value);
}

ECode BigInteger::Negate(
    /* [out] */ IBigInteger** value)
{
    VALIDATE_NOT_NULL(value);

    AutoPtr<BigInt> bigInt = GetBigInt();
    Integer sign = bigInt->Sign();
    if (sign == 0) {
        *value = this;
        REFCOUNT_ADD(*value);
        return NOERROR;
    }
    AutoPtr<BigInt> a = bigInt->Copy();
    a->SetSign(-sign);
    return CBigInteger::New(a, IID_IBigInteger, (IInterface**)value);
}

ECode BigInteger::Add(
    /* [in] */ IBigInteger* value,
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<BigInt> lhs = GetBigInt();
    AutoPtr<BigInt> rhs = From(value)->GetBigInt();
    if (rhs->Sign() == 0) {
        *result = this;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    if (lhs->Sign() == 0) {
        *result = value;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    return CBigInteger::New(BigInt::Addition(lhs, rhs), IID_IBigInteger, (IInterface**)result);
}

ECode BigInteger::Subtract(
    /* [in] */ IBigInteger* value,
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<BigInt> lhs = GetBigInt();
    AutoPtr<BigInt> rhs = From(value)->GetBigInt();
    if (rhs->Sign() == 0) {
        *result = this;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    return CBigInteger::New(BigInt::Subtraction(lhs, rhs), IID_IBigInteger, (IInterface**)result);
}

ECode BigInteger::Signum(
    /* [out] */ Integer* sign)
{
    VALIDATE_NOT_NULL(sign);

    if (mIsValid) {
        *sign = mSign;
        return NOERROR;
    }
    *sign = GetBigInt()->Sign();
    return NOERROR;
}

ECode BigInteger::ShiftRight(
    /* [in] */ Integer n,
    /* [out] */ IBigInteger** value)
{
    return ShiftLeft(-n, value);
}

ECode BigInteger::ShiftLeft(
    /* [in] */ Integer n,
    /* [out] */ IBigInteger** value)
{
    VALIDATE_NOT_NULL(value);

    if (n == 0) {
        *value = this;
        REFCOUNT_ADD(*value);
        return NOERROR;
    }
    Integer sign;
    Signum(&sign);
    if (sign == 0) {
        *value = this;
        REFCOUNT_ADD(*value);
        return NOERROR;
    }
    if ((sign > 0) || (n >= 0)) {
        return CBigInteger::New(BigInt::Shift(GetBigInt(), n), IID_IBigInteger, (IInterface**)value);
    }
    else {
        // Negative numbers faking 2's complement
        return BitLevel::ShiftRight(this, -n, value);
    }
}

AutoPtr<IBigInteger> BigInteger::ShiftLeftOneBit()
{
    Integer sign;
    Signum(&sign);
    if (sign == 0) {
        return this;
    }
    else {
        AutoPtr<IBigInteger> bi;
        BitLevel::ShiftLeftOneBit(this, &bi);
        return bi;
    }
}

ECode BigInteger::BitLength(
    /* [out] */ Integer* length)
{
    if (!mNativeIsValid && mIsValid) {
        *length = BitLevel::BitLength(this);
        return NOERROR;
    }
    *length = GetBigInt()->BitLength();
    return NOERROR;
}

ECode BigInteger::TestBit(
    /* [in] */ Integer n,
    /* [out] */ Boolean* set)
{
    VALIDATE_NOT_NULL(set);

    if (n < 0) {
        Logger::E("BigInteger", "n < 0: %d", n);
        return E_ARITHMETIC_EXCEPTION;
    }
    Integer sign;
    Signum(&sign);
    if (sign > 0 && mNativeIsValid && !mIsValid) {
        *set = GetBigInt()->IsBitSet(n);
        return NOERROR;
    }
    else {
        // Negative numbers faking 2's complement:
        PrepareRepresentation();
        if (n == 0) {
            *set = ((mDigits[0] & 1) != 0);
            return NOERROR;
        }
        Integer intCount = n >> 5;
        if (intCount >= mNumberLength) {
            *set = sign < 0;
            return NOERROR;
        }
        Integer digit = mDigits[intCount];
        n = (1 << (n & 31)); // Integer with 1 set to the needed position
        if (sign < 0) {
            Integer firstNonZeroDigit = GetFirstNonzeroDigit();
            if (intCount < firstNonZeroDigit) {
                *set = false;
                return NOERROR;
            }
            else if (firstNonZeroDigit == intCount) {
                digit = -digit;
            }
            else {
                digit = ~digit;
            }
        }
        *set = ((digit & n) != 0);
        return NOERROR;
    }
}

ECode BigInteger::SetBit(
    /* [in] */ Integer n,
    /* [out] */ IBigInteger** value)
{
    VALIDATE_NOT_NULL(value);

    PrepareRepresentation();
    Boolean set;
    if (TestBit(n, &set), !set) {
        return BitLevel::FlipBit(this, n, value);
    }
    else {
        *value = this;
        REFCOUNT_ADD(*value);
        return NOERROR;
    }
}

ECode BigInteger::ClearBit(
    /* [in] */ Integer n,
    /* [out] */ IBigInteger** value)
{
    VALIDATE_NOT_NULL(value);

    PrepareRepresentation();
    Boolean set;
    if (TestBit(n, &set), set) {
        return BitLevel::FlipBit(this, n, value);
    }
    else {
        *value = this;
        REFCOUNT_ADD(*value);
        return NOERROR;
    }
}

ECode BigInteger::FlipBit(
    /* [in] */ Integer n,
    /* [out] */ IBigInteger** value)
{
    VALIDATE_NOT_NULL(value);

    PrepareRepresentation();
    if ( n < 0) {
        Logger::E("BigInteger", "n < 0: %d", n);
        return E_ARITHMETIC_EXCEPTION;
    }
    return BitLevel::FlipBit(this, n, value);
}

ECode BigInteger::GetLowestSetBit(
    /* [out] */ Integer* setBit)
{
    VALIDATE_NOT_NULL(setBit);

    PrepareRepresentation();
    if (mSign == 0) {
        *setBit = -1;
        return NOERROR;
    }
    // (sign != 0) implies that exists some non zero digit
    Integer i = GetFirstNonzeroDigit();
    *setBit = ((i << 5) + Math::NumberOfTrailingZeros(mDigits[i]));
    return NOERROR;
}

ECode BigInteger::BitCount(
    /* [out] */ Integer* count)
{
    VALIDATE_NOT_NULL(count);

    PrepareRepresentation();
    *count = BitLevel::BitCount(this);
    return NOERROR;
}

ECode BigInteger::Not(
    /* [out] */ IBigInteger** value)
{
    VALIDATE_NOT_NULL(value);

    PrepareRepresentation();
    return Logical::Not(this, value);
}

ECode BigInteger::And(
    /* [in] */ IBigInteger* value,
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);

    PrepareRepresentation();
    From(value)->PrepareRepresentation();
    return Logical::And(this, From(value), result);
}

ECode BigInteger::Or(
    /* [in] */ IBigInteger* value,
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);

    PrepareRepresentation();
    From(value)->PrepareRepresentation();
    return Logical::Or(this, From(value), result);
}

ECode BigInteger::Xor(
    /* [in] */ IBigInteger* value,
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);

    PrepareRepresentation();
    From(value)->PrepareRepresentation();
    return Logical::Xor(this, From(value), result);
}

ECode BigInteger::AndNot(
    /* [in] */ IBigInteger* value,
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);

    PrepareRepresentation();
    From(value)->PrepareRepresentation();
    return Logical::AndNot(this, From(value), result);
}

ECode BigInteger::IntegerValue(
    /* [out] */ Integer* value)
{
    VALIDATE_NOT_NULL(value);

    if (mNativeIsValid && mBigInt->TwosCompFitsIntoBytes(4)) {
        *value = (Integer)mBigInt->LongInt();
        return NOERROR;
    }
    PrepareRepresentation();
    *value = (mSign * mDigits[0]);
    return NOERROR;
}

ECode BigInteger::LongValue(
    /* [out] */ Long* value)
{
    VALIDATE_NOT_NULL(value);

    if (mNativeIsValid && mBigInt->TwosCompFitsIntoBytes(8)) {
        *value = mBigInt->LongInt();
        return NOERROR;
    }
    PrepareRepresentation();
    Long lv = mNumberLength > 1 ?
            ((Long) mDigits[1]) << 32 | mDigits[0] & 0xFFFFFFFFLL :
            mDigits[0] & 0xFFFFFFFFLL;
    *value = mSign * lv;
    return NOERROR;
}

ECode BigInteger::FloatValue(
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);

    Double dv;
    DoubleValue(&dv);
    *value = (Float)dv;
    return NOERROR;
}

ECode BigInteger::DoubleValue(
    /* [out] */ Double* value)
{
    VALIDATE_NOT_NULL(value);

    *value = Conversion::BigInteger2Double(this);
    return NOERROR;
}

ECode BigInteger::CompareTo(
    /* [in] */ IInterface* other,
    /* [out] */ Integer* result)
{
    VALIDATE_NOT_NULL(result);

    BigInteger* value = (BigInteger*)IBigInteger::Probe(other);
    if (value == nullptr) {
        *result = -1;
        return E_NULL_POINTER_EXCEPTION;
    }

    *result = BigInt::Compare(GetBigInt(), value->GetBigInt());
    return NOERROR;
}

ECode BigInteger::Min(
    /* [in] */ IBigInteger* value,
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);

    Integer cmp;
    FAIL_RETURN(CompareTo(value, &cmp));
    *result = cmp == -1 ? this : value;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode BigInteger::Max(
    /* [in] */ IBigInteger* value,
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);

    Integer cmp;
    FAIL_RETURN(CompareTo(value, &cmp));
    *result = cmp == 1 ? this : value;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode BigInteger::GetHashCode(
    /* [out] */ Integer* hash)
{
    VALIDATE_NOT_NULL(hash);

    if (mHashCode == 0) {
        PrepareRepresentation();
        Integer h = 0;
        for (Integer i = 0; i < mNumberLength; ++i) {
            h = h * 33 + mDigits[i];
        }
        mHashCode = h * mSign;
    }
    *hash = mHashCode;
    return NOERROR;
}

ECode BigInteger::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* same)
{
    VALIDATE_NOT_NULL(same);

    BigInteger* value = (BigInteger*)IBigInteger::Probe(obj);
    if (this == value) {
        *same = true;
        return NOERROR;
    }
    if (value == nullptr) {
        *same = false;
        return NOERROR;
    }
    Integer cmp;
    CompareTo(obj, &cmp);
    *same = (cmp == 0);
    return NOERROR;
}

ECode BigInteger::ToString(
    /* [out] */ String* desc)
{
    VALIDATE_NOT_NULL(desc);

    *desc = GetBigInt()->DecString();
    return NOERROR;
}

ECode BigInteger::ToString(
    /* [in] */ Integer radix,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);

    if (radix == 10) {
        *value = GetBigInt()->DecString();
        return NOERROR;
    }
    else {
        PrepareRepresentation();
        *value = Conversion::BigInteger2String(this, radix);
        return NOERROR;
    }
}

ECode BigInteger::Gcd(
    /* [in] */ IBigInteger* value,
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);

    return CBigInteger::New(BigInt::Gcd(
            GetBigInt(), From(value)->GetBigInt()),
            IID_IBigInteger, (IInterface**)result);
}

ECode BigInteger::Multiply(
    /* [in] */ IBigInteger* value,
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);

    return CBigInteger::New(BigInt::Product(
            GetBigInt(), From(value)->GetBigInt()),
            IID_IBigInteger, (IInterface**)result);
}

ECode BigInteger::Pow(
    /* [in] */ Integer exp,
    /* [out] */ IBigInteger** value)
{
    VALIDATE_NOT_NULL(value);

    if (exp < 0) {
        Logger::E("BigInteger", "exp < 0: %d", exp);
        return E_ARITHMETIC_EXCEPTION;
    }
    return CBigInteger::New(BigInt::Exp(GetBigInt(), exp),
            IID_IBigInteger, (IInterface**)value);
}

ECode BigInteger::DivideAndRemainder(
    /* [in] */ IBigInteger* divisor,
    /* [out, callee] */ Array<IBigInteger*>* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<BigInt> divisorBigInt = From(divisor)->GetBigInt();
    AutoPtr<BigInt> quotient = new BigInt();
    AutoPtr<BigInt> remainder = new BigInt();
    BigInt::Division(GetBigInt(), divisorBigInt, quotient, remainder);
    AutoPtr<IBigInteger> quotientBi, remainderBi;
    CBigInteger::New(quotient, IID_IBigInteger, (IInterface**)&quotientBi);
    CBigInteger::New(remainder, IID_IBigInteger, (IInterface**)&remainderBi);
    *result = Array<IBigInteger*>(2);
    result->Set(0, quotientBi);
    result->Set(1, remainderBi);
    return NOERROR;
}

ECode BigInteger::Divide(
    /* [in] */ IBigInteger* divisor,
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<BigInt> quotient = new BigInt();
    BigInt::Division(GetBigInt(), From(divisor)->GetBigInt(), quotient, nullptr);
    return CBigInteger::New(quotient, IID_IBigInteger, (IInterface**)result);
}

ECode BigInteger::Remainder(
    /* [in] */ IBigInteger* divisor,
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<BigInt> remainder = new BigInt();
    BigInt::Division(GetBigInt(), From(divisor)->GetBigInt(), nullptr, remainder);
    return CBigInteger::New(remainder, IID_IBigInteger, (IInterface**)result);
}

ECode BigInteger::ModInverse(
    /* [in] */ IBigInteger* m,
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);

    Integer sign;
    if (m->Signum(&sign), sign <= 0) {
        Logger::E("BigInteger", "modulus not positive");
        return E_ARITHMETIC_EXCEPTION;
    }
    return CBigInteger::New(BigInt::ModInverse(GetBigInt(), From(m)->GetBigInt()),
            IID_IBigInteger, (IInterface**)result);
}

ECode BigInteger::ModPow(
    /* [in] */ IBigInteger* exponent,
    /* [in] */ IBigInteger* modulus,
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);

    Integer sign;
    if (modulus->Signum(&sign), sign <= 0) {
        Logger::E("BigInteger", "modulus.signum() <= 0");
        return E_ARITHMETIC_EXCEPTION;
    }
    Integer exponentSign;
    exponent->Signum(&exponentSign);
    if (exponentSign == 0) { // OpenSSL gets this case wrong; http://b/8574367.
        return GetONE()->Mod(modulus, result);
    }
    AutoPtr<IBigInteger> base;
    if (exponentSign < 0) {
        FAIL_RETURN(ModInverse(modulus, &base));
    }
    else {
        base = this;
    }
    return CBigInteger::New(BigInt::ModExp(From(base)->GetBigInt(),
            From(exponent)->GetBigInt(), From(modulus)->GetBigInt()),
            IID_IBigInteger, (IInterface**)result);
}

ECode BigInteger::Mod(
    /* [in] */ IBigInteger* m,
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);

    Integer sign;
    if (m->Signum(&sign), sign <= 0) {
        Logger::E("BigInteger", "m.signum() <= 0");
        return E_ARITHMETIC_EXCEPTION;
    }
    return CBigInteger::New(BigInt::Modulus(GetBigInt(), From(m)->GetBigInt()),
            IID_IBigInteger, (IInterface**)result);
}

ECode BigInteger::IsProbablePrime(
    /* [in] */ Integer certainty,
    /* [out] */ Boolean* prime)
{
    VALIDATE_NOT_NULL(prime);

    if (certainty <= 0) {
        *prime = true;
        return NOERROR;
    }
    *prime = GetBigInt()->IsPrime(certainty);
    return NOERROR;
}

ECode BigInteger::NextProbablePrime(
    /* [out] */ IBigInteger** value)
{
    VALIDATE_NOT_NULL(value);

    if (mSign < 0) {
        Logger::E("BigInteger", "sign < 0");
        *value = nullptr;
        return NOERROR;
    }
    return Primality::NextProbablePrime(this, value);
}

ECode BigInteger::ProbablePrime(
    /* [in] */ Integer bitLength,
    /* [in] */ IRandom* random,
    /* [out] */ IBigInteger** value)
{
    VALIDATE_NOT_NULL(value);

    return CBigInteger::New(bitLength, 100, random, IID_IBigInteger, (IInterface**)value);
}

ECode BigInteger::TwosComplement(
    /* [out, callee] */ Array<Byte>* complement)
{
    PrepareRepresentation();
    if (mSign == 0) {
        *complement = Array<Byte>(1);
        (*complement)[0] = 0;
        return NOERROR;
    }
    BigInteger* temp = this;
    Integer bitLen;
    BitLength(&bitLen);
    Integer iThis = GetFirstNonzeroDigit();
    Integer bytesLen = (bitLen >> 3) + 1;
    /* Puts the little-endian int array representing the magnitude
     * of this BigInteger into the big-endian byte array. */
    Array<Byte> bytes(bytesLen);
    Integer firstByteNumber = 0;
    Integer highBytes;
    Integer bytesInInteger = 4;
    Integer hB;

    if (bytesLen - (mNumberLength << 2) == 1) {
        bytes[0] = (Byte)((mSign < 0) ? -1 : 0);
        highBytes = 4;
        firstByteNumber++;
    }
    else {
        hB = bytesLen & 3;
        highBytes = (hB == 0) ? 4 : hB;
    }

    Integer digitIndex = iThis;
    bytesLen -= iThis << 2;

    if (mSign < 0) {
        Integer digit = -temp->mDigits[digitIndex];
        digitIndex++;
        if (digitIndex == mNumberLength) {
            bytesInInteger = highBytes;
        }
        for (Integer i = 0; i < bytesInInteger; i++, digit >>= 8) {
            bytes[--bytesLen] = (Byte)digit;
        }
        while (bytesLen > firstByteNumber) {
            digit = ~temp->mDigits[digitIndex];
            digitIndex++;
            if (digitIndex == mNumberLength) {
                bytesInInteger = highBytes;
            }
            for (Integer i = 0; i < bytesInInteger; i++, digit >>= 8) {
                bytes[--bytesLen] = (Byte)digit;
            }
        }
    }
    else {
        while (bytesLen > firstByteNumber) {
            Integer digit = temp->mDigits[digitIndex];
            digitIndex++;
            if (digitIndex == mNumberLength) {
                bytesInInteger = highBytes;
            }
            for (Integer i = 0; i < bytesInInteger; i++, digit >>= 8) {
                bytes[--bytesLen] = (Byte)digit;
            }
        }
    }
    *complement = bytes;
    return NOERROR;
}

Integer BigInteger::MultiplyByInteger(
    /* [in] */ Array<Integer>& res,
    /* [in] */ Array<Integer>& a,
    /* [in] */ Integer aSize,
    /* [in] */ Integer factor)
{
    Long carry = 0;

    for (Integer i = 0; i < aSize; i++) {
        carry += (a[i] & 0xFFFFFFFFLL) * (factor & 0xFFFFFFFFLL);
        res[i] = (Integer)carry;
        carry = ((unsigned Long)carry) >> 32;
    }
    return (Integer)carry;
}

Integer BigInteger::InplaceAdd(
    /* [in] */ Array<Integer>& a,
    /* [in] */ Integer aSize,
    /* [in] */ Integer addend)
{
    Long carry = addend & 0xFFFFFFFFLL;

    for (Integer i = 0; (carry != 0) && (i < aSize); i++) {
        carry += a[i] & 0xFFFFFFFFLL;
        a[i] = (Integer)carry;
        carry >>= 32;
    }
    return (Integer)carry;
}

ECode BigInteger::ParseFromString(
    /* [in] */ BigInteger* bi,
    /* [in] */ const String& value,
    /* [in] */ Integer radix)
{
    Integer stringLength = value.GetLength();
    Integer endChar = stringLength;

    Integer sign;
    Integer startChar;
    if (value.GetChar(0) == '-') {
        sign = -1;
        startChar = 1;
        stringLength--;
    }
    else {
        sign = 1;
        startChar = 0;
    }

    /*
     * We use the following algorithm: split a string into portions of n
     * characters and convert each portion to an integer according to the
     * radix. Then convert an pow(radix, n) based number to binary using the
     * multiplication method. See D. Knuth, The Art of Computer Programming,
     * vol. 2.
     */

    Integer charsPerInt = Conversion::sDigitFitInInt[radix];
    Integer bigRadixDigitsLength = stringLength / charsPerInt;
    Integer topChars = stringLength % charsPerInt;

    if (topChars != 0) {
        bigRadixDigitsLength++;
    }
    Array<Integer> digits(bigRadixDigitsLength);
    // Get the maximal power of radix that fits in int
    Integer bigRadix = Conversion::sBigRadices[radix - 2];
    // Parse an input string and accumulate the BigInteger's magnitude
    Integer digitIndex = 0; // index of digits array
    Integer substrEnd = startChar + ((topChars == 0) ? charsPerInt : topChars);

    for (Integer substrStart = startChar; substrStart < endChar;
            substrStart = substrEnd, substrEnd = substrStart + charsPerInt) {
        Integer bigRadixDigit;
        FAIL_RETURN(StringUtils::ParseInteger(value.Substring(substrStart, substrEnd), radix, &bigRadixDigit));
        Integer newDigit = MultiplyByInteger(digits, digits, digitIndex, bigRadix);
        newDigit += InplaceAdd(digits, digitIndex, bigRadixDigit);
        digits[digitIndex++] = newDigit;
    }
    Integer numberLength = digitIndex;
    bi->SetRepresentation(sign, numberLength, digits);
    return NOERROR;
}

Integer BigInteger::GetFirstNonzeroDigit()
{
    if (mFirstNonzeroDigit == -2) {
        Integer i;
        if (mSign == 0) {
            i = -1;
        }
        else {
            for (i = 0; mDigits[i] == 0; i++) {
                ;
            }
        }
        mFirstNonzeroDigit = i;
    }
    return mFirstNonzeroDigit;
}

ECode BigInteger::ByteValue(
    /* [out] */ Byte* value)
{
    VALIDATE_NOT_NULL(value);

    Integer iv;
    IntegerValue(&iv);
    *value = (Byte)iv;
    return NOERROR;
}

ECode BigInteger::ShortValue(
    /* [out] */ Short* value)
{
    VALIDATE_NOT_NULL(value);

    Integer iv;
    IntegerValue(&iv);
    *value = (Short)iv;
    return NOERROR;
}

}
}
