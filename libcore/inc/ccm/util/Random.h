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

#ifndef __CCM_UTIL_RANDOM_H__
#define __CCM_UTIL_RANDOM_H__

#include "ccm/core/SyncObject.h"
#include "ccm.io.ISerializable.h"
#include "ccm.util.IRandom.h"
#include "ccm.util.concurrent.atomic.IAtomicLong.h"
#include <ccmautoptr.h>

using ccm::core::SyncObject;
using ccm::io::ISerializable;
using ccm::util::concurrent::atomic::IAtomicLong;

namespace ccm {
namespace util {

class Random
    : public SyncObject
    , public IRandom
    , public ISerializable
{
public:
    CCM_INTERFACE_DECL();

    ECode Constructor();

    ECode Constructor(
        /* [in] */ Long seed);

    ECode SetSeed(
        /* [in] */ Long seed) override;

    ECode NextBytes(
        /* [out] */ Array<Byte>& bytes) override;

    ECode NextInteger(
        /* [out] */ Integer* value) override;

    ECode NextInteger(
        /* [in] */ Integer bound,
        /* [out] */ Integer* value) override;

    ECode NextLong(
        /* [out] */ Long* value) override;

    ECode NextBoolean(
        /* [out] */ Boolean* value) override;

    ECode NextFloat(
        /* [out] */ Float* value) override;

    ECode NextDouble(
        /* [out] */ Double* value) override;

    ECode NextGaussian(
        /* [out] */ Double* value) override;

protected:
    virtual Integer Next(
        /* [in] */ Integer bits);

private:
    static Long SeedUniquifier();

    static AutoPtr<IAtomicLong> GetSeedUniquifier();

    static Long InitialScramble(
        /* [in] */ Long seed);

private:
    AutoPtr<IAtomicLong> mSeed;

    static constexpr Long sMultiplier = 0x5DEECE66Dll;
    static constexpr Long sAddend = 0xBll;
    static constexpr Long sMask = (1ll << 48) - 1;

    static constexpr Double DOUBLE_UNIT = 0x1.0p-53;

    Double mNextNextGaussian = 0;
    Boolean mHaveNextNextGaussian = false;
};

}
}

#endif // __CCM_UTIL_RANDOM_H__
