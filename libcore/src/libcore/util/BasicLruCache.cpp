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

#include "como/core/AutoLock.h"
#include "como/util/CLinkedHashMap.h"
#include "como.util.IMapEntry.h"
#include "libcore/util/BasicLruCache.h"
#include <comolog.h>

using como::core::AutoLock;
using como::core::E_NULL_POINTER_EXCEPTION;
using como::util::CLinkedHashMap;
using como::util::IID_IMap;
using como::util::ILinkedHashMap;
using como::util::IMapEntry;

namespace libcore {
namespace util {

COMO_INTERFACE_IMPL_1(BasicLruCache, SyncObject, IBasicLruCache);

ECode BasicLruCache::Constructor(
    /* [in] */ Integer maxSize)
{
    if (maxSize <= 0) {
        Logger::E("BasicLruCache", "maxSize <= 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mMaxSize = maxSize;
    CLinkedHashMap::New(0, 0.75f, true, IID_IMap, (IInterface**)&mMap);
    return NOERROR;
}

ECode BasicLruCache::Get(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value);

    if (key == nullptr) {
        Logger::E("BasicLruCache", "key == null");
        return E_NULL_POINTER_EXCEPTION;
    }

    {
        AutoLock lock(this);

        mMap->Get(key, value);
        if (*value != nullptr) {
            return NOERROR;
        }
    }

    // Don't hold any locks while calling create.
    AutoPtr<IInterface> result;
    FAIL_RETURN(Create(key, &result));

    {
        AutoLock lock(this);
        // NOTE: Another thread might have already inserted a value for |key| into the map.
        // This shouldn't be an observable change as long as create creates equal values for
        // equal keys. We will however attempt to trim the map twice, but that shouldn't be
        // a big deal since uses of this class aren't heavily contended (and this class
        // isn't design for such usage anyway).
        if (result != nullptr) {
            mMap->Put(key, result);
            TrimToSize(mMaxSize);
        }
    }

    result.MoveTo(value);
    return NOERROR;
}

ECode BasicLruCache::Put(
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value,
    /* [out] */ IInterface** prevValue)
{
    if (key == nullptr) {
        Logger::E("BasicLruCache", "key == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    else if (value == nullptr) {
        Logger::E("BasicLruCache", "value == null");
        return E_NULL_POINTER_EXCEPTION;
    }

    mMap->Put(key, value, prevValue);
    TrimToSize(mMaxSize);
    return NOERROR;
}

void BasicLruCache::TrimToSize(
    /* [in] */ Integer maxSize)
{
    Integer size;
    while (mMap->GetSize(&size), size > maxSize) {
        AutoPtr<IMapEntry> toEvict;
        ILinkedHashMap::Probe(mMap)->GetEldest(&toEvict);

        AutoPtr<IInterface> key, value;
        toEvict->GetKey(&key);
        toEvict->GetValue(&value);
        mMap->Remove(key);

        EntryEvicted(key, value);
    }
}

void BasicLruCache::EntryEvicted(
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value)
{}

ECode BasicLruCache::Create(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** value)
{
    *value = nullptr;
    return NOERROR;
}

ECode BasicLruCache::Snapshot(
    /* [out] */ IMap** snapshot)
{
    return CLinkedHashMap::New(IMap::Probe(mMap), IID_IMap, (IInterface**)snapshot);
}

ECode BasicLruCache::EvictAll()
{
    TrimToSize(0);
    return NOERROR;
}

}
}
