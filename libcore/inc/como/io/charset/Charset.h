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

#ifndef __COMO_IO_CHARSET_CHARSET_H__
#define __COMO_IO_CHARSET_CHARSET_H__

#include "como/core/SyncObject.h"
#include "como/core/volatile.h"
#include "como/io/charset/StandardCharsets.h"
#include "como.core.IComparable.h"
#include "como.core.IThreadLocal.h"
#include "como.io.IByteBuffer.h"
#include "como.io.ICharBuffer.h"
#include "como.io.charset.ICharset.h"
#include "como.util.IHashMap.h"
#include "como.util.IIterator.h"
#include "como.util.ILocale.h"
#include "como.util.IMap.h"
#include "como.util.IMapEntry.h"
#include "como.util.ISet.h"
#include "como.util.ISortedMap.h"

using como::core::IComparable;
using como::core::IThreadLocal;
using como::core::SyncObject;
using como::io::IByteBuffer;
using como::io::ICharBuffer;
using como::util::IHashMap;
using como::util::IIterator;
using como::util::ILocale;
using como::util::IMap;
using como::util::IMapEntry;
using como::util::ISet;
using como::util::ISortedMap;

namespace como {
namespace io {
namespace charset {

class Charset
    : public SyncObject
    , public ICharset
    , public IComparable
{
public:
    COMO_INTERFACE_DECL();

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
        /* [out] */ Integer& hash) override;

    ECode Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean& same) override;

    ECode ToString(
        /* [out] */ String& desc) override;

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

#endif // __COMO_IO_CHARSET_CHARSET_H__
