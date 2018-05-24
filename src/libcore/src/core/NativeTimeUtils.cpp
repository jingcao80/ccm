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

}
}
