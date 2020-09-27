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

#include "como/core/AutoLock.h"
#include "como/core/CoreUtils.h"
#include "como/util/CHashSet.h"
#include "como/util/CLocale.h"
#include "como/util/Currency.h"
#include "como/util/concurrent/CConcurrentHashMap.h"
#include "como.util.IMap.h"
#include "libcore/icu/ICU.h"
#include "libcore/icu/LocaleData.h"
#include <comolog.h>

using como::core::AutoLock;
using como::core::CoreUtils;
using como::core::E_NULL_POINTER_EXCEPTION;
using como::io::IID_ISerializable;
using como::util::CHashSet;
using como::util::CLocale;
using como::util::IID_ISet;
using como::util::IMap;
using como::util::concurrent::CConcurrentHashMap;
using como::util::concurrent::IID_IConcurrentMap;
using libcore::icu::ICU;
using libcore::icu::LocaleData;

namespace como {
namespace util {

AutoPtr<IHashSet> Currency::sAvailable;
SyncObject Currency::sLock;

static AutoPtr<IConcurrentMap> CreateConcurrentHashMap()
{
    AutoPtr<IConcurrentMap> map;
    CConcurrentHashMap::New(7, IID_IConcurrentMap, (IInterface**)&map);
    return map;
}

AutoPtr<IConcurrentMap> Currency::GetInstances()
{
    static AutoPtr<IConcurrentMap> sInstances = CreateConcurrentHashMap();
    return sInstances;
}

COMO_INTERFACE_IMPL_2(Currency, SyncObject, ICurrency, ISerializable);

ECode Currency::Constructor(
    /* [in] */ const String& currencyCode)
{
    mCurrencyCode = currencyCode;
    return NOERROR;
}

AutoPtr<ICurrency> Currency::GetInstance(
    /* [in] */ const String& currencyCode)
{
    IMap* instances = IMap::Probe(GetInstances());

    AutoPtr<IInterface> v;
    instances->Get(CoreUtils::Box(currencyCode), v);
    AutoPtr<ICurrency> instance = std::move(v);
    if (instance != nullptr) {
        return instance;
    }
    AutoPtr<Currency> currency = new Currency();
    currency->Constructor(currencyCode);
    instances->PutIfAbsent(CoreUtils::Box(currencyCode),
            (ICurrency*)currency.Get(), (IInterface**)&instance);
    return (instance != nullptr ? instance.Get() : (ICurrency*)currency.Get());
}

AutoPtr<ICurrency> Currency::GetInstance(
    /* [in] */ ILocale* locale)
{
    String variant;
    String country;
    locale->GetCountry(&country);
    locale->GetVariant(&variant);
    if (!variant.IsEmpty()
        && (variant.Equals("EURO")
        || variant.Equals("HK")
        || variant.Equals("PREEURO"))) {
        country = country + String("_") + variant;
    }

    String currencyCode = ICU::GetCurrencyCode(country);
    if (currencyCode.IsNull()) {
        Logger::E("Currency", "Unsupported ISO 3166 country: %s", Object::ToString(locale).string());
        CHECK(0);
    }
    else if (currencyCode.Equals("XXX")) {
        return nullptr;
    }
    return GetInstance(currencyCode);
}

AutoPtr<ISet> Currency::GetAvailableCurrencies()
{
    {
        AutoLock lock(sLock);
        if (sAvailable == nullptr) {
            CHashSet::New(IID_IHashSet, (IInterface**)&sAvailable);
            Array<String> currencyCodes = ICU::GetAvailableCurrencyCodes();
            for (String currencyCode : currencyCodes) {
                AutoPtr<ICurrency> currency = GetInstance(currencyCode);
                sAvailable->Add(currency);
            }
        }
    }

    return (ISet*)CoreUtils::Clone(sAvailable, IID_ISet).Get();
}

ECode Currency::GetCurrencyCode(
    /* [out] */ String& currencyCode)
{
    currencyCode = mCurrencyCode;
    return NOERROR;
}

ECode Currency::GetSymbol(
    /* [out] */ String& symbol)
{
    return GetSymbol(CLocale::GetDefault(CLocale::Category::GetDISPLAY()), symbol);
}

ECode Currency::GetSymbol(
    /* [in] */ ILocale* locale,
    /* [out] */ String& symbol)
{
    if (locale == nullptr) {
        Logger::E("Currency", "locale == null");
        return E_NULL_POINTER_EXCEPTION;
    }

    String country;
    locale->GetCountry(&country);
    if (country.IsEmpty()) {
        symbol = mCurrencyCode;
        return NOERROR;
    }

    // Check the locale first, in case the locale has the same currency.
    AutoPtr<ILocaleData> localeData;
    LocaleData::Get(locale, &localeData);
    String intlCurrencySymbol;
    localeData->GetInternationalCurrencySymbol(&intlCurrencySymbol);
    if (intlCurrencySymbol.Equals(mCurrencyCode)) {
        return localeData->GetCurrencySymbol(&symbol);
    }

    // Try ICU, and fall back to the currency code if ICU has nothing.
    symbol = ICU::GetCurrencySymbol(locale, mCurrencyCode);
    if (!symbol.IsNull()) {
        return NOERROR;
    }

    symbol = mCurrencyCode;
    return NOERROR;
}

ECode Currency::GetDefaultFractionDigits(
    /* [out] */ Integer& digits)
{
    if (mCurrencyCode.Equals("XXX")) {
        digits = -1;
        return NOERROR;
    }
    digits = ICU::GetCurrencyFractionDigits(mCurrencyCode);
    return NOERROR;
}

ECode Currency::GetNumericCode(
    /* [out] */ Integer& numericCode)
{
    numericCode = ICU::GetCurrencyNumericCode(mCurrencyCode);
    return NOERROR;
}

ECode Currency::GetDisplayName(
    /* [out] */ String& displayName)
{
    return GetDisplayName(CLocale::GetDefault(CLocale::Category::GetDISPLAY()), displayName);
}

ECode Currency::GetDisplayName(
    /* [in] */ ILocale* locale,
    /* [out] */ String& displayName)
{
    displayName = ICU::GetCurrencyDisplayName(locale, mCurrencyCode);
    return NOERROR;
}

ECode Currency::ToString(
    /* [out] */ String& desc)
{
    desc = mCurrencyCode;
    return NOERROR;
}

}
}
