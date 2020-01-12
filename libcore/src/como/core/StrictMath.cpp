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

#include "como/core/StrictMath.h"
#if defined(__android__)
#include <fdlibm/fdlibm.h>
#elif defined(__linux__)
#include <fdlibm.h>
#endif

namespace como {
namespace core {

Double StrictMath::Log(
    /* [in] */ Double a)
{
    return (Double)ieee_log((double)a);
}

Double StrictMath::Sqrt(
    /* [in] */ Double a)
{
    return (Double)ieee_sqrt((double)a);
}

}
}
