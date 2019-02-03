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

#ifndef __CCM_MATH_CBIGINTEGERFACTORY_H__
#define __CCM_MATH_CBIGINTEGERFACTORY_H__

#include "ccm/math/CBigInteger.h"
#include "ccm.math.IBigIntegerFactory.h"
#include "_ccm_math_CBigIntegerFactory.h"

namespace ccm {
namespace math {

class CBigIntegerFactory
    : public Object
    , public IBigIntegerFactory
{
public:
    CCM_INTERFACE_DECL();

    CCM_OBJECT_DECL();

    ECode ValueOf(
        /* [in] */ Long value,
        /* [out] */ IBigInteger** result);
};

}
}

#endif // __CCM_MATH_CBIGINTEGERFACTORY_H__
