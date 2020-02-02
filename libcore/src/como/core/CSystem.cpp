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

#include "como/core/CSystem.h"
#include "como/core/System.h"

namespace como {
namespace core {

COMO_INTERFACE_IMPL_1(CSystem, Object, ISystem);
COMO_OBJECT_IMPL(CSystem);

ECode CSystem::GetCurrentTimeMillis(
    /* [out] */ Long& millis)
{
    millis = System::GetCurrentTimeMillis();
    return NOERROR;
}

ECode CSystem::GetNanoTime(
    /* [out] */ Long& time)
{
    time = System::GetNanoTime();
    return NOERROR;
}

}
}
