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
#include <gtest/gtest.h>

using namespace ccm;

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

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
