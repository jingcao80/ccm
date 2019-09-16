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

#include "ccmspinlock.h"

namespace ccm {

void Spinlock::Lock()
{
    if (mLocked.exchange(true, std::memory_order_acquire)) {
        // Lock was contended.  Fall back to an out-of-line spin loop.
        while (mLocked.exchange(true, std::memory_order_acquire)) {
        }
    }
}

Boolean Spinlock::TryLock()
{
    if (mLocked.exchange(true, std::memory_order_acquire)) {
        return false;
    }
    return true;
}

void Spinlock::Unlock()
{
    mLocked.store(false, std::memory_order_release);
}

}
