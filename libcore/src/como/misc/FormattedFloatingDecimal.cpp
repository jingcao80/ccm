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
#include "como/core/ThreadLocal.h"
#include "como/misc/FloatingDecimal.h"
#include "como/misc/FormattedFloatingDecimal.h"
#include "como/util/Arrays.h"

using como::core::Math;
using como::core::ThreadLocal;
using como::util::Arrays;

namespace como {
namespace misc {

class Wrapper
    : public Object
{
public:
    Wrapper()
        : mBuffer(20)
    {};

public:
    Array<Char> mBuffer;
};

class ThreadLocalCharBuffer
    : public ThreadLocal
{
protected:
    AutoPtr<IInterface> InitialValue() override
    {
        AutoPtr<IObject> buffer = new Wrapper();
        return buffer.Get();
    }
};

AutoPtr<IThreadLocal> FormattedFloatingDecimal::GetThreadLocalCharBuffer()
{
    static const AutoPtr<IThreadLocal> sThreadLocalCharBuffer =
            new ThreadLocalCharBuffer();
    return sThreadLocalCharBuffer;
}

COMO_INTERFACE_IMPL_1(FormattedFloatingDecimal, SyncObject, IFormattedFloatingDecimal);

ECode FormattedFloatingDecimal::ValueOf(
    /* [in] */ Double d,
    /* [in] */ Integer precision,
    /* [in] */ FormattedFloatingDecimalForm form,
    /* [out] */ IFormattedFloatingDecimal** fd)
{
    VALIDATE_NOT_NULL(fd);

    AutoPtr<IFloatingDecimalBinaryToASCIIConverter> fdConverter =
            FloatingDecimal::GetBinaryToASCIIConverter(d, form == FormattedFloatingDecimalForm::COMPATIBLE);
    AutoPtr<FormattedFloatingDecimal> fdObj = new FormattedFloatingDecimal();
    fdObj->Constructor(precision, form, fdConverter);
    *fd = fdObj.Get();
    REFCOUNT_ADD(*fd);
    return NOERROR;
}

Array<Char>& FormattedFloatingDecimal::GetBuffer()
{
    AutoPtr<IInterface> object;
    GetThreadLocalCharBuffer()->Get(object);
    return ((Wrapper*)IObject::Probe(object))->mBuffer;
}

ECode FormattedFloatingDecimal::Constructor(
    /* [in] */ Integer precision,
    /* [in] */ FormattedFloatingDecimalForm form,
    /* [in] */ IFloatingDecimalBinaryToASCIIConverter* fdConverter)
{
    Boolean exceptional;
    if (fdConverter->IsExceptional(&exceptional), exceptional) {
        String str;
        fdConverter->ToFormatString(&str);
        mMantissa = str.GetChars();
        mExponent = Array<Char>::Null();
        return NOERROR;
    }
    Array<Char> digits = GetBuffer();
    Integer nDigits;
    fdConverter->GetDigits(digits, &nDigits);
    Integer decExp;
    fdConverter->GetDecimalExponent(&decExp);
    Integer exp;
    Boolean isNegative;
    fdConverter->IsNegative(&isNegative);
    switch (form) {
        case FormattedFloatingDecimalForm::COMPATIBLE:
            exp = decExp;
            mDecExponentRounded = exp;
            FillCompatible(precision, digits, nDigits, exp, isNegative);
            break;

        case FormattedFloatingDecimalForm::DECIMAL_FLOAT:
            exp = ApplyPrecision(decExp, digits, nDigits, decExp + precision);
            FillDecimal(precision, digits, nDigits, exp, isNegative);
            mDecExponentRounded = exp;
            break;

        case FormattedFloatingDecimalForm::SCIENTIFIC:
            exp = ApplyPrecision(decExp, digits, nDigits, precision + 1);
            FillScientific(precision, digits, nDigits, exp, isNegative);
            mDecExponentRounded = exp;
            break;

        case FormattedFloatingDecimalForm::GENERAL:
            exp = ApplyPrecision(decExp, digits, nDigits, precision);
            // adjust precision to be the number of digits to right of decimal
            // the real exponent to be output is actually exp - 1, not exp
            if ((exp - 1 < -4) || (exp - 1 >= precision)) {
                // form = Form.SCIENTIFIC;
                precision--;
                FillScientific(precision, digits, nDigits, exp, isNegative);
            }
            else {
                // form = Form.DECIMAL_FLOAT;
                precision = precision - exp;
                FillDecimal(precision, digits, nDigits, exp, isNegative);
            }
            mDecExponentRounded = exp;
            break;

        default:
            CHECK(0);
            break;
    }
    return NOERROR;
}

ECode FormattedFloatingDecimal::GetExponentRounded(
    /* [out] */ Integer* exponent)
{
    VALIDATE_NOT_NULL(exponent);

    *exponent = mDecExponentRounded - 1;
    return NOERROR;
}

ECode FormattedFloatingDecimal::GetMantissa(
    /* [out, callee] */ Array<Char>* mantissa)
{
    VALIDATE_NOT_NULL(mantissa);

    *mantissa = mMantissa;
    return NOERROR;
}

ECode FormattedFloatingDecimal::GetExponent(
    /* [out, callee] */ Array<Char>* exponent)
{
    VALIDATE_NOT_NULL(exponent);

    *exponent = mExponent;
    return NOERROR;
}

Integer FormattedFloatingDecimal::ApplyPrecision(
    /* [in] */ Integer decExp,
    /* [out] */ Array<Char>& digits,
    /* [in] */ Integer nDigits,
    /* [in] */ Integer prec)
{
    if (prec >= nDigits || prec < 0) {
        // no rounding necessary
        return decExp;
    }
    if (prec == 0) {
        // only one digit (0 or 1) is returned because the precision
        // excludes all significant digits
        if (digits[0] >= U'5') {
            digits[0] = U'1';
            Arrays::Fill(digits, 1, nDigits, U'0');
            return decExp + 1;
        }
        else {
            Arrays::Fill(digits, 0, nDigits, U'0');
            return decExp;
        }
    }
    Integer q = digits[prec];
    if (q >= U'5') {
        Integer i = prec;
        q = digits[--i];
        if (q == U'9') {
            while (q == U'9' && i > 0) {
                q = digits[--i];
            }
            if (q == U'9') {
                // carryout! High-order 1, rest 0s, larger exp.
                digits[0] = U'1';
                Arrays::Fill(digits, 1, nDigits, U'0');
                return decExp + 1;
            }
        }
        digits[i] = (Char)(q + 1);
        Arrays::Fill(digits, i + 1, nDigits, U'0');
    }
    else {
        Arrays::Fill(digits, prec, nDigits, U'0');
    }
    return decExp;
}

void FormattedFloatingDecimal::FillCompatible(
    /* [in] */ Integer precision,
    /* [out] */ Array<Char>& digits,
    /* [in] */ Integer nDigits,
    /* [in] */ Integer exp,
    /* [in] */ Boolean isNegative)
{
    Integer startIndex = isNegative ? 1 : 0;
    if (exp > 0 && exp < 8) {
        // print digits.digits.
        if (nDigits < exp) {
            Integer extraZeros = exp - nDigits;
            mMantissa = Create(isNegative, nDigits + extraZeros + 2);
            mMantissa.Copy(startIndex, digits, 0, nDigits);
            Arrays::Fill(mMantissa, startIndex + nDigits, startIndex + nDigits + extraZeros, U'0');
            mMantissa[startIndex + nDigits + extraZeros] = U'.';
            mMantissa[startIndex + nDigits + extraZeros + 1] = U'0';
        }
        else if (exp < nDigits) {
            Integer t = Math::Min(nDigits - exp, precision);
            mMantissa = Create(isNegative, exp + 1 + t);
            mMantissa.Copy(startIndex, digits, 0, exp);
            mMantissa[startIndex + exp] = U'.';
            mMantissa.Copy(startIndex + exp + 1, digits, exp, t);
        }
        else {
            mMantissa = Create(isNegative, nDigits + 2);
            mMantissa.Copy(startIndex, digits, 0, nDigits);
            mMantissa[startIndex + nDigits] = U'.';
            mMantissa[startIndex + nDigits + 1] = U'0';
        }
    }
    else if (exp <= 0 && exp > -3) {
        Integer zeros = Math::Max(0, Math::Min(-exp, precision));
        Integer t = Math::Max(0, Math::Min(nDigits, precision + exp));
        // write '0' s before the significant digits
        if (zeros > 0) {
            mMantissa = Create(isNegative, zeros + 2 + t);
            mMantissa[startIndex] = U'0';
            mMantissa[startIndex + 1] = U'.';
            Arrays::Fill(mMantissa, startIndex + 2, startIndex + 2 + zeros, U'0');
            if (t > 0) {
                // copy only when significant digits are within the precision
                mMantissa.Copy(startIndex + 2 + zeros, digits, 0, t);
            }
        }
        else if (t > 0) {
            mMantissa = Create(isNegative, zeros + 2 + t);
            mMantissa[startIndex] = U'0';
            mMantissa[startIndex + 1] = U'.';
            // copy only when significant digits are within the precision
            mMantissa.Copy(startIndex + 2, digits, 0, t);
        }
        else {
            mMantissa = Create(isNegative, 1);
            mMantissa[startIndex] = U'0';
        }
    }
    else {
        if (nDigits > 1) {
            mMantissa = Create(isNegative, nDigits + 1);
            mMantissa[startIndex] = digits[0];
            mMantissa[startIndex + 1] = U'.';
            mMantissa.Copy(startIndex + 2, digits, 1, nDigits - 1);
        }
        else {
            mMantissa = Create(isNegative, 3);
            mMantissa[startIndex] = digits[0];
            mMantissa[startIndex + 1] = U'.';
            mMantissa[startIndex + 2] = U'0';
        }
        Integer e, expStartIndex;
        Boolean isNegExp = (exp <= 0);
        if (isNegExp) {
            e = -exp + 1;
            expStartIndex = 1;
        }
        else {
            e = exp - 1;
            expStartIndex = 0;
        }
        // decExponent has 1, 2, or 3, digits
        if (e < 9) {
            mExponent = Create(isNegExp, 1);
            mExponent[expStartIndex] = (Char)(e + U'0');
        }
        else if (e <= 99) {
            mExponent = Create(isNegExp, 2);
            mExponent[expStartIndex] = (Char)(e / 10 + U'0');
            mExponent[expStartIndex + 1] = (Char)(e % 10 + U'0');
        }
        else {
            mExponent = Create(isNegExp, 3);
            mExponent[expStartIndex] = (Char)(e / 100 + U'0');
            e %= 100;
            mExponent[expStartIndex + 1] = (Char)(e / 10 + U'0');
            mExponent[expStartIndex + 2] = (Char)(e % 10 + U'0');
        }
    }
}

Array<Char> FormattedFloatingDecimal::Create(
    /* [in] */ Boolean isNegative,
    /* [in] */ Integer size)
{
    if (isNegative) {
        Array<Char> r(size + 1);
        r[0] = U'-';
        return r;
    }
    else {
        return Array<Char>(size);
    }
}

void FormattedFloatingDecimal::FillDecimal(
    /* [in] */ Integer precision,
    /* [out] */ Array<Char>& digits,
    /* [in] */ Integer nDigits,
    /* [in] */ Integer exp,
    /* [in] */ Boolean isNegative)
{
    Integer startIndex = isNegative ? 1 : 0;
    if (exp > 0) {
        if (nDigits < exp) {
            mMantissa = Create(isNegative, exp);
            mMantissa.Copy(startIndex, digits, 0, nDigits);
            Arrays::Fill(mMantissa, startIndex + nDigits, startIndex + exp, U'0');
            // Do not append ".0" for formatted floats since the user
            // may request that it be omitted. It is added as necessary
            // by the Formatter.
        }
        else {
            Integer t = Math::Min(nDigits - exp, precision);
            mMantissa = Create(isNegative, exp + (t > 0 ? (t + 1) : 0));
            mMantissa.Copy(startIndex, digits, 0, exp);
            // Do not append ".0" for formatted floats since the user
            // may request that it be omitted. It is added as necessary
            // by the Formatter.
            if (t > 0) {
                mMantissa[startIndex + exp] = U'.';
                mMantissa.Copy(startIndex + exp + 1, digits, exp, t);
            }
        }
    }
    else if (exp <= 0) {
        Integer zeros = Math::Max(0, Math::Min(-exp, precision));
        Integer t = Math::Max(0, Math::Min(nDigits, precision + exp));
        // write '0' s before the significant digits
        if (zeros > 0) {
            mMantissa = Create(isNegative, zeros + 2 + t);
            mMantissa[startIndex] = U'0';
            mMantissa[startIndex + 1] = U'.';
            Arrays::Fill(mMantissa, startIndex + 2, startIndex + 2 + zeros, U'0');
            if (t > 0) {
                // copy only when significant digits are within the precision
                mMantissa.Copy(startIndex + 2 + zeros, digits, 0, t);
            }
        }
        else if (t > 0) {
            mMantissa = Create(isNegative, zeros + 2 + t);
            mMantissa[startIndex] = U'0';
            mMantissa[startIndex + 1] = U'.';
            // copy only when significant digits are within the precision
            mMantissa.Copy(startIndex + 2, digits, 0, t);
        }
        else {
            mMantissa = Create(isNegative, 1);
            mMantissa[startIndex] = U'0';
        }
    }
}

void FormattedFloatingDecimal::FillScientific(
    /* [in] */ Integer precision,
    /* [out] */ Array<Char>& digits,
    /* [in] */ Integer nDigits,
    /* [in] */ Integer exp,
    /* [in] */ Boolean isNegative)
{
    Integer startIndex = isNegative ? 1 : 0;
    Integer t = Math::Max(0, Math::Min(nDigits - 1, precision));
    if (t > 0) {
        mMantissa = Create(isNegative, t + 2);
        mMantissa[startIndex] = digits[0];
        mMantissa[startIndex + 1] = U'.';
        mMantissa.Copy(startIndex + 2, digits, 1, t);
    }
    else {
        mMantissa = Create(isNegative, 1);
        mMantissa[startIndex] = digits[0];
    }
    Char expSign;
    Integer e;
    if (exp <= 0) {
        expSign = U'-';
        e = -exp + 1;
    }
    else {
        expSign = U'+';
        e = exp - 1;
    }
    // decExponent has 1, 2, or 3, digits
    if (e <= 9) {
        mExponent = { expSign, U'0', (Char)(e + U'0') };
    }
    else if (e <= 99) {
        mExponent = { expSign, (Char)(e / 10 + U'0'), (Char)(e % 10 + U'0') };
    }
    else {
        Char hiExpChar = (Char)(e / 100 + U'0');
        e %= 100;
        mExponent = { expSign, hiExpChar, (Char)(e / 10 + U'0'),
                (Char)(e % 10 + U'0') };
    }
}

}
}
