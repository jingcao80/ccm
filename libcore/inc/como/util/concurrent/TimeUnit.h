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

#ifndef __COMO_UTIL_CONCURRENT_TIMEUNIT_H__
#define __COMO_UTIL_CONCURRENT_TIMEUNIT_H__

#include <ccmtypes.h>

namespace como {
namespace util {
namespace concurrent {

class TimeUnit
{
public:
    class NANOSECONDS
    {
    public:
        inline static Long ToMillis(
            /* [in] */ Long d)
        {
            return d / (C2 / C0);
        }
    };

public:
    static constexpr Long C0 = 1LL;
    static constexpr Long C1 = C0 * 1000LL;
    static constexpr Long C2 = C1 * 1000LL;
    static constexpr Long C3 = C2 * 1000LL;
    static constexpr Long C4 = C3 * 60LL;
    static constexpr Long C5 = C4 * 60LL;
    static constexpr Long C6 = C5 * 24LL;
};

}
}
}

#endif // __COMO_UTIL_CONCURRENT_TIMEUNIT_H__
