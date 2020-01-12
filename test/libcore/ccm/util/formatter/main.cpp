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
#include "como/text/NumberFormatFactory.h"
#include "como/util/CalendarFactory.h"
#include "como/util/LocaleFactory.h"
#include "como/util/TimeZoneFactory.h"
#include "como.math.CBigDecimal.h"
#include "como.math.IBigDecimal.h"
#include "como.text.IDecimalFormat.h"
#include "como.text.IDecimalFormatSymbols.h"
#include "como.text.INumberFormat.h"
#include "como.util.CLocale.h"
#include "como.util.ICalendar.h"
#include "como.util.ILocale.h"
#include "como.util.ITimeZone.h"
#include <ccmautoptr.h>
#include <gtest/gtest.h>

using namespace como;
using como::core::CoreUtils;
using como::core::StringUtils;
using como::math::CBigDecimal;
using como::math::IBigDecimal;
using como::math::IID_IBigDecimal;
using como::text::IDecimalFormat;
using como::text::IDecimalFormatSymbols;
using como::text::INumberFormat;
using como::text::NumberFormatFactory;
using como::util::CalendarFactory;
using como::util::CLocale;
using como::util::IID_ILocale;
using como::util::ICalendar;
using como::util::ILocale;
using como::util::LocaleFactory;
using como::util::ITimeZone;
using como::util::TimeZoneFactory;

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

    arabicStr = StringUtils::Format(arabic, String("12 %,d 34"), &args);
    arabicChars = {
        U'1', U'2', U' ',
        0x0661, 0x066c, 0x0662, 0x0663, 0x0664, 0x066c, 0x0665, 0x0666, 0x0667, 0x066c, 0x0668, 0x0669, 0x0660,
        U' ', U'3', U'4'
    };
    EXPECT_STREQ(arabicStr.string(), String(arabicChars).string());

    args = { CoreUtils::Box(1.23) };
    arabicStr = StringUtils::Format(arabic, String("12 %.3e 34"), &args);
    arabicChars = {
        U'1', U'2', U' ',
        0x0661, 0x066b, 0x0662, 0x0663, 0x0660, 0x0627, 0x0633, U'+', 0x0660, 0x0660,
        U' ', U'3', U'4'
    };
    EXPECT_STREQ(arabicStr.string(), String(arabicChars).string());

    arabicStr = StringUtils::Format(arabic, String("12 %.3f 34"), &args);
    arabicChars = {
        U'1', U'2', U' ',
        0x0661, 0x066b, 0x0662, 0x0663, 0x0660,
        U' ', U'3', U'4'
    };
    EXPECT_STREQ(arabicStr.string(), String(arabicChars).string());

    arabicStr = StringUtils::Format(arabic, String("12 %.3g 34"), &args);
    arabicChars = {
        U'1', U'2', U' ',
        0x0661, 0x066b, 0x0662, 0x0663,
        U' ', U'3', U'4'
    };
    EXPECT_STREQ(arabicStr.string(), String(arabicChars).string());
    // And date/time formatting (we assume that all time/date number formatting is done by the
    // same code, so this is representative):
    AutoPtr<ITimeZone> tz;
    TimeZoneFactory::GetTimeZone(String("GMT-08:00"), &tz);
    AutoPtr<ICalendar> c;
    CalendarFactory::GetInstance(tz, &c);
    c->SetTimeInMillis(0);
    args = { c };
    arabicStr = StringUtils::Format(arabic, String("12 %tT 34"), &args);
    arabicChars = {
        U'1', U'2', U' ',
        0x0661, 0x0666, U':', 0x0660, 0x0660, U':', 0x0660, 0x0660,
        U' ', U'3', U'4'
    };
    EXPECT_STREQ(arabicStr.string(), String(arabicChars).string());
    // These shouldn't get localized:
    arabicStr = StringUtils::Format(arabic, String("1234"), nullptr);
    EXPECT_STREQ(arabicStr.string(), "1234");
    args = { CoreUtils::Box(String("1234")) };
    arabicStr = StringUtils::Format(arabic, String("%s"), &args);
    EXPECT_STREQ(arabicStr.string(), "1234");
    args = { CoreUtils::Box(1234) };
    arabicStr = StringUtils::Format(arabic, String("%s"), &args);
    EXPECT_STREQ(arabicStr.string(), "1234");
    args = { CoreUtils::Box(1234) };
    arabicStr = StringUtils::Format(arabic, String("%o"), &args);
    EXPECT_STREQ(arabicStr.string(), "2322");
    args = { CoreUtils::Box(1234) };
    arabicStr = StringUtils::Format(arabic, String("%x"), &args);
    EXPECT_STREQ(arabicStr.string(), "4d2");
    args = { CoreUtils::Box(1.0) };
    arabicStr = StringUtils::Format(arabic, String("%a"), &args);
    EXPECT_STREQ(arabicStr.string(), "0x1.0p0");
}

TEST(FormatterTest, TestInternationalizedExponent)
{
    AutoPtr<ILocale> eng;
    LocaleFactory::GetENGLISH(&eng);

    Array<IInterface*> args{ CoreUtils::Box(100.0) };
    String engStr = StringUtils::Format(eng, String("%.0E"), &args);
    EXPECT_STREQ(engStr.string(), "1E+02");

    engStr = StringUtils::Format(eng, String("%.0e"), &args);
    EXPECT_STREQ(engStr.string(), "1e+02");

    AutoPtr<ILocale> arabic;
    CLocale::New(String("ar"), IID_ILocale, (IInterface**)&arabic);

    String arabicStr = StringUtils::Format(arabic, String("%.0E"), &args);
    Array<Char> arabicChars{
        0x0661, 0x0627, 0x0633, U'+', 0x0660, 0x0662
    };
    EXPECT_STREQ(arabicStr.string(), String(arabicChars).string());

    arabicStr = StringUtils::Format(arabic, String("%.0e"), &args);
    arabicChars = {
        0x0661, 0x0627, 0x0633, U'+', 0x0660, 0x0662
    };
    EXPECT_STREQ(arabicStr.string(), String(arabicChars).string());

    AutoPtr<ILocale> et;
    CLocale::New(String("et"), IID_ILocale, (IInterface**)&et);

    String etStr = StringUtils::Format(et, String("%.0E"), &args);
    Array<Char> etChars = {
        U'1', 0x00d7, U'1', U'0', U'^', U'+', U'0', U'2'
    };
    EXPECT_STREQ(etStr.string(), String(etChars).string());

    etStr = StringUtils::Format(et, String("%.0e"), &args);
    etChars = {
        U'1', 0x00d7, U'1', U'0', U'^', U'+', U'0', U'2'
    };
    EXPECT_STREQ(etStr.string(), String(etChars).string());
}

TEST(FormatterTest, TestUppercaseConversiont)
{
    AutoPtr<ILocale> us;
    LocaleFactory::GetENGLISH(&us);

    Array<IInterface*> args{ CoreUtils::Box(String("jakob arjouni")) };
    String usStr = StringUtils::Format(us, String("%S"), &args);
    EXPECT_STREQ(usStr.string(), "JAKOB ARJOUNI");

#ifdef __aarch64__
    AutoPtr<ILocale> tr;
    CLocale::New(String("tr"), String("TR"), IID_ILocale, (IInterface**)&tr);
    String trStr = StringUtils::Format(tr, String("%S"), &args);
    Array<Char> trChars{ 0x0130 };
    EXPECT_STREQ(trStr.string(), (String("JAKOB ARJOUN") + String(trChars)).string());
#endif
}

TEST(FormatterTest, TestNumberFormatReuse)
{
    Array<IInterface*> args{ CoreUtils::Box(7.0), CoreUtils::Box(7) };
    String usStr = StringUtils::Format(String("%.6f %d"), &args);
    EXPECT_STREQ(usStr.string(), "7.000000 7");
}

TEST(FormatterTest, TestGrouping)
{
    // The interesting case is -123, where you might naively output "-,123" if you're just
    // inserting a separator every three characters. The cases where there are three digits
    // before the first separator may also be interesting.
    Array<IInterface*> args{ CoreUtils::Box(-1) };
    String usStr = StringUtils::Format(String("%,d"), &args);
    EXPECT_STREQ(usStr.string(), "-1");

    args = { CoreUtils::Box(-12) };
    usStr = StringUtils::Format(String("%,d"), &args);
    EXPECT_STREQ(usStr.string(), "-12");

    args = { CoreUtils::Box(-123) };
    usStr = StringUtils::Format(String("%,d"), &args);
    EXPECT_STREQ(usStr.string(), "-123");

    args = { CoreUtils::Box(-1234) };
    usStr = StringUtils::Format(String("%,d"), &args);
    EXPECT_STREQ(usStr.string(), "-1,234");

    args = { CoreUtils::Box(-12345) };
    usStr = StringUtils::Format(String("%,d"), &args);
    EXPECT_STREQ(usStr.string(), "-12,345");

    args = { CoreUtils::Box(-123456) };
    usStr = StringUtils::Format(String("%,d"), &args);
    EXPECT_STREQ(usStr.string(), "-123,456");

    args = { CoreUtils::Box(-1234567) };
    usStr = StringUtils::Format(String("%,d"), &args);
    EXPECT_STREQ(usStr.string(), "-1,234,567");

    args = { CoreUtils::Box(-12345678) };
    usStr = StringUtils::Format(String("%,d"), &args);
    EXPECT_STREQ(usStr.string(), "-12,345,678");

    args = { CoreUtils::Box(-123456789) };
    usStr = StringUtils::Format(String("%,d"), &args);
    EXPECT_STREQ(usStr.string(), "-123,456,789");

    args = { CoreUtils::Box(1) };
    usStr = StringUtils::Format(String("%,d"), &args);
    EXPECT_STREQ(usStr.string(), "1");

    args = { CoreUtils::Box(12) };
    usStr = StringUtils::Format(String("%,d"), &args);
    EXPECT_STREQ(usStr.string(), "12");

    args = { CoreUtils::Box(123) };
    usStr = StringUtils::Format(String("%,d"), &args);
    EXPECT_STREQ(usStr.string(), "123");

    args = { CoreUtils::Box(1234) };
    usStr = StringUtils::Format(String("%,d"), &args);
    EXPECT_STREQ(usStr.string(), "1,234");

    args = { CoreUtils::Box(12345) };
    usStr = StringUtils::Format(String("%,d"), &args);
    EXPECT_STREQ(usStr.string(), "12,345");

    args = { CoreUtils::Box(123456) };
    usStr = StringUtils::Format(String("%,d"), &args);
    EXPECT_STREQ(usStr.string(), "123,456");

    args = { CoreUtils::Box(1234567) };
    usStr = StringUtils::Format(String("%,d"), &args);
    EXPECT_STREQ(usStr.string(), "1,234,567");

    args = { CoreUtils::Box(12345678) };
    usStr = StringUtils::Format(String("%,d"), &args);
    EXPECT_STREQ(usStr.string(), "12,345,678");

    args = { CoreUtils::Box(123456789) };
    usStr = StringUtils::Format(String("%,d"), &args);
    EXPECT_STREQ(usStr.string(), "123,456,789");
}

TEST(FormatterTest, TestFormatNull)
{
    // We fast-path %s and %d (with no configuration) but need to make sure we handle the
    // special case of the null argument...
    AutoPtr<ILocale> us;
    LocaleFactory::GetENGLISH(&us);

    String usStr = StringUtils::Format(us, String("%s"), nullptr);
    EXPECT_STREQ(usStr.string(), "null");

    usStr = StringUtils::Format(us, String("%d"), nullptr);
    EXPECT_STREQ(usStr.string(), "null");

    usStr = StringUtils::Format(us, String("%%"), nullptr);
    EXPECT_STREQ(usStr.string(), "%");
}

TEST(FormatterTest, TestBigDecimalFormatting)
{
    AutoPtr<IBigDecimal> bd;
    CBigDecimal::New(String("20.00000"), IID_IBigDecimal, (IInterface**)&bd);
    Array<IInterface*> args{ bd };
    String result = StringUtils::Format(String("%.2f"), &args);
    EXPECT_STREQ(result.string(), "20.00");

    bd = nullptr;
    CBigDecimal::New(String("20.000000"), IID_IBigDecimal, (IInterface**)&bd);
    args = { bd };
    result = StringUtils::Format(String("%.2f"), &args);
    EXPECT_STREQ(result.string(), "20.00");

    bd = nullptr;
    CBigDecimal::New(String(".2"), IID_IBigDecimal, (IInterface**)&bd);
    args = { bd };
    result = StringUtils::Format(String("%.2f"), &args);
    EXPECT_STREQ(result.string(), "0.20");

    bd = nullptr;
    CBigDecimal::New(String("2"), IID_IBigDecimal, (IInterface**)&bd);
    args = { bd };
    result = StringUtils::Format(String("%.2f"), &args);
    EXPECT_STREQ(result.string(), "2.00");

    bd = nullptr;
    CBigDecimal::New(String("-2"), IID_IBigDecimal, (IInterface**)&bd);
    args = { bd };
    result = StringUtils::Format(String("%.2f"), &args);
    EXPECT_STREQ(result.string(), "-2.00");

    bd = nullptr;
    CBigDecimal::New(String("200000000000000000000000"), IID_IBigDecimal, (IInterface**)&bd);
    args = { bd };
    result = StringUtils::Format(String("%.2f"), &args);
    EXPECT_STREQ(result.string(), "200000000000000000000000.00");

    bd = nullptr;
    CBigDecimal::New(String("2000000000000000000000000000000000000000000000000"), IID_IBigDecimal, (IInterface**)&bd);
    args = { bd };
    result = StringUtils::Format(String("%.2f"), &args);
    EXPECT_STREQ(result.string(), "2000000000000000000000000000000000000000000000000.00");
}

TEST(FormatterTest, Test42936)
{
    Array<IInterface*> args{ CoreUtils::Box(0.0) };
    String result = StringUtils::Format(String("%.15g"), &args);
    EXPECT_STREQ(result.string(), "0.00000000000000");
}

TEST(FormatterTest, TestGroupingSizeZero)
{
#ifdef __aarch64__
    AutoPtr<ILocale> localeWithoutGrouping;
    CLocale::New(String("en"), String("US"), String("POSIX"), IID_ILocale, (IInterface**)&localeWithoutGrouping);
    AutoPtr<INumberFormat> nf;
    NumberFormatFactory::GetInstance(localeWithoutGrouping, &nf);
    IDecimalFormat* decimalFormat = IDecimalFormat::Probe(nf);

    // Confirm the locale is still a good example: it has a group separator, but no grouping in
    // the default decimal format.
    Integer groupingSize;
    decimalFormat->GetGroupingSize(&groupingSize);
    EXPECT_EQ(0, groupingSize);
    Boolean groupingUsed;
    decimalFormat->IsGroupingUsed(&groupingUsed);
    EXPECT_FALSE(groupingUsed);
    AutoPtr<IDecimalFormatSymbols> symbols;
    decimalFormat->GetDecimalFormatSymbols(&symbols);
    Char separator;
    symbols->GetGroupingSeparator(&separator);
    EXPECT_TRUE(separator != U'\0');
#endif
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
