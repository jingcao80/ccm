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

#include "ccm.core.CSystem.h"
#include "ccm.core.ISystem.h"
#include <ccmautoptr.h>
#include <gtest/gtest.h>

using namespace ccm;
using ccm::core::CSystem;
using ccm::core::ISystem;
using ccm::core::IID_ISystem;

static constexpr Long N = 1000000000ll;

static const Integer GLOBAL = 9;

static Integer GetLocal()
{
    static const Integer Local = 46;
    return Local;
}

TEST(LocalStaticVariableTest, LocalStaticVariablePerformanceTest)
{
    AutoPtr<ISystem> system;
    CSystem::New(IID_ISystem, (IInterface**)&system);
    Long beginTime, endTime;
    system->GetNanoTime(&beginTime);
    for (Long i = 0; i < N; i++) {
        Integer k = GLOBAL;
    }
    system->GetNanoTime(&endTime);
    printf("==== begin[%lld] end[%lld] last[%lld ns %lld ms] ====\n",
            beginTime, endTime, endTime - beginTime, (endTime - beginTime) / 1000000);
    system->GetNanoTime(&beginTime);
    for (Long i = 0; i < N; i++) {
        Integer k = GetLocal();
    }
    system->GetNanoTime(&endTime);
    printf("==== begin[%lld] end[%lld] last[%lld ns %lld ms] ====\n",
            beginTime, endTime, endTime - beginTime, (endTime - beginTime) / 1000000);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
