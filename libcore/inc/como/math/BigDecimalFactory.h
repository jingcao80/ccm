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

#ifndef __COMO_MATH_BIGDECIMALFACTORY_H__
#define __COMO_MATH_BIGDECIMALFACTORY_H__

#include "como.math.IBigDecimal.h"

namespace como {
namespace math {

class COM_PUBLIC BigDecimalFactory
{
public:
    static ECode GetONE(
        /* [out] */ AutoPtr<IBigDecimal>& value);

    static ECode GetTEN(
        /* [out] */ AutoPtr<IBigDecimal>& value);

    static ECode GetZERO(
        /* [out] */ AutoPtr<IBigDecimal>& value);

    static ECode ValueOf(
        /* [in] */ Long unscaledValue,
        /* [out] */ AutoPtr<IBigDecimal>& result);

    static ECode ValueOf(
        /* [in] */ Long unscaledValue,
        /* [in] */ Integer scale,
        /* [out] */ AutoPtr<IBigDecimal>& result);

    static ECode ValueOf(
        /* [in] */ Double value,
        /* [out] */ AutoPtr<IBigDecimal>& result);

private:
    BigDecimalFactory();
};

}
}

#endif // __COMO_MATH_BIGDECIMALFACTORY_H__
