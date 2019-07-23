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

#include "ccm.core.CInteger.h"
#include "ccm.core.IInteger.h"
#include "ccm.util.CVector.h"
#include "ccm.util.IVector.h"
#include <ccmautoptr.h>
#include <gtest/gtest.h>

using namespace ccm;
using ccm::core::CInteger;
using ccm::core::IID_IInteger;
using ccm::core::IInteger;
using ccm::util::CVector;
using ccm::util::IID_IVector;
using ccm::util::IVector;

TEST(VectorTest, TestConstructor)
{
    AutoPtr<IVector> tv;
    CVector::New(100, IID_IVector, (IInterface**)&tv);
    for (Integer i = 0; i < 100; i++) {
        AutoPtr<IInteger> intObj;
        CInteger::New(i, IID_IInteger, (IInterface**)&intObj);
        tv->AddElement(intObj);
    }
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
