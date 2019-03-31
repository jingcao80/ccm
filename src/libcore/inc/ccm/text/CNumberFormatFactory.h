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

#ifndef __CCM_TEXT_CNUMBERFORMATFACTORY_H__
#define __CCM_TEXT_CNUMBERFORMATFACTORY_H__

#include "ccm.text.INumberFormatFactory.h"
#include "_ccm_text_CNumberFormatFactory.h"
#include "ccm.util.ILocale.h"
#include <ccmobject.h>

using ccm::util::ILocale;

namespace ccm {
namespace text {

class CNumberFormatFactory
    : public Object
    , public INumberFormatFactory
{
public:
    CCM_INTERFACE_DECL();

    CCM_OBJECT_DECL();

    ECode GetAvailableLocales(
        /* [out, callee] */ Array<ILocale*>* locales) override;

    ECode GetCurrencyInstance(
        /* [out] */ INumberFormat** instance) override;

    ECode GetCurrencyInstance(
        /* [in] */ ILocale* locale,
        /* [out] */ INumberFormat** instance) override;

    ECode GetInstance(
        /* [out] */ INumberFormat** instance) override;

    ECode GetInstance(
        /* [in] */ ILocale* locale,
        /* [out] */ INumberFormat** instance) override;

    ECode GetIntegerInstance(
        /* [out] */ INumberFormat** instance) override;

    ECode GetIntegerInstance(
        /* [in] */ ILocale* locale,
        /* [out] */ INumberFormat** instance) override;

    ECode GetNumberInstance(
        /* [out] */ INumberFormat** instance) override;

    ECode GetNumberInstance(
        /* [in] */ ILocale* locale,
        /* [out] */ INumberFormat** instance) override;

    ECode GetPercentInstance(
        /* [out] */ INumberFormat** instance) override;

    ECode GetPercentInstance(
        /* [in] */ ILocale* locale,
        /* [out] */ INumberFormat** instance) override;
};

}
}

#endif // __CCM_TEXT_CNUMBERFORMATFACTORY_H__
