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

#include "coredef.h"
#include "ccm/core/AutoLock.h"
#include "ccm/core/CoreUtils.h"
#include "ccm/core/CThreadLocal.h"
#include "ccm/io/charset/Charset.h"
#include "ccm/util/CHashMap.h"
#include "ccm/util/CSimpleImmutableEntry.h"
#include "ccm.core.ICharSequence.h"
#include "ccm.util.ISet.h"
#include "libcore/icu/NativeConverter.h"

using ccm::core::AutoLock;
using ccm::core::CThreadLocal;
using ccm::core::ICharSequence;
using ccm::core::IID_IComparable;
using ccm::core::IID_IThreadLocal;
using ccm::core::CoreUtils;
using ccm::util::CHashMap;
using ccm::util::CSimpleImmutableEntry;
using ccm::util::IID_IHashMap;
using ccm::util::IID_IMapEntry;
using ccm::util::IIterator;
using ccm::util::ISet;
using libcore::icu::NativeConverter;

namespace ccm {
namespace io {
namespace charset {

VOLATILE AutoPtr<IMapEntry> Charset::sCache1;

static AutoPtr<IHashMap> CreateHashMap()
{
    AutoPtr<IHashMap> map;
    CHashMap::New(IID_IHashMap, (IInterface**)&map);
    return map;
}

AutoPtr<IHashMap> Charset::GetCache2()
{
    static AutoPtr<IHashMap> sCache2 = CreateHashMap();
    return sCache2;
}

static AutoPtr<IThreadLocal> CreateThreadLocale()
{
    AutoPtr<IThreadLocal> tl;
    CThreadLocal::New(IID_IThreadLocal, (IInterface**)&tl);
    return tl;
}

AutoPtr<IThreadLocal> Charset::GetGate()
{
    static AutoPtr<IThreadLocal> sGate = CreateThreadLocale();
    return sGate;
}

CCM_INTERFACE_IMPL_1(Charset, SyncObject, IComparable);

ECode Charset::CheckName(
    /* [in] */ const String& s)
{
    Integer n = s.GetLength();
    if (n == 0) {
        return E_ILLEGAL_CHARSET_NAME_EXCEPTION;
    }
    for (Integer i = 0; i < n; i++) {
        Char c = s.GetChar(i);
        if (c >= U'A' && c <= U'Z') continue;
        if (c >= U'a' && c <= U'z') continue;
        if (c >= U'0' && c <= U'9') continue;
        if (c == U'-' && i != 0) continue;
        if (c == U'+' && i != 0) continue;
        if (c == U':' && i != 0) continue;
        if (c == U'_' && i != 0) continue;
        if (c == U'.' && i != 0) continue;
        return E_ILLEGAL_CHARSET_NAME_EXCEPTION;
    }
    return NOERROR;
}

void Charset::Cache(
    /* [in] */ const String& charsetName,
    /* [in] */ ICharset* cs_)
{
    AutoPtr<ICharset> cs = cs_;
    AutoPtr<IHashMap> cache2 = GetCache2();
    {
        AutoLock lock(ISynchronize::Probe(cache2));

        String canonicalName;
        cs->GetName(&canonicalName);
        AutoPtr<ICharset> canonicalCharset;
        cache2->Get(CoreUtils::Box(canonicalName), (IInterface**)&canonicalCharset);

        if (canonicalCharset != nullptr) {
            cs = canonicalCharset;
        }
        else {
            cache2->Put(CoreUtils::Box(canonicalName), cs);

            AutoPtr<ISet> aliases;
            cs->GetAliases(&aliases);
            FOR_EACH(ICharSequence*, alias, ICharSequence::Probe, aliases) {
                cache2->Put(alias, cs);
            } END_FOR_EACH();
        }

        cache2->Put(CoreUtils::Box(charsetName), cs);
    }

    AutoPtr<IMapEntry> entry;
    CSimpleImmutableEntry::New(CoreUtils::Box(charsetName), cs, IID_IMapEntry, (IInterface**)&entry);
    VOLATILE_SET(sCache1, entry);
}

ECode Charset::LookupViaProviders(
    /* [in] */ const String& charsetName,
    /* [out] */ ICharset** cs)
{

}

ECode Charset::Lookup(
    /* [in] */ const String& charsetName,
    /* [out] */ ICharset** cs)
{
    if (charsetName.IsNull()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    VOLATILE_GET(AutoPtr<IMapEntry> cached, sCache1);
    if (cached != nullptr) {
        AutoPtr<ICharSequence> key;
        cached->GetKey((IInterface**)&key);
        if (charsetName.Equals(CoreUtils::Unbox(key))) {
            return cached->GetValue((IInterface**)cs);
        }
    }
    return Lookup2(charsetName, cs);
}

ECode Charset::Lookup2(
    /* [in] */ const String& charsetName,
    /* [out] */ ICharset** cs)
{
    AutoPtr<IHashMap> cache2 = GetCache2();
    {
        AutoLock lock(ISynchronize::Probe(cache2));

        cache2->Get(CoreUtils::Box(charsetName), (IInterface**)cs);
        if (*cs != nullptr) {
            AutoPtr<IMapEntry> entry;
            CSimpleImmutableEntry::New(CoreUtils::Box(charsetName), *cs, IID_IMapEntry, (IInterface**)&entry);
            VOLATILE_SET(sCache1, entry);
            return NOERROR;
        }
    }

    NativeConverter::CharsetForName(charsetName, cs);
    if (*cs != nullptr) {
        Cache(charsetName, *cs);
        return NOERROR;
    }

    LookupViaProviders(charsetName, cs);
    if (*cs != nullptr) {
        Cache(charsetName, *cs);
        return NOERROR;
    }

    FAIL_RETURN(CheckName(charsetName));
    *cs = nullptr;
    return NOERROR;
}

ECode Charset::ForName(
    /* [in] */ const String& charsetName,
    /* [out] */ ICharset** cs)
{
    VALIDATE_NOT_NULL(cs);

    FAIL_RETURN(Lookup(charsetName, cs));
    if (*cs != nullptr) {
        return NOERROR;
    }
    return E_UNSUPPORTED_CHARSET_EXCEPTION;
}

ECode Charset::ForNameUEE(
    /* [in] */ const String& charsetName,
    /* [out] */ ICharset** cs)
{
    VALIDATE_NOT_NULL(cs);

    ECode ec = ForName(charsetName, cs);
    if (FAILED(ec)) {
        return E_UNSUPPORTED_ENCODING_EXCEPTION;
    }
    return NOERROR;
}

}
}
}
