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

#ifndef __CCM_CORE_HARDCODEDSYSTEMPROPERTIES_H__
#define __CCM_CORE_HARDCODEDSYSTEMPROPERTIES_H__

#include <ccmtypes.h>

namespace ccm {
namespace core {

class HardcodedSystemProperties final
{
private:
    HardcodedSystemProperties();

public:
    static const String CCM_VERSION;

    static const String STATIC_PROPERTIES[3][2];
};

}
}

#endif // __CCM_CORE_HARDCODEDSYSTEMPROPERTIES_H__
