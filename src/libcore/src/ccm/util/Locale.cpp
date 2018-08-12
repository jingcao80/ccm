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

#include "ccm/core/AutoLock.h"
#include "ccm/core/CoreUtils.h"
#include "ccm/core/CStringBuilder.h"
#include "ccm/core/System.h"
#include "ccm/util/CLocale.h"
#include "ccm/util/Collections.h"
#include "ccm/util/CPropertyPermission.h"
#include "ccm/util/Locale.h"
#include "ccm/util/locale/InternalLocaleBuilder.h"
#include "ccm/util/locale/LanguageTag.h"
#include "libcore/icu/ICU.h"
#include "ccm.core.ICharSequence.h"
#include "ccm.core.IStringBuilder.h"
#include "ccm.core.ISecurityManager.h"
#include "ccm.security.IPermission.h"
#include "ccm.util.locale.ILanguageTag.h"
#include <ccmlogger.h>

using ccm::core::AutoLock;
using ccm::core::CoreUtils;
using ccm::core::CStringBuilder;
using ccm::core::ICharSequence;
using ccm::core::ISecurityManager;
using ccm::core::IStringBuilder;
using ccm::core::IID_IStringBuilder;
using ccm::core::System;
using ccm::core::IID_ICloneable;
using ccm::io::IID_ISerializable;
using ccm::security::IPermission;
using ccm::security::IID_IPermission;
using ccm::util::locale::ILanguageTag;
using ccm::util::locale::InternalLocaleBuilder;
using ccm::util::locale::LanguageTag;
using libcore::icu::ICU;

namespace ccm {
namespace util {

INIT_PROI_3 const String Locale::UNDETERMINED_LANGUAGE("und");
INIT_PROI_3 AutoPtr<ILocale> Locale::sDefaultDisplayLocale;
INIT_PROI_3 AutoPtr<ILocale> Locale::sDefaultFormatLocale;

CCM_INTERFACE_IMPL_3(Locale, SyncObject, ILocale, ISerializable, ICloneable);

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

AutoPtr<ILocale> Locale::GetOrSetDefaultLocale(
    /* [in] */ ILocale* locale)
{
    static AutoPtr<ILocale> sDefaultLocale = InitDefault();
    if (locale != nullptr) {
        sDefaultLocale = locale;
    }
    return sDefaultLocale;
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
        return ccm::core::E_NULL_POINTER_EXCEPTION;
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
    GetLOCALECACHE()->Get((IObject*)key.Get(), (IInterface**)&value);
    return ILocale::Probe(value);
}

AutoPtr<ILocale> Locale::GetDefault()
{
    return GetOrSetDefaultLocale(nullptr);
}

AutoPtr<ILocale> Locale::GetDefault(
    /* [in] */ ILocaleCategory* category)
{
    if (category == GetDisplayCategory()) {
        if (sDefaultDisplayLocale == nullptr) {
            AutoLock lock(GetClassLock());
            if (sDefaultDisplayLocale == nullptr) {
                sDefaultDisplayLocale = InitDefault(category);
            }
        }
        return sDefaultDisplayLocale;
    }
    if (category == GetFormatCategory()) {
        if (sDefaultFormatLocale == nullptr) {
            AutoLock lock(GetClassLock());
            if (sDefaultFormatLocale == nullptr) {
                sDefaultFormatLocale == InitDefault(category);
            }
        }
        return sDefaultFormatLocale;
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
        Integer i = region.IndexOf('_');
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

    FAIL_RETURN(SetDefault(GetDisplayCategory(), newLocale));
    FAIL_RETURN(SetDefault(GetFormatCategory(), newLocale));
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
        return ccm::core::E_NULL_POINTER_EXCEPTION;
    }
    if (newLocale == nullptr) {
        Logger::E("Locale", "Can't set default locale to NULL");
        return ccm::core::E_NULL_POINTER_EXCEPTION;
    }

    AutoLock lock(GetClassLock());

    AutoPtr<ISecurityManager> sm = System::GetSecurityManager();
    if (sm != nullptr) {
        AutoPtr<IPermission> pp;
        CPropertyPermission::New(String("user.language"), String("write"),
                IID_IPermission, (IInterface**)&pp);
        FAIL_RETURN(sm->CheckPermission(pp));
    }
    if (category == GetDisplayCategory()) {
        sDefaultDisplayLocale = newLocale;
        return NOERROR;
    }
    if (category == GetFormatCategory()) {
        sDefaultFormatLocale = newLocale;
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
        *keys = Collections::GetEmptySet();
        REFCOUNT_ADD(*keys);
        return NOERROR;
    }
    *keys = mLocaleExtensions->GetKeys();
    REFCOUNT_ADD(*keys);
    return NOERROR;
}

ECode Locale::GetUnicodeLocaleAttributes(
    /* [out] */ ISet** attrs)
{
    VALIDATE_NOT_NULL(attrs);

    Boolean hasExt;
    if (HasExtensions(&hasExt), !hasExt) {
        *attrs = Collections::GetEmptySet();
        REFCOUNT_ADD(*attrs);
        return NOERROR;
    }
    *attrs = mLocaleExtensions->GetUnicodeLocaleAttributes();
    REFCOUNT_ADD(*attrs);
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
        *keys = Collections::GetEmptySet();
        REFCOUNT_ADD(*keys);
        return NOERROR;
    }
    *keys = mLocaleExtensions->GetUnicodeLocaleKeys();
    REFCOUNT_ADD(*keys);
    return NOERROR;
}

ECode Locale::ToString(
    /* [out] */ String* desc)
{
    VALIDATE_NOT_NULL(desc);

    Boolean l = (mBaseLocale->GetLanguage().GetLength() != 0);
    Boolean s = (mBaseLocale->GetScript().GetLength() != 0);
    Boolean r = (mBaseLocale->GetRegion().GetLength() != 0);
    Boolean v = (mBaseLocale->GetVariant().GetLength() != 0);
    Boolean e = (mLocaleExtensions != nullptr && mLocaleExtensions->GetID().GetLength() != 0);

    AutoPtr<IStringBuilder> result;
    CStringBuilder::New(mBaseLocale->GetLanguage(), IID_IStringBuilder, (IInterface**)&result);
    if (r || (l && (v || s || e))) {
        result->AppendChar('_');
        result->Append(mBaseLocale->GetRegion()); // This may just append '_'
    }
    if (v && (l || r)) {
        result->AppendChar('_');
        result->Append(mBaseLocale->GetVariant());
    }

    if (s && (l || r)) {
        result->Append(String("_#"));
        result->Append(mBaseLocale->GetScript());
    }

    if (e && (l || r)) {
        result->AppendChar('_');
        if (!s) {
            result->AppendChar('#');
        }
        result->Append(mLocaleExtensions->GetID());
    }

    return result->ToString(desc);
}

ECode Locale::ToLanguageTag(
    /* [out] */ String* langTag)
{
    VALIDATE_NOT_NULL(langTag);

    if (!mLanguageTag.IsNull()) {
        *langTag = mLanguageTag;
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
    tag->GetVariants((IList**)&subtags);
    AutoPtr<IIterator> it;
    subtags->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->Next((IInterface**)&obj);
        buf->Append(ILanguageTag::SEP);
        buf->Append(CoreUtils::Unbox(ICharSequence::Probe(obj)));
    }

    subtags = nullptr;
    tag->GetExtensions((IList**)&subtags);
    it = nullptr;
    subtags->GetIterator((IIterator**)&it);
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->Next((IInterface**)&obj);
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

    buf->ToString(langTag);
    {
        AutoLock lock(this);
        if (mLanguageTag.IsNull()) {
            mLanguageTag = *langTag;
        }
    }
    *langTag = mLanguageTag;
    return NOERROR;
}

AutoPtr<ILocale> Locale::ForLanguageTag(
    /* [in] */ const String& languageTag)
{
    AutoPtr<ILanguageTag> tag = LanguageTag::Parse(languageTag, nullptr);
    AutoPtr<InternalLocaleBuilder> bldr = new InternalLocaleBuilder();
    bldr->SetLanguageTag(tag);
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
        ToString(&str);
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
        ToString(&str);
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

    return GetDisplayLanguage(GetDefault(GetDisplayCategory()), language);
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
        {{0x90e49e2f,0xbd27,0x480b,0xb2a5,{0x2,0x3,0x3,0x8,0xb,0x1,0xf,0xb,0x9,0xf,0x6,0xd}}, &CID_libcore};

Locale::LocaleKey::LocaleKey(
    /* [in] */ BaseLocale* baseLocale,
    /* [in] */ LocaleExtensions* extensions)
    : mBase(baseLocale)
    , mExts(extensions)
{
    // Calculate the hash value here because it's always used.
    Integer h;
    mBase->GetHashCode(&h);
    if (mExts != nullptr) {
        Integer eh;
        mExts->GetHashCode(&eh);
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
    /* [out] */ Boolean* same)
{
    VALIDATE_NOT_NULL(same);

    if (IInterface::Equals((IObject*)this, obj)) {
        *same = true;
        return NOERROR;
    }
    if (obj->Probe(IID_LocaleKey) == nullptr) {
        *same = false;
        return NOERROR;
    }
    LocaleKey* other = (LocaleKey*)IObject::Probe(obj);
    Boolean equals;
    if (mHash != other->mHash ||
            (mBase->Equals((IObject*)other->mBase, &equals), !equals)) {
        *same = false;
        return NOERROR;
    }
    if (mExts == nullptr) {
        *same = other->mExts == nullptr;
        return NOERROR;
    }
    return mExts->Equals((IObject*)other->mExts, same);
}

ECode Locale::LocaleKey::GetHashCode(
    /* [out] */ Integer* hash)
{
    VALIDATE_NOT_NULL(hash);

    *hash = mHash;
    return NOERROR;
}

}
}
