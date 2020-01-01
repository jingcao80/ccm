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

#ifndef __COMO_UTIL_LOCALE_UNICODELOCALEEXTENSION_H__
#define __COMO_UTIL_LOCALE_UNICODELOCALEEXTENSION_H__

#include "como/util/locale/Extension.h"
#include "como/util/locale/LocaleUtils.h"
#include "como.util.IMap.h"
#include "como.util.ISet.h"
#include "como.util.ISortedMap.h"
#include "como.util.ISortedSet.h"
#include <ccmautoptr.h>

namespace como {
namespace util {
namespace locale {

extern const CoclassID CID_UnicodeLocaleExtension;

COCLASS_ID(94c64925-5b95-400c-ac19-4d2568340a47)
class UnicodeLocaleExtension
    : public Extension
{
public:
    UnicodeLocaleExtension(
        /* [in] */ ISortedSet* attributes,
        /* [in] */ ISortedMap* keywords);

    AutoPtr<ISet> GetUnicodeLocaleAttributes();

    AutoPtr<ISet> GetUnicodeLocaleKeys();

    String GetUnicodeLocaleType(
        /* [in] */ const String& unicodeLocaleKey);

    static Boolean IsSingletonChar(
        /* [in] */ Char c);

    static Boolean IsAttribute(
        /* [in] */ const String& s);

    static Boolean IsKey(
        /* [in] */ const String& s);

    static Boolean IsTypeSubtag(
        /* [in] */ const String& s);

    ECode GetCoclassID(
        /* [out] */ CoclassID* cid);

private:
    UnicodeLocaleExtension(
        /* [in] */ const String& key,
        /* [in] */ const String& value);

public:
    static AutoPtr<UnicodeLocaleExtension> GetCA_JAPANESE();

    static AutoPtr<UnicodeLocaleExtension> GetNU_THAI();

public:
    static constexpr Char SINGLETON = U'u';

private:
    AutoPtr<ISet> mAttributes;
    AutoPtr<IMap> mKeywords;
};

inline Boolean UnicodeLocaleExtension::IsSingletonChar(
    /* [in] */ Char c)
{
    return (SINGLETON == LocaleUtils::ToLower(c));
}

inline Boolean UnicodeLocaleExtension::IsKey(
    /* [in] */ const String& s)
{
    // 2alphanum
    return (s.GetByteLength() == 2) && LocaleUtils::IsAlphaNumericString(s);
}

}
}
}

#endif // __COMO_UTIL_LOCALE_UNICODELOCALEEXTENSION_H__
