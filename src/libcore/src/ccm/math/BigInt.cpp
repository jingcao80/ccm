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

#include "ccm/math/BigInt.h"
#include <openssl/bn.h>
#include <openssl/crypto.h>

namespace ccm {
namespace math {

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
        return String(nullptr);
    }
    char* retStr = leadingZerosTrimmed(tmpStr);
    String returnString(retStr);
    OPENSSL_free(tmpStr);
    return returnString;
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

}
}
