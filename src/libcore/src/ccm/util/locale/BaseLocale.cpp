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

#include "ccm/core/CStringBuilder.h"
#include "ccm/core/NativeAtomic.h"
#include "ccm/util/locale/BaseLocale.h"
#include "ccm/util/locale/LocaleUtils.h"
#include "ccm.core.IStringBuilder.h"

using ccm::core::CStringBuilder;
using ccm::core::IID_IComparable;
using ccm::core::IStringBuilder;
using ccm::core::IID_IStringBuilder;

namespace ccm {
namespace util {
namespace locale {

static const InterfaceID IID_BaseLocale =
        {{0xd4f5d57e,0x3ac4,0x4289,0xbda9,{0x7,0x8,0x0,0xf,0x4,0x0,0x9,0x7,0xa,0xc,0x6,0x5}}, &CID_libcore};

const String BaseLocale::SEP("_");

BaseLocale::Cache* BaseLocale::Get_CACHE()
{
    static AutoPtr<Cache> CACHE = new Cache();
    return CACHE;
}

BaseLocale::BaseLocale(
    /* [in] */ const String& language,
    /* [in] */ const String& region)
    : mLanguage(language)
    , mScript("")
    , mRegion(region)
    , mVariant("")
{}

BaseLocale::BaseLocale(
    /* [in] */ const String& language,
    /* [in] */ const String& script,
    /* [in] */ const String& region,
    /* [in] */ const String& variant)
{
    mLanguage = !language.IsNull() ? LocaleUtils::ToLowerString(language) : String("");
    mScript = !script.IsNull() ? LocaleUtils::ToTitleString(script) : String("");
    mRegion = !region.IsNull() ? LocaleUtils::ToUpperString(region) : String("");
    mVariant = !variant.IsNull() ? variant : String("");
}

IInterface* BaseLocale::Probe(
    /* [in] */ const InterfaceID& iid)
{
    if (iid == IID_BaseLocale) {
        return (IInterface*)(IObject*)this;
    }
    return Object::Probe(iid);
}

AutoPtr<BaseLocale> BaseLocale::CreateInstance(
    /* [in] */ const String& language,
    /* [in] */ const String& region)
{
    AutoPtr<BaseLocale> base = new BaseLocale(language, region);
    Get_CACHE()->Put((IComparable*)new Key(language, region), (IObject*)base.Get());
    return base;
}

AutoPtr<BaseLocale> BaseLocale::GetInstance(
    /* [in] */ const String& language_,
    /* [in] */ const String& script,
    /* [in] */ const String& region,
    /* [in] */ const String& variant)
{
    String language = language_;
    // Uses deprecated ISO639.1 language codes for he, yi and id
    if (!language.IsNull()) {
        if (LocaleUtils::CaseIgnoreMatch(language, String("he"))) {
            language = "iw";
        }
        else if (LocaleUtils::CaseIgnoreMatch(language, String("yi"))) {
            language = "ji";
        }
        else if (LocaleUtils::CaseIgnoreMatch(language, String("id"))) {
            language = "in";
        }
    }

    AutoPtr<Key> key = new Key(language, script, region, variant);
    AutoPtr<IInterface> value;
    Get_CACHE()->Get((IComparable*)key.Get(), (IInterface**)&value);
    return (BaseLocale*)IObject::Probe(value);
}

ECode BaseLocale::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* same)
{
    VALIDATE_NOT_NULL(same);

    if (IInterface::Equals((IObject*)this, obj)) {
        *same = true;
        return NOERROR;
    }
    if (obj->Probe(IID_BaseLocale) == nullptr) {
        *same = false;
        return NOERROR;
    }
    BaseLocale* oth = (BaseLocale*)IObject::Probe(obj);
    *same = mLanguage.Equals(oth->mLanguage) &&
            mScript.Equals(oth->mScript) &&
            mRegion.Equals(oth->mRegion) &&
            mVariant.Equals(oth->mVariant);
    return NOERROR;
}

ECode BaseLocale::ToString(
    /* [out] */ String* desc)
{
    VALIDATE_NOT_NULL(desc);

    AutoPtr<IStringBuilder> buf;
    CStringBuilder::New(IID_IStringBuilder, (IInterface**)&buf);
    if (mLanguage.GetByteLength() > 0) {
        buf->Append(String("language="));
        buf->Append(mLanguage);
    }
    if (mScript.GetByteLength() > 0) {
        Integer len;
        if (buf->GetLength(&len), len > 0) {
            buf->Append(String(", "));
        }
        buf->Append(String("script="));
        buf->Append(mScript);
    }
    if (mRegion.GetByteLength() > 0) {
        Integer len;
        if (buf->GetLength(&len), len > 0) {
            buf->Append(String(", "));
        }
        buf->Append(String("region="));
        buf->Append(mRegion);
    }
    if (mVariant.GetByteLength() > 0) {
        Integer len;
        if (buf->GetLength(&len), len > 0) {
            buf->Append(String(", "));
        }
        buf->Append(String("variant="));
        buf->Append(mVariant);
    }
    return buf->ToString(desc);
}

ECode BaseLocale::GetHashCode(
    /* [out] */ Integer* hash)
{
    VALIDATE_NOT_NULL(hash);

    VOLATILE_GET(Integer h, mHash);
    if (h == 0) {
        // Generating a hash value from language, script, region and variant
        h = mLanguage.GetHashCode();
        h = 31 * h + mScript.GetHashCode();
        h = 31 * h + mRegion.GetHashCode();
        h = 31 * h + mVariant.GetHashCode();
        VOLATILE_SET(mHash, h);
    }
    *hash = h;
    return NOERROR;
}

//-------------------------------------------------------------------------

static const InterfaceID IID_BaseLocaleKey =
        {{0x5d8edb21,0x2caf,0x425d,0x825b,{0x0,0xd,0x8,0xa,0x4,0x3,0x6,0x9,0x1,0x4,0xb,0x7}}, &CID_libcore};

Integer BaseLocale::Key::AddRef(
    /* [in] */ HANDLE id)
{
    return Object::AddRef(id);
}

Integer BaseLocale::Key::Release(
    /* [in] */ HANDLE id)
{
    return Object::Release(id);
}

IInterface* BaseLocale::Key::Probe(
    /* [in] */ const InterfaceID& iid)
{
    if (iid == IID_IInterface) {
        return (IInterface*)(IComparable*)this;
    }
    else if (iid == IID_IComparable) {
        return (IComparable*)this;
    }
    else if (iid == IID_BaseLocaleKey) {
        return (IComparable*)this;
    }
    return Object::Probe(iid);
}

ECode BaseLocale::Key::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    VALIDATE_NOT_NULL(iid);

    if (object == (IInterface*)(IComparable*)this) {
        *iid = IID_IComparable;
        return NOERROR;
    }
    return Object::GetInterfaceID(object, iid);
}

BaseLocale::Key::Key(
    /* [in] */ const String& language,
    /* [in] */ const String& region)
    : mLang(language)
    , mScrt("")
    , mRegn(region)
    , mVart("")
    , mNormalized(true)
{
    Integer h = language.GetHashCode();
    if (!region.Equals("")) {
        Integer len = region.GetByteLength();
        for (Integer i = 0; i < len; i++) {
            h = 31 * h + LocaleUtils::ToLower(region.string()[i]);
        }
    }
    mHash = h;
}

BaseLocale::Key::Key(
    /* [in] */ const String& language,
    /* [in] */ const String& script,
    /* [in] */ const String& region,
    /* [in] */ const String& variant,
    /* [in] */ Boolean normalized)
{
    Integer h = 0;
    if (!language.IsNull()) {
        mLang = language;
        Integer len = language.GetByteLength();
        for (Integer i = 0; i < len; i++) {
            h = 31 * h + LocaleUtils::ToLower(language.string()[i]);
        }
    }
    else {
        mLang = "";
    }
    if (!script.IsNull()) {
        mScrt = script;
        Integer len = script.GetByteLength();
        for (Integer i = 0; i < len; i++) {
            h = 31 * h + LocaleUtils::ToLower(script.string()[i]);
        }
    }
    else {
        mScrt = "";
    }
    if (!region.IsNull()) {
        mRegn = region;
        Integer len = region.GetByteLength();
        for (Integer i = 0; i < len; i++) {
            h = 31 * h + LocaleUtils::ToLower(region.string()[i]);
        }
    }
    else {
        mRegn = "";
    }
    if (!variant.IsNull()) {
        mVart = variant;
        Integer len = variant.GetByteLength();
        for (Integer i = 0; i < len; i++) {
            h = 31 * h + variant.string()[i];
        }
    }
    else {
        mVart = "";
    }
    mHash = h;
    mNormalized = normalized;
}

ECode BaseLocale::Key::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* same)
{
    VALIDATE_NOT_NULL(same);

    if (IInterface::Equals((IComparable*)this, obj)) {
        *same = true;
        return NOERROR;
    }

    if (obj->Probe(IID_BaseLocaleKey) != nullptr) {
        Key* oth = (Key*)IComparable::Probe(obj);
        *same = mHash == oth->mHash &&
                LocaleUtils::CaseIgnoreMatch(oth->mLang, mLang) &&
                LocaleUtils::CaseIgnoreMatch(oth->mScrt, mScrt) &&
                LocaleUtils::CaseIgnoreMatch(oth->mRegn, mRegn) &&
                oth->mVart.Equals(mVart);
        return NOERROR;
    }

    *same = false;
    return NOERROR;
}

ECode BaseLocale::Key::CompareTo(
    /* [in] */ IInterface* obj,
    /* [out] */ Integer* result)
{
    VALIDATE_NOT_NULL(result);

    Key* oth = (Key*)IComparable::Probe(obj);
    Integer res = LocaleUtils::CaseIgnoreCompare(mLang, oth->mLang);
    if (res == 0) {
        res = LocaleUtils::CaseIgnoreCompare(mScrt, oth->mScrt);
        if (res == 0) {
            res = LocaleUtils::CaseIgnoreCompare(mRegn, oth->mRegn);
            if (res == 0) {
                res = mVart.Compare(oth->mVart);
            }
        }
    }
    *result = res;
    return NOERROR;
}

ECode BaseLocale::Key::GetHashCode(
    /* [out] */ Integer* hash)
{
    VALIDATE_NOT_NULL(hash);

    *hash = mHash;
    return NOERROR;
}

AutoPtr<BaseLocale::Key> BaseLocale::Key::Normalize(
    /* [in] */ Key* key)
{
    if (key->mNormalized) {
        return key;
    }

    String lang = LocaleUtils::ToLowerString(key->mLang);
    String scrt = LocaleUtils::ToTitleString(key->mScrt);
    String regn = LocaleUtils::ToUpperString(key->mRegn);
    String vart = key->mVart;

    return new Key(lang, scrt, regn, vart, true);
}

//-------------------------------------------------------------------------

AutoPtr<IInterface> BaseLocale::Cache::NormalizeKey(
    /* [in] */ IInterface* key)
{
    AutoPtr<Key> kObj = Key::Normalize((Key*)IComparable::Probe(key));
    return (IComparable*)kObj.Get();
}

AutoPtr<IInterface> BaseLocale::Cache::CreateObject(
    /* [in] */ IInterface* key)
{
    Key* kObj = (Key*)IComparable::Probe(key);
    return (IObject*) new BaseLocale(kObj->mLang, kObj->mScrt, kObj->mRegn, kObj->mVart);
}

}
}
}
