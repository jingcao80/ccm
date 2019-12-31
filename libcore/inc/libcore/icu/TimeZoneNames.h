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

#ifndef __LIBCORE_ICU_TIMEZONENAMES_H__
#define __LIBCORE_ICU_TIMEZONENAMES_H__

#include "ccm.util.IComparator.h"
#include "ccm.util.ILocale.h"
#include "libcore/util/BasicLruCache.h"
#include <ccmautoptr.h>

using ccm::util::IComparator;
using ccm::util::ILocale;
using libcore::util::BasicLruCache;

namespace libcore {
namespace icu {

class TimeZoneNames
{
public:
    class ZoneStringsCache
        : public BasicLruCache
    {
    public:
        inline ECode Constructor()
        {
            return BasicLruCache::Constructor(5);
        }

    protected:
        ECode Create(
            /* [in] */ IInterface* key,
            /* [out] */ IInterface** value) override;
    };

public:
    static ECode GetDisplayName(
        /* [in] */ const Array<Array<String>>& zoneStrings,
        /* [in] */ const String& id,
        /* [in] */ Boolean daylight,
        /* [in] */ Integer style,
        /* [out] */ String* name);

    static ECode GetZoneStrings(
        /* [in] */ ILocale* locale,
        /* [out, callee] */ Array<Array<String>>* zoneStrings);

private:
    TimeZoneNames();

    static void FillZoneStrings(
        /* [in] */ const String& languageTag,
        /* [out] */ Array<Array<String>>& result);

    static Array<String> GetAvailableTimeZoneIds();

    static AutoPtr<ZoneStringsCache> GetCachedZoneStrings();

    static AutoPtr<IComparator> GetZONE_STRINGS_COMPARATOR();

public:
    static constexpr Integer OLSON_NAME = 0;
    static constexpr Integer LONG_NAME = 1;
    static constexpr Integer SHORT_NAME = 2;
    static constexpr Integer LONG_NAME_DST = 3;
    static constexpr Integer SHORT_NAME_DST = 4;
    static constexpr Integer NAME_COUNT = 5;
};

}
}

#endif // __LIBCORE_ICU_TIMEZONENAMES_H__
