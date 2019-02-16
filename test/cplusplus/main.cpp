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

#include <ccmobject.h>
#include <gtest/gtest.h>

using namespace ccm;

static Boolean CA_CREATED = false;
static Boolean CA_DESTROYED = false;

static Boolean CB_CREATED = false;
static Boolean CB_DESTROYED = false;

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
};

class CB
    : public Object
{
public:
    CB()
    {
        CB_CREATED = true;
    }

    ~CB()
    {
        CB_DESTROYED = true;
    }
};

static CA sObject;

CB& GetCB()
{
    static CB sObject;
    return sObject;
}

void Init()
{
    CA_CREATED = false;
    CA_DESTROYED = false;
    CB_CREATED = false;
    CB_DESTROYED = false;
}

TEST(CPPTest, FunctionStaticVariableTest)
{
    EXPECT_TRUE(CA_CREATED);
    EXPECT_FALSE(CA_DESTROYED);
    EXPECT_FALSE(CB_CREATED);
    EXPECT_FALSE(CB_DESTROYED);
    {
        CB& obj = GetCB();
    }
    EXPECT_TRUE(CA_CREATED);
    EXPECT_FALSE(CA_DESTROYED);
    EXPECT_TRUE(CB_CREATED);
    EXPECT_FALSE(CB_DESTROYED);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
