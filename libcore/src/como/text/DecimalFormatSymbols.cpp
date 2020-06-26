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

#include "como/core/CoreUtils.h"
#include "como/text/CDecimalFormatSymbols.h"
#include "como/text/DecimalFormatSymbols.h"
#include "como/text/StringArrayWrapper.h"
#include "como/util/Currency.h"
#include "como/util/CLocale.h"
#include "como/util/concurrent/CConcurrentHashMap.h"
#include "como.core.IArray.h"
#include "como.core.ICharSequence.h"
#include "libcore/icu/ICU.h"
#include "libcore/icu/LocaleData.h"

using como::core::CoreUtils;
using como::core::E_NULL_POINTER_EXCEPTION;
using como::core::IArray;
using como::core::ICharSequence;
using como::core::IID_ICloneable;
using como::io::IID_ISerializable;
using como::util::Currency;
using como::util::CLocale;
using como::util::concurrent::CConcurrentHashMap;
using como::util::concurrent::IID_IConcurrentHashMap;
using libcore::icu::ICU;
using libcore::icu::ILocaleData;
using libcore::icu::LocaleData;

namespace como {
namespace text {

AutoPtr<IConcurrentHashMap> CreateConcurrentHashMap()
{
    AutoPtr<IConcurrentHashMap> map;
    CConcurrentHashMap::New(3, IID_IConcurrentHashMap, (IInterface**)&map);
    return map;
}

AutoPtr<IConcurrentHashMap> DecimalFormatSymbols::GetCachedLocaleData()
{
    static const AutoPtr<IConcurrentHashMap> sCachedLocaleData =
            CreateConcurrentHashMap();
    return sCachedLocaleData;
}

COMO_INTERFACE_IMPL_3(DecimalFormatSymbols, SyncObject, IDecimalFormatSymbols, ICloneable, ISerializable);

ECode DecimalFormatSymbols::Constructor()
{
    Initialize(CLocale::GetDefault(CLocale::Category::GetFORMAT()));
    return NOERROR;
}

ECode DecimalFormatSymbols::Constructor(
    /* [in] */ ILocale* locale)
{
    Initialize(locale);
    return NOERROR;
}

Array<ILocale*> DecimalFormatSymbols::GetAvailableLocales()
{
    return ICU::GetAvailableLocales();
}

AutoPtr<IDecimalFormatSymbols> DecimalFormatSymbols::GetInstance()
{
    return GetInstance(CLocale::GetDefault(CLocale::Category::GetFORMAT()));
}

AutoPtr<IDecimalFormatSymbols> DecimalFormatSymbols::GetInstance(
    /* [in] */ ILocale* locale)
{
    AutoPtr<IDecimalFormatSymbols> dfs;
    CDecimalFormatSymbols::New(locale, IID_IDecimalFormatSymbols, (IInterface**)&dfs);
    return dfs;
}

ECode DecimalFormatSymbols::GetZeroDigit(
    /* [out] */ Char& zeroDigit)
{
    zeroDigit = mZeroDigit;
    return NOERROR;
}

ECode DecimalFormatSymbols::SetZeroDigit(
    /* [in] */ Char zeroDigit)
{
    mZeroDigit = zeroDigit;
    return NOERROR;
}

ECode DecimalFormatSymbols::GetGroupingSeparator(
    /* [out] */ Char& groupingSeparator)
{
    groupingSeparator = mGroupingSeparator;
    return NOERROR;
}

ECode DecimalFormatSymbols::SetGroupingSeparator(
    /* [in] */ Char groupingSeparator)
{
    mGroupingSeparator = groupingSeparator;
    return NOERROR;
}

ECode DecimalFormatSymbols::GetDecimalSeparator(
    /* [out] */ Char& decimalSeparator)
{
    decimalSeparator = mDecimalSeparator;
    return NOERROR;
}

ECode DecimalFormatSymbols::SetDecimalSeparator(
    /* [in] */ Char decimalSeparator)
{
    mDecimalSeparator = decimalSeparator;
    return NOERROR;
}

ECode DecimalFormatSymbols::GetPerMill(
    /* [out] */ Char& perMill)
{
    perMill = mPerMill;
    return NOERROR;
}

ECode DecimalFormatSymbols::SetPerMill(
    /* [in] */ Char perMill)
{
    mPerMill = perMill;
    return NOERROR;
}

ECode DecimalFormatSymbols::GetPercent(
    /* [out] */ Char& percent)
{
    percent = mPercent;
    return NOERROR;
}

ECode DecimalFormatSymbols::GetPercentString(
    /* [out] */ String& percentStr)
{
    percentStr = String::ValueOf(mPercent);
    return NOERROR;
}

ECode DecimalFormatSymbols::SetPercent(
    /* [in] */ Char percent)
{
    mPercent = percent;
    return NOERROR;
}

ECode DecimalFormatSymbols::GetDigit(
    /* [out] */ Char& digit)
{
    digit = mDigit;
    return NOERROR;
}

ECode DecimalFormatSymbols::SetDigit(
    /* [in] */ Char digit)
{
    mDigit = digit;
    return NOERROR;
}

ECode DecimalFormatSymbols::GetPatternSeparator(
    /* [out] */ Char& patternSeparator)
{
    patternSeparator = mPatternSeparator;
    return NOERROR;
}

ECode DecimalFormatSymbols::SetPatternSeparator(
    /* [in] */ Char patternSeparator)
{
    mPatternSeparator = patternSeparator;
    return NOERROR;
}

ECode DecimalFormatSymbols::GetInfinity(
    /* [out] */ String& infinity)
{
    infinity = mInfinity;
    return NOERROR;
}

ECode DecimalFormatSymbols::SetInfinity(
    /* [in] */ const String& infinity)
{
    mInfinity = infinity;
    return NOERROR;
}

ECode DecimalFormatSymbols::GetNaN(
    /* [out] */ String& naN)
{
    naN = mNaN;
    return NOERROR;
}

ECode DecimalFormatSymbols::SetNaN(
    /* [in] */ const String& naN)
{
    mNaN = naN;
    return NOERROR;
}

ECode DecimalFormatSymbols::GetMinusSign(
    /* [out] */ Char& minusSign)
{
    minusSign = mMinusSign;
    return NOERROR;
}

ECode DecimalFormatSymbols::GetMinusSignString(
    /* [out] */ String& minusSignStr)
{
    minusSignStr = String::ValueOf(mMinusSign);
    return NOERROR;
}

ECode DecimalFormatSymbols::SetMinusSign(
    /* [in] */ Char minusSign)
{
    mMinusSign = minusSign;
    return NOERROR;
}

ECode DecimalFormatSymbols::GetCurrencySymbol(
    /* [out] */ String& currency)
{
    currency = mCurrencySymbol;
    return NOERROR;
}

ECode DecimalFormatSymbols::SetCurrencySymbol(
    /* [in] */ const String& currency)
{
    mCurrencySymbol = currency;
    return NOERROR;
}

ECode DecimalFormatSymbols::GetInternationalCurrencySymbol(
    /* [out] */ String& currency)
{
    currency = mIntlCurrencySymbol;
    return NOERROR;
}

ECode DecimalFormatSymbols::SetInternationalCurrencySymbol(
    /* [in] */ const String& currency)
{
    mIntlCurrencySymbol = currency;
    return NOERROR;
}

ECode DecimalFormatSymbols::GetCurrency(
    /* [out] */ AutoPtr<ICurrency>& currency)
{
    currency = mCurrency;
    return NOERROR;
}

ECode DecimalFormatSymbols::SetCurrency(
    /* [in] */ ICurrency* currency)
{
    if (currency == nullptr) {
        return E_NULL_POINTER_EXCEPTION;
    }
    mCurrency = currency;
    currency->GetCurrencyCode(&mIntlCurrencySymbol);
    currency->GetSymbol(mLocale, &mCurrencySymbol);
    return NOERROR;
}

ECode DecimalFormatSymbols::GetMonetaryDecimalSeparator(
    /* [out] */ Char& monetarySeparator)
{
    monetarySeparator = mMonetarySeparator;
    return NOERROR;
}

ECode DecimalFormatSymbols::SetMonetaryDecimalSeparator(
    /* [in] */ Char sep)
{
    mMonetarySeparator = sep;
    return NOERROR;
}

Char DecimalFormatSymbols::GetExponentialSymbol()
{
    return mExponential;
}

ECode DecimalFormatSymbols::GetExponentSeparator(
    /* [out] */ String& exponentSeparator)
{
    exponentSeparator = mExponentialSeparator;
    return NOERROR;
}

void DecimalFormatSymbols::SetExponentialSymbol(
    /* [in] */ Char exp)
{
    mExponential = exp;
}

ECode DecimalFormatSymbols::SetExponentSeparator(
    /* [in] */ const String& exp)
{
    if (exp.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
    }
    mExponentialSeparator = exp;
    return NOERROR;
}

ECode DecimalFormatSymbols::CloneImpl(
    /* [in] */ IDecimalFormatSymbols* newObj)
{
    DecimalFormatSymbols* dfs = (DecimalFormatSymbols*)newObj;

    dfs->mZeroDigit = mZeroDigit;
    dfs->mGroupingSeparator = mGroupingSeparator;
    dfs->mDecimalSeparator = mDecimalSeparator;
    dfs->mPerMill = mPerMill;
    dfs->mPercent = mPercent;
    dfs->mDigit = mDigit;
    dfs->mPatternSeparator = mPatternSeparator;
    dfs->mInfinity = mInfinity;
    dfs->mNaN = mNaN;
    dfs->mMinusSign = mMinusSign;
    dfs->mCurrencySymbol = mCurrencySymbol;
    dfs->mIntlCurrencySymbol = mIntlCurrencySymbol;
    dfs->mMonetarySeparator = mMonetarySeparator;
    dfs->mExponential = mExponential;
    dfs->mExponentialSeparator = mExponentialSeparator;
    dfs->mLocale = mLocale;
    dfs->mCurrency = mCurrency;
    return NOERROR;
}

ECode DecimalFormatSymbols::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean& same)
{
    DecimalFormatSymbols* other =
        (DecimalFormatSymbols*)IDecimalFormatSymbols::Probe(obj);
    if (other == nullptr) {
        same = false;
        return NOERROR;
    }
    if (this == other) {
        same = true;
        return NOERROR;
    }
    same = (mZeroDigit == other->mZeroDigit &&
            mGroupingSeparator == other->mGroupingSeparator &&
            mDecimalSeparator == other->mDecimalSeparator &&
            mPercent == other->mPercent &&
            mPerMill == other->mPerMill &&
            mDigit == other->mDigit &&
            mMinusSign == other->mMinusSign &&
            mPatternSeparator == other->mPatternSeparator &&
            mInfinity.Equals(other->mInfinity) &&
            mNaN.Equals(other->mNaN) &&
            mCurrencySymbol.Equals(other->mCurrencySymbol) &&
            mIntlCurrencySymbol.Equals(other->mIntlCurrencySymbol) &&
            mCurrency == other->mCurrency &&
            mMonetarySeparator == other->mMonetarySeparator &&
            mExponentialSeparator.Equals(other->mExponentialSeparator) &&
            Object::Equals(mLocale, other->mLocale));
    return NOERROR;
}

ECode DecimalFormatSymbols::GetHashCode(
    /* [out] */ Integer& hash)
{
    hash = mZeroDigit;
    hash = hash * 37 + mGroupingSeparator;
    hash = hash * 37 + mDecimalSeparator;
    hash = hash * 37 + mPercent;
    hash = hash * 37 + mPerMill;
    hash = hash * 37 + mDigit;
    hash = hash * 37 + mMinusSign;
    hash = hash * 37 + mPatternSeparator;
    hash = hash * 37 + mInfinity.GetHashCode();
    hash = hash * 37 + mNaN.GetHashCode();
    hash = hash * 37 + mCurrencySymbol.GetHashCode();
    hash = hash * 37 + mIntlCurrencySymbol.GetHashCode();
    hash = hash * 37 + Object::GetHashCode(mCurrency);
    hash = hash * 37 + mMonetarySeparator;
    hash = hash * 37 + mExponentialSeparator.GetHashCode();
    hash = hash * 37 + Object::GetHashCode(mLocale);
    return NOERROR;
}


void DecimalFormatSymbols::Initialize(
    /* [in] */ ILocale* _locale)
{
    AutoPtr<ILocale> locale = _locale;
    mLocale = locale;

    Boolean needCacheUpdate = false;
    AutoPtr<IInterface> value;
    GetCachedLocaleData()->Get(locale, value);
    Array<IInterface*> data;
    if (value != nullptr) {
        data = CoreUtils::Unbox(IArray::Probe(value));
    }
    if (data.IsNull()) {
        locale = LocaleData::MapInvalidAndNullLocales(locale);
        AutoPtr<ILocaleData> localeData;
        LocaleData::Get(locale, &localeData);
        data = Array<IInterface*>(3);
        Array<String> values(11);
        Char separator;
        localeData->GetDecimalSeparator(&separator);
        values[0] = String::ValueOf(separator);
        localeData->GetGroupingSeparator(&separator);
        values[1] = String::ValueOf(separator);
        localeData->GetPatternSeparator(&separator);
        values[2] = String::ValueOf(separator);
        String percent;
        localeData->GetPercent(&percent);
        values[3] = percent;
        Char zero;
        localeData->GetZeroDigit(&zero);
        values[4] = String::ValueOf(zero);
        values[5] = "#";
        String minusSign;
        localeData->GetMinusSign(&minusSign);
        values[6] = minusSign;
        String expSeperator;
        localeData->GetExponentSeparator(&expSeperator);
        values[7] = expSeperator;
        Char perMill;
        localeData->GetPerMill(&perMill);
        values[8] = String::ValueOf(perMill);
        String infinity;
        localeData->GetInfinity(&infinity);
        values[9] = infinity;
        String nan;
        localeData->GetNaN(&nan);
        values[10] = nan;
        data.Set(0, new StringArrayWrapper(values));
        needCacheUpdate = true;
    }

    Array<String> numberElements = ((StringArrayWrapper*)data[0])->mValues;

    mDecimalSeparator = numberElements[0].GetChar(0);
    mGroupingSeparator = numberElements[1].GetChar(0);
    mPatternSeparator = numberElements[2].GetChar(0);
    mPercent = MaybeStripMarkers(numberElements[3], U'%');
    mZeroDigit = numberElements[4].GetChar(0);  //different for Arabic,etc.
    mDigit = numberElements[5].GetChar(0);
    mMinusSign = MaybeStripMarkers(numberElements[6], U'-');
    mExponential = numberElements[7].GetChar(0);
    mExponentialSeparator = numberElements[7];
    mPerMill = numberElements[8].GetChar(0);
    mInfinity = numberElements[9];
    mNaN = numberElements[10];

    // Try to obtain the currency used in the locale's country.
    // Check for empty country string separately because it's a valid
    // country ID for Locale (and used for the C locale), but not a valid
    // ISO 3166 country code, and exceptions are expensive.
    String country;
    locale->GetCountry(&country);
    if (!country.Equals("")) {
        mCurrency = Currency::GetInstance(locale);
    }
    if (mCurrency != nullptr) {
        mCurrency->GetCurrencyCode(&mIntlCurrencySymbol);
        if (data[1] != nullptr && CoreUtils::Unbox(
                ICharSequence::Probe(data[1])).Equals(mIntlCurrencySymbol)) {
            mCurrencySymbol = CoreUtils::Unbox(ICharSequence::Probe(data[2]));
        }
        else {
            mCurrency->GetSymbol(locale, &mCurrencySymbol);
            data.Set(1, CoreUtils::Box(mIntlCurrencySymbol));
            data.Set(2, CoreUtils::Box(mCurrencySymbol));
            needCacheUpdate = true;
        }
    }
    else {
        // default values
        mIntlCurrencySymbol = "XXX";
        mCurrency = Currency::GetInstance(mIntlCurrencySymbol);
        mCurrencySymbol = String::ValueOf((Char)0x00A4);
    }
    // Currently the monetary decimal separator is the same as the
    // standard decimal separator for all locales that we support.
    // If that changes, add a new entry to NumberElements.
    mMonetarySeparator = mDecimalSeparator;

    if (needCacheUpdate) {
        GetCachedLocaleData()->PutIfAbsent(locale, CoreUtils::Box(data));
    }
}

Char DecimalFormatSymbols::MaybeStripMarkers(
    /* [in] */ const String& symbol,
    /* [in] */ Char fallback)
{
    Integer length = symbol.GetLength();
    if (length >= 1) {
        Boolean sawNonMarker = false;
        Char nonMarker = 0;
        for (Integer i = 0; i < length; i++) {
            Char c = symbol.GetChar(i);
            if (c == 0x200E || c == 0x200F || c == 0x061C) {
                continue;
            }
            if (sawNonMarker) {
                // More than one non-marker character.
                return fallback;
            }
            sawNonMarker = true;
            nonMarker = c;
        }
        if (sawNonMarker) {
            return nonMarker;
        }
    }
    return fallback;
}

}
}
