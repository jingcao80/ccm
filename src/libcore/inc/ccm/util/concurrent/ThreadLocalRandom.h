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

#ifndef __CCM_UTIL_CONCURRENT_THREADLOCALRANDOM_H__
#define __CCM_UTIL_CONCURRENT_THREADLOCALRANDOM_H__

#include "ccm/util/Random.h"
#include "ccm.core.IThreadLocal.h"
#include "ccm.util.concurrent.IThreadLocalRandom.h"
#include "ccm.util.concurrent.atomic.IAtomicLong.h"

using ccm::core::IThreadLocal;
using ccm::util::concurrent::atomic::IAtomicLong;

namespace ccm {
namespace util {
namespace concurrent {

class ThreadLocalRandom
    : public Random
    , public IThreadLocalRandom
{
public:
    CCM_INTERFACE_DECL();

    static void LocalInit();

    static AutoPtr<IThreadLocalRandom> GetCurrent();

    ECode SetSeed(
        /* [in] */ Long seed) override;

    Long NextSeed();

    Long InternalNextLong(
        /* [in] */ Long origin,
        /* [in] */ Long bound);

    Integer InternalNextInt(
        /* [in] */ Integer origin,
        /* [in] */ Integer bound);

    Double InternalNextDouble(
        /* [in] */ Double origin,
        /* [in] */ Double bound);

    ECode NextInt(
        /* [out] */ Integer* value) override;

    ECode NextInt(
        /* [in] */ Integer bound,
        /* [out] */ Integer* value) override;

    ECode NextInt(
        /* [in] */ Integer origin,
        /* [in] */ Integer bound,
        /* [out] */ Integer* value) override;

    ECode NextLong(
        /* [out] */ Long* value) override;

    ECode NextLong(
        /* [in] */ Long bound,
        /* [out] */ Long* value) override;

    ECode NextLong(
        /* [in] */ Long origin,
        /* [in] */ Long bound,
        /* [out] */ Long* value) override;

    ECode NextDouble(
        /* [out] */ Double* value) override;

    ECode NextDouble(
        /* [in] */ Double bound,
        /* [out] */ Double* value) override;

    ECode NextDouble(
        /* [in] */ Double origin,
        /* [in] */ Double bound,
        /* [out] */ Double* value) override;

    ECode NextBoolean(
        /* [out] */ Boolean* value) override;

    ECode NextFloat(
        /* [out] */ Float* value) override;

    ECode NextGaussian(
        /* [out] */ Double* value) override;

    static Integer GetProbe();

    static Integer AdvanceProbe(
        /* [in] */ Integer probe);

    static Integer NextSecondarySeed();

protected:
    virtual Integer Next(
        /* [in] */ Integer bits);

private:
    ThreadLocalRandom();

    /** Rarely-used holder for the second of a pair of Gaussians */
    static AutoPtr<IThreadLocal> GetNextLocalGaussian();

    static AutoPtr<IAtomicLong> GetSeeder();

    static void StaticInitialize();

    static Long Mix64(
        /* [in] */ Long z);

    static Integer Mix32(
        /* [in] */ Long z);

public:
    Boolean mInitialized = false;

private:
    /**
     * The seed increment.
     */
    static constexpr Long GAMMA = 0x9e3779b97f4a7c15ll;

    /**
     * The increment for generating probe values.
     */
    static constexpr Integer PROBE_INCREMENT = 0x9e3779b9;

    /**
     * The increment of seeder per new instance.
     */
    static constexpr Long SEEDER_INCREMENT = 0xbb67ae8584caa73bll;

    // Constants from SplittableRandom
    static constexpr Double DOUBLE_UNIT = 0x1.0p-53;  // 1.0  / (1L << 53)
    static constexpr Float FLOAT_UNIT  = 0x1.0p-24f; // 1.0f / (1 << 24)
};

inline ThreadLocalRandom::ThreadLocalRandom()
    : mInitialized(true)
{}

}
}
}

#endif // __CCM_UTIL_CONCURRENT_THREADLOCALRANDOM_H__
