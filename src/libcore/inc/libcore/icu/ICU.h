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

#ifndef __LIBCORE_ICU_ICU_H__
#define __LIBCORE_ICU_ICU_H__

#include "ccm.util.ILocale.h"

using ccm::util::ILocale;

namespace libcore {
namespace icu {

class ICU
{
public:
    static Array<String> GetISOLanguages()
    {
        return Array<String>::Null();
    }

    static Array<String> GetISOCountries()
    {
        return Array<String>::Null();
    }

    static Array<ILocale*> GetAvailableLocales()
    {
        return Array<ILocale*>::Null();
    }

    static String GetISO3Country(
        /* [in] */ const String& languageTag)
    {
        return String();
    }

    static String GetISO3Language(
        /* [in] */ const String& languageTag)
    {
        return String();
    }

    static ECode SetDefaultLocale(
        /* [in] */ const String& languageTag)
    {
        return NOERROR;
    }

    static String GetDisplayLanguage(
        /* [in] */ ILocale* targetLocale,
        /* [in] */ ILocale* locale)
    {
        return String();
    }

private:
    ICU();
};

}
}

#endif // __LIBCORE_ICU_ICU_H__
