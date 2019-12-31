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

#ifndef __CCM_UTIL_LOCALE_INTERNALLOCALEBUILDER_H__
#define __CCM_UTIL_LOCALE_INTERNALLOCALEBUILDER_H__

#include "ccm/core/SyncObject.h"
#include "ccm/util/locale/BaseLocale.h"
#include "ccm/util/locale/LocaleExtensions.h"
#include "ccm.util.IList.h"
#include "ccm.util.ISet.h"
#include "ccm.util.locale.ILanguageTag.h"

using ccm::core::SyncObject;
using ccm::util::IList;

namespace ccm {
namespace util {
namespace locale {

class InternalLocaleBuilder
    : public SyncObject
{
public:
    class CaseInsensitiveString
        : public Object
    {
    public:
        CaseInsensitiveString(
            /* [in] */ const String& s);

        inline String Value()
        {
            return mStr;
        }

        ECode GetHashCode(
            /* [out] */ Integer* hash) override;

        ECode Equals(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* same) override;

        ECode GetCoclassID(
            /* [out] */ CoclassID* cid) override;

    private:
        String mStr;
        String mLowerStr;
    };

    class CaseInsensitiveChar
        : public Object
    {
    public:
        CaseInsensitiveChar(
            /* [in] */ const String& s);

        CaseInsensitiveChar(
            /* [in] */ Char c);

        inline Char Value()
        {
            return mCh;
        }

        ECode GetHashCode(
            /* [out] */ Integer* hash) override;

        ECode Equals(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* same) override;

        ECode GetCoclassID(
            /* [out] */ CoclassID* cid) override;

    private:
        Char mCh;
        Char mLowerCh;
    };

public:
    InternalLocaleBuilder();

    ECode SetLanguage(
        /* [in] */ const String& language);

    ECode SetScript(
        /* [in] */ const String& script);

    ECode SetRegion(
        /* [in] */ const String& region);

    ECode SetVariant(
        /* [in] */ const String& variant);

    ECode AddUnicodeLocaleAttribute(
        /* [in] */ const String& attribute);

    ECode RemoveUnicodeLocaleAttribute(
        /* [in] */ const String& attribute);

    ECode SetUnicodeLocaleKeyword(
        /* [in] */ const String& key,
        /* [in] */ const String& type);

    ECode SetExtension(
        /* [in] */ Char singleton,
        /* [in] */ const String& value);

    ECode SetExtensions(
        /* [in] */ const String& subtags);

    ECode SetLanguageTag(
        /* [in] */ ILanguageTag* langtag);

    ECode SetLocale(
        /* [in] */ BaseLocale* base,
        /* [in] */ LocaleExtensions* localeExtensions);

    ECode Clear();

    ECode ClearExtensions();

    AutoPtr<BaseLocale> GetBaseLocale();

    AutoPtr<LocaleExtensions> GetLocaleExtensions();

    static String RemovePrivateuseVariant(
        /* [in] */ const String& privuseVal);

private:
    ECode SetExtensions(
        /* [in] */ IList* bcpExtensions,
        /* [in] */ const String& privateuse);

    Integer CheckVariants(
        /* [in] */ const String& variants,
        /* [in] */ const String& sep);

    void SetUnicodeLocaleExtension(
        /* [in] */ const String& subtags);

    static AutoPtr<CaseInsensitiveChar> GetPRIVATEUSE_KEY();

private:
    String mLanguage;
    String mScript;
    String mRegion;
    String mVariant;

    AutoPtr<IMap> mExtensions;
    AutoPtr<ISet> mUattributes;
    AutoPtr<IMap> mUkeywords;
};

}
}
}

#endif // __CCM_UTIL_LOCALE_INTERNALLOCALEBUILDER_H__
