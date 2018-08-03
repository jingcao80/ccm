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

#include "ccm/util/locale/BaseLocale.h"
#include "ccm/util/locale/LocaleUtils.h"

using ccm::core::IID_IComparable;

namespace ccm {
namespace util {
namespace locale {

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

AutoPtr<BaseLocale> BaseLocale::CreateInstance(
    /* [in] */ const String& language,
    /* [in] */ const String& region)
{
    AutoPtr<BaseLocale> base = new BaseLocale(language, region);
    // Get_CACHE()->Put()
}

//-------------------------------------------------------------------------

CCM_INTERFACE_IMPL_1(BaseLocale::Key, Object, IComparable);

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

}
}
}
