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

#include "libcore.h"
#include "ccm/core/Character.h"
#include "ccm/core/CStringBuilder.h"
#include "ccm/math/BigInt.h"
#include <openssl/bn.h>
#include <openssl/crypto.h>

using ccm::core::Character;
using ccm::core::CStringBuilder;
using ccm::core::IID_IStringBuilder;
using ccm::core::IStringBuilder;
using ccm::core::E_NULL_POINTER_EXCEPTION;
using ccm::core::E_NUMBER_FORMAT_EXCEPTION;

namespace ccm {
namespace math {

struct BN_CTX_Deleter
{
    void operator()(BN_CTX* p) const {
        BN_CTX_free(p);
    }
};
typedef std::unique_ptr<BN_CTX, BN_CTX_Deleter> Unique_BN_CTX;

static BIGNUM* toBigNum(Long address)
{
    return reinterpret_cast<BIGNUM*>(static_cast<uintptr_t>(address));
}

BigInt::~BigInt()
{
    if (mBignum != 0) {
        BN_free(toBigNum(mBignum));
    }
}

void BigInt::MakeValid()
{
    if (mBignum == 0) {
        mBignum = static_cast<Long>(reinterpret_cast<uintptr_t>(BN_new()));
        if (mBignum == 0) {
            CHECK(0);
        }
    }
}

AutoPtr<BigInt> BigInt::NewBigInt()
{
    AutoPtr<BigInt> bi = new BigInt();
    bi->mBignum = static_cast<Long>(reinterpret_cast<uintptr_t>(BN_new()));
    return bi;
}

Integer BigInt::Compare(
    /* [in] */ BigInt* a,
    /* [in] */ BigInt* b)
{
    return BN_cmp(toBigNum(a->mBignum), toBigNum(b->mBignum));
}

void BigInt::PutCopy(
    /* [in] */ BigInt* from)
{
    MakeValid();
    if (!BN_copy(toBigNum(mBignum), toBigNum(from->mBignum))) {
        CHECK(0);
    }
}

void BigInt::PutLongInt(
    /* [in] */ Long val)
{
    MakeValid();
    if (val >= 0) {
        PutULongInt(val, false);
    }
    else {
        PutULongInt(-val, true);
    }
}

void BigInt::PutULongInt(
    /* [in] */ Long val,
    /* [in] */ Boolean neg)
{
    MakeValid();

    uint64_t dw = val;
    BIGNUM* a = toBigNum(mBignum);

    if (!BN_set_u64(a, dw)) {
        CHECK(0);
        return;
    }

    BN_set_negative(a, neg);
}

ECode BigInt::PutDecString(
    /* [in] */ const String& original)
{
    String s;
    FAIL_RETURN(CheckString(original, 10, &s));
    MakeValid();

    BIGNUM* a = toBigNum(mBignum);
    Integer usedLen = BN_dec2bn(&a, s.string());
    if (usedLen < s.GetLength()) {
        Logger::E("BigInt", "Invalid BigInteger");
        return E_NUMBER_FORMAT_EXCEPTION;
    }
    return NOERROR;
}

ECode BigInt::PutHexString(
    /* [in] */ const String& original)
{
    String s;
    FAIL_RETURN(CheckString(original, 16, &s));
    MakeValid();

    BIGNUM* a = toBigNum(mBignum);
    Integer usedLen = BN_hex2bn(&a, s.string());
    if (usedLen < s.GetLength()) {
        Logger::E("BigInt", "Invalid BigInteger");
        return E_NUMBER_FORMAT_EXCEPTION;
    }
    return NOERROR;
}

ECode BigInt::CheckString(
    /* [in] */ const String& original,
    /* [in] */ Integer base,
    /* [out] */ String* result)
{
    String s = original;
    if (s.IsNull()) {
        Logger::E("BigInt", "s == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    // A valid big integer consists of an optional '-' or '+' followed by
    // one or more digit characters appropriate to the given base,
    // and no other characters.
    Integer charCount = s.GetLength();
    Integer i = 0;
    if (charCount > 0) {
        Char ch = s.GetChar(0);
        if (ch == '+') {
            // We support leading +, but OpenSSL doesn't, so we need to strip it.
            s = s.Substring(1);
            --charCount;
        }
        else if (ch == '-') {
            ++i;
        }
    }
    if (charCount - i == 0) {
        Logger::E("BigInt", "Invalid BigInteger");
        return E_NUMBER_FORMAT_EXCEPTION;
    }
    Boolean nonAscii = false;
    for (; i < charCount; ++i) {
        Char ch = s.GetChar(i);
        if (Character::Digit(ch, base) == -1) {
            Logger::E("BigInt", "Invalid BigInteger");
            return E_NUMBER_FORMAT_EXCEPTION;
        }
        if (ch > 128) {
            nonAscii = true;
        }
    }
    *result = nonAscii ? ToAscii(s, base) : s;
    return NOERROR;
}

String BigInt::ToAscii(
    /* [in] */ const String& s,
    /* [in] */ Integer base)
{
    Integer length = s.GetLength();
    AutoPtr<IStringBuilder> result;
    CStringBuilder::New(length, IID_IStringBuilder, (IInterface**)&result);
    for (Integer i = 0; i < length; ++i) {
        Char ch = s.GetChar(i);
        Integer value = Character::Digit(ch, base);
        if (value >= 0 && value <= 9) {
            ch = (Char)('0' + value);
        }
        result->AppendChar(ch);
    }
    String str;
    result->ToString(&str);
    return str;
}

void BigInt::PutBigEndian(
    /* [in] */ const Array<Byte>& a,
    /* [in] */ Boolean neg)
{
    MakeValid();
    if (a.IsNull()) {
        return;
    }

    BIGNUM* bn = toBigNum(mBignum);
    if (!BN_bin2bn(reinterpret_cast<const unsigned char*>(a.GetPayload()),
            a.GetLength(), bn)) {
        CHECK(0);
        return;
    }

    BN_set_negative(bn, neg);
}

void BigInt::PutLittleEndianIntegers(
    /* [in] */ const Array<Integer>& a,
    /* [in] */ Boolean neg)
{
    MakeValid();
    if (a.IsNull()) {
        return;
    }

    BIGNUM* bn = toBigNum(mBignum);
    // We can simply interpret the little-endian integer stream as a
    // little-endian byte stream and use BN_le2bn.
    const uint8_t* tmpBytes = reinterpret_cast<const uint8_t*>(a.GetPayload());
    size_t numBytes = a.GetLength() * sizeof(Integer);

    if (!BN_le2bn(tmpBytes, numBytes, bn)) {
        CHECK(0);
        return;
    }

    BN_set_negative(bn, neg);
}

void BigInt::PutBigEndianTwosComplement(
    /* [in] */ const Array<Byte>& a)
{
    MakeValid();
    if (a.IsNull()) {
        return;
    }

    BIGNUM* bn = toBigNum(mBignum);
    int bytesLen = a.GetLength();

    if (bytesLen == 0) {
        BN_zero(bn);
        return;
    }

    const unsigned char* bytesTmp =  reinterpret_cast<const unsigned char*>(a.GetPayload());

    if (!BN_bin2bn(bytesTmp, bytesLen, bn)) {
        CHECK(0);
        return;
    }

    // Use the high bit to determine the sign in twos-complement.
    BN_set_negative(bn, (a[0] & 0x80) != 0);

    if (BN_is_negative(bn)) {
        // For negative values, BN_bin2bn doesn't interpret the twos-complement
        // representation, so ret is now (- value - 2^N). We can use nnmod_pow2 to set
        // ret to (-value).
        if (!BN_nnmod_pow2(bn, bn, bytesLen * 8)) {
            CHECK(0);
            return;
        }

        // And now we correct the sign.
        BN_set_negative(bn, 1);
    }
}

Long BigInt::LongInt()
{
    BIGNUM* a = toBigNum(mBignum);
    uint64_t word;

    if (BN_get_u64(a, &word)) {
        return BN_is_negative(a) ? -((Long) word) : word;
    }
    else {
        // This should be unreachable if our caller checks BigInt::twosCompFitsIntoBytes(8)
        CHECK(0);
        return 0;
    }
}

static char* leadingZerosTrimmed(char* s)
{
    char* p = s;
    if (*p == '-') {
        p++;
        while ((*p == '0') && (*(p + 1) != 0)) { p++; }
        p--;
        *p = '-';
    }
    else {
        while ((*p == '0') && (*(p + 1) != 0)) { p++; }
    }
    return p;
}

String BigInt::DecString()
{
    char* tmpStr = BN_bn2dec(toBigNum(mBignum));
    if (tmpStr == nullptr) {
        CHECK(0);
        return String(nullptr);
    }
    char* retStr = leadingZerosTrimmed(tmpStr);
    String returnString(retStr);
    OPENSSL_free(tmpStr);
    return returnString;
}

String BigInt::HexString()
{
    char* tmpStr = BN_bn2hex(toBigNum(mBignum));
    if (tmpStr == nullptr) {
        CHECK(0);
        return String(nullptr);
    }
    char* retStr = leadingZerosTrimmed(tmpStr);
    String returnString(retStr);
    OPENSSL_free(tmpStr);
    return returnString;
}

Array<Byte> BigInt::BigEndianMagnitude()
{
    BIGNUM* a = toBigNum(mBignum);
    Array<Byte> result(BN_num_bytes(a));
    BN_bn2bin(a, reinterpret_cast<unsigned char*>(result.GetPayload()));
    return result;
}

Array<Integer> BigInt::LittleEndianIntegersMagnitude()
{
    BIGNUM* a = toBigNum(mBignum);

    // The number of integers we need is BN_num_bytes(a) / sizeof(int), rounded up
    Integer intLen = (BN_num_bytes(a) + sizeof(Integer) - 1) / sizeof(Integer);

    Array<Integer> result(intLen);

    // We can simply interpret a little-endian byte stream as a little-endian integer stream.
    if (!BN_bn2le_padded(reinterpret_cast<uint8_t*>(result.GetPayload()), intLen * sizeof(Integer), a)) {
        CHECK(0);
        return Array<Integer>::Null();
    }

    return result;
}

Integer BigInt::Sign()
{
    BIGNUM* a = toBigNum(mBignum);
    if (BN_is_zero(a)) {
        return 0;
    }
    else if (BN_is_negative(a)) {
        return -1;
    }
    return 1;
}

void BigInt::SetSign(
    /* [in] */ Integer val)
{
    BIGNUM* a = toBigNum(mBignum);
    if (val > 0) {
        BN_set_negative(a, 0);
    }
    else if (val < 0) {
        BN_set_negative(a, 1);
    }
}

Integer BigInt::BitLength()
{
    BIGNUM* a = toBigNum(mBignum);

    // If a is not negative, we can use BN_num_bits directly.
    if (!BN_is_negative(a)) {
        return BN_num_bits(a);
    }

    // In the negative case, the number of bits in a is the same as the number of bits in |a|,
    // except one less when |a| is a power of two.
    BIGNUM positiveA;
    BN_init(&positiveA);

    if (!BN_copy(&positiveA, a)) {
        BN_free(&positiveA);
        return -1;
    }

    BN_set_negative(&positiveA, false);
    int numBits = BN_is_pow2(&positiveA) ? BN_num_bits(&positiveA) - 1 : BN_num_bits(&positiveA);

    BN_free(&positiveA);
    return numBits;
}

Boolean BigInt::IsBitSet(
    /* [in] */ Integer n)
{
    BIGNUM* a = toBigNum(mBignum);

    return BN_is_bit_set(a, n) ? true : false;
}

AutoPtr<BigInt> BigInt::Shift(
    /* [in] */ BigInt* a,
    /* [in] */ Integer n)
{
    AutoPtr<BigInt> r = NewBigInt();

    BIGNUM* bnR = toBigNum(r->mBignum);
    BIGNUM* bnA = toBigNum(a->mBignum);
    int ok;
    if (n >= 0) {
        ok = BN_lshift(bnR, bnA, n);
    }
    else {
        ok = BN_rshift(bnR, bnA, -n);
    }
    if (!ok) {
        CHECK(0);
    }
    return r;
}

void BigInt::Shift(
    /* [in] */ Integer n)
{
    BIGNUM* bnR = toBigNum(mBignum);
    BIGNUM* bnA = toBigNum(mBignum);
    int ok;
    if (n >= 0) {
        ok = BN_lshift(bnR, bnA, n);
    }
    else {
        ok = BN_rshift(bnR, bnA, -n);
    }
    if (!ok) {
        CHECK(0);
    }
}

void BigInt::AddPositiveInteger(
    /* [in] */ Integer w)
{
    BIGNUM* a = toBigNum(mBignum);
    if (!BN_add_word(a, w)) {
        CHECK(0);
    }
}

void BigInt::MultiplyByPositiveInteger(
    /* [in] */ Integer w)
{
    BIGNUM* a = toBigNum(mBignum);
    if (!BN_mul_word(a, w)) {
        CHECK(0);
    }
}

Integer BigInt::RemainderByPositiveInteger(
    /* [in] */ BigInt* a,
    /* [in] */ Integer w)
{
    BIGNUM* bnA = toBigNum(a->mBignum);
    BN_ULONG result = BN_mod_word(bnA, w);
    if (result == (BN_ULONG)-1) {
        CHECK(0);
    }
    return result;
}

AutoPtr<BigInt> BigInt::Addition(
    /* [in] */ BigInt* a,
    /* [in] */ BigInt* b)
{
    AutoPtr<BigInt> r = NewBigInt();
    BIGNUM* bnR = toBigNum(r->mBignum);
    BIGNUM* bnA = toBigNum(a->mBignum);
    BIGNUM* bnB = toBigNum(b->mBignum);
    if (!BN_add(bnR, bnA, bnB)) {
        CHECK(0);
    }
    return r;
}

void BigInt::Add(
    /* [in] */ BigInt* a)
{
    BIGNUM* bnR = toBigNum(mBignum);
    BIGNUM* bnA = toBigNum(a->mBignum);
    if (!BN_add(bnR, bnR, bnA)) {
        CHECK(0);
    }
}

AutoPtr<BigInt> BigInt::Subtraction(
    /* [in] */ BigInt* a,
    /* [in] */ BigInt* b)
{
    AutoPtr<BigInt> r = NewBigInt();
    BIGNUM* bnR = toBigNum(r->mBignum);
    BIGNUM* bnA = toBigNum(a->mBignum);
    BIGNUM* bnB = toBigNum(b->mBignum);
    if (!BN_sub(bnR, bnA, bnB)) {
        CHECK(0);
    }
    return r;
}

AutoPtr<BigInt> BigInt::Gcd(
    /* [in] */ BigInt* a,
    /* [in] */ BigInt* b)
{
    AutoPtr<BigInt> r = NewBigInt();
    BIGNUM* bnR = toBigNum(r->mBignum);
    BIGNUM* bnA = toBigNum(a->mBignum);
    BIGNUM* bnB = toBigNum(b->mBignum);
    Unique_BN_CTX ctx(BN_CTX_new());
    if (!BN_gcd(bnR, bnA, bnB, ctx.get())) {
        CHECK(0);
    }
    return r;
}

AutoPtr<BigInt> BigInt::Product(
    /* [in] */ BigInt* a,
    /* [in] */ BigInt* b)
{
    AutoPtr<BigInt> r = NewBigInt();
    BIGNUM* bnR = toBigNum(r->mBignum);
    BIGNUM* bnA = toBigNum(a->mBignum);
    BIGNUM* bnB = toBigNum(b->mBignum);
    Unique_BN_CTX ctx(BN_CTX_new());
    if (!BN_mul(bnR, bnA, bnB, ctx.get())) {
        CHECK(0);
    }
    return r;
}

AutoPtr<BigInt> BigInt::BigExp(
    /* [in] */ BigInt* a,
    /* [in] */ BigInt* p)
{
    // Sign of p is ignored!
    AutoPtr<BigInt> r = NewBigInt();
    BIGNUM* bnR = toBigNum(r->mBignum);
    BIGNUM* bnA = toBigNum(a->mBignum);
    BIGNUM* bnP = toBigNum(p->mBignum);
    Unique_BN_CTX ctx(BN_CTX_new());
    if (!BN_exp(bnR, bnA, bnP, ctx.get())) {
        CHECK(0);
    }
    return r;
}

AutoPtr<BigInt> BigInt::Exp(
    /* [in] */ BigInt* a,
    /* [in] */ Integer p)
{
    // Sign of p is ignored!
    AutoPtr<BigInt> power = new BigInt();
    power->PutLongInt(p);
    return BigExp(a, power);
}

void BigInt::Division(
    /* [in] */ BigInt* dividend,
    /* [in] */ BigInt* divisor,
    /* [in] */ BigInt* quotient,
    /* [in] */ BigInt* remainder)
{
    Long quot, rem;
    if (quotient != nullptr) {
        quotient->MakeValid();
        quot = quotient->mBignum;
    }
    else {
        quot = 0;
    }
    if (remainder != nullptr) {
        remainder->MakeValid();
        rem = remainder->mBignum;
    }
    else {
        rem = 0;
    }

    BIGNUM* bnQ = toBigNum(quot);
    BIGNUM* bnR = toBigNum(rem);
    BIGNUM* bnDD = toBigNum(dividend->mBignum);
    BIGNUM* bnDR = toBigNum(divisor->mBignum);
    Unique_BN_CTX ctx(BN_CTX_new());
    if (!BN_div(bnQ, bnR, bnDD, bnDR, ctx.get())) {
        CHECK(0);
    }
}

AutoPtr<BigInt> BigInt::Modulus(
    /* [in] */ BigInt* a,
    /* [in] */ BigInt* m)
{
    AutoPtr<BigInt> r = NewBigInt();
    BIGNUM* bnR = toBigNum(r->mBignum);
    BIGNUM* bnA = toBigNum(a->mBignum);
    BIGNUM* bnM = toBigNum(m->mBignum);
    Unique_BN_CTX ctx(BN_CTX_new());
    if (!BN_nnmod(bnR, bnA, bnM, ctx.get())) {
        CHECK(0);
    }
    return r;
}

AutoPtr<BigInt> BigInt::ModExp(
    /* [in] */ BigInt* a,
    /* [in] */ BigInt* p,
    /* [in] */ BigInt* m)
{
    AutoPtr<BigInt> r = NewBigInt();
    BIGNUM* bnR = toBigNum(r->mBignum);
    BIGNUM* bnA = toBigNum(a->mBignum);
    BIGNUM* bnP = toBigNum(p->mBignum);
    BIGNUM* bnM = toBigNum(m->mBignum);
    Unique_BN_CTX ctx(BN_CTX_new());
    if (!BN_mod_exp(bnR, bnA, bnP, bnM, ctx.get())) {
        CHECK(0);
    }
    return r;
}

AutoPtr<BigInt> BigInt::ModInverse(
    /* [in] */ BigInt* a,
    /* [in] */ BigInt* m)
{
    AutoPtr<BigInt> r = NewBigInt();
    BIGNUM* bnR = toBigNum(r->mBignum);
    BIGNUM* bnA = toBigNum(a->mBignum);
    BIGNUM* bnM = toBigNum(m->mBignum);
    Unique_BN_CTX ctx(BN_CTX_new());
    if (!BN_mod_inverse(bnR, bnA, bnM, ctx.get())) {
        CHECK(0);
    }
    return r;
}

AutoPtr<BigInt> BigInt::GeneratePrimeDefault(
    /* [in] */ Integer bitLength)
{
    AutoPtr<BigInt> r = NewBigInt();
    BIGNUM* bnR = toBigNum(r->mBignum);
    if (!BN_generate_prime_ex(bnR, bitLength, false, 0, 0, nullptr)) {
        CHECK(0);
    }
    return r;
}

Boolean BigInt::IsPrime(
    /* [in] */ Integer certainty)
{
    BIGNUM* a = toBigNum(mBignum);
    Unique_BN_CTX ctx(BN_CTX_new());
    int is_probably_prime;
    if (!BN_primality_test(&is_probably_prime, a, certainty, ctx.get(), false, nullptr)) {
        CHECK(0);
        return false;
    }
    return is_probably_prime ? true : false;
}

}
}
