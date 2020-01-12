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

#include "como/core/Math.h"
#include "como/math/Logical.h"

using como::core::Math;

namespace como {
namespace math {

ECode Logical::Not(
    /* [in] */ BigInteger* value,
    /* [out] */ IBigInteger** result)
{
    if (value->mSign == 0) {
        CBigInteger::GetMINUS_ONE().MoveTo(result);
        return NOERROR;
    }
    Boolean same;
    if (value->Equals(CBigInteger::GetMINUS_ONE(), same), same) {
        CBigInteger::GetZERO().MoveTo(result);
        return NOERROR;
    }
    Array<Integer> resDigits(value->mNumberLength + 1);
    Integer i;

    if (value->mSign > 0) {
        if (value->mDigits[value->mNumberLength - 1] != -1) {
            for (i = 0; value->mDigits[i] == -1; i++) {
                ;
            }
        }
        else {
            for (i = 0; (i < value->mNumberLength) && (value->mDigits[i] == -1); i++) {
                ;
            }
            if (i == value->mNumberLength) {
                resDigits[i] = 1;
                return CBigInteger::New(-value->mSign, i + 1, resDigits, IID_IBigInteger, (IInterface**)result);
            }
        }
        // Here a carry 1 was generated
    }
    else {
        for (i = 0; value->mDigits[i] == 0; i++) {
            resDigits[i] = -1;
        }
        // Here a borrow -1 was generated
    }
    // Now, the carry/borrow can be absorbed
    resDigits[i] = value->mDigits[i] + value->mSign;
    // Copying the remaining unchanged digit
    for (i++; i < value->mNumberLength; i++) {
        resDigits[i] = value->mDigits[i];
    }
    return CBigInteger::New(-value->mSign, i, resDigits, IID_IBigInteger, (IInterface**)result);
}

ECode Logical::And(
    /* [in] */ BigInteger* value,
    /* [in] */ BigInteger* that,
    /* [out] */ IBigInteger** result)
{
    if (that->mSign == 0 || value->mSign == 0) {
        CBigInteger::GetZERO().MoveTo(result);
        return NOERROR;
    }
    Boolean same;
    if (that->Equals(CBigInteger::GetMINUS_ONE(), same), same) {
        *result = value;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    if (value->Equals(CBigInteger::GetMINUS_ONE(), same), same) {
        *result = that;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    if (value->mSign > 0) {
        if (that->mSign > 0) {
            return AddPositive(value, that, result);
        }
        else {
            return AddDiffSigns(value, that, result);
        }
    }
    else {
        if (that->mSign > 0) {
            return AddDiffSigns(that, value, result);
        }
        else if (value->mNumberLength > that->mNumberLength) {
            return AddNegative(value, that, result);
        }
        else {
            return AddNegative(that, value, result);
        }
    }
}

ECode Logical::AddPositive(
    /* [in] */ BigInteger* value,
    /* [in] */ BigInteger* that,
    /* [out] */ IBigInteger** result)
{
    Integer resLength = Math::Min(value->mNumberLength, that->mNumberLength);
    Integer i = Math::Max(value->GetFirstNonzeroDigit(), that->GetFirstNonzeroDigit());

    if (i >= resLength) {
        CBigInteger::GetZERO().MoveTo(result);
        return NOERROR;
    }

    Array<Integer> resDigits(resLength);
    for (; i < resLength; i++) {
        resDigits[i] = value->mDigits[i] & that->mDigits[i];
    }

    return CBigInteger::New(1, resLength, resDigits, IID_IBigInteger, (IInterface**)result);
}

ECode Logical::AddDiffSigns(
    /* [in] */ BigInteger* positive,
    /* [in] */ BigInteger* negative,
    /* [out] */ IBigInteger** result)
{
    Integer iPos = positive->GetFirstNonzeroDigit();
    Integer iNeg = negative->GetFirstNonzeroDigit();

    // Look if the trailing zeros of the negative will "blank" all
    // the positive digits
    if (iNeg >= positive->mNumberLength) {
        CBigInteger::GetZERO().MoveTo(result);
        return NOERROR;
    }
    Integer resLength = positive->mNumberLength;
    Array<Integer> resDigits(resLength);

    // Must start from max(iPos, iNeg)
    Integer i = Math::Max(iPos, iNeg);
    if (i == iNeg) {
        resDigits[i] = -negative->mDigits[i] & positive->mDigits[i];
        i++;
    }
    Integer limit = Math::Min(negative->mNumberLength, positive->mNumberLength);
    for (; i < limit; i++) {
        resDigits[i] = ~negative->mDigits[i] & positive->mDigits[i];
    }
    // if the negative was shorter must copy the remaining digits
    // from positive
    if (i >= negative->mNumberLength) {
        for (; i < positive->mNumberLength; i++) {
            resDigits[i] = positive->mDigits[i];
        }
    } // else positive ended and must "copy" virtual 0's, do nothing then

    return CBigInteger::New(1, resLength, resDigits, IID_IBigInteger, (IInterface**)result);
}

ECode Logical::AddNegative(
    /* [in] */ BigInteger* longer,
    /* [in] */ BigInteger* shorter,
    /* [out] */ IBigInteger** result)
{
    Integer iLonger = longer->GetFirstNonzeroDigit();
    Integer iShorter = shorter->GetFirstNonzeroDigit();

    // Does shorter matter?
    if (iLonger >= shorter->mNumberLength) {
        *result = longer;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    Integer resLength;
    Array<Integer> resDigits;
    Integer i = Math::Max(iShorter, iLonger);
    Integer digit;
    if (iShorter > iLonger) {
        digit = -shorter->mDigits[i] & ~longer->mDigits[i];
    }
    else if (iShorter < iLonger) {
        digit = ~shorter->mDigits[i] & -longer->mDigits[i];
    }
    else {
        digit = -shorter->mDigits[i] & -longer->mDigits[i];
    }
    if (digit == 0) {
        for (i++; i < shorter->mNumberLength && (digit = ~(longer->mDigits[i] | shorter->mDigits[i])) == 0; i++) {
            ;
        }
        if (digit == 0) {
            // shorter has only the remaining virtual sign bits
            for (; i < longer->mNumberLength && (digit = ~longer->mDigits[i]) == 0; i++) {
                ;
            }
            if (digit == 0) {
                resLength = longer->mNumberLength + 1;
                resDigits = Array<Integer>(resLength);
                resDigits[resLength - 1] = 1;

                return CBigInteger::New(-1, resLength, resDigits, IID_IBigInteger, (IInterface**)result);
            }
        }
    }
    resLength = longer->mNumberLength;
    resDigits = Array<Integer>(resLength);
    resDigits[i] = -digit;
    for (i++; i < shorter->mNumberLength; i++) {
        resDigits[i] = longer->mDigits[i] | shorter->mDigits[i];
    }
    // shorter has only the remaining virtual sign bits
    for (; i < longer->mNumberLength; i++) {
        resDigits[i] = longer->mDigits[i];
    }

    return CBigInteger::New(-1, resLength, resDigits, IID_IBigInteger, (IInterface**)result);
}

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
        CBigInteger::GetZERO().MoveTo(result);
        return NOERROR;
    }
    Boolean same;
    if (value->Equals(CBigInteger::GetMINUS_ONE(), same), same) {
        return that->Not(result);
    }
    if (that->Equals(CBigInteger::GetMINUS_ONE(), same), same) {
        CBigInteger::GetZERO().MoveTo(result);
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
        CBigInteger::GetZERO().MoveTo(result);
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

ECode Logical::Or(
    /* [in] */ BigInteger* value,
    /* [in] */ BigInteger* that,
    /* [out] */ IBigInteger** result)
{
    Boolean same;
    if ((that->Equals(CBigInteger::GetMINUS_ONE(), same), same) ||
            (value->Equals(CBigInteger::GetMINUS_ONE(), same), same)) {
        CBigInteger::GetMINUS_ONE().MoveTo(result);
        return NOERROR;
    }
    if (that->mSign == 0) {
        *result = value;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    if (value->mSign == 0) {
        *result = that;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    if (value->mSign > 0) {
        if (that->mSign > 0) {
            if (value->mNumberLength > that->mNumberLength) {
                return OrPositive(value, that, result);
            }
            else {
                return OrPositive(that, value, result);
            }
        }
        else {
            return OrDiffSigns(value, that, result);
        }
    }
    else {
        if (that->mSign > 0) {
            return OrDiffSigns(that, value, result);
        }
        else if (that->GetFirstNonzeroDigit() > value->GetFirstNonzeroDigit()) {
            return OrNegative(that, value, result);
        }
        else {
            return OrNegative(value, that, result);
        }
    }
}

ECode Logical::OrPositive(
    /* [in] */ BigInteger* longer,
    /* [in] */ BigInteger* shorter,
    /* [out] */ IBigInteger** result)
{
    Integer resLength = longer->mNumberLength;
    Array<Integer> resDigits(resLength);

    Integer i;
    for (i = 0; i < shorter->mNumberLength; i++) {
        resDigits[i] = longer->mDigits[i] | shorter->mDigits[i];
    }
    for (; i < resLength; i++) {
        resDigits[i] = longer->mDigits[i];
    }

    return CBigInteger::New(1, resLength, resDigits, IID_IBigInteger, (IInterface**)result);
}

ECode Logical::OrNegative(
    /* [in] */ BigInteger* value,
    /* [in] */ BigInteger* that,
    /* [out] */ IBigInteger** result)
{
    Integer iThat = that->GetFirstNonzeroDigit();
    Integer iValue = value->GetFirstNonzeroDigit();
    Integer i;

    if (iValue >= that->mNumberLength) {
        *result = that;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    else if (iThat >= value->mNumberLength) {
        *result = value;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    Integer resLength = Math::Min(value->mNumberLength, that->mNumberLength);
    Array<Integer> resDigits(resLength);

    //Looking for the first non-zero digit of the result
    if (iThat == iValue) {
        resDigits[iValue] = -(-value->mDigits[iValue] | -that->mDigits[iValue]);
        i = iValue;
    }
    else {
        for (i = iThat; i < iValue; i++) {
            resDigits[i] = that->mDigits[i];
        }
        resDigits[i] = that->mDigits[i] & (value->mDigits[i] - 1);
    }

    for (i++; i < resLength; i++) {
        resDigits[i] = value->mDigits[i] & that->mDigits[i];
    }

    return CBigInteger::New(-1, resLength, resDigits, IID_IBigInteger, (IInterface**)result);
}

ECode Logical::OrDiffSigns(
    /* [in] */ BigInteger* positive,
    /* [in] */ BigInteger* negative,
    /* [out] */ IBigInteger** result)
{
    // Jumping over the least significant zero bits
    Integer iNeg = negative->GetFirstNonzeroDigit();
    Integer iPos = positive->GetFirstNonzeroDigit();
    Integer i;
    Integer limit;

    // Look if the trailing zeros of the positive will "copy" all
    // the negative digits
    if (iPos >= negative->mNumberLength) {
        *result = negative;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    Integer resLength = negative->mNumberLength;
    Array<Integer> resDigits(resLength);

    if (iNeg < iPos) {
        // We know for sure that this will
        // be the first non zero digit in the result
        for (i = iNeg; i < iPos; i++) {
            resDigits[i] =  negative->mDigits[i];
        }
    }
    else if (iPos < iNeg) {
        i = iPos;
        resDigits[i] = -positive->mDigits[i];
        limit = Math::Min(positive->mNumberLength, iNeg);
        for (i++; i < limit; i++) {
            resDigits[i] = ~positive->mDigits[i];
        }
        if (i != positive->mNumberLength) {
            resDigits[i] = ~(-negative->mDigits[i] | positive->mDigits[i]);
        }
        else {
            for (; i < iNeg; i++) {
                resDigits[i] = -1;
            }
            resDigits[i] = negative->mDigits[i] - 1;
        }
        i++;
    }
    else {  // iNeg == iPos
        // Applying two complement to negative and to result
        i = iPos;
        resDigits[i] = -(-negative->mDigits[i] | positive->mDigits[i]);
        i++;
    }
    limit = Math::Min(negative->mNumberLength, positive->mNumberLength);
    for (; i < limit; i++) {
        // Applying two complement to negative and to result
        resDigits[i] = negative->mDigits[i] & ~positive->mDigits[i];
    }
    for (; i < negative->mNumberLength; i++) {
        resDigits[i] = negative->mDigits[i];
    }

    return CBigInteger::New(-1, resLength, resDigits, IID_IBigInteger, (IInterface**)result);
}

ECode Logical::Xor(
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
        *result = that;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    Boolean same;
    if (that->Equals(CBigInteger::GetMINUS_ONE(), same), same) {
        return value->Not(result);
    }
    if (value->Equals(CBigInteger::GetMINUS_ONE(), same), same) {
        return that->Not(result);
    }

    if (value->mSign > 0) {
        if (that->mSign > 0) {
            if (value->mNumberLength > that->mNumberLength) {
                return XorPositive(value, that, result);
            }
            else {
                return XorPositive(that, value, result);
            }
        }
        else {
            return XorDiffSigns(value, that, result);
        }
    }
    else {
        if (that->mSign > 0) {
            return XorDiffSigns(that, value, result);
        }
        else if (that->GetFirstNonzeroDigit() > value->GetFirstNonzeroDigit()) {
            return XorNegative(that, value, result);
        }
        else {
            return XorNegative(value, that, result);
        }
    }
}

ECode Logical::XorPositive(
    /* [in] */ BigInteger* longer,
    /* [in] */ BigInteger* shorter,
    /* [out] */ IBigInteger** result)
{
    Integer resLength = longer->mNumberLength;
    Array<Integer> resDigits(resLength);
    Integer i = Math::Min(longer->GetFirstNonzeroDigit(), shorter->GetFirstNonzeroDigit());
    for (; i < shorter->mNumberLength; i++) {
        resDigits[i] = longer->mDigits[i] ^ shorter->mDigits[i];
    }
    for (; i < longer->mNumberLength; i++) {
        resDigits[i] = longer->mDigits[i];
    }

    return CBigInteger::New(1, resLength, resDigits, IID_IBigInteger, (IInterface**)result);
}

ECode Logical::XorNegative(
    /* [in] */ BigInteger* value,
    /* [in] */ BigInteger* that,
    /* [out] */ IBigInteger** result)
{
    Integer resLength = Math::Max(value->mNumberLength, that->mNumberLength);
    Array<Integer> resDigits(resLength);
    Integer iValue = value->GetFirstNonzeroDigit();
    Integer iThat = that->GetFirstNonzeroDigit();
    Integer i = iThat;
    Integer limit;

    if (iValue == iThat) {
        resDigits[i] = -value->mDigits[i] ^ -that->mDigits[i];
    }
    else {
        resDigits[i] = - that->mDigits[i];
        limit = Math::Min(that->mNumberLength, iValue);
        for (i++; i < limit; i++) {
            resDigits[i] = ~that->mDigits[i];
        }
        // Remains digits in that?
        if (i == that->mNumberLength) {
            //Jumping over the remaining zero to the first non one
            for (; i < iValue; i++) {
                resDigits[i] = -1;
            }
            resDigits[i] = value->mDigits[i] - 1;
        }
        else {
            resDigits[i] = -value->mDigits[i] ^ ~that->mDigits[i];
        }
    }

    limit = Math::Min(value->mNumberLength, that->mNumberLength);
    //Perform ^ between that al val until that ends
    for (i++; i < limit; i++) {
        resDigits[i] = value->mDigits[i] ^ that->mDigits[i];
    }
    //Perform ^ between val digits and -1 until val ends
    for (; i < value->mNumberLength; i++) {
        resDigits[i] = value->mDigits[i];
    }
    for (; i < that->mNumberLength; i++) {
        resDigits[i] = that->mDigits[i];
    }

    return CBigInteger::New(1, resLength, resDigits, IID_IBigInteger, (IInterface**)result);
}

ECode Logical::XorDiffSigns(
    /* [in] */ BigInteger* positive,
    /* [in] */ BigInteger* negative,
    /* [out] */ IBigInteger** result)
{
    Integer resLength = Math::Max(negative->mNumberLength, positive->mNumberLength);
    Array<Integer> resDigits;
    Integer iNeg = negative->GetFirstNonzeroDigit();
    Integer iPos = positive->GetFirstNonzeroDigit();
    Integer i;
    Integer limit;

    //The first
    if (iNeg < iPos) {
        resDigits = Array<Integer>(resLength);
        i = iNeg;
        resDigits[i] = negative->mDigits[i];
        limit = Math::Min(negative->mNumberLength, iPos);
        //Skip the positive digits while they are zeros
        for (i++; i < limit; i++) {
            resDigits[i] = negative->mDigits[i];
        }
        //if the negative has no more elements, must fill the
        //result with the remaining digits of the positive
        if (i == negative->mNumberLength) {
            for (; i < positive->mNumberLength; i++) {
                resDigits[i] = positive->mDigits[i];
            }
        }
    }
    else if (iPos < iNeg) {
        resDigits = Array<Integer>(resLength);
        i = iPos;
        //Applying two complement to the first non-zero digit of the result
        resDigits[i] = -positive->mDigits[i];
        limit = Math::Min(positive->mNumberLength, iNeg);
        for (i++; i < limit; i++) {
            //Continue applying two complement the result
            resDigits[i] = ~positive->mDigits[i];
        }
        //When the first non-zero digit of the negative is reached, must apply
        //two complement (arithmetic negation) to it, and then operate
        if (i == iNeg) {
            resDigits[i] = ~(positive->mDigits[i] ^ -negative->mDigits[i]);
            i++;
        }
        else {
            //if the positive has no more elements must fill the remaining digits with
            //the negative ones
            for (; i < iNeg; i++) {
                resDigits[i] = -1;
            }
            for (; i < negative->mNumberLength; i++) {
                resDigits[i] = negative->mDigits[i];
            }
        }
    }
    else {
        //The first non-zero digit of the positive and negative are the same
        i = iNeg;
        Integer digit = positive->mDigits[i] ^ -negative->mDigits[i];
        if (digit == 0) {
            limit = Math::Min(positive->mNumberLength, negative->mNumberLength);
            for (i++; i < limit && (digit = positive->mDigits[i] ^ ~negative->mDigits[i]) == 0; i++) {
                ;
            }
            if (digit == 0) {
                // shorter has only the remaining virtual sign bits
                for (; i < positive->mNumberLength && (digit = ~positive->mDigits[i]) == 0; i++) {
                    ;
                }
                for (; i < negative->mNumberLength && (digit = ~negative->mDigits[i]) == 0; i++) {
                    ;
                }
                if (digit == 0) {
                    resLength = resLength + 1;
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

    limit = Math::Min(negative->mNumberLength, positive->mNumberLength);
    for (; i < limit; i++) {
        resDigits[i] = ~(~negative->mDigits[i] ^ positive->mDigits[i]);
    }
    for (; i < positive->mNumberLength; i++) {
        resDigits[i] = positive->mDigits[i];
    }
    for (; i < negative->mNumberLength; i++) {
        resDigits[i] = negative->mDigits[i];
    }

    return CBigInteger::New(-1, resLength, resDigits, IID_IBigInteger, (IInterface**)result);
}

}
}
