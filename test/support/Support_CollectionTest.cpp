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

#include "Support_CollectionTest.h"
#include "Support_UnmodifiableCollectionTest.h"
#include "como.core.CInteger.h"
#include "como.core.IInteger.h"
#include "como.util.CTreeSet.h"
#include <gtest/gtest.h>

using como::core::CInteger;
using como::core::IID_IInteger;
using como::core::IInteger;
using como::util::CTreeSet;
using como::util::IID_ICollection;

namespace como {
namespace test {

ECode Support_CollectionTest::RunTest()
{
    AutoPtr<Support_UnmodifiableCollectionTest> support = new Support_UnmodifiableCollectionTest(mCol);
    support->RunTest();

    AutoPtr<ICollection> myCollection;
    CTreeSet::New(IID_ICollection, (IInterface**)&myCollection);
    AutoPtr<IInteger> newInt;
    CInteger::New(101, IID_IInteger, (IInterface**)&newInt);
    myCollection->Add(newInt);
    newInt = nullptr;
    CInteger::New(102, IID_IInteger, (IInterface**)&newInt);
    myCollection->Add(newInt);
    newInt = nullptr;
    CInteger::New(103, IID_IInteger, (IInterface**)&newInt);
    myCollection->Add(newInt);

    // Add
    newInt = nullptr;
    CInteger::New(101, IID_IInteger, (IInterface**)&newInt);
    Boolean changed;
    mCol->Add(newInt, &changed);
    EXPECT_TRUE(changed);
    AutoPtr<IInteger> othNewInt;
    CInteger::New(101, IID_IInteger, (IInterface**)&othNewInt);
    Boolean contains;
    mCol->Contains(othNewInt, contains);
    EXPECT_TRUE(contains);

    // Remove
    othNewInt = nullptr;
    CInteger::New(101, IID_IInteger, (IInterface**)&othNewInt);
    mCol->Remove(othNewInt, &changed);
    EXPECT_TRUE(changed);
    othNewInt = nullptr;
    CInteger::New(101, IID_IInteger, (IInterface**)&othNewInt);
    mCol->Contains(othNewInt, contains);
    EXPECT_TRUE(!contains);

    // AddAll
    mCol->AddAll(myCollection, &changed);
    EXPECT_TRUE(changed);
    mCol->ContainsAll(myCollection, contains);
    EXPECT_TRUE(contains);

    // containsAll
    mCol->ContainsAll(myCollection, contains);
    EXPECT_TRUE(contains);
    othNewInt = nullptr;
    CInteger::New(101, IID_IInteger, (IInterface**)&othNewInt);
    mCol->Remove(othNewInt, &changed);
    mCol->ContainsAll(myCollection, contains);
    EXPECT_TRUE(!contains);

    //removeAll
    mCol->RemoveAll(myCollection, &changed);
    EXPECT_TRUE(changed);
    mCol->RemoveAll(myCollection, &changed);
    EXPECT_TRUE(!changed);
    othNewInt = nullptr;
    CInteger::New(102, IID_IInteger, (IInterface**)&othNewInt);
    mCol->Contains(othNewInt, contains);
    EXPECT_TRUE(!contains);
    othNewInt = nullptr;
    CInteger::New(103, IID_IInteger, (IInterface**)&othNewInt);
    mCol->Contains(othNewInt, contains);
    EXPECT_TRUE(!contains);

    // retainAll
    mCol->AddAll(myCollection);
    mCol->RetainAll(myCollection, &changed);
    EXPECT_TRUE(changed);
    mCol->RetainAll(myCollection, &changed);
    EXPECT_TRUE(!changed);

    // the 2nd time around
    mCol->ContainsAll(myCollection, contains);
    EXPECT_TRUE(contains);
    othNewInt = nullptr;
    CInteger::New(0, IID_IInteger, (IInterface**)&othNewInt);
    mCol->Contains(othNewInt, contains);
    EXPECT_TRUE(!contains);
    othNewInt = nullptr;
    CInteger::New(50, IID_IInteger, (IInterface**)&othNewInt);
    mCol->Contains(othNewInt, contains);
    EXPECT_TRUE(!contains);

    // clear
    mCol->Clear();
    Boolean isEmpty;
    mCol->IsEmpty(isEmpty);
    EXPECT_TRUE(isEmpty);
    othNewInt = nullptr;
    CInteger::New(101, IID_IInteger, (IInterface**)&othNewInt);
    mCol->Contains(othNewInt, contains);
    EXPECT_TRUE(!contains);
    return NOERROR;
}

}
}
