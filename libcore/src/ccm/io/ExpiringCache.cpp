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

#include "ccm/core/AutoLock.h"
#include "ccm/core/CoreUtils.h"
#include "ccm/core/System.h"
#include "ccm.core.ICharSequence.h"
#include "ccm/io/ExpiringCache.h"
#include "ccm/util/LinkedHashMap.h"
#include "ccm.util.IIterator.h"
#include "ccm.util.IMapEntry.h"
#include "ccm.util.ISet.h"

using ccm::core::AutoLock;
using ccm::core::CoreUtils;
using ccm::core::ICharSequence;
using ccm::core::System;
using ccm::util::IIterator;
using ccm::util::IMapEntry;
using ccm::util::ISet;
using ccm::util::LinkedHashMap;

namespace ccm {
namespace io {

ExpiringCache::ExpiringCache(
    /* [in] */ Long millisUntilExpiration)
    : mMillisUntilExpiration(millisUntilExpiration)
{
    class _LinkedHashMap
        : public LinkedHashMap
    {
    public:
        ECode Clone(
            /* [in] */ const InterfaceID& iid,
            /* [out] */ IInterface** obj) override
        {
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
        }

    protected:
        Boolean RemoveEldestEntry(
            /* [in] */ IMapEntry* eldest) override
        {
            Integer size;
            GetSize(&size);
            return size > ExpiringCache::MAX_ENTRIES;
        }
    };

    mMap = new _LinkedHashMap();
}

String ExpiringCache::Get(
    /* [in] */ const String& key)
{
    AutoLock lock(this);

    if (++mQueryCount >= mQueryOverflow) {
        Cleanup();
    }
    AutoPtr<Entry> entry = GetEntryFor(key);
    if (entry != nullptr) {
        return entry->GetVal();
    }
    return String();
}

void ExpiringCache::Put(
    /* [in] */ const String& key,
    /* [in] */ const String& val)
{
    AutoLock lock(this);

    if (++mQueryCount >= mQueryOverflow) {
        Cleanup();
    }
    AutoPtr<Entry> entry = GetEntryFor(key);
    if (entry != nullptr) {
        entry->SetTimestamp(System::GetCurrentTimeMillis());
        entry->SetVal(val);
    }
    else {
        entry = new Entry(System::GetCurrentTimeMillis(), val);
        mMap->Put(CoreUtils::Box(key), entry);
    }
}

void ExpiringCache::Clear()
{
    AutoLock lock(this);

    mMap->Clear();
}

AutoPtr<ExpiringCache::Entry> ExpiringCache::GetEntryFor(
    /* [in] */ const String& key)
{
    AutoPtr<IInterface> obj;
    mMap->Get(CoreUtils::Box(key), &obj);
    Entry* entry = (Entry*)obj.Get();
    if (entry != nullptr) {
        Long delta = System::GetCurrentTimeMillis() - entry->GetTimestamp();
        if (delta < 0 || delta >= mMillisUntilExpiration) {
            mMap->Remove(CoreUtils::Box(key));
            entry = nullptr;
        }
    }
    return entry;
}

void ExpiringCache::Cleanup()
{
    AutoPtr<ISet> keySet;
    mMap->GetKeySet(&keySet);
    Integer size;
    keySet->GetSize(&size);
    Array<String> keys(size);
    Integer i = 0;
    AutoPtr<IIterator> it;
    keySet->GetIterator(&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->Next(&obj);
        keys[i++] = CoreUtils::Unbox(ICharSequence::Probe(obj));
    }
    for (Integer j = 0; j < keys.GetLength(); j++) {
        GetEntryFor(keys[j]);
    }
    mQueryCount = 0;
}

//-------------------------------------------------------------------------

CCM_INTERFACE_IMPL_LIGHT_1(ExpiringCache::Entry, LightRefBase, IInterface);

}
}
