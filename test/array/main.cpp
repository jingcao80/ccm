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

#include "ccmobject.h"
#include "ccm.core.ICharSequence.h"
#include <core/CoreUtils.h>
#include <gtest/gtest.h>

using namespace ccm;
using ccm::core::CoreUtils;
using ccm::core::ICharSequence;

class CA
    : public Object
{
public:
    CA()
    {
        CONS_COUNT++;
    }

    ~CA()
    {
        DEST_COUNT++;
    }

    Integer AddRef(
        /* [in] */ HANDLE id) override
    {
        ADD_COUNT++;
        return Object::AddRef(id);
    }

    Integer Release(
        /* [in] */ HANDLE id) override
    {
        RELEASE_COUNT++;
        return Object::Release(id);
    }

    static void Initialize()
    {
        CONS_COUNT = 0;
        ADD_COUNT = 0;
        RELEASE_COUNT = 0;
        DEST_COUNT = 0;
    }

public:
    static Integer CONS_COUNT;
    static Integer ADD_COUNT;
    static Integer RELEASE_COUNT;
    static Integer DEST_COUNT;
};

Integer CA::CONS_COUNT;
Integer CA::ADD_COUNT;
Integer CA::RELEASE_COUNT;
Integer CA::DEST_COUNT;

TEST(ArrayTest, ObjectArrayTest)
{
    CA::Initialize();
    Integer size = 199;
    Array<IObject*> array(size);
    for (Integer i = 0; i < size; i++) {
        array.Set(i, (IObject*)new CA());
    }
    array = Array<IObject*>::Null();
    EXPECT_EQ(CA::CONS_COUNT, size);
    EXPECT_EQ(CA::ADD_COUNT, size);
    EXPECT_EQ(CA::RELEASE_COUNT, size);
    EXPECT_EQ(CA::DEST_COUNT, size);
}

TEST(ArrayTest, ObjectArraySelfAssignmentTest)
{
    CA::Initialize();
    Integer size = 199;
    Array<IObject*> array(size);
    for (Integer i = 0; i < size; i++) {
        array.Set(i, (IObject*)new CA());
    }
    array = array;
    EXPECT_EQ(CA::CONS_COUNT, size);
    EXPECT_EQ(CA::ADD_COUNT, size);
    EXPECT_EQ(CA::RELEASE_COUNT, 0);
    EXPECT_EQ(CA::DEST_COUNT, 0);
}

TEST(ArrayTest, ObjectArrayClearTest)
{
    CA::Initialize();
    Integer size = 99;
    Array<IObject*> array(size);
    for (Integer i = 0; i < size; i++) {
        array.Set(i, (IObject*)new CA());
    }
    array.Clear();
    EXPECT_EQ(CA::CONS_COUNT, size);
    EXPECT_EQ(CA::ADD_COUNT, size);
    EXPECT_EQ(CA::RELEASE_COUNT, size);
    EXPECT_EQ(CA::DEST_COUNT, size);
}

TEST(ArrayTest, ObjectArrayArrayTest)
{
    CA::Initialize();
    Integer arraySize = 9;
    Integer size = 199;
    Array<Array<IObject*>> arrays(arraySize);
    for (Integer i = 0; i < arraySize; i++) {
        arrays[i] = Array<IObject*>(199);
        for (Integer j = 0; j < size; j++) {
            arrays[i].Set(j, (IObject*)new CA());
        }
    }
    arrays = Array<Array<IObject*>>::Null();
    EXPECT_EQ(CA::CONS_COUNT, arraySize * size);
    EXPECT_EQ(CA::ADD_COUNT, arraySize * size);
    EXPECT_EQ(CA::RELEASE_COUNT, arraySize * size);
    EXPECT_EQ(CA::DEST_COUNT, arraySize * size);
}

TEST(ArrayTest, ObjectArrayArraySelfAssignmentTest)
{
    CA::Initialize();
    Integer arraySize = 9;
    Integer size = 199;
    Array<Array<IObject*>> arrays(arraySize);
    for (Integer i = 0; i < arraySize; i++) {
        arrays[i] = Array<IObject*>(199);
        for (Integer j = 0; j < size; j++) {
            arrays[i].Set(j, (IObject*)new CA());
        }
    }
    arrays = arrays;
    EXPECT_EQ(CA::CONS_COUNT, arraySize * size);
    EXPECT_EQ(CA::ADD_COUNT, arraySize * size);
    EXPECT_EQ(CA::RELEASE_COUNT, 0);
    EXPECT_EQ(CA::DEST_COUNT, 0);
}

TEST(ArrayTest, ObjectArrayArrayClearTest)
{
    CA::Initialize();
    Integer arraySize = 9;
    Integer size = 99;
    Array<Array<IObject*>> arrays(arraySize);
    for (Integer i = 0; i < arraySize; i++) {
        arrays[i] = Array<IObject*>(99);
        for (Integer j = 0; j < size; j++) {
            arrays[i].Set(j, (IObject*)new CA());
        }
    }
    arrays.Clear();
    EXPECT_EQ(CA::CONS_COUNT, arraySize * size);
    EXPECT_EQ(CA::ADD_COUNT, arraySize * size);
    EXPECT_EQ(CA::RELEASE_COUNT, arraySize * size);
    EXPECT_EQ(CA::DEST_COUNT, arraySize * size);
}

TEST(ArrayTest, ICharSequenceArrayToInterfaceArrayTest)
{
    Array<String> strArray(3);
    strArray[0] = "hello";
    strArray[1] = "world";
    strArray[2] = "helloworld";
    Array<ICharSequence*> seqArray = CoreUtils::Box(strArray);
    String str0, str1, str2;
    seqArray[0]->ToString(&str0);
    seqArray[1]->ToString(&str1);
    seqArray[2]->ToString(&str2);
    EXPECT_STREQ(str0.string(), "hello");
    EXPECT_STREQ(str1.string(), "world");
    EXPECT_STREQ(str2.string(), "helloworld");

    Array<IInterface*> itfArray = seqArray.ToInterfaces();
    ICharSequence::Probe(seqArray[0])->ToString(&str0);
    ICharSequence::Probe(seqArray[1])->ToString(&str1);
    ICharSequence::Probe(seqArray[2])->ToString(&str2);
    EXPECT_STREQ(str0.string(), "hello");
    EXPECT_STREQ(str1.string(), "world");
    EXPECT_STREQ(str2.string(), "helloworld");
}

TEST(ArrayTest, IntegerArrayInitializerListConstructorTest)
{
    Array<Integer> intArray{ 1, 2, 3 };
    EXPECT_EQ(intArray.GetLength(), 3);
    EXPECT_EQ(intArray[0], 1);
    EXPECT_EQ(intArray[1], 2);
    EXPECT_EQ(intArray[2], 3);
}

TEST(ArrayTest, IntegerArrayInitializerListCopyConstructorTest)
{
    Array<Integer> intArray;

    EXPECT_EQ(intArray.GetLength(), 0);

    intArray = { 9, 99, 999, 9999 };
    EXPECT_EQ(intArray.GetLength(), 4);
    EXPECT_EQ(intArray[0], 9);
    EXPECT_EQ(intArray[1], 99);
    EXPECT_EQ(intArray[2], 999);
    EXPECT_EQ(intArray[3], 9999);
}

TEST(ArrayTest, ObjectArrayInitializerListConstructorTest)
{
    CA::Initialize();
    Array<IObject*> objArray{
        new CA(), new CA(), new CA(), new CA(), new CA() };
    Long size = objArray.GetLength();
    EXPECT_EQ(size, 5);
    EXPECT_EQ(CA::CONS_COUNT, size);
    EXPECT_EQ(CA::ADD_COUNT, size);
    objArray.Clear();
    EXPECT_EQ(CA::RELEASE_COUNT, size);
    EXPECT_EQ(CA::DEST_COUNT, size);
    EXPECT_EQ(objArray.GetLength(), 0);
}

TEST(ArrayTest, ObjectArrayInitializerListCopyConstructorTest)
{
    CA::Initialize();
    Array<IObject*> objArray;
    EXPECT_EQ(objArray.GetLength(), 0);
    objArray = {
        new CA(), new CA(), new CA(), new CA(), new CA() };
    Long size = objArray.GetLength();
    EXPECT_EQ(size, 5);
    EXPECT_EQ(CA::CONS_COUNT, size);
    EXPECT_EQ(CA::ADD_COUNT, size);
    objArray.Clear();
    EXPECT_EQ(CA::RELEASE_COUNT, size);
    EXPECT_EQ(CA::DEST_COUNT, size);
    EXPECT_EQ(objArray.GetLength(), 0);
}

TEST(ArrayTest, ObjectArrayStdMoveTest)
{
    CA::Initialize();
    Array<CA*> objArray(2);
    objArray.Set(0, new CA());
    objArray.Set(1, new CA());
    EXPECT_TRUE(objArray[0] != nullptr);
    EXPECT_EQ(1, objArray[0]->GetStrongCount());
    EXPECT_TRUE(objArray[1] != nullptr);
    EXPECT_EQ(1, objArray[1]->GetStrongCount());
    AutoPtr<CA> obj;
    obj = objArray[0];
    EXPECT_TRUE(objArray[0] != nullptr);
    EXPECT_EQ(2, objArray[0]->GetStrongCount());
    obj = std::move(objArray[1]);
    EXPECT_TRUE(objArray[1] != nullptr);
    EXPECT_EQ(2, obj->GetStrongCount());
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
