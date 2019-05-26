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
#include "ccm.core.IComparable.h"
#include "ccm.core.IThreadLocal.h"
#include "ccm.io.charset.ICharset.h"
#include "ccm.util.IHashMap.h"
#include "ccm.util.IMapEntry.h"

using ccm::core::IComparable;
using ccm::core::IThreadLocal;
using ccm::core::SyncObject;
using ccm::util::IHashMap;
using ccm::util::IMapEntry;

namespace ccm {
namespace io {
namespace charset {

class Charset
    : public SyncObject
    , public IComparable
{
public:
    CCM_INTERFACE_DECL();

    static ECode ForName(
        /* [in] */ const String& charsetName,
        /* [out] */ ICharset** cs);

    static ECode ForNameUEE(
        /* [in] */ const String& charsetName,
        /* [out] */ ICharset** cs);

private:
    static ECode CheckName(
        /* [in] */ const String& s);

    static void Cache(
        /* [in] */ const String& charsetName,
        /* [in] */ ICharset* cs);

    static ECode LookupViaProviders(
        /* [in] */ const String& charsetName,
        /* [out] */ ICharset** cs);

    static ECode Lookup(
        /* [in] */ const String& charsetName,
        /* [out] */ ICharset** cs);

    static ECode Lookup2(
        /* [in] */ const String& charsetName,
        /* [out] */ ICharset** cs);

    static AutoPtr<IHashMap> GetCache2();

    static AutoPtr<IThreadLocal> GetGate();

private:
    static VOLATILE AutoPtr<IMapEntry> sCache1;
};

}
}
}

#endif // __CCM_IO_CHARSET_CHARSET_H__
