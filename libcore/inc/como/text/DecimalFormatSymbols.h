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

#ifndef __COMO_TEXT_DECIMALFORMATSYMBOLS_H__
#define __COMO_TEXT_DECIMALFORMATSYMBOLS_H__

#include "como/core/SyncObject.h"
#include "como.core.ICloneable.h"
#include "como.io.ISerializable.h"
#include "como.text.IDecimalFormatSymbols.h"
#include "como.util.ICurrency.h"
#include "como.util.ILocale.h"
#include "como.util.concurrent.IConcurrentHashMap.h"

using como::core::ICloneable;
using como::core::SyncObject;
using como::io::ISerializable;
using como::util::ICurrency;
using como::util::ILocale;
using como::util::concurrent::IConcurrentHashMap;

namespace como {
namespace text {

class DecimalFormatSymbols
    : public SyncObject
    , public IDecimalFormatSymbols
    , public ICloneable
    , public ISerializable
{
public:
    COMO_INTERFACE_DECL();

    ECode Constructor();

    ECode Constructor(
        /* [in] */ ILocale* locale);

    static Array<ILocale*> GetAvailableLocales();

    static AutoPtr<IDecimalFormatSymbols> GetInstance();

    static AutoPtr<IDecimalFormatSymbols> GetInstance(
        /* [in] */ ILocale* locale);

    ECode GetZeroDigit(
        /* [out] */ Char& zeroDigit) override;

    ECode SetZeroDigit(
        /* [in] */ Char zeroDigit) override;

    ECode GetGroupingSeparator(
        /* [out] */ Char& groupingSeparator) override;

    ECode SetGroupingSeparator(
        /* [in] */ Char groupingSeparator) override;

    ECode GetDecimalSeparator(
        /* [out] */ Char& decimalSeparator) override;

    ECode SetDecimalSeparator(
        /* [in] */ Char decimalSeparator) override;

    ECode GetPerMill(
        /* [out] */ Char& perMill) override;

    ECode SetPerMill(
        /* [in] */ Char perMill) override;

    ECode GetPercent(
        /* [out] */ Char& percent) override;

    ECode GetPercentString(
        /* [out] */ String& percentStr) override;

    ECode SetPercent(
        /* [in] */ Char percent) override;

    ECode GetDigit(
        /* [out] */ Char& digit) override;

    ECode SetDigit(
        /* [in] */ Char digit) override;

    ECode GetPatternSeparator(
        /* [out] */ Char& patternSeparator) override;

    ECode SetPatternSeparator(
        /* [in] */ Char patternSeparator) override;

    ECode GetInfinity(
        /* [out] */ String& infinity) override;

    ECode SetInfinity(
        /* [in] */ const String& infinity) override;

    ECode GetNaN(
        /* [out] */ String& naN) override;

    ECode SetNaN(
        /* [in] */ const String& naN) override;

    ECode GetMinusSign(
        /* [out] */ Char& minusSign) override;

    ECode GetMinusSignString(
        /* [out] */ String& minusSignStr) override;

    ECode SetMinusSign(
        /* [in] */ Char minusSign) override;

    ECode GetCurrencySymbol(
        /* [out] */ String& currency) override;

    ECode SetCurrencySymbol(
        /* [in] */ const String& currency) override;

    ECode GetInternationalCurrencySymbol(
        /* [out] */ String& currency) override;

    ECode SetInternationalCurrencySymbol(
        /* [in] */ const String& currency) override;

    ECode GetCurrency(
        /* [out] */ AutoPtr<ICurrency>& currency) override;

    ECode SetCurrency(
        /* [in] */ ICurrency* currency) override;

    ECode GetMonetaryDecimalSeparator(
        /* [out] */ Char& monetarySeparator) override;

    ECode SetMonetaryDecimalSeparator(
        /* [in] */ Char sep) override;

    virtual Char GetExponentialSymbol();

    ECode GetExponentSeparator(
        /* [out] */ String& exponentSeparator) override;

    virtual void SetExponentialSymbol(
        /* [in] */ Char exp);

    ECode SetExponentSeparator(
        /* [in] */ const String& exp) override;

    ECode Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean& same) override;

    ECode GetHashCode(
        /* [out] */ Integer& hash) override;

    static Char MaybeStripMarkers(
        /* [in] */ const String& symbol,
        /* [in] */ Char fallback);

protected:
    ECode CloneImpl(
        /* [in] */ IDecimalFormatSymbols* newObj);

private:
    void Initialize(
        /* [in] */ ILocale* locale);

    static AutoPtr<IConcurrentHashMap> GetCachedLocaleData();

private:
    Char mZeroDigit;

    Char mGroupingSeparator;

    Char mDecimalSeparator;

    Char mPerMill;

    Char mPercent;

    Char mDigit;

    Char mPatternSeparator;

    String mInfinity;

    String mNaN;

    Char mMinusSign;

    String mCurrencySymbol;

    String mIntlCurrencySymbol;

    Char mMonetarySeparator;

    Char mExponential;

    String mExponentialSeparator;

    AutoPtr<ILocale> mLocale;

    AutoPtr<ICurrency> mCurrency;
};

}
}

#endif // __COMO_TEXT_DECIMALFORMATSYMBOLS_H__
