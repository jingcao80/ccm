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

#ifndef __COMO_UTIL_LOCALE_LOCALEUTILS_H__
#define __COMO_UTIL_LOCALE_LOCALEUTILS_H__

#include "como.util.IList.h"
#include "como.util.IMap.h"
#include "como.util.ISet.h"
#include <comotypes.h>

namespace como {
namespace util {
namespace locale {

class LocaleUtils
{
public:
    static Boolean CaseIgnoreMatch(
        /* [in] */ const String& s1,
        /* [in] */ const String& s2);

    static Integer CaseIgnoreCompare(
        /* [in] */ const String& s1,
        /* [in] */ const String& s2);

    static Char ToUpper(
        /* [in] */ Char c);

    static Char ToLower(
        /* [in] */ Char c);

    static String ToLowerString(
        /* [in] */ const String& s);

    static String ToUpperString(
        /* [in] */ const String& s);

    static String ToTitleString(
        /* [in] */ const String& s);

    static Boolean IsAlpha(
        /* [in] */ Char c);

    static Boolean IsAlphaString(
        /* [in] */ const String& s);

    static Boolean IsNumeric(
        /* [in] */ Char c);

    static Boolean IsNumericString(
        /* [in] */ const String& s);

    static Boolean IsAlphaNumeric(
        /* [in] */ Char c);

    static Boolean IsAlphaNumericString(
        /* [in] */ const String& s);

    static Boolean IsEmpty(
        /* [in] */ const String& str);

    static Boolean IsEmpty(
        /* [in] */ ISet* set);

    static Boolean IsEmpty(
        /* [in] */ IMap* map);

    static Boolean IsEmpty(
        /* [in] */ IList* list);

private:
    LocaleUtils();

    static Boolean IsUpper(
        /* [in] */ Char c);

    static Boolean IsLower(
        /* [in] */ Char c);
};

inline Boolean LocaleUtils::CaseIgnoreMatch(
    /* [in] */ const String& s1,
    /* [in] */ const String& s2)
{
    return s1.EqualsIgnoreCase(s2);
}

inline Integer LocaleUtils::CaseIgnoreCompare(
        /* [in] */ const String& s1,
        /* [in] */ const String& s2)
{
    return s1.CompareIgnoreCase(s2);
}

inline String LocaleUtils::ToLowerString(
    /* [in] */ const String& s)
{
    return s.ToLowerCase();
}

inline String LocaleUtils::ToUpperString(
    /* [in] */ const String& s)
{
    return s.ToUpperCase();
}

inline Boolean LocaleUtils::IsUpper(
    /* [in] */ Char c)
{
    return c >= U'A' && c <= U'Z';
}

inline Boolean LocaleUtils::IsLower(
    /* [in] */ Char c)
{
    return c >= U'a' && c <= U'z';
}

inline Boolean LocaleUtils::IsAlpha(
    /* [in] */ Char c)
{
    return (c >= U'A' && c <= U'Z') || (c >= U'a' && c <= U'z');
}

inline Boolean LocaleUtils::IsNumeric(
    /* [in] */ Char c)
{
    return (c >= U'0' && c <= U'9');
}

inline Boolean LocaleUtils::IsAlphaNumeric(
    /* [in] */ Char c)
{
    return IsAlpha(c) || IsNumeric(c);
}

inline Boolean LocaleUtils::IsEmpty(
    /* [in] */ const String& str)
{
    return str.IsEmpty();
}

inline Boolean LocaleUtils::IsEmpty(
    /* [in] */ ISet* set)
{
    Boolean empty;
    return set == nullptr || (set->IsEmpty(empty), empty);
}

inline Boolean LocaleUtils::IsEmpty(
    /* [in] */ IMap* map)
{
    Boolean empty;
    return map == nullptr || (map->IsEmpty(empty), empty);
}

inline Boolean LocaleUtils::IsEmpty(
    /* [in] */ IList* list)
{
    Boolean empty;
    return list == nullptr || (list->IsEmpty(empty), empty);
}

}
}
}

#endif // __COMO_UTIL_LOCALE_LOCALEUTILS_H__
