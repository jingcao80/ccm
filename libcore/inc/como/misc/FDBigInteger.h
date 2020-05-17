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

#ifndef __COMO_MISC_FDBIGINTEGER_H__
#define __COMO_MISC_FDBIGINTEGER_H__

#include "como/core/SyncObject.h"
#include "como.math.IBigInteger.h"
#include "como.misc.IFDBigInteger.h"

using como::core::SyncObject;
using como::math::IBigInteger;

namespace como {
namespace misc {

static Array<IFDBigInteger*>
CreatePOW_5_CACHE(Integer max_five_pow, const Integer[], Integer);

class FDBigInteger
    : public SyncObject
    , public IFDBigInteger
{
public:
    COMO_INTERFACE_DECL();

    ECode Constructor(
        /* [in] */ Long lValue,
        /* [in] */ const Array<Char>& digits,
        /* [in] */ Integer kDigits,
        /* [in] */ Integer nDigits);

    static AutoPtr<IFDBigInteger> ValueOfPow52(
        /* [in] */ Integer p5,
        /* [in] */ Integer p2);

    static AutoPtr<IFDBigInteger> ValueOfMulPow52(
        /* [in] */ Long value,
        /* [in] */ Integer p5,
        /* [in] */ Integer p2);

    ECode GetNormalizationBias(
        /* [out] */ Integer& bias) override;

    ECode LeftShift(
        /* [in] */ Integer shift,
        /* [out] */ AutoPtr<IFDBigInteger>& value) override;

    ECode QuoRemIteration(
        /* [in] */ IFDBigInteger* s,
        /* [out] */ Integer& q) override;

    ECode MultBy10(
        /* [out] */ AutoPtr<IFDBigInteger>& value) override;

    ECode MultByPow52(
        /* [in] */ Integer p5,
        /* [in] */ Integer p2,
        /* [out] */ AutoPtr<IFDBigInteger>& value) override;

    ECode LeftInplaceSub(
        /* [in] */ IFDBigInteger* subtrahend,
        /* [out] */ AutoPtr<IFDBigInteger>& value) override;

    ECode RightInplaceSub(
        /* [in] */ IFDBigInteger* subtrahend,
        /* [out] */ AutoPtr<IFDBigInteger>& value) override;

    ECode Cmp(
        /* [in] */ IFDBigInteger* other,
        /* [out] */ Integer& result) override;

    ECode CmpPow52(
        /* [in] */ Integer p5,
        /* [in] */ Integer p2,
        /* [out] */ Integer& result) override;

    ECode AddAndCmp(
        /* [in] */ IFDBigInteger* x,
        /* [in] */ IFDBigInteger* y,
        /* [out] */ Integer& result) override;

    ECode MakeImmutable() override;

    ECode ToHexString(
        /* [out] */ String& str) override;

    ECode ToBigInteger(
        /* [out] */ AutoPtr<IBigInteger>& value) override;

    ECode ToString(
        /* [out] */ String& str) override;

    static const AutoPtr<IFDBigInteger> GetZERO();

protected:
    ECode Constructor(
        /* [in] */ const Array<Integer>& data,
        /* [in] */ Integer offset);

private:
    static AutoPtr<IFDBigInteger> ValueOfPow2(
        /* [in] */ Integer p2);

    void TrimLeadingZeros();

    static void LeftShift(
        /* [in] */ const Array<Integer>& src,
        /* [in] */ Integer idx,
        /* [out] */ Array<Integer>& result,
        /* [in] */ Integer bitcount,
        /* [in] */ Integer anticount,
        /* [in] */ Integer prev);

    Integer Size();

    static void Mult(
        /* [in] */ const Array<Integer>& s1,
        /* [in] */ Integer s1Len,
        /* [in] */ const Array<Integer>& s2,
        /* [in] */ Integer s2Len,
        /* [out] */ Array<Integer>& dst);

    static Integer CheckZeroTail(
        /* [in] */ const Array<Integer>& a,
        /* [in] */ Integer from);

    AutoPtr<IFDBigInteger> Mult(
        /* [in] */ Integer i);

    AutoPtr<IFDBigInteger> Mult(
        /* [in] */ IFDBigInteger* other);

    AutoPtr<IFDBigInteger> Add(
        /* [in] */ IFDBigInteger* other);

    void MultAddMe(
        /* [in] */ Integer iv,
        /* [in] */ Integer addend);

    Long MultDiffMe(
        /* [in] */ Long q,
        /* [in] */ FDBigInteger* s);

    Integer MultAndCarryBy10(
        /* [in] */ const Array<Integer>& src,
        /* [in] */ Integer srcLen,
        /* [out] */ Array<Integer>& dst);

    static void Mult(
        /* [in] */ const Array<Integer>& src,
        /* [in] */ Integer srcLen,
        /* [in] */ Integer value,
        /* [out] */ Array<Integer>& dst);

    static void Mult(
        /* [in] */ const Array<Integer>& src,
        /* [in] */ Integer srcLen,
        /* [in] */ Integer v0,
        /* [in] */ Integer v1,
        /* [out] */ Array<Integer>& dst);

    static AutoPtr<IFDBigInteger> Big5pow(
        /* [in] */ Integer p);

    static AutoPtr<IFDBigInteger> Big5powRec(
        /* [in] */ Integer p);

    static const Array<IFDBigInteger*>& GetPOW_5_CACHE();

    static FDBigInteger* From(
        /* [in] */ IFDBigInteger* obj);

public:
    static constexpr Integer SMALL_5_POW[] = {
            1,
            5,
            5 * 5,
            5 * 5 * 5,
            5 * 5 * 5 * 5,
            5 * 5 * 5 * 5 * 5,
            5 * 5 * 5 * 5 * 5 * 5,
            5 * 5 * 5 * 5 * 5 * 5 * 5,
            5 * 5 * 5 * 5 * 5 * 5 * 5 * 5,
            5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5,
            5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5,
            5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5,
            5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5,
            5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5
    };

    static constexpr Long LONG_5_POW[] = {
            1LL,
            5LL,
            5LL * 5,
            5LL * 5 * 5,
            5LL * 5 * 5 * 5,
            5LL * 5 * 5 * 5 * 5,
            5LL * 5 * 5 * 5 * 5 * 5,
            5LL * 5 * 5 * 5 * 5 * 5 * 5,
            5LL * 5 * 5 * 5 * 5 * 5 * 5 * 5,
            5LL * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5,
            5LL * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5,
            5LL * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5,
            5LL * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5,
            5LL * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5,
            5LL * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5,
            5LL * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5,
            5LL * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5,
            5LL * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5,
            5LL * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5,
            5LL * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5,
            5LL * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5,
            5LL * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5,
            5LL * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5,
            5LL * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5,
            5LL * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5,
            5LL * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5,
            5LL * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5,
    };

private:
    friend Array<IFDBigInteger*> CreatePOW_5_CACHE(
            Integer max_five_pow, const Integer[], Integer);

    // Maximum size of cache of powers of 5 as FDBigIntegers.
    static constexpr Integer MAX_FIVE_POW = 340;
;
    // Constant for casting an int to a long via bitwise AND.
    static constexpr Long LONG_MASK = 0xffffffffLL;

    Array<Integer> mData;
    Integer mOffset;
    Integer mNWords;
    Boolean mIsImmutable = false;
};

inline Integer FDBigInteger::Size()
{
    return mNWords + mOffset;
}

inline FDBigInteger* FDBigInteger::From(
    /* [in] */ IFDBigInteger* obj)
{
    return (FDBigInteger*)obj;
}

}
}

#endif // __COMO_MISC_FDBIGINTEGER_H__
