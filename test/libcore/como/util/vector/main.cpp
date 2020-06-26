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

#include "test/support/Support_ListTest.h"
#include "como.core.CInteger.h"
#include "como.core.IInteger.h"
#include "como.util.CVector.h"
#include "como.util.IVector.h"
#include <comosp.h>
#include <gtest/gtest.h>

using namespace como;
using como::core::CInteger;
using como::core::IID_IInteger;
using como::core::IInteger;
using como::test::Support_ListTest;
using como::util::CVector;
using como::util::IID_IVector;
using como::util::IVector;

TEST(VectorTest, TestConstructor)
{
    AutoPtr<IVector> tv;
    CVector::New(100, IID_IVector, (IInterface**)&tv);
    for (Integer i = 0; i < 100; i++) {
        AutoPtr<IInteger> intObj;
        CInteger::New(i, IID_IInteger, (IInterface**)&intObj);
        tv->AddElement(intObj);
    }
    AutoPtr<Support_ListTest> support = new Support_ListTest(IList::Probe(tv));
    support->RunTest();

    tv = nullptr;
    CVector::New(200, IID_IVector, (IInterface**)&tv);
    for (Integer i = -50; i < 150; i++) {
        AutoPtr<IInteger> intObj;
        CInteger::New(i, IID_IInteger, (IInterface**)&intObj);
        tv->AddElement(intObj);
    }
    AutoPtr<IList> subList;
    tv->SubList(50, 150, &subList);
    support = new Support_ListTest(subList);
    support->RunTest();

    tv = nullptr;
    CVector::New(IID_IVector, (IInterface**)&tv);
    Integer size;
    tv->GetSize(size);
    EXPECT_EQ(0, size);
    Integer capacity;
    tv->GetCapacity(&capacity);
    EXPECT_EQ(10, capacity);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
