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

#include "Support_UnmodifiableCollectionTest.h"
#include "ccm/core/CoreUtils.h"
#include "ccm.core.CInteger.h"
#include "ccm.core.IInteger.h"
#include "ccm.util.CHashSet.h"
#include "ccm.util.CTreeSet.h"
#include "ccm.util.IHashSet.h"
#include "ccm.util.IIterator.h"
#include "ccm.util.ISortedSet.h"
#include <gtest/gtest.h>

using ccm::core::CInteger;
using ccm::core::CoreUtils;
using ccm::core::IID_IInteger;
using ccm::core::IInteger;
using ccm::util::CHashSet;
using ccm::util::CTreeSet;
using ccm::util::IHashSet;
using ccm::util::IID_IHashSet;
using ccm::util::IID_ISortedSet;
using ccm::util::IIterator;
using ccm::util::ISortedSet;

namespace ccm {
namespace test {

ECode Support_UnmodifiableCollectionTest::RunTest()
{
    AutoPtr<IInteger> othNewInt;
    CInteger::New(0, IID_IInteger, (IInterface**)&othNewInt);
    Boolean contains;
    mCol->Contains(othNewInt, &contains);
    EXPECT_TRUE(contains);
    othNewInt = nullptr;
    CInteger::New(50, IID_IInteger, (IInterface**)&othNewInt);
    mCol->Contains(othNewInt, &contains);
    EXPECT_TRUE(contains);
    othNewInt = nullptr;
    CInteger::New(100, IID_IInteger, (IInterface**)&othNewInt);
    mCol->Contains(othNewInt, &contains);
    EXPECT_TRUE(!contains);

    // containsAll
    AutoPtr<IHashSet> hs;
    CHashSet::New(IID_IHashSet, (IInterface**)&hs);
    AutoPtr<IInteger> newInt;
    CInteger::New(0, IID_IInteger, (IInterface**)&newInt);
    hs->Add(newInt);
    newInt = nullptr;
    CInteger::New(25, IID_IInteger, (IInterface**)&newInt);
    hs->Add(newInt);
    newInt = nullptr;
    CInteger::New(99, IID_IInteger, (IInterface**)&newInt);
    hs->Add(newInt);
    mCol->ContainsAll(ICollection::Probe(hs), &contains);
    EXPECT_TRUE(contains);
    newInt = nullptr;
    CInteger::New(100, IID_IInteger, (IInterface**)&newInt);
    hs->Add(newInt);
    mCol->ContainsAll(ICollection::Probe(hs), &contains);
    EXPECT_TRUE(!contains);

    // isEmpty
    Boolean isEmpty;
    mCol->IsEmpty(&isEmpty);
    EXPECT_TRUE(!isEmpty);

    // iterator
    AutoPtr<IIterator> it;
    mCol->GetIterator(&it);
    AutoPtr<ISortedSet> ss;
    CTreeSet::New(IID_ISortedSet, (IInterface**)&ss);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> elem;
        it->Next(&elem);
        ss->Add(elem);
    }
    it = nullptr;
    ss->GetIterator(&it);
    for (Integer counter = 0; it->HasNext(&hasNext), hasNext; counter++) {
        AutoPtr<IInterface> elem;
        it->Next(&elem);
        Integer nextValue = CoreUtils::Unbox(IInteger::Probe(elem));
        EXPECT_TRUE(nextValue == counter);
    }

    // size
    Integer size;
    mCol->GetSize(&size);
    EXPECT_TRUE(size == 100);

    // toArray
    Array<IInterface*> objArray;
    mCol->ToArray(&objArray);
    for (Integer counter = 0; it->HasNext(&hasNext), hasNext; counter++) {
        AutoPtr<IInterface> elem;
        it->Next(&elem);
        EXPECT_TRUE(objArray[counter] == elem);
    }

    return NOERROR;
}

}
}
