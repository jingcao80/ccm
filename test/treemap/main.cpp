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
#include "ccm/core/StringUtils.h"
#include "ccm.core.ICharSequence.h"
#include "ccm.util.CHashMap.h"
#include "ccm.util.CSimpleEntry.h"
#include "ccm.util.CTreeMap.h"
#include "ccm.util.IHashMap.h"
#include "ccm.util.IIterator.h"
#include "ccm.util.IMap.h"
#include "ccm.util.IMapEntry.h"
#include "ccm.util.INavigableMap.h"
#include "ccm.util.ISet.h"
#include "ccm.util.ISortedMap.h"
#include "ccm.util.ITreeMap.h"
#include <gtest/gtest.h>

using namespace ccm;
using ccm::core::CoreUtils;
using ccm::core::E_NULL_POINTER_EXCEPTION;
using ccm::core::ICharSequence;
using ccm::core::StringUtils;
using ccm::util::CHashMap;
using ccm::util::CSimpleEntry;
using ccm::util::CTreeMap;
using ccm::util::E_CONCURRENT_MODIFICATION_EXCEPTION;
using ccm::util::IHashMap;
using ccm::util::IID_IHashMap;
using ccm::util::IID_IMap;
using ccm::util::IID_IMapEntry;
using ccm::util::IID_INavigableMap;
using ccm::util::IIterator;
using ccm::util::IMap;
using ccm::util::IMapEntry;
using ccm::util::INavigableMap;
using ccm::util::ISet;
using ccm::util::ISortedMap;
using ccm::util::ITreeMap;

TEST(TreeMapTest, TestEntrySetSetValue)
{
    AutoPtr<IMap> map;
    CTreeMap::New(IID_IMap, (IInterface**)&map);
    map->Put(CoreUtils::Box(String("A")), CoreUtils::Box(String("a")));
    map->Put(CoreUtils::Box(String("B")), CoreUtils::Box(String("b")));
    map->Put(CoreUtils::Box(String("C")), CoreUtils::Box(String("c")));

    AutoPtr<ISet> entries;
    map->GetEntrySet(&entries);
    AutoPtr<IIterator> it;
    entries->GetIterator(&it);
    AutoPtr<IMapEntry> entryA;
    it->Next((IInterface**)&entryA);
    AutoPtr<IInterface> prevValue;
    entryA->SetValue(CoreUtils::Box(String("x")), &prevValue);
    EXPECT_STREQ("a", CoreUtils::Unbox(ICharSequence::Probe(prevValue)).string());
    AutoPtr<IInterface> value;
    entryA->GetValue(&value);
    EXPECT_STREQ("x", CoreUtils::Unbox(ICharSequence::Probe(value)).string());
    value = nullptr;
    map->Get(CoreUtils::Box(String("A")), &value);
    EXPECT_STREQ("x", CoreUtils::Unbox(ICharSequence::Probe(value)).string());
    AutoPtr<IMapEntry> entryB;
    it->Next((IInterface**)&entryB);
    prevValue = nullptr;
    entryB->SetValue(CoreUtils::Box(String("y")), &prevValue);
    EXPECT_STREQ("b", CoreUtils::Unbox(ICharSequence::Probe(prevValue)).string());
    value = nullptr;
    entryB->GetValue(&value);
    EXPECT_STREQ("y", CoreUtils::Unbox(ICharSequence::Probe(value)).string());
    AutoPtr<IMapEntry> entryC;
    it->Next((IInterface**)&entryC);
    prevValue = nullptr;
    entryC->SetValue(CoreUtils::Box(String("z")), &prevValue);
    EXPECT_STREQ("c", CoreUtils::Unbox(ICharSequence::Probe(prevValue)).string());
    value = nullptr;
    entryC->GetValue(&value);
    EXPECT_STREQ("z", CoreUtils::Unbox(ICharSequence::Probe(value)).string());
    value = nullptr;
    map->Get(CoreUtils::Box(String("B")), &value);
    EXPECT_STREQ("y", CoreUtils::Unbox(ICharSequence::Probe(value)).string());
    value = nullptr;
    map->Get(CoreUtils::Box(String("C")), &value);
    EXPECT_STREQ("z", CoreUtils::Unbox(ICharSequence::Probe(value)).string());
}

TEST(TreeMapTest, TestSubMapEntrySetSetValue)
{
    AutoPtr<IMap> map;
    CTreeMap::New(IID_IMap, (IInterface**)&map);
    map->Put(CoreUtils::Box(String("A")), CoreUtils::Box(String("a")));
    map->Put(CoreUtils::Box(String("B")), CoreUtils::Box(String("b")));
    map->Put(CoreUtils::Box(String("C")), CoreUtils::Box(String("c")));
    map->Put(CoreUtils::Box(String("D")), CoreUtils::Box(String("d")));
    AutoPtr<INavigableMap> subMap;
    INavigableMap::Probe(map)->SubMap(CoreUtils::Box(String("A")), true, CoreUtils::Box(String("C")), true, &subMap);

    AutoPtr<ISet> entries;
    IMap::Probe(subMap)->GetEntrySet(&entries);
    AutoPtr<IIterator> it;
    entries->GetIterator(&it);
    AutoPtr<IMapEntry> entryA;
    it->Next((IInterface**)&entryA);
    AutoPtr<IInterface> prevValue;
    entryA->SetValue(CoreUtils::Box(String("x")), &prevValue);
    EXPECT_STREQ("a", CoreUtils::Unbox(ICharSequence::Probe(prevValue)).string());
    AutoPtr<IInterface> value;
    entryA->GetValue(&value);
    EXPECT_STREQ("x", CoreUtils::Unbox(ICharSequence::Probe(value)).string());
    value = nullptr;
    IMap::Probe(subMap)->Get(CoreUtils::Box(String("A")), &value);
    EXPECT_STREQ("x", CoreUtils::Unbox(ICharSequence::Probe(value)).string());
    value = nullptr;
    map->Get(CoreUtils::Box(String("A")), &value);
    EXPECT_STREQ("x", CoreUtils::Unbox(ICharSequence::Probe(value)).string());
    AutoPtr<IMapEntry> entryB;
    it->Next((IInterface**)&entryB);
    prevValue = nullptr;
    entryB->SetValue(CoreUtils::Box(String("y")), &prevValue);
    EXPECT_STREQ("b", CoreUtils::Unbox(ICharSequence::Probe(prevValue)).string());
    value = nullptr;
    entryB->GetValue(&value);
    EXPECT_STREQ("y", CoreUtils::Unbox(ICharSequence::Probe(value)).string());
    AutoPtr<IMapEntry> entryC;
    it->Next((IInterface**)&entryC);
    prevValue = nullptr;
    entryC->SetValue(CoreUtils::Box(String("z")), &prevValue);
    EXPECT_STREQ("c", CoreUtils::Unbox(ICharSequence::Probe(prevValue)).string());
    value = nullptr;
    entryC->GetValue(&value);
    EXPECT_STREQ("z", CoreUtils::Unbox(ICharSequence::Probe(value)).string());
    value = nullptr;
    IMap::Probe(subMap)->Get(CoreUtils::Box(String("B")), &value);
    EXPECT_STREQ("y", CoreUtils::Unbox(ICharSequence::Probe(value)).string());
    value = nullptr;
    map->Get(CoreUtils::Box(String("B")), &value);
    EXPECT_STREQ("y", CoreUtils::Unbox(ICharSequence::Probe(value)).string());
    value = nullptr;
    IMap::Probe(subMap)->Get(CoreUtils::Box(String("C")), &value);
    EXPECT_STREQ("z", CoreUtils::Unbox(ICharSequence::Probe(value)).string());
    value = nullptr;
    map->Get(CoreUtils::Box(String("C")), &value);
    EXPECT_STREQ("z", CoreUtils::Unbox(ICharSequence::Probe(value)).string());
}

void AssertImmutable(
    /* [in] */ IMapEntry* entry)
{
    AutoPtr<IInterface> value;
    entry->GetValue(&value);
    String valueStr = CoreUtils::Unbox(ICharSequence::Probe(value));
    ECode ec = entry->SetValue(CoreUtils::Box(String("x")));
    EXPECT_EQ(ec, E_UNSUPPORTED_OPERATION_EXCEPTION);
    value = nullptr;
    entry->GetValue(&value);
    EXPECT_STREQ(valueStr.string(), CoreUtils::Unbox(ICharSequence::Probe(value)).string());
}

void AssertAllEntryMethodsReturnImmutableEntries(
    /* [in] */ INavigableMap* map)
{
    AutoPtr<IMapEntry> entry;
    map->CeilingEntry(CoreUtils::Box(String("B")), &entry);
    AssertImmutable(entry);
    entry = nullptr;
    map->FirstEntry(&entry);
    AssertImmutable(entry);
    entry = nullptr;
    map->FloorEntry(CoreUtils::Box(String("D")), &entry);
    AssertImmutable(entry);
    entry = nullptr;
    map->HigherEntry(CoreUtils::Box(String("A")), &entry);
    AssertImmutable(entry);
    entry = nullptr;
    map->LastEntry(&entry);
    AssertImmutable(entry);
    entry = nullptr;
    map->LowerEntry(CoreUtils::Box(String("C")), &entry);
    AssertImmutable(entry);
    entry = nullptr;
    map->PollFirstEntry(&entry);
    AssertImmutable(entry);
    entry = nullptr;
    map->PollLastEntry(&entry);
    AssertImmutable(entry);
}

TEST(TreeMapTest, TestExceptionsOnSetValue)
{
    AutoPtr<IMap> map;
    CTreeMap::New(IID_IMap, (IInterface**)&map);
    map->Put(CoreUtils::Box(String("A")), CoreUtils::Box(String("a")));
    map->Put(CoreUtils::Box(String("B")), CoreUtils::Box(String("b")));
    map->Put(CoreUtils::Box(String("C")), CoreUtils::Box(String("c")));

    AutoPtr<INavigableMap> subMap;
    INavigableMap::Probe(map)->SubMap(CoreUtils::Box(String("A")), true, CoreUtils::Box(String("C")), true, &subMap);
    AssertAllEntryMethodsReturnImmutableEntries(subMap);
}

TEST(TreeMapTest, TestConcurrentModificationDetection)
{
    AutoPtr<IMap> map;
    CTreeMap::New(IID_IMap, (IInterface**)&map);
    map->Put(CoreUtils::Box(String("A")), CoreUtils::Box(String("a")));
    map->Put(CoreUtils::Box(String("B")), CoreUtils::Box(String("b")));
    map->Put(CoreUtils::Box(String("C")), CoreUtils::Box(String("c")));

    AutoPtr<ISet> entries;
    map->GetEntrySet(&entries);
    AutoPtr<IIterator> it;
    entries->GetIterator(&it);
    it->Next();
    it->Next();
    it->Remove();
    map->Put(CoreUtils::Box(String("D")), CoreUtils::Box(String("d")));
    ECode ec = it->Next();
    EXPECT_EQ(ec, E_CONCURRENT_MODIFICATION_EXCEPTION);
}

TEST(TreeMapTest, TestIteratorRemoves)
{
    AutoPtr<IMap> map;
    CTreeMap::New(IID_IMap, (IInterface**)&map);
    map->Put(CoreUtils::Box(String("A")), CoreUtils::Box(String("a")));
    map->Put(CoreUtils::Box(String("B")), CoreUtils::Box(String("b")));
    map->Put(CoreUtils::Box(String("C")), CoreUtils::Box(String("c")));

    AutoPtr<ISet> entries;
    map->GetEntrySet(&entries);
    AutoPtr<IIterator> it;
    entries->GetIterator(&it);

    AutoPtr<IMapEntry> entry;
    it->Next((IInterface**)&entry);
    AutoPtr<IInterface> key;
    entry->GetKey(&key);
    EXPECT_STREQ("A", CoreUtils::Unbox(ICharSequence::Probe(key)).string());

    entry = nullptr;
    key = nullptr;
    it->Next((IInterface**)&entry);
    entry->GetKey(&key);
    EXPECT_STREQ("B", CoreUtils::Unbox(ICharSequence::Probe(key)).string());

    it->Remove();

    entry = nullptr;
    key = nullptr;
    it->Next((IInterface**)&entry);
    entry->GetKey(&key);
    EXPECT_STREQ("C", CoreUtils::Unbox(ICharSequence::Probe(key)).string());

    it->Remove();

    Boolean hasNext;
    it->HasNext(&hasNext);
    EXPECT_FALSE(hasNext);
}

TEST(TreeMapTest, TestEntrySetUsesComparatorOnly)
{
    AutoPtr<IMap> map;
    CTreeMap::New(StringUtils::GetStringCASE_INSENSITIVE_ORDER(), IID_IMap, (IInterface**)&map);
    map->Put(CoreUtils::Box(String("ABC")), CoreUtils::Box(String("a")));
    AutoPtr<ISet> entries;
    map->GetEntrySet(&entries);
    AutoPtr<IMapEntry> entry1;
    CSimpleEntry::New(CoreUtils::Box(String("abc")), CoreUtils::Box(String("a")), IID_IMapEntry, (IInterface**)&entry1);
    Boolean contained;
    entries->Contains(entry1, &contained);
    EXPECT_TRUE(contained);
    entries->Remove(entry1);
    Integer size;
    map->GetSize(&size);
    EXPECT_EQ(0, size);
}

TEST(TreeMapTest, TestMapConstructorPassingSortedMap)
{
    AutoPtr<IMap> source;
    CTreeMap::New(StringUtils::GetStringCASE_INSENSITIVE_ORDER(), IID_IMap, (IInterface**)&source);
    AutoPtr<INavigableMap> copy;
    CTreeMap::New(source, IID_INavigableMap, (IInterface**)&copy);
    AutoPtr<IComparator> comparator;
    ISortedMap::Probe(copy)->Comparator(&comparator);
    EXPECT_EQ(nullptr, comparator.Get());
}

TEST(TreeMapTest, TestNullsWithNaturalOrder)
{
    AutoPtr<IHashMap> copyFrom;
    CHashMap::New(IID_IHashMap, (IInterface**)&copyFrom);
    ECode ec = copyFrom->Put(nullptr, CoreUtils::Box(String("b")));
    EXPECT_EQ(ec, NOERROR);
    {
        AutoPtr<IMap> map;
        ec = CTreeMap::New(IMap::Probe(copyFrom), IID_IMap, (IInterface**)&map);
        EXPECT_EQ(ec, E_NULL_POINTER_EXCEPTION);
    }

    AutoPtr<IMap> map;
    CTreeMap::New(IID_IMap, (IInterface**)&map);
    ec = map->Put(nullptr, CoreUtils::Box(String("b")));
    EXPECT_EQ(ec, E_NULL_POINTER_EXCEPTION);

    {
        AutoPtr<INavigableMap> subMap;
        INavigableMap::Probe(map)->DescendingMap(&subMap);
        ec = IMap::Probe(subMap)->Put(nullptr, CoreUtils::Box(String("b")));
        EXPECT_EQ(ec, E_NULL_POINTER_EXCEPTION);
    }

    {
        AutoPtr<ISortedMap> subMap;
        INavigableMap::Probe(map)->SubMap(CoreUtils::Box(String("a")),
                CoreUtils::Box(String("z")), &subMap);
        ec = IMap::Probe(subMap)->Put(nullptr, CoreUtils::Box(String("b")));
        EXPECT_EQ(ec, E_NULL_POINTER_EXCEPTION);
    }
}

TEST(TreeMapTest, TestClassCastExceptions)
{
    AutoPtr<IMap> map;
    CTreeMap::New(IID_IMap, (IInterface**)&map);
    map->Put(CoreUtils::Box(String("A")), CoreUtils::Box(String("a")));
    {
        AutoPtr<IInterface> value;
        ECode ec = map->Get(CoreUtils::Box(5), &value);
        EXPECT_EQ(ec, NOERROR);
        EXPECT_EQ(nullptr, value.Get());
    }
    {
        Boolean contained;
        ECode ec = map->ContainsKey(CoreUtils::Box(5), &contained);
        EXPECT_EQ(ec, NOERROR);
        EXPECT_FALSE(contained);
    }
    {
        ECode ec = map->Remove(CoreUtils::Box(5));
        EXPECT_EQ(ec, NOERROR);
    }
}

TEST(TreeMapTest, TestClone)
{
    AutoPtr<IMap> map;
    CTreeMap::New(IID_IMap, (IInterface**)&map);
    map->Put(CoreUtils::Box(String("A")), CoreUtils::Box(String("a")));
    map->Put(CoreUtils::Box(String("B")), CoreUtils::Box(String("b")));

    AutoPtr<IMap> clone = (IMap*)CoreUtils::Clone(map, IID_IMap).Get();
    AutoPtr<IInterface> value;
    clone->Get(CoreUtils::Box(String("A")), &value);
    EXPECT_STREQ("a", CoreUtils::Unbox(ICharSequence::Probe(value)));
    value = nullptr;
    clone->Get(CoreUtils::Box(String("B")), &value);
    EXPECT_STREQ("b", CoreUtils::Unbox(ICharSequence::Probe(value)));
}

static Boolean IsBoundInclusive(
    /* [in] */ Char bound)
{
    return bound == '[' || bound == ']';
}

static Boolean IsLowerBound(
    /* [in] */ Char bound)
{
    return bound == '[' || bound == '(';
}

static ECode ApplyBound(
    /* [in] */ Char bound,
    /* [in] */ INavigableMap* m,
    /* [out] */ INavigableMap** submap)
{
    Integer boundValue = 0;
    if (IsLowerBound(bound)) {
        return m->TailMap(CoreUtils::Box(boundValue), IsBoundInclusive(bound), submap);
    }
    else {
        return m->HeadMap(CoreUtils::Box(boundValue), IsBoundInclusive(bound), submap);
    }
}

static Boolean IsWithinBounds(
    /* [in] */ Char submapBound,
    /* [in] */ Char mapBound)
{
    AutoPtr<INavigableMap> source;
    CTreeMap::New(IID_INavigableMap, (IInterface**)&source);
    AutoPtr<INavigableMap> m, submap;
    ApplyBound(mapBound, source, &m);
    ECode ec = ApplyBound(submapBound, m, &submap);
    return SUCCEEDED(ec);
}

TEST(TreeMapTest, TestBoundsOpenSubrangeOfOpenRange)
{
    EXPECT_TRUE(IsWithinBounds(U')', U')'));
    EXPECT_TRUE(IsWithinBounds(U'(', U'('));

    EXPECT_FALSE(IsWithinBounds(U')', U'('));
    EXPECT_FALSE(IsWithinBounds(U'(', U')'));
}

TEST(TreeMapTest, TestBoundsClosedSubrangeOfOpenRange)
{
    EXPECT_FALSE(IsWithinBounds(U']', U'('));
    EXPECT_FALSE(IsWithinBounds(U'[', U')'));
    EXPECT_FALSE(IsWithinBounds(U']', U')'));
    EXPECT_FALSE(IsWithinBounds(U'[', U'('));
}

TEST(TreeMapTest, TestBoundsClosedSubrangeOfClosedRange)
{
    EXPECT_TRUE(IsWithinBounds(U']', U'['));
    EXPECT_TRUE(IsWithinBounds(U'[', U']'));
    EXPECT_TRUE(IsWithinBounds(U']', U']'));
    EXPECT_TRUE(IsWithinBounds(U'[', U'['));
}

TEST(TreeMapTest, TestBoundsOpenSubrangeOfClosedRange)
{
    EXPECT_TRUE(IsWithinBounds(U')', U'['));
    EXPECT_TRUE(IsWithinBounds(U'(', U']'));
    EXPECT_TRUE(IsWithinBounds(U'(', U'['));
    EXPECT_TRUE(IsWithinBounds(U')', U']'));

    AutoPtr<INavigableMap> m, tm, hm;
    CTreeMap::New(IID_INavigableMap, (IInterface**)&m);
    ECode ec = m->TailMap(CoreUtils::Box(0), true, &tm);
    ec = tm->HeadMap(CoreUtils::Box(0), false, &hm);
    EXPECT_EQ(ec, NOERROR);

    m = tm = hm = nullptr;
    CTreeMap::New(IID_INavigableMap, (IInterface**)&m);
    ec = m->TailMap(CoreUtils::Box(0), false, &tm);
    ec = tm->HeadMap(CoreUtils::Box(0), true, &hm);
    EXPECT_EQ(ec, E_ILLEGAL_ARGUMENT_EXCEPTION);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}