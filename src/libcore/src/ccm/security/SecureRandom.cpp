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

#include "ccm/security/CSecureRandom.h"
#include "ccm/security/SecureRandom.h"
#include "ccm/security/SecureRandomSpi.h"
#include <ccmlogger.h>

namespace ccm {
namespace security {

CCM_INTERFACE_IMPL_1(SecureRandom, Random, ISecureRandom);

AutoPtr<ISecureRandom> SecureRandom::sSeedGenerator;

ECode SecureRandom::Constructor()
{
    Random::Constructor(0);
    return GetDefaultPRNG(true, Array<Byte>::Null());
}

ECode SecureRandom::Constructor(
    /* [in] */ const Array<Byte>& seed)
{
    Random::Constructor(0);
    return GetDefaultPRNG(true, seed);
}

ECode SecureRandom::GetDefaultPRNG(
    /* [in] */ Boolean setSeed,
    /* [in] */ const Array<Byte>& seed)
{
    String prng = GetPrngAlgorithm();
    if (prng.IsNull()) {
        Logger::E("SecureRandom", "No SecureRandom implementation!");
        return ccm::core::E_ILLEGAL_STATE_EXCEPTION;
    }
    else  {
        AutoPtr<ISecureRandom> random;
        ECode ec = SecureRandom::GetInstance(prng, &random);
        if (FAILED(ec)) {
            return E_RUNTIME_EXCEPTION;
        }
        mSecureRandomSpi = From(random)->GetSecureRandomSpi();
        random->GetProvider(&mProvider);
        if (setSeed) {
            SecureRandomSpi::From(mSecureRandomSpi)->EngineSetSeed(seed);
        }
    }
    // Implementations subclass SecureRandom instead of
    // SecureRandomSpi. They will also go through this code path because
    // they must call a SecureRandom constructor as it is their superclass.
    // If we are dealing with such an implementation, do not set the
    // algorithm value as it would be inaccurate.
    CoclassID cid;
    GetCoclassID(&cid);
    if (cid == CID_CSecureRandom) {
        mAlgorithm = prng;
    }
    return NOERROR;
}

ECode SecureRandom::Constructor(
    /* [in] */ ISecureRandomSpi* secureRandomSpi,
    /* [in] */ IProvider* provider,
    /* [in] */ const String& algorithm)
{
    Random::Constructor(0);
    mSecureRandomSpi = secureRandomSpi;
    mProvider = provider;
    mAlgorithm = algorithm;
    return NOERROR;
}

ECode SecureRandom::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ ISecureRandom** sr)
{
    return NOERROR;
}

ECode SecureRandom::GetProvider(
    /* [out] */ IProvider** provider)
{
    VALIDATE_NOT_NULL(provider);

    *provider = mProvider;
    REFCOUNT_ADD(*provider);
    return NOERROR;
}

ECode SecureRandom::GetSeed(
    /* [in] */ Integer numBytes,
    /* [out, callee] */ Array<Byte>* seed)
{
    if (sSeedGenerator == nullptr) {

    }
    return NOERROR;
}

}
}
