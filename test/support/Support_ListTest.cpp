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
#include "Support_ListTest.h"
#include "como.core.CInteger.h"
#include "como.core.IInteger.h"
#include "como.util.CLinkedList.h"
#include "como.util.ICollection.h"
#include "como.util.IListIterator.h"
#include <gtest/gtest.h>

using como::core::CInteger;
using como::core::IID_IInteger;
using como::core::IInteger;
using como::util::CLinkedList;
using como::util::ICollection;
using como::util::IID_IList;
using como::util::IListIterator;

namespace como {
namespace test {

ECode Support_ListTest::RunTest()
{
    Integer hashCode = 1;
    for (Integer counter = 0; counter < 100; counter++) {
        AutoPtr<IInterface> elem;
        mList->Get(counter, &elem);
        hashCode = 31 * hashCode + Object::GetHashCode(elem);
        AutoPtr<IInteger> intObj;
        CInteger::New(counter, IID_IInteger, (IInterface**)&intObj);
        EXPECT_TRUE(Object::Equals(elem, intObj));
    }
    EXPECT_TRUE(hashCode == Object::GetHashCode(mList));

    AutoPtr<IInteger> newInt;
    CInteger::New(1000, IID_IInteger, (IInterface**)&newInt);
    mList->Add(50, newInt);
    AutoPtr<IInteger> othNewInt;
    CInteger::New(1000, IID_IInteger, (IInterface**)&othNewInt);
    AutoPtr<IInterface> elem;
    mList->Get(50, &elem);
    EXPECT_TRUE(Object::Equals(elem, othNewInt));
    othNewInt = nullptr;
    CInteger::New(50, IID_IInteger, (IInterface**)&othNewInt);
    elem = nullptr;
    mList->Get(51, &elem);
    EXPECT_TRUE(Object::Equals(elem, othNewInt));
    othNewInt = nullptr;
    CInteger::New(49, IID_IInteger, (IInterface**)&othNewInt);
    elem = nullptr;
    mList->Get(49, &elem);
    EXPECT_TRUE(Object::Equals(elem, othNewInt));

    newInt = nullptr;
    CInteger::New(2000, IID_IInteger, (IInterface**)&newInt);
    mList->Set(50, newInt);
    othNewInt = nullptr;
    CInteger::New(2000, IID_IInteger, (IInterface**)&othNewInt);
    elem = nullptr;
    mList->Get(50, &elem);
    EXPECT_TRUE(Object::Equals(elem, othNewInt));
    othNewInt = nullptr;
    CInteger::New(50, IID_IInteger, (IInterface**)&othNewInt);
    elem = nullptr;
    mList->Get(51, &elem);
    EXPECT_TRUE(Object::Equals(elem, othNewInt));
    othNewInt = nullptr;
    CInteger::New(49, IID_IInteger, (IInterface**)&othNewInt);
    elem = nullptr;
    mList->Get(49, &elem);
    EXPECT_TRUE(Object::Equals(elem, othNewInt));

    mList->Remove(50);
    othNewInt = nullptr;
    CInteger::New(50, IID_IInteger, (IInterface**)&othNewInt);
    elem = nullptr;
    mList->Get(50, &elem);
    EXPECT_TRUE(Object::Equals(elem, othNewInt));
    othNewInt = nullptr;
    CInteger::New(51, IID_IInteger, (IInterface**)&othNewInt);
    elem = nullptr;
    mList->Get(51, &elem);
    EXPECT_TRUE(Object::Equals(elem, othNewInt));
    othNewInt = nullptr;
    CInteger::New(49, IID_IInteger, (IInterface**)&othNewInt);
    elem = nullptr;
    mList->Get(49, &elem);
    EXPECT_TRUE(Object::Equals(elem, othNewInt));

    AutoPtr<IList> myList;
    CLinkedList::New(IID_IList, (IInterface**)&myList);
    newInt = nullptr;
    CInteger::New(500, IID_IInteger, (IInterface**)&newInt);
    myList->Add(newInt);
    newInt = nullptr;
    CInteger::New(501, IID_IInteger, (IInterface**)&newInt);
    myList->Add(newInt);
    newInt = nullptr;
    CInteger::New(502, IID_IInteger, (IInterface**)&newInt);
    myList->Add(newInt);

    mList->AddAll(50, ICollection::Probe(myList));
    othNewInt = nullptr;
    CInteger::New(500, IID_IInteger, (IInterface**)&othNewInt);
    elem = nullptr;
    mList->Get(50, &elem);
    EXPECT_TRUE(Object::Equals(elem, othNewInt));
    othNewInt = nullptr;
    CInteger::New(501, IID_IInteger, (IInterface**)&othNewInt);
    elem = nullptr;
    mList->Get(51, &elem);
    EXPECT_TRUE(Object::Equals(elem, othNewInt));
    othNewInt = nullptr;
    CInteger::New(502, IID_IInteger, (IInterface**)&othNewInt);
    elem = nullptr;
    mList->Get(52, &elem);
    EXPECT_TRUE(Object::Equals(elem, othNewInt));
    othNewInt = nullptr;
    CInteger::New(50, IID_IInteger, (IInterface**)&othNewInt);
    elem = nullptr;
    mList->Get(53, &elem);
    EXPECT_TRUE(Object::Equals(elem, othNewInt));
    othNewInt = nullptr;
    CInteger::New(49, IID_IInteger, (IInterface**)&othNewInt);
    elem = nullptr;
    mList->Get(49, &elem);
    EXPECT_TRUE(Object::Equals(elem, othNewInt));

    AutoPtr<IList> mySubList;
    mList->SubList(50, 53, &mySubList);
    Integer size;
    mySubList->GetSize(&size);
    EXPECT_EQ(3, size);
    othNewInt = nullptr;
    CInteger::New(500, IID_IInteger, (IInterface**)&othNewInt);
    elem = nullptr;
    mySubList->Get(0, &elem);
    EXPECT_TRUE(Object::Equals(elem, othNewInt));
    othNewInt = nullptr;
    CInteger::New(501, IID_IInteger, (IInterface**)&othNewInt);
    elem = nullptr;
    mySubList->Get(1, &elem);
    EXPECT_TRUE(Object::Equals(elem, othNewInt));
    othNewInt = nullptr;
    CInteger::New(502, IID_IInteger, (IInterface**)&othNewInt);
    elem = nullptr;
    mySubList->Get(2, &elem);
    EXPECT_TRUE(Object::Equals(elem, othNewInt));

    TestListIterator(mySubList);

    mySubList->Clear();
    mList->GetSize(&size);
    EXPECT_EQ(100, size);

    TestListIterator(mList);
    AutoPtr<IListIterator> li;
    mList->GetListIterator(&li);
    Boolean hasNext;
    for (Integer counter = 0; li->HasNext(&hasNext), hasNext; counter++) {
        AutoPtr<IInterface> elem;
        li->Next(&elem);
        othNewInt = nullptr;
        CInteger::New(counter, IID_IInteger, (IInterface**)&othNewInt);
        EXPECT_TRUE(Object::Equals(elem, othNewInt));
    }

    AutoPtr<Support_CollectionTest> test = new Support_CollectionTest(ICollection::Probe(mList));
    test->RunTest();
    return NOERROR;
}

ECode Support_ListTest::TestListIterator(
    /* [in] */ IList* list)
{
    AutoPtr<IListIterator> li;
    list->GetListIterator(1, &li);
    AutoPtr<IInterface> elem1, elem2;
    li->Next(&elem1);
    list->Get(1, &elem2);
    EXPECT_TRUE(elem1 == elem2);

    Integer orgSize;
    list->GetSize(&orgSize);
    li = nullptr;
    list->GetListIterator(&li);
    for (Integer i = 0; i <= orgSize; i++) {
        if (i == 0) {
            Boolean hasPrevious;
            li->HasPrevious(&hasPrevious);
            EXPECT_TRUE(!hasPrevious);
        }
        else {
            Boolean hasPrevious;
            li->HasPrevious(&hasPrevious);
            EXPECT_TRUE(hasPrevious);
        }
        if (i == orgSize) {
            Boolean hasNext;
            li->HasNext(&hasNext);
            EXPECT_TRUE(!hasNext);
        }
        else {
            Boolean hasNext;
            li->HasNext(&hasNext);
            EXPECT_TRUE(hasNext);
        }
        Integer idx;
        li->GetNextIndex(&idx);
        EXPECT_EQ(idx, i);
        li->GetPreviousIndex(&idx);
        EXPECT_EQ(idx, i - 1);

        elem1 = elem2 = nullptr;
        li->Next(&elem1);
        ECode ec = list->Get(i, &elem2);
        EXPECT_TRUE(elem1 == elem2);
        if (i == orgSize) {
            EXPECT_TRUE(FAILED(ec));
        }
        else {
            EXPECT_TRUE(SUCCEEDED(ec));
        }
    }

    for (Integer i = orgSize - 1; i >= 0; i--) {
        elem1 = elem2 = nullptr;
        li->Previous(&elem1);
        list->Get(i, &elem2);
        EXPECT_TRUE(elem1 == elem2);
        Integer idx;
        li->GetNextIndex(&idx);
        EXPECT_EQ(idx, i);
        li->GetPreviousIndex(&idx);
        EXPECT_EQ(idx, i - 1);
        if (i == 0) {
            Boolean hasPrevious;
            li->HasPrevious(&hasPrevious);
            EXPECT_TRUE(!hasPrevious);
        }
        else {
            Boolean hasPrevious;
            li->HasPrevious(&hasPrevious);
            EXPECT_TRUE(hasPrevious);
        }
        Boolean hasNext;
        li->HasNext(&hasNext);
        EXPECT_TRUE(hasNext);
    }
    ECode ec = li->Previous();
    EXPECT_TRUE(FAILED(ec));

    AutoPtr<IInteger> add1;
    CInteger::New(600, IID_IInteger, (IInterface**)&add1);
    AutoPtr<IInteger> add2;
    CInteger::New(601, IID_IInteger, (IInterface**)&add2);
    li->Add(add1);
    Integer size;
    list->GetSize(&size);
    EXPECT_TRUE(size == orgSize + 1);
    Integer idx;
    li->GetNextIndex(&idx);
    EXPECT_EQ(1, idx);
    li->GetPreviousIndex(&idx);
    EXPECT_EQ(0, idx);
    AutoPtr<IInterface> next;
    li->Next(&next);
    elem1 = nullptr;
    list->Get(1, &elem1);
    EXPECT_TRUE(next == elem1);
    li->Add(add2);
    AutoPtr<IInterface> previous;
    li->Previous(&previous);
    EXPECT_TRUE(previous == add2);
    li->GetNextIndex(&idx);
    EXPECT_EQ(2, idx);
    li->GetPreviousIndex(&idx);
    EXPECT_EQ(1, idx);

    li->Remove();
    list->GetSize(&size);
    EXPECT_TRUE(size == orgSize + 1);
    li->GetNextIndex(&idx);
    EXPECT_EQ(2, idx);
    li->GetPreviousIndex(&idx);
    EXPECT_EQ(1, idx);
    elem1 = elem2 = nullptr;
    li->Previous(&elem1);
    list->Get(1, &elem2);
    EXPECT_TRUE(elem1 == elem2);
    elem1 = elem2 = nullptr;
    li->Previous(&elem1);
    list->Get(0, &elem2);
    EXPECT_TRUE(elem1 == elem2);
    elem1 = elem2 = nullptr;
    li->Next(&elem1);
    list->Get(0, &elem2);
    EXPECT_TRUE(elem1 == elem2);

    li->Remove();
    Boolean hasPrevious;
    li->HasPrevious(&hasPrevious);
    EXPECT_TRUE(!hasPrevious);
    Boolean hasNext;
    li->HasNext(&hasNext);
    EXPECT_TRUE(hasNext);
    list->GetSize(&size);
    EXPECT_TRUE(size == orgSize);
    li->GetNextIndex(&idx);
    EXPECT_EQ(0, idx);
    li->GetPreviousIndex(&idx);
    EXPECT_EQ(-1, idx);
    return NOERROR;
}

}
}
