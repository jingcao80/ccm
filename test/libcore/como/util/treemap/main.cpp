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

#include "como/core/CoreUtils.h"
#include "como/core/StringUtils.h"
#include "como.core.ICharSequence.h"
#include "como.util.CHashMap.h"
#include "como.util.CSimpleEntry.h"
#include "como.util.CTreeMap.h"
#include "como.util.IHashMap.h"
#include "como.util.IIterator.h"
#include "como.util.IMap.h"
#include "como.util.IMapEntry.h"
#include "como.util.INavigableMap.h"
#include "como.util.ISet.h"
#include "como.util.ISortedMap.h"
#include "como.util.ITreeMap.h"
#include <gtest/gtest.h>

using namespace como;
using como::core::CoreUtils;
using como::core::E_NULL_POINTER_EXCEPTION;
using como::core::ICharSequence;
using como::core::StringUtils;
using como::util::CHashMap;
using como::util::CSimpleEntry;
using como::util::CTreeMap;
using como::util::E_CONCURRENT_MODIFICATION_EXCEPTION;
using como::util::IHashMap;
using como::util::IID_IHashMap;
using como::util::IID_IMap;
using como::util::IID_IMapEntry;
using como::util::IID_INavigableMap;
using como::util::IIterator;
using como::util::IMap;
using como::util::IMapEntry;
using como::util::INavigableMap;
using como::util::ISet;
using como::util::ISortedMap;
using como::util::ITreeMap;

TEST(TreeMapTest, TestEntrySetSetValue)
{
    AutoPtr<IMap> map;
    CTreeMap::New(IID_IMap, (IInterface**)&map);
    map->Put(CoreUtils::Box("A"), CoreUtils::Box("a"));
    map->Put(CoreUtils::Box("B"), CoreUtils::Box("b"));
    map->Put(CoreUtils::Box("C"), CoreUtils::Box("c"));

    AutoPtr<ISet> entries;
    map->GetEntrySet(entries);
    AutoPtr<IIterator> it;
    entries->GetIterator(it);
    AutoPtr<IMapEntry> entryA;
    it->Next(entryA);
    AutoPtr<IInterface> prevValue;
    entryA->SetValue(CoreUtils::Box("x"), &prevValue);
    EXPECT_STREQ("a", CoreUtils::Unbox(ICharSequence::Probe(prevValue)).string());
    AutoPtr<IInterface> value;
    entryA->GetValue(value);
    EXPECT_STREQ("x", CoreUtils::Unbox(ICharSequence::Probe(value)).string());
    map->Get(CoreUtils::Box("A"), value);
    EXPECT_STREQ("x", CoreUtils::Unbox(ICharSequence::Probe(value)).string());
    AutoPtr<IMapEntry> entryB;
    it->Next(entryB);
    entryB->SetValue(CoreUtils::Box("y"), &prevValue);
    EXPECT_STREQ("b", CoreUtils::Unbox(ICharSequence::Probe(prevValue)).string());
    entryB->GetValue(value);
    EXPECT_STREQ("y", CoreUtils::Unbox(ICharSequence::Probe(value)).string());
    AutoPtr<IMapEntry> entryC;
    it->Next(entryC);
    prevValue = nullptr;
    entryC->SetValue(CoreUtils::Box("z"), &prevValue);
    EXPECT_STREQ("c", CoreUtils::Unbox(ICharSequence::Probe(prevValue)).string());
    entryC->GetValue(value);
    EXPECT_STREQ("z", CoreUtils::Unbox(ICharSequence::Probe(value)).string());
    map->Get(CoreUtils::Box("B"), value);
    EXPECT_STREQ("y", CoreUtils::Unbox(ICharSequence::Probe(value)).string());
    map->Get(CoreUtils::Box("C"), value);
    EXPECT_STREQ("z", CoreUtils::Unbox(ICharSequence::Probe(value)).string());
}

TEST(TreeMapTest, TestSubMapEntrySetSetValue)
{
    AutoPtr<IMap> map;
    CTreeMap::New(IID_IMap, (IInterface**)&map);
    map->Put(CoreUtils::Box("A"), CoreUtils::Box("a"));
    map->Put(CoreUtils::Box("B"), CoreUtils::Box("b"));
    map->Put(CoreUtils::Box("C"), CoreUtils::Box("c"));
    map->Put(CoreUtils::Box("D"), CoreUtils::Box("d"));
    AutoPtr<INavigableMap> subMap;
    INavigableMap::Probe(map)->SubMap(CoreUtils::Box("A"), true, CoreUtils::Box("C"), true, subMap);

    AutoPtr<ISet> entries;
    IMap::Probe(subMap)->GetEntrySet(entries);
    AutoPtr<IIterator> it;
    entries->GetIterator(it);
    AutoPtr<IMapEntry> entryA;
    it->Next(entryA);
    AutoPtr<IInterface> prevValue;
    entryA->SetValue(CoreUtils::Box("x"), &prevValue);
    EXPECT_STREQ("a", CoreUtils::Unbox(ICharSequence::Probe(prevValue)).string());
    AutoPtr<IInterface> value;
    entryA->GetValue(value);
    EXPECT_STREQ("x", CoreUtils::Unbox(ICharSequence::Probe(value)).string());
    IMap::Probe(subMap)->Get(CoreUtils::Box("A"), value);
    EXPECT_STREQ("x", CoreUtils::Unbox(ICharSequence::Probe(value)).string());
    map->Get(CoreUtils::Box("A"), value);
    EXPECT_STREQ("x", CoreUtils::Unbox(ICharSequence::Probe(value)).string());
    AutoPtr<IMapEntry> entryB;
    it->Next(entryB);
    prevValue = nullptr;
    entryB->SetValue(CoreUtils::Box("y"), &prevValue);
    EXPECT_STREQ("b", CoreUtils::Unbox(ICharSequence::Probe(prevValue)).string());
    entryB->GetValue(value);
    EXPECT_STREQ("y", CoreUtils::Unbox(ICharSequence::Probe(value)).string());
    AutoPtr<IMapEntry> entryC;
    it->Next(entryC);
    prevValue = nullptr;
    entryC->SetValue(CoreUtils::Box("z"), &prevValue);
    EXPECT_STREQ("c", CoreUtils::Unbox(ICharSequence::Probe(prevValue)).string());
    entryC->GetValue(value);
    EXPECT_STREQ("z", CoreUtils::Unbox(ICharSequence::Probe(value)).string());
    IMap::Probe(subMap)->Get(CoreUtils::Box("B"), value);
    EXPECT_STREQ("y", CoreUtils::Unbox(ICharSequence::Probe(value)).string());
    map->Get(CoreUtils::Box("B"), value);
    EXPECT_STREQ("y", CoreUtils::Unbox(ICharSequence::Probe(value)).string());
    IMap::Probe(subMap)->Get(CoreUtils::Box("C"), value);
    EXPECT_STREQ("z", CoreUtils::Unbox(ICharSequence::Probe(value)).string());
    map->Get(CoreUtils::Box("C"), value);
    EXPECT_STREQ("z", CoreUtils::Unbox(ICharSequence::Probe(value)).string());
}

void AssertImmutable(
    /* [in] */ IMapEntry* entry)
{
    AutoPtr<IInterface> value;
    entry->GetValue(value);
    String valueStr = CoreUtils::Unbox(ICharSequence::Probe(value));
    ECode ec = entry->SetValue(CoreUtils::Box("x"));
    EXPECT_EQ(ec, E_UNSUPPORTED_OPERATION_EXCEPTION);
    entry->GetValue(value);
    EXPECT_STREQ(valueStr.string(), CoreUtils::Unbox(ICharSequence::Probe(value)).string());
}

void AssertAllEntryMethodsReturnImmutableEntries(
    /* [in] */ INavigableMap* map)
{
    AutoPtr<IMapEntry> entry;
    map->CeilingEntry(CoreUtils::Box("B"), entry);
    AssertImmutable(entry);
    map->FirstEntry(entry);
    AssertImmutable(entry);
    map->FloorEntry(CoreUtils::Box("D"), entry);
    AssertImmutable(entry);
    map->HigherEntry(CoreUtils::Box("A"), entry);
    AssertImmutable(entry);
    map->LastEntry(entry);
    AssertImmutable(entry);
    map->LowerEntry(CoreUtils::Box("C"), entry);
    AssertImmutable(entry);
    map->PollFirstEntry(entry);
    AssertImmutable(entry);
    map->PollLastEntry(entry);
    AssertImmutable(entry);
}

TEST(TreeMapTest, TestExceptionsOnSetValue)
{
    AutoPtr<IMap> map;
    CTreeMap::New(IID_IMap, (IInterface**)&map);
    map->Put(CoreUtils::Box("A"), CoreUtils::Box("a"));
    map->Put(CoreUtils::Box("B"), CoreUtils::Box("b"));
    map->Put(CoreUtils::Box("C"), CoreUtils::Box("c"));

    AutoPtr<INavigableMap> subMap;
    INavigableMap::Probe(map)->SubMap(CoreUtils::Box("A"), true, CoreUtils::Box("C"), true, subMap);
    AssertAllEntryMethodsReturnImmutableEntries(subMap);
}

TEST(TreeMapTest, TestConcurrentModificationDetection)
{
    AutoPtr<IMap> map;
    CTreeMap::New(IID_IMap, (IInterface**)&map);
    map->Put(CoreUtils::Box("A"), CoreUtils::Box("a"));
    map->Put(CoreUtils::Box("B"), CoreUtils::Box("b"));
    map->Put(CoreUtils::Box("C"), CoreUtils::Box("c"));

    AutoPtr<ISet> entries;
    map->GetEntrySet(entries);
    AutoPtr<IIterator> it;
    entries->GetIterator(it);
    AutoPtr<IInterface> value;
    it->Next(value);
    it->Next(value);
    it->Remove();
    map->Put(CoreUtils::Box(String("D")), CoreUtils::Box(String("d")));
    ECode ec = it->Next(value);
    EXPECT_EQ(ec, E_CONCURRENT_MODIFICATION_EXCEPTION);
}

TEST(TreeMapTest, TestIteratorRemoves)
{
    AutoPtr<IMap> map;
    CTreeMap::New(IID_IMap, (IInterface**)&map);
    map->Put(CoreUtils::Box("A"), CoreUtils::Box("a"));
    map->Put(CoreUtils::Box("B"), CoreUtils::Box("b"));
    map->Put(CoreUtils::Box("C"), CoreUtils::Box("c"));

    AutoPtr<ISet> entries;
    map->GetEntrySet(entries);
    AutoPtr<IIterator> it;
    entries->GetIterator(it);

    AutoPtr<IMapEntry> entry;
    it->Next(entry);
    AutoPtr<IInterface> key;
    entry->GetKey(key);
    EXPECT_STREQ("A", CoreUtils::Unbox(ICharSequence::Probe(key)).string());

    it->Next(entry);
    entry->GetKey(key);
    EXPECT_STREQ("B", CoreUtils::Unbox(ICharSequence::Probe(key)).string());

    it->Remove();

    it->Next(entry);
    entry->GetKey(key);
    EXPECT_STREQ("C", CoreUtils::Unbox(ICharSequence::Probe(key)).string());

    it->Remove();

    Boolean hasNext;
    it->HasNext(hasNext);
    EXPECT_FALSE(hasNext);
}

TEST(TreeMapTest, TestEntrySetUsesComparatorOnly)
{
    AutoPtr<IMap> map;
    CTreeMap::New(StringUtils::GetStringCASE_INSENSITIVE_ORDER(), IID_IMap, (IInterface**)&map);
    map->Put(CoreUtils::Box(String("ABC")), CoreUtils::Box("a"));
    AutoPtr<ISet> entries;
    map->GetEntrySet(entries);
    AutoPtr<IMapEntry> entry1;
    CSimpleEntry::New(CoreUtils::Box(String("abc")), CoreUtils::Box("a"), IID_IMapEntry, (IInterface**)&entry1);
    Boolean contained;
    entries->Contains(entry1, contained);
    EXPECT_TRUE(contained);
    entries->Remove(entry1);
    Integer size;
    map->GetSize(size);
    EXPECT_EQ(0, size);
}

TEST(TreeMapTest, TestMapConstructorPassingSortedMap)
{
    AutoPtr<IMap> source;
    CTreeMap::New(StringUtils::GetStringCASE_INSENSITIVE_ORDER(), IID_IMap, (IInterface**)&source);
    AutoPtr<INavigableMap> copy;
    CTreeMap::New(source, IID_INavigableMap, (IInterface**)&copy);
    AutoPtr<IComparator> comparator;
    ISortedMap::Probe(copy)->Comparator(comparator);
    EXPECT_EQ(nullptr, comparator.Get());
}

TEST(TreeMapTest, TestNullsWithNaturalOrder)
{
    AutoPtr<IHashMap> copyFrom;
    CHashMap::New(IID_IHashMap, (IInterface**)&copyFrom);
    ECode ec = copyFrom->Put(nullptr, CoreUtils::Box("b"));
    EXPECT_EQ(ec, NOERROR);
    {
        AutoPtr<IMap> map;
        ec = CTreeMap::New(IMap::Probe(copyFrom), IID_IMap, (IInterface**)&map);
        EXPECT_EQ(ec, E_NULL_POINTER_EXCEPTION);
    }

    AutoPtr<IMap> map;
    CTreeMap::New(IID_IMap, (IInterface**)&map);
    ec = map->Put(nullptr, CoreUtils::Box("b"));
    EXPECT_EQ(ec, E_NULL_POINTER_EXCEPTION);

    {
        AutoPtr<INavigableMap> subMap;
        INavigableMap::Probe(map)->DescendingMap(subMap);
        ec = IMap::Probe(subMap)->Put(nullptr, CoreUtils::Box("b"));
        EXPECT_EQ(ec, E_NULL_POINTER_EXCEPTION);
    }

    {
        AutoPtr<ISortedMap> subMap;
        INavigableMap::Probe(map)->SubMap(CoreUtils::Box("a"),
                CoreUtils::Box("z"), subMap);
        ec = IMap::Probe(subMap)->Put(nullptr, CoreUtils::Box("b"));
        EXPECT_EQ(ec, E_NULL_POINTER_EXCEPTION);
    }
}

TEST(TreeMapTest, TestClassCastExceptions)
{
    AutoPtr<IMap> map;
    CTreeMap::New(IID_IMap, (IInterface**)&map);
    map->Put(CoreUtils::Box("A"), CoreUtils::Box("a"));
    {
        AutoPtr<IInterface> value;
        ECode ec = map->Get(CoreUtils::Box(5), value);
        EXPECT_EQ(ec, NOERROR);
        EXPECT_EQ(nullptr, value.Get());
    }
    {
        Boolean contained;
        ECode ec = map->ContainsKey(CoreUtils::Box(5), contained);
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
    map->Put(CoreUtils::Box("A"), CoreUtils::Box("a"));
    map->Put(CoreUtils::Box("B"), CoreUtils::Box("b"));

    AutoPtr<IMap> clone = (IMap*)CoreUtils::Clone(map, IID_IMap).Get();
    AutoPtr<IInterface> value;
    clone->Get(CoreUtils::Box("A"), value);
    EXPECT_STREQ("a", CoreUtils::Unbox(ICharSequence::Probe(value)));
    clone->Get(CoreUtils::Box("B"), value);
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
    /* [out] */ AutoPtr<INavigableMap>& submap)
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
    ApplyBound(mapBound, source, m);
    ECode ec = ApplyBound(submapBound, m, submap);
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
    ECode ec = m->TailMap(CoreUtils::Box(0), true, tm);
    ec = tm->HeadMap(CoreUtils::Box(0), false, hm);
    EXPECT_EQ(ec, NOERROR);

    m = tm = hm = nullptr;
    CTreeMap::New(IID_INavigableMap, (IInterface**)&m);
    ec = m->TailMap(CoreUtils::Box(0), false, tm);
    ec = tm->HeadMap(CoreUtils::Box(0), true, hm);
    EXPECT_EQ(ec, E_ILLEGAL_ARGUMENT_EXCEPTION);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}