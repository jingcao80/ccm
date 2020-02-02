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
#include "como/core/Math.h"
#include "como/core/StrictMath.h"
#include "como/core/System.h"
#include "como/util/CRandom.h"
#include "como/util/Random.h"
#include "como/util/concurrent/atomic/CAtomicLong.h"
#include "como.core.IByte.h"
#include "como.core.IInteger.h"
#include <comolog.h>

using como::core::AutoLock;
using como::core::IByte;
using como::core::IInteger;
using como::core::Math;
using como::core::StrictMath;
using como::core::System;
using como::io::IID_ISerializable;
using como::util::concurrent::atomic::CAtomicLong;
using como::util::concurrent::atomic::IID_IAtomicLong;

namespace como {
namespace util {

COMO_INTERFACE_IMPL_2(Random, SyncObject, IRandom, ISerializable);

static AutoPtr<IAtomicLong> CreateAtomicLong(
    /* [in] */ Long initialValue)
{
    AutoPtr<IAtomicLong> atomic;
    CAtomicLong::New(initialValue, IID_IAtomicLong, (IInterface**)&atomic);
    return atomic;
}

AutoPtr<IAtomicLong> Random::GetSeedUniquifier()
{
    static AutoPtr<IAtomicLong> sSeedUniquifier = CreateAtomicLong(8682522807148012ll);
    return sSeedUniquifier;
}

ECode Random::Constructor()
{
    return Constructor(SeedUniquifier() ^ System::GetNanoTime());
}

ECode Random::Constructor(
    /* [in] */ Long seed)
{
    CoclassID cid;
    GetCoclassID(cid);
    if (cid == CID_CRandom) {
        CAtomicLong::New(InitialScramble(seed), IID_IAtomicLong, (IInterface**)&mSeed);
    }
    else {
        CAtomicLong::New(IID_IAtomicLong, (IInterface**)&mSeed);
        SetSeed(seed);
    }
    return NOERROR;
}

Long Random::SeedUniquifier()
{
    // L'Ecuyer, "Tables of Linear Congruential Generators of
    // Different Sizes and Good Lattice Structure", 1999
    for (;;) {
        Long current;
        GetSeedUniquifier()->Get(current);
        Long next = current * 181783497276652981ll;
        Boolean succeeded;
        if (GetSeedUniquifier()->CompareAndSet(current, next, &succeeded), succeeded) {
            return next;
        }
    }
}

Long Random::InitialScramble(
    /* [in] */ Long seed)
{
    return (seed ^ sMultiplier) & sMask;
}

ECode Random::SetSeed(
    /* [in] */ Long seed)
{
    AutoLock lock(this);

    mSeed->Set(InitialScramble(seed));
    mHaveNextNextGaussian = false;
    return NOERROR;
}

Integer Random::Next(
    /* [in] */ Integer bits)
{
    Long oldseed, nextseed;
    Boolean succeeded;
    do {
        mSeed->Get(oldseed);
        nextseed = (oldseed * sMultiplier + sAddend) & sMask;
    } while (mSeed->CompareAndSet(oldseed, nextseed, &succeeded), !succeeded);
    return (Integer)(((ULong)nextseed) >> (48 - bits));
}

ECode Random::NextBytes(
    /* [out] */ Array<Byte>& bytes)
{
    for (Integer i = 0, len = bytes.GetLength(); i < len;) {
        Integer rnd, n;
        for (NextInteger(&rnd), n = Math::Min(len - i, IInteger::SIZE / IByte::SIZE);
                n-- >0; rnd >>= IByte::SIZE) {
            bytes[i++] = (Byte)rnd;
        }
    }
    return NOERROR;
}

ECode Random::NextInteger(
    /* [out] */ Integer* value)
{
    VALIDATE_NOT_NULL(value);

    *value = Next(32);
    return NOERROR;
}

ECode Random::NextInteger(
    /* [in] */ Integer bound,
    /* [out] */ Integer* value)
{
    VALIDATE_NOT_NULL(value);

    if (bound <= 0) {
        Logger::E("Random", "bound must be positive");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer r = Next(31);
    Integer m = bound - 1;
    if ((bound & m) == 0) { // i.e., bound is a power of 2
        r = (Integer)((bound * (Long)r) >> 31);
    }
    else {
        for (Integer u = r;
             u - (r = u % bound) + m < 0;
             u = Next(31));
    }
    *value = r;
    return NOERROR;
}

ECode Random::NextLong(
    /* [out] */ Long* value)
{
    VALIDATE_NOT_NULL(value);

    *value = ((Long)(Next(32)) << 32) + Next(32);
    return NOERROR;
}

ECode Random::NextBoolean(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);

    *value = Next(1) != 0;
    return NOERROR;
}

ECode Random::NextFloat(
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);

    *value = Next(24) / ((Float)(1 << 24));
    return NOERROR;
}

ECode Random::NextDouble(
    /* [out] */ Double* value)
{
    VALIDATE_NOT_NULL(value);

    *value = (((Long)(Next(26)) << 27) + Next(27)) * DOUBLE_UNIT;
    return NOERROR;
}

ECode Random::NextGaussian(
    /* [out] */ Double* value)
{
    VALIDATE_NOT_NULL(value);

    // See Knuth, ACP, Section 3.4.1 Algorithm C.
    if (mHaveNextNextGaussian) {
        mHaveNextNextGaussian = false;
        *value = mNextNextGaussian;
        return NOERROR;
    }
    else {
        Double v1, v2, s;
        do {
            NextDouble(&v1);
            NextDouble(&v2);
            v1 = 2 * v1 - 1; // between -1 and 1
            v2 = 2 * v2 - 1; // between -1 and 1
            s = v1 * v1 + v2 * v2;
        } while (s >= 1 || s == 0);
        Double multiplier = StrictMath::Sqrt(-2 * StrictMath::Log(s)/s);
        mNextNextGaussian = v2 * sMultiplier;
        mHaveNextNextGaussian = true;
        *value = v1 * multiplier;
        return NOERROR;
    }
}

}
}
