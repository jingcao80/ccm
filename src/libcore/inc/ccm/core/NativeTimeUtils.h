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

#ifndef __CCM_CORE_NATIVETIMEUTILS_H__
#define __CCM_CORE_NATIVETIMEUTILS_H__

#include <ccmtypes.h>
#include <stdint.h>
#include <time.h>

namespace ccm {
namespace core {

// Returns the monotonic time since some unspecified starting point in nanoseconds.
uint64_t NanoTime();

// Converts the given number of nanoseconds to milliseconds.
static constexpr inline uint64_t NsToMs(
    /* [in] */ uint64_t ns)
{
    return ns / 1000 / 1000;
}

// Converts the given number of milliseconds to nanoseconds
static constexpr inline uint64_t MsToNs(
    /* [in] */ uint64_t ms)
{
    return ms * 1000 * 1000;
}

// Sleep for the given number of nanoseconds, a bad way to handle contention.
void NanoSleep(
    /* [in] */ uint64_t ns);

// Initialize a timespec to either a relative time (ms,ns), or to the absolute
// time corresponding to the indicated clock value plus the supplied offset.
void InitTimeSpec(
    /* [in] */ Boolean absolute,
    /* [in] */ int clock,
    /* [in] */ int64_t ms,
    /* [in] */ int32_t ns,
    /* [in] */ timespec* ts);

}
}

#endif // __CCM_CORE_NATIVETIMEUTILS_H__
