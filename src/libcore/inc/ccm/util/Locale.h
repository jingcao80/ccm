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

#ifndef __CCM_UTIL_LOCALE_H__
#define __CCM_UTIL_LOCALE_H__

#include "ccm/core/SyncObject.h"
#include "ccm/util/locale/BaseLocale.h"
#include "ccm/util/locale/LocaleExtensions.h"
#include "ccm/util/locale/LocaleObjectCache.h"
#include "ccm.core.ICloneable.h"
#include "ccm.io.ISerializable.h"
#include "ccm.util.ILocale.h"
#include "ccm.util.ILocaleCategory.h"
#include <ccmautoptr.h>
#include <ccmobject.h>

using ccm::core::ICloneable;
using ccm::core::SyncObject;
using ccm::io::ISerializable;
using ccm::util::locale::BaseLocale;
using ccm::util::locale::LocaleExtensions;
using ccm::util::locale::LocaleObjectCache;

namespace ccm {
namespace util {

class Locale
    : public SyncObject
    , public ILocale
    , public ISerializable
    , public ICloneable
{
public:
    class Category
    {
    public:
        static AutoPtr<ILocaleCategory> GetDISPLAY();
    };

private:
    class Cache
        : public LocaleObjectCache
    {
    protected:
        AutoPtr<IInterface> CreateObject(
            /* [in] */ IInterface* key) override;
    };

    class LocaleKey
        : public Object
    {
    public:
        LocaleKey(
            /* [in] */ BaseLocale* baseLocale,
            /* [in] */ LocaleExtensions* extensions);

        IInterface* Probe(
            /* [in] */ const InterfaceID& iid) override;

        ECode Equals(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* same) override;

        ECode GetHashCode(
            /* [out] */ Integer* hash) override;

    public:
        AutoPtr<BaseLocale> mBase;
        AutoPtr<LocaleExtensions> mExts;
        Integer mHash;
    };

public:
    CCM_INTERFACE_DECL();

    static AutoPtr<ILocale> GetUS()
    {
        return nullptr;
    }

    ECode Constructor(
        /* [in] */ BaseLocale* baseLocale,
        /* [in] */ LocaleExtensions* extensions);

    ECode Constructor(
        /* [in] */ const String& language,
        /* [in] */ const String& country,
        /* [in] */ const String& variant);

    ECode Constructor(
        /* [in] */ const String& language,
        /* [in] */ const String& country);

    ECode Constructor(
        /* [in] */ const String& language);

    static AutoPtr<ILocale> GetInstance(
        /* [in] */ const String& language,
        /* [in] */ const String& country,
        /* [in] */ const String& variant);

    static AutoPtr<ILocale> GetInstance(
        /* [in] */ const String& language,
        /* [in] */ const String& script,
        /* [in] */ const String& country,
        /* [in] */ const String& variant,
        /* [in] */ LocaleExtensions* extensions);

    static AutoPtr<ILocale> GetInstance(
        /* [in] */ BaseLocale* baseloc,
        /* [in] */ LocaleExtensions* extensions);

    static AutoPtr<ILocale> GetDefault();

    static AutoPtr<ILocale> GetDefault(
        /* [in] */ ILocaleCategory* category);

    static AutoPtr<ILocale> InitDefault();

    static ECode SetDefault(
        /* [in] */ ILocale* newLocale);

    static ECode SetDefault(
        /* [in] */ ILocaleCategory* category,
        /* [in] */ ILocale* newLocale);

    static Array<ILocale*> GetAvailableLocales();

    static Array<String> GetISOCountries();

    static Array<String> GetISOLanguages();

    ECode GetLanguage(
        /* [out] */ String* language) override;

    ECode GetScript(
        /* [out] */ String* script) override;

    ECode GetCountry(
        /* [out] */ String* country) override;

    ECode GetVariant(
        /* [out] */ String* variant) override;

    ECode HasExtensions(
        /* [out] */ Boolean* result);

    ECode GetExtension(
        /* [in] */ Char key,
        /* [out] */ String* extension) override;

    ECode GetExtensionKeys(
        /* [out] */ ISet** keys) override;

    ECode GetUnicodeLocaleAttributes(
        /* [out] */ ISet** attrs) override;

    ECode GetUnicodeLocaleType(
        /* [in] */ const String& key,
        /* [out] */ String* type) override;

    ECode GetUnicodeLocaleKeys(
        /* [out] */ ISet** keys) override;

    AutoPtr<BaseLocale> GetBaseLocale();

    AutoPtr<LocaleExtensions> GetLocaleExtensions();

    ECode ToString(
        /* [out] */ String* desc) override;

    ECode ToLanguageTag(
        /* [out] */ String* langTag) override;

    static AutoPtr<ILocale> ForLanguageTag(
        /* [in] */ const String& languageTag);

    ECode GetISO3Language(
        /* [out] */ String* language) override;

    ECode GetISO3Country(
        /* [out] */ String* country) override;

    ECode GetDisplayLanguage(
        /* [out] */ String* language) override;

    ECode GetDisplayLanguage(
        /* [in] */ ILocale* locale,
        /* [out] */ String* language) override;




    static AutoPtr<ILocaleCategory> GetDisplayCategory()
    {
        return nullptr;
    }

    static AutoPtr<ILocaleCategory> GetFormatCategory()
    {
        return nullptr;
    }











protected:
    ECode CloneImpl(
        /* [in] */ ILocale* newObj)
    {
        return NOERROR;
    }

private:
    static SyncObject& GetClassLock();

    static AutoPtr<Cache> GetLOCALECACHE();

    static AutoPtr<ILocale> GetOrSetDefaultLocale(
        /* [in] */ ILocale* locale);

    static AutoPtr<ILocale> CreateConstant(
        /* [in] */ const String& lang,
        /* [in] */ const String& country);

    static AutoPtr<ILocale> InitDefault(
        /* [in] */ ILocaleCategory* category);

    static ECode NormalizeAndValidateLanguage(
        /* [in] */ const String& language,
        /* [in] */ Boolean strict,
        /* [out] */ String* retLanguage)
    {
        return NOERROR;
    }



    static Boolean IsUnicodeExtensionKey(
        /* [in] */ const String& s)
    {
        return false;
    }

    static String ConvertOldISOCodes(
        /* [in] */ const String& language)
    {
        return String();
    }

    static AutoPtr<LocaleExtensions> GetCompatibilityExtensions(
        /* [in] */ const String& language,
        /* [in] */ const String& script,
        /* [in] */ const String& country,
        /* [in] */ const String& variant)
    {
        return nullptr;
    }

private:
    static const String UNDETERMINED_LANGUAGE;

    AutoPtr<BaseLocale> mBaseLocale;
    AutoPtr<LocaleExtensions> mLocaleExtensions;

    static AutoPtr<ILocale> sDefaultDisplayLocale;
    static AutoPtr<ILocale> sDefaultFormatLocale;

    String mLanguageTag;
};

inline AutoPtr<BaseLocale> Locale::GetBaseLocale()
{
    return mBaseLocale;
}

inline AutoPtr<LocaleExtensions> Locale::GetLocaleExtensions()
{
    return mLocaleExtensions;
}

}
}

#endif // __CCM_UTIL_LOCALE_H__
