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

#include "core/CoreUtils.h"
#include "core/StringUtils.h"
#include "ccm.util.CLocale.h"
#include "ccm.util.ILocale.h"
#include <ccmautoptr.h>
#include <gtest/gtest.h>

using namespace ccm;
using ccm::core::CoreUtils;
using ccm::core::StringUtils;
using ccm::util::CLocale;
using ccm::util::IID_ILocale;
using ccm::util::ILocale;

TEST(FormatterTest, TestNumberLocalization)
{
    AutoPtr<ILocale> arabic;
    CLocale::New(String("ar"), IID_ILocale, (IInterface**)&arabic);

    Array<IInterface*> args{ CoreUtils::Box(1234567890) };
    String arabicStr = StringUtils::Format(arabic, String("12 %d 34"), &args);
    Array<Char> arabicChars{
        U'1', U'2', U' ',
        0x0661, 0x0662, 0x0663, 0x0664, 0x0665, 0x0666, 0x0667, 0x0668, 0x0669, 0x0660,
        U' ', U'3', U'4'
    };
    EXPECT_STREQ(arabicStr.string(), String(arabicChars).string());
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
