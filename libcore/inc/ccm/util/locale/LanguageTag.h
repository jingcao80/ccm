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

#ifndef __CCM_UTIL_LOCALE_LANGUAGETAG_H__
#define __CCM_UTIL_LOCALE_LANGUAGETAG_H__

#include "ccm/util/Collections.h"
#include "ccm/util/locale/BaseLocale.h"
#include "ccm/util/locale/LocaleExtensions.h"
#include "ccm/util/locale/LocaleUtils.h"
#include "ccm/util/locale/ParseStatus.h"
#include "ccm/util/locale/StringTokenIterator.h"
#include "ccm.util.locale.ILanguageTag.h"
#include <ccmautoptr.h>

namespace ccm {
namespace util {
namespace locale {

class LanguageTag
    : public SyncObject
    , public ILanguageTag
{
public:
    CCM_INTERFACE_DECL();

    static AutoPtr<ILanguageTag> Parse(
        /* [in] */ const String& languageTag,
        /* [in] */ ParseStatus* sts);

    static AutoPtr<ILanguageTag> ParseLocale(
        /* [in] */ BaseLocale* baseLocale,
        /* [in] */ LocaleExtensions* localeExtensions);

    ECode GetLanguage(
        /* [out] */ String* language) override;

    ECode GetExtlangs(
        /* [out] */ IList** extlangs) override;

    ECode GetScript(
        /* [out] */ String* script) override;

    ECode GetRegion(
        /* [out] */ String* region) override;

    ECode GetVariants(
        /* [out] */ IList** variants) override;

    ECode GetExtensions(
        /* [out] */ IList** extensions) override;

    ECode GetPrivateuse(
        /* [out] */ String* privateuse) override;

    static Boolean IsLanguage(
        /* [in] */ const String& s);

    static Boolean IsExtlang(
        /* [in] */ const String& s);

    static Boolean IsScript(
        /* [in] */ const String& s);

    static Boolean IsRegion(
        /* [in] */ const String& s);

    static Boolean IsVariant(
        /* [in] */ const String& s);

    static Boolean IsExtensionSingleton(
        /* [in] */ const String& s);

    static Boolean IsExtensionSingletonChar(
        /* [in] */ Char c);

    static Boolean IsExtensionSubtag(
        /* [in] */ const String& s);

    static Boolean IsPrivateusePrefix(
        /* [in] */ const String& s);

    static Boolean IsPrivateusePrefixChar(
        /* [in] */ Char c);

    static Boolean IsPrivateuseSubtag(
        /* [in] */ const String& s);

    static String CanonicalizeLanguage(
        /* [in] */ const String& s);

    static String CanonicalizeExtlang(
        /* [in] */ const String& s);

    static String CanonicalizeScript(
        /* [in] */ const String& s);

    static String CanonicalizeRegion(
        /* [in] */ const String& s);

    static String CanonicalizeVariant(
        /* [in] */ const String& s);

    static String CanonicalizeExtension(
        /* [in] */ const String& s);

    static String CanonicalizeExtensionSingleton(
        /* [in] */ const String& s);

    static String CanonicalizeExtensionSubtag(
        /* [in] */ const String& s);

    static String CanonicalizePrivateuse(
        /* [in] */ const String& s);

    static String CanonicalizePrivateuseSubtag(
        /* [in] */ const String& s);

    ECode ToString(
        /* [out] */ String* desc) override;

private:
    static AutoPtr<IMap> GetGRANDFATHERED();

    Boolean ParseLanguage(
        /* [in] */ StringTokenIterator* itr,
        /* [in] */ ParseStatus* sts);

    Boolean ParseExtlangs(
        /* [in] */ StringTokenIterator* itr,
        /* [in] */ ParseStatus* sts);

    Boolean ParseScript(
        /* [in] */ StringTokenIterator* itr,
        /* [in] */ ParseStatus* sts);

    Boolean ParseRegion(
        /* [in] */ StringTokenIterator* itr,
        /* [in] */ ParseStatus* sts);

    Boolean ParseVariants(
        /* [in] */ StringTokenIterator* itr,
        /* [in] */ ParseStatus* sts);

    Boolean ParseExtensions(
        /* [in] */ StringTokenIterator* itr,
        /* [in] */ ParseStatus* sts);

    Boolean ParsePrivateuse(
        /* [in] */ StringTokenIterator* itr,
        /* [in] */ ParseStatus* sts);

private:
    String mLanguage = String("");
    String mScript = String("");
    String mRegion = String("");
    String mPrivateuse = String("");

    AutoPtr<IList> mExtlangs = Collections::GetEmptyList();
    AutoPtr<IList> mVariants = Collections::GetEmptyList();
    AutoPtr<IList> mExtensions = Collections::GetEmptyList();
};

inline String LanguageTag::CanonicalizeLanguage(
    /* [in] */ const String& s)
{
    return LocaleUtils::ToLowerString(s);
}

inline String LanguageTag::CanonicalizeExtlang(
    /* [in] */ const String& s)
{
    return LocaleUtils::ToLowerString(s);
}

inline String LanguageTag::CanonicalizeScript(
    /* [in] */ const String& s)
{
    return LocaleUtils::ToTitleString(s);
}

inline String LanguageTag::CanonicalizeRegion(
    /* [in] */ const String& s)
{
    return LocaleUtils::ToUpperString(s);
}

inline String LanguageTag::CanonicalizeVariant(
    /* [in] */ const String& s)
{
    return LocaleUtils::ToLowerString(s);
}

inline String LanguageTag::CanonicalizeExtension(
    /* [in] */ const String& s)
{
    return LocaleUtils::ToLowerString(s);
}

inline String LanguageTag::CanonicalizeExtensionSingleton(
    /* [in] */ const String& s)
{
    return LocaleUtils::ToLowerString(s);
}

inline String LanguageTag::CanonicalizeExtensionSubtag(
    /* [in] */ const String& s)
{
    return LocaleUtils::ToLowerString(s);
}

inline String LanguageTag::CanonicalizePrivateuse(
    /* [in] */ const String& s)
{
    return LocaleUtils::ToLowerString(s);
}

inline String LanguageTag::CanonicalizePrivateuseSubtag(
    /* [in] */ const String& s)
{
    return LocaleUtils::ToLowerString(s);
}

}
}
}

#endif // __CCM_UTIL_LOCALE_LANGUAGETAG_H__
