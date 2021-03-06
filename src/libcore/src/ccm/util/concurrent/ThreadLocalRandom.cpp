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
#include "ccm/core/CThreadLocal.h"
#include "ccm/core/Math.h"
#include "ccm/core/StrictMath.h"
#include "ccm/core/System.h"
#include "ccm/core/volatile.h"
#include "ccm/security/AccessController.h"
#include "ccm/security/CSecureRandom.h"
#include "ccm/util/concurrent/ThreadLocalRandom.h"
#include "ccm/util/concurrent/atomic/CAtomicInteger.h"
#include "ccm/util/concurrent/atomic/CAtomicLong.h"
#include "ccm.core.IBoolean.h"
#include "ccm.core.IDouble.h"
#include "ccm.security.IPrivilegedAction.h"
#include <ccmlogger.h>
#include <pthread.h>

using ccm::core::CoreUtils;
using ccm::core::CThread;
using ccm::core::CThreadLocal;
using ccm::core::IBoolean;
using ccm::core::IDouble;
using ccm::core::IID_IThreadLocal;
using ccm::core::IThread;
using ccm::core::Math;
using ccm::core::StrictMath;
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

static AutoPtr<IThreadLocal> CreateThreadLocal()
{
    AutoPtr<IThreadLocal> l;
    CThreadLocal::New(IID_IThreadLocal, (IInterface**)&l);
    return l;
}

AutoPtr<IThreadLocal> ThreadLocalRandom::GetNextLocalGaussian()
{
    static AutoPtr<IThreadLocal> sNextLocalGaussian = CreateThreadLocal();
    return sNextLocalGaussian;
}

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

static pthread_once_t sThreadLocalRandomIsStaticInitialized = PTHREAD_ONCE_INIT;

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

            CoreUtils::Box(
                    CoreUtils::GetBoolean(String("ccm.util.secureRandomSeed"))).MoveTo((IBoolean**)result);
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
    pthread_once(&sThreadLocalRandomIsStaticInitialized, StaticInitialize);
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

ECode ThreadLocalRandom::NextInteger(
    /* [out] */ Integer* value)
{
    VALIDATE_NOT_NULL(value);

    *value = Mix32(NextSeed());
    return NOERROR;
}

ECode ThreadLocalRandom::NextInteger(
    /* [in] */ Integer bound,
    /* [out] */ Integer* value)
{
    VALIDATE_NOT_NULL(value);

    if (bound <= 0) {
        Logger::E("ThreadLocalRandom", "bound must be positive");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Integer r = Mix32(NextSeed());
    Integer m = bound - 1;
    if ((bound & m) == 0) { // power of two
        r &= m;
    }
    else { // reject over-represented candidates
        for (Integer u = (unsigned Integer)r >> 1;
             u + m - (r = u % bound) < 0;
             u = (unsigned Integer)Mix32(NextSeed()) >> 1)
            ;
    }
    *value = r;
    return NOERROR;
}

ECode ThreadLocalRandom::NextInteger(
    /* [in] */ Integer origin,
    /* [in] */ Integer bound,
    /* [out] */ Integer* value)
{
    VALIDATE_NOT_NULL(value);

    if (origin >= bound) {
        Logger::E("ThreadLocalRandom", "bound must be greater than origin");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *value = InternalNextInt(origin, bound);
    return NOERROR;
}

ECode ThreadLocalRandom::NextLong(
    /* [out] */ Long* value)
{
    VALIDATE_NOT_NULL(value);

    *value = Mix64(NextSeed());
    return NOERROR;
}

ECode ThreadLocalRandom::NextLong(
    /* [in] */ Long bound,
    /* [out] */ Long* value)
{
    VALIDATE_NOT_NULL(value);

    if (bound <= 0) {
        Logger::E("ThreadLocalRandom", "bound must be positive");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Long r = Mix64(NextSeed());
    Long m = bound - 1;
    if ((bound & m) == 0ll) { // power of two
        r &= m;
    }
    else { // reject over-represented candidates
        for (Long u = (unsigned Long)r >> 1;
             u + m - (r = u % bound) < 0ll;
             u = (unsigned Long)Mix64(NextSeed()) >> 1)
            ;
    }
    return r;
}

ECode ThreadLocalRandom::NextLong(
    /* [in] */ Long origin,
    /* [in] */ Long bound,
    /* [out] */ Long* value)
{
    VALIDATE_NOT_NULL(value);

    if (origin >= bound) {
        Logger::E("ThreadLocalRandom", "bound must be greater than origin");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *value = InternalNextLong(origin, bound);
    return NOERROR;
}

ECode ThreadLocalRandom::NextDouble(
    /* [out] */ Double* value)
{
    VALIDATE_NOT_NULL(value);

    *value = ((unsigned Long)Mix64(NextSeed()) >> 11) * DOUBLE_UNIT;
    return NOERROR;
}

ECode ThreadLocalRandom::NextDouble(
    /* [in] */ Double bound,
    /* [out] */ Double* value)
{
    VALIDATE_NOT_NULL(value);

    if (!(bound > 0.0)) {
        Logger::E("ThreadLocalRandom", "bound must be positive");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Double result = ((unsigned Long)Mix64(NextSeed()) >> 11) * DOUBLE_UNIT * bound;
    *value = (result < bound) ? result : // correct for rounding
            Math::LongBitsToDouble(Math::DoubleToLongBits(bound) - 1);
    return NOERROR;
}

ECode ThreadLocalRandom::NextDouble(
    /* [in] */ Double origin,
    /* [in] */ Double bound,
    /* [out] */ Double* value)
{
    VALIDATE_NOT_NULL(value);

    if (!(origin < bound)) {
        Logger::E("ThreadLocalRandom", "bound must be greater than origin");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *value = InternalNextDouble(origin, bound);
    return NOERROR;
}

ECode ThreadLocalRandom::NextBoolean(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);

    *value = Mix32(NextSeed()) < 0;
    return NOERROR;
}

ECode ThreadLocalRandom::NextFloat(
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);

    *value = ((unsigned Integer)Mix32(NextSeed()) >> 8) * FLOAT_UNIT;
    return NOERROR;
}

ECode ThreadLocalRandom::NextGaussian(
    /* [out] */ Double* value)
{
    VALIDATE_NOT_NULL(value);

    // Use nextLocalGaussian instead of nextGaussian field
    AutoPtr<IInterface> d;
    GetNextLocalGaussian()->Get(&d);
    if (d != nullptr) {
        GetNextLocalGaussian()->Set(nullptr);
        *value = CoreUtils::Unbox(IDouble::Probe(d));
        return NOERROR;
    }
    Double v1, v2, s;
    do {
        v1 = 2 * (NextDouble(&v1), v1) - 1; // between -1 and 1
        v2 = 2 * (NextDouble(&v2), v2) - 1; // between -1 and 1
        s = v1 * v1 + v2 * v2;
    } while (s >= 1 || s == 0);
    Double multiplier = StrictMath::Sqrt(-2 * StrictMath::Log(s) / s);
    GetNextLocalGaussian()->Set(CoreUtils::Box(v2 * multiplier));
    *value = v1 * multiplier;
    return NOERROR;
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

Integer ThreadLocalRandom::NextSecondarySeed()
{
    pthread_once(&sThreadLocalRandomIsStaticInitialized, StaticInitialize);
    Integer r;
    Long seed;
    AutoPtr<IThread> t;
    CThread::GetCurrentThread(&t);
    if ((r = GET_INT(CThread::From(t), mThreadLocalRandomSecondarySeed)) != 0) {
        r ^= r << 13;   // xorshift
        r ^= (unsigned Integer)r >> 17;
        r ^= r << 5;
    }
    else if (GetSeeder()->GetAndAdd(SEEDER_INCREMENT, &seed),
            (r = Mix32(seed)) == 0) {
        r = 1; // avoid zero
    }
    PUT_INT(CThread::From(t), mThreadLocalRandomSecondarySeed, r);
    return r;
}

}
}
}
