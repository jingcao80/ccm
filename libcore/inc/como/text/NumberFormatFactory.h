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

#ifndef __COMO_TEXT_NUMBERFORMATFACTORY_H__
#define __COMO_TEXT_NUMBERFORMATFACTORY_H__

#include "como.text.INumberFormat.h"
#include "como.util.ILocale.h"

using como::util::ILocale;

namespace como {
namespace text {

class COM_PUBLIC NumberFormatFactory
{
public:
    static ECode GetAvailableLocales(
        /* [out, callee] */ Array<ILocale*>* locales);

    static ECode GetCurrencyInstance(
        /* [out] */ AutoPtr<INumberFormat>& instance);

    static ECode GetCurrencyInstance(
        /* [in] */ ILocale* locale,
        /* [out] */ AutoPtr<INumberFormat>& instance);

    static ECode GetInstance(
        /* [out] */ AutoPtr<INumberFormat>& instance);

    static ECode GetInstance(
        /* [in] */ ILocale* locale,
        /* [out] */ AutoPtr<INumberFormat>& instance);

    static ECode GetIntegerInstance(
        /* [out] */ AutoPtr<INumberFormat>& instance);

    static ECode GetIntegerInstance(
        /* [in] */ ILocale* locale,
        /* [out] */ AutoPtr<INumberFormat>& instance);

    static ECode GetNumberInstance(
        /* [out] */ AutoPtr<INumberFormat>& instance);

    static ECode GetNumberInstance(
        /* [in] */ ILocale* locale,
        /* [out] */ AutoPtr<INumberFormat>& instance);

    static ECode GetPercentInstance(
        /* [out] */ AutoPtr<INumberFormat>& instance);

    static ECode GetPercentInstance(
        /* [in] */ ILocale* locale,
        /* [out] */ AutoPtr<INumberFormat>& instance);

private:
    NumberFormatFactory();
};

}
}

#endif // __COMO_TEXT_NUMBERFORMATFACTORY_H__
