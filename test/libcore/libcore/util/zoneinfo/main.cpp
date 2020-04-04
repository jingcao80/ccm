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
#include "ZoneInfoTestHelper.h"
#include "como/io/ByteBufferFactory.h"
#include "libcore/util/ZoneInfoDBFactory.h"
#include "libcore/util/ZoneInfoFactory.h"
#include "como.core.CSystem.h"
#include "como.core.ILong.h"
#include "como.core.ISystem.h"
#include "como.io.IBuffer.h"
#include "como.io.IByteBuffer.h"
#include "como.io.IIntegerBuffer.h"
#include "como.io.IShortBuffer.h"
#include "como.util.CDate.h"
#include "como.util.IDate.h"
#include "como.util.ITimeZone.h"
#include "libcore.io.IBufferIterator.h"
#include "libcore.util.CZoneInfoWallTime.h"
#include "libcore.util.IZoneInfo.h"
#include "libcore.util.IZoneInfoDBTzData.h"
#include "libcore.util.IZoneInfoWallTime.h"
#include <comoobj.h>
#include <gtest/gtest.h>

using como::core::CSystem;
using como::core::IID_ISystem;
using como::core::ILong;
using como::core::ISystem;
using como::io::ByteBufferFactory;
using como::io::IBuffer;
using como::io::IByteBuffer;
using como::io::IIntegerBuffer;
using como::io::IShortBuffer;
using como::util::CDate;
using como::util::IDate;
using como::util::IID_IDate;
using como::util::ITimeZone;
using libcore::io::IBufferIterator;
using libcore::io::IID_IBufferIterator;
using libcore::util::CZoneInfoWallTime;
using libcore::util::IID_IZoneInfoWallTime;
using libcore::util::IZoneInfo;
using libcore::util::IZoneInfoDBTzData;
using libcore::util::IZoneInfoWallTime;
using libcore::util::ZoneInfoDBFactory;
using libcore::util::ZoneInfoFactory;

class ByteBufferIterator
    : public Object
    , public IBufferIterator
{
public:
    ByteBufferIterator(
        /* [in] */ IByteBuffer* buffer)
        : mBuffer(buffer)
    {}

    COMO_INTERFACE_DECL();

    ECode Seek(
        /* [in] */ Integer offset) override
    {
        return IBuffer::Probe(mBuffer)->SetPosition(offset);
    }

    ECode Skip(
        /* [in] */ Integer byteCount) override
    {
        Integer pos;
        IBuffer::Probe(mBuffer)->GetPosition(pos);
        IBuffer::Probe(mBuffer)->SetPosition(pos + byteCount);
        return NOERROR;
    }

    ECode Pos(
        /* [out] */ Integer* offset) override
    {
        return IBuffer::Probe(mBuffer)->GetPosition(*offset);
    }

    ECode ReadByteArray(
        /* [out] */ Array<Byte>& dst,
        /* [in] */ Integer dstOffset,
        /* [in] */ Integer byteCount) override
    {
        return mBuffer->Get(dst, dstOffset, byteCount);
    }

    ECode ReadByte(
        /* [out] */ Byte* result) override
    {
        return mBuffer->Get(*result);
    }

    ECode ReadInteger(
        /* [out] */ Integer* result) override
    {
        AutoPtr<IIntegerBuffer> buffer;
        mBuffer->AsIntegerBuffer(buffer);
        FAIL_RETURN(buffer->Get(*result));

        Skip(4);
        return NOERROR;
    }

    ECode ReadIntegerArray(
        /* [out] */ Array<Integer>& dst,
        /* [in] */ Integer dstOffset,
        /* [in] */ Integer intCount) override
    {
        AutoPtr<IIntegerBuffer> buffer;
        mBuffer->AsIntegerBuffer(buffer);
        FAIL_RETURN(buffer->Get(dst, dstOffset, intCount));

        Skip(4 * intCount);
        return NOERROR;
    }

    ECode ReadShort(
        /* [out] */ Short* result) override
    {
        AutoPtr<IShortBuffer> buffer;
        mBuffer->AsShortBuffer(buffer);
        FAIL_RETURN(buffer->Get(*result));

        Skip(2);
        return NOERROR;
    }

private:
    AutoPtr<IByteBuffer> mBuffer;
};

COMO_INTERFACE_IMPL_1(ByteBufferIterator, Object, IBufferIterator);

static Integer SecondsInMillis(
    /* [in] */ Integer seconds)
{
    return seconds * 1000;
}

AutoPtr<IZoneInfo> CreateZoneInfo(
    /* [in] */ const String& name,
    /* [in] */ Long currentTimeMillis,
    /* [in] */ Array<Byte>& bytes)
{
    AutoPtr<IByteBuffer> bb;
    ByteBufferFactory::Wrap(bytes, &bb);
    AutoPtr<ByteBufferIterator> bufferIterator = new ByteBufferIterator(bb);
    AutoPtr<IZoneInfo> zoneInfo;
    ZoneInfoFactory::ReadTimeZone(
            String::Format("TimeZone for '%s'", name.string()),
            bufferIterator, currentTimeMillis, &zoneInfo);
    return zoneInfo;
}

AutoPtr<IZoneInfo> CreateZoneInfo(
    /* [in] */ const String& name,
    /* [in] */ Array<Array<Integer>>& transitions,
    /* [in] */ Array<Array<Integer>>& types,
    /* [in] */ Long currentTimeMillis)
{
    AutoPtr<ZoneInfoTestHelper::ZicDataBuilder> builder =
            new ZoneInfoTestHelper::ZicDataBuilder();
    builder->SetTransitionsAndTypes(transitions, types);
    Array<Byte> bytes = builder->Build();
    return CreateZoneInfo(name, currentTimeMillis, bytes);
}

AutoPtr<IZoneInfo> CreateZoneInfo(
    /* [in] */ Array<Array<Integer>>& transitions,
    /* [in] */ Array<Array<Integer>>& types)
{
    AutoPtr<ISystem> system;
    CSystem::New(IID_ISystem, (IInterface**)&system);
    Long millis;
    system->GetCurrentTimeMillis(millis);
    return CreateZoneInfo(String("ZoneInfoTest"), transitions, types, millis);
}

AutoPtr<IZoneInfo> CreateZoneInfo(
    /* [in] */ Array<Array<Integer>>& transitions,
    /* [in] */ Array<Array<Integer>>& types,
    /* [in] */ Long currentTimeMillis)
{
    return CreateZoneInfo(String("ZoneInfoTest"), transitions, types, currentTimeMillis);
}

TEST(ZoneInfoTest, TestMakeTimeZoneNoTypes)
{
    Array<Array<Integer>> transitions;
    Array<Array<Integer>> types;
    AutoPtr<IZoneInfo> zoneInfo = CreateZoneInfo(transitions, types);
    EXPECT_EQ(zoneInfo, nullptr);
}

TEST(ZoneInfoTest, TestMakeTimeZoneOneTypeNoTransitions)
{
    Array<Array<Integer>> transitions;
    Array<Array<Integer>> types{ {4800, 0} };
    AutoPtr<IZoneInfo> zoneInfo = CreateZoneInfo(transitions, types);
    EXPECT_NE(zoneInfo, nullptr);

    Integer offset;
    ITimeZone::Probe(zoneInfo)->GetOffset(ILong::MIN_VALUE, &offset);
    EXPECT_EQ(SecondsInMillis(4800), offset);
    ITimeZone::Probe(zoneInfo)->GetOffset(0, &offset);
    EXPECT_EQ(SecondsInMillis(4800), offset);
    ITimeZone::Probe(zoneInfo)->GetOffset(ILong::MAX_VALUE, &offset);
    EXPECT_EQ(SecondsInMillis(4800), offset);

    Boolean useDaylightTime;
    ITimeZone::Probe(zoneInfo)->UseDaylightTime(&useDaylightTime);
    EXPECT_FALSE(useDaylightTime);
    Integer savingTime;
    ITimeZone::Probe(zoneInfo)->GetDSTSavings(&savingTime);
    EXPECT_EQ(0, savingTime);

    ITimeZone::Probe(zoneInfo)->GetRawOffset(&offset);
    EXPECT_EQ(SecondsInMillis(4800), offset);
}

TEST(ZoneInfoTest, TestReadTimeZoneOneNonDstTransition)
{
    Array<Array<Integer>> transitions{ {0, 0} };
    Array<Array<Integer>> types{ {3600, 0} };
    AutoPtr<IZoneInfo> zoneInfo = CreateZoneInfo(transitions, types);
    EXPECT_NE(zoneInfo, nullptr);

    Integer offset;
    ITimeZone::Probe(zoneInfo)->GetOffset(SecondsInMillis(-2), &offset);
    EXPECT_EQ(SecondsInMillis(3600), offset);
    ITimeZone::Probe(zoneInfo)->GetOffset(0, &offset);
    EXPECT_EQ(SecondsInMillis(3600), offset);
    ITimeZone::Probe(zoneInfo)->GetOffset(SecondsInMillis(2), &offset);
    EXPECT_EQ(SecondsInMillis(3600), offset);

    Boolean useDaylightTime;
    ITimeZone::Probe(zoneInfo)->UseDaylightTime(&useDaylightTime);
    EXPECT_FALSE(useDaylightTime);
    Integer savingTime;
    ITimeZone::Probe(zoneInfo)->GetDSTSavings(&savingTime);
    EXPECT_EQ(0, savingTime);

    ITimeZone::Probe(zoneInfo)->GetRawOffset(&offset);
    EXPECT_EQ(SecondsInMillis(3600), offset);
}

TEST(ZoneInfoTest, TestReadTimeZoneOneDstTransition)
{
    Array<Array<Integer>> transitions{ {0, 0} };
    Array<Array<Integer>> types{ {3600, 1} };
    AutoPtr<IZoneInfo> zoneInfo = CreateZoneInfo(transitions, types);
    EXPECT_EQ(zoneInfo, nullptr);
}

TEST(ZoneInfoTest, TestReadTimeZoneNegativeTranstion)
{
    Array<Array<Integer>> transitions{
        {-2000, 0}, {-5, 1}, {0, 2} };
    Array<Array<Integer>> types{
        {1800, 0}, {3600, 1}, {5400, 0} };
    AutoPtr<IZoneInfo> zoneInfo = CreateZoneInfo(transitions, types);
    EXPECT_NE(zoneInfo, nullptr);

    Integer offset;
    ITimeZone::Probe(zoneInfo)->GetOffset(SecondsInMillis(-5) - 1, &offset);
    EXPECT_EQ(SecondsInMillis(1800), offset);

    AutoPtr<IDate> date;
    CDate::New(SecondsInMillis(-5) - 1, IID_IDate, (IInterface**)&date);
    Boolean inDaylightTime;
    ITimeZone::Probe(zoneInfo)->InDaylightTime(date, &inDaylightTime);
    EXPECT_FALSE(inDaylightTime);

    ITimeZone::Probe(zoneInfo)->GetOffset(SecondsInMillis(-5), &offset);
    EXPECT_EQ(SecondsInMillis(3600), offset);
    date = nullptr;
    CDate::New(SecondsInMillis(-5), IID_IDate, (IInterface**)&date);
    ITimeZone::Probe(zoneInfo)->InDaylightTime(date, &inDaylightTime);
    EXPECT_TRUE(inDaylightTime);

    ITimeZone::Probe(zoneInfo)->GetOffset(SecondsInMillis(-5) + 1, &offset);
    EXPECT_EQ(SecondsInMillis(3600), offset);
    date = nullptr;
    CDate::New(SecondsInMillis(-5) + 1, IID_IDate, (IInterface**)&date);
    ITimeZone::Probe(zoneInfo)->InDaylightTime(date, &inDaylightTime);
    EXPECT_TRUE(inDaylightTime);

    Boolean useDaylightTime;
    ITimeZone::Probe(zoneInfo)->UseDaylightTime(&useDaylightTime);
    EXPECT_FALSE(useDaylightTime);
    Integer savingTime;
    ITimeZone::Probe(zoneInfo)->GetDSTSavings(&savingTime);
    EXPECT_EQ(0, savingTime);

    ITimeZone::Probe(zoneInfo)->GetRawOffset(&offset);
    EXPECT_EQ(SecondsInMillis(5400), offset);
}

TEST(ZoneInfoTest, TestReadTimeZonePositiveTranstion)
{
    Array<Array<Integer>> transitions{
        {0, 0}, {5, 1}, {2000, 2} };
    Array<Array<Integer>> types{
        {1800, 0}, {3600, 1}, {5400, 0} };
    AutoPtr<IZoneInfo> zoneInfo = CreateZoneInfo(transitions, types);
    EXPECT_NE(zoneInfo, nullptr);

    Integer offset;
    ITimeZone::Probe(zoneInfo)->GetOffset(SecondsInMillis(5) - 1, &offset);
    EXPECT_EQ(SecondsInMillis(1800), offset);

    AutoPtr<IDate> date;
    CDate::New(SecondsInMillis(5) - 1, IID_IDate, (IInterface**)&date);
    Boolean inDaylightTime;
    ITimeZone::Probe(zoneInfo)->InDaylightTime(date, &inDaylightTime);
    EXPECT_FALSE(inDaylightTime);

    ITimeZone::Probe(zoneInfo)->GetOffset(SecondsInMillis(5), &offset);
    EXPECT_EQ(SecondsInMillis(3600), offset);
    date = nullptr;
    CDate::New(SecondsInMillis(5), IID_IDate, (IInterface**)&date);
    ITimeZone::Probe(zoneInfo)->InDaylightTime(date, &inDaylightTime);
    EXPECT_TRUE(inDaylightTime);

    ITimeZone::Probe(zoneInfo)->GetOffset(SecondsInMillis(5) + 1, &offset);
    EXPECT_EQ(SecondsInMillis(3600), offset);
    date = nullptr;
    CDate::New(SecondsInMillis(5) + 1, IID_IDate, (IInterface**)&date);
    ITimeZone::Probe(zoneInfo)->InDaylightTime(date, &inDaylightTime);
    EXPECT_TRUE(inDaylightTime);

    Boolean useDaylightTime;
    ITimeZone::Probe(zoneInfo)->UseDaylightTime(&useDaylightTime);
    EXPECT_FALSE(useDaylightTime);
    Integer savingTime;
    ITimeZone::Probe(zoneInfo)->GetDSTSavings(&savingTime);
    EXPECT_EQ(0, savingTime);

    ITimeZone::Probe(zoneInfo)->GetRawOffset(&offset);
    EXPECT_EQ(SecondsInMillis(5400), offset);
}

TEST(ZoneInfoTest, TestReadTimeZoneHasFutureDSTNoPastDSTNegativeTransitions)
{
    Array<Array<Integer>> transitions{
        {-2000, 0}, {-500, 1}, {-100, 2} };
    Array<Array<Integer>> types{
        {1800, 0}, {3600, 0}, {5400, 1} };
    Integer expectedDSTSavings = SecondsInMillis(5400 - 3600);

    AutoPtr<IZoneInfo> zoneInfo = CreateZoneInfo(transitions, types, SecondsInMillis(-700));
    EXPECT_NE(zoneInfo, nullptr);

    Boolean useDaylightTime;
    ITimeZone::Probe(zoneInfo)->UseDaylightTime(&useDaylightTime);
    EXPECT_TRUE(useDaylightTime);
    Integer savingTime;
    ITimeZone::Probe(zoneInfo)->GetDSTSavings(&savingTime);
    EXPECT_EQ(expectedDSTSavings, savingTime);

    zoneInfo = CreateZoneInfo(transitions, types, SecondsInMillis(-100) - 5);
    EXPECT_NE(zoneInfo, nullptr);

    ITimeZone::Probe(zoneInfo)->UseDaylightTime(&useDaylightTime);
    EXPECT_TRUE(useDaylightTime);
    ITimeZone::Probe(zoneInfo)->GetDSTSavings(&savingTime);
    EXPECT_EQ(expectedDSTSavings, savingTime);
}

TEST(ZoneInfoTest, TestReadTimeZoneHasFutureDSTNoPastDSTPositiveTransitions)
{
    Array<Array<Integer>> transitions{
        {4000, 0}, {5500, 1}, {6000, 2} };
    Array<Array<Integer>> types{
        {1800, 0}, {3600, 0}, {7200, 1} };
    Integer expectedDSTSavings = SecondsInMillis(7200 - 3600);

    AutoPtr<IZoneInfo> zoneInfo = CreateZoneInfo(transitions, types, SecondsInMillis(4500));
    EXPECT_NE(zoneInfo, nullptr);

    Boolean useDaylightTime;
    ITimeZone::Probe(zoneInfo)->UseDaylightTime(&useDaylightTime);
    EXPECT_TRUE(useDaylightTime);
    Integer savingTime;
    ITimeZone::Probe(zoneInfo)->GetDSTSavings(&savingTime);
    EXPECT_EQ(expectedDSTSavings, savingTime);

    zoneInfo = CreateZoneInfo(transitions, types, SecondsInMillis(6000) - 5);
    EXPECT_NE(zoneInfo, nullptr);

    ITimeZone::Probe(zoneInfo)->UseDaylightTime(&useDaylightTime);
    EXPECT_TRUE(useDaylightTime);
    ITimeZone::Probe(zoneInfo)->GetDSTSavings(&savingTime);
    EXPECT_EQ(expectedDSTSavings, savingTime);
}

TEST(ZoneInfoTest, TestReadTimeZoneHasPastDSTNoFutureDSTNegativeTransitions)
{
    Array<Array<Integer>> transitions{
        {-5000, 0}, {-2000, 1}, {-500, 0}, {0, 2} };
    Array<Array<Integer>> types{
        {3600, 0}, {1800, 1}, {5400, 0} };
    AutoPtr<IZoneInfo> zoneInfo = CreateZoneInfo(transitions, types, SecondsInMillis(-1));
    EXPECT_NE(zoneInfo, nullptr);

    Boolean useDaylightTime;
    ITimeZone::Probe(zoneInfo)->UseDaylightTime(&useDaylightTime);
    EXPECT_FALSE(useDaylightTime);
    Integer savingTime;
    ITimeZone::Probe(zoneInfo)->GetDSTSavings(&savingTime);
    EXPECT_EQ(0, savingTime);

    zoneInfo = CreateZoneInfo(transitions, types, SecondsInMillis(-2000) + 5);
    EXPECT_NE(zoneInfo, nullptr);

    ITimeZone::Probe(zoneInfo)->UseDaylightTime(&useDaylightTime);
    EXPECT_FALSE(useDaylightTime);
    ITimeZone::Probe(zoneInfo)->GetDSTSavings(&savingTime);
    EXPECT_EQ(0, savingTime);
}

TEST(ZoneInfoTest, TestReadTimeZoneHasPastDSTNoFutureDSTPositiveTransitions)
{
    Array<Array<Integer>> transitions{
        {1000, 0}, {4000, 1}, {5500, 0}, {6000, 2} };
    Array<Array<Integer>> types{
        {3600, 0}, {1800, 1}, {5400, 0} };
    AutoPtr<IZoneInfo> zoneInfo = CreateZoneInfo(transitions, types, SecondsInMillis(4700));
    EXPECT_NE(zoneInfo, nullptr);

    Boolean useDaylightTime;
    ITimeZone::Probe(zoneInfo)->UseDaylightTime(&useDaylightTime);
    EXPECT_FALSE(useDaylightTime);
    Integer savingTime;
    ITimeZone::Probe(zoneInfo)->GetDSTSavings(&savingTime);
    EXPECT_EQ(0, savingTime);

    zoneInfo = CreateZoneInfo(transitions, types, SecondsInMillis(4000) + 5);
    EXPECT_NE(zoneInfo, nullptr);

    ITimeZone::Probe(zoneInfo)->UseDaylightTime(&useDaylightTime);
    EXPECT_FALSE(useDaylightTime);
    ITimeZone::Probe(zoneInfo)->GetDSTSavings(&savingTime);
    EXPECT_EQ(0, savingTime);
}

TEST(ZoneInfoTest, TestReadTimeZoneLotsOfTypes)
{
    Array<Array<Integer>> transitions{ {-2000, 255} };
    Array<Array<Integer>> types(256);
    for (Integer i = 0; i < types.GetLength(); i++) {
        types[i] = Array<Integer>(2);
    }
    types[255] = {3600, 0};
    AutoPtr<IZoneInfo> zoneInfo = CreateZoneInfo(transitions, types, IInteger::MIN_VALUE);
    EXPECT_NE(zoneInfo, nullptr);

    Boolean useDaylightTime;
    ITimeZone::Probe(zoneInfo)->UseDaylightTime(&useDaylightTime);
    EXPECT_FALSE(useDaylightTime);
    Integer savingTime;
    ITimeZone::Probe(zoneInfo)->GetDSTSavings(&savingTime);
    EXPECT_EQ(0, savingTime);

    AutoPtr<IZoneInfoWallTime> wallTime;
    CZoneInfoWallTime::New(IID_IZoneInfoWallTime, (IInterface**)&wallTime);
    ECode ec = wallTime->Localtime(0, zoneInfo);
    EXPECT_EQ(ec, NOERROR);
    Integer time;
    ec = wallTime->Mktime(zoneInfo, &time);
    EXPECT_EQ(ec, NOERROR);
    EXPECT_NE(time, -1);
}

TEST(ZoneInfoTest, TestReadTimeZoneAll)
{
    AutoPtr<IZoneInfoDBTzData> instance;
    ZoneInfoDBFactory::GetInstance(&instance);
    Array<String> availableIDs;
    instance->GetAvailableIDs(&availableIDs);
    for (String id : availableIDs) {
        AutoPtr<IBufferIterator> bufferIterator;
        instance->GetBufferIterator(id, &bufferIterator);

        AutoPtr<IZoneInfo> zoneInfo;
        ZoneInfoFactory::ReadTimeZone(id, bufferIterator, ILong::MIN_VALUE, &zoneInfo);
        EXPECT_NE(zoneInfo, nullptr);
        String zID;
        ITimeZone::Probe(zoneInfo)->GetID(&zID);
        EXPECT_STREQ(id.string(), zID.string());
    }
}

TEST(ZoneInfoTest, TestReadTimeZoneValid)
{
    AutoPtr<ZoneInfoTestHelper::ZicDataBuilder> builder =
            new ZoneInfoTestHelper::ZicDataBuilder();
    builder->InitializeToValid();
    AutoPtr<ISystem> system;
    CSystem::New(IID_ISystem, (IInterface**)&system);
    Long millis;
    system->GetCurrentTimeMillis(millis);
    Array<Byte> bytes = builder->Build();
    AutoPtr<IZoneInfo> zoneInfo = CreateZoneInfo(String("ZoneInfoTest"),
            millis, bytes);
    EXPECT_NE(zoneInfo, nullptr);
}

TEST(ZoneInfoTest, TestReadTimeZoneBadMagic)
{
    AutoPtr<ZoneInfoTestHelper::ZicDataBuilder> builder =
            new ZoneInfoTestHelper::ZicDataBuilder();
    builder->InitializeToValid();
    builder->SetMagic(0xdeadbeef);
    AutoPtr<ISystem> system;
    CSystem::New(IID_ISystem, (IInterface**)&system);
    Long millis;
    system->GetCurrentTimeMillis(millis);
    Array<Byte> bytes = builder->Build();
    AutoPtr<IZoneInfo> zoneInfo = CreateZoneInfo(String("ZoneInfoTest"),
            millis, bytes);
    EXPECT_EQ(zoneInfo, nullptr);
}

TEST(ZoneInfoTest, TestReadTimeZoneTooManyTypes)
{
    AutoPtr<ZoneInfoTestHelper::ZicDataBuilder> builder =
            new ZoneInfoTestHelper::ZicDataBuilder();
    builder->InitializeToValid();
    builder->SetTypeCountOverride(257);
    AutoPtr<ISystem> system;
    CSystem::New(IID_ISystem, (IInterface**)&system);
    Long millis;
    system->GetCurrentTimeMillis(millis);
    Array<Byte> bytes = builder->Build();
    AutoPtr<IZoneInfo> zoneInfo = CreateZoneInfo(String("ZoneInfoTest"),
            millis, bytes);
    EXPECT_EQ(zoneInfo, nullptr);
}

TEST(ZoneInfoTest, TestReadTimeZoneTooManyTransitions)
{
    AutoPtr<ZoneInfoTestHelper::ZicDataBuilder> builder =
            new ZoneInfoTestHelper::ZicDataBuilder();
    builder->InitializeToValid();
    builder->SetTransitionCountOverride(2001);
    AutoPtr<ISystem> system;
    CSystem::New(IID_ISystem, (IInterface**)&system);
    Long millis;
    system->GetCurrentTimeMillis(millis);
    Array<Byte> bytes = builder->Build();
    AutoPtr<IZoneInfo> zoneInfo = CreateZoneInfo(String("ZoneInfoTest"),
            millis, bytes);
    EXPECT_EQ(zoneInfo, nullptr);
}

TEST(ZoneInfoTest, TestReadTimeZoneNegativeTypes)
{
    AutoPtr<ZoneInfoTestHelper::ZicDataBuilder> builder =
            new ZoneInfoTestHelper::ZicDataBuilder();
    builder->InitializeToValid();
    builder->SetTypeCountOverride(-1);
    AutoPtr<ISystem> system;
    CSystem::New(IID_ISystem, (IInterface**)&system);
    Long millis;
    system->GetCurrentTimeMillis(millis);
    Array<Byte> bytes = builder->Build();
    AutoPtr<IZoneInfo> zoneInfo = CreateZoneInfo(String("ZoneInfoTest"),
            millis, bytes);
    EXPECT_EQ(zoneInfo, nullptr);
}

TEST(ZoneInfoTest, TestReadTimeZoneNegativeTransitions)
{
    AutoPtr<ZoneInfoTestHelper::ZicDataBuilder> builder =
            new ZoneInfoTestHelper::ZicDataBuilder();
    builder->InitializeToValid();
    builder->SetTransitionCountOverride(-1);
    AutoPtr<ISystem> system;
    CSystem::New(IID_ISystem, (IInterface**)&system);
    Long millis;
    system->GetCurrentTimeMillis(millis);
    Array<Byte> bytes = builder->Build();
    AutoPtr<IZoneInfo> zoneInfo = CreateZoneInfo(String("ZoneInfoTest"),
            millis, bytes);
    EXPECT_EQ(zoneInfo, nullptr);
}

TEST(ZoneInfoTest, TestReadTimeZoneTransitionsNotSorted)
{
    Array<Array<Integer>> transitions{
        {1000, 0}, {3000, 1}, {2000, 0} };
    Array<Array<Integer>> types{
        {3600, 0}, {1800, 1} };

    AutoPtr<ZoneInfoTestHelper::ZicDataBuilder> builder =
            new ZoneInfoTestHelper::ZicDataBuilder();
    builder->InitializeToValid();
    builder->SetTransitionsAndTypes(transitions, types);
    AutoPtr<ISystem> system;
    CSystem::New(IID_ISystem, (IInterface**)&system);
    Long millis;
    system->GetCurrentTimeMillis(millis);
    Array<Byte> bytes = builder->Build();
    AutoPtr<IZoneInfo> zoneInfo = CreateZoneInfo(String("ZoneInfoTest"),
            millis, bytes);
    EXPECT_EQ(zoneInfo, nullptr);
}

TEST(ZoneInfoTest, TestReadTimeZoneInvalidTypeIndex)
{
    Array<Array<Integer>> transitions{
        {1000, 0}, {2000, 2}, {3000, 0} };
    Array<Array<Integer>> types{
        {3600, 0}, {1800, 1} };

    AutoPtr<ZoneInfoTestHelper::ZicDataBuilder> builder =
            new ZoneInfoTestHelper::ZicDataBuilder();
    builder->InitializeToValid();
    builder->SetTransitionsAndTypes(transitions, types);
    AutoPtr<ISystem> system;
    CSystem::New(IID_ISystem, (IInterface**)&system);
    Long millis;
    system->GetCurrentTimeMillis(millis);
    Array<Byte> bytes = builder->Build();
    AutoPtr<IZoneInfo> zoneInfo = CreateZoneInfo(String("ZoneInfoTest"),
            millis, bytes);
    EXPECT_EQ(zoneInfo, nullptr);
}

TEST(ZoneInfoTest, TestReadTimeZoneInvalidIsDst)
{
    Array<Array<Integer>> transitions{
        {1000, 0}, {2000, 1}, {3000, 0} };
    Array<Array<Integer>> types{
        {3600, 0}, {1800, 2} };

    AutoPtr<ZoneInfoTestHelper::ZicDataBuilder> builder =
            new ZoneInfoTestHelper::ZicDataBuilder();
    builder->InitializeToValid();
    builder->SetTransitionsAndTypes(transitions, types);
    AutoPtr<ISystem> system;
    CSystem::New(IID_ISystem, (IInterface**)&system);
    Long millis;
    system->GetCurrentTimeMillis(millis);
    Array<Byte> bytes = builder->Build();
    AutoPtr<IZoneInfo> zoneInfo = CreateZoneInfo(String("ZoneInfoTest"),
            millis, bytes);
    EXPECT_EQ(zoneInfo, nullptr);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
