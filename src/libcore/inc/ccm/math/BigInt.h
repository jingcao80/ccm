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

#ifndef __CCM_MATH_BIGINT_H__
#define __CCM_MATH_BIGINT_H__

#include <ccmobject.h>

namespace ccm {
namespace math {

class BigInt
    : public Object
{
public:
    ~BigInt();

    Long GetNativeBIGNUM();

    static Integer Compare(
        /* [in] */ BigInt* a,
        /* [in] */ BigInt* b);

    void PutCopy(
        /* [in] */ BigInt* from);

    AutoPtr<BigInt> Copy();

    void PutULongInt(
        /* [in] */ Long val,
        /* [in] */ Boolean neg)
    {}

    void PutDecString(
        /* [in] */ const String& original)
    {}

    void PutHexString(
        /* [in] */ const String& original)
    {}

    void PutBigEndian(
        /* [in] */ const Array<Byte>& a,
        /* [in] */ Boolean neg)
    {}

    void PutLittleEndianInts(
        /* [in] */ const Array<Integer>& a,
        /* [in] */ Boolean neg)
    {}

    void PutBigEndianTwosComplement(
        /* [in] */ const Array<Byte>& a)
    {}

    Long LongInt();

    String DecString();

    Array<Integer> LittleEndianIntsMagnitude()
    {
        return Array<Integer>::Null();
    }

    Integer Sign()
    {
        return -1;
    }

    void SetSign(
        /* [in] */ Integer val)
    {}

    Boolean TwosCompFitsIntoBytes(
        /* [in] */ Integer desiredByteCount);

    Integer BitLength();

    Boolean IsBitSet(
        /* [in] */ Integer n)
    {
        return false;
    }

    static AutoPtr<BigInt> Shift(
        /* [in] */ BigInt* a,
        /* [in] */ Integer n)
    {
        return nullptr;
    }

    void Shift(
        /* [in] */ Integer n)
    {}

    void AddPositiveInteger(
        /* [in] */ Integer w)
    {}

    static Integer RemainderByPositiveInteger(
        /* [in] */ BigInt* a,
        /* [in] */ Integer w)
    {}

    static AutoPtr<BigInt> Addition(
        /* [in] */ BigInt* a,
        /* [in] */ BigInt* b)
    {
        return nullptr;
    }

    static AutoPtr<BigInt> Subtraction(
        /* [in] */ BigInt* a,
        /* [in] */ BigInt* b)
    {
        return nullptr;
    }

    static AutoPtr<BigInt> Gcd(
        /* [in] */ BigInt* a,
        /* [in] */ BigInt* b)
    {
        return nullptr;
    }

    static AutoPtr<BigInt> Product(
        /* [in] */ BigInt* a,
        /* [in] */ BigInt* b)
    {
        return nullptr;
    }

    static AutoPtr<BigInt> Exp(
        /* [in] */ BigInt* a,
        /* [in] */ Integer p)
    {
        return nullptr;
    }

    static void Division(
        /* [in] */ BigInt* dividend,
        /* [in] */ BigInt* divisor,
        /* [in] */ BigInt* quotient,
        /* [in] */ BigInt* remainder)
    {}

    static AutoPtr<BigInt> Modulus(
        /* [in] */ BigInt* a,
        /* [in] */ BigInt* m)
    {
        return nullptr;
    }

    static AutoPtr<BigInt> ModExp(
        /* [in] */ BigInt* a,
        /* [in] */ BigInt* p,
        /* [in] */ BigInt* m)
    {
        return nullptr;
    }

    static AutoPtr<BigInt> ModInverse(
        /* [in] */ BigInt* a,
        /* [in] */ BigInt* m)
    {
        return nullptr;
    }

    static AutoPtr<BigInt> GeneratePrimeDefault(
        /* [in] */ Integer bitLength)
    {
        return nullptr;
    }

    Boolean IsPrime(
        /* [in] */ Integer certainty)
    {
        return false;
    }

private:
    void MakeValid();

public:
    Long mBignum = 0;
};

inline Long BigInt::GetNativeBIGNUM()
{
    return mBignum;
}

inline AutoPtr<BigInt> BigInt::Copy()
{
    AutoPtr<BigInt> bi = new BigInt();
    bi->PutCopy(this);
    return bi;
}

inline Boolean BigInt::TwosCompFitsIntoBytes(
    /* [in] */ Integer desiredByteCount)
{
    Integer actualByteCount = (BitLength() + 7) / 8;
    return actualByteCount <= desiredByteCount;
}

}
}

#endif // __CCM_MATH_BIGINT_H__
