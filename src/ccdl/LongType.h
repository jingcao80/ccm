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

#ifndef __CCM_CCDL_LONGTYPE_H__
#define __CCM_CCDL_LONGTYPE_H__

#include "Type.h"

namespace ccm {
namespace ccdl {

class LongType : public Type
{
public:
    LongType()
    { SetName(String("Long")); }

    bool IsPrimitiveType() override
    { return true; }

    bool IsNumericType() override
    { return true; }

    bool IsIntegralType() override
    { return true; }
};

}
}

#endif // __CCM_CCDL_LONGTYPE_H__
