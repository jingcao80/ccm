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

#include "ccm.io.charset.CCharsetFactory.h"
#include "ccm.io.charset.ICharset.h"
#include "ccm.io.charset.ICharsetFactory.h"
#include <ccmautoptr.h>
#include <gtest/gtest.h>

using ccm::io::charset::CCharsetFactory;
using ccm::io::charset::ICharset;
using ccm::io::charset::ICharsetFactory;
using ccm::io::charset::IID_ICharsetFactory;

TEST(CharsetTest, TestNonstandardCharsetName)
{
    AutoPtr<ICharsetFactory> factory;
    CCharsetFactory::New(IID_ICharsetFactory, (IInterface**)&factory);
    AutoPtr<ICharset> cs;
    factory->ForName(String("UTF8"), &cs);
    EXPECT_TRUE(cs != nullptr);
    String name;
    cs->GetName(&name);
    EXPECT_STREQ("UTF-8", name.string());

    AutoPtr<ICharset> cs2;
    factory->ForName(String("UTF8"), &cs2);
    EXPECT_TRUE(cs2 != nullptr);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
