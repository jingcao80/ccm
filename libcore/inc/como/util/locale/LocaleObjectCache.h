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

#ifndef __COMO_UTIL_LOCALE_LOCALEOBJECTCACHE_H__
#define __COMO_UTIL_LOCALE_LOCALEOBJECTCACHE_H__

#include "como.util.IQueue.h"
#include "como.util.concurrent.IConcurrentLinkedQueue.h"
#include "como.util.concurrent.IConcurrentMap.h"
#include <ccmautoptr.h>
#include <ccmobject.h>
#include <ccmrefbase.h>

using como::util::concurrent::IConcurrentLinkedQueue;
using como::util::concurrent::IConcurrentMap;

namespace como {
namespace util {
namespace locale {

class LocaleObjectCache
    : public Object
{
private:
    class CacheEntry
        : public LightRefBase
        , public IReferenceObserver
    {
    public:
        CacheEntry(
            /* [in] */ IInterface* key,
            /* [in] */ IInterface* value,
            /* [in] */ IQueue* queue);

        COMO_INTERFACE_DECL();

        ECode OnLastStrongRef(
            /* [in] */ IObject* obj) override;

        ECode OnLastWeakRef(
            /* [in] */ IObject* obj) override;

        static CacheEntry* From(
            /* [in] */ IInterface* entry)
        {
            return (CacheEntry*)entry;
        }

    public:
        AutoPtr<IInterface> mKey;
        AutoPtr<IWeakReference> mValue;
        AutoPtr<IQueue> mQueue;
    };

public:
    AutoPtr<IInterface> Put(
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* value);

    ECode Get(
        /* [in] */ IInterface* key,
        /* [out] */ IInterface** value);

protected:
    LocaleObjectCache();

    LocaleObjectCache(
        /* [in] */ Integer initialCapacity,
        /* [in] */ Float loadFactor,
        /* [in] */ Integer concurrencyLevel);

    virtual AutoPtr<IInterface> CreateObject(
        /* [in] */ IInterface* key) = 0;

    virtual AutoPtr<IInterface> NormalizeKey(
        /* [in] */ IInterface* key);

private:
    void CleanStaleEntries();

private:
    AutoPtr<IConcurrentMap> mMap;
    AutoPtr<IConcurrentLinkedQueue> mQueue;
};

inline LocaleObjectCache::LocaleObjectCache()
    : LocaleObjectCache(16, 0.75f, 16)
{}

}
}
}

#endif // __COMO_UTIL_LOCALE_LOCALEOBJECTCACHE_H__
