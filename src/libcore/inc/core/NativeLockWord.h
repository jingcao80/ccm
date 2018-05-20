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

#ifndef __CCM_CORE_NATIVELOCKWORD_H__
#define __CCM_CORE_NATIVELOCKWORD_H__

#include <stdint.h>

namespace ccm {
namespace core {

class NativeMonitor;

class NativeLockWord
{
public:
    enum LockState {
        kUnlocked,    // No lock owners.
        kThinLocked,  // Single uncontended owner.
        kFatLocked,   // See associated monitor.
        kHashCode,    // Lock word contains an identity hash.
        kForwardingAddress,  // Lock word contains the forwarding address of an object.
    };

public:
    LockState GetState() const;

    // Return the owner thin lock thread id.
    uint32_t ThinLockOwner() const;

    // Return the Monitor encoded in a fat lock.
    NativeMonitor* FatLockMonitor() const;
};

}
}

#endif // __CCM_CORE_NATIVELOCKWORD_H__
