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

#ifndef __CCMRT_SYSTEM_CLOSEGUARD_H__
#define __CCMRT_SYSTEM_CLOSEGUARD_H__

#include "ccm/core/SyncObject.h"
#include "ccmrt.system.ICloseGuard.h"
#include <ccmautoptr.h>

using ccm::core::SyncObject;

namespace ccmrt {
namespace system {

class CloseGuard
    : public SyncObject
    , public ICloseGuard
{
public:
    static AutoPtr<ICloseGuard> Get();

private:
    CloseGuard();
};

}
}

#endif // __CCMRT_SYSTEM_CLOSEGUARD_H__
