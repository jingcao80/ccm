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

#include <ccmautoptr.h>
#include <ccmobject.h>
#include <gtest/gtest.h>

using namespace ccm;

static Boolean CA_CREATED = false;
static Boolean CA_DESTROYED = false;

class CA
    : public Object
{
public:
    CA()
    {
        CA_CREATED = true;
    }

    ~CA()
    {
        CA_DESTROYED = true;
    }

    Integer Get()
    {
        return mValue;
    }

    void Set(
        /* [in] */ Integer i)
    {
        mValue = i;
    }

private:
    Integer mValue = 99;
};

AutoPtr<CA> CreateCA()
{
    return new CA();
}

void CreateCA2(
    /* [out] */ CA** o)
{
    *o = new CA();
    REFCOUNT_ADD(*o);
}

void Init()
{
    CA_CREATED = false;
    CA_DESTROYED = false;
}

void CallCA(CA* obj)
{
    EXPECT_TRUE(CA_CREATED);
    EXPECT_EQ(1, obj->GetStrongCount());
    EXPECT_FALSE(CA_DESTROYED);
}

void CallCA2(AutoPtr<CA> obj)
{
    EXPECT_TRUE(CA_CREATED);
    EXPECT_EQ(1, obj->GetStrongCount());
    EXPECT_FALSE(CA_DESTROYED);
}

void CallCA3(AutoPtr<CA>&& obj)
{
    EXPECT_TRUE(CA_CREATED);
    EXPECT_EQ(1, obj->GetStrongCount());
    EXPECT_FALSE(CA_DESTROYED);
}

TEST(AutoPtrTest, AutoPtrOutParameterTest)
{
    Init();
    EXPECT_FALSE(CA_CREATED);
    EXPECT_FALSE(CA_DESTROYED);
    AutoPtr<CA> ca;
    CreateCA2(&ca);
    EXPECT_TRUE(CA_CREATED);
    EXPECT_FALSE(CA_DESTROYED);
    EXPECT_EQ(99, ca->Get());
    ca->Set(999);
    EXPECT_EQ(999, ca->Get());
}

TEST(AutoPtrTest, AutoPtrParameterTest)
{
    Init();
    EXPECT_FALSE(CA_CREATED);
    EXPECT_FALSE(CA_DESTROYED);
    CallCA(CreateCA());
    EXPECT_TRUE(CA_DESTROYED);
    Init();
    EXPECT_FALSE(CA_CREATED);
    EXPECT_FALSE(CA_DESTROYED);
    CallCA2(new CA());
    EXPECT_TRUE(CA_DESTROYED);
    Init();
    EXPECT_FALSE(CA_CREATED);
    EXPECT_FALSE(CA_DESTROYED);
    CallCA3(AutoPtr<CA>(new CA()));
    EXPECT_TRUE(CA_DESTROYED);
}

TEST(AutoPtrTest, AutoPtrReturnValueTest1)
{
    Init();
    EXPECT_FALSE(CA_CREATED);
    EXPECT_FALSE(CA_DESTROYED);
    CA* obj = CreateCA();
    EXPECT_TRUE(CA_CREATED);
    EXPECT_TRUE(CA_DESTROYED);
}

TEST(AutoPtrTest, AutoPtrReturnValueTest2)
{
    Init();
    EXPECT_FALSE(CA_CREATED);
    EXPECT_FALSE(CA_DESTROYED);
    AutoPtr<CA> obj = CreateCA();
    EXPECT_TRUE(CA_CREATED);
    EXPECT_FALSE(CA_DESTROYED);
}

TEST(AutoPtrTest, AutoPtrReturnValueTest3)
{
    Init();
    EXPECT_FALSE(CA_CREATED);
    EXPECT_FALSE(CA_DESTROYED);
    {
        AutoPtr<CA> obj = CreateCA();
    }
    EXPECT_TRUE(CA_CREATED);
    EXPECT_TRUE(CA_DESTROYED);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
