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

#include "core/NativeTimeUtils.h"
#include <ccmlogger.h>
#include <limits>

namespace ccm {
namespace core {

uint64_t NanoTime()
{
  timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);
  return static_cast<uint64_t>(now.tv_sec) * UINT64_C(1000000000) + now.tv_nsec;
}

void NanoSleep(
    /* [in] */ uint64_t ns)
{
    timespec tm;
    tm.tv_sec = ns / MsToNs(1000);
    tm.tv_nsec = ns - static_cast<uint64_t>(tm.tv_sec) * MsToNs(1000);
    nanosleep(&tm, nullptr);
}

void InitTimeSpec(
    /* [in] */ bool absolute,
    /* [in] */ int clock,
    /* [in] */ int64_t ms,
    /* [in] */ int32_t ns,
    /* [in] */ timespec* ts)
{
    if (absolute) {
        clock_gettime(clock, ts);
    }
    else {
        ts->tv_sec = 0;
        ts->tv_nsec = 0;
    }

    int64_t end_sec = ts->tv_sec + ms / 1000;
    constexpr int32_t int32_max = std::numeric_limits<int32_t>::max();
    if (UNLIKELY(end_sec >= int32_max)) {
        // Either ms was intended to denote an infinite timeout, or we have a
        // problem. The former generally uses the largest possible millisecond
        // or nanosecond value.  Log only in the latter case.
        constexpr int64_t int64_max = std::numeric_limits<int64_t>::max();
        if (ms != int64_max && ms != int64_max / (1000 * 1000)) {
            Logger::V("NativeTimeUtils", "Note: end time exceeds INT32_MAX: %d", end_sec);
        }
        end_sec = int32_max - 1;  // Allow for increment below.
    }
    ts->tv_sec = end_sec;
    ts->tv_nsec = (ts->tv_nsec + (ms % 1000) * 1000000) + ns;

    // Catch rollover.
    if (ts->tv_nsec >= 1000000000L) {
        ts->tv_sec++;
        ts->tv_nsec -= 1000000000L;
    }
}

}
}
