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

#include "como/core/CoreUtils.h"
#include "como/core/CStringBuilder.h"
#include "como/util/CArrayList.h"
#include "como/util/CHashMap.h"
#include "como/util/CHashSet.h"
#include "como/util/locale/InternalLocaleBuilder.h"
#include "como/util/locale/LanguageTag.h"
#include "como/util/locale/LocaleUtils.h"
#include "como/util/locale/StringTokenIterator.h"
#include "como/util/locale/UnicodeLocaleExtension.h"
#include "como.core.IChar.h"
#include "como.core.ICharSequence.h"
#include <coredef.h>
#include <comolog.h>

using como::core::CoreUtils;
using como::core::CStringBuilder;
using como::core::IChar;
using como::core::ICharSequence;
using como::core::IStringBuilder;
using como::core::IID_IStringBuilder;

namespace como {
namespace util {
namespace locale {

AutoPtr<InternalLocaleBuilder::CaseInsensitiveChar> InternalLocaleBuilder::GetPRIVATEUSE_KEY()
{
    static AutoPtr<CaseInsensitiveChar> PRIVATEUSE_KEY = new CaseInsensitiveChar(ILanguageTag::PRIVATEUSE);
    return PRIVATEUSE_KEY;
}

InternalLocaleBuilder::InternalLocaleBuilder()
    : mLanguage("")
    , mScript("")
    , mRegion("")
    , mVariant("")
{}

ECode InternalLocaleBuilder::SetLanguage(
    /* [in] */ const String& language)
{
    if (LocaleUtils::IsEmpty(language)) {
        mLanguage = "";
    }
    else {
        if (!LanguageTag::IsLanguage(language)) {
            Logger::E("InternalLocaleBuilder", "Ill-formed language: %s", language.string());
            return E_LOCALE_SYNTAX_EXCEPTION;
        }
        mLanguage = language;
    }
    return NOERROR;
}

ECode InternalLocaleBuilder::SetScript(
    /* [in] */ const String& script)
{
    if (LocaleUtils::IsEmpty(script)) {
        mScript = "";
    }
    else {
        if (!LanguageTag::IsScript(script)) {
            Logger::E("InternalLocaleBuilder", "Ill-formed script: %s", script.string());
            return E_LOCALE_SYNTAX_EXCEPTION;
        }
        mScript = script;
    }
    return NOERROR;
}

ECode InternalLocaleBuilder::SetRegion(
    /* [in] */ const String& region)
{
    if (LocaleUtils::IsEmpty(region)) {
        mRegion = "";
    }
    else {
        if (!LanguageTag::IsRegion(region)) {
            Logger::E("InternalLocaleBuilder", "Ill-formed region: %s", region.string());
            return E_LOCALE_SYNTAX_EXCEPTION;
        }
        mRegion = region;
    }
    return NOERROR;
}

ECode InternalLocaleBuilder::SetVariant(
    /* [in] */ const String& variant)
{
    if (LocaleUtils::IsEmpty(variant)) {
        mVariant = "";
    }
    else {
        String var = variant.Replace(ILanguageTag::SEP, BaseLocale::SEP);
        Integer errIdx = CheckVariants(var, BaseLocale::SEP);
        if (errIdx != -1) {
            Logger::E("InternalLocaleBuilder", "Ill-formed variant: %s", variant.string());
            return E_LOCALE_SYNTAX_EXCEPTION;
        }
        mVariant = var;
    }
    return NOERROR;
}

ECode InternalLocaleBuilder::AddUnicodeLocaleAttribute(
    /* [in] */ const String& attribute)
{
    if (!UnicodeLocaleExtension::IsAttribute(attribute)) {
        Logger::E("InternalLocaleBuilder", "Ill-formed Unicode locale attribute: %s", attribute.string());
        return E_LOCALE_SYNTAX_EXCEPTION;
    }
    if (mUattributes == nullptr) {
        CHashSet::New(4, IID_ISet, (IInterface**)&mUattributes);
    }
    AutoPtr<CaseInsensitiveString> uattribute = new CaseInsensitiveString(attribute);
    mUattributes->Add((IObject*)uattribute.Get());
    return NOERROR;
}

ECode InternalLocaleBuilder::RemoveUnicodeLocaleAttribute(
    /* [in] */ const String& attribute)
{
    if (attribute.IsNull() || !UnicodeLocaleExtension::IsAttribute(attribute)) {
        Logger::E("InternalLocaleBuilder", "Ill-formed Unicode locale attribute: %s", attribute.string());
        return E_LOCALE_SYNTAX_EXCEPTION;
    }
    if (mUattributes != nullptr) {
        AutoPtr<CaseInsensitiveString> uattribute = new CaseInsensitiveString(attribute);
        mUattributes->Remove((IObject*)uattribute.Get());
    }
    return NOERROR;
}

ECode InternalLocaleBuilder::SetUnicodeLocaleKeyword(
    /* [in] */ const String& key,
    /* [in] */ const String& type)
{
    if (!UnicodeLocaleExtension::IsKey(key)) {
        Logger::E("InternalLocaleBuilder", "Ill-formed Unicode locale keyword key: %s", key.string());
        return E_LOCALE_SYNTAX_EXCEPTION;
    }

    AutoPtr<CaseInsensitiveString> ciKey = new CaseInsensitiveString(key);
    if (type.IsNull()) {
        if (mUkeywords != nullptr) {
            // null type is used for remove the key
            mUkeywords->Remove((IObject*)ciKey.Get());
        }
    }
    else {
        if (type.GetLength() != 0) {
            String tp = type.Replace(BaseLocale::SEP, ILanguageTag::SEP);
            // validate
            AutoPtr<StringTokenIterator> itr = new StringTokenIterator();
            itr->Constructor(tp, ILanguageTag::SEP);
            while (!itr->IsDone()) {
                String s = itr->Current();
                if (!UnicodeLocaleExtension::IsTypeSubtag(s)) {
                    Logger::E("InternalLocaleBuilder", "Ill-formed Unicode locale keyword type: %s", type.string());
                    return E_LOCALE_SYNTAX_EXCEPTION;
                }
                itr->Next();
            }
        }
        if (mUkeywords == nullptr) {
            CHashMap::New(4, IID_IMap, (IInterface**)&mUkeywords);
        }
        mUkeywords->Put((IObject*)ciKey.Get(), CoreUtils::Box(type));
    }
    return NOERROR;
}

ECode InternalLocaleBuilder::SetExtension(
    /* [in] */ Char singleton,
    /* [in] */ const String& value)
{
    // validate key
    Boolean isBcpPrivateuse = LanguageTag::IsPrivateusePrefixChar(singleton);
    if (!isBcpPrivateuse && !LanguageTag::IsExtensionSingletonChar(singleton)) {
        Logger::E("InternalLocaleBuilder", "Ill-formed extension key: %c", singleton);
        return E_LOCALE_SYNTAX_EXCEPTION;
    }

    Boolean remove = LocaleUtils::IsEmpty(value);
    AutoPtr<CaseInsensitiveChar> key = new CaseInsensitiveChar(singleton);

    if (remove) {
        if (UnicodeLocaleExtension::IsSingletonChar(key->Value())) {
            if (mUattributes != nullptr) {
                mUattributes->Clear();
            }
            if (mUkeywords != nullptr) {
                mUkeywords->Clear();
            }
        }
        else {
            Boolean contained;
            if (mExtensions != nullptr &&
                    (mExtensions->ContainsKey((IObject*)key.Get(), contained), contained)) {
                mExtensions->Remove((IObject*)key.Get());
            }
        }
    }
    else {
        // validate
        String val = value.Replace(BaseLocale::SEP, ILanguageTag::SEP);
        AutoPtr<StringTokenIterator> itr = new StringTokenIterator();
        itr->Constructor(val, ILanguageTag::SEP);
        while (!itr->IsDone()) {
            String s = itr->Current();
            Boolean validSubtag;
            if (isBcpPrivateuse) {
                validSubtag = LanguageTag::IsPrivateuseSubtag(s);
            }
            else {
                validSubtag = LanguageTag::IsExtensionSubtag(s);
            }
            if (!validSubtag) {
                Logger::E("InternalLocaleBuilder", "Ill-formed extension value: %s", s.string());
                return E_LOCALE_SYNTAX_EXCEPTION;
            }
            itr->Next();
        }

        if (UnicodeLocaleExtension::IsSingletonChar(key->Value())) {
            SetUnicodeLocaleExtension(val);
        }
        else {
            if (mExtensions == nullptr) {
                CHashMap::New(4, IID_IMap, (IInterface**)&mExtensions);
            }
            mExtensions->Put((IObject*)key.Get(), CoreUtils::Box(val));
        }
    }
    return NOERROR;
}

ECode InternalLocaleBuilder::SetExtensions(
    /* [in] */ const String& _subtags)
{
    if (LocaleUtils::IsEmpty(_subtags)) {
        ClearExtensions();
        return NOERROR;
    }
    String subtags = _subtags.Replace(BaseLocale::SEP, ILanguageTag::SEP);
    AutoPtr<StringTokenIterator> itr = new StringTokenIterator();
    itr->Constructor(subtags, ILanguageTag::SEP);

    AutoPtr<IList> extensions;
    String privateuse;

    Integer parsed = 0;
    Integer start;

    // Make a list of extension subtags
    while (!itr->IsDone()) {
        String s = itr->Current();
        if (LanguageTag::IsExtensionSingleton(s)) {
            start = itr->CurrentStart();
            String singleton = s;
            AutoPtr<IStringBuilder> sb;
            CStringBuilder::New(singleton, IID_IStringBuilder, (IInterface**)&sb);

            itr->Next();
            while (!itr->IsDone()) {
                s = itr->Current();
                if (LanguageTag::IsExtensionSubtag(s)) {
                    sb->Append(ILanguageTag::SEP);
                    sb->Append(s);
                    parsed = itr->CurrentEnd();
                }
                else {
                    break;
                }
                itr->Next();
            }

            if (parsed < start) {
                Logger::E("InternalLocaleBuilder", "Incomplete extension \"%s\"", singleton.string());
                return E_LOCALE_SYNTAX_EXCEPTION;
            }

            if (extensions == nullptr) {
                CArrayList::New(4, IID_IList, (IInterface**)&extensions);
            }
            String extension;
            sb->ToString(extension);
            extensions->Add(CoreUtils::Box(extension));
        }
        else {
            break;
        }
    }
    if (!itr->IsDone()) {
        String s = itr->Current();
        if (LanguageTag::IsPrivateusePrefix(s)) {
            start = itr->CurrentStart();
            AutoPtr<IStringBuilder> sb;
            CStringBuilder::New(s, IID_IStringBuilder, (IInterface**)&sb);

            itr->Next();
            while (!itr->IsDone()) {
                s = itr->Current();
                if (!LanguageTag::IsPrivateuseSubtag(s)) {
                    break;
                }
                sb->Append(ILanguageTag::SEP);
                sb->Append(s);
                parsed = itr->CurrentEnd();

                itr->Next();
            }
            if (parsed <= start) {
                Logger::E("InternalLocaleBuilder", "Incomplete privateuse:",
                        subtags.Substring(start).string());
                return E_LOCALE_SYNTAX_EXCEPTION;
            }
            else {
                sb->ToString(privateuse);
            }
        }
    }

    if (!itr->IsDone()) {
        Logger::E("InternalLocaleBuilder", "Ill-formed extension subtags:",
                subtags.Substring(itr->CurrentStart()).string());
        return E_LOCALE_SYNTAX_EXCEPTION;
    }

    return SetExtensions(extensions, privateuse);
}

ECode InternalLocaleBuilder::SetExtensions(
    /* [in] */ IList* bcpExtensions,
    /* [in] */ const String& privateuse)
{
    ClearExtensions();

    if (!LocaleUtils::IsEmpty(bcpExtensions)) {
        Integer size;
        bcpExtensions->GetSize(size);
        AutoPtr<ISet> done;
        CHashSet::New(size, IID_ISet, (IInterface**)&done);
        FOR_EACH(ICharSequence*, bcpExtension, ICharSequence::Probe, done) {
            String bcpExt = CoreUtils::Unbox(bcpExtension);
            AutoPtr<CaseInsensitiveChar> key = new CaseInsensitiveChar(bcpExt);
            Boolean contained;
            if (done->Contains((IObject*)key.Get(), contained), !contained) {
                // each extension string contains singleton, e.g. "a-abc-def"
                if (UnicodeLocaleExtension::IsSingletonChar(key->Value())) {
                    SetUnicodeLocaleExtension(bcpExt.Substring(2));
                }
                else {
                    if (mExtensions == nullptr) {
                        CHashMap::New(4, IID_IMap, (IInterface**)&mExtensions);
                    }
                    mExtensions->Put((IObject*)key.Get(), CoreUtils::Box(bcpExt.Substring(2)));
                }
            }
            done->Add((IObject*)key.Get());
        } END_FOR_EACH();
    }
    if (!privateuse.IsNull() && privateuse.GetLength() > 0) {
        // privateuse string contains prefix, e.g. "x-abc-def"
        if (mExtensions == nullptr) {
            CHashMap::New(1, IID_IMap, (IInterface**)&mExtensions);
        }
        AutoPtr<CaseInsensitiveChar> key = new CaseInsensitiveChar(privateuse);
        mExtensions->Put((IObject*)key.Get(), CoreUtils::Box(privateuse.Substring(2)));
    }

    return NOERROR;
}

ECode InternalLocaleBuilder::SetLanguageTag(
    /* [in] */ ILanguageTag* langtag)
{
    Clear();
    AutoPtr<IList> extLangs;
    langtag->GetExtlangs(extLangs);
    Boolean empty;
    if (extLangs->IsEmpty(empty), !empty) {
        AutoPtr<IInterface> extLang;
        extLangs->Get(0, &extLang);
        mLanguage = CoreUtils::Unbox(ICharSequence::Probe(extLang));
    }
    else {
        String lang;
        langtag->GetLanguage(lang);
        if (!lang.Equals(ILanguageTag::UNDETERMINED)) {
            mLanguage = lang;
        }
    }
    langtag->GetScript(mScript);
    langtag->GetRegion(mRegion);

    AutoPtr<IList> bcpVariants;
    langtag->GetVariants(bcpVariants);
    if (bcpVariants->IsEmpty(empty), !empty) {
        AutoPtr<IInterface> bcpVariant;
        bcpVariants->Get(0, &bcpVariant);
        AutoPtr<IStringBuilder> var;
        CStringBuilder::New(CoreUtils::Unbox(ICharSequence::Probe(bcpVariant)),
                IID_IStringBuilder, (IInterface**)&var);
        Integer size;
        bcpVariants->GetSize(size);
        for (Integer i = 1; i < size; i++) {
            bcpVariant = nullptr;
            bcpVariants->Get(i, &bcpVariant);
            var->Append(BaseLocale::SEP);
            var->Append(CoreUtils::Unbox(ICharSequence::Probe(bcpVariant)));
        }
        var->ToString(mVariant);
    }

    AutoPtr<IList> extensions;
    langtag->GetExtensions(extensions);
    String privateuse;
    langtag->GetPrivateuse(privateuse);

    return SetExtensions(extensions, privateuse);
}

ECode InternalLocaleBuilder::SetLocale(
    /* [in] */ BaseLocale* base,
    /* [in] */ LocaleExtensions* localeExtensions)
{
    String language = base->GetLanguage();
    String script = base->GetScript();
    String region = base->GetRegion();
    String variant = base->GetVariant();

    // Exception 1 - ja_JP_JP
    if (language.Equals("ja") && region.Equals("JP") && variant.Equals("JP")) {
        // When locale ja_JP_JP is created, ca-japanese is always there.
        // The builder ignores the variant "JP"
        CHECK(String("japanese").Equals(localeExtensions->GetUnicodeLocaleType(String("ca"))));
        variant = "";
    }
    // Exception 2 - th_TH_TH
    else if (language.Equals("th") && region.Equals("TH") && variant.Equals("TH")) {
        // When locale th_TH_TH is created, nu-thai is always there.
        // The builder ignores the variant "TH"
        CHECK(String("thai").Equals(localeExtensions->GetUnicodeLocaleType(String("nu"))));
        variant = "";
    }
    // Exception 3 - no_NO_NY
    else if (language.Equals("no") && region.Equals("NO") && variant.Equals("NY")) {
        // no_NO_NY is a valid locale and used by Java 6 or older versions.
        // The build ignores the variant "NY" and change the language to "nn".
        language = "nn";
        variant = "";
    }

    // Validate base locale fields before updating internal state.
    // LocaleExtensions always store validated/canonicalized values,
    // so no checks are necessary.
    if (language.GetLength() > 0 && !LanguageTag::IsLanguage(language)) {
        Logger::E("InternalLocaleBuilder", "Ill-formed language:", language.string());
        return E_LOCALE_SYNTAX_EXCEPTION;
    }

    if (script.GetLength() > 0 && !LanguageTag::IsScript(script)) {
        Logger::E("InternalLocaleBuilder", "Ill-formed script:", script.string());
        return E_LOCALE_SYNTAX_EXCEPTION;
    }

    if (region.GetLength() > 0 && !LanguageTag::IsRegion(region)) {
        Logger::E("InternalLocaleBuilder", "Ill-formed region:", region.string());
        return E_LOCALE_SYNTAX_EXCEPTION;
    }

    if (variant.GetLength() > 0) {
        variant = variant.Replace(ILanguageTag::SEP, BaseLocale::SEP);
        Integer errIdx = CheckVariants(variant, BaseLocale::SEP);
        if (errIdx != -1) {
            Logger::E("InternalLocaleBuilder", "Ill-formed variant:", variant.string());
            return E_LOCALE_SYNTAX_EXCEPTION;
        }
    }

    mLanguage = language;
    mScript = script;
    mRegion = region;
    mVariant = variant;
    ClearExtensions();

    AutoPtr<ISet> extKeys;
    if (localeExtensions != nullptr) {
        extKeys = localeExtensions->GetKeys();
    }
    if (extKeys != nullptr) {
        FOR_EACH(IChar*, key, IChar::Probe, extKeys) {
            AutoPtr<Extension> e = localeExtensions->GetExtension(CoreUtils::Unbox(key));
            CoclassID cid;
            e->GetCoclassID(cid);
            if (cid == CID_UnicodeLocaleExtension) {
                UnicodeLocaleExtension* ue = (UnicodeLocaleExtension*)e.Get();
                AutoPtr<ISet> uattributes = ue->GetUnicodeLocaleAttributes();
                FOR_EACH(ICharSequence*, uatr, ICharSequence::Probe, uattributes) {
                    if (mUattributes == nullptr) {
                        CHashSet::New(4, IID_ISet, (IInterface**)&mUattributes);
                    }
                    AutoPtr<CaseInsensitiveString> attr = new CaseInsensitiveString(CoreUtils::Unbox(uatr));
                    mUattributes->Add((IObject*)attr.Get());
                } END_FOR_EACH();
                AutoPtr<ISet> ukeys = ue->GetUnicodeLocaleKeys();
                FOR_EACH(ICharSequence*, ukey, ICharSequence::Probe, ukeys) {
                    if (mUkeywords == nullptr) {
                        CHashMap::New(4, IID_IMap, (IInterface**)&mUkeywords);
                    }
                    AutoPtr<CaseInsensitiveString> key =  new CaseInsensitiveString(CoreUtils::Unbox(ukey));
                    mUkeywords->Put((IObject*)key.Get(),
                            CoreUtils::Box(ue->GetUnicodeLocaleType(CoreUtils::Unbox(ukey))));
                } END_FOR_EACH();
            }
            else {
                if (mExtensions == nullptr) {
                    CHashMap::New(4, IID_IMap, (IInterface**)&mExtensions);
                }
                AutoPtr<CaseInsensitiveChar> extKey = new CaseInsensitiveChar(CoreUtils::Unbox(key));
                mExtensions->Put((IObject*)extKey.Get(), CoreUtils::Box(e->GetValue()));
            }
        } END_FOR_EACH();
    }
    return NOERROR;
}

ECode InternalLocaleBuilder::Clear()
{
    mLanguage = "";
    mScript = "";
    mRegion = "";
    mVariant = "";
    ClearExtensions();
    return NOERROR;
}

ECode InternalLocaleBuilder::ClearExtensions()
{
    if (mExtensions != nullptr) {
        mExtensions->Clear();
    }
    if (mUattributes != nullptr) {
        mUattributes->Clear();
    }
    if (mUkeywords != nullptr) {
        mUkeywords->Clear();
    }
    return NOERROR;
}

AutoPtr<BaseLocale> InternalLocaleBuilder::GetBaseLocale()
{
    String language = mLanguage;
    String script = mScript;
    String region = mRegion;
    String variant = mVariant;

    if (mExtensions != nullptr) {
        AutoPtr<IInterface> v;
        mExtensions->Get((IObject*)GetPRIVATEUSE_KEY().Get(), v);
        String privuse = CoreUtils::Unbox(ICharSequence::Probe(v));
        if (!privuse.IsNull()) {
            AutoPtr<StringTokenIterator> itr = new StringTokenIterator();
            itr->Constructor(privuse, ILanguageTag::SEP);
            Boolean sawPrefix = false;
            Integer privVarStart = -1;
            while (!itr->IsDone()) {
                if (sawPrefix) {
                    privVarStart = itr->CurrentStart();
                    break;
                }
                if (LocaleUtils::CaseIgnoreMatch(itr->Current(), ILanguageTag::PRIVUSE_VARIANT_PREFIX)) {
                    sawPrefix = true;
                }
                itr->Next();
            }
            if (privVarStart != -1) {
                AutoPtr<IStringBuilder> sb;
                CStringBuilder::New(variant, IID_IStringBuilder, (IInterface**)&sb);
                Integer size;
                if (sb->GetLength(size), size != 0) {
                    sb->Append(BaseLocale::SEP);
                }
                sb->Append(privuse.Substring(privVarStart).Replace(ILanguageTag::SEP, BaseLocale::SEP));
                sb->ToString(variant);
            }
        }
    }

    return BaseLocale::GetInstance(language, script, region, variant);
}

AutoPtr<LocaleExtensions> InternalLocaleBuilder::GetLocaleExtensions()
{
    if (LocaleUtils::IsEmpty(mExtensions) && LocaleUtils::IsEmpty(mUattributes) &&
            LocaleUtils::IsEmpty(mUkeywords)) {
        return nullptr;
    }

    AutoPtr<LocaleExtensions> lext = new LocaleExtensions(mExtensions, mUattributes, mUkeywords);
    return lext->IsEmpty() ? nullptr : lext;
}

String InternalLocaleBuilder::RemovePrivateuseVariant(
    /* [in] */ const String& privuseVal)
{
    AutoPtr<StringTokenIterator> itr = new StringTokenIterator();
    itr->Constructor(privuseVal, ILanguageTag::SEP);

    // Note: privateuse value "abc-lvariant" is unchanged
    // because no subtags after "lvariant".

    Integer prefixStart = -1;
    Boolean sawPrivuseVar = false;
    while (!itr->IsDone()) {
        if (prefixStart != -1) {
            // Note: privateuse value "abc-lvariant" is unchanged
            // because no subtags after "lvariant".
            sawPrivuseVar = true;
            break;
        }
        if (LocaleUtils::CaseIgnoreMatch(itr->Current(), ILanguageTag::PRIVUSE_VARIANT_PREFIX)) {
            prefixStart = itr->CurrentStart();
        }
        itr->Next();
    }
    if (!sawPrivuseVar) {
        return privuseVal;
    }

    CHECK(prefixStart == 0 || prefixStart > 1);
    return prefixStart == 0 ? String() : privuseVal.Substring(0, prefixStart - 1);
}

Integer InternalLocaleBuilder::CheckVariants(
    /* [in] */ const String& variants,
    /* [in] */ const String& sep)
{
    AutoPtr<StringTokenIterator> itr = new StringTokenIterator();
    itr->Constructor(variants, sep);
    while (!itr->IsDone()) {
        String s = itr->Current();
        if (!LanguageTag::IsVariant(s)) {
            return itr->CurrentStart();
        }
        itr->Next();
    }
    return -1;
}

void InternalLocaleBuilder::SetUnicodeLocaleExtension(
    /* [in] */ const String& subtags)
{
    if (mUattributes != nullptr) {
        mUattributes->Clear();
    }
    if (mUkeywords != nullptr) {
        mUkeywords->Clear();
    }

    AutoPtr<StringTokenIterator> itr = new StringTokenIterator();
    itr->Constructor(subtags, ILanguageTag::SEP);

    while (!itr->IsDone()) {
        if (!UnicodeLocaleExtension::IsAttribute(itr->Current())) {
            break;
        }
        if (mUattributes == nullptr) {
            CHashSet::New(4, IID_ISet, (IInterface**)&mUattributes);
        }
        AutoPtr<CaseInsensitiveString> attr = new CaseInsensitiveString(itr->Current());
        itr->Next();
    }

    AutoPtr<CaseInsensitiveString> key;
    String type;
    Integer typeStart = -1;
    Integer typeEnd = -1;
    while (!itr->IsDone()) {
        if (key != nullptr) {
            if (UnicodeLocaleExtension::IsKey(itr->Current())) {
                // next keyword - emit previous one
                CHECK(typeStart == -1 || typeEnd != -1);
                type = (typeStart == -1) ? String("") : subtags.Substring(typeStart, typeEnd);
                if (mUkeywords == nullptr) {
                    CHashMap::New(4, IID_IMap, (IInterface**)&mUkeywords);
                }
                mUkeywords->Put((IObject*)key.Get(), CoreUtils::Box(type));

                // reset keyword info
                AutoPtr<CaseInsensitiveString> tmpKey = new CaseInsensitiveString(itr->Current());
                Boolean contained;
                mUkeywords->ContainsKey((IObject*)tmpKey.Get(), contained);
                key = contained ? nullptr : tmpKey;
                typeStart = typeEnd = -1;
            }
            else {
                if (typeStart == -1) {
                    typeStart = itr->CurrentStart();
                }
                typeEnd = itr->CurrentEnd();
            }

        }
        else if (UnicodeLocaleExtension::IsKey(itr->Current())) {
            // 1. first keyword or
            // 2. next keyword, but previous one was duplicate
            key = new CaseInsensitiveString(itr->Current());
            Boolean contained;
            if (mUkeywords != nullptr &&
                    (mUkeywords->ContainsKey((IObject*)key.Get(), contained), contained)) {
                // duplicate
                key = nullptr;
            }
        }

        if (!itr->HasNext()) {
            if (key != nullptr) {
                // last keyword
                CHECK(typeStart == -1 || typeEnd != -1);
                type = (typeStart == -1) ? String("") : subtags.Substring(typeStart, typeEnd);
                if (mUkeywords == nullptr) {
                    CHashMap::New(4, IID_IMap, (IInterface**)&mUkeywords);
                }
                mUkeywords->Put((IObject*)key.Get(), CoreUtils::Box(type));
            }
            break;
        }

        itr->Next();
    }
}

//-----------------------------------------------------------------------------------------

extern const CoclassID CID_CaseInsensitiveString =
        {{0xbb82f417,0xf166,0x45df,0xb91f,{0x1a,0xbe,0xfa,0xa2,0xce,0x50}}, &CID_libcore};

InternalLocaleBuilder::CaseInsensitiveString::CaseInsensitiveString(
    /* [in] */ const String& s)
    : mStr(s)
{
    mLowerStr = LocaleUtils::ToLowerString(s);
}

ECode InternalLocaleBuilder::CaseInsensitiveString::GetHashCode(
    /* [out] */ Integer& hash)
{
    hash = mLowerStr.GetHashCode();
    return NOERROR;
}

ECode InternalLocaleBuilder::CaseInsensitiveString::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean& same)
{
    if (!Object::InstanceOf(obj, CID_CaseInsensitiveString)) {
        same = false;
        return NOERROR;
    }

    CaseInsensitiveString* other = (CaseInsensitiveString*)IObject::Probe(obj);
    if (other == this){
        same = true;
        return NOERROR;
    }
    same = mLowerStr.Equals(other->mLowerStr);
    return NOERROR;
}

ECode InternalLocaleBuilder::CaseInsensitiveString::GetCoclassID(
    /* [out] */ CoclassID& cid)
{
    cid = CID_CaseInsensitiveString;
    return NOERROR;
}

//-----------------------------------------------------------------------------------------

extern const CoclassID CID_CaseInsensitiveChar =
        {{0x5b2d4ebd,0xef24,0x47d0,0x9235,{0xe6,0x55,0x58,0xab,0x91,0x94}}, &CID_libcore};

InternalLocaleBuilder::CaseInsensitiveChar::CaseInsensitiveChar(
    /* [in] */ const String& s)
    : CaseInsensitiveChar(s.GetChar(0))
{}

InternalLocaleBuilder::CaseInsensitiveChar::CaseInsensitiveChar(
    /* [in] */ Char c)
    : mCh(c)
{
    mLowerCh = LocaleUtils::ToLower(c);
}

ECode InternalLocaleBuilder::CaseInsensitiveChar::GetHashCode(
    /* [out] */ Integer& hash)
{
    hash = mLowerCh;
    return NOERROR;
}

ECode InternalLocaleBuilder::CaseInsensitiveChar::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean& same)
{
    if (!Object::InstanceOf(obj, CID_CaseInsensitiveChar)) {
        same = false;
        return NOERROR;
    }

    CaseInsensitiveChar* other = (CaseInsensitiveChar*)IObject::Probe(obj);
    if (other == this){
        same = true;
        return NOERROR;
    }
    same = mLowerCh == other->mLowerCh;
    return NOERROR;
}

ECode InternalLocaleBuilder::CaseInsensitiveChar::GetCoclassID(
    /* [out] */ CoclassID& cid)
{
    cid = CID_CaseInsensitiveChar;
    return NOERROR;
}

}
}
}
