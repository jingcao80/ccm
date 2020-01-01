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
#include "como/util/Collections.h"
#include "como/util/locale/UnicodeLocaleExtension.h"
#include "como.core.ICharSequence.h"
#include "como.core.IStringBuilder.h"
#include "como.util.IMapEntry.h"
#include "como.util.locale.ILanguageTag.h"

using como::core::CoreUtils;
using como::core::CStringBuilder;
using como::core::ICharSequence;
using como::core::IStringBuilder;
using como::core::IID_IStringBuilder;

namespace como {
namespace util {
namespace locale {

extern const CoclassID CID_UnicodeLocaleExtension =
        {{0x94c64925,0x5b95,0x400c,0xac19,{0x4,0xd,0x2,0x5,0x6,0x8,0x3,0x4,0x0,0xa,0x4,0x7}}, &CID_libcore};

AutoPtr<UnicodeLocaleExtension> UnicodeLocaleExtension::GetCA_JAPANESE()
{
    static AutoPtr<UnicodeLocaleExtension> CA_JAPANESE =
            new UnicodeLocaleExtension(String("ca"), String("japanese"));
    return CA_JAPANESE;
}

AutoPtr<UnicodeLocaleExtension> UnicodeLocaleExtension::GetNU_THAI()
{
    static AutoPtr<UnicodeLocaleExtension> NU_THAI =
            new UnicodeLocaleExtension(String("nu"), String("thai"));
    return NU_THAI;
}

UnicodeLocaleExtension::UnicodeLocaleExtension(
    /* [in] */ const String& key,
    /* [in] */ const String& value)
    : Extension(SINGLETON, key + "-" + value)
{
    mAttributes = Collections::GetEmptySet();
    mKeywords = Collections::CreateSingletonMap(
            CoreUtils::Box(key), CoreUtils::Box(value));
}

UnicodeLocaleExtension::UnicodeLocaleExtension(
    /* [in] */ ISortedSet* attributes,
    /* [in] */ ISortedMap* keywords)
    : Extension(SINGLETON)
{
    if (attributes != nullptr) {
        mAttributes = ISet::Probe(attributes);
    }
    else {
        mAttributes = Collections::GetEmptySet();
    }
    if (keywords != nullptr) {
        mKeywords = IMap::Probe(keywords);
    }
    else {
        mKeywords = Collections::GetEmptyMap();
    }

    Boolean empty;
    if ((mAttributes->IsEmpty(&empty), !empty) ||
            (mKeywords->IsEmpty(&empty), !empty)) {
        AutoPtr<IStringBuilder> sb;
        CStringBuilder::New(IID_IStringBuilder, (IInterface**)&sb);
        AutoPtr<IIterator> it;
        mAttributes->GetIterator(&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> attribute;
            it->Next(&attribute);
            sb->Append(ILanguageTag::SEP);
            sb->Append(ICharSequence::Probe(attribute));
        }
        AutoPtr<ISet> entries;
        mKeywords->GetEntrySet(&entries);
        it = nullptr;
        entries->GetIterator(&it);
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> keyword;
            it->Next(&keyword);
            AutoPtr<IInterface> key, value;
            IMapEntry::Probe(keyword)->GetKey(&key);
            IMapEntry::Probe(keyword)->GetValue(&value);

            sb->Append(ILanguageTag::SEP);
            sb->Append(ICharSequence::Probe(key));
            Integer len;
            if (ICharSequence::Probe(value)->GetLength(&len), len > 0) {
                sb->Append(ILanguageTag::SEP);
                sb->Append(ICharSequence::Probe(value));
            }

        }
        String str;
        sb->Substring(1, &str);
        SetValue(str); // skip leading '-'
    }
}

AutoPtr<ISet> UnicodeLocaleExtension::GetUnicodeLocaleAttributes()
{
    if (mAttributes == Collections::GetEmptySet()) {
        return mAttributes;
    }
    return Collections::CreateUnmodifiableSet(mAttributes);
}

AutoPtr<ISet> UnicodeLocaleExtension::GetUnicodeLocaleKeys()
{
    if (mKeywords == Collections::GetEmptyMap()) {
        return Collections::GetEmptySet();
    }
    AutoPtr<ISet> keyset;
    mKeywords->GetKeySet(&keyset);
    return Collections::CreateUnmodifiableSet(keyset);
}

String UnicodeLocaleExtension::GetUnicodeLocaleType(
    /* [in] */ const String& unicodeLocaleKey)
{
    AutoPtr<IInterface> value;
    mKeywords->Get(CoreUtils::Box(unicodeLocaleKey), &value);
    return CoreUtils::Unbox(ICharSequence::Probe(value));
}

Boolean UnicodeLocaleExtension::IsAttribute(
    /* [in] */ const String& s)
{
    // 3*8alphanum
    Integer len = s.GetByteLength();
    return (len >= 3) && (len <= 8) && LocaleUtils::IsAlphaNumericString(s);
}

Boolean UnicodeLocaleExtension::IsTypeSubtag(
    /* [in] */ const String& s)
{
    // 3*8alphanum
    Integer len = s.GetByteLength();
    return (len >= 3) && (len <= 8) && LocaleUtils::IsAlphaNumericString(s);
}

ECode UnicodeLocaleExtension::GetCoclassID(
    /* [out] */ CoclassID* cid)
{
    VALIDATE_NOT_NULL(cid);

    *cid = CID_UnicodeLocaleExtension;
    return NOERROR;
}

}
}
}
