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
#include "ccm.text.IDateFormatSymbols.h"
#include "ccm.text.IFieldPosition.h"
#include "ccm.text.IFormatFieldDelegate.h"
#include "ccm.text.INumberFormat.h"
#include "ccm.text.ISimpleDateFormat.h"
#include "ccm.util.ISet.h"
#include "ccm.util.concurrent.IConcurrentMap.h"
#include "pisces.icu.text.ITimeZoneNames.h"

using ccm::core::IStringBuilder;
using ccm::core::IStringBuffer;
using ccm::util::ISet;
using ccm::util::concurrent::IConcurrentMap;
using pisces::icu::text::ITimeZoneNames;

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

    ECode Parse(
        /* [in] */ const String& text,
        /* [in] */ IParsePosition* pos,
        /* [out] */ IDate** date) override;

    ECode ToPattern(
        /* [out] */ String* pattern) override;

    ECode ToLocalizedPattern(
        /* [out] */ String* pattern) override;

    ECode ApplyPattern(
        /* [in] */ const String& pattern) override;

    ECode ApplyLocalizedPattern(
        /* [in] */ const String& pattern) override;

    ECode SetDateFormatSymbols(
        /* [in] */ IDateFormatSymbols* newFormatSymbols) override;

    ECode GetHashCode(
        /* [out] */ Integer* hash) override;

    ECode Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* same) override;

protected:
    ECode Constructor(
        /* [in] */ Integer timeStyle,
        /* [in] */ Integer dateStyle,
        /* [in] */ ILocale* locale);

    ECode CloneImpl(
        /* [in] */ SimpleDateFormat* newObj);

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

    String FormatWeekday(
        /* [in] */ Integer count,
        /* [in] */ Integer value,
        /* [in] */ Boolean useDateFormatSymbols,
        /* [in] */ Boolean standalone);

    String FormatMonth(
        /* [in] */ Integer count,
        /* [in] */ Integer value,
        /* [in] */ Integer maxIntCount,
        /* [in] */ IStringBuffer* buffer,
        /* [in] */ Boolean useDateFormatSymbols,
        /* [in] */ Boolean standalone);

    /**
     * Formats a number with the specified minimum and maximum number of digits.
     */
    void ZeroPaddingNumber(
        /* [in] */ Integer value,
        /* [in] */ Integer minDigits,
        /* [in] */ Integer maxDigits,
        /* [in] */ IStringBuffer* buffer);

    ECode ParseInternal(
        /* [in] */ const String& text,
        /* [in] */ IParsePosition* pos,
        /* [out] */ IDate** date);

    Integer MatchString(
        /* [in] */ const String& text,
        /* [in] */ Integer start,
        /* [in] */ Integer field,
        /* [in] */ Array<String>& data,
        /* [in] */ CalendarBuilder* calb);

    Integer MatchString(
        /* [in] */ const String& text,
        /* [in] */ Integer start,
        /* [in] */ Integer field,
        /* [in] */ IMap* data,
        /* [in] */ CalendarBuilder* calb);

    Integer MatchZoneString(
        /* [in] */ const String& text,
        /* [in] */ Integer start,
        /* [in] */ Array<String>& zoneNames);

    Integer SubParseZoneString(
        /* [in] */ const String& text,
        /* [in] */ Integer start,
        /* [in] */ CalendarBuilder* calb);

    AutoPtr<ITimeZoneNames> GetTimeZoneNames();

    Integer SubParseZoneStringFromICU(
        /* [in] */ const String& text,
        /* [in] */ Integer start,
        /* [in] */ CalendarBuilder* calb);

    Integer SubParseZoneStringFromSymbols(
        /* [in] */ const String& text,
        /* [in] */ Integer start,
        /* [in] */ CalendarBuilder* calb);

    Integer SubParseNumericZone(
        /* [in] */ const String& text,
        /* [in] */ Integer start,
        /* [in] */ Integer sign,
        /* [in] */ Integer count,
        /* [in] */ Boolean colonRequired,
        /* [in] */ CalendarBuilder* calb);

    Boolean IsDigit(
        /* [in] */ Char c);

    Integer SubParse(
        /* [in] */ const String& text,
        /* [in] */ Integer start,
        /* [in] */ Integer patternCharIndex,
        /* [in] */ Integer count,
        /* [in] */ Boolean obeyCount,
        /* [in] */ Array<Boolean>& ambiguousYear,
        /* [in] */ IParsePosition* origPos,
        /* [in] */ Boolean useFollowingMinusSignAsDelimiter,
        /* [in] */ CalendarBuilder* calb);

    Integer ParseMonth(
        /* [in] */ const String& text,
        /* [in] */ Integer count,
        /* [in] */ Integer value,
        /* [in] */ Integer start,
        /* [in] */ Integer field,
        /* [in] */ IParsePosition* pos,
        /* [in] */ Boolean useDateFormatSymbols,
        /* [in] */ Boolean standalone,
        /* [in] */ CalendarBuilder* out);

    Integer ParseWeekday(
        /* [in] */ const String& text,
        /* [in] */ Integer start,
        /* [in] */ Integer field,
        /* [in] */ Boolean useDateFormatSymbols,
        /* [in] */ Boolean standalone,
        /* [in] */ CalendarBuilder* out);

    String GetCalendarName();

    Boolean UseDateFormatSymbols();

    Boolean IsGregorianCalendar();

    ECode TranslatePattern(
        /* [in] */ const String& pattern,
        /* [in] */ const String& from,
        /* [in] */ const String& to,
        /* [out] */ String* result);

    void CheckNegativeNumberExpression();

    static AutoPtr<IConcurrentMap> GetCachedNumberFormatData();

    static AutoPtr<ISet> GetUTC_ZONE_IDS();

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

    AutoPtr<INumberFormat> mOriginalNumberFormat;
    String mOriginalNumberPattern;

    /**
     * The minus sign to be used with format and parse.
     */
    Char mMinusSign = U'-';

    /**
     * True when a negative sign follows a number.
     * (True as default in Arabic.)
     */
    Boolean mHasFollowingMinusSign = false;

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
     * Locale dependent digit zero.
     */
    Char mZeroDigit = 0;

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

    Integer mDefaultCenturyStartYear = 0;

    static constexpr Integer MILLIS_PER_MINUTE = 60 * 1000;

    // For time zones that have no names, use strings GMT+minutes and
    // GMT-minutes. For instance, in France the time zone is GMT+60.
    static const String GMT;

    /**
     * The Locale used to instantiate this
     * <code>SimpleDateFormat</code>. The value may be null if this object
     * has been created by an older <code>SimpleDateFormat</code> and
     * deserialized.
     */
    AutoPtr<ILocale> mLocale;

    /**
     * ICU TimeZoneNames used to format and parse time zone names.
     */
    AutoPtr<ITimeZoneNames> mTimeZoneNames;

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
