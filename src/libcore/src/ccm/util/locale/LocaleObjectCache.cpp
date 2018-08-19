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

#include "ccm/util/concurrent/CConcurrentLinkedQueue.h"
#include "ccm/util/concurrent/CConcurrentHashMap.h"
#include "ccm/util/locale/LocaleObjectCache.h"
#include "ccm.util.IMap.h"

using ccm::util::concurrent::CConcurrentLinkedQueue;
using ccm::util::concurrent::CConcurrentHashMap;
using ccm::util::concurrent::IID_IConcurrentLinkedQueue;
using ccm::util::concurrent::IID_IConcurrentMap;

namespace ccm {
namespace util {
namespace locale {

LocaleObjectCache::LocaleObjectCache(
    /* [in] */ Integer initialCapacity,
    /* [in] */ Float loadFactor,
    /* [in] */ Integer concurrencyLevel)
{
    CConcurrentLinkedQueue::New(IID_IConcurrentLinkedQueue, (IInterface**)&mQueue);
    CConcurrentHashMap::New(initialCapacity, loadFactor, concurrencyLevel,
            IID_IConcurrentMap, (IInterface**)&mMap);
}

ECode LocaleObjectCache::Get(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value);

    AutoPtr<IInterface> v;

    CleanStaleEntries();
    AutoPtr<IInterface> entry;
    IMap::Probe(mMap)->Get(key, (IInterface**)&entry);
    if (entry != nullptr) {
        CacheEntry::From(entry)->mValue->Resolve(
                IID_IInterface, (IInterface**)&v);
    }
    if (v == nullptr) {
        AutoPtr<IInterface> normalizedKey = NormalizeKey(key);
        AutoPtr<IInterface> newVal = CreateObject(key);
        if (key == nullptr || newVal == nullptr) {
            *value = nullptr;
            return NOERROR;
        }

        AutoPtr<CacheEntry> newEntry = new CacheEntry(key, newVal, IQueue::Probe(mQueue));
        entry = nullptr;
        mMap->PutIfAbsent(key, newEntry.Get(), (IInterface**)&entry);
        if (entry == nullptr) {
            v = newVal;
        }
        else {
            CacheEntry::From(entry)->mValue->Resolve(
                    IID_IInterface, (IInterface**)&v);
            if (v == nullptr) {
                IMap::Probe(mMap)->Put(key, newEntry.Get());
                v = newVal;
            }
        }
    }
    *value = v;
    REFCOUNT_ADD(*value);
    return NOERROR;
}

AutoPtr<IInterface> LocaleObjectCache::Put(
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value)
{
    AutoPtr<CacheEntry> entry = new CacheEntry(key, value, IQueue::Probe(mQueue));
    AutoPtr<IInterface> oldEntry;
    IMap::Probe(mMap)->Put(key, entry, (IInterface**)&oldEntry);
    if (oldEntry == nullptr) {
        return nullptr;
    }
    AutoPtr<IInterface> v;
    CacheEntry::From(oldEntry)->mValue->Resolve(
            IID_IInterface, (IInterface**)&v);
    return v;
}

void LocaleObjectCache::CleanStaleEntries()
{
    IMap* map = IMap::Probe(mMap);
    while (true) {
        AutoPtr<IInterface> obj;
        mQueue->Poll((IInterface**)&obj);
        if (obj == nullptr) {
            return;
        }
        map->Remove(CacheEntry::From(obj)->mKey);
    }
}

AutoPtr<IInterface> LocaleObjectCache::NormalizeKey(
    /* [in] */ IInterface* key)
{
    return key;
}

//-------------------------------------------------------------------------

CCM_INTERFACE_IMPL_LIGHT_1(LocaleObjectCache::CacheEntry, IReferenceObserver);

LocaleObjectCache::CacheEntry::CacheEntry(
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value,
    /* [in] */ IQueue* queue)
    : mKey(key)
    , mQueue(queue)
{
    mValue = Object::GetWeakReference(value);
}

ECode LocaleObjectCache::CacheEntry::OnLastStrongRef(
    /* [in] */ IObject* obj)
{
    return mQueue->Add(obj);
}

ECode LocaleObjectCache::CacheEntry::OnLastWeakRef(
    /* [in] */ IObject* obj)
{
    return NOERROR;
}

}
}
}
