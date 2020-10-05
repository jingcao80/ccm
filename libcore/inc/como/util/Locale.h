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

#ifndef __COMO_UTIL_LOCALE_H__
#define __COMO_UTIL_LOCALE_H__

#include "coredef.h"
#include "como/core/SyncObject.h"
#include "como/util/locale/BaseLocale.h"
#include "como/util/locale/InternalLocaleBuilder.h"
#include "como/util/locale/LocaleExtensions.h"
#include "como/util/locale/LocaleObjectCache.h"
#include "como.core.ICloneable.h"
#include "como.io.ISerializable.h"
#include "como.util.ILocale.h"
#include "como.util.ILocaleBuilder.h"
#include "como.util.ILocaleCategory.h"
#include <comosp.h>
#include <comoobj.h>
#include <comoref.h>

using como::core::ICloneable;
using como::core::SyncObject;
using como::io::ISerializable;
using como::util::locale::BaseLocale;
using como::util::locale::InternalLocaleBuilder;
using como::util::locale::LocaleExtensions;
using como::util::locale::LocaleObjectCache;

namespace como {
namespace util {

class Locale
    : public SyncObject
    , public ILocale
    , public ISerializable
    , public ICloneable
{
public:
    class Category
        : public LightRefBase
        , public ILocaleCategory
    {
    public:
        COMO_INTERFACE_DECL();

        static AutoPtr<ILocaleCategory> GetDISPLAY();

        static AutoPtr<ILocaleCategory> GetFORMAT();

        inline Category(
            /* [in] */ const String& languageKey,
            /* [in] */ const String& scriptKey,
            /* [in] */ const String& countryKey,
            /* [in] */ const String& variantKey)
            : mLanguageKey(languageKey)
            , mScriptKey(scriptKey)
            , mCountryKey(countryKey)
            , mVariantKey(variantKey)
        {}

        ECode GetLanguageKey(
            /* [out] */ String& key) override;

        ECode GetScriptKey(
            /* [out] */ String& key) override;

        ECode GetCountryKey(
            /* [out] */ String& key) override;

        ECode GetVariantKey(
            /* [out] */ String& key) override;

    public:
        String mLanguageKey;
        String mScriptKey;
        String mCountryKey;
        String mVariantKey;
    };

    class Builder
        : public Object
        , public ILocaleBuilder
    {
    public:
        COMO_INTERFACE_DECL();

        ECode Constructor();

        ECode SetLocale(
            /* [in] */ ILocale* locale) override;

        ECode SetLanguageTag(
            /* [in] */ const String& languageTag) override;

        ECode SetLanguage(
            /* [in] */ const String& language) override;

        ECode SetScript(
            /* [in] */ const String& script) override;

        ECode SetRegion(
            /* [in] */ const String& region) override;

        ECode SetVariant(
            /* [in] */ const String& variant) override;

        ECode SetExtension(
            /* [in] */ Char key,
            /* [in] */ const String& value) override;

        ECode SetUnicodeLocaleKeyword(
            /* [in] */ const String& key,
            /* [in] */ const String& type) override;

        ECode AddUnicodeLocaleAttribute(
            /* [in] */ const String& attribute) override;

        ECode RemoveUnicodeLocaleAttribute(
            /* [in] */ const String& attribute) override;

        ECode Clear() override;

        ECode ClearExtensions() override;

        ECode Build(
            /* [out] */ AutoPtr<ILocale>& locale) override;

    private:
        AutoPtr<InternalLocaleBuilder> mLocaleBuilder;
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
            /* [out] */ Boolean& same) override;

        ECode GetHashCode(
            /* [out] */ Integer& hash) override;

    public:
        AutoPtr<BaseLocale> mBase;
        AutoPtr<LocaleExtensions> mExts;
        Integer mHash;
    };

public:
    COMO_INTERFACE_DECL();

    static AutoPtr<ILocale> GetENGLISH();

    static AutoPtr<ILocale> GetFRENCH();

    static AutoPtr<ILocale> GetGERMAN();

    static AutoPtr<ILocale> GetITALIAN();

    static AutoPtr<ILocale> GetJAPANESE();

    static AutoPtr<ILocale> GetKOREAN();

    static AutoPtr<ILocale> GetCHINESE();

    static AutoPtr<ILocale> GetSIMPLIFIED_CHINESE();

    static AutoPtr<ILocale> GetTRADITIONAL_CHINESE();

    static AutoPtr<ILocale> GetFRANCE();

    static AutoPtr<ILocale> GetGERMANY();

    static AutoPtr<ILocale> GetITALY();

    static AutoPtr<ILocale> GetJAPAN();

    static AutoPtr<ILocale> GetKOREA();

    static AutoPtr<ILocale> GetCHINA();

    static AutoPtr<ILocale> GetPRC();

    static AutoPtr<ILocale> GetTAIWAN();

    static AutoPtr<ILocale> GetUK();

    static AutoPtr<ILocale> GetUS();

    static AutoPtr<ILocale> GetCANADA();

    static AutoPtr<ILocale> GetCANADA_FRENCH();

    static AutoPtr<ILocale> GetROOT();

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
        /* [out] */ String& language) override;

    ECode GetScript(
        /* [out] */ String& script) override;

    ECode GetCountry(
        /* [out] */ String& country) override;

    ECode GetVariant(
        /* [out] */ String& variant) override;

    ECode HasExtensions(
        /* [out] */ Boolean* result);

    ECode GetExtension(
        /* [in] */ Char key,
        /* [out] */ String& extension) override;

    ECode GetExtensionKeys(
        /* [out] */ AutoPtr<ISet>& keys) override;

    ECode GetUnicodeLocaleAttributes(
        /* [out] */ AutoPtr<ISet>& attrs) override;

    ECode GetUnicodeLocaleType(
        /* [in] */ const String& key,
        /* [out] */ String& type) override;

    ECode GetUnicodeLocaleKeys(
        /* [out] */ AutoPtr<ISet>& keys) override;

    AutoPtr<BaseLocale> GetBaseLocale();

    AutoPtr<LocaleExtensions> GetLocaleExtensions();

    ECode ToString(
        /* [out] */ String& desc) override;

    ECode ToLanguageTag(
        /* [out] */ String& langTag) override;

    static AutoPtr<ILocale> ForLanguageTag(
        /* [in] */ const String& languageTag);

    ECode GetISO3Language(
        /* [out] */ String& language) override;

    ECode GetISO3Country(
        /* [out] */ String& country) override;

    ECode GetDisplayLanguage(
        /* [out] */ String& language) override;

    ECode GetDisplayLanguage(
        /* [in] */ ILocale* locale,
        /* [out] */ String& language) override;

    ECode GetDisplayScript(
        /* [out] */ String& script) override;

    ECode GetDisplayScript(
        /* [in] */ ILocale* inLocale,
        /* [out] */ String& script) override;

    ECode GetDisplayCountry(
        /* [out] */ String& country) override;

    ECode GetDisplayCountry(
        /* [in] */ ILocale* locale,
        /* [out] */ String& country) override;

    ECode GetDisplayVariant(
        /* [out] */ String& variant) override;

    ECode GetDisplayVariant(
        /* [in] */ ILocale* inLocale,
        /* [out] */ String& variant) override;

    ECode GetDisplayName(
        /* [out] */ String& name) override;

    ECode GetDisplayName(
        /* [in] */ ILocale* locale,
        /* [out] */ String& name) override;

    ECode GetHashCode(
        /* [out] */ Integer& hash) override;

    ECode Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean& same) override;

protected:
    ECode CloneImpl(
        /* [in] */ ILocale* newObj);

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
        /* [out] */ String* retLanguage);

    static Boolean IsAsciiAlphaNum(
        /* [in] */ const String& string);

    static ECode NormalizeAndValidateRegion(
        /* [in] */ const String& region,
        /* [in] */ Boolean strict,
        /* [out] */ String* retRegion);

    static Boolean IsValidBcp47Alpha(
        /* [in] */ const String& string,
        /* [in] */ Integer lowerBound,
        /* [in] */ Integer upperBound);

    static Boolean IsUnM49AreaCode(
        /* [in] */ const String& code);

    static ECode NormalizeAndValidateVariant(
        /* [in] */ const String& variant,
        /* [out] */ String* retVariant);

    static Boolean IsValidVariantSubtag(
        /* [in] */ const String& subTag);

    static Boolean IsUnicodeExtensionKey(
        /* [in] */ const String& s);

    static String ConvertOldISOCodes(
        /* [in] */ const String& language);

    static AutoPtr<LocaleExtensions> GetCompatibilityExtensions(
        /* [in] */ const String& language,
        /* [in] */ const String& script,
        /* [in] */ const String& country,
        /* [in] */ const String& variant);

private:
    static const String UNDETERMINED_LANGUAGE;

    AutoPtr<BaseLocale> mBaseLocale;
    AutoPtr<LocaleExtensions> mLocaleExtensions;

    VOLATILE Integer mHashCodeValue = 0;

    VOLATILE static AutoPtr<ILocale> sDefaultDisplayLocale;
    VOLATILE static AutoPtr<ILocale> sDefaultFormatLocale;

    VOLATILE String mLanguageTag;
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

#endif // __COMO_UTIL_LOCALE_H__
