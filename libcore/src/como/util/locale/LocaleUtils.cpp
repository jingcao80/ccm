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

#include "como/util/locale/LocaleUtils.h"

namespace como {
namespace util {
namespace locale {

Char LocaleUtils::ToUpper(
    /* [in] */ Char c)
{
    return IsLower(c) ? (Char)(c - 0x20) : c;
}

Char LocaleUtils::ToLower(
    /* [in] */ Char c)
{
    return IsUpper(c) ? (Char)(c + 0x20) : c;
}

String LocaleUtils::ToTitleString(
    /* [in] */ const String& s)
{
    Integer len;
    if ((len = s.GetByteLength()) == 0) {
        return s;
    }
    const char* str = s.string();
    Integer idx = 0;
    if (!IsLower(str[idx])) {
        for (idx = 1; idx < len; idx++) {
            if (IsUpper(str[idx])) {
                break;
            }
        }
    }
    if (idx == len) {
        return s;
    }

    Array<Byte> buf(len);
    for (Integer i = 0; i < len; i++) {
        char c = str[i];
        if (i == 0 && idx == 0) {
            buf[i] = (char)ToUpper(c);
        }
        else if (i < idx) {
            buf[i] = c;
        }
        else {
            buf[i] = (char)ToLower(c);
        }
    }
    return String(buf, 0, len);
}

Boolean LocaleUtils::IsAlphaString(
    /* [in] */ const String& s)
{
    Integer len = s.GetByteLength();
    for (Integer i = 0; i < len; i++) {
        if (!IsAlpha(s.string()[i])) {
            return false;
        }
    }
    return true;
}

Boolean LocaleUtils::IsNumericString(
    /* [in] */ const String& s)
{
    Integer len = s.GetByteLength();
    for (Integer i = 0; i < len; i++) {
        if (!IsNumeric(s.string()[i])) {
            return false;
        }
    }
    return true;
}

Boolean LocaleUtils::IsAlphaNumericString(
    /* [in] */ const String& s)
{
    Integer len = s.GetByteLength();
    for (Integer i = 0; i < len; i++) {
        if (!IsAlphaNumeric(s.string()[i])) {
            return false;
        }
    }
    return true;
}

}
}
}
