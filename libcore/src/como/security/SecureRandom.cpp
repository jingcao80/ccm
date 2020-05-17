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
#include "como/core/AutoLock.h"
#include "como/security/CSecureRandom.h"
#include "como/security/SecureRandom.h"
#include "como/security/SecureRandomSpi.h"
#include "como/security/cca/InstanceFactory.h"
#include "como/security/cca/Providers.h"
#include "como.security.IProvider.h"
#include "como.security.IProviderService.h"
#include "como.security.cca.IInstance.h"
#include "como.security.cca.IProviderList.h"
#include "como.util.IIterator.h"
#include "como.util.IList.h"
#include "como.util.ISet.h"
#include <comosp.h>
#include <comolog.h>

using como::core::AutoLock;
using como::security::cca::InstanceFactory;
using como::security::cca::IInstance;
using como::security::cca::IProviderList;
using como::security::cca::Providers;
using como::util::IIterator;
using como::util::IList;
using como::util::ISet;

namespace como {
namespace security {

COMO_INTERFACE_IMPL_1(SecureRandom, Random, ISecureRandom);

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
        return como::core::E_ILLEGAL_STATE_EXCEPTION;
    }
    else  {
        AutoPtr<ISecureRandom> random;
        ECode ec = SecureRandom::GetInstance(prng, random);
        if (FAILED(ec)) {
            return E_RUNTIME_EXCEPTION;
        }
        mSecureRandomSpi = From(random)->GetSecureRandomSpi();
        random->GetProvider(mProvider);
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
    GetCoclassID(cid);
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
    /* [out] */ AutoPtr<ISecureRandom>& sr)
{
    AutoPtr<IInstance> instance;
    FAIL_RETURN(InstanceFactory::GetInstance(String("CSecureRandom"),
            IID_ISecureRandomSpi, algorithm, &instance));
    AutoPtr<IInterface> impl;
    instance->GetImpl(impl);
    AutoPtr<IProvider> provider;
    instance->GetProvider(provider);
    return CSecureRandom::New(ISecureRandomSpi::Probe(impl),
            provider, algorithm, IID_ISecureRandom, (IInterface**)&sr);
}

ECode SecureRandom::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ const String& provider,
    /* [out] */ AutoPtr<ISecureRandom>& sr)
{
    AutoPtr<IInstance> instance;
    ECode ec = InstanceFactory::GetInstance(String("CSecureRandom"),
            IID_ISecureRandomSpi, algorithm, provider, &instance);
    if (FAILED(ec)) {
        if (provider.Equals("Crypto")) {
            Logger::E("SecureRandom", "COMO no longer support the Crypto provider.");
        }
        return ec;
    }
    AutoPtr<IInterface> impl;
    instance->GetImpl(impl);
    AutoPtr<IProvider> providerObj;
    instance->GetProvider(providerObj);
    return CSecureRandom::New(ISecureRandomSpi::Probe(impl),
            providerObj, algorithm, IID_ISecureRandom, (IInterface**)&sr);
}

ECode SecureRandom::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ IProvider* provider,
    /* [out] */ AutoPtr<ISecureRandom>& sr)
{
    AutoPtr<IInstance> instance;
    FAIL_RETURN(InstanceFactory::GetInstance(String("CSecureRandom"),
            IID_ISecureRandomSpi, algorithm, provider, &instance));
    AutoPtr<IInterface> impl;
    instance->GetImpl(impl);
    AutoPtr<IProvider> providerObj;
    instance->GetProvider(providerObj);
    return CSecureRandom::New(ISecureRandomSpi::Probe(impl),
            providerObj, algorithm, IID_ISecureRandom, (IInterface**)&sr);
}

ECode SecureRandom::GetProvider(
    /* [out] */ AutoPtr<IProvider>& provider)
{
    provider = mProvider;
    return NOERROR;
}

ECode SecureRandom::GetAlgorithm(
    /* [out] */ String& algorithm)
{
    algorithm = !mAlgorithm.IsNull() ? mAlgorithm : "unknown";
    return NOERROR;
}

ECode SecureRandom::SetSeed(
    /* [in] */ const Array<Byte>& seed)
{
    AutoLock lock(this);
    return SecureRandomSpi::From(mSecureRandomSpi)->EngineSetSeed(seed);
}

ECode SecureRandom::SetSeed(
    /* [in] */ Long seed)
{
    /*
     * Ignore call from super constructor (as well as any other calls
     * unfortunate enough to be passing 0).  It's critical that we
     * ignore call from superclass constructor, as digest has not
     * yet been initialized at that point.
     */
    if (seed != 0) {
        return SecureRandomSpi::From(mSecureRandomSpi)->EngineSetSeed(
                LongToByteArray(seed));
    }
    return NOERROR;
}

ECode SecureRandom::NextBytes(
    /* [out] */ Array<Byte>& bytes)
{
    AutoLock lock(this);
    return SecureRandomSpi::From(mSecureRandomSpi)->EngineNextBytes(bytes);
}

Integer SecureRandom::Next(
    /* [in] */ Integer bits)
{
    Integer numBytes = (bits + 7) / 8;
    Array<Byte> b(numBytes);
    Integer next = 0;

    NextBytes(b);
    for (Integer i = 0; i < numBytes; i++) {
        next = (next << 8) + (b[i] & 0xff);
    }

    return (UInteger)next >> (numBytes * 8 - bits);
}

ECode SecureRandom::GetSeed(
    /* [in] */ Integer numBytes,
    /* [out, callee] */ Array<Byte>* seed)
{
    if (sSeedGenerator == nullptr) {
        CSecureRandom::New(IID_ISecureRandom, (IInterface**)&sSeedGenerator);
    }
    return sSeedGenerator->GenerateSeed(numBytes, seed);
}

ECode SecureRandom::GenerateSeed(
    /* [in] */ Integer numBytes,
    /* [out, callee] */ Array<Byte>* seed)
{
    return SecureRandomSpi::From(mSecureRandomSpi)->EngineGenerateSeed(numBytes, seed);
}

Array<Byte> SecureRandom::LongToByteArray(
    /* [in] */ Long l)
{
    Array<Byte> retVal(8);

    for (Integer i = 0; i < 8; i++) {
        retVal[i] = (Byte) l;
        l >>= 8;
    }

    return retVal;
}

String SecureRandom::GetPrngAlgorithm()
{
    AutoPtr<IProviderList> pl = Providers::GetProviderList();
    AutoPtr<IList> ps;
    pl->Providers(ps);
    FOR_EACH(IProvider*, p, IProvider::Probe, ps) {
        AutoPtr<ISet> ss;
        p->GetServices(ss);
        FOR_EACH(IProviderService*, s, IProviderService::Probe, ss) {
            String type;
            s->GetType(type);
            if (type.Equals("CSecureRandom")) {
                String algorithm;
                s->GetAlgorithm(algorithm);
                return algorithm;
            }
        } END_FOR_EACH();
    } END_FOR_EACH();
    return String();
}

}
}
