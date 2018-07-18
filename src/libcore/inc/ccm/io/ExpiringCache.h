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

#ifndef __CCM_IO_EXPIRINGCACHE_H__
#define __CCM_IO_EXPIRINGCACHE_H__

#include "ccm/core/SyncObject.h"
#include "ccm.util.IMap.h"
#include <ccmautoptr.h>

using ccm::core::SyncObject;
using ccm::util::IMap;

namespace ccm {
namespace io {

class ExpiringCache
    : public SyncObject
{
protected:
    class Entry
        : public LightRefBase
        , public IInterface
    {
    public:
        inline Entry(
            /* [in] */ Long timestamp,
            /* [in] */ const String& val)
            : mTimestamp(timestamp)
            , mVal(val)
        {}

        CCM_INTERFACE_DECL();

        inline Long GetTimestamp()
        {
            return mTimestamp;
        }

        inline void SetTimestamp(
            /* [in] */ Long timestamp)
        {
            mTimestamp = timestamp;
        }

        inline String GetVal()
        {
            return mVal;
        }

        inline void SetVal(
            /* [in] */ const String& val)
        {
            mVal = val;
        }

    private:
        Long mTimestamp;
        String mVal;
    };

public:
    inline ExpiringCache()
        : ExpiringCache(30000)
    {}

    ExpiringCache(
        /* [in] */ Long millisUntilExpiration);

    String Get(
        /* [in] */ const String& key);

    void Put(
        /* [in] */ const String& key,
        /* [in] */ const String& val);

    void Clear();

private:
    AutoPtr<Entry> GetEntryFor(
        /* [in] */ const String& key);

    void Cleanup();

private:
    Long mMillisUntilExpiration;
    AutoPtr<IMap> mMap;
    Integer mQueryCount = 0;
    Integer mQueryOverflow = 300;
    static constexpr Integer MAX_ENTRIES = 200;
};

}
}

#endif // __CCM_IO_EXPIRINGCACHE_H__
