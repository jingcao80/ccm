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

#ifndef __CCM_IO_CHARSET_CHARSET_H__
#define __CCM_IO_CHARSET_CHARSET_H__

#include "ccm/core/SyncObject.h"
#include "ccm/core/volatile.h"
#include "ccm/io/charset/StandardCharsets.h"
#include "ccm.core.IComparable.h"
#include "ccm.core.IThreadLocal.h"
#include "ccm.io.IByteBuffer.h"
#include "ccm.io.ICharBuffer.h"
#include "ccm.io.charset.ICharset.h"
#include "ccm.util.IHashMap.h"
#include "ccm.util.IIterator.h"
#include "ccm.util.ILocale.h"
#include "ccm.util.IMap.h"
#include "ccm.util.IMapEntry.h"
#include "ccm.util.ISet.h"
#include "ccm.util.ISortedMap.h"

using ccm::core::IComparable;
using ccm::core::IThreadLocal;
using ccm::core::SyncObject;
using ccm::io::IByteBuffer;
using ccm::io::ICharBuffer;
using ccm::util::IHashMap;
using ccm::util::IIterator;
using ccm::util::ILocale;
using ccm::util::IMap;
using ccm::util::IMapEntry;
using ccm::util::ISet;
using ccm::util::ISortedMap;

namespace ccm {
namespace io {
namespace charset {

class Charset
    : public SyncObject
    , public ICharset
    , public IComparable
{
public:
    CCM_INTERFACE_DECL();

    static ECode IsSupported(
        /* [in] */ const String& charsetName,
        /* [out] */ Boolean* supported);

    static ECode ForName(
        /* [in] */ const String& charsetName,
        /* [out] */ ICharset** cs);

    static ECode ForNameUEE(
        /* [in] */ const String& charsetName,
        /* [out] */ ICharset** cs);

    static ECode AvailableCharsets(
        /* [out] */ ISortedMap** map);

    static AutoPtr<ICharset> GetDefaultCharset();

    ECode GetName(
        /* [out] */ String* name) override;

    ECode GetAliases(
        /* [out] */ ISet** aliases) override;

    ECode GetDisplayName(
        /* [out] */ String* name) override;

    ECode IsRegistered(
        /* [out] */ Boolean* registered) override;

    ECode GetDisplayName(
        /* [in] */ ILocale* locale,
        /* [out] */ String* name) override;

    ECode CanEncode(
        /* [out] */ Boolean* supported) override;

    ECode Decode(
        /* [in] */ IByteBuffer* bb,
        /* [out] */ ICharBuffer** cb) override;

    ECode Encode(
        /* [in] */ ICharBuffer* cb,
        /* [out] */ IByteBuffer** bb) override;

    ECode Encode(
        /* [in] */ const String& str,
        /* [out] */ IByteBuffer** bb) override;

    ECode CompareTo(
        /* [in] */ IInterface* other,
        /* [out] */ Integer* result) override;

    ECode GetHashCode(
        /* [out] */ Integer* hash) override;

    ECode Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* same) override;

    ECode ToString(
        /* [out] */ String* desc) override;

protected:
    ECode Constructor(
        /* [in] */ const String& canonicalName,
        /* [in] */ const Array<String>& aliases);

private:
    static ECode CheckName(
        /* [in] */ const String& s);

    static void Cache(
        /* [in] */ const String& charsetName,
        /* [in] */ ICharset* cs);

    static AutoPtr<IIterator> GetProviders();

    static ECode LookupViaProviders(
        /* [in] */ const String& charsetName,
        /* [out] */ ICharset** cs);

    static ECode Lookup(
        /* [in] */ const String& charsetName,
        /* [out] */ ICharset** cs);

    static ECode Lookup2(
        /* [in] */ const String& charsetName,
        /* [out] */ ICharset** cs);

    static void Put(
        /* [in] */ IIterator* i,
        /* [in] */ IMap* m);

    static AutoPtr<IHashMap> GetCache2();

    static AutoPtr<IThreadLocal> GetGate();

private:
    static VOLATILE AutoPtr<IMapEntry> sCache1;

    static AutoPtr<ICharset> sDefaultCharset;

    String mName;
    Array<String> mAliases;
    AutoPtr<ISet> mAliasSet;

    static SyncObject sLock;
};

}
}
}

#endif // __CCM_IO_CHARSET_CHARSET_H__
