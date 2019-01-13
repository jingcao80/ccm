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

#ifndef __CCM_TEXT_SIMPLEDATEFORMAT_H__
#define __CCM_TEXT_SIMPLEDATEFORMAT_H__

#include "ccm/text/CalendarBuilder.h"
#include "ccm/text/DateFormat.h"
#include "ccm.core.IStringBuilder.h"
#include "ccm.core.IStringBuffer.h"
#include "ccm.text.IFieldPosition.h"
#include "ccm.text.IFormatFieldDelegate.h"
#include "ccm.text.ISimpleDateFormat.h"
#include "ccm.util.ISet.h"
#include "ccm.util.concurrent.IConcurrentMap.h"

using ccm::core::IStringBuilder;
using ccm::core::IStringBuffer;
using ccm::util::ISet;
using ccm::util::concurrent::IConcurrentMap;

namespace ccm {
namespace text {

class SimpleDateFormat
    : public DateFormat
    , public ISimpleDateFormat
{
public:
    CCM_INTERFACE_DECL();

    ECode Constructor();

    ECode Constructor(
        /* [in] */ const String& pattern);

    ECode Constructor(
        /* [in] */ const String& pattern,
        /* [in] */ ILocale* locale);

    ECode Constructor(
        /* [in] */ const String& pattern,
        /* [in] */ IDateFormatSymbols* formatSymbols);

    ECode Set2DigitYearStart(
        /* in] */ IDate* startDate) override;

    ECode Get2DigitYearStart(
        /* [out] */ IDate** startDate) override;

    ECode Format(
        /* [in] */ IDate* date,
        /* [in, out] */ IStringBuffer* toAppendTo,
        /* [in] */ IFieldPosition* pos) override;

    ECode FormatToCharacterIterator(
        /* [in] */ IInterface* obj,
        /* [out] */ IAttributedCharacterIterator** it) override;

protected:
    ECode Constructor(
        /* [in] */ Integer timeStyle,
        /* [in] */ Integer dateStyle,
        /* [in] */ ILocale* locale);

private:
    ECode Initialize(
        /* [in] */ ILocale* locale);

    void InitializeCalendar(
        /* [in] */ ILocale* locale);

    ECode Compile(
        /* [in] */ const String& pattern,
        /* [out, callee] */ Array<Char>* compiledFormat);

    static ECode Encode(
        /* [in] */ Integer tag,
        /* [in] */ Integer length,
        /* [in] */ IStringBuilder* buffer);

    void InitializeDefaultCentury();

    void ParseAmbiguousDatesAsAfter(
        /* [in] */ IDate* startDate);

    ECode Format(
        /* [in] */ IDate* date,
        /* [in, out] */ IStringBuffer* toAppendTo,
        /* [in] */ IFormatFieldDelegate* delegate);

    ECode SubFormat(
        /* [in] */ Integer patternCharIndex,
        /* [in] */ Integer count,
        /* [in] */ IFormatFieldDelegate* delegate,
        /* [in] */ IStringBuffer* buffer,
        /* [in] */ Boolean useDateFormatSymbols);


    static AutoPtr<IConcurrentMap> GetCachedNumberFormatData();

    static AutoPtr<ISet> GetUTC_ZONE_IDS();



    Boolean UseDateFormatSymbols()
    {
        return false;
    }

    /**
     * Formats a number with the specified minimum and maximum number of digits.
     */
    void ZeroPaddingNumber(
        /* [in] */ Integer value,
        /* [in] */ Integer minDigits,
        /* [in] */ Integer maxDigits,
        /* [in] */ IStringBuffer* buffer)
    {}

    String FormatWeekday(
        /* [in] */ Integer count,
        /* [in] */ Integer value,
        /* [in] */ Boolean useDateFormatSymbols,
        /* [in] */ Boolean standalone)
    {
        return String(nullptr);
    }

    String FormatMonth(
        /* [in] */ Integer count,
        /* [in] */ Integer value,
        /* [in] */ Integer maxIntCount,
        /* [in] */ IStringBuffer* buffer,
        /* [in] */ Boolean useDateFormatSymbols,
        /* [in] */ Boolean standalone)
    {
        return String(nullptr);
    }

protected:
    /**
     * Indicates whether this <code>SimpleDateFormat</code> should use
     * the DateFormatSymbols. If true, the format and parse methods
     * use the DateFormatSymbols values. If false, the format and
     * parse methods call Calendar.getDisplayName or
     * Calendar.getDisplayNames.
     */
    Boolean mUseDateFormatSymbols = false;

private:
    /**
     * The pattern string of this formatter.  This is always a non-localized
     * pattern.
     */
    String mPattern;

    /**
     * The compiled pattern.
     */
    Array<Char> mCompiledPattern;

    /**
     * Tags for the compiled pattern.
     */
    const static Integer TAG_QUOTE_ASCII_CHAR = 100;
    const static Integer TAG_QUOTE_CHARS = 101;

    /**
     * The symbols used by this formatter for week names, month names,
     * etc.
     */
    AutoPtr<IDateFormatSymbols> mFormatData;

    /**
     * We map dates with two-digit years into the century starting at
     * <code>defaultCenturyStart</code>, which may be any date.
     */
    AutoPtr<IDate> mDefaultCenturyStart;

    Integer mDefaultCenturyStartYear;

    /**
     * The Locale used to instantiate this
     * <code>SimpleDateFormat</code>. The value may be null if this object
     * has been created by an older <code>SimpleDateFormat</code> and
     * deserialized.
     */
    AutoPtr<ILocale> mLocale;

    // Map index into pattern character string to Calendar field number
    static constexpr Integer PATTERN_INDEX_TO_CALENDAR_FIELD[] = {
        ICalendar::ERA,
        ICalendar::YEAR,
        ICalendar::MONTH,
        ICalendar::DATE,
        ICalendar::HOUR_OF_DAY,
        ICalendar::HOUR_OF_DAY,
        ICalendar::MINUTE,
        ICalendar::SECOND,
        ICalendar::MILLISECOND,
        ICalendar::DAY_OF_WEEK,
        ICalendar::DAY_OF_YEAR,
        ICalendar::DAY_OF_WEEK_IN_MONTH,
        ICalendar::WEEK_OF_YEAR,
        ICalendar::WEEK_OF_MONTH,
        ICalendar::AM_PM,
        ICalendar::HOUR,
        ICalendar::HOUR,
        ICalendar::ZONE_OFFSET,
        ICalendar::ZONE_OFFSET,
        CalendarBuilder::WEEK_YEAR,         // Pseudo Calendar field
        CalendarBuilder::ISO_DAY_OF_WEEK,   // Pseudo Calendar field
        ICalendar::ZONE_OFFSET,
        // 'L' and 'c',
        ICalendar::MONTH,
        ICalendar::DAY_OF_WEEK
    };

    // Map index into pattern character string to DateFormat field number
    static constexpr Integer PATTERN_INDEX_TO_DATE_FORMAT_FIELD[] = {
        IDateFormat::ERA_FIELD,
        IDateFormat::YEAR_FIELD,
        IDateFormat::MONTH_FIELD,
        IDateFormat::DATE_FIELD,
        IDateFormat::HOUR_OF_DAY1_FIELD,
        IDateFormat::HOUR_OF_DAY0_FIELD,
        IDateFormat::MINUTE_FIELD,
        IDateFormat::SECOND_FIELD,
        IDateFormat::MILLISECOND_FIELD,
        IDateFormat::DAY_OF_WEEK_FIELD,
        IDateFormat::DAY_OF_YEAR_FIELD,
        IDateFormat::DAY_OF_WEEK_IN_MONTH_FIELD,
        IDateFormat::WEEK_OF_YEAR_FIELD,
        IDateFormat::WEEK_OF_MONTH_FIELD,
        IDateFormat::AM_PM_FIELD,
        IDateFormat::HOUR1_FIELD,
        IDateFormat::HOUR0_FIELD,
        IDateFormat::TIMEZONE_FIELD,
        IDateFormat::TIMEZONE_FIELD,
        IDateFormat::YEAR_FIELD,
        IDateFormat::DAY_OF_WEEK_FIELD,
        IDateFormat::TIMEZONE_FIELD,
        // 'L' and 'c'
        IDateFormat::MONTH_FIELD,
        IDateFormat::DAY_OF_WEEK_FIELD
    };

    static const IDateFormatField* PATTERN_INDEX_TO_DATE_FORMAT_FIELD_ID[24];

    static const String UTC;
};

}
}

#endif // __CCM_TEXT_SIMPLEDATEFORMAT_H__
