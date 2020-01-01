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

#include "como/text/NumberFormat.h"
#include "como/text/NumberFormatFactory.h"

namespace como {
namespace text {

ECode NumberFormatFactory::GetAvailableLocales(
    /* [out, callee] */ Array<ILocale*>* locales)
{
    VALIDATE_NOT_NULL(locales);

    *locales = NumberFormat::GetAvailableLocales();
    return NOERROR;
}

ECode NumberFormatFactory::GetCurrencyInstance(
    /* [out] */ INumberFormat** instance)
{
    return NumberFormat::GetCurrencyInstance(instance);
}

ECode NumberFormatFactory::GetCurrencyInstance(
    /* [in] */ ILocale* locale,
    /* [out] */ INumberFormat** instance)
{
    return NumberFormat::GetCurrencyInstance(locale, instance);
}

ECode NumberFormatFactory::GetInstance(
    /* [out] */ INumberFormat** instance)
{
    return NumberFormat::GetInstance(instance);
}

ECode NumberFormatFactory::GetInstance(
    /* [in] */ ILocale* locale,
    /* [out] */ INumberFormat** instance)
{
    return NumberFormat::GetInstance(locale, instance);
}

ECode NumberFormatFactory::GetIntegerInstance(
    /* [out] */ INumberFormat** instance)
{
    return NumberFormat::GetIntegerInstance(instance);
}

ECode NumberFormatFactory::GetIntegerInstance(
    /* [in] */ ILocale* locale,
    /* [out] */ INumberFormat** instance)
{
    return NumberFormat::GetIntegerInstance(locale, instance);
}

ECode NumberFormatFactory::GetNumberInstance(
    /* [out] */ INumberFormat** instance)
{
    return NumberFormat::GetNumberInstance(instance);
}

ECode NumberFormatFactory::GetNumberInstance(
    /* [in] */ ILocale* locale,
    /* [out] */ INumberFormat** instance)
{
    return NumberFormat::GetNumberInstance(locale, instance);
}

ECode NumberFormatFactory::GetPercentInstance(
    /* [out] */ INumberFormat** instance)
{
    return NumberFormat::GetPercentInstance(instance);
}

ECode NumberFormatFactory::GetPercentInstance(
    /* [in] */ ILocale* locale,
    /* [out] */ INumberFormat** instance)
{
    return NumberFormat::GetPercentInstance(locale, instance);
}

}
}
