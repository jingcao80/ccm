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

#include "ccm/core/SyncObject.h"
#include "ccm.misc.IFDBigInteger.h"

using ccm::core::SyncObject;

namespace ccm {
namespace misc {

class FDBigInteger
    : public SyncObject
    , public IFDBigInteger
{
public:
    CCM_INTERFACE_DECL();

    ECode Constructor(
        /* [in] */ Long lValue,
        /* [in] */ const Array<Char>& digits,
        /* [in] */ Integer kDigits,
        /* [in] */ Integer nDigits)
    {
        return NOERROR;
    }


    ECode AddAndCmp(
        /* [in] */ IFDBigInteger* x,
        /* [in] */ IFDBigInteger* y,
        /* [out] */ Integer* result) override
    {
        return NOERROR;
    }

    ECode Cmp(
        /* [in] */ IFDBigInteger* other,
        /* [out] */ Integer* result) override
    {
        return NOERROR;
    }

    ECode CmpPow52(
        /* [in] */ Integer p5,
        /* [in] */ Integer p2,
        /* [out] */ Integer* result) override
    {
        return NOERROR;
    }

    ECode GetNormalizationBias(
        /* [out] */ Integer* bias) override
    {
        return NOERROR;
    }

    ECode LeftShift(
        /* [in] */ Integer shift,
        /* [out] */ IFDBigInteger** value) override
    {
        return NOERROR;
    }

    ECode LeftInplaceSub(
        /* [in] */ IFDBigInteger* subtrahend,
        /* [out] */ IFDBigInteger** value) override
    {
        return NOERROR;
    }

    ECode MakeImmutable() override
    {
        return NOERROR;
    }

    ECode MultBy10(
        /* [out] */ IFDBigInteger** value) override
    {
        return NOERROR;
    }

    ECode MultByPow52(
        /* [in] */ Integer p5,
        /* [in] */ Integer p2,
        /* [out] */ IFDBigInteger** value) override
    {
        return NOERROR;
    }

    ECode QuoRemIteration(
        /* [in] */ IFDBigInteger* s,
        /* [out] */ Integer* q) override
    {
        return NOERROR;
    }

    ECode RightInplaceSub(
        /* [in] */ IFDBigInteger* subtrahend,
        /* [out] */ IFDBigInteger** value) override
    {
        return NOERROR;
    }



    static AutoPtr<IFDBigInteger> ValueOfPow52(
        /* [in] */ Integer p5,
        /* [in] */ Integer p2)
    {
        return nullptr;
    }

    static AutoPtr<IFDBigInteger> ValueOfMulPow52(
        /* [in] */ Long value,
        /* [in] */ Integer p5,
        /* [in] */ Integer p2)
    {
        return nullptr;
    }

    static const AutoPtr<IFDBigInteger> GetZERO()
    {
        return nullptr;
    }

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
};

}
}