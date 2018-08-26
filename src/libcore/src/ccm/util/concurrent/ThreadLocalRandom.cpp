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

#include "ccm/core/CoreUtils.h"
#include "ccm/core/CThread.h"
#include "ccm/core/Math.h"
#include "ccm/core/System.h"
#include "ccm/core/volatile.h"
#include "ccm/security/AccessController.h"
#include "ccm/security/CSecureRandom.h"
#include "ccm/util/concurrent/ThreadLocalRandom.h"
#include "ccm/util/concurrent/atomic/CAtomicInteger.h"
#include "ccm/util/concurrent/atomic/CAtomicLong.h"
#include "ccm.core.IBoolean.h"
#include "ccm.security.IPrivilegedAction.h"

using ccm::core::CoreUtils;
using ccm::core::CThread;
using ccm::core::IBoolean;
using ccm::core::IThread;
using ccm::core::Math;
using ccm::core::System;
using ccm::security::AccessController;
using ccm::security::CSecureRandom;
using ccm::security::IPrivilegedAction;
using ccm::security::IID_IPrivilegedAction;
using ccm::util::concurrent::atomic::CAtomicInteger;
using ccm::util::concurrent::atomic::CAtomicLong;
using ccm::util::concurrent::atomic::IAtomicInteger;
using ccm::util::concurrent::atomic::IAtomicLong;
using ccm::util::concurrent::atomic::IID_IAtomicInteger;
using ccm::util::concurrent::atomic::IID_IAtomicLong;

namespace ccm {
namespace util {
namespace concurrent {

CCM_INTERFACE_IMPL_1(ThreadLocalRandom, Random, IThreadLocalRandom);

static AutoPtr<IAtomicInteger> CreateAtomicInteger()
{
    AutoPtr<IAtomicInteger> atomic;
    CAtomicInteger::New(IID_IAtomicInteger, (IInterface**)&atomic);
    return atomic;
}

static AutoPtr<IAtomicLong> CreateAtomicLong(
    /* [in] */ Long initialValue)
{
    AutoPtr<IAtomicLong> atomic;
    CAtomicLong::New(initialValue, IID_IAtomicLong, (IInterface**)&atomic);
    return atomic;
}

AutoPtr<IAtomicLong> ThreadLocalRandom::GetSeeder()
{
    static AutoPtr<IAtomicLong> sSeeder = CreateAtomicLong(
            Mix64(System::GetCurrentTimeMillis()) ^
            Mix64(System::GetNanoTime()));
    return sSeeder;
}

void ThreadLocalRandom::StaticInitialize()
{
    class _PrivilegedAction
        : public LightRefBase
        , public IPrivilegedAction
    {
    public:
        Integer AddRef(
            /* [in] */ HANDLE id)
        {
            return LightRefBase::AddRef(id);
        }

        Integer Release(
            /* [in] */ HANDLE id)
        {
            return LightRefBase::Release(id);
        }

        IInterface* Probe(
            /* [in] */ const InterfaceID& iid)
        {
            if (iid == IID_IInterface) {
                return (IInterface*)(IPrivilegedAction*)this;
            }
            else if (iid == IID_IPrivilegedAction) {
                return (IPrivilegedAction*)this;
            }
            return nullptr;
        }

        ECode GetInterfaceID(
            /* [in] */ IInterface* object,
            /* [out] */ InterfaceID* iid)
        {
            VALIDATE_NOT_NULL(iid);

            if (object == (IInterface*)(IPrivilegedAction*)this) {
                *iid = IID_IPrivilegedAction;
                return NOERROR;
            }
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        ECode Run(
            /* [out] */ IInterface** result)
        {
            VALIDATE_NOT_NULL(result);

            AutoPtr<IBoolean> value = CoreUtils::Box(
                    CoreUtils::GetBoolean(String("ccm.util.secureRandomSeed")));
            *result = value;
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
    };

    AutoPtr<IInterface> lsRet;
    AutoPtr<IPrivilegedAction> lsAction = new _PrivilegedAction();
    ECode ec = AccessController::DoPrivileged(lsAction, &lsRet);
    CHECK(SUCCEEDED(ec));
    if (CoreUtils::Unbox(IBoolean::Probe(lsRet))) {
        Array<Byte> seedBytes;
        CSecureRandom::GetSeed(8, &seedBytes);
        Long s = (Long)seedBytes[0] & 0xffll;
        for (Integer i = 1; i < 8; ++i) {
            s = (s << 8) | ((Long)seedBytes[i] & 0xffll);
        }
        GetSeeder()->Set(s);
    }
}

Long ThreadLocalRandom::Mix64(
    /* [in] */ Long z)
{
    z = (z ^ (((unsigned Long)z) >> 33)) * 0xff51afd7ed558ccdll;
    z = (z ^ (((unsigned Long)z) >> 33)) * 0xc4ceb9fe1a85ec53ll;
    return z ^ (((unsigned Long)z) >> 33);
}

Integer ThreadLocalRandom::Mix32(
    /* [in] */ Long z)
{
    z = (z ^ (((unsigned Long)z) >> 33)) * 0xff51afd7ed558ccdll;
    z = (z ^ (((unsigned Long)z) >> 33)) * 0xc4ceb9fe1a85ec53ll;
    return (Integer)(((unsigned Long)z) >> 32);
}

void ThreadLocalRandom::LocalInit()
{
    static AutoPtr<IAtomicInteger> sProbeGenerator = CreateAtomicInteger();
    Integer p;
    sProbeGenerator->AddAndGet(PROBE_INCREMENT, &p);
    Integer probe = (p == 0) ? 1 : p; // skip 0
    Long seed;
    GetSeeder()->GetAndAdd(SEEDER_INCREMENT, &seed);
    seed = Mix64(seed);
    AutoPtr<IThread> t;
    CThread::GetCurrentThread(&t);
    PUT_LONG(CThread::From(t), mThreadLocalRandomSeed, seed);
    PUT_INT(CThread::From(t), mThreadLocalRandomProbe, probe);
}

AutoPtr<IThreadLocalRandom> ThreadLocalRandom::GetCurrent()
{
    static const AutoPtr<IThreadLocalRandom> sInstance = new ThreadLocalRandom();
    AutoPtr<IThread> t;
    CThread::GetCurrentThread(&t);
    if (GET_INT(CThread::From(t), mThreadLocalRandomProbe) == 0) {
        LocalInit();
    }
    return sInstance;
}

ECode ThreadLocalRandom::SetSeed(
    /* [in] */ Long seed)
{
    if (mInitialized) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    return NOERROR;
}

Long ThreadLocalRandom::NextSeed()
{
    AutoPtr<IThread> t;
    CThread::GetCurrentThread(&t);
    Long r = GET_LONG(CThread::From(t), mThreadLocalRandomSeed);
    PUT_LONG(CThread::From(t), mThreadLocalRandomSeed, r + GAMMA);
    return r;
}

Integer ThreadLocalRandom::Next(
    /* [in] */ Integer bits)
{
    return (Integer)(((unsigned Long)Mix64(NextSeed())) >> (64 - bits));
}

Long ThreadLocalRandom::InternalNextLong(
    /* [in] */ Long origin,
    /* [in] */ Long bound)
{
    Long r = Mix64(NextSeed());
    if (origin < bound) {
        Long n = bound - origin, m = n - 1;
        if ((n & m) == 0ll) { // power of two
            r = (r & m) + origin;
        }
        else if (n > 0ll) {  // reject over-represented candidates
            for (Long u = ((unsigned Long)r) >> 1;            // ensure nonnegative
                 u + m - (r = u % n) < 0ll;    // rejection check
                 u = ((unsigned Long)Mix64(NextSeed())) >> 1) // retry
                ;
            r += origin;
        }
        else {              // range not representable as long
            while (r < origin || r >= bound) {
                r = Mix64(NextSeed());
            }
        }
    }
    return r;
}

Integer ThreadLocalRandom::InternalNextInt(
    /* [in] */ Integer origin,
    /* [in] */ Integer bound)
{
    Integer r = Mix32(NextSeed());
    if (origin < bound) {
        Integer n = bound - origin, m = n - 1;
        if ((n & m) == 0) {
            r = (r & m) + origin;
        }
        else if (n > 0) {
            for (Integer u = ((unsigned Integer)r) >> 1;
                 u + m - (r = u % n) < 0;
                 u = ((unsigned Integer)Mix32(NextSeed())) >> 1)
                ;
            r += origin;
        }
        else {
            while (r < origin || r >= bound) {
                r = Mix32(NextSeed());
            }
        }
    }
    return r;
}

Double ThreadLocalRandom::InternalNextDouble(
    /* [in] */ Double origin,
    /* [in] */ Double bound)
{
    Long l;
    NextLong(&l);
    Double r = ((unsigned Long)l >> 11) * DOUBLE_UNIT;
    if (origin < bound) {
        r = r * (bound - origin) + origin;
        if (r >= bound) { // correct for rounding
            r = Math::LongBitsToDouble(Math::DoubleToLongBits(bound) - 1);
        }
    }
    return r;
}

Integer ThreadLocalRandom::GetProbe()
{
    AutoPtr<IThread> t;
    CThread::GetCurrentThread(&t);
    return GET_INT(CThread::From(t), mThreadLocalRandomProbe);
}

Integer ThreadLocalRandom::AdvanceProbe(
    /* [in] */ Integer probe)
{
    probe ^= probe << 13;   // xorshift
    probe ^= ((unsigned Integer)probe) >> 17;
    probe ^= probe << 5;
    AutoPtr<IThread> t;
    CThread::GetCurrentThread(&t);
    PUT_INT(CThread::From(t), mThreadLocalRandomProbe, probe);
    return probe;
}

}
}
}
