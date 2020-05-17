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

#include "como/math/BigInt.h"
#include "como/math/Multiplication.h"
#include "como.core.IInteger.h"

using como::core::IInteger;

namespace como {
namespace math {

const Array<Integer> Multiplication::sTenPows{
    1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000
};

const Array<Integer> Multiplication::sFivePows{
    1, 5, 25, 125, 625, 3125, 15625, 78125, 390625,
    1953125, 9765625, 48828125, 244140625, 1220703125
};

Array<IBigInteger*> CreateBigTenPows()
{
    Integer i;
    Long fivePow = 1LL;
    Array<IBigInteger*> bigTenPows(32);

    for (i = 0; i <= 18; i++) {
        AutoPtr<IBigInteger> tempBI;
        CBigInteger::ValueOf(fivePow << i, tempBI);
        bigTenPows.Set(i, tempBI);
        fivePow *= 5;
    }
    for (; i < bigTenPows.GetLength(); i++) {
        AutoPtr<IBigInteger> tempBI;
        bigTenPows[i - 1]->Multiply(CBigInteger::GetTEN(), tempBI);
        bigTenPows.Set(i, tempBI);
    }

    return bigTenPows;
}

Array<IBigInteger*> CreateBigFivePows()
{
    Integer i;
    Long fivePow = 1LL;
    Array<IBigInteger*> bigFivePows(32);

    for (i = 0; i <= 18; i++) {
        AutoPtr<IBigInteger> tempBI;
        CBigInteger::ValueOf(fivePow, tempBI);
        bigFivePows.Set(i, tempBI);
        fivePow *= 5;
    }
    for (; i < bigFivePows.GetLength(); i++) {
        AutoPtr<IBigInteger> tempBI;
        bigFivePows[i - 1]->Multiply(bigFivePows[1], tempBI);
        bigFivePows.Set(i, tempBI);
    }

    return bigFivePows;
}

const Array<IBigInteger*>& Multiplication::GetBigTenPows()
{
    static const Array<IBigInteger*> sBigTenPows = CreateBigTenPows();
    return sBigTenPows;
}

const Array<IBigInteger*>& Multiplication::GetBigFivePows()
{
    static const Array<IBigInteger*> sBigFivePows = CreateBigFivePows();
    return sBigFivePows;
}

AutoPtr<IBigInteger> Multiplication::MultiplyByPositiveInteger(
    /* [in] */ BigInteger* value,
    /* [in] */ Integer factor)
{
    AutoPtr<BigInt> bi = value->GetBigInt()->Copy();
    bi->MultiplyByPositiveInteger(factor);
    AutoPtr<IBigInteger> result;
    CBigInteger::New(bi, IID_IBigInteger, (IInterface**)&result);
    return result;
}

AutoPtr<IBigInteger> Multiplication::MultiplyByTenPow(
    /* [in] */ BigInteger* value,
    /* [in] */ Long exp)
{
    if (exp < sTenPows.GetLength()) {
        return MultiplyByPositiveInteger(value, sTenPows[(Integer)exp]);
    }
    else {
        AutoPtr<IBigInteger> tempBI;
        value->Multiply(PowerOf10(exp), tempBI);
        return tempBI;
    }
}

AutoPtr<IBigInteger> Multiplication::PowerOf10(
    /* [in] */ Long exp)
{
    Integer intExp = (Integer)exp;
    // "SMALL POWERS"
    if (exp < GetBigTenPows().GetLength()) {
        // The largest power that fit in 'long' type
        return GetBigTenPows()[intExp];
    }
    else if (exp <= 50) {
        AutoPtr<IBigInteger> tempBI;
        CBigInteger::GetTEN()->Pow(intExp, tempBI);
        return tempBI;
    }

    AutoPtr<IBigInteger> res;
    // "LARGE POWERS"
    if (exp < IInteger::MAX_VALUE) {
        // To calculate:    5^exp * 2^exp
        AutoPtr<IBigInteger> tempBI;
        GetBigFivePows()[1]->Pow(intExp, tempBI);
        tempBI->ShiftLeft(intExp, res);
    }
    else {
        /*
         * "HUGE POWERS"
         *
         * This branch probably won't be executed since the power of ten is too
         * big.
         */
        // To calculate:    5^exp
        AutoPtr<IBigInteger> powerOfFive;
        GetBigFivePows()[1]->Pow(IInteger::MAX_VALUE, powerOfFive);
        res = powerOfFive;
        Long longExp = exp - IInteger::MAX_VALUE;

        AutoPtr<IBigInteger> tempBI, bi1;
        intExp = (Integer)(exp % IInteger::MAX_VALUE);
        while (longExp > IInteger::MAX_VALUE) {
            res->Multiply(powerOfFive, tempBI);
            res = std::move(tempBI);
            longExp -= IInteger::MAX_VALUE;
        }
        GetBigFivePows()[1]->Pow(intExp, bi1);
        res->Multiply(bi1, tempBI);
        res = std::move(tempBI);
        // To calculate:    5^exp << exp
        res->ShiftLeft(IInteger::MAX_VALUE, tempBI);
        res = std::move(tempBI);
        longExp = exp - IInteger::MAX_VALUE;
        while (longExp > IInteger::MAX_VALUE) {
            res->ShiftLeft(IInteger::MAX_VALUE, tempBI);
            res = std::move(tempBI);
            longExp -= IInteger::MAX_VALUE;
        }
        res->ShiftLeft(intExp, tempBI);
        res = std::move(tempBI);
    }

    return res;
}

AutoPtr<IBigInteger> Multiplication::MultiplyByFivePow(
    /* [in] */ BigInteger* value,
    /* [in] */ Integer exp)
{
    if (exp < sFivePows.GetLength()) {
        return MultiplyByPositiveInteger(value, sFivePows[exp]);
    }
    else if (exp > GetBigFivePows().GetLength()) {
        AutoPtr<IBigInteger> result;
        value->Multiply(GetBigFivePows()[exp], result);
        return result;
    }
    else {
        // Large powers of five
        AutoPtr<IBigInteger> tempBI, result;
        GetBigFivePows()[1]->Pow(exp, tempBI);
        value->Multiply(tempBI, result);
        return result;
    }
}

}
}
