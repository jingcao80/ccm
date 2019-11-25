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

#include <comoobj.h>
#include <gtest/gtest.h>

using namespace como;

static const InterfaceID IID_ICharSequence =
        {{0x9c006766,0xd20c,0x4b8a,0xb3cc,{0x96,0x29,0x9a,0x3e,0xf7,0xd0}}, nullptr};

interface ICharSequence
    : public IInterface
{
    using IInterface::Probe;

    inline static ICharSequence* Probe(
        /* [in] */ IInterface* object)
    {
        if (object == nullptr) {
            return nullptr;
        }
        return (ICharSequence*)object->Probe(IID_ICharSequence);
    }

    inline static const InterfaceID& GetInterfaceID()
    {
        return IID_ICharSequence;
    }

    virtual ECode GetCharAt(
        /* [in] */ Integer index,
        /* [out] */ Char& c) = 0;

    virtual ECode GetLength(
        /* [out] */ Integer& number) = 0;

    virtual ECode SubSequence(
        /* [in] */ Integer start,
        /* [in] */ Integer end,
        /* [out] */ AutoPtr<ICharSequence>& subcsq) = 0;

    virtual ECode ToString(
        /* [out] */ String& str) = 0;
};

class CString
    : public Object
    , public ICharSequence
{
public:
    CString(
        /* [in] */ const String& str)
        : mString(str)
    {}

    Integer AddRef(
        /* [in] */ HANDLE id) override
    {
        return Object::AddRef(id);
    }

    Integer Release(
        /* [in] */ HANDLE id) override
    {
        return Object::Release(id);
    }

    IInterface* Probe(
        /* [in] */ const InterfaceID& iid) override
    {
        if (iid == IID_IInterface) {
            return (IInterface*)(ICharSequence*)this;
        }
        else if (iid == IID_ICharSequence) {
            return (ICharSequence*)this;
        }
        return Object::Probe(iid);
    }

    ECode GetInterfaceID(
        /* [in] */ IInterface* object,
        /* [out] */ InterfaceID& iid) override
    {
        if (object == (IInterface*)(ICharSequence*)this) {
            iid = IID_ICharSequence;
            return NOERROR;
        }
        return Object::GetInterfaceID(object, iid);
    }

    ECode GetCharAt(
        /* [in] */ Integer index,
        /* [out] */ Char& c) override
    {
        c = mString.GetChar(index);
        return NOERROR;
    }

    ECode GetLength(
        /* [out] */ Integer& number) override
    {
        number = mString.GetLength();
        return NOERROR;
    }

    ECode SubSequence(
        /* [in] */ Integer start,
        /* [in] */ Integer end,
        /* [out] */ AutoPtr<ICharSequence>& subcsq) override
    {
        subcsq = new CString(mString.Substring(start, end));
        return NOERROR;
    }

    ECode ToString(
        /* [out] */ String& str) override
    {
        str = mString;
        return NOERROR;
    }

private:
    String mString;
};

static Array<ICharSequence*> Box(
    /* [in] */ const Array<String>& strArray)
{
    if (strArray.IsEmpty()) {
        return Array<ICharSequence*>::Null();
    }

    Long size = strArray.GetLength();
    Array<ICharSequence*> seqArray(size);
    for (Long i = 0; i < size; i++) {
        AutoPtr<ICharSequence> cs = new CString(strArray[i]);
        seqArray.Set(i, cs);
    }
    return seqArray;
}

class CA
    : public Object
{
public:
    CA()
    {
        CONS_COUNT++;
    }

    CA(
        /* [in] */ Integer value)
        : mValue(value)
    {}

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

    Integer mValue;
};

Integer CA::CONS_COUNT;
Integer CA::ADD_COUNT;
Integer CA::RELEASE_COUNT;
Integer CA::DEST_COUNT;

TEST(ArrayTest, TestObjectArray)
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

TEST(ArrayTest, TestObjectArraySelfAssignment)
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

TEST(ArrayTest, TestObjectArrayClear)
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

TEST(ArrayTest, TestObjectArrayArray)
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

TEST(ArrayTest, TestObjectArrayArraySelfAssignment)
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

TEST(ArrayTest, TestObjectArrayArrayClear)
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

TEST(ArrayTest, TestICharSequenceArrayToInterfaceArray)
{
    Array<String> strArray(3);
    strArray[0] = "hello";
    strArray[1] = "world";
    strArray[2] = "helloworld";
    Array<ICharSequence*> seqArray = Box(strArray);
    String str0, str1, str2;
    seqArray[0]->ToString(str0);
    seqArray[1]->ToString(str1);
    seqArray[2]->ToString(str2);
    EXPECT_STREQ(str0.string(), "hello");
    EXPECT_STREQ(str1.string(), "world");
    EXPECT_STREQ(str2.string(), "helloworld");

    Array<IInterface*> itfArray = seqArray;
    ICharSequence::Probe(seqArray[0])->ToString(str0);
    ICharSequence::Probe(seqArray[1])->ToString(str1);
    ICharSequence::Probe(seqArray[2])->ToString(str2);
    EXPECT_STREQ(str0.string(), "hello");
    EXPECT_STREQ(str1.string(), "world");
    EXPECT_STREQ(str2.string(), "helloworld");
}

TEST(ArrayTest, TestIntegerArrayInitializerListConstructor)
{
    Array<Integer> intArray{ 1, 2, 3 };
    EXPECT_EQ(intArray.GetLength(), 3);
    EXPECT_EQ(intArray[0], 1);
    EXPECT_EQ(intArray[1], 2);
    EXPECT_EQ(intArray[2], 3);
}

TEST(ArrayTest, TestIntegerArrayInitializerListCopyConstructor)
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

TEST(ArrayTest, TestObjectArrayInitializerListConstructor)
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

TEST(ArrayTest, TestObjectArrayInitializerListCopyConstructor)
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

TEST(ArrayTest, TestObjectArrayStdMove)
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

TEST(ArrayTest, TestIntegerRangeFor)
{
    Array<Integer> intArray{
        9, 99, 999, 9999, 99999
    };
    EXPECT_TRUE(intArray.GetPayload() == intArray.begin());
    EXPECT_TRUE(intArray.GetPayload() + 5 == intArray.end());
    Integer i = 0;
    for (Integer v : intArray) {
        if (i == 0) {
            EXPECT_EQ(9, v);
        }
        else if (i == 1) {
            EXPECT_EQ(99, v);
        }
        else if (i == 2) {
            EXPECT_EQ(999, v);
        }
        else if (i == 3) {
            EXPECT_EQ(9999, v);
        }
        else if (i == 4) {
            EXPECT_EQ(99999, v);
        }
        i++;
    }
    EXPECT_EQ(5, i);
}

TEST(ArrayTest, TestObjectRangeFor)
{
    Array<CA*> objArray{
        new CA(9), new CA(99), new CA(999), new CA(9999), new CA(99999)
    };
    EXPECT_TRUE(objArray.GetPayload() == objArray.begin());
    EXPECT_TRUE(objArray.GetPayload() + 5 == objArray.end());
    Integer i = 0;
    for (AutoPtr<CA> o : objArray) {
        if (i == 0) {
            EXPECT_EQ(9, o->mValue);
            EXPECT_EQ(2, o->GetStrongCount());
        }
        else if (i == 1) {
            EXPECT_EQ(99, o->mValue);
            EXPECT_EQ(2, o->GetStrongCount());
        }
        else if (i == 2) {
            EXPECT_EQ(999, o->mValue);
            EXPECT_EQ(2, o->GetStrongCount());
        }
        else if (i == 3) {
            EXPECT_EQ(9999, o->mValue);
            EXPECT_EQ(2, o->GetStrongCount());
        }
        else if (i == 4) {
            EXPECT_EQ(99999, o->mValue);
            EXPECT_EQ(2, o->GetStrongCount());
        }
        i++;
    }
    EXPECT_EQ(5, i);
    i = 0;
    for (CA* o : objArray) {
        if (i == 0) {
            EXPECT_EQ(9, o->mValue);
            EXPECT_EQ(1, o->GetStrongCount());
        }
        else if (i == 1) {
            EXPECT_EQ(99, o->mValue);
            EXPECT_EQ(1, o->GetStrongCount());
        }
        else if (i == 2) {
            EXPECT_EQ(999, o->mValue);
            EXPECT_EQ(1, o->GetStrongCount());
        }
        else if (i == 3) {
            EXPECT_EQ(9999, o->mValue);
            EXPECT_EQ(1, o->GetStrongCount());
        }
        else if (i == 4) {
            EXPECT_EQ(99999, o->mValue);
            EXPECT_EQ(1, o->GetStrongCount());
        }
        i++;
    }
    EXPECT_EQ(5, i);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
