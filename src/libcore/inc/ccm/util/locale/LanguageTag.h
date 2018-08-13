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

#ifndef __CCM_UTIL_LOCALE_LANGUAGETAG_H__
#define __CCM_UTIL_LOCALE_LANGUAGETAG_H__

#include "ccm/util/Collections.h"
#include "ccm/util/locale/BaseLocale.h"
#include "ccm/util/locale/LocaleExtensions.h"
#include "ccm/util/locale/ParseStatus.h"
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
        /* [in] */ ParseStatus* sts)
    {
        return nullptr;
    }

    static AutoPtr<ILanguageTag> ParseLocale(
        /* [in] */ BaseLocale* baseLocale,
        /* [in] */ LocaleExtensions* localeExtensions)
    {
        return nullptr;
    }

    static String CanonicalizeLanguage(
        /* [in] */ const String& s)
    {
        return String();
    }

    static String CanonicalizeScript(
        /* [in] */ const String& s)
    {
        return String();
    }

    static String CanonicalizeRegion(
        /* [in] */ const String& s)
    {
        return String();
    }

    static String CanonicalizeExtension(
        /* [in] */ const String& s)
    {
        return String();
    }

private:
    static AutoPtr<IMap> GetGRANDFATHERED();

private:
    String mLanguage = String("");
    String mScript = String("");
    String mRegion = String("");
    String mPrivateuse = String("");

    AutoPtr<IList> mExtlangs = Collections::GetEmptyList();
    AutoPtr<IList> mVariants = Collections::GetEmptyList();
    AutoPtr<IList> mExtensions = Collections::GetEmptyList();
};

}
}
}

#endif // __CCM_UTIL_LOCALE_LANGUAGETAG_H__
