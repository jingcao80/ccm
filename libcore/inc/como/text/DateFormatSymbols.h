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

#ifndef __COMO_TEXT_DATEFORMATSYMBOLS_H__
#define __COMO_TEXT_DATEFORMATSYMBOLS_H__

#include "como/core/SyncObject.h"
#include "como/core/volatile.h"
#include "como.core.ICloneable.h"
#include "como.io.ISerializable.h"
#include "como.text.IDateFormatSymbols.h"
#include "como.util.ILocale.h"
#include "como.util.concurrent.IConcurrentMap.h"
#include "libcore.icu.ILocaleData.h"
#include <comosp.h>

using como::core::SyncObject;
using como::core::ICloneable;
using como::io::ISerializable;
using como::util::ILocale;
using como::util::concurrent::IConcurrentMap;
using libcore::icu::ILocaleData;

namespace como {
namespace text {

class DateFormatSymbols
    : public SyncObject
    , public IDateFormatSymbols
    , public ISerializable
    , public ICloneable
{
public:
    COMO_INTERFACE_DECL();

    ECode Constructor();

    ECode Constructor(
        /* [in] */ ILocale* locale);

    static Array<ILocale*> GetAvailableLocales();

    static AutoPtr<IDateFormatSymbols> GetInstance();

    static AutoPtr<IDateFormatSymbols> GetInstance(
        /* [in] */ ILocale* locale);

    static AutoPtr<IDateFormatSymbols> GetInstanceRef(
        /* [in] */ ILocale* locale);

    ECode GetEras(
        /* [out, callee] */ Array<String>* eras) override;

    ECode SetEras(
        /* [in] */ const Array<String>& newEras) override;

    ECode GetMonths(
        /* [out, callee] */ Array<String>* months) override;

    ECode SetMonths(
        /* [in] */ const Array<String>& newMonths) override;

    ECode GetShortMonths(
        /* [out, callee] */ Array<String>* months) override;

    ECode SetShortMonths(
        /* [in] */ const Array<String>& newShortMonths) override;

    ECode GetWeekdays(
        /* [out, callee] */ Array<String>* weekdays) override;

    ECode SetWeekdays(
        /* [in] */ const Array<String>& newWeekdays) override;

    ECode GetShortWeekdays(
        /* [out, callee] */ Array<String>* weekdays) override;

    ECode SetShortWeekdays(
        /* [in] */ const Array<String>& newShortWeekdays) override;

    ECode GetAmPmStrings(
        /* [out, callee] */ Array<String>* ampm) override;

    ECode SetAmPmStrings(
        /* [in] */ const Array<String>& newAmpms) override;

    ECode GetZoneStrings(
        /* [out, callee] */ Array<Array<String>>* zoneStrings) override;

    ECode SetZoneStrings(
        /* [in] */ const Array<Array<String>>& newZoneStrings) override;

    ECode GetLocalPatternChars(
        /* [out] */ String* localPatternChars) override;

    ECode SetLocalPatternChars(
        /* [in] */ const String& newLocalPatternChars) override;

    Array<String> GetTinyMonths();

    Array<String> GetStandAloneMonths();

    Array<String> GetShortStandAloneMonths();

    Array<String> GetTinyStandAloneMonths();

    Array<String> GetTinyWeekdays();

    Array<String> GetStandAloneWeekdays();

    Array<String> GetShortStandAloneWeekdays();

    Array<String> GetTinyStandAloneWeekdays();

    ECode GetHashCode(
        /* [out] */ Integer& hash) override;

    ECode Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean& same) override;

    Integer GetZoneIndex(
        /* [in] */ const String& ID);

    Array<Array<String>> GetZoneStringsWrapper();

    static DateFormatSymbols* From(
        /* [in] */ IDateFormatSymbols* obj);

protected:
    ECode CloneImpl(
        /* [in] */ IDateFormatSymbols* newObj);

private:
    static AutoPtr<IDateFormatSymbols> GetCachedInstance(
        /* [in] */ ILocale* locale);

    void InitializeData(
        /* [in] */ ILocale* desiredLocale);

    void InitializeSupplementaryData(
        /* [in] */ ILocaleData* localeData);

    Array<Array<String>> InternalZoneStrings();

    Array<Array<String>> GetZoneStringsImpl(
        /* [in] */ Boolean needsCopy);

    void CopyMembers(
        /* [in] */ DateFormatSymbols* src,
        /* [in] */ DateFormatSymbols* dst);

    static AutoPtr<IConcurrentMap> GetCachedInstances();

public:
    /**
     * Era strings. For example: "AD" and "BC".
     */
    Array<String> mEras;

    /**
     * Month strings. For example: "January", "February", etc.
     */
    Array<String> mMonths;

    /**
     * Short month strings. For example: "Jan", "Feb", etc.
     */
    Array<String> mShortMonths;

    /**
     * Weekday strings. For example: "Sunday", "Monday", etc.
     */
    Array<String> mWeekdays;

    /**
     * Short weekday strings. For example: "Sun", "Mon", etc.
     */
    Array<String> mShortWeekdays;

    /**
     * AM and PM strings. For example: "AM" and "PM".
     */
    Array<String> mAmpms;

    /**
     * Localized names of time zones in this locale.
     */
    Array<Array<String>> mZoneStrings;

    /**
     * Indicates that zoneStrings is set externally with setZoneStrings() method.
     */
    Boolean mIsZoneStringsSet = false;

    /**
     * Unlocalized date-time pattern characters. For example: 'y', 'd', etc.
     * All locales use the same these unlocalized pattern characters.
     *
     * Pretend to support 'L' and 'c' for now. It's meant for standalone weekday and
     * month names, but we just use the non-standalone versions for now.
     */
    static const String sPatternChars;

    static const Integer PATTERN_ERA = 0; // G
    static const Integer PATTERN_YEAR = 1; // y
    static const Integer PATTERN_MONTH = 2; // M
    static const Integer PATTERN_DAY_OF_MONTH = 3; // d
    static const Integer PATTERN_HOUR_OF_DAY1 = 4; // k
    static const Integer PATTERN_HOUR_OF_DAY0 = 5; // H
    static const Integer PATTERN_MINUTE = 6; // m
    static const Integer PATTERN_SECOND = 7; // s
    static const Integer PATTERN_MILLISECOND = 8; // S
    static const Integer PATTERN_DAY_OF_WEEK = 9; // E
    static const Integer PATTERN_DAY_OF_YEAR = 10; // D
    static const Integer PATTERN_DAY_OF_WEEK_IN_MONTH = 11; // F
    static const Integer PATTERN_WEEK_OF_YEAR = 12; // w
    static const Integer PATTERN_WEEK_OF_MONTH = 13; // W
    static const Integer PATTERN_AM_PM = 14; // a
    static const Integer PATTERN_HOUR1 = 15; // h
    static const Integer PATTERN_HOUR0 = 16; // K
    static const Integer PATTERN_ZONE_NAME = 17; // z
    static const Integer PATTERN_ZONE_VALUE = 18; // Z
    static const Integer PATTERN_WEEK_YEAR = 19; // Y
    static const Integer PATTERN_ISO_DAY_OF_WEEK = 20; // u
    static const Integer PATTERN_ISO_ZONE = 21; // X
    static const Integer PATTERN_MONTH_STANDALONE = 22; // L
    static const Integer PATTERN_STANDALONE_DAY_OF_WEEK = 23; // c

    /**
     * Localized date-time pattern characters.
     */
    String mLocalPatternChars;

    AutoPtr<ILocale> mLocale;

private:
    /**
     * Tiny month strings; "J", "F", "M" etc.
     */
    Array<String> mTinyMonths;

    /**
     * Tiny weekday strings: "M", "F", "W" etc.
     */
    Array<String> mTinyWeekdays;

    /**
     * Standalone month strings; "January", "February", "March" etc.
     */
    Array<String> mStandAloneMonths;

    /**
     * Short standalone month strings: "Jan", "Feb", "Mar" etc.
     */
    Array<String> mShortStandAloneMonths;

    /**
     * Tiny standalone month strings: "J", "F", "M" etc.
     */
    Array<String> mTinyStandAloneMonths;

    /**
     * Standalone weekday strings; "Monday", "Tuesday", "Wednesday" etc.
     */
    Array<String> mStandAloneWeekdays;

    /**
     * Short standalone weekday strings; "Mon", "Tue", "Wed" etc.
     */
    Array<String> mShortStandAloneWeekdays;

    /**
     * Tiny standalone weekday strings; "M", "T", "W" etc.
     */
    Array<String> mTinyStandAloneWeekdays;

    Integer mLastZoneIndex = 0;

    VOLATILE Integer mCachedHashCode = 0;
};

inline DateFormatSymbols* DateFormatSymbols::From(
    /* [in] */ IDateFormatSymbols* obj)
{
    return (DateFormatSymbols*)obj;
}

}
}

#endif // __COMO_TEXT_DATEFORMATSYMBOLS_H__
