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

#include "ccm/text/CNumberFormatFactory.h"
#include "ccm/text/NumberFormat.h"

namespace ccm {
namespace text {

CCM_INTERFACE_IMPL_1(CNumberFormatFactory, Object, INumberFormatFactory);

CCM_OBJECT_IMPL(CNumberFormatFactory);

ECode CNumberFormatFactory::GetAvailableLocales(
    /* [out, callee] */ Array<ILocale*>* locales)
{
    VALIDATE_NOT_NULL(locales);

    *locales = NumberFormat::GetAvailableLocales();
    return NOERROR;
}

ECode CNumberFormatFactory::GetCurrencyInstance(
    /* [out] */ INumberFormat** instance)
{
    return NumberFormat::GetCurrencyInstance(instance);
}

ECode CNumberFormatFactory::GetCurrencyInstance(
    /* [in] */ ILocale* locale,
    /* [out] */ INumberFormat** instance)
{
    return NumberFormat::GetCurrencyInstance(locale, instance);
}

ECode CNumberFormatFactory::GetInstance(
    /* [out] */ INumberFormat** instance)
{
    return NumberFormat::GetInstance(instance);
}

ECode CNumberFormatFactory::GetInstance(
    /* [in] */ ILocale* locale,
    /* [out] */ INumberFormat** instance)
{
    return NumberFormat::GetInstance(locale, instance);
}

ECode CNumberFormatFactory::GetIntegerInstance(
    /* [out] */ INumberFormat** instance)
{
    return NumberFormat::GetIntegerInstance(instance);
}

ECode CNumberFormatFactory::GetIntegerInstance(
    /* [in] */ ILocale* locale,
    /* [out] */ INumberFormat** instance)
{
    return NumberFormat::GetIntegerInstance(locale, instance);
}

ECode CNumberFormatFactory::GetNumberInstance(
    /* [out] */ INumberFormat** instance)
{
    return NumberFormat::GetNumberInstance(instance);
}

ECode CNumberFormatFactory::GetNumberInstance(
    /* [in] */ ILocale* locale,
    /* [out] */ INumberFormat** instance)
{
    return NumberFormat::GetNumberInstance(locale, instance);
}

ECode CNumberFormatFactory::GetPercentInstance(
    /* [out] */ INumberFormat** instance)
{
    return NumberFormat::GetPercentInstance(instance);
}

ECode CNumberFormatFactory::GetPercentInstance(
    /* [in] */ ILocale* locale,
    /* [out] */ INumberFormat** instance)
{
    return NumberFormat::GetPercentInstance(locale, instance);
}

}
}
