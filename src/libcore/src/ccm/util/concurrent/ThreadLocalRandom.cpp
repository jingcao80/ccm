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

#include "ccm/core/CThread.h"
#include "ccm/core/volatile.h"
#include "ccm/util/concurrent/ThreadLocalRandom.h"
#include "ccm/util/concurrent/atomic/CAtomicInteger.h"

using ccm::core::CThread;
using ccm::core::IThread;
using ccm::util::concurrent::atomic::CAtomicInteger;
using ccm::util::concurrent::atomic::IAtomicInteger;
using ccm::util::concurrent::atomic::IID_IAtomicInteger;

namespace ccm {
namespace util {
namespace concurrent {

static AutoPtr<IAtomicInteger> CreateAtomicInteger()
{
    AutoPtr<IAtomicInteger> atomic;
    CAtomicInteger::New(IID_IAtomicInteger, (IInterface**)&atomic);
    return atomic;
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
