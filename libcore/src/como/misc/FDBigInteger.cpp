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

#include "innerdef.h"
#include "como/core/CStringBuilder.h"
#include "como/core/Math.h"
#include "como/core/StringUtils.h"
#include "como/math/CBigInteger.h"
#include "como/misc/CFDBigInteger.h"
#include "como/misc/FDBigInteger.h"
#include "como/util/Arrays.h"
#include <comolog.h>

using como::core::CStringBuilder;
using como::core::IID_IStringBuilder;
using como::core::IStringBuilder;
using como::core::Math;
using como::core::StringUtils;
using como::math::CBigInteger;
using como::math::IBigInteger;
using como::math::IID_IBigInteger;
using como::util::Arrays;

namespace como {
namespace misc {

constexpr Integer FDBigInteger::SMALL_5_POW[];
constexpr Long FDBigInteger::LONG_5_POW[];

static AutoPtr<IFDBigInteger> CreateZERO()
{
    Array<Integer> data(0);
    AutoPtr<IFDBigInteger> value;
    CFDBigInteger::New(data, 0, IID_IFDBigInteger, (IInterface**)&value);
    value->MakeImmutable();
    return value;
}

const AutoPtr<IFDBigInteger> FDBigInteger::GetZERO()
{
    static const AutoPtr<IFDBigInteger> ZERO = CreateZERO();
    return ZERO;
}

static Array<IFDBigInteger*> CreatePOW_5_CACHE(
    /* [in] */ Integer max_five_pow,
    /* [in] */ const Integer small_5_pow[],
    /* [in] */ Integer small_5_pow_len)
{
    Array<IFDBigInteger*> array(max_five_pow);
    Integer i = 0;
    while (i < small_5_pow_len) {
        Array<Integer> data{ small_5_pow[i] };
        AutoPtr<IFDBigInteger> pow5;
        CFDBigInteger::New(data, 0, IID_IFDBigInteger, (IInterface**)&pow5);
        pow5->MakeImmutable();
        array.Set(i, pow5);
        i++;
    }
    AutoPtr<IFDBigInteger> prev = array[i - 1];
    while (i < max_five_pow) {
        prev = FDBigInteger::From(prev)->Mult(5);
        array.Set(i, prev);
        prev->MakeImmutable();
        i++;
    }
    return array;
}

const Array<IFDBigInteger*>& FDBigInteger::GetPOW_5_CACHE()
{
    static const Array<IFDBigInteger*> POW_5_CACHE = CreatePOW_5_CACHE(
            MAX_FIVE_POW, SMALL_5_POW, ArrayLength(SMALL_5_POW));
    return POW_5_CACHE;
}

COMO_INTERFACE_IMPL_1(FDBigInteger, SyncObject, IFDBigInteger);

ECode FDBigInteger::Constructor(
    /* [in] */ const Array<Integer>& data,
    /* [in] */ Integer offset)
{
    mData = data;
    mOffset = offset;
    mNWords = data.GetLength();
    TrimLeadingZeros();
    return NOERROR;
}

ECode FDBigInteger::Constructor(
    /* [in] */ Long lValue,
    /* [in] */ const Array<Char>& digits,
    /* [in] */ Integer kDigits,
    /* [in] */ Integer nDigits)
{
    Integer n = Math::Max((nDigits + 8) / 9, 2);
    mData = Array<Integer>(n);
    mData[0] = lValue;
    mData[1] = (((ULong)lValue) >> 32);
    mOffset = 0;
    mNWords = 2;
    Integer i = kDigits;
    Integer limit = nDigits - 5;
    Integer v;
    while (i < limit) {
        Integer ilim = i + 5;
        v = digits[i++] - '0';
        while (i < ilim) {
            v = 10 * v + digits[i++] - '0';
        }
        MultAddMe(100000, v);
    }
    Integer factor = 1;
    v = 0;
    while (i < nDigits) {
        v = 10 * v + digits[i++] - '0';
        factor *= 10;
    }
    if (factor != 1) {
        MultAddMe(factor, v);
    }
    TrimLeadingZeros();
    return NOERROR;
}

AutoPtr<IFDBigInteger> FDBigInteger::ValueOfPow52(
    /* [in] */ Integer p5,
    /* [in] */ Integer p2)
{
    if (p5 != 0) {
        if (p2 == 0) {
            return (IFDBigInteger*)Big5pow(p5).Get();
        }
        else if (p5 < ArrayLength(SMALL_5_POW)) {
            Integer pow5 = SMALL_5_POW[p5];
            Integer wordcount = p2 >> 5;
            Integer bitcount = p2 & 0x1f;
            if (bitcount == 0) {
                Array<Integer> data{ pow5 };
                AutoPtr<IFDBigInteger> value;
                CFDBigInteger::New(data, wordcount, IID_IFDBigInteger, (IInterface**)&value);
                return value;
            } else {
                Array<Integer> data{ pow5 << bitcount,
                        (Integer)(((UInteger)pow5) >> (32 - bitcount))};
                AutoPtr<IFDBigInteger> value;
                CFDBigInteger::New(data, wordcount, IID_IFDBigInteger, (IInterface**)&value);
                return value;
            }
        } else {
            AutoPtr<IFDBigInteger> value;
            Big5pow(p5)->LeftShift(p2, value);
            return value;
        }
    }
    else {
        return ValueOfPow2(p2);
    }
}

AutoPtr<IFDBigInteger> FDBigInteger::ValueOfMulPow52(
    /* [in] */ Long value,
    /* [in] */ Integer p5,
    /* [in] */ Integer p2)
{
    CHECK(p5 >= 0);
    CHECK(P2 >= 0);
    Integer v0 = value;
    Integer v1 = (((ULong)value) >> 32);
    Integer wordcount = p2 >> 5;
    Integer bitcount = p2 & 0x1f;
    if (p5 != 0) {
        if (p5 < ArrayLength(SMALL_5_POW)) {
            Long pow5 = SMALL_5_POW[p5] & LONG_MASK;
            Long carry = (v0 & LONG_MASK) * pow5;
            v0 = carry;
            carry = ((ULong)carry) >> 32;
            carry = (v1 & LONG_MASK) * pow5 + carry;
            v1 = carry;
            Integer v2 = (Integer)(((ULong)carry) >> 32);
            if (bitcount == 0) {
                Array<Integer> data{ v0, v1, v2 };
                AutoPtr<IFDBigInteger> fdValue;
                CFDBigInteger::New(data, wordcount, IID_IFDBigInteger, (IInterface**)&fdValue);
                return fdValue;
            }
            else {
                Array<Integer> data{
                    v0 << bitcount,
                    (v1 << bitcount) | (Integer)(((UInteger)v0) >> (32 - bitcount)),
                    (v2 << bitcount) | (Integer)(((UInteger)v1) >> (32 - bitcount)),
                    (Integer)(((UInteger)v2) >> (32 - bitcount)) };
                AutoPtr<IFDBigInteger> fdValue;
                CFDBigInteger::New(data, wordcount, IID_IFDBigInteger, (IInterface**)&fdValue);
                return fdValue;
            }
        }
        else {
            AutoPtr<FDBigInteger> pow5 = From(Big5pow(p5));
            Array<Integer> r;
            if (v1 == 0) {
                r = Array<Integer>(pow5->mNWords + 1 + ((p2 != 0) ? 1 : 0));
                Mult(pow5->mData, pow5->mNWords, v0, r);
            }
            else {
                r = Array<Integer>(pow5->mNWords + 2 + ((p2 != 0) ? 1 : 0));
                Mult(pow5->mData, pow5->mNWords, v0, v1, r);
            }
            AutoPtr<IFDBigInteger> tmpValue, fdValue;
            CFDBigInteger::New(r, pow5->mOffset, IID_IFDBigInteger, (IInterface**)&tmpValue);
            tmpValue->LeftShift(p2, fdValue);
            return fdValue;
        }
    }
    else if (p2 != 0) {
        if (bitcount == 0) {
            Array<Integer> data{ v0, v1 };
            AutoPtr<IFDBigInteger> fdValue;
            CFDBigInteger::New(data, wordcount, IID_IFDBigInteger, (IInterface**)&fdValue);
            return fdValue;
        }
        else {
            Array<Integer> data{
                v0 << bitcount,
                (v1 << bitcount) | (Integer)(((UInteger)v0) >> (32 - bitcount)),
                (Integer)(((UInteger)v1) >> (32 - bitcount)) };
            AutoPtr<IFDBigInteger> fdValue;
            CFDBigInteger::New(data, wordcount, IID_IFDBigInteger, (IInterface**)&fdValue);
            return fdValue;
        }
    }
    Array<Integer> data{ v0, v1 };
    AutoPtr<IFDBigInteger> fdValue;
    CFDBigInteger::New(data, 0, IID_IFDBigInteger, (IInterface**)&fdValue);
    return fdValue;
};


AutoPtr<IFDBigInteger> FDBigInteger::ValueOfPow2(
    /* [in] */ Integer p2)
{
    Integer wordcount = p2 >> 5;
    Integer bitcount = p2 & 0x1f;
    Array<Integer> data{ 1 << bitcount };
    AutoPtr<IFDBigInteger> value;
    CFDBigInteger::New(data, wordcount, IID_IFDBigInteger, (IInterface**)&value);
    return value;
}

void FDBigInteger::TrimLeadingZeros()
{
    Integer i = mNWords;
    if (i > 0 && mData[--i] == 0) {
        while (i > 0 && mData[i - 1] == 0) {
            i--;
        }
        mNWords = i;
        if (i == 0) {
            mOffset = 0;
        }
    }
}

ECode FDBigInteger::GetNormalizationBias(
    /* [out] */ Integer& bias)
{
    if (mNWords == 0) {
        Logger::E("FDBigInteger", "Zero value cannot be normalized");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Integer zeros = Math::NumberOfLeadingZeros(mData[mNWords - 1]);
    bias = (zeros < 4) ? 28 + zeros : zeros - 4;
    return NOERROR;
}

void FDBigInteger::LeftShift(
    /* [in] */ const Array<Integer>& src,
    /* [in] */ Integer idx,
    /* [out] */ Array<Integer>& result,
    /* [in] */ Integer bitcount,
    /* [in] */ Integer anticount,
    /* [in] */ Integer prev)
{
    for (; idx > 0; idx--) {
        Integer v = (prev << bitcount);
        prev = src[idx - 1];
        v |= ((UInteger)prev) >> anticount;
        result[idx] = v;
    }
    Integer v = prev << bitcount;
    result[0] = v;
}

ECode FDBigInteger::LeftShift(
    /* [in] */ Integer shift,
    /* [out] */ AutoPtr<IFDBigInteger>& value)
{
    if (shift == 0 || mNWords == 0) {
        value = this;
        return NOERROR;
    }
    Integer wordcount = shift >> 5;
    Integer bitcount = shift & 0x1f;
    if (mIsImmutable) {
        if (bitcount == 0) {
            Array<Integer> data;
            Arrays::CopyOf(mData, mNWords, &data);
            return CFDBigInteger::New(data, mOffset + wordcount, IID_IFDBigInteger, (IInterface**)&value);
        }
        else {
            Integer anticount = 32 - bitcount;
            Integer idx = mNWords - 1;
            Integer prev = mData[idx];
            Integer hi = ((UInteger)prev) >> anticount;
            Array<Integer> result;
            if (hi != 0) {
                result = Array<Integer>(mNWords + 1);
                result[mNWords] = hi;
            }
            else {
                result = Array<Integer>(mNWords);
            }
            LeftShift(mData, idx, result, bitcount, anticount, prev);
            return CFDBigInteger::New(result, mOffset + wordcount, IID_IFDBigInteger, (IInterface**)&value);
        }
    }
    else {
        if (bitcount != 0) {
            Integer anticount = 32 - bitcount;
            if ((mData[0] << bitcount) == 0) {
                Integer idx = 0;
                Integer prev = mData[idx];
                for (; idx < mNWords - 1; idx++) {
                    Integer v = ((UInteger)prev) >> anticount;
                    prev = mData[idx + 1];
                    v |= (prev << bitcount);
                    mData[idx] = v;
                }
                Integer v = ((UInteger)prev) >> anticount;
                mData[idx] = v;
                if (v == 0) {
                    mNWords--;
                }
                mOffset++;
            }
            else {
                Integer idx = mNWords - 1;
                Integer prev = mData[idx];
                Integer hi = ((UInteger)prev) >> anticount;
                Array<Integer> result = mData;
                Array<Integer> src = mData;
                if (hi != 0) {
                    if (mNWords == mData.GetLength()) {
                        result = Array<Integer>(mNWords + 1);
                        mData = result;
                    }
                    result[mNWords++] = hi;
                }
                LeftShift(src, idx, result, bitcount, anticount, prev);
            }
        }
        mOffset += wordcount;
        value = (IFDBigInteger*)this;
        return NOERROR;
    }
}

ECode FDBigInteger::QuoRemIteration(
    /* [in] */ IFDBigInteger* s,
    /* [out] */ Integer& ret)
{
    CHECK(!mIsImmutable);
    // ensure that this and S have the same number of
    // digits. If S is properly normalized and q < 10 then
    // this must be so.
    Integer thSize = Size();
    Integer sSize = From(s)->Size();
    if (thSize < sSize) {
        // this value is significantly less than S, result of division is zero.
        // just mult this by 10.
        Integer p = MultAndCarryBy10(mData, mNWords, mData);
        if (p != 0) {
            mData[mNWords++] = p;
        }
        else {
            TrimLeadingZeros();
        }
        ret = 0;
        return NOERROR;
    }
    else if (thSize > sSize) {
        Logger::E("FDBigInteger", "disparate values");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // estimate q the obvious way. We will usually be
    // right. If not, then we're only off by a little and
    // will re-add.
    Long q = (mData[mNWords - 1] & LONG_MASK) / (From(s)->mData[From(s)->mNWords - 1] & LONG_MASK);
    Long diff = MultDiffMe(q, From(s));
    if (diff != 0) {
        // q is too big.
        // add S back in until this turns +. This should
        // not be very many times!
        Long sum = 0;
        Integer tStart = From(s)->mOffset - mOffset;
        Array<Integer> sd = From(s)->mData;
        Array<Integer> td = mData;
        while (sum == 0) {
            for (Integer sIndex = 0, tIndex = tStart; tIndex < mNWords; sIndex++, tIndex++) {
                sum += (td[tIndex] & LONG_MASK) + (sd[sIndex] & LONG_MASK);
                td[tIndex] = sum;
                sum = ((ULong)sum) >> 32; // Signed or unsigned, answer is 0 or 1
            }
            //
            // Originally the following line read
            // "if ( sum !=0 && sum != -1 )"
            // but that would be wrong, because of the
            // treatment of the two values as entirely unsigned,
            // it would be impossible for a carry-out to be interpreted
            // as -1 -- it would have to be a single-bit carry-out, or +1.
            //
            CHECK(sum == 0 || sum == 1);
            q -= 1;
        }
    }
    // finally, we can multiply this by 10.
    // it cannot overflow, right, as the high-order word has
    // at least 4 high-order zeros!
    Integer p = MultAndCarryBy10(mData, mNWords, mData);
    CHECK(p == 0);
    TrimLeadingZeros();
    ret = q;
    return NOERROR;
}

ECode FDBigInteger::MultBy10(
    /* [out] */ AutoPtr<IFDBigInteger>& value)
{
    if (mNWords == 0) {
        value = this;
        return NOERROR;
    }
    if (mIsImmutable) {
        Array<Integer> res(mNWords + 1);
        res[mNWords] = MultAndCarryBy10(mData, mNWords, res);
        return CFDBigInteger::New(res, mOffset, IID_IFDBigInteger, (IInterface**)&value);
    }
    else {
        Integer p = MultAndCarryBy10(mData, mNWords, mData);
        if (p != 0) {
            if (mNWords == mData.GetLength()) {
                if (mData[0] == 0) {
                    mData.Copy(0, mData, 1, --mNWords);
                    mOffset++;
                }
                else {
                    Array<Integer> newData;
                    Arrays::CopyOf(mData, mData.GetLength() + 1, &newData);
                    mData = newData;
                }
            }
            mData[mNWords++] = p;
        }
        else {
            TrimLeadingZeros();
        }
        value = this;
        return NOERROR;
    }
}

ECode FDBigInteger::MultByPow52(
    /* [in] */ Integer p5,
    /* [in] */ Integer p2,
    /* [out] */ AutoPtr<IFDBigInteger>& value)
{
    if (mNWords == 0) {
        value = this;
        return NOERROR;
    }
    AutoPtr<IFDBigInteger> res = this;
    if (p5 != 0) {
        Array<Integer> r;
        Integer extraSize = (p2 != 0) ? 1 : 0;
        if (p5 < ArrayLength(SMALL_5_POW)) {
            r = Array<Integer>(mNWords + 1 + extraSize);
            Mult(mData, mNWords, SMALL_5_POW[p5], r);
            AutoPtr<IFDBigInteger> fdValue;
            CFDBigInteger::New(r, mOffset, IID_IFDBigInteger, (IInterface**)&fdValue);
            res = std::move(fdValue);
        }
        else {
            AutoPtr<FDBigInteger> pow5 = From(Big5pow(p5));
            r = Array<Integer>(mNWords + pow5->Size() + extraSize);
            Mult(mData, mNWords, pow5->mData, pow5->mNWords, r);
            AutoPtr<IFDBigInteger> fdValue;
            CFDBigInteger::New(r, mOffset + pow5->mOffset, IID_IFDBigInteger, (IInterface**)&fdValue);
            res = std::move(fdValue);
        }
    }
    return res->LeftShift(p2, value);
}

void FDBigInteger::Mult(
    /* [in] */ const Array<Integer>& s1,
    /* [in] */ Integer s1Len,
    /* [in] */ const Array<Integer>& s2,
    /* [in] */ Integer s2Len,
    /* [out] */ Array<Integer>& dst)
{
    for (Integer i = 0; i < s1Len; i++) {
        Long v = s1[i] & LONG_MASK;
        Long p = 0;
        for (Integer j = 0; j < s2Len; j++) {
            p += (dst[i + j] & LONG_MASK) + v * (s2[j] & LONG_MASK);
            dst[i + j] = p;
            p = ((ULong)p) >> 32;
        }
        dst[i + s2Len] = p;
    }
}

ECode FDBigInteger::LeftInplaceSub(
    /* [in] */ IFDBigInteger* subtrahend,
    /* [out] */ AutoPtr<IFDBigInteger>& value)
{
    CHECK(Size() >= From(subtrahend)->Size());
    AutoPtr<IFDBigInteger> minuend;
    if (mIsImmutable) {
        CFDBigInteger::New(mData.Clone(), mOffset, IID_IFDBigInteger, (IInterface**)&minuend);
    }
    else {
        minuend = this;
    }
    Integer offsetDiff = From(subtrahend)->mOffset - From(minuend)->mOffset;
    Array<Integer> subData = From(subtrahend)->mData;
    Array<Integer> minData = From(minuend)->mData;
    Integer subLen = From(subtrahend)->mNWords;
    Integer minLen = From(minuend)->mNWords;
    if (offsetDiff < 0) {
        // need to expand minuend
        Integer rLen = minLen - offsetDiff;
        if (rLen < minData.GetLength()) {
            minData.Copy(-offsetDiff, minData, 0, minLen);
            Arrays::Fill(minData, 0, -offsetDiff, 0);
        }
        else {
            Array<Integer> r(rLen);
            r.Copy(-offsetDiff, minData, 0, minLen);
            minData = r;
            From(minuend)->mData = r;
        }
        From(minuend)->mOffset = From(subtrahend)->mOffset;
        From(minuend)->mNWords = minLen = rLen;
        offsetDiff = 0;
    }
    Long borrow = 0;
    Integer minIndex = offsetDiff;
    for (Integer subIndex = 0; subIndex < subLen && minIndex < minLen; subIndex++, minIndex++) {
        Long diff = (minData[minIndex] & LONG_MASK) - (subData[subIndex] & LONG_MASK) + borrow;
        minData[minIndex] = diff;
        borrow = diff >> 32; // signed shift
    }
    for (; borrow != 0 && minIndex < minLen; minIndex++) {
        Long diff = (minData[minIndex] & LONG_MASK) + borrow;
        minData[minIndex] = diff;
        borrow = diff >> 32; // signed shift
    }
    CHECK(borrow == 0);
    // result should be positive;
    From(minuend)->TrimLeadingZeros();
    value = minuend;
    return NOERROR;
}

ECode FDBigInteger::RightInplaceSub(
    /* [in] */ IFDBigInteger* subtrahend,
    /* [out] */ AutoPtr<IFDBigInteger>& value)
{
    CHECK(Size() > From(subtrahend)->Size());
    AutoPtr<IFDBigInteger> minuend = this;
    AutoPtr<IFDBigInteger> newSubtrahend;
    if (From(subtrahend)->mIsImmutable) {
        CFDBigInteger::New(From(subtrahend)->mData.Clone(), From(subtrahend)->mOffset,
                IID_IFDBigInteger, (IInterface**)&newSubtrahend);
    }
    else {
        newSubtrahend = subtrahend;
    }
    Integer offsetDiff = From(minuend)->mOffset - From(newSubtrahend)->mOffset;
    Array<Integer> subData = From(newSubtrahend)->mData;
    Array<Integer> minData = From(minuend)->mData;
    Integer subLen = From(newSubtrahend)->mNWords;
    Integer minLen = From(minuend)->mNWords;
    if (offsetDiff < 0) {
        Integer rLen = minLen;
        if (rLen < subData.GetLength()) {
            subData.Copy(-offsetDiff, subData, 0, subLen);
            Arrays::Fill(subData, 0, -offsetDiff, 0);
        }
        else {
            Array<Integer> r(rLen);
            subData.Copy(-offsetDiff, subData, 0, subLen);
            subData = r;
            From(newSubtrahend)->mData = r;
        }
        From(newSubtrahend)->mOffset = From(minuend)->mOffset;
        subLen -= offsetDiff;
        offsetDiff = 0;
    }
    else {
        Integer rLen = minLen + offsetDiff;
        if (rLen >= subData.GetLength()) {
            Arrays::CopyOf(subData, rLen, &subData);
            From(newSubtrahend)->mData = subData;
        }
    }
    Integer subIndex = 0;
    Long borrow = 0;
    for (; subIndex < offsetDiff; subIndex++) {
        Long diff = 0LL - (subData[subIndex] & LONG_MASK) + borrow;
        subData[subIndex] = diff;
        borrow = diff >> 32; // signed shift
    }
    for (Integer minIndex = 0; minIndex < minLen; subIndex++, minIndex++) {
        Long diff = (minData[minIndex] & LONG_MASK) - (subData[subIndex] & LONG_MASK) + borrow;
        subData[subIndex] = diff;
        borrow = diff >> 32; // signed shift
    }
    CHECK(borrow == 0); // borrow out of subtract,
    // result should be positive
    From(newSubtrahend)->mNWords = subIndex;
    From(newSubtrahend)->TrimLeadingZeros();
    value = newSubtrahend;
    return NOERROR;
}

Integer FDBigInteger::CheckZeroTail(
    /* [in] */ const Array<Integer>& a,
    /* [in] */ Integer from)
{
    while (from > 0) {
        if (a[--from] != 0) {
            return 1;
        }
    }
    return 0;
}

ECode FDBigInteger::Cmp(
    /* [in] */ IFDBigInteger* other,
    /* [out] */ Integer& result)
{
    Integer aSize = mNWords + mOffset;
    Integer bSize = From(other)->mNWords + From(other)->mOffset;
    if (aSize > bSize) {
        result = 1;
        return NOERROR;
    }
    else if (aSize < bSize) {
        result = -1;
        return NOERROR;
    }
    Integer aLen = mNWords;
    Integer bLen = From(other)->mNWords;
    while (aLen > 0 && bLen > 0) {
        Integer a = mData[--aLen];
        Integer b = From(other)->mData[--bLen];
        if (a != b) {
            result = ((a & LONG_MASK) < (b & LONG_MASK)) ? -1 : 1;
            return NOERROR;
        }
    }
    if (aLen > 0) {
        result = CheckZeroTail(mData, aLen);
        return NOERROR;
    }
    if (bLen > 0) {
        result = -CheckZeroTail(From(other)->mData, bLen);
        return NOERROR;
    }
    result = 0;
    return NOERROR;
}

ECode FDBigInteger::CmpPow52(
    /* [in] */ Integer p5,
    /* [in] */ Integer p2,
    /* [out] */ Integer& result)
{
    if (p5 == 0) {
        Integer wordcount = p2 >> 5;
        Integer bitcount = p2 & 0x1f;
        Integer size = mNWords + mOffset;
        if (size > wordcount + 1) {
            result = 1;
            return NOERROR;
        }
        else if (size < wordcount + 1) {
            result = -1;
            return NOERROR;
        }
        Integer a = mData[mNWords - 1];
        Integer b = 1 << bitcount;
        if (a != b) {
            result = ((a & LONG_MASK) < (b & LONG_MASK)) ? -1 : 1;
            return NOERROR;
        }
        result = CheckZeroTail(mData, mNWords - 1);
        return NOERROR;
    }
    AutoPtr<IFDBigInteger> fdValue;
    Big5pow(p5)->LeftShift(p2, fdValue);
    return Cmp(fdValue, result);
}

ECode FDBigInteger::AddAndCmp(
    /* [in] */ IFDBigInteger* x,
    /* [in] */ IFDBigInteger* y,
    /* [out] */ Integer& result)
{
    AutoPtr<IFDBigInteger> big, small;
    Integer xSize = From(x)->Size();
    Integer ySize = From(y)->Size();
    Integer bSize, sSize;
    if (xSize >= ySize) {
        big = x;
        small = y;
        bSize = xSize;
        sSize = ySize;
    }
    else {
        big = y;
        small = x;
        bSize = ySize;
        sSize = xSize;
    }
    Integer thSize = Size();
    if (bSize == 0) {
        result = thSize == 0 ? 0 : 1;
        return NOERROR;
    }
    if (sSize == 0) {
        return Cmp(big, result);
    }
    if (bSize > thSize) {
        result = -1;
        return NOERROR;
    }
    if (bSize + 1 < thSize) {
        result = 1;
        return NOERROR;
    }
    Long top = (From(big)->mData[From(big)->mNWords - 1] & LONG_MASK);
    if (sSize == bSize) {
        top += (From(small)->mData[From(small)->mNWords - 1] & LONG_MASK);
    }
    if ((((ULong)top) >> 32) == 0) {
        if ((((ULong)(top + 1)) >> 32) == 0) {
            // good case - no carry extension
            if (bSize < thSize) {
                result = 1;
                return NOERROR;
            }
            // here sum.nWords == this.nWords
            Long v = (mData[mNWords - 1] & LONG_MASK);
            if (v < top) {
                result = -1;
                return NOERROR;
            }
            if (v > top + 1) {
                result = 1;
                return NOERROR;
            }
        }
    }
    else { // (top>>>32)!=0 guaranteed carry extension
        if (bSize + 1 > thSize) {
            result = -1;
            return NOERROR;
        }
        // here sum.nWords == this.nWords
        top = ((ULong)top) >> 32;
        Long v = (mData[mNWords - 1] & LONG_MASK);
        if (v < top) {
            result = -1;
            return NOERROR;
        }
        if (v > top + 1) {
            result = 1;
            return NOERROR;
        }
    }
    return Cmp(From(big)->Add(small), result);
}

ECode FDBigInteger::MakeImmutable()
{
    mIsImmutable = true;
    return NOERROR;
}

AutoPtr<IFDBigInteger> FDBigInteger::Mult(
    /* [in] */ Integer i)
{
    if (mNWords == 0) {
        return this;
    }
    Array<Integer> r(mNWords + 1);
    Mult(mData, mNWords, i, r);
    AutoPtr<IFDBigInteger> value;
    CFDBigInteger::New(r, mOffset, IID_IFDBigInteger, (IInterface**)&value);
    return value;
}

AutoPtr<IFDBigInteger> FDBigInteger::Mult(
    /* [in] */ IFDBigInteger* other)
{
    if (mNWords == 0) {
        return this;
    }
    if (Size() == 1) {
        return From(other)->Mult(mData[0]);
    }
    if (From(other)->mNWords == 0) {
        return other;
    }
    if (From(other)->Size() == 1) {
        return Mult(From(other)->mData[0]);
    }
    Array<Integer> r(mNWords + From(other)->mNWords);
    Mult(mData, mNWords, From(other)->mData, From(other)->mNWords, r);
    AutoPtr<IFDBigInteger> value;
    CFDBigInteger::New(r, mOffset + From(other)->mOffset, IID_IFDBigInteger, (IInterface**)&value);
    return value;
}

AutoPtr<IFDBigInteger> FDBigInteger::Add(
    /* [in] */ IFDBigInteger* other)
{
    AutoPtr<IFDBigInteger> big, small;
    Integer bigLen, smallLen;
    Integer tSize = Size();
    Integer oSize = From(other)->Size();
    if (tSize >= oSize) {
        big = this;
        bigLen = tSize;
        small = other;
        smallLen = oSize;
    }
    else {
        big = other;
        bigLen = oSize;
        small = this;
        smallLen = tSize;
    }
    Array<Integer> r(bigLen + 1);
    Integer i = 0;
    Long carry = 0;
    for (; i < smallLen; i++) {
        carry += (i < From(big)->mOffset ? 0 : (From(big)->mData[i - From(big)->mOffset] & LONG_MASK))
                + (i < From(small)->mOffset ? 0 : (From(small)->mData[i - From(small)->mOffset] & LONG_MASK));
        r[i] = carry;
        carry = carry >> 32; // signed shift.
    }
    for (; i < bigLen; i++) {
        carry += (i < From(big)->mOffset ? 0 : (From(big)->mData[i - From(big)->mOffset] & LONG_MASK));
        r[i] = carry;
        carry = carry >> 32; // signed shift.
    }
    r[bigLen] = carry;
    AutoPtr<IFDBigInteger> fdValue;
    CFDBigInteger::New(r, 0, IID_IFDBigInteger, (IInterface**)&fdValue);
    return fdValue;
}

void FDBigInteger::MultAddMe(
    /* [in] */ Integer iv,
    /* [in] */ Integer addend)
{
    Long v = iv & LONG_MASK;
    // unroll 0th iteration, doing addition.
    Long p = v * (mData[0] & LONG_MASK) + (addend & LONG_MASK);
    mData[0] = p;
    p = ((ULong)p) >> 32;
    for (Integer i = 1; i < mNWords; i++) {
        p += v * (mData[i] & LONG_MASK);
        mData[i] = p;
        p = ((ULong)p) >> 32;
    }
    if (p != 0) {
        mData[mNWords++] = p; // will fail noisily if illegal!
    }
}

Long FDBigInteger::MultDiffMe(
    /* [in] */ Long q,
    /* [in] */ FDBigInteger* s)
{
    Long diff = 0;
    if (q != 0) {
        Integer deltaSize = From(s)->mOffset - mOffset;
        if (deltaSize >= 0) {
            Array<Integer> sd = From(s)->mData;
            Array<Integer> td = mData;
            for (Integer sIndex = 0, tIndex = deltaSize; sIndex < From(s)->mNWords; sIndex++, tIndex++) {
                diff += (td[tIndex] & LONG_MASK) - q * (sd[sIndex] & LONG_MASK);
                td[tIndex] = diff;
                diff = diff >> 32; // signed shift.
            }
        }
        else {
            deltaSize = -deltaSize;
            Array<Integer> rd(mNWords + deltaSize);
            Integer sIndex = 0;
            Integer rIndex = 0;
            Array<Integer> sd = From(s)->mData;
            for (; rIndex < deltaSize && sIndex < From(s)->mNWords; sIndex++, rIndex++) {
                diff -= q * (sd[sIndex] & LONG_MASK);
                rd[rIndex] = diff;
                diff = diff >> 32; // signed shift.
            }
            Integer tIndex = 0;
            Array<Integer> td = mData;
            for (; sIndex < From(s)->mNWords; sIndex++, tIndex++, rIndex++) {
                diff += (td[tIndex] & LONG_MASK) - q * (sd[sIndex] & LONG_MASK);
                rd[rIndex] = diff;
                diff = diff >> 32; // signed shift.
            }
            mNWords += deltaSize;
            mOffset -= deltaSize;
            mData = rd;
        }
    }
    return diff;
}

Integer FDBigInteger::MultAndCarryBy10(
    /* [in] */ const Array<Integer>& src,
    /* [in] */ Integer srcLen,
    /* [out] */ Array<Integer>& dst)
{
    Long carry = 0;
    for (Integer i = 0; i < srcLen; i++) {
        Long product = (src[i] & LONG_MASK) * 10LL + carry;
        dst[i] = product;
        carry = ((ULong)product) >> 32;
    }
    return carry;
}

void FDBigInteger::Mult(
    /* [in] */ const Array<Integer>& src,
    /* [in] */ Integer srcLen,
    /* [in] */ Integer value,
    /* [out] */ Array<Integer>& dst)
{
    Long val = value & LONG_MASK;
    Long carry = 0;
    for (Integer i = 0; i < srcLen; i++) {
        Long product = (src[i] & LONG_MASK) * val + carry;
        dst[i] = product;
        carry = ((ULong)product) >> 32;
    }
    dst[srcLen] = carry;
}

void FDBigInteger::Mult(
    /* [in] */ const Array<Integer>& src,
    /* [in] */ Integer srcLen,
    /* [in] */ Integer v0,
    /* [in] */ Integer v1,
    /* [out] */ Array<Integer>& dst)
{
    Long v = v0 & LONG_MASK;
    Long carry = 0;
    for (Integer j = 0; j < srcLen; j++) {
        Long product = v * (src[j] & LONG_MASK) + carry;
        dst[j] = product;
        carry = ((ULong)product) >> 32;
    }
    dst[srcLen] = carry;
    v = v1 & LONG_MASK;
    carry = 0;
    for (Integer j = 0; j < srcLen; j++) {
        Long product = (dst[j + 1] & LONG_MASK) + v * (src[j] & LONG_MASK) + carry;
        dst[j + 1] = product;
        carry = ((ULong)product) >> 32;
    }
    dst[srcLen + 1] = carry;
}

AutoPtr<IFDBigInteger> FDBigInteger::Big5pow(
    /* [in] */ Integer p)
{
    CHECK(p >= 0);
    if (p < MAX_FIVE_POW) {
        return GetPOW_5_CACHE()[p];
    }
    return Big5powRec(p);
}

AutoPtr<IFDBigInteger> FDBigInteger::Big5powRec(
    /* [in] */ Integer p)
{
    if (p < MAX_FIVE_POW) {
        return GetPOW_5_CACHE()[p];
    }
    // construct the value.
    // recursively.
    Integer q, r;
    // in order to compute 5^p,
    // compute its square root, 5^(p/2) and square.
    // or, let q = p / 2, r = p -q, then
    // 5^p = 5^(q+r) = 5^q * 5^r
    q = p >> 1;
    r = p - q;
    AutoPtr<IFDBigInteger> bigq = Big5powRec(q);
    if (r < ArrayLength(SMALL_5_POW)) {
        return From(bigq)->Mult(SMALL_5_POW[r]);
    }
    else {
        return From(bigq)->Mult(Big5powRec(r));
    }
}

ECode FDBigInteger::ToHexString(
    /* [out] */ String& str)
{
    if (mNWords == 0) {
        str = "0";
        return NOERROR;
    }
    AutoPtr<IStringBuilder> sb;
    CStringBuilder::New((mNWords + mOffset) * 8, IID_IStringBuilder, (IInterface**)&sb);
    for (Integer i = mNWords - 1; i >= 0; i--) {
        String subStr = StringUtils::ToHexString(mData[i]);
        for (Integer j = subStr.GetLength(); j < 8; j++) {
            sb->Append(U'0');
        }
        sb->Append(subStr);
    }
    for (Integer i = mOffset; i > 0; i--) {
        sb->Append("00000000");
    }
    return sb->ToString(str);
}

ECode FDBigInteger::ToBigInteger(
    /* [out] */ AutoPtr<IBigInteger>& value)
{
    Array<Byte> magnitude(mNWords * 4 + 1);
    for (Integer i = 0; i < mNWords; i++) {
        Integer w = mData[i];
        magnitude[magnitude.GetLength() - 4 * i - 1] = w;
        magnitude[magnitude.GetLength() - 4 * i - 2] = (w >> 8);
        magnitude[magnitude.GetLength() - 4 * i - 3] = (w >> 16);
        magnitude[magnitude.GetLength() - 4 * i - 4] = (w >> 24);
    }
    AutoPtr<IBigInteger> bi;
    CBigInteger::New(magnitude, IID_IBigInteger, (IInterface**)&bi);
    return bi->ShiftLeft(mOffset * 32, value);
}

ECode FDBigInteger::ToString(
    /* [out] */ String& str)
{
    AutoPtr<IBigInteger> value;
    ToBigInteger(value);
    return IObject::Probe(value)->ToString(str);
}

}
}
