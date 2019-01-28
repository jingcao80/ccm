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

#include "ccm/core/Math.h"
#include "ccm/math/Logical.h"

using ccm::core::Math;

namespace ccm {
namespace math {

ECode Logical::AndNot(
    /* [in] */ BigInteger* value,
    /* [in] */ BigInteger* that,
    /* [out] */ IBigInteger** result)
{
    if (that->mSign == 0) {
        *result = value;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    if (value->mSign == 0) {
        *result = CBigInteger::GetZERO();
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    Boolean same;
    if (value->Equals(CBigInteger::GetMINUS_ONE(), &same), same) {
        return that->Not(result);
    }
    if (that->Equals(CBigInteger::GetMINUS_ONE(), &same), same) {
        *result = CBigInteger::GetZERO();
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    if (value->mSign > 0) {
        if (that->mSign > 0) {
            return AddNotPositive(value, that, result);
        }
        else {
            return AddNotPositiveNegative(value, that, result);
        }
    }
    else {
        if (that->mSign > 0) {
            return AddNotNegativePositive(value, that, result);
        }
        else {
            return AddNotNegative(value, that, result);
        }
    }
}

ECode Logical::AddNotPositive(
    /* [in] */ BigInteger* value,
    /* [in] */ BigInteger* that,
    /* [out] */ IBigInteger** result)
{
    Array<Integer> resDigits(value->mNumberLength);

    Integer limit = Math::Min(value->mNumberLength, that->mNumberLength);
    Integer i;
    for (i = value->GetFirstNonzeroDigit(); i < limit; i++) {
        resDigits[i] = value->mDigits[i] & ~that->mDigits[i];
    }
    for (; i < value->mNumberLength; i++) {
        resDigits[i] = value->mDigits[i];
    }

    return CBigInteger::New(1, value->mNumberLength, resDigits, IID_IBigInteger, (IInterface**)result);
}

ECode Logical::AddNotPositiveNegative(
    /* [in] */ BigInteger* positive,
    /* [in] */ BigInteger* negative,
    /* [out] */ IBigInteger** result)
{
    Integer iNeg = negative->GetFirstNonzeroDigit();
    Integer iPos = positive->GetFirstNonzeroDigit();

    if (iNeg >= positive->mNumberLength) {
        *result = positive;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    Integer resLength = Math::Min(positive->mNumberLength, negative->mNumberLength);
    Array<Integer> resDigits(resLength);

    // Always start from first non zero of positive
    Integer i = iPos;
    for (; i < iNeg; i++) {
        resDigits[i] = positive->mDigits[i];
    }
    if (i == iNeg) {
        resDigits[i] = positive->mDigits[i] & (negative->mDigits[i] - 1);
        i++;
    }
    for (; i < resLength; i++) {
        resDigits[i] = positive->mDigits[i] & negative->mDigits[i];
    }

    return CBigInteger::New(1, resLength, resDigits, IID_IBigInteger, (IInterface**)result);
}

ECode Logical::AddNotNegativePositive(
    /* [in] */ BigInteger* negative,
    /* [in] */ BigInteger* positive,
    /* [out] */ IBigInteger** result)
{
    Integer resLength;
    Array<Integer> resDigits;
    Integer limit;
    Integer digit;

    Integer iNeg = negative->GetFirstNonzeroDigit();
    Integer iPos = positive->GetFirstNonzeroDigit();

    if (iNeg >= positive->mNumberLength) {
        *result = negative;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    resLength = Math::Max(negative->mNumberLength, positive->mNumberLength);
    Integer i = iNeg;
    if (iPos > iNeg) {
        resDigits = Array<Integer>(resLength);
        limit = Math::Min(negative->mNumberLength, iPos);
        for (; i < limit; i++) {
            resDigits[i] = negative->mDigits[i];
        }
        if (i == negative->mNumberLength) {
            for (i = iPos; i < positive->mNumberLength; i++) {
                resDigits[i] = positive->mDigits[i];
            }
        }
    }
    else {
        digit = -negative->mDigits[i] & ~positive->mDigits[i];
        if (digit == 0) {
            limit = Math::Min(positive->mNumberLength, negative->mNumberLength);
            for (i++; i < limit && (digit = ~(negative->mDigits[i] | positive->mDigits[i])) == 0; i++) {
                ;
            }
            if (digit == 0) {
                // the shorter has only the remaining virtual sign bits
                for (; i < positive->mNumberLength && (digit = ~positive->mDigits[i]) == 0; i++) {
                    ;
                }
                for (; i < negative->mNumberLength && (digit = ~negative->mDigits[i]) == 0; i++) {
                    ;
                }
                if (digit == 0) {
                    resLength++;
                    resDigits = Array<Integer>(resLength);
                    resDigits[resLength - 1] = 1;

                    return CBigInteger::New(-1, resLength, resDigits, IID_IBigInteger, (IInterface**)result);
                }
            }
        }
        resDigits = Array<Integer>(resLength);
        resDigits[i] = -digit;
        i++;
    }

    limit = Math::Min(positive->mNumberLength, negative->mNumberLength);
    for (; i < limit; i++) {
        resDigits[i] = negative->mDigits[i] | positive->mDigits[i];
    }
    // Actually one of the next two cycles will be executed
    for (; i < negative->mNumberLength; i++) {
        resDigits[i] = negative->mDigits[i];
    }
    for (; i < positive->mNumberLength; i++) {
        resDigits[i] = positive->mDigits[i];
    }

    return CBigInteger::New(-1, resLength, resDigits, IID_IBigInteger, (IInterface**)result);
}

ECode Logical::AddNotNegative(
    /* [in] */ BigInteger* value,
    /* [in] */ BigInteger* that,
    /* [out] */ IBigInteger** result)
{
    Integer iValue = value->GetFirstNonzeroDigit();
    Integer iThat =  that->GetFirstNonzeroDigit();

    if (iValue >= that->mNumberLength) {
        *result = CBigInteger::GetZERO();
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    Integer resLength = that->mNumberLength;
    Array<Integer> resDigits(resLength);
    Integer limit;
    Integer i = iValue;
    if (iValue < iThat) {
        resDigits[i] = -value->mDigits[i];
        limit = Math::Min(value->mNumberLength, iThat);
        for (i++; i < limit; i++) {
            resDigits[i] = ~value->mDigits[i];
        }
        if (i == value->mNumberLength) {
            for (; i < iThat; i++) {
                resDigits[i] = -1;
            }
            resDigits[i] = that->mDigits[i] - 1;
        }
        else {
            resDigits[i] = ~value->mDigits[i] & (that->mDigits[i] - 1);
        }
    }
    else if (iThat < iValue) {
        resDigits[i] = -value->mDigits[i] & that->mDigits[i];
    }
    else {
        resDigits[i] = -value->mDigits[i] & (that->mDigits[i] - 1);
    }

    limit = Math::Min(value->mNumberLength, that->mNumberLength);
    for (i++; i < limit; i++) {
        resDigits[i] = ~value->mDigits[i] & that->mDigits[i];
    }
    for (; i < that->mNumberLength; i++) {
        resDigits[i] = that->mDigits[i];
    }

    return CBigInteger::New(1, resLength, resDigits, IID_IBigInteger, (IInterface**)result);
}

}
}
