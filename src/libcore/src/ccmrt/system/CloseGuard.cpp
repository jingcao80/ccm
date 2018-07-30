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

#include "ccmrt/system/CloseGuard.h"

namespace ccmrt {
namespace system {

Boolean CloseGuard::ENABLED = true;

CCM_INTERFACE_IMPL_1(CloseGuard, SyncObject, ICloseGuard);

AutoPtr<ICloseGuard> CloseGuard::GetNOOP()
{
    static AutoPtr<ICloseGuard> NOOP = new CloseGuard();
    return NOOP;
}

AutoPtr<ICloseGuard> CloseGuard::Get()
{
    if (!ENABLED) {
        return GetNOOP();
    }
    return new CloseGuard();
}

ECode CloseGuard::Close()
{
    return NOERROR;
}

ECode CloseGuard::Open(
    /* [in] */ const String& closer)
{
    return NOERROR;
}

ECode CloseGuard::WarnIfOpen()
{
    return NOERROR;
}

}
}
