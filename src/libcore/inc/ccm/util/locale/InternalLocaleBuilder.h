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

#ifndef __CCM_UTIL_LOCALE_INTERNALLOCALEBUILDER_H__
#define __CCM_UTIL_LOCALE_INTERNALLOCALEBUILDER_H__

#include "ccm/core/SyncObject.h"
#include "ccm/util/locale/BaseLocale.h"
#include "ccm/util/locale/LocaleExtensions.h"
#include "ccm.util.locale.ILanguageTag.h"

using ccm::core::SyncObject;

namespace ccm {
namespace util {
namespace locale {

class InternalLocaleBuilder
    : public SyncObject
{
public:
    ECode SetLanguage(
        /* [in] */ const String& language)
    {
        return NOERROR;
    }

    ECode SetScript(
        /* [in] */ const String& script)
    {
        return NOERROR;
    }

    ECode SetRegion(
        /* [in] */ const String& region)
    {
        return NOERROR;
    }

    ECode SetVariant(
        /* [in] */ const String& variant)
    {
        return NOERROR;
    }

    ECode SetExtension(
        /* [in] */ Char singleton,
        /* [in] */ const String& value)
    {
        return NOERROR;
    }

    ECode SetLanguageTag(
        /* [in] */ ILanguageTag* langtag)
    {
        return NOERROR;
    }

    ECode Clear()
    {
        return NOERROR;
    }

    ECode ClearExtensions()
    {
        return NOERROR;
    }

    AutoPtr<BaseLocale> GetBaseLocale()
    {
        return nullptr;
    }

    AutoPtr<LocaleExtensions> GetLocaleExtensions()
    {
        return nullptr;
    }

    ECode SetLocale(
        /* [in] */ BaseLocale* base,
        /* [in] */ LocaleExtensions* localeExtensions)
    {
        return NOERROR;
    }

    ECode AddUnicodeLocaleAttribute(
        /* [in] */ const String& attribute)
    {
        return NOERROR;
    }

    ECode RemoveUnicodeLocaleAttribute(
        /* [in] */ const String& attribute)
    {
        return NOERROR;
    }

    ECode SetUnicodeLocaleKeyword(
        /* [in] */ const String& key,
        /* [in] */ const String& type)
    {
        return NOERROR;
    }
};

}
}
}


#endif // __CCM_UTIL_LOCALE_INTERNALLOCALEBUILDER_H__
