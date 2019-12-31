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

#include "ccm/core/CArray.h"
#include "ccm/core/CoreUtils.h"
#include "ccm/core/CStringBuilder.h"
#include "ccm/util/CArrayList.h"
#include "ccm/util/CHashMap.h"
#include "ccm/util/locale/LanguageTag.h"
#include "ccm.core.IArray.h"
#include "ccm.core.IChar.h"
#include "ccm.core.ICharSequence.h"
#include "ccm.util.IMap.h"

using ccm::core::CArray;
using ccm::core::CoreUtils;
using ccm::core::CStringBuilder;
using ccm::core::IArray;
using ccm::core::IChar;
using ccm::core::ICharSequence;
using ccm::core::IID_IArray;
using ccm::core::IID_ICharSequence;
using ccm::core::IStringBuilder;
using ccm::core::IID_IStringBuilder;

namespace ccm {
namespace util {
namespace locale {

CCM_INTERFACE_IMPL_1(LanguageTag, SyncObject, ILanguageTag);

static void PutStringArray(
    /* [in] */ IMap* map,
    /* [in] */ const String& tag,
    /* [in] */ const String& prefered)
{
    AutoPtr<ICharSequence> key = CoreUtils::Box(tag);
    AutoPtr<ICharSequence> value = CoreUtils::Box(prefered);
    AutoPtr<IArray> strArray;
    CArray::New(IID_ICharSequence, 2, IID_IArray, (IInterface**)&strArray);
    strArray->Set(0, key);
    strArray->Set(1, value);
    map->Put(key, value);
}

static AutoPtr<IMap> CreateGRANDFATHERED()
{
    AutoPtr<IMap> map;
    CHashMap::New(IID_IMap, (IInterface**)&map);
    PutStringArray(map, String("art-lojban"), String("jbo"));
    PutStringArray(map, String("cel-gaulish"), String("xtg-x-cel-gaulish"));   // fallback
    PutStringArray(map, String("en-GB-oed"), String("en-GB-x-oed"));         // fallback
    PutStringArray(map, String("i-ami"), String("ami"));
    PutStringArray(map, String("i-bnn"), String("bnn"));
    PutStringArray(map, String("i-default"), String("en-x-i-default"));      // fallback
    PutStringArray(map, String("i-enochian"), String("und-x-i-enochian"));    // fallback
    PutStringArray(map, String("i-hak"), String("hak"));
    PutStringArray(map, String("i-klingon"), String("tlh"));
    PutStringArray(map, String("i-lux"), String("lb"));
    PutStringArray(map, String("i-mingo"), String("see-x-i-mingo"));       // fallback
    PutStringArray(map, String("i-navajo"), String("nv"));
    PutStringArray(map, String("i-pwn"), String("pwn"));
    PutStringArray(map, String("i-tao"), String("tao"));
    PutStringArray(map, String("i-tay"), String("tay"));
    PutStringArray(map, String("i-tsu"), String("tsu"));
    PutStringArray(map, String("no-bok"), String("nb"));
    PutStringArray(map, String("no-nyn"), String("nn"));
    PutStringArray(map, String("sgn-BE-FR"), String("sfb"));
    PutStringArray(map, String("sgn-BE-NL"), String("vgt"));
    PutStringArray(map, String("sgn-CH-DE"), String("sgg"));
    PutStringArray(map, String("zh-guoyu"), String("cmn"));
    PutStringArray(map, String("zh-hakka"), String("hak"));
    PutStringArray(map, String("zh-min"), String("nan-x-zh-min"));        // fallback
    PutStringArray(map, String("zh-min-nan"), String("nan"));
    PutStringArray(map, String("zh-xiang"), String("hsn"));
    return map;
}

AutoPtr<IMap> LanguageTag::GetGRANDFATHERED()
{
    static AutoPtr<IMap> GRANDFATHERED = CreateGRANDFATHERED();
    return GRANDFATHERED;
}

AutoPtr<ILanguageTag> LanguageTag::Parse(
    /* [in] */ const String& languageTag,
    /* [in] */ ParseStatus* sts)
{
    if (sts == nullptr) {
        sts = new ParseStatus();
    }
    else {
        sts->Reset();
    }

    AutoPtr<StringTokenIterator> itr;

    AutoPtr<IArray> gfmap;
    GetGRANDFATHERED()->Get(CoreUtils::Box(
            LocaleUtils::ToLowerString(languageTag)), (IInterface**)&gfmap);
    if (gfmap != nullptr) {
        AutoPtr<ICharSequence> text;
        gfmap->Get(1, (IInterface**)&text);
        itr = new StringTokenIterator();
        itr->Constructor(CoreUtils::Unbox(text), SEP);
    }
    else {
        itr = new StringTokenIterator();
        itr->Constructor(languageTag, SEP);
    }

    AutoPtr<LanguageTag> tag = new LanguageTag();

    if (tag->ParseLanguage(itr, sts)) {
        tag->ParseExtlangs(itr, sts);
        tag->ParseScript(itr, sts);
        tag->ParseRegion(itr, sts);
        tag->ParseVariants(itr, sts);
        tag->ParseExtensions(itr, sts);
    }
    tag->ParsePrivateuse(itr, sts);

    if (!itr->IsDone() && !sts->IsError()) {
        String s = itr->Current();
        sts->mErrorIndex = itr->CurrentStart();
        if (s.GetByteLength() == 0) {
            sts->mErrorMsg = "Empty subtag";
        }
        else {
            sts->mErrorMsg = String::Format("Invalid subtag: %s", s.string());
        }
    }

    return tag.Get();
}

Boolean LanguageTag::ParseLanguage(
    /* [in] */ StringTokenIterator* itr,
    /* [in] */ ParseStatus* sts)
{
    if (itr->IsDone() || sts->IsError()) {
        return false;
    }

    Boolean found = false;

    String s = itr->Current();
    if (IsLanguage(s)) {
        found = true;
        mLanguage = s;
        sts->mParseLength = itr->CurrentEnd();
        itr->Next();
    }

    return found;
}

Boolean LanguageTag::ParseExtlangs(
    /* [in] */ StringTokenIterator* itr,
    /* [in] */ ParseStatus* sts)
{
    if (itr->IsDone() || sts->IsError()) {
        return false;
    }

    Boolean found = false;

    while (!itr->IsDone()) {
        String s = itr->Current();
        if (!IsExtlang(s)) {
            break;
        }
        found = true;
        Boolean empty;
        if (mExtlangs->IsEmpty(&empty), empty) {
            mExtlangs = nullptr;
            CArrayList::New(3, IID_IList, (IInterface**)&mExtlangs);
        }
        mExtlangs->Add(CoreUtils::Box(s));
        sts->mParseLength = itr->CurrentEnd();
        itr->Next();

        Integer size;
        if (mExtlangs->GetSize(&size), size == 3) {
            // Maximum 3 extlangs
            break;
        }
    }

    return found;
}

Boolean LanguageTag::ParseScript(
    /* [in] */ StringTokenIterator* itr,
    /* [in] */ ParseStatus* sts)
{
    if (itr->IsDone() || sts->IsError()) {
        return false;
    }

    Boolean found = false;

    String s = itr->Current();
    if (IsScript(s)) {
        found = true;
        mScript = s;
        sts->mParseLength = itr->CurrentEnd();
        itr->Next();
    }

    return found;
}

Boolean LanguageTag::ParseRegion(
    /* [in] */ StringTokenIterator* itr,
    /* [in] */ ParseStatus* sts)
{
    if (itr->IsDone() || sts->IsError()) {
        return false;
    }

    Boolean found = false;

    String s = itr->Current();
    if (IsRegion(s)) {
        found = true;
        mRegion = s;
        sts->mParseLength = itr->CurrentEnd();
        itr->Next();
    }

    return found;
}

Boolean LanguageTag::ParseVariants(
    /* [in] */ StringTokenIterator* itr,
    /* [in] */ ParseStatus* sts)
{
    if (itr->IsDone() || sts->IsError()) {
        return false;
    }

    Boolean found = false;

    while (!itr->IsDone()) {
        String s = itr->Current();
        if (!IsVariant(s)) {
            break;
        }
        found = true;
        Boolean empty;
        if (mVariants->IsEmpty(&empty), empty) {
            mVariants = nullptr;
            CArrayList::New(3, IID_IList, (IInterface**)&mVariants);
        }
        mVariants->Add(CoreUtils::Box(s));
        sts->mParseLength = itr->CurrentEnd();
        itr->Next();
    }

    return found;
}

Boolean LanguageTag::ParseExtensions(
    /* [in] */ StringTokenIterator* itr,
    /* [in] */ ParseStatus* sts)
{
    if (itr->IsDone() || sts->IsError()) {
        return false;
    }

    Boolean found = false;

    while (!itr->IsDone()) {
        String s = itr->Current();
        if (IsExtensionSingleton(s)) {
            Integer start = itr->CurrentStart();
            String singleton = s;
            AutoPtr<IStringBuilder> sb;
            CStringBuilder::New(singleton, IID_IStringBuilder, (IInterface**)&sb);

            itr->Next();
            while (!itr->IsDone()) {
                s = itr->Current();
                if (IsExtensionSubtag(s)) {
                    sb->Append(SEP);
                    sb->Append(s);
                    sts->mParseLength = itr->CurrentEnd();
                }
                else {
                    break;
                }
                itr->Next();
            }

            if (sts->mParseLength <= start) {
                sts->mErrorIndex = start;
                sts->mErrorMsg = String::Format("Incomplete extension '%s'", singleton.string());
                break;
            }

            Boolean empty;
            if (mExtensions->IsEmpty(&empty), empty) {
                mExtensions = nullptr;
                CArrayList::New(4, IID_IList, (IInterface**)&mExtensions);
            }
            String str;
            sb->ToString(&str);
            mExtensions->Add(CoreUtils::Box(str));
            found = true;
        }
        else {
            break;
        }
    }
    return found;
}

Boolean LanguageTag::ParsePrivateuse(
    /* [in] */ StringTokenIterator* itr,
    /* [in] */ ParseStatus* sts)
{
    if (itr->IsDone() || sts->IsError()) {
        return false;
    }

    Boolean found = false;

    String s = itr->Current();
    if (IsPrivateusePrefix(s)) {
        Integer start = itr->CurrentStart();
        AutoPtr<IStringBuilder> sb;
        CStringBuilder::New(s, IID_IStringBuilder, (IInterface**)&sb);

        itr->Next();
        while (!itr->IsDone()) {
            s = itr->Current();
            if (!IsPrivateuseSubtag(s)) {
                break;
            }
            sb->Append(SEP);
            sb->Append(s);
            sts->mParseLength = itr->CurrentEnd();

            itr->Next();
        }

        if (sts->mParseLength <= start) {
            // need at least 1 private subtag
            sts->mErrorIndex = start;
            sts->mErrorMsg = "Incomplete privateuse";
        }
        else {
            sb->ToString(&mPrivateuse);
            found = true;
        }
    }

    return found;
}

AutoPtr<ILanguageTag> LanguageTag::ParseLocale(
    /* [in] */ BaseLocale* baseLocale,
    /* [in] */ LocaleExtensions* localeExtensions)
{
    AutoPtr<LanguageTag> tag = new LanguageTag();

    String language = baseLocale->GetLanguage();
    String script = baseLocale->GetScript();
    String region = baseLocale->GetRegion();
    String variant = baseLocale->GetVariant();

    Boolean hasSubtag = false;

    String privuseVar;

    if (IsLanguage(language)) {
        // Convert a deprecated language code to its new code
        if (language.Equals("iw")) {
            language = "he";
        }
        else if (language.Equals("ji")) {
            language = "yi";
        }
        else if (language.Equals("in")) {
            language = "id";
        }
        tag->mLanguage = language;
    }

    if (IsScript(script)) {
        tag->mScript = CanonicalizeScript(script);
        hasSubtag = true;
    }

    if (IsRegion(region)) {
        tag->mRegion = CanonicalizeRegion(region);
        hasSubtag = true;
    }

    // Special handling for no_NO_NY - use nn_NO for language tag
    if (tag->mLanguage.Equals("no") && tag->mRegion.Equals("NO") && variant.Equals("NY")) {
        tag->mLanguage = "nn";
        variant = "";
    }

    if (variant.GetByteLength() > 0) {
        AutoPtr<IList> variants;
        AutoPtr<StringTokenIterator> varitr = new StringTokenIterator();
        varitr->Constructor(variant, BaseLocale::SEP);
        while (!varitr->IsDone()) {
            String var = varitr->Current();
            if (!IsVariant(var)) {
                break;
            }
            if (variants == nullptr) {
                CArrayList::New(IID_IList, (IInterface**)&variants);
            }
            variants->Add(CoreUtils::Box(var));
            varitr->Next();
        }
        if (variants != nullptr) {
            tag->mVariants = variants;
            hasSubtag = true;
        }
        if (!varitr->IsDone()) {
            // ill-formed variant subtags
            AutoPtr<IStringBuilder> buf;
            CStringBuilder::New(IID_IStringBuilder, (IInterface**)&buf);
            while (!varitr->IsDone()) {
                String prvv = varitr->Current();
                if (!IsPrivateuseSubtag(prvv)) {
                    // cannot use private use subtag - truncated
                    break;
                }
                Integer len;
                if (buf->GetLength(&len), len > 0) {
                    buf->Append(SEP);
                }
                buf->Append(prvv);
                varitr->Next();
            }
            Integer len;
            if (buf->GetLength(&len), len > 0) {
                buf->ToString(&privuseVar);
            }
        }
    }

    AutoPtr<IList> extensions;
    String privateuse;

    if (localeExtensions != nullptr) {
        AutoPtr<ISet> locextKeys = localeExtensions->GetKeys();
        AutoPtr<IIterator> it;
        locextKeys->GetIterator(&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IChar> locextKey;
            it->Next((IInterface**)&locextKey);
            AutoPtr<Extension> ext = localeExtensions->GetExtension(
                    CoreUtils::Unbox(locextKey));
            if (IsPrivateusePrefixChar(CoreUtils::Unbox(locextKey))) {
                privateuse = ext->GetValue();
            }
            else {
                if (extensions == nullptr) {
                    CArrayList::New(IID_IList, (IInterface**)&extensions);
                }
                extensions->Add(CoreUtils::Box(String::Format("%c%s%s",
                        CoreUtils::Unbox(locextKey), SEP.string(), ext->GetValue().string())));
            }
        }
    }

    if (extensions != nullptr) {
        tag->mExtensions = extensions;
        hasSubtag = true;
    }

    // append ill-formed variant subtags to private use
    if (!privuseVar.IsNull()) {
        if (privateuse.IsNull()) {
            privateuse = PRIVUSE_VARIANT_PREFIX + SEP + privuseVar;
        }
        else {
            privateuse = privateuse + SEP + PRIVUSE_VARIANT_PREFIX
                         + SEP + privuseVar.Replace(BaseLocale::SEP, SEP);
        }
    }

    if (!privateuse.IsNull()) {
        tag->mPrivateuse = privateuse;
    }

    if (tag->mLanguage.GetByteLength() == 0 && (hasSubtag || privateuse.IsNull())) {
        // use lang "und" when 1) no language is available AND
        // 2) any of other subtags other than private use are available or
        // no private use tag is available
        tag->mLanguage = UNDETERMINED;
    }

    return tag.Get();
}

ECode LanguageTag::GetLanguage(
    /* [out] */ String* language)
{
    VALIDATE_NOT_NULL(language);

    *language = mLanguage;
    return NOERROR;
}

ECode LanguageTag::GetExtlangs(
    /* [out] */ IList** extlangs)
{
    VALIDATE_NOT_NULL(extlangs);

    Boolean empty;
    if (mExtlangs->IsEmpty(&empty), empty) {
        *extlangs = mExtlangs;
        REFCOUNT_ADD(*extlangs);
        return NOERROR;
    }
    Collections::CreateUnmodifiableList(mExtlangs).MoveTo(extlangs);
    return NOERROR;
}

ECode LanguageTag::GetScript(
    /* [out] */ String* script)
{
    VALIDATE_NOT_NULL(script);

    *script = mScript;
    return NOERROR;
}

ECode LanguageTag::GetRegion(
    /* [out] */ String* region)
{
    VALIDATE_NOT_NULL(region);

    *region = mRegion;
    return NOERROR;
}

ECode LanguageTag::GetVariants(
    /* [out] */ IList** variants)
{
    VALIDATE_NOT_NULL(variants);

    Boolean empty;
    if (mVariants->IsEmpty(&empty), empty) {
        *variants = mVariants;
        REFCOUNT_ADD(*variants);
        return NOERROR;
    }
    Collections::CreateUnmodifiableList(mVariants).MoveTo(variants);
    return NOERROR;
}

ECode LanguageTag::GetExtensions(
    /* [out] */ IList** extensions)
{
    VALIDATE_NOT_NULL(extensions);

    Boolean empty;
    if (mExtensions->IsEmpty(&empty), empty) {
        *extensions = mExtensions;
        REFCOUNT_ADD(*extensions);
        return NOERROR;
    }
    Collections::CreateUnmodifiableList(mExtensions).MoveTo(extensions);
    return NOERROR;
}

ECode LanguageTag::GetPrivateuse(
    /* [out] */ String* privateuse)
{
    VALIDATE_NOT_NULL(privateuse);

    *privateuse = mPrivateuse;
    return NOERROR;
}

Boolean LanguageTag::IsLanguage(
    /* [in] */ const String& s)
{
    // language      = 2*3ALPHA            ; shortest ISO 639 code
    //                 ["-" extlang]       ; sometimes followed by
    //                                     ;   extended language subtags
    //               / 4ALPHA              ; or reserved for future use
    //               / 5*8ALPHA            ; or registered language subtag
    Integer len = s.GetByteLength();
    return (len >= 2) && (len <= 8) && LocaleUtils::IsAlphaString(s);
}

Boolean LanguageTag::IsExtlang(
    /* [in] */ const String& s)
{
    // extlang       = 3ALPHA              ; selected ISO 639 codes
    //                 *2("-" 3ALPHA)      ; permanently reserved
    return (s.GetByteLength() == 3) && LocaleUtils::IsAlphaString(s);
}

Boolean LanguageTag::IsScript(
    /* [in] */ const String& s)
{
    // script        = 4ALPHA              ; ISO 15924 code
    return (s.GetByteLength() == 4) && LocaleUtils::IsAlphaString(s);
}

Boolean LanguageTag::IsRegion(
    /* [in] */ const String& s)
{
    // region        = 2ALPHA              ; ISO 3166-1 code
    //               / 3DIGIT              ; UN M.49 code
    return ((s.GetByteLength() == 2) && LocaleUtils::IsAlphaString(s))
            || ((s.GetByteLength() == 3) && LocaleUtils::IsNumericString(s));
}

Boolean LanguageTag::IsVariant(
    /* [in] */ const String& s)
{
    // variant       = 5*8alphanum         ; registered variants
    //               / (DIGIT 3alphanum)
    Integer len = s.GetByteLength();
    if (len >= 5 && len <= 8) {
        return LocaleUtils::IsAlphaNumericString(s);
    }
    if (len == 4) {
        return LocaleUtils::IsNumeric(s.string()[0])
                && LocaleUtils::IsAlphaNumeric(s.string()[1])
                && LocaleUtils::IsAlphaNumeric(s.string()[2])
                && LocaleUtils::IsAlphaNumeric(s.string()[3]);
    }
    return false;
}

Boolean LanguageTag::IsExtensionSingleton(
    /* [in] */ const String& s)
{
    // singleton     = DIGIT               ; 0 - 9
    //               / %x41-57             ; A - W
    //               / %x59-5A             ; Y - Z
    //               / %x61-77             ; a - w
    //               / %x79-7A             ; y - z

    return (s.GetByteLength() == 1)
            && LocaleUtils::IsAlphaString(s)
            && !LocaleUtils::CaseIgnoreMatch(PRIVATEUSE, s);
}

Boolean LanguageTag::IsExtensionSingletonChar(
    /* [in] */ Char c)
{
    return IsExtensionSingleton(String::ValueOf(c));
}

Boolean LanguageTag::IsExtensionSubtag(
    /* [in] */ const String& s)
{
    // extension     = singleton 1*("-" (2*8alphanum))
    Integer len = s.GetByteLength();
    return (len >= 2) && (len <= 8) && LocaleUtils::IsAlphaNumericString(s);
}

Boolean LanguageTag::IsPrivateusePrefix(
    /* [in] */ const String& s)
{
   // privateuse    = "x" 1*("-" (1*8alphanum))
    return (s.GetByteLength() == 1)
            && LocaleUtils::CaseIgnoreMatch(PRIVATEUSE, s);
}

Boolean LanguageTag::IsPrivateusePrefixChar(
    /* [in] */ Char c)
{
    return LocaleUtils::CaseIgnoreMatch(PRIVATEUSE, String::ValueOf(c));
}

Boolean LanguageTag::IsPrivateuseSubtag(
    /* [in] */ const String& s)
{
    // privateuse    = "x" 1*("-" (1*8alphanum))
    Integer len = s.GetByteLength();
    return (len >= 1) && (len <= 8) && LocaleUtils::IsAlphaNumericString(s);
}

ECode LanguageTag::ToString(
    /* [out] */ String* desc)
{
    VALIDATE_NOT_NULL(desc);

    AutoPtr<IStringBuilder> sb;
    CStringBuilder::New(IID_IStringBuilder, (IInterface**)&sb);

    if (mLanguage.GetByteLength() > 0) {
        sb->Append(mLanguage);

        AutoPtr<IIterator> it;
        mExtlangs->GetIterator(&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<ICharSequence> extlang;
            it->Next((IInterface**)&extlang);
            sb->Append(SEP);
            sb->Append(CoreUtils::Unbox(extlang));
        }

        if (mScript.GetByteLength() > 0) {
            sb->Append(SEP);
            sb->Append(mScript);
        }

        if (mRegion.GetByteLength() > 0) {
            sb->Append(SEP);
            sb->Append(mRegion);
        }

        it = nullptr;
        mVariants->GetIterator(&it);
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<ICharSequence> variant;
            it->Next((IInterface**)&variant);
            sb->Append(SEP);
            sb->Append(CoreUtils::Unbox(variant));
        }

        it = nullptr;
        mExtensions->GetIterator(&it);
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<ICharSequence> extension;
            it->Next((IInterface**)&extension);
            sb->Append(SEP);
            sb->Append(CoreUtils::Unbox(extension));
        }
    }
    if (mPrivateuse.GetByteLength() > 0) {
        Integer len;
        if (sb->GetLength(&len), len > 0) {
            sb->Append(SEP);
        }
        sb->Append(mPrivateuse);
    }

    return sb->ToString(desc);
}

}
}
}
