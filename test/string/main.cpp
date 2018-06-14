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

#include <core/StringUtils.h>
#include <ccmautoptr.h>
#include <stdio.h>

using namespace ccm;
using ccm::core::StringUtils;

int main(int argv, char** argc)
{
    String byteStr("119");
    Byte b;
    StringUtils::ParseByte(byteStr, &b);
    printf("==== StringUtils::ParseByte(\"%s\") is %d ====\n", byteStr.string(), b);

    String byteHexStr("77");
    StringUtils::ParseByte(byteHexStr, 16, &b);
    printf("==== StringUtils::ParseByte(\"%s\", 16) is %d ====\n", byteHexStr.string(), b);

    printf("==== StringUtils::ToString(%d) is %s ====\n", b, StringUtils::ToString(b).string());


    String shortStr("12345");
    Short s;
    StringUtils::ParseShort(shortStr, &s);
    printf("==== StringUtils::ParseShort(\"%s\") is %d ====\n", shortStr.string(), s);

    String shortHexStr("3039");
    StringUtils::ParseShort(shortHexStr, 16, &s);
    printf("==== StringUtils::ParseShort(\"%s\", 16) is %d ====\n", shortHexStr.string(), s);

    printf("==== StringUtils::ToString(%d) is %s ====\n", s, StringUtils::ToString(s).string());


    String floatStr("0.01234");
    Float a;
    StringUtils::ParseFloat(floatStr, &a);
    printf("==== StringUtils::ParseFloat(\"%s\") is %f ====\n", floatStr.string(), a);

    printf("==== StringUtils::ToString(%f) is %s ====\n", a, StringUtils::ToString(a).string());


    String doubleStr("0.056789e-23");
    Double d;
    StringUtils::ParseDouble(doubleStr, &d);
    printf("==== StringUtils::ParseDouble(\"%s\") is %e ====\n", doubleStr.string(), d);

    printf("==== StringUtils::ToString(%e) is %s ====\n", d, StringUtils::ToString(d).string());

    return 0;
}
