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

#include "ccm/math/CBigDecimalFactory.h"

namespace ccm {
namespace math {

CCM_INTERFACE_IMPL_1(CBigDecimalFactory, Object, IBigDecimalFactory);

CCM_OBJECT_IMPL(CBigDecimalFactory);

ECode CBigDecimalFactory::GetONE(
    /* [out] */ IBigDecimal** value)
{
    VALIDATE_NOT_NULL(value);

    CBigDecimal::GetONE().MoveTo(value);
    return NOERROR;
}

ECode CBigDecimalFactory::GetTEN(
    /* [out] */ IBigDecimal** value)
{
    VALIDATE_NOT_NULL(value);

    CBigDecimal::GetTEN().MoveTo(value);
    return NOERROR;
}

ECode CBigDecimalFactory::GetZERO(
    /* [out] */ IBigDecimal** value)
{
    VALIDATE_NOT_NULL(value);

    CBigDecimal::GetZERO().MoveTo(value);
    return NOERROR;
}

ECode CBigDecimalFactory::ValueOf(
    /* [in] */ Long unscaledValue,
    /* [out] */ IBigDecimal** result)
{
    return CBigDecimal::ValueOf(unscaledValue, result);
}

ECode CBigDecimalFactory::ValueOf(
    /* [in] */ Long unscaledValue,
    /* [in] */ Integer scale,
    /* [out] */ IBigDecimal** result)
{
    return CBigDecimal::ValueOf(unscaledValue, scale, result);
}

ECode CBigDecimalFactory::ValueOf(
    /* [in] */ Double value,
    /* [out] */ IBigDecimal** result)
{
    return CBigDecimal::ValueOf(value, result);
}

}
}
