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

#ifndef __CCM_UTIL_LOCALE_UNICODELOCALEEXTENSION_H__
#define __CCM_UTIL_LOCALE_UNICODELOCALEEXTENSION_H__

#include "ccm/util/locale/Extension.h"
#include "ccm/util/locale/LocaleUtils.h"
#include "ccm.util.IMap.h"
#include "ccm.util.ISet.h"
#include "ccm.util.ISortedMap.h"
#include "ccm.util.ISortedSet.h"
#include <ccmautoptr.h>

namespace ccm {
namespace util {
namespace locale {

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

private:
    UnicodeLocaleExtension(
        /* [in] */ const String& key,
        /* [in] */ const String& value);

public:
    static AutoPtr<UnicodeLocaleExtension> GetCA_JAPANESE();

    static AutoPtr<UnicodeLocaleExtension> GetNU_THAI();

public:
    static constexpr Char SINGLETON = 'u';

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

#endif // __CCM_UTIL_LOCALE_UNICODELOCALEEXTENSION_H__
