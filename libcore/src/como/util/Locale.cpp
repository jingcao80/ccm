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

#include "como/core/AutoLock.h"
#include "como/core/CoreUtils.h"
#include "como/core/CStringBuilder.h"
#include "como/core/NativeAtomic.h"
#include "como/core/StringUtils.h"
#include "como/core/System.h"
#include "como/util/CLocale.h"
#include "como/util/Collections.h"
#include "como/util/CPropertyPermission.h"
#include "como/util/Locale.h"
#include "como/util/locale/LanguageTag.h"
#include "como/util/locale/LocaleUtils.h"
#include "como/util/locale/ParseStatus.h"
#include "libcore/icu/ICU.h"
#include "como.core.ICharSequence.h"
#include "como.core.IStringBuilder.h"
#include "como.core.ISecurityManager.h"
#include "como.security.IPermission.h"
#include "como.util.locale.ILanguageTag.h"
#include <comolog.h>

using como::core::AutoLock;
using como::core::CoreUtils;
using como::core::CStringBuilder;
using como::core::ICharSequence;
using como::core::ISecurityManager;
using como::core::IStringBuilder;
using como::core::IID_IStringBuilder;
using como::core::IID_ICloneable;
using como::core::System;
using como::core::StringUtils;
using como::io::IID_ISerializable;
using como::security::IPermission;
using como::security::IID_IPermission;
using como::util::locale::ILanguageTag;
using como::util::locale::LanguageTag;
using como::util::locale::LocaleUtils;
using como::util::locale::ParseStatus;
using libcore::icu::ICU;

namespace como {
namespace util {

INIT_PROI_3 const String Locale::UNDETERMINED_LANGUAGE("und");
INIT_PROI_3 AutoPtr<ILocale> Locale::sDefaultDisplayLocale;
INIT_PROI_3 AutoPtr<ILocale> Locale::sDefaultFormatLocale;

COMO_INTERFACE_IMPL_3(Locale, SyncObject, ILocale, ISerializable, ICloneable);

SyncObject& Locale::GetClassLock()
{
    static SyncObject sLock;
    return sLock;
}

AutoPtr<Locale::Cache> Locale::GetLOCALECACHE()
{
    static AutoPtr<Cache> LOCALECACHE = new Cache();
    return LOCALECACHE;
}

AutoPtr<ILocale> Locale::GetENGLISH()
{
    static AutoPtr<ILocale> ENGLISH = CreateConstant(String("en"), String(""));
    return ENGLISH;
}

AutoPtr<ILocale> Locale::GetFRENCH()
{
    static AutoPtr<ILocale> FRENCH = CreateConstant(String("fr"), String(""));
    return FRENCH;
}

AutoPtr<ILocale> Locale::GetGERMAN()
{
    static AutoPtr<ILocale> GERMAN = CreateConstant(String("de"), String(""));
    return GERMAN;
}

AutoPtr<ILocale> Locale::GetITALIAN()
{
    static AutoPtr<ILocale> ITALIAN = CreateConstant(String("it"), String(""));
    return ITALIAN;
}

AutoPtr<ILocale> Locale::GetJAPANESE()
{
    static AutoPtr<ILocale> JAPANESE = CreateConstant(String("ja"), String(""));
    return JAPANESE;
}

AutoPtr<ILocale> Locale::GetKOREAN()
{
    static AutoPtr<ILocale> KOREAN = CreateConstant(String("ko"), String(""));
    return KOREAN;
}

AutoPtr<ILocale> Locale::GetCHINESE()
{
    static AutoPtr<ILocale> CHINESE = CreateConstant(String("zh"), String(""));
    return CHINESE;
}

AutoPtr<ILocale> Locale::GetSIMPLIFIED_CHINESE()
{
    static AutoPtr<ILocale> SIMPLIFIED_CHINESE = CreateConstant(String("zh"), String("CN"));
    return SIMPLIFIED_CHINESE;
}

AutoPtr<ILocale> Locale::GetTRADITIONAL_CHINESE()
{
    static AutoPtr<ILocale> TRADITIONAL_CHINESE = CreateConstant(String("zh"), String("TW"));
    return TRADITIONAL_CHINESE;
}

AutoPtr<ILocale> Locale::GetFRANCE()
{
    static AutoPtr<ILocale> FRANCE = CreateConstant(String("fr"), String("FR"));
    return FRANCE;
}

AutoPtr<ILocale> Locale::GetGERMANY()
{
    static AutoPtr<ILocale> GERMANY = CreateConstant(String("de"), String("DE"));
    return GERMANY;
}

AutoPtr<ILocale> Locale::GetITALY()
{
    static AutoPtr<ILocale> ITALY = CreateConstant(String("it"), String("IT"));
    return ITALY;
}

AutoPtr<ILocale> Locale::GetJAPAN()
{
    static AutoPtr<ILocale> JAPAN = CreateConstant(String("ja"), String("JP"));
    return JAPAN;
}

AutoPtr<ILocale> Locale::GetKOREA()
{
    static AutoPtr<ILocale> KOREA = CreateConstant(String("ko"), String("KR"));
    return KOREA;
}

AutoPtr<ILocale> Locale::GetCHINA()
{
    return GetSIMPLIFIED_CHINESE();
}

AutoPtr<ILocale> Locale::GetPRC()
{
    return GetSIMPLIFIED_CHINESE();
}

AutoPtr<ILocale> Locale::GetTAIWAN()
{
    return GetTRADITIONAL_CHINESE();
}

AutoPtr<ILocale> Locale::GetUK()
{
    static AutoPtr<ILocale> UK = CreateConstant(String("en"), String("GB"));
    return UK;
}

AutoPtr<ILocale> Locale::GetUS()
{
    static AutoPtr<ILocale> US = CreateConstant(String("en"), String("US"));
    return US;
}

AutoPtr<ILocale> Locale::GetCANADA()
{
    static AutoPtr<ILocale> CANADA = CreateConstant(String("en"), String("CA"));
    return CANADA;
}

AutoPtr<ILocale> Locale::GetCANADA_FRENCH()
{
    static AutoPtr<ILocale> CANADA_FRENCH = CreateConstant(String("fr"), String("CA"));
    return CANADA_FRENCH;
}

AutoPtr<ILocale> Locale::GetROOT()
{
    static AutoPtr<ILocale> ROOT = CreateConstant(String(""), String(""));
    return ROOT;
}

AutoPtr<ILocale> Locale::GetOrSetDefaultLocale(
    /* [in] */ ILocale* locale)
{
    static AutoPtr<ILocale> sDefaultLocale = InitDefault();
    if (locale != nullptr) {
        VOLATILE_SET(sDefaultLocale, locale);
    }
    VOLATILE_GET(AutoPtr<ILocale> l, sDefaultLocale);
    return l;
}

ECode Locale::Constructor(
    /* [in] */ BaseLocale* baseLocale,
    /* [in] */ LocaleExtensions* extensions)
{
    mBaseLocale = baseLocale;
    mLocaleExtensions = extensions;
    return NOERROR;
}

ECode Locale::Constructor(
    /* [in] */ const String& language,
    /* [in] */ const String& country,
    /* [in] */ const String& variant)
{
    if (language.IsNull() || country.IsNull() || variant.IsNull()) {
        return como::core::E_NULL_POINTER_EXCEPTION;
    }
    mBaseLocale = BaseLocale::GetInstance(ConvertOldISOCodes(language), String(""), country, variant);
    mLocaleExtensions = GetCompatibilityExtensions(language, String(""), country, variant);
    return NOERROR;
}

ECode Locale::Constructor(
    /* [in] */ const String& language,
    /* [in] */ const String& country)
{
    return Constructor(language, country, String(""));
}

ECode Locale::Constructor(
    /* [in] */ const String& language)
{
    return Constructor(language, String(""), String(""));
}

AutoPtr<ILocale> Locale::CreateConstant(
    /* [in] */ const String& lang,
    /* [in] */ const String& country)
{
    AutoPtr<BaseLocale> base = BaseLocale::CreateInstance(lang, country);
    return GetInstance(base, nullptr);
}

AutoPtr<ILocale> Locale::GetInstance(
    /* [in] */ const String& language,
    /* [in] */ const String& country,
    /* [in] */ const String& variant)
{
    return GetInstance(language, String(""), country, variant, nullptr);
}

AutoPtr<ILocale> Locale::GetInstance(
    /* [in] */ const String& language,
    /* [in] */ const String& script,
    /* [in] */ const String& country,
    /* [in] */ const String& variant,
    /* [in] */ LocaleExtensions* extensions)
{
    if (language.IsNull() || script.IsNull() || country.IsNull() || variant.IsNull()) {
        CHECK(0);
        return nullptr;
    }

    if (extensions == nullptr) {
        extensions = GetCompatibilityExtensions(language, script, country, variant);
    }

    AutoPtr<BaseLocale> baseloc = BaseLocale::GetInstance(language, script, country, variant);
    return GetInstance(baseloc, extensions);
}

AutoPtr<ILocale> Locale::GetInstance(
    /* [in] */ BaseLocale* baseloc,
    /* [in] */ LocaleExtensions* extensions)
{
    AutoPtr<LocaleKey> key = new LocaleKey(baseloc, extensions);
    AutoPtr<IInterface> value;
    GetLOCALECACHE()->Get((IObject*)key.Get(), &value);
    return ILocale::Probe(value);
}

AutoPtr<ILocale> Locale::GetDefault()
{
    return GetOrSetDefaultLocale(nullptr);
}

AutoPtr<ILocale> Locale::GetDefault(
    /* [in] */ ILocaleCategory* category)
{
    if (category == Category::GetDISPLAY()) {
        VOLATILE_GET(AutoPtr<ILocale> l, sDefaultDisplayLocale);
        if (l == nullptr) {
            AutoLock lock(GetClassLock());
            VOLATILE_GET(l, sDefaultDisplayLocale);
            if (l == nullptr) {
                l = InitDefault(category);
                VOLATILE_SET(sDefaultDisplayLocale, l);
            }
        }
        return l;
    }
    if (category == Category::GetFORMAT()) {
        VOLATILE_GET(AutoPtr<ILocale> l, sDefaultFormatLocale);
        if (l == nullptr) {
            AutoLock lock(GetClassLock());
            VOLATILE_GET(l, sDefaultFormatLocale);
            if (l == nullptr) {
                l = InitDefault(category);
                VOLATILE_SET(sDefaultFormatLocale, l);
            }
        }
        return l;
    }
    return GetDefault();
}

AutoPtr<ILocale> Locale::InitDefault()
{
    // user.locale gets priority
    String languageTag;
    System::GetProperty(String("user.locale"), String(""), &languageTag);
    if (!languageTag.IsEmpty()) {
        return ForLanguageTag(languageTag);
    }

    // user.locale is empty
    String language, region, script, country, variant;
    System::GetProperty(String("user.language"), String("en"), &language);
    // for compatibility, check for old user.region property
    System::GetProperty(String("user.region"), &region);
    if (!region.IsNull()) {
        // region can be of form country, country_variant, or _variant
        Integer i = region.IndexOf(U'_');
        if (i >= 0) {
            country = region.Substring(0, i);
            variant = region.Substring(i + 1);
        }
        else {
            country = region;
            variant = "";
        }
        script = "";
    }
    else {
        System::GetProperty(String("user.script"), String(""), &script);
        System::GetProperty(String("user.country"), String(""), &country);
        System::GetProperty(String("user.variant"), String(""), &variant);
    }

    return GetInstance(language, script, country, variant, nullptr);
}

AutoPtr<ILocale> Locale::InitDefault(
    /* [in] */ ILocaleCategory* category)
{
    AutoPtr<ILocale> defaultLocale = GetOrSetDefaultLocale(nullptr);

    String languageKey, scriptKey, countryKey, variantKey;
    category->GetLanguageKey(&languageKey);
    category->GetScriptKey(&scriptKey);
    category->GetCountryKey(&countryKey);
    category->GetVariantKey(&variantKey);
    String defaultLanguage, defaultScript, defaultCountry, defaultVariant;
    defaultLocale->GetLanguage(&defaultLanguage);
    defaultLocale->GetScript(&defaultScript);
    defaultLocale->GetCountry(&defaultCountry);
    defaultLocale->GetVariant(&defaultVariant);
    String language, script, country, variant;
    System::GetProperty(languageKey, defaultLanguage, &language);
    System::GetProperty(scriptKey, defaultScript, &script);
    System::GetProperty(countryKey, defaultCountry, &country);
    System::GetProperty(variantKey, defaultVariant, &variant);
    return GetInstance(language, script, country, variant, nullptr);
}

ECode Locale::SetDefault(
    /* [in] */ ILocale* newLocale)
{
    AutoLock lock(GetClassLock());

    FAIL_RETURN(SetDefault(Category::GetDISPLAY(), newLocale));
    FAIL_RETURN(SetDefault(Category::GetFORMAT(), newLocale));
    GetOrSetDefaultLocale(newLocale);
    String languageTag;
    newLocale->ToLanguageTag(&languageTag);
    return ICU::SetDefaultLocale(languageTag);
}

ECode Locale::SetDefault(
    /* [in] */ ILocaleCategory* category,
    /* [in] */ ILocale* newLocale)
{
    if (category == nullptr) {
        Logger::E("Locale", "Category cannot be NULL");
        return como::core::E_NULL_POINTER_EXCEPTION;
    }
    if (newLocale == nullptr) {
        Logger::E("Locale", "Can't set default locale to NULL");
        return como::core::E_NULL_POINTER_EXCEPTION;
    }

    AutoLock lock(GetClassLock());

    AutoPtr<ISecurityManager> sm = System::GetSecurityManager();
    if (sm != nullptr) {
        AutoPtr<IPermission> pp;
        CPropertyPermission::New(String("user.language"), String("write"),
                IID_IPermission, (IInterface**)&pp);
        FAIL_RETURN(sm->CheckPermission(pp));
    }
    if (category == Category::GetDISPLAY()) {
        VOLATILE_SET(sDefaultDisplayLocale, newLocale);
        return NOERROR;
    }
    if (category == Category::GetFORMAT()) {
        VOLATILE_SET(sDefaultFormatLocale, newLocale);
        return NOERROR;
    }
    Logger::E("Locale", "Unknown Category");
    CHECK(0);
    return NOERROR;
}

Array<ILocale*> Locale::GetAvailableLocales()
{
    return ICU::GetAvailableLocales();
}

Array<String> Locale::GetISOCountries()
{
    return ICU::GetISOCountries();
}

Array<String> Locale::GetISOLanguages()
{
    return ICU::GetISOLanguages();
}

ECode Locale::GetLanguage(
    /* [out] */ String* language)
{
    VALIDATE_NOT_NULL(language);

    *language = mBaseLocale->GetLanguage();
    return NOERROR;
}

ECode Locale::GetScript(
    /* [out] */ String* script)
{
    VALIDATE_NOT_NULL(script);

    *script = mBaseLocale->GetScript();
    return NOERROR;
}

ECode Locale::GetCountry(
    /* [out] */ String* country)
{
    VALIDATE_NOT_NULL(country);

    *country = mBaseLocale->GetRegion();
    return NOERROR;
}

ECode Locale::GetVariant(
    /* [out] */ String* variant)
{
    VALIDATE_NOT_NULL(variant);

    *variant = mBaseLocale->GetVariant();
    return NOERROR;
}

ECode Locale::HasExtensions(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mLocaleExtensions != nullptr;
    return NOERROR;
}

ECode Locale::GetExtension(
    /* [in] */ Char key,
    /* [out] */ String* extension)
{
    VALIDATE_NOT_NULL(extension);

    if (!LocaleExtensions::IsValidKey(key)) {
        Logger::E("Locale", "Ill-formed extension key: %c", key);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Boolean hasExt;
    HasExtensions(&hasExt);
    *extension = hasExt ? mLocaleExtensions->GetExtensionValue(key) : String();
    return NOERROR;
}

ECode Locale::GetExtensionKeys(
    /* [out] */ ISet** keys)
{
    VALIDATE_NOT_NULL(keys);

    Boolean hasExt;
    if (HasExtensions(&hasExt), !hasExt) {
        Collections::GetEmptySet().MoveTo(keys);
        return NOERROR;
    }
    mLocaleExtensions->GetKeys().MoveTo(keys);
    return NOERROR;
}

ECode Locale::GetUnicodeLocaleAttributes(
    /* [out] */ ISet** attrs)
{
    VALIDATE_NOT_NULL(attrs);

    Boolean hasExt;
    if (HasExtensions(&hasExt), !hasExt) {
        Collections::GetEmptySet().MoveTo(attrs);
        return NOERROR;
    }
    mLocaleExtensions->GetUnicodeLocaleAttributes().MoveTo(attrs);
    return NOERROR;
}

ECode Locale::GetUnicodeLocaleType(
    /* [in] */ const String& key,
    /* [out] */ String* type)
{
    VALIDATE_NOT_NULL(type);

    if (!IsUnicodeExtensionKey(key)) {
        Logger::E("Locale", "Ill-formed Unicode locale key: %s", key.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Boolean hasExt;
    HasExtensions(&hasExt);
    *type = hasExt ? mLocaleExtensions->GetUnicodeLocaleType(key) : String();
    return NOERROR;
}

ECode Locale::GetUnicodeLocaleKeys(
    /* [out] */ ISet** keys)
{
    VALIDATE_NOT_NULL(keys);

    if (mLocaleExtensions == nullptr) {
        Collections::GetEmptySet().MoveTo(keys);
        return NOERROR;
    }
    mLocaleExtensions->GetUnicodeLocaleKeys().MoveTo(keys);
    return NOERROR;
}

ECode Locale::ToString(
    /* [out] */ String& desc)
{
    Boolean l = (mBaseLocale->GetLanguage().GetLength() != 0);
    Boolean s = (mBaseLocale->GetScript().GetLength() != 0);
    Boolean r = (mBaseLocale->GetRegion().GetLength() != 0);
    Boolean v = (mBaseLocale->GetVariant().GetLength() != 0);
    Boolean e = (mLocaleExtensions != nullptr && mLocaleExtensions->GetID().GetLength() != 0);

    AutoPtr<IStringBuilder> result;
    CStringBuilder::New(mBaseLocale->GetLanguage(), IID_IStringBuilder, (IInterface**)&result);
    if (r || (l && (v || s || e))) {
        result->Append(U'_');
        result->Append(mBaseLocale->GetRegion()); // This may just append '_'
    }
    if (v && (l || r)) {
        result->Append(U'_');
        result->Append(mBaseLocale->GetVariant());
    }

    if (s && (l || r)) {
        result->Append(String("_#"));
        result->Append(mBaseLocale->GetScript());
    }

    if (e && (l || r)) {
        result->Append(U'_');
        if (!s) {
            result->Append(U'#');
        }
        result->Append(mLocaleExtensions->GetID());
    }

    return result->ToString(desc);
}

ECode Locale::ToLanguageTag(
    /* [out] */ String* langTag)
{
    VALIDATE_NOT_NULL(langTag);

    VOLATILE_GET(String languageTag, mLanguageTag);
    if (!languageTag.IsNull()) {
        *langTag = languageTag;
        return NOERROR;
    }

    AutoPtr<ILanguageTag> tag = LanguageTag::ParseLocale(mBaseLocale, mLocaleExtensions);
    AutoPtr<IStringBuilder> buf;
    CStringBuilder::New(IID_IStringBuilder, (IInterface**)&buf);

    String subtag;
    tag->GetLanguage(&subtag);
    if (subtag.GetLength() > 0) {
        buf->Append(LanguageTag::CanonicalizeLanguage(subtag));
    }

    tag->GetScript(&subtag);
    if (subtag.GetLength() > 0) {
        buf->Append(ILanguageTag::SEP);
        buf->Append(LanguageTag::CanonicalizeScript(subtag));
    }

    tag->GetRegion(&subtag);
    if (subtag.GetLength() > 0) {
        buf->Append(ILanguageTag::SEP);
        buf->Append(LanguageTag::CanonicalizeRegion(subtag));
    }

    AutoPtr<IList> subtags;
    tag->GetVariants(&subtags);
    AutoPtr<IIterator> it;
    subtags->GetIterator(&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->Next(&obj);
        buf->Append(ILanguageTag::SEP);
        buf->Append(CoreUtils::Unbox(ICharSequence::Probe(obj)));
    }

    subtags = nullptr;
    tag->GetExtensions(&subtags);
    it = nullptr;
    subtags->GetIterator(&it);
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->Next(&obj);
        buf->Append(ILanguageTag::SEP);
        buf->Append(LanguageTag::CanonicalizeExtension(
                CoreUtils::Unbox(ICharSequence::Probe(obj))));
    }

    tag->GetPrivateuse(&subtag);
    if (subtag.GetLength() > 0) {
        Integer len;
        if (buf->GetLength(&len), len > 0) {
            buf->Append(ILanguageTag::SEP);
        }
        buf->Append(ILanguageTag::PRIVATEUSE);
        buf->Append(ILanguageTag::SEP);
        buf->Append(subtag);
    }

    buf->ToString(*langTag);
    {
        AutoLock lock(this);
        VOLATILE_GET(String languageTag, mLanguageTag);
        if (languageTag.IsNull()) {
            VOLATILE_SET(mLanguageTag, *langTag);
            return NOERROR;
        }
    }
    VOLATILE_GET(*langTag, languageTag);
    return NOERROR;
}

AutoPtr<ILocale> Locale::ForLanguageTag(
    /* [in] */ const String& languageTag)
{
    AutoPtr<ILanguageTag> tag = LanguageTag::Parse(languageTag, nullptr);
    AutoPtr<InternalLocaleBuilder> bldr = new InternalLocaleBuilder();
    ECode ec = bldr->SetLanguageTag(tag);
    CHECK(SUCCEEDED(ec));
    AutoPtr<BaseLocale> base = bldr->GetBaseLocale();
    AutoPtr<LocaleExtensions> exts = bldr->GetLocaleExtensions();
    if (exts == nullptr && (base->GetVariant().GetLength() > 0)) {
        exts = GetCompatibilityExtensions(base->GetLanguage(), base->GetScript(),
                base->GetRegion(), base->GetVariant());
    }
    return GetInstance(base, exts);
}

ECode Locale::GetISO3Language(
    /* [out] */ String* language)
{
    VALIDATE_NOT_NULL(language);

    String lang = mBaseLocale->GetLanguage();
    if (lang.GetLength() == 3) {
        *language = lang;
        return NOERROR;
    }
    else if (lang.IsEmpty()) {
        *language = "";
        return NOERROR;
    }

    String language3 = ICU::GetISO3Language(lang);
    if (!lang.IsEmpty() && language3.IsEmpty()) {
        String str;
        ToString(str);
        Logger::E("Locale", "Couldn't find 3-letter language code for %sFormatData_%sShortLanguage",
                lang.string(), str.string());
        return E_MISSING_RESOURCE_EXCEPTION;
    }
    *language = language3;
    return NOERROR;
}

ECode Locale::GetISO3Country(
    /* [out] */ String* country)
{
    VALIDATE_NOT_NULL(country);

    String region = mBaseLocale->GetRegion();
    if (region.GetLength() == 3) {
        *country = region;
        return NOERROR;
    }
    else if (region.IsEmpty()) {
        *country = "";
        return NOERROR;
    }

    // Prefix "en-" because ICU doesn't really care about what the language is.
    String country3 = ICU::GetISO3Country(String("en-") + region);
    if (!region.IsEmpty() && country3.IsEmpty()) {
        String str;
        ToString(str);
        Logger::E("Locale", "Couldn't find 3-letter country code for %sFormatData_%sShortCountry",
                region.string(), str.string());
        return E_MISSING_RESOURCE_EXCEPTION;
    }
    *country = country3;
    return NOERROR;
}

ECode Locale::GetDisplayLanguage(
    /* [out] */ String* language)
{
    VALIDATE_NOT_NULL(language);

    return GetDisplayLanguage(GetDefault(Category::GetDISPLAY()), language);
}

ECode Locale::GetDisplayLanguage(
    /* [in] */ ILocale* locale,
    /* [out] */ String* language)
{
    VALIDATE_NOT_NULL(language);

    String languageCode = mBaseLocale->GetLanguage();
    if (languageCode.IsEmpty()) {
        *language = "";
        return NOERROR;
    }

    // Hacks for backward compatibility.
    //
    // Our language tag will contain "und" if the languageCode is invalid
    // or missing. ICU will then return "langue indéterminée" or the equivalent
    // display language for the indeterminate language code.
    //
    String normalizedLanguage;
    FAIL_RETURN(NormalizeAndValidateLanguage(
            languageCode, false, &normalizedLanguage));
    if (UNDETERMINED_LANGUAGE.Equals(normalizedLanguage)) {
        *language = languageCode;
        return NOERROR;
    }

    String result = ICU::GetDisplayLanguage(this, locale);
    if (result.IsNull()) {
        result = ICU::GetDisplayLanguage(this, GetDefault());
    }
    *language = result;
    return NOERROR;
}

ECode Locale::NormalizeAndValidateLanguage(
    /* [in] */ const String& language,
    /* [in] */ Boolean strict,
    /* [out] */ String* retLanguage)
{
    if (language.IsEmpty()) {
        *retLanguage = "";
        return NOERROR;
    }

    String lowercaseLanguage = language.ToLowerCase(/*Locale::ROOT*/);
    if (!IsValidBcp47Alpha(lowercaseLanguage, 2, 3)) {
        if (strict) {
            Logger::E("Locale", "Invalid language: %s", language.string());
            return E_ILLFORMED_LOCALE_EXCEPTION;
        }
        else {
            *retLanguage = UNDETERMINED_LANGUAGE;
            return NOERROR;
        }
    }

    *retLanguage = lowercaseLanguage;
    return NOERROR;
}

Boolean Locale::IsAsciiAlphaNum(
    /* [in] */ const String& string)
{
    for (Integer i = 0; i < string.GetByteLength(); i++) {
        char c = string.string()[i];
        if (!((c >= U'a' && c <= U'z') ||
                (c >= U'A' && c <= U'Z') ||
                (c >= U'0' && c <= U'9'))) {
            return false;
        }
    }

    return true;
}

ECode Locale::GetDisplayScript(
    /* [out] */ String* script)
{
    VALIDATE_NOT_NULL(script);

    return GetDisplayScript(GetDefault(Category::GetDISPLAY()), script);
}

ECode Locale::GetDisplayScript(
    /* [in] */ ILocale* inLocale,
    /* [out] */ String* script)
{
    VALIDATE_NOT_NULL(script);

    String scriptCode = mBaseLocale->GetScript();
    if (scriptCode.IsEmpty()) {
        *script = "";
        return NOERROR;
    }

    String result = ICU::GetDisplayScript(this, inLocale);
    if (result.IsNull()) {
        result = ICU::GetDisplayScript(this, GetDefault(Category::GetDISPLAY()));
    }

    *script = result;
    return NOERROR;
}

ECode Locale::GetDisplayCountry(
    /* [out] */ String* country)
{
    VALIDATE_NOT_NULL(country);

    return GetDisplayCountry(GetDefault(Category::GetDISPLAY()), country);
}

ECode Locale::GetDisplayCountry(
    /* [in] */ ILocale* locale,
    /* [out] */ String* country)
{
    VALIDATE_NOT_NULL(country);

    String countryCode = mBaseLocale->GetRegion();
    if (countryCode.IsEmpty()) {
        *country = "";
        return NOERROR;
    }

    String normalizedRegion;
    FAIL_RETURN(NormalizeAndValidateRegion(countryCode, false, &normalizedRegion));
    if (normalizedRegion.IsEmpty()) {
        *country = countryCode;
        return NOERROR;
    }

    String result = ICU::GetDisplayCountry(this, locale);
    if (result.IsNull()) {
        result = ICU::GetDisplayCountry(this, GetDefault());
    }
    *country = result;
    return NOERROR;
}

ECode Locale::NormalizeAndValidateRegion(
    /* [in] */ const String& region,
    /* [in] */ Boolean strict,
    /* [out] */ String* retRegion)
{
    if (region.IsEmpty()) {
        *retRegion = "";
        return NOERROR;
    }

    String uppercaseRegion = region.ToUpperCase(/*Locale::ROOT*/);
    if (!IsValidBcp47Alpha(uppercaseRegion, 2, 2) &&
            !IsUnM49AreaCode(uppercaseRegion)) {
        if (strict) {
            Logger::E("Locale", "Invalid region: %s", region.string());
            return E_ILLFORMED_LOCALE_EXCEPTION;
        }
        else {
            *retRegion = "";
            return NOERROR;
        }
    }

    *retRegion = uppercaseRegion;
    return NOERROR;
}

Boolean Locale::IsValidBcp47Alpha(
    /* [in] */ const String& string,
    /* [in] */ Integer lowerBound,
    /* [in] */ Integer upperBound)
{
    Integer length = string.GetByteLength();
    if (length < lowerBound || length > upperBound) {
        return false;
    }

    for (Integer i = 0; i < length; i++) {
        char c = string.string()[i];
        if (!((c >= U'a' && c <= U'z') ||
                (c >= U'A' && c <= U'Z'))) {
            return false;
        }
    }

    return true;
}

Boolean Locale::IsUnM49AreaCode(
    /* [in] */ const String& code)
{
    if (code.GetByteLength() != 3) {
        return false;
    }

    for (Integer i = 0; i < 3; ++i) {
        char c = code.string()[i];
        if (!(c >= U'0' && c <= U'9')) {
            return false;
        }
    }

    return true;
}

ECode Locale::GetDisplayVariant(
    /* [out] */ String* variant)
{
    VALIDATE_NOT_NULL(variant);

    return GetDisplayVariant(GetDefault(Category::GetDISPLAY()), variant);
}

ECode Locale::GetDisplayVariant(
    /* [in] */ ILocale* inLocale,
    /* [out] */ String* variant)
{
    VALIDATE_NOT_NULL(variant);

    String variantCode = mBaseLocale->GetVariant();
    if (variantCode.IsEmpty()) {
        *variant = "";
        return NOERROR;
    }

    String normalizedVariant;
    ECode ec = NormalizeAndValidateVariant(variantCode, &normalizedVariant);
    if (FAILED(ec)) {
        *variant = variantCode;
        return NOERROR;
    }

    String result = ICU::GetDisplayVariant(this, inLocale);
    if (result.IsNull()) {
        result = ICU::GetDisplayVariant(this, GetDefault());
    }

    if (result.IsEmpty()) {
        *variant = variantCode;
        return NOERROR;
    }
    *variant = result;
    return NOERROR;
}

ECode Locale::NormalizeAndValidateVariant(
    /* [in] */ const String& variant,
    /* [out] */ String* retVariant)
{
    if (variant.IsEmpty()) {
        *retVariant = "";
        return NOERROR;
    }

    String normalizedVariant = variant.Replace(U'-', U'_');
    Array<String> subTags = StringUtils::Split(normalizedVariant, String("_"));

    for (Integer i = 0; i < subTags.GetLength(); i++) {
        if (!IsValidVariantSubtag(subTags[i])) {
            Logger::E("Locale", "Invalid variant: %s", variant.string());
            return E_ILLFORMED_LOCALE_EXCEPTION;
        }
    }

    *retVariant = normalizedVariant;
    return NOERROR;
}

Boolean Locale::IsValidVariantSubtag(
    /* [in] */ const String& subTag)
{
    // The BCP-47 spec states that :
    // - Subtags can be between [5, 8] alphanumeric chars in length.
    // - Subtags that start with a number are allowed to be 4 chars in length.
    if (subTag.GetByteLength() >= 5 && subTag.GetByteLength() <= 8) {
        if (IsAsciiAlphaNum(subTag)) {
            return true;
        }
    }
    else if (subTag.GetByteLength() == 4) {
        char firstChar = subTag.string()[0];
        if ((firstChar >= U'0' && firstChar <= U'9') && IsAsciiAlphaNum(subTag)) {
            return true;
        }
    }

    return false;
}

ECode Locale::GetDisplayName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    return GetDisplayName(GetDefault(Category::GetDISPLAY()), name);
}

ECode Locale::GetDisplayName(
    /* [in] */ ILocale* locale,
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    Integer count = 0;
    AutoPtr<IStringBuilder> buffer;
    CStringBuilder::New(IID_IStringBuilder, (IInterface**)&buffer);
    String languageCode = mBaseLocale->GetLanguage();
    if (!languageCode.IsEmpty()) {
        String displayLanguage;
        FAIL_RETURN(GetDisplayLanguage(locale, &displayLanguage));
        buffer->Append(displayLanguage.IsEmpty() ? languageCode : displayLanguage);
        ++count;
    }
    String scriptCode = mBaseLocale->GetScript();
    if (!scriptCode.IsEmpty()) {
        if (count == 1) {
            buffer->Append(String(" ("));
        }
        String displayScript;
        FAIL_RETURN(GetDisplayScript(locale, &displayScript));
        buffer->Append(displayScript.IsEmpty() ? scriptCode : displayScript);
        ++count;
    }
    String countryCode = mBaseLocale->GetRegion();
    if (!countryCode.IsEmpty()) {
        if (count == 1) {
            buffer->Append(String(" ("));
        }
        else if (count == 2) {
            buffer->Append(U',');
        }
        String displayCountry;
        FAIL_RETURN(GetDisplayCountry(locale, &displayCountry));
        buffer->Append(displayCountry.IsEmpty() ? countryCode : displayCountry);
        ++count;
    }
    String variantCode = mBaseLocale->GetVariant();
    if (!variantCode.IsEmpty()) {
        if (count == 1) {
            buffer->Append(String(" ("));
        }
        else if (count == 2 || count == 3) {
            buffer->Append(U',');
        }
        String displayVariant;
        FAIL_RETURN(GetDisplayVariant(locale, &displayVariant));
        buffer->Append(displayVariant.IsEmpty() ? variantCode : displayVariant);
        ++count;
    }
    if (count > 1) {
        buffer->Append(U')');
    }
    return buffer->ToString(*name);
}

ECode Locale::CloneImpl(
    /* [in] */ ILocale* newObj)
{
    Locale* l = (Locale*)newObj;

    l->mBaseLocale = mBaseLocale;
    l->mLocaleExtensions = mLocaleExtensions;
    VOLATILE_GET(Integer hc, mHashCodeValue);
    VOLATILE_SET(l->mHashCodeValue, hc);
    VOLATILE_GET(String tag, mLanguageTag);
    VOLATILE_SET(l->mLanguageTag, tag);
    return NOERROR;
}

ECode Locale::GetHashCode(
    /* [out] */ Integer& hash)
{
    VOLATILE_GET(hash, mHashCodeValue);
    if (hash == 0) {
        mBaseLocale->GetHashCode(hash);
        if (mLocaleExtensions != nullptr) {
            Integer lhc;
            mLocaleExtensions->GetHashCode(lhc);
            hash ^= lhc;
        }
        VOLATILE_SET(mHashCodeValue, hash);
    }
    return NOERROR;
}

ECode Locale::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean& same)
{
    if (IInterface::Equals((ILocale*)this, obj)) {
        same = true;
        return NOERROR;
    }
    if (ILocale::Probe(obj) == nullptr) {
        same = false;
        return NOERROR;
    }
    Locale* l = (Locale*)ILocale::Probe(obj);
    BaseLocale* otherBase = l->mBaseLocale;
    if (mBaseLocale->Equals((IObject*)otherBase, same), !same) {
        return NOERROR;
    }
    if (mLocaleExtensions == nullptr) {
        same = l->mLocaleExtensions == nullptr;
        return NOERROR;
    }
    return mLocaleExtensions->Equals((IObject*)l->mLocaleExtensions, same);
}

Boolean Locale::IsUnicodeExtensionKey(
    /* [in] */ const String& s)
{
    return (s.GetByteLength() == 2) && LocaleUtils::IsAlphaNumericString(s);
}

String Locale::ConvertOldISOCodes(
    /* [in] */ const String& language_)
{
    String language = LocaleUtils::ToLowerString(language_);
    if (language.Equals("he")) {
        return String("iw");
    }
    else if (language.Equals("yi")) {
        return String("ji");
    }
    else if (language.Equals("id")) {
        return String("in");
    }
    else {
        return language;
    }
}

AutoPtr<LocaleExtensions> Locale::GetCompatibilityExtensions(
    /* [in] */ const String& language,
    /* [in] */ const String& script,
    /* [in] */ const String& country,
    /* [in] */ const String& variant)
{
    AutoPtr<LocaleExtensions> extensions;
    // Special cases for backward compatibility support
    if (LocaleUtils::CaseIgnoreMatch(language, String("ja")) &&
            script.GetLength() == 0 &&
            LocaleUtils::CaseIgnoreMatch(country, String("jp")) &&
            variant.Equals("JP")) {
        // ja_JP_JP -> u-ca-japanese (calendar = japanese)
        extensions = LocaleExtensions::GetCALENDAR_JAPANESE();
    }
    else if (LocaleUtils::CaseIgnoreMatch(language, String("th")) &&
            script.GetLength() == 0 &&
            LocaleUtils::CaseIgnoreMatch(country, String("th")) &&
            variant.Equals("TH")) {
        // th_TH_TH -> u-nu-thai (numbersystem = thai)
        extensions = LocaleExtensions::GetNUMBER_THAI();
    }
    return extensions;
}

//-------------------------------------------------------------------------

AutoPtr<IInterface> Locale::Cache::CreateObject(
    /* [in] */ IInterface* key)
{
    LocaleKey* keyObj = (LocaleKey*)IObject::Probe(key);
    AutoPtr<ILocale> locale;
    CLocale::New(keyObj->mBase, keyObj->mExts, IID_ILocale, (IInterface**)&locale);
    return locale.Get();
}

//-------------------------------------------------------------------------

const InterfaceID IID_LocaleKey =
        {{0x90e49e2f,0xbd27,0x480b,0xb2a5,{0x23,0x38,0xb1,0xfb,0x9f,0x6d}}, &CID_libcore};

Locale::LocaleKey::LocaleKey(
    /* [in] */ BaseLocale* baseLocale,
    /* [in] */ LocaleExtensions* extensions)
    : mBase(baseLocale)
    , mExts(extensions)
{
    // Calculate the hash value here because it's always used.
    Integer h;
    mBase->GetHashCode(h);
    if (mExts != nullptr) {
        Integer eh;
        mExts->GetHashCode(eh);
        h ^= eh;
    }
    mHash = h;
}

IInterface* Locale::LocaleKey::Probe(
    /* [in] */ const InterfaceID& iid)
{
    if (iid == IID_LocaleKey) {
        return (IInterface*)(IObject*)this;
    }
    return Object::Probe(iid);
}

ECode Locale::LocaleKey::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean& same)
{
    if (IInterface::Equals((IObject*)this, obj)) {
        same = true;
        return NOERROR;
    }
    if (obj->Probe(IID_LocaleKey) == nullptr) {
        same = false;
        return NOERROR;
    }
    LocaleKey* other = (LocaleKey*)IObject::Probe(obj);
    Boolean equals;
    if (mHash != other->mHash ||
            (mBase->Equals((IObject*)other->mBase, equals), !equals)) {
        same = false;
        return NOERROR;
    }
    if (mExts == nullptr) {
        same = other->mExts == nullptr;
        return NOERROR;
    }
    return mExts->Equals((IObject*)other->mExts, same);
}

ECode Locale::LocaleKey::GetHashCode(
    /* [out] */ Integer& hash)
{
    hash = mHash;
    return NOERROR;
}

//-------------------------------------------------------------------------

COMO_INTERFACE_IMPL_LIGHT_1(Locale::Category, LightRefBase, ILocaleCategory);

AutoPtr<ILocaleCategory> Locale::Category::GetDISPLAY()
{
    static AutoPtr<ILocaleCategory> DISPLAY = new Category(
            String("user.language.display"),
            String("user.script.display"),
            String("user.country.display"),
            String("user.variant.display"));
    return DISPLAY;
}

AutoPtr<ILocaleCategory> Locale::Category::GetFORMAT()
{
    static AutoPtr<ILocaleCategory> FORMAT = new Category(
            String("user.language.format"),
            String("user.script.format"),
            String("user.country.format"),
            String("user.variant.format"));
    return FORMAT;
}

ECode Locale::Category::GetLanguageKey(
    /* [out] */ String* key)
{
    VALIDATE_NOT_NULL(key);

    *key = mLanguageKey;
    return  NOERROR;
}

ECode Locale::Category::GetScriptKey(
    /* [out] */ String* key)
{
    VALIDATE_NOT_NULL(key);

    *key = mScriptKey;
    return NOERROR;
}

ECode Locale::Category::GetCountryKey(
    /* [out] */ String* key)
{
    VALIDATE_NOT_NULL(key);

    *key = mCountryKey;
    return NOERROR;
}

ECode Locale::Category::GetVariantKey(
    /* [out] */ String* key)
{
    VALIDATE_NOT_NULL(key);

    *key = mVariantKey;
    return NOERROR;
}

//-------------------------------------------------------------------------

COMO_INTERFACE_IMPL_1(Locale::Builder, Object, ILocaleBuilder);

ECode Locale::Builder::Constructor()
{
    mLocaleBuilder = new InternalLocaleBuilder();
    return NOERROR;
}

ECode Locale::Builder::SetLocale(
    /* [in] */ ILocale* locale)
{
    Locale* l = (Locale*)locale;
    ECode ec = mLocaleBuilder->SetLocale(l->mBaseLocale, l->mLocaleExtensions);
    return SUCCEEDED(ec) ? ec : E_ILLFORMED_LOCALE_EXCEPTION;
}

ECode Locale::Builder::SetLanguageTag(
    /* [in] */ const String& languageTag)
{
    AutoPtr<ParseStatus> sts = new ParseStatus();
    AutoPtr<ILanguageTag> tag = LanguageTag::Parse(languageTag, sts);
    if (sts->IsError()) {
        Logger::E("Locale::Builder", "%s", sts->GetErrorMessage().string());
        return E_ILLFORMED_LOCALE_EXCEPTION;
    }
    ECode ec = mLocaleBuilder->SetLanguageTag(tag);
    return SUCCEEDED(ec) ? ec : E_ILLFORMED_LOCALE_EXCEPTION;
}

ECode Locale::Builder::SetLanguage(
    /* [in] */ const String& language)
{
    ECode ec = mLocaleBuilder->SetLanguage(language);
    return SUCCEEDED(ec) ? ec : E_ILLFORMED_LOCALE_EXCEPTION;
}

ECode Locale::Builder::SetScript(
    /* [in] */ const String& script)
{
    ECode ec = mLocaleBuilder->SetScript(script);
    return SUCCEEDED(ec) ? ec : E_ILLFORMED_LOCALE_EXCEPTION;
}

ECode Locale::Builder::SetRegion(
    /* [in] */ const String& region)
{
    ECode ec = mLocaleBuilder->SetRegion(region);
    return SUCCEEDED(ec) ? ec : E_ILLFORMED_LOCALE_EXCEPTION;
}

ECode Locale::Builder::SetVariant(
    /* [in] */ const String& variant)
{
    ECode ec = mLocaleBuilder->SetVariant(variant);
    return SUCCEEDED(ec) ? ec : E_ILLFORMED_LOCALE_EXCEPTION;
}

ECode Locale::Builder::SetExtension(
    /* [in] */ Char key,
    /* [in] */ const String& value)
{
    ECode ec = mLocaleBuilder->SetExtension(key, value);
    return SUCCEEDED(ec) ? ec : E_ILLFORMED_LOCALE_EXCEPTION;
}

ECode Locale::Builder::SetUnicodeLocaleKeyword(
    /* [in] */ const String& key,
    /* [in] */ const String& type)
{
    ECode ec = mLocaleBuilder->SetUnicodeLocaleKeyword(key, type);
    return SUCCEEDED(ec) ? ec : E_ILLFORMED_LOCALE_EXCEPTION;
}

ECode Locale::Builder::AddUnicodeLocaleAttribute(
    /* [in] */ const String& attribute)
{
    ECode ec = mLocaleBuilder->AddUnicodeLocaleAttribute(attribute);
    return SUCCEEDED(ec) ? ec : E_ILLFORMED_LOCALE_EXCEPTION;
}

ECode Locale::Builder::RemoveUnicodeLocaleAttribute(
    /* [in] */ const String& attribute)
{
    if (attribute.IsNull()) {
        Logger::E("Locale::Builder", "attribute == nullptr");
        return como::core::E_NULL_POINTER_EXCEPTION;
    }

    ECode ec = mLocaleBuilder->RemoveUnicodeLocaleAttribute(attribute);
    return SUCCEEDED(ec) ? ec : E_ILLFORMED_LOCALE_EXCEPTION;
}

ECode Locale::Builder::Clear()
{
    return mLocaleBuilder->Clear();
}

ECode Locale::Builder::ClearExtensions()
{
    return mLocaleBuilder->ClearExtensions();
}

ECode Locale::Builder::Build(
    /* [out] */ ILocale** locale)
{
    VALIDATE_NOT_NULL(locale);

    AutoPtr<BaseLocale> baseloc = mLocaleBuilder->GetBaseLocale();
    AutoPtr<LocaleExtensions> extensions = mLocaleBuilder->GetLocaleExtensions();
    if (extensions == nullptr && baseloc->GetVariant().GetLength() > 0) {
        extensions = Locale::GetCompatibilityExtensions(
                baseloc->GetLanguage(), baseloc->GetScript(),
                baseloc->GetRegion(), baseloc->GetVariant());
    }
    Locale::GetInstance(baseloc, extensions).MoveTo(locale);
    return NOERROR;
}

}
}
