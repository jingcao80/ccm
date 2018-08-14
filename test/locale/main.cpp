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

#include "ccm.util.CLocale.h"
#include "ccm.util.ILocale.h"
#include <ccmautoptr.h>
#include <gtest/gtest.h>

using namespace ccm;
using ccm::util::CLocale;
using ccm::util::ILocale;
using ccm::util::IID_ILocale;

TEST(LocaleTest, NewTest)
{
    AutoPtr<ILocale> locale;
    ECode ec = CLocale::New(String("en"), IID_ILocale, (IInterface**)&locale);
    EXPECT_EQ(ec, NOERROR);
    EXPECT_TRUE(locale != nullptr);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
