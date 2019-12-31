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

#ifndef __LIBCORE_UTIL_BASICLRUCACHE_H__
#define __LIBCORE_UTIL_BASICLRUCACHE_H__

#include "ccm/core/SyncObject.h"
#include "ccm.util.IMap.h"
#include "libcore.util.IBasicLruCache.h"

using ccm::core::SyncObject;
using ccm::util::IMap;

namespace libcore {
namespace util {

class BasicLruCache
    : public SyncObject
    , public IBasicLruCache
{
public:
    CCM_INTERFACE_DECL();

    ECode Constructor(
        /* [in] */ Integer maxSize);

    ECode Get(
        /* [in] */ IInterface* key,
        /* [out] */ IInterface** value) override;

    ECode Put(
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* value,
        /* [out] */ IInterface** prevValue = nullptr) override;

    ECode Snapshot(
        /* [out] */ IMap** snapshot) override;

    ECode EvictAll() override;

protected:
    virtual void EntryEvicted(
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* value);

    virtual ECode Create(
        /* [in] */ IInterface* key,
        /* [out] */ IInterface** value);

private:
    void TrimToSize(
        /* [in] */ Integer maxSize);

private:
    AutoPtr<IMap> mMap;
    Integer mMaxSize = 0;
};

}
}

#endif // __LIBCORE_UTIL_BASICLRUCACHE_H__
