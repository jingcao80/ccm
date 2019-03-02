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

#ifndef __LIBCORE_ICU_LOCALEDATE_H__
#define __LIBCORE_ICU_LOCALEDATE_H__

#include "ccm/core/SyncObject.h"
#include "ccm.core.IInteger.h"
#include "ccm.util.IHashMap.h"
#include "ccm.util.ILocale.h"
#include "libcore.icu.ILocaleData.h"
#include <ccmautoptr.h>

using ccm::core::IInteger;
using ccm::core::SyncObject;
using ccm::util::IHashMap;
using ccm::util::ILocale;
using libcore::icu::ILocaleData;

namespace libcore {
namespace icu {

class LocaleData
    : public SyncObject
    , public ILocaleData
{
public:
    CCM_INTERFACE_DECL();

    static AutoPtr<ILocale> MapInvalidAndNullLocales(
        /* [in] */ ILocale* locale);

    static ECode Get(
        /* [in] */ ILocale* locale,
        /* [out] */ ILocaleData** data);

    ECode ToString(
        /* [out] */ String* desc) override;

    ECode GetDateFormat(
        /* [in] */ Integer style,
        /* [out] */ String* dateFormat) override;

    ECode GetTimeFormat(
        /* [in] */ Integer style,
        /* [out] */ String* timeFormat) override;

    ECode GetCurrencySymbol(
        /* [out] */ String* currencySymbol) override;

    ECode GetDecimalSeparator(
        /* [out] */ Char* decSeparator) override;

    ECode GetExponentSeparator(
        /* [out] */ String* expSeparator) override;

    ECode GetGroupingSeparator(
        /* [out] */ Char* grpSeparator) override;

    ECode GetFirstDayOfWeek(
        /* [out] */ IInteger** day) override;

    ECode GetInfinity(
        /* [out] */ String* infinity) override;

    ECode GetInternationalCurrencySymbol(
        /* [out] */ String* intlCurrencySymbol) override;

    ECode GetMinimalDaysInFirstWeek(
        /* [out] */ IInteger** days) override;

    ECode GetMinusSign(
        /* [out] */ String* sign) override;

    ECode GetNaN(
        /* [out] */ String* nan) override;

    ECode GetPatternSeparator(
        /* [out] */ Char* patSeparator) override;

    ECode GetPercent(
        /* [out] */ String* percent) override;

    ECode GetPerMill(
        /* [out] */ Char* perMill) override;

    ECode GetZeroDigit(
        /* [out] */ Char* zeroDigit) override;

private:
    inline LocaleData() {}

    static ECode GetInner(
        /* [in] */ ILocale* locale,
        /* [out] */ ILocaleData** data);

    static ECode InitLocaleData(
        /* [in] */ ILocale* locale,
        /* [out] */ ILocaleData** localeData);

    static AutoPtr<IHashMap> GetLocaleDataCache();

    static Boolean StaticInitialize();

public:
    AutoPtr<IInteger> mFirstDayOfWeek;
    AutoPtr<IInteger> mMinimalDaysInFirstWeek;

    Array<String> mAmPm; // "AM", "PM".
    Array<String> mEras; // "BC", "AD".

    Array<String> mLongMonthNames; // "January", ...
    Array<String> mShortMonthNames; // "Jan", ...
    Array<String> mTinyMonthNames; // "J", ...
    Array<String> mLongStandAloneMonthNames; // "January", ...
    Array<String> mShortStandAloneMonthNames; // "Jan", ...
    Array<String> mTinyStandAloneMonthNames; // "J", ...

    Array<String> mLongWeekdayNames; // "Sunday", ...
    Array<String> mShortWeekdayNames; // "Sun", ...
    Array<String> mTinyWeekdayNames; // "S", ...
    Array<String> mLongStandAloneWeekdayNames; // "Sunday", ...
    Array<String> mShortStandAloneWeekdayNames; // "Sun", ...
    Array<String> mTinyStandAloneWeekdayNames; // "S", ...

    String mYesterday; // "Yesterday".
    String mToday; // "Today".
    String mTomorrow; // "Tomorrow".

    String mFullTimeFormat;
    String mLongTimeFormat;
    String mMediumTimeFormat;
    String mShortTimeFormat;

    String mFullDateFormat;
    String mLongDateFormat;
    String mMediumDateFormat;
    String mShortDateFormat;

    String mNarrowAm; // "a".
    String mNarrowPm; // "p".

    String mTimeFormat_hm;
    String mTimeFormat_Hm;
    String mTimeFormat_hms;
    String mTimeFormat_Hms;

    Char mZeroDigit;
    Char mDecimalSeparator;
    Char mGroupingSeparator;
    Char mPatternSeparator;
    String mPercent;
    Char mPerMill;
    Char mMonetarySeparator;
    String mMinusSign;
    String mExponentSeparator;
    String mInfinity;
    String mNaN;

    String mCurrencySymbol;
    String mInternationalCurrencySymbol;

    String mNumberPattern;
    String mIntegerPattern;
    String mCurrencyPattern;
    String mPercentPattern;
};

}
}

#endif // __LIBCORE_ICU_LOCALEDATE_H__
