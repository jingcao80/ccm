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

#include "coredef.h"
#include "como/core/AutoLock.h"
#include "como/core/CoreUtils.h"
#include "como/core/CThreadLocal.h"
#include "como/io/CharBuffer.h"
#include "como/io/charset/Charset.h"
#include "como/io/charset/CodingErrorAction.h"
#include "como/io/charset/ThreadLocalCoders.h"
#include "como/misc/ASCIICaseInsensitiveComparator.h"
#include "como/security/AccessController.h"
#include "como/util/CHashMap.h"
#include "como/util/CHashSet.h"
#include "como/util/Collections.h"
#include "como/util/CSimpleImmutableEntry.h"
#include "como/util/CTreeMap.h"
#include "como/util/ServiceLoader.h"
#include "como.core.ICharSequence.h"
#include "como.core.IIterable.h"
#include "como.io.charset.spi.ICharsetProvider.h"
#include "como.security.IPrivilegedAction.h"
#include "como.util.IServiceLoader.h"
#include "libcore/icu/NativeConverter.h"

using como::core::AutoLock;
using como::core::CThreadLocal;
using como::core::ICharSequence;
using como::core::IID_IComparable;
using como::core::IID_IThreadLocal;
using como::core::IIterable;
using como::core::CoreUtils;
using como::io::charset::spi::ICharsetProvider;
using como::io::charset::spi::IID_ICharsetProvider;
using como::misc::ASCIICaseInsensitiveComparator;
using como::security::AccessController;
using como::security::IID_IPrivilegedAction;
using como::security::IPrivilegedAction;
using como::util::CHashMap;
using como::util::CHashSet;
using como::util::Collections;
using como::util::CSimpleImmutableEntry;
using como::util::CTreeMap;
using como::util::E_NO_SUCH_ELEMENT_EXCEPTION;
using como::util::IID_IHashMap;
using como::util::IID_IHashSet;
using como::util::IID_IIterator;
using como::util::IID_IMapEntry;
using como::util::IID_ITreeMap;
using como::util::IHashSet;
using como::util::IServiceLoader;
using como::util::ITreeMap;
using como::util::ServiceLoader;
using libcore::icu::NativeConverter;

namespace como {
namespace io {
namespace charset {

VOLATILE AutoPtr<IMapEntry> Charset::sCache1;
AutoPtr<ICharset> Charset::sDefaultCharset;
SyncObject Charset::sLock;

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

COMO_INTERFACE_IMPL_2(Charset, SyncObject, ICharset, IComparable);

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
        cs->GetName(canonicalName);
        AutoPtr<IInterface> v;
        cache2->Get(CoreUtils::Box(canonicalName), v);
        AutoPtr<ICharset> canonicalCharset = std::move(v);

        if (canonicalCharset != nullptr) {
            cs = canonicalCharset;
        }
        else {
            cache2->Put(CoreUtils::Box(canonicalName), cs);

            AutoPtr<ISet> aliases;
            cs->GetAliases(aliases);
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

AutoPtr<IIterator> Charset::GetProviders()
{
    class _Iterator
        : public LightRefBase
        , public IIterator
    {
    public:
        _Iterator()
        {
            AutoPtr<IServiceLoader> sl = ServiceLoader::Load(IID_ICharsetProvider);
            IIterable::Probe(sl)->GetIterator(mI);
        }

        Integer AddRef(
            /* [in] */ HANDLE id) override
        {
            return LightRefBase::AddRef(id);
        }

        Integer Release(
            /* [in] */ HANDLE id) override
        {
            return LightRefBase::Release(id);
        }

        IInterface* Probe(
            /* [in] */ const InterfaceID& iid) override
        {
            if (iid == IID_IInterface) {
                return (IInterface*)(IIterator*)this;
            }
            else if (iid == IID_IIterator) {
                return (IIterator*)this;
            }
            return nullptr;
        }

        ECode GetInterfaceID(
            /* [in] */ IInterface* object,
            /* [out] */ InterfaceID& iid) override
        {
            if (object == (IInterface*)(IIterator*)this) {
                iid = IID_IIterator;
                return NOERROR;
            }
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        ECode HasNext(
            /* [out] */ Boolean& hasNext) override
        {
            return GetNext(hasNext);
        }

        ECode Next(
            /* [out] */ IInterface** object = nullptr) override
        {
            Boolean hasNext;
            if (GetNext(hasNext), !hasNext) {
                return E_NO_SUCH_ELEMENT_EXCEPTION;
            }
            if (object != nullptr) {
                *object = mNext;
                REFCOUNT_ADD(*object);
                mNext = nullptr;
            }
            return NOERROR;
        }

        ECode Remove() override
        {
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
        }

    private:
        ECode GetNext(
            /* [out] */ Boolean& hasNext)
        {
            while (mNext == nullptr) {
                if (mI->HasNext(hasNext), !hasNext) {
                    return NOERROR;
                }
                FAIL_RETURN(mI->Next((IInterface**)&mNext));
            }
            hasNext = true;
            return NOERROR;
        }

    private:
        AutoPtr<IIterator> mI;
        AutoPtr<ICharsetProvider> mNext;
    };

    return new _Iterator();
}

ECode Charset::LookupViaProviders(
    /* [in] */ const String& charsetName,
    /* [out] */ AutoPtr<ICharset>& cs)
{
    AutoPtr<IThreadLocal> gate = GetGate();
    AutoPtr<IInterface> value;
    gate->Get(value);
    if (value != nullptr) {
        cs = nullptr;
        return NOERROR;
    }

    gate->Set(gate);

    class _PrivilegedAction
        : public LightRefBase
        , public IPrivilegedAction
    {
    public:
        _PrivilegedAction(
            /* [in] */ const String& charsetName)
            : mCharsetName(charsetName)
        {}

        Integer AddRef(
            /* [in] */ HANDLE id)
        {
            return LightRefBase::AddRef(id);
        }

        Integer Release(
            /* [in] */ HANDLE id)
        {
            return LightRefBase::Release(id);
        }

        IInterface* Probe(
            /* [in] */ const InterfaceID& iid)
        {
            if (iid == IID_IInterface) {
                return (IInterface*)(IPrivilegedAction*)this;
            }
            else if (iid == IID_IPrivilegedAction) {
                return (IPrivilegedAction*)this;
            }
            return nullptr;
        }

        ECode GetInterfaceID(
            /* [in] */ IInterface* object,
            /* [out] */ InterfaceID& iid)
        {
            if (object == (IInterface*)(IPrivilegedAction*)this) {
                iid = IID_IPrivilegedAction;
                return NOERROR;
            }
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        ECode Run(
            /* [out] */ AutoPtr<IInterface>& result)
        {
            AutoPtr<IIterator> i = GetProviders();
            Boolean hasNext;
            while (i->HasNext(hasNext), hasNext) {
                AutoPtr<ICharsetProvider> cp;
                i->Next((IInterface**)&cp);
                AutoPtr<ICharset> cs;
                cp->CharsetForName(mCharsetName, cs);
                if (cs != nullptr) {
                    result = cs;
                    return NOERROR;
                }
            }

            result = nullptr;
            return NOERROR;
        }

    private:
        String mCharsetName;
    };

    AutoPtr<IPrivilegedAction> action = new _PrivilegedAction(charsetName);
    AutoPtr<IInterface> result;
    ECode ec = AccessController::DoPrivileged(action, result);
    gate->Set(nullptr);
    cs = ICharset::Probe(result);
    return ec;
}

ECode Charset::Lookup(
    /* [in] */ const String& charsetName,
    /* [out] */ AutoPtr<ICharset>& cs)
{
    if (charsetName.IsNull()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    VOLATILE_GET(AutoPtr<IMapEntry> cached, sCache1);
    if (cached != nullptr) {
        AutoPtr<IInterface> k;
        cached->GetKey(k);
        if (charsetName.Equals(CoreUtils::Unbox(ICharSequence::Probe(k)))) {
            AutoPtr<IInterface> v;
            ECode ec = cached->GetValue(v);
            cs = std::move(v);
            return ec;
        }
    }
    return Lookup2(charsetName, cs);
}

ECode Charset::Lookup2(
    /* [in] */ const String& charsetName,
    /* [out] */ AutoPtr<ICharset>& cs)
{
    AutoPtr<IHashMap> cache2 = GetCache2();
    {
        AutoLock lock(ISynchronize::Probe(cache2));

        AutoPtr<IInterface> v;
        cache2->Get(CoreUtils::Box(charsetName), v);
        cs = std::move(v);
        if (cs != nullptr) {
            AutoPtr<IMapEntry> entry;
            CSimpleImmutableEntry::New(CoreUtils::Box(charsetName), cs, IID_IMapEntry, (IInterface**)&entry);
            VOLATILE_SET(sCache1, entry);
            return NOERROR;
        }
    }

    NativeConverter::CharsetForName(charsetName, cs);
    if (cs != nullptr) {
        Cache(charsetName, cs);
        return NOERROR;
    }

    LookupViaProviders(charsetName, cs);
    if (cs != nullptr) {
        Cache(charsetName, cs);
        return NOERROR;
    }

    FAIL_RETURN(CheckName(charsetName));
    cs = nullptr;
    return NOERROR;
}

ECode Charset::IsSupported(
    /* [in] */ const String& charsetName,
    /* [out] */ Boolean& supported)
{
    AutoPtr<ICharset> cs;
    FAIL_RETURN(Lookup(charsetName, cs));
    supported = cs != nullptr;
    return NOERROR;
}

ECode Charset::ForName(
    /* [in] */ const String& charsetName,
    /* [out] */ AutoPtr<ICharset>& cs)
{
    FAIL_RETURN(Lookup(charsetName, cs));
    if (cs != nullptr) {
        return NOERROR;
    }
    return E_UNSUPPORTED_CHARSET_EXCEPTION;
}

ECode Charset::ForNameUEE(
    /* [in] */ const String& charsetName,
    /* [out] */ AutoPtr<ICharset>& cs)
{
    ECode ec = ForName(charsetName, cs);
    if (FAILED(ec)) {
        return E_UNSUPPORTED_ENCODING_EXCEPTION;
    }
    return NOERROR;
}

void Charset::Put(
    /* [in] */ IIterator* i,
    /* [in] */ IMap* m)
{
    Boolean hasNext;
    while (i->HasNext(hasNext), hasNext) {
        AutoPtr<ICharset> cs;
        i->Next((IInterface**)&cs);
        String canonicalName;
        cs->GetName(canonicalName);
        Boolean contains;
        if (m->ContainsKey(CoreUtils::Box(canonicalName), contains), !contains) {
            m->Put(CoreUtils::Box(canonicalName), cs);
        }
    }
}

ECode Charset::AvailableCharsets(
    /* [out] */ AutoPtr<ISortedMap>& map)
{
    class _PrivilegedAction
        : public LightRefBase
        , public IPrivilegedAction
    {
    public:
        Integer AddRef(
            /* [in] */ HANDLE id)
        {
            return LightRefBase::AddRef(id);
        }

        Integer Release(
            /* [in] */ HANDLE id)
        {
            return LightRefBase::Release(id);
        }

        IInterface* Probe(
            /* [in] */ const InterfaceID& iid)
        {
            if (iid == IID_IInterface) {
                return (IInterface*)(IPrivilegedAction*)this;
            }
            else if (iid == IID_IPrivilegedAction) {
                return (IPrivilegedAction*)this;
            }
            return nullptr;
        }

        ECode GetInterfaceID(
            /* [in] */ IInterface* object,
            /* [out] */ InterfaceID& iid)
        {
            if (object == (IInterface*)(IPrivilegedAction*)this) {
                iid = IID_IPrivilegedAction;
                return NOERROR;
            }
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        ECode Run(
            /* [out] */ AutoPtr<IInterface>& result)
        {
            AutoPtr<ITreeMap> m;
            CTreeMap::New(ASCIICaseInsensitiveComparator::GetCASE_INSENSITIVE_ORDER(),
                    IID_ITreeMap, (IInterface**)&m);
            for (String charsetName : NativeConverter::GetAvailableCharsetNames()) {
                AutoPtr<ICharset> charset;
                NativeConverter::CharsetForName(charsetName, charset);
                String canonicalName;
                charset->GetName(canonicalName);
                IMap::Probe(m)->Put(CoreUtils::Box(canonicalName), charset);
            }
            AutoPtr<IIterator> i = GetProviders();
            Boolean hasNext;
            while(i->HasNext(hasNext), hasNext) {
                AutoPtr<ICharsetProvider> cp;
                i->Next((IInterface**)&cp);
                AutoPtr<IIterator> it;
                cp->Charsets(it);
                Put(it, IMap::Probe(m));
            }
            result = Collections::CreateUnmodifiableSortedMap(ISortedMap::Probe(m));
            return NOERROR;
        }

    private:
    };

    AutoPtr<IPrivilegedAction> action = new _PrivilegedAction();
    AutoPtr<IInterface> result;
    ECode ec = AccessController::DoPrivileged(action, result);
    map = ISortedMap::Probe(result);
    return NOERROR;
}

AutoPtr<ICharset> Charset::GetDefaultCharset()
{
    if (sDefaultCharset == nullptr) {
        AutoLock lock(sLock);

        if (sDefaultCharset == nullptr) {
            sDefaultCharset = StandardCharsets::GetUTF_8();
        }
    }

    return sDefaultCharset;
}

ECode Charset::Constructor(
    /* [in] */ const String& canonicalName,
    /* [in] */ const Array<String>& aliases)
{
    FAIL_RETURN(CheckName(canonicalName));
    Array<String> as = aliases;
    if (as.IsNull()) {
        as = Array<String>(0);
    }
    for (Integer i = 0; i < as.GetLength(); i++) {
        FAIL_RETURN(CheckName(as[i]));
    }
    mName = canonicalName;
    mAliases = as;
    return NOERROR;
}

ECode Charset::GetName(
    /* [out] */ String& name)
{
    name = mName;
    return NOERROR;
}

ECode Charset::GetAliases(
    /* [out] */ AutoPtr<ISet>& aliases)
{
    if (mAliasSet != nullptr) {
        aliases = mAliasSet;
        return NOERROR;
    }
    Integer n = mAliases.GetLength();
    AutoPtr<IHashSet> hs;
    CHashSet::New(n, IID_IHashSet, (IInterface**)&hs);
    for (Integer i = 0; i < n; i++) {
        hs->Add(CoreUtils::Box(mAliases[i]));
    }
    mAliasSet = Collections::CreateUnmodifiableSet(ISet::Probe(hs));
    aliases = mAliasSet;
    return NOERROR;
}

ECode Charset::GetDisplayName(
    /* [out] */ String& name)
{
    name = mName;
    return NOERROR;
}

ECode Charset::IsRegistered(
    /* [out] */ Boolean& registered)
{
    registered = !mName.StartsWith("X-") && !mName.StartsWith("x-");
    return NOERROR;
}

ECode Charset::GetDisplayName(
    /* [in] */ ILocale* locale,
    /* [out] */ String& name)
{
    name = mName;
    return NOERROR;
}

ECode Charset::CanEncode(
    /* [out] */ Boolean& supported)
{
    supported = true;
    return NOERROR;
}

ECode Charset::Decode(
    /* [in] */ IByteBuffer* bb,
    /* [out] */ AutoPtr<ICharBuffer>& cb)
{
    AutoPtr<ICharsetDecoder> cd = ThreadLocalCoders::DecoderFor((ICharset*)this);
    cd->OnMalformedInput(CodingErrorAction::GetREPLACE());
    cd->OnUnmappableCharacter(CodingErrorAction::GetREPLACE());
    cd->Decode(bb, cb);
    return NOERROR;
}

ECode Charset::Encode(
    /* [in] */ ICharBuffer* cb,
    /* [out] */ AutoPtr<IByteBuffer>& bb)
{
    AutoPtr<ICharsetEncoder> ce = ThreadLocalCoders::EncoderFor((ICharset*)this);
    ce->OnMalformedInput(CodingErrorAction::GetREPLACE());
    ce->OnUnmappableCharacter(CodingErrorAction::GetREPLACE());
    ce->Encode(cb, bb);
    return NOERROR;
}

ECode Charset::Encode(
    /* [in] */ const String& str,
    /* [out] */ AutoPtr<IByteBuffer>& bb)
{
    AutoPtr<ICharBuffer> cb;
    CharBuffer::Wrap(CoreUtils::Box(str), &cb);
    return Encode(cb, bb);
}

ECode Charset::CompareTo(
    /* [in] */ IInterface* other,
    /* [out] */ Integer& result)
{
    ICharset* othercs = ICharset::Probe(other);
    if (othercs == nullptr) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    String thisName, thatName;
    GetName(thisName);
    othercs->GetName(thatName);
    result = thisName.CompareIgnoreCase(thatName);
    return NOERROR;
}

ECode Charset::GetHashCode(
    /* [out] */ Integer& hash)
{
    String canonicalName;
    GetName(canonicalName);
    hash = canonicalName.GetHashCode();
    return NOERROR;
}

ECode Charset::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean& same)
{
    ICharset* cs = ICharset::Probe(obj);
    if (cs == nullptr) {
        same = false;
        return NOERROR;
    }
    if (cs == (ICharset*)this) {
        same = true;
        return NOERROR;
    }
    String thisName, thatName;
    GetName(thisName);
    cs->GetName(thatName);
    same = thisName.Equals(thatName);
    return NOERROR;
}

ECode Charset::ToString(
    /* [out] */ String& desc)
{
    return GetName(desc);
}

}
}
}
