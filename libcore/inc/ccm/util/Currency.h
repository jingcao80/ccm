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

#ifndef __CCM_UTIL_CURRENCY_H__
#define __CCM_UTIL_CURRENCY_H__

#include "ccm/core/SyncObject.h"
#include "ccm.io.ISerializable.h"
#include "ccm.util.ICurrency.h"
#include "ccm.util.IHashSet.h"
#include "ccm.util.ILocale.h"
#include "ccm.util.concurrent.IConcurrentMap.h"

using ccm::core::SyncObject;
using ccm::io::ISerializable;
using ccm::util::IHashSet;
using ccm::util::concurrent::IConcurrentMap;

namespace ccm {
namespace util {

class Currency
    : public SyncObject
    , public ICurrency
    , public ISerializable
{
public:
    CCM_INTERFACE_DECL();

    static AutoPtr<ICurrency> GetInstance(
        /* [in] */ const String& currencyCode);

    static AutoPtr<ICurrency> GetInstance(
        /* [in] */ ILocale* locale);

    static AutoPtr<ISet> GetAvailableCurrencies();

    ECode GetCurrencyCode(
        /* [out] */ String* currencyCode) override;

    ECode GetSymbol(
        /* [out] */ String* symbol) override;

    ECode GetSymbol(
        /* [in] */ ILocale* locale,
        /* [out] */ String* symbol) override;

    ECode GetDefaultFractionDigits(
        /* [out] */ Integer* digits) override;

    ECode GetNumericCode(
        /* [out] */ Integer* numericCode) override;

    ECode GetDisplayName(
        /* [out] */ String* displayName) override;

    ECode GetDisplayName(
        /* [in] */ ILocale* locale,
        /* [out] */ String* displayName) override;

    ECode ToString(
        /* [out] */ String* desc) override;

private:
    ECode Constructor(
        /* [in] */ const String& currencyCode);

    static AutoPtr<IConcurrentMap> GetInstances();

private:
    String mCurrencyCode;
    static AutoPtr<IHashSet> sAvailable;

    static SyncObject sLock;
};

}
}

#endif // __CCM_UTIL_CURRENCY_H__
