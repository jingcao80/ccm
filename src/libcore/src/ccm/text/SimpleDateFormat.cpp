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

#include "ccm/text/SimpleDateFormat.h"
#include "ccm/core/CoreUtils.h"
#include "ccm/core/CStringBuffer.h"
#include "ccm/core/CStringBuilder.h"
#include "ccm/core/Math.h"
#include "ccm/core/System.h"
#include "ccm/text/CharacterIteratorFieldDelegate.h"
#include "ccm/text/DateFormatSymbols.h"
#include "ccm/text/MessageFormat.h"
#include "ccm/text/NumberFormat.h"
#include "ccm/util/Arrays.h"
#include "ccm/util/Calendar.h"
#include "ccm/util/CDate.h"
#include "ccm/util/CHashSet.h"
#include "ccm/util/Collections.h"
#include "ccm/util/Locale.h"
#include "ccm/util/TimeZone.h"
#include "ccm/util/calendar/CalendarUtils.h"
#include "ccm/util/concurrent/CConcurrentHashMap.h"
#include "ccm.core.INumber.h"
#include "ccm.util.ICollection.h"
#include "ccm.util.IGregorianCalendar.h"
#include "ccm.util.IList.h"
#include "ccm.util.IMap.h"
#include "libcore/icu/LocaleData.h"
#include "libcore/icu/TimeZoneNames.h"
#include "libcore.icu.ILocaleData.h"
#include <ccmlogger.h>

using ccm::core::CoreUtils;
using ccm::core::CStringBuffer;
using ccm::core::CStringBuilder;
using ccm::core::E_NULL_POINTER_EXCEPTION;
using ccm::core::IID_IStringBuffer;
using ccm::core::IID_IStringBuilder;
using ccm::core::INumber;
using ccm::core::Math;
using ccm::core::System;
using ccm::util::Arrays;
using ccm::util::Calendar;
using ccm::util::CDate;
using ccm::util::CHashSet;
using ccm::util::Collections;
using ccm::util::ICollection;
using ccm::util::IID_IDate;
using ccm::util::IID_ISet;
using ccm::util::IGregorianCalendar;
using ccm::util::IList;
using ccm::util::IMap;
using ccm::util::Locale;
using ccm::util::TimeZone;
using ccm::util::calendar::CalendarUtils;
using ccm::util::concurrent::CConcurrentHashMap;
using ccm::util::concurrent::IID_IConcurrentMap;
using libcore::icu::ILocaleData;
using libcore::icu::ITimeZoneNamesNameType;
using libcore::icu::LocaleData;
using libcore::icu::TimeZoneNames;

namespace ccm {
namespace text {

static AutoPtr<IConcurrentMap> CreateMap()
{
    AutoPtr<IConcurrentMap> map;
    CConcurrentHashMap::New(3, IID_IConcurrentMap, (IInterface**)&map);
    return map;
}

AutoPtr<IConcurrentMap> SimpleDateFormat::GetCachedNumberFormatData()
{
    /**
     * Cache NumberFormat instances with Locale key.
     */
    static const AutoPtr<IConcurrentMap> sCachedNumberFormatData = CreateMap();
    return sCachedNumberFormatData;
}

static AutoPtr<ISet> CreateSet()
{
    Array<String> zones(8);
    zones[0] = "Etc/UCT";
    zones[1] = "Etc/UTC";
    zones[2] = "Etc/Universal";
    zones[3] = "Etc/Zulu";
    zones[4] = "UCT";
    zones[5] = "UTC";
    zones[6] = "Universal";
    zones[7] = "Zulu";
    AutoPtr<IList> c;
    Arrays::AsList(CoreUtils::Box(zones).ToInterfaces(), &c);
    AutoPtr<ISet> s;
    CHashSet::New(ICollection::Probe(c), IID_ISet, (IInterface**)&s);
    return Collections::CreateUnmodifiableSet(s);
}

AutoPtr<ISet> SimpleDateFormat::GetUTC_ZONE_IDS()
{
    static const AutoPtr<ISet> UTC_ZONE_IDS = CreateSet();
    return UTC_ZONE_IDS;
}

constexpr Integer SimpleDateFormat::PATTERN_INDEX_TO_CALENDAR_FIELD[];
constexpr Integer SimpleDateFormat::PATTERN_INDEX_TO_DATE_FORMAT_FIELD[];
const IDateFormatField* SimpleDateFormat::PATTERN_INDEX_TO_DATE_FORMAT_FIELD_ID[24] =
{
    DateFormat::Field::GetERA(),
    DateFormat::Field::GetYEAR(),
    DateFormat::Field::GetMONTH(),
    DateFormat::Field::GetDAY_OF_MONTH(),
    DateFormat::Field::GetHOUR_OF_DAY1(),
    DateFormat::Field::GetHOUR_OF_DAY0(),
    DateFormat::Field::GetMINUTE(),
    DateFormat::Field::GetSECOND(),
    DateFormat::Field::GetMILLISECOND(),
    DateFormat::Field::GetDAY_OF_WEEK(),
    DateFormat::Field::GetDAY_OF_YEAR(),
    DateFormat::Field::GetDAY_OF_WEEK_IN_MONTH(),
    DateFormat::Field::GetWEEK_OF_YEAR(),
    DateFormat::Field::GetWEEK_OF_MONTH(),
    DateFormat::Field::GetAM_PM(),
    DateFormat::Field::GetHOUR1(),
    DateFormat::Field::GetHOUR0(),
    DateFormat::Field::GetTIME_ZONE(),
    DateFormat::Field::GetTIME_ZONE(),
    DateFormat::Field::GetYEAR(),
    DateFormat::Field::GetDAY_OF_WEEK(),
    DateFormat::Field::GetTIME_ZONE(),
    // 'L' and 'c'
    DateFormat::Field::GetMONTH(),
    DateFormat::Field::GetDAY_OF_WEEK()
};

const String SimpleDateFormat::UTC("UTC");

CCM_INTERFACE_IMPL_1(SimpleDateFormat, DateFormat, ISimpleDateFormat);

ECode SimpleDateFormat::Constructor()
{
    return Constructor(SHORT, SHORT, Locale::GetDefault(Locale::Category::GetFORMAT()));
}

ECode SimpleDateFormat::Constructor(
    /* [in] */ const String& pattern)
{
    return Constructor(pattern, Locale::GetDefault(Locale::Category::GetFORMAT()));
}

ECode SimpleDateFormat::Constructor(
    /* [in] */ const String& pattern,
    /* [in] */ ILocale* locale)
{
    if (pattern.IsNull() || locale == nullptr) {
        return E_NULL_POINTER_EXCEPTION;
    }

    InitializeCalendar(locale);
    mPattern = pattern;
    mFormatData = DateFormatSymbols::GetInstanceRef(locale);
    mLocale = locale;
    Initialize(locale);
    return NOERROR;
}

ECode SimpleDateFormat::Constructor(
    /* [in] */ const String& pattern,
    /* [in] */ IDateFormatSymbols* formatSymbols)
{
    if (pattern.IsNull() | formatSymbols == nullptr) {
        return E_NULL_POINTER_EXCEPTION;
    }

    mPattern = pattern;
    mFormatData = (IDateFormatSymbols*)CoreUtils::Clone(formatSymbols, IID_IDateFormatSymbols).Get();
    mLocale = Locale::GetDefault(Locale::Category::GetFORMAT());
    InitializeCalendar(mLocale);
    FAIL_RETURN(Initialize(mLocale));
    mUseDateFormatSymbols = true;
    return NOERROR;
}

ECode SimpleDateFormat::Constructor(
    /* [in] */ Integer timeStyle,
    /* [in] */ Integer dateStyle,
    /* [in] */ ILocale* locale)
{
    if (locale == nullptr) {
        return E_NULL_POINTER_EXCEPTION;
    }

    mLocale = locale;
    InitializeCalendar(locale);

    mFormatData = DateFormatSymbols::GetInstanceRef(locale);
    AutoPtr<ILocaleData> localeData;
    LocaleData::Get(locale, &localeData);
    if ((timeStyle >= 0) && (dateStyle >= 0)) {
        String df, tf;
        localeData->GetDateFormat(dateStyle, &df);
        localeData->GetTimeFormat(timeStyle, &tf);
        Array<IInterface*> dateTimeArgs(2);
        dateTimeArgs[0] = CoreUtils::Box(df);
        dateTimeArgs[1] = CoreUtils::Box(tf);
        MessageFormat::Format(String("{0} {1}"), dateTimeArgs, &mPattern);
    }
    else if (timeStyle >= 0) {
        localeData->GetTimeFormat(timeStyle, &mPattern);
    }
    else if (dateStyle >= 0) {
        localeData->GetDateFormat(dateStyle, &mPattern);
    }
    else {
        Logger::E("SimpleDateFormat", "No date or time style specified");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return Initialize(locale);
}

ECode SimpleDateFormat::Initialize(
    /* [in] */ ILocale* locale)
{
    FAIL_RETURN(Compile(mPattern, &mCompiledPattern));

    IMap::Probe(GetCachedNumberFormatData())->Get(locale, (IInterface**)&mNumberFormat);
    if (mNumberFormat == nullptr) {
        NumberFormat::GetIntegerInstance(locale, &mNumberFormat);
        mNumberFormat->SetGroupingUsed(false);

        GetCachedNumberFormatData()->PutIfAbsent(locale, mNumberFormat);
    }
    mNumberFormat = (INumberFormat*)CoreUtils::Clone(mNumberFormat, IID_INumberFormat).Get();

    InitializeDefaultCentury();
    return NOERROR;
}

void SimpleDateFormat::InitializeCalendar(
    /* [in] */ ILocale* locale)
{
    if (mCalendar == nullptr) {
        CHECK(locale != nullptr);
        // The format object must be constructed using the symbols for this zone.
        // However, the calendar should use the current default TimeZone.
        // If this is not contained in the locale zone strings, then the zone
        // will be formatted using generic GMT+/-H:MM nomenclature.
        mCalendar = Calendar::GetInstance(TimeZone::GetDefault(), locale);
    }
}

ECode SimpleDateFormat::Compile(
    /* [in] */ const String& pattern,
    /* [out, callee] */ Array<Char>* compiledFormat)
{
    VALIDATE_NOT_NULL(compiledFormat);

    Integer length = pattern.GetLength();
    Boolean inQuote = false;
    AutoPtr<IStringBuilder> compiledCode;
    CStringBuilder::New(length * 2, IID_IStringBuilder, (IInterface**)&compiledCode);
    AutoPtr<IStringBuilder> tmpBuffer;
    Integer count = 0;
    Integer lastTag = -1;

    for (Integer i = 0; i < length; i++) {
        Char c = pattern.GetChar(i);

        if (c == '\'') {
            // '' is treated as a single quote regardless of being
            // in a quoted section.
            if ((i + 1) < length) {
                c = pattern.GetChar(i + 1);
                if (c == '\'') {
                    i++;
                    if (count != 0) {
                        FAIL_RETURN(Encode(lastTag, count, compiledCode));
                        lastTag = -1;
                        count = 0;
                    }
                    if (inQuote) {
                        tmpBuffer->AppendChar(c);
                    }
                    else {
                        compiledCode->AppendChar((Char)(TAG_QUOTE_ASCII_CHAR << 8 | c));
                    }
                    continue;
                }
            }
            if (!inQuote) {
                if (count != 0) {
                    FAIL_RETURN(Encode(lastTag, count, compiledCode));
                    lastTag = -1;
                    count = 0;
                }
                if (tmpBuffer == nullptr) {
                    CStringBuilder::New(length, IID_IStringBuilder, (IInterface**)&tmpBuffer);
                }
                else {
                    tmpBuffer->SetLength(0);
                }
                inQuote = true;
            }
            else {
                Integer len;
                tmpBuffer->GetLength(&len);
                if (len == 1) {
                    Char ch;
                    tmpBuffer->GetCharAt(0, &ch);
                    if (ch < 128) {
                        compiledCode->AppendChar((Char)(TAG_QUOTE_ASCII_CHAR << 8 | ch));
                    }
                    else {
                        compiledCode->AppendChar((Char)(TAG_QUOTE_CHARS << 8 | 1));
                        compiledCode->AppendChar(ch);
                    }
                }
                else {
                    FAIL_RETURN(Encode(TAG_QUOTE_CHARS, len, compiledCode));
                    compiledCode->Append(tmpBuffer);
                }
                inQuote = false;
            }
            continue;
        }
        if (inQuote) {
            tmpBuffer->AppendChar(c);
            continue;
        }
        if (!(c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z')) {
            if (count != 0) {
                FAIL_RETURN(Encode(lastTag, count, compiledCode));
                lastTag = -1;
                count = 0;
            }
            if (c < 128) {
                // In most cases, c would be a delimiter, such as ':'.
                compiledCode->AppendChar((Char)(TAG_QUOTE_ASCII_CHAR << 8 | c));
            }
            else {
                // Take any contiguous non-ASCII alphabet characters and
                // put them in a single TAG_QUOTE_CHARS.
                Integer j;
                for (j = i + 1; j < length; j++) {
                    Char d = pattern.GetChar(j);
                    if (d == '\'' || (d >= 'a' && d <= 'z' || d >= 'A' && d <= 'Z')) {
                        break;
                    }
                }
                compiledCode->AppendChar((Char)(TAG_QUOTE_CHARS << 8 | (j - i)));
                for (; i < j; i++) {
                    compiledCode->AppendChar(pattern.GetChar(i));
                }
                i--;
            }
            continue;
        }

        Integer tag;
        if ((tag = DateFormatSymbols::sPatternChars.IndexOf(c)) == -1) {
            Logger::E("SimpleDateFormat", "Illegal pattern character '%c'", c);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        if (lastTag == -1 || lastTag == tag) {
            lastTag = tag;
            count++;
            continue;
        }
        FAIL_RETURN(Encode(lastTag, count, compiledCode));
        lastTag = tag;
        count = 1;
    }

    if (inQuote) {
        Logger::E("SimpleDateFormat", "Unterminated quote");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (count != 0) {
        FAIL_RETURN(Encode(lastTag, count, compiledCode));
    }

    Integer len;
    compiledCode->GetLength(&len);
    Array<Char> r(len);
    compiledCode->GetChars(0, len, r, 0);
    *compiledFormat = r;
    return NOERROR;
}

ECode SimpleDateFormat::Encode(
    /* [in] */ Integer tag,
    /* [in] */ Integer length,
    /* [in] */ IStringBuilder* buffer)
{
    if (tag == DateFormatSymbols::PATTERN_ISO_ZONE && length >= 4) {
        Logger::E("SimpleDateFormat", "invalid ISO 8601 format: length=%d", length);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (length < 255) {
        buffer->AppendChar((Char)(tag << 8 | length));
    }
    else {
        buffer->AppendChar((Char)((tag << 8) | 0xff));
        buffer->AppendChar((Char)(((unsigned Integer)length) >> 16));
        buffer->AppendChar((Char)(length & 0xffff));
    }
    return NOERROR;
}

void SimpleDateFormat::InitializeDefaultCentury()
{
    mCalendar->SetTimeInMillis(System::GetCurrentTimeMillis());
    mCalendar->Add(ICalendar::YEAR, -80);
    AutoPtr<IDate> date;
    mCalendar->GetTime(&date);
    ParseAmbiguousDatesAsAfter(date);
}

void SimpleDateFormat::ParseAmbiguousDatesAsAfter(
    /* [in] */ IDate* startDate)
{
    mDefaultCenturyStart = startDate;
    mCalendar->SetTime(startDate);
    mCalendar->Get(ICalendar::YEAR, &mDefaultCenturyStartYear);
}

ECode SimpleDateFormat::Set2DigitYearStart(
    /* in] */ IDate* startDate)
{
    Long time;
    startDate->GetTime(&time);
    AutoPtr<IDate> newDate;
    CDate::New(time, IID_IDate, (IInterface**)&newDate);
    ParseAmbiguousDatesAsAfter(newDate);
    return NOERROR;
}

ECode SimpleDateFormat::Get2DigitYearStart(
    /* [out] */ IDate** startDate)
{
    VALIDATE_NOT_NULL(startDate);

    return ICloneable::Probe(mDefaultCenturyStart)->Clone(IID_IDate, (IInterface**)startDate);
}

ECode SimpleDateFormat::Format(
    /* [in] */ IDate* date,
    /* [in, out] */ IStringBuffer* toAppendTo,
    /* [in] */ IFieldPosition* pos)
{
    pos->SetBeginIndex(0);
    pos->SetEndIndex(0);
    AutoPtr<IFormatFieldDelegate> delegate;
    pos->GetFieldDelegate(&delegate);
    return Format(date, toAppendTo, delegate);
}

ECode SimpleDateFormat::Format(
    /* [in] */ IDate* date,
    /* [in, out] */ IStringBuffer* toAppendTo,
    /* [in] */ IFormatFieldDelegate* delegate)
{
    mCalendar->SetTime(date);

    Boolean useDateFormatSymbols = UseDateFormatSymbols();

    for (Integer i = 0; i < mCompiledPattern.GetLength(); ) {
        Integer tag = ((unsigned Char)mCompiledPattern[i]) >> 8;
        Integer count = mCompiledPattern[i++] && 0xff;
        if (count == 255) {
            count = mCompiledPattern[i++] << 16;
            count |= mCompiledPattern[i++];
        }

        switch (tag) {
            case TAG_QUOTE_ASCII_CHAR:
                toAppendTo->AppendChar((Char)count);
                break;

            case TAG_QUOTE_CHARS:
                toAppendTo->Append(mCompiledPattern, i, count);
                i += count;
                break;

            default:
                FAIL_RETURN(SubFormat(tag, count, delegate, toAppendTo, useDateFormatSymbols));
                break;
        }
    }
    return NOERROR;
}

ECode SimpleDateFormat::FormatToCharacterIterator(
    /* [in] */ IInterface* obj,
    /* [out] */ IAttributedCharacterIterator** it)
{
    VALIDATE_NOT_NULL(it);

    AutoPtr<IStringBuffer> sb;
    CStringBuffer::New(IID_IStringBuffer, (IInterface**)&sb);
    AutoPtr<CharacterIteratorFieldDelegate> delegate = new CharacterIteratorFieldDelegate();

    if (IDate::Probe(obj) != nullptr) {
        FAIL_RETURN(Format(IDate::Probe(obj), sb, delegate.Get()));
    }
    else if (INumber::Probe(obj) != nullptr) {
        Long value;
        INumber::Probe(obj)->LongValue(&value);
        AutoPtr<IDate> date;
        CDate::New(value, IID_IDate, (IInterface**)&date);
        FAIL_RETURN(Format(date, sb, delegate.Get()));
    }
    else if (obj == nullptr) {
        Logger::E("SimpleDateFormat", "formatToCharacterIterator must be passed non-null object");
        return E_NULL_POINTER_EXCEPTION;
    }
    else {
        Logger::E("SimpleDateFormat", "Cannot format given Object as a Date");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    String text;
    sb->ToString(&text);
    return delegate->GetIterator(text, it);
}

ECode SimpleDateFormat::SubFormat(
    /* [in] */ Integer patternCharIndex,
    /* [in] */ Integer count,
    /* [in] */ IFormatFieldDelegate* delegate,
    /* [in] */ IStringBuffer* buffer,
    /* [in] */ Boolean useDateFormatSymbols)
{
    Integer maxIntCount = IInteger::MAX_VALUE;
    String current;
    Integer beginOffset;
    buffer->GetLength(&beginOffset);

    Integer field = PATTERN_INDEX_TO_CALENDAR_FIELD[patternCharIndex];
    Integer value;
    if (field == CalendarBuilder::WEEK_YEAR) {
        Boolean supported;
        if (mCalendar->IsWeekDateSupported(&supported), supported) {
            mCalendar->GetWeekYear(&value);
        }
        else {
            patternCharIndex = DateFormatSymbols::PATTERN_YEAR;
            field = PATTERN_INDEX_TO_CALENDAR_FIELD[patternCharIndex];
            mCalendar->Get(field, &value);
        }
    }
    else if (field == CalendarBuilder::ISO_DAY_OF_WEEK) {
        mCalendar->Get(ICalendar::DAY_OF_WEEK, &value);
        value = CalendarBuilder::ToISODayOfWeek(value);
    }
    else {
        mCalendar->Get(field, &value);
    }

    Integer style = (count >= 4) ? ICalendar::LONG : ICalendar::SHORT;
    if (!useDateFormatSymbols && field != CalendarBuilder::ISO_DAY_OF_WEEK) {
        mCalendar->GetDisplayName(field, style, mLocale, &current);
    }

    // Note: zeroPaddingNumber() assumes that maxDigits is either
    // 2 or maxIntCount. If we make any changes to this,
    // zeroPaddingNumber() must be fixed.

    switch (patternCharIndex) {
        case DateFormatSymbols::PATTERN_ERA: // 'G'
        {
            if (useDateFormatSymbols) {
                Array<String> eras;
                mFormatData->GetEras(&eras);
                if (value < eras.GetLength()) {
                    current = eras[value];
                }
            }
            if (current.IsNull()) {
                current = "";
            }
            break;
        }

        case DateFormatSymbols::PATTERN_WEEK_YEAR: // 'Y'
        case DateFormatSymbols::PATTERN_YEAR: // 'y'
        {
            if (IGregorianCalendar::Probe(mCalendar) != nullptr) {
                if (count != 2) {
                    ZeroPaddingNumber(value, count, maxIntCount, buffer);
                }
                else {
                    ZeroPaddingNumber(value, 2, 2, buffer);
                } // clip 1996 to 96
            }
            else {
                if (current.IsNull()) {
                    ZeroPaddingNumber(value, style == ICalendar::LONG ? 1 : count,
                            maxIntCount, buffer);
                }
            }
            break;
        }

        case DateFormatSymbols::PATTERN_MONTH: // 'M'
        {
            current = FormatMonth(count, value, maxIntCount, buffer, useDateFormatSymbols,
                    false /* standalone */);
            break;
        }

        case DateFormatSymbols::PATTERN_MONTH_STANDALONE: // 'L'
        {
            current = FormatMonth(count, value, maxIntCount, buffer, useDateFormatSymbols,
                    true /* standalone */);
            break;
        }

        case DateFormatSymbols::PATTERN_HOUR_OF_DAY1: // 'k' 1-based.  eg, 23:59 + 1 hour =>> 24:59
        {
            if (current.IsNull()) {
                if (value == 0) {
                    mCalendar->GetMaximum(ICalendar::HOUR_OF_DAY, &value);
                    ZeroPaddingNumber(value + 1, count, maxIntCount, buffer);
                }
                else {
                    ZeroPaddingNumber(value, count, maxIntCount, buffer);
                }
            }
            break;
        }

        case DateFormatSymbols::PATTERN_DAY_OF_WEEK: // 'E'
        {
            current = FormatWeekday(count, value, useDateFormatSymbols, false /* standalone */);
            break;
        }

        case DateFormatSymbols::PATTERN_STANDALONE_DAY_OF_WEEK: // 'c'
        {
            current = FormatWeekday(count, value, useDateFormatSymbols, true /* standalone */);
            break;
        }

        case DateFormatSymbols::PATTERN_AM_PM: // 'a'
        {
            if (useDateFormatSymbols) {
                Array<String> ampm;
                mFormatData->GetAmPmStrings(&ampm);
                current = ampm[value];
            }
            break;
        }

        case DateFormatSymbols::PATTERN_HOUR1: // 'h' 1-based.  eg, 11PM + 1 hour =>> 12 AM
        {
            if (current.IsNull()) {
                if (value == 0) {
                    mCalendar->GetLeastMaximum(ICalendar::HOUR, &value);
                    ZeroPaddingNumber(value + 1, count, maxIntCount, buffer);
                }
                else {
                    ZeroPaddingNumber(value, count, maxIntCount, buffer);
                }
            }
            break;
        }

        case DateFormatSymbols::PATTERN_ZONE_NAME: // 'z'
        {
            if (current.IsNull()) {
                AutoPtr<ITimeZone> tz;
                mCalendar->GetTimeZone(&tz);
                mCalendar->Get(ICalendar::DST_OFFSET, &value);
                Boolean daylight = (value != 0);
                String zoneString;
                if (DateFormatSymbols::From(mFormatData)->mIsZoneStringsSet) {
                    Integer tzstyle = count < 4 ? ITimeZone::SHORT : ITimeZone::LONG;
                    Array<Array<String>> zoneStrings = DateFormatSymbols::From(mFormatData)->GetZoneStringsWrapper();
                    String tzID;
                    tz->GetID(&tzID);
                    TimeZoneNames::GetDisplayName(zoneStrings, tzID, daylight, tzstyle, &zoneString);
                }
                else {
                    String tzID;
                    tz->GetID(&tzID);
                    Boolean contained;
                    if (GetUTC_ZONE_IDS()->Contains(CoreUtils::Box(tzID), &contained), contained) {
                        zoneString = UTC;
                    }
                    else {
                        ITimeZoneNamesNameType nameType;
                        if (count < 4) {
                            nameType = daylight
                                    ? ITimeZoneNamesNameType::SHORT_DAYLIGHT
                                    : ITimeZoneNamesNameType::SHORT_STANDARD;
                        }
                        else {
                            nameType =  daylight
                                    ? ITimeZoneNamesNameType::LONG_DAYLIGHT
                                    : ITimeZoneNamesNameType::LONG_STANDARD;
                        }
                        CHECK(0);
                        // String canonicalID =
                    }
                }
                if (!zoneString.IsNull()) {
                    buffer->Append(zoneString);
                }
                else {
                    Integer zoneOffset, dstOffset;
                    mCalendar->Get(ICalendar::ZONE_OFFSET, &zoneOffset);
                    mCalendar->Get(ICalendar::DST_OFFSET, &dstOffset);
                    Integer offsetMillis = zoneOffset + dstOffset;
                    buffer->Append(TimeZone::CreateGmtOffsetString(true, true, offsetMillis));
                }
            }
            break;
        }

        case DateFormatSymbols::PATTERN_ZONE_VALUE: // 'Z' ("-/+hhmm" form)
        {
            Integer zoneOffset, dstOffset;
            mCalendar->Get(ICalendar::ZONE_OFFSET, &zoneOffset);
            mCalendar->Get(ICalendar::DST_OFFSET, &dstOffset);
            value = zoneOffset + dstOffset;
            Boolean includeSeparator = (count >= 4);
            Boolean includeGmt = (count >= 4);
            buffer->Append(TimeZone::CreateGmtOffsetString(includeGmt, includeSeparator, value));

            break;
        }

        case DateFormatSymbols::PATTERN_ISO_ZONE: // 'X'
        {
            Integer zoneOffset, dstOffset;
            mCalendar->Get(ICalendar::ZONE_OFFSET, &zoneOffset);
            mCalendar->Get(ICalendar::DST_OFFSET, &dstOffset);
            value = zoneOffset + dstOffset;

            if (value == 0) {
                buffer->AppendChar('Z');
                break;
            }

            value /= 60000;
            if (value >= 0) {
                buffer->AppendChar('+');
            }
            else {
                buffer->AppendChar('-');
                value = -value;
            }

            CalendarUtils::Sprintf0d(buffer, value / 60, 2);
            if (count == 1) {
                break;
            }

            if (count == 3) {
                buffer->Append(':');
            }
            CalendarUtils::Sprintf0d(buffer, value % 60, 2);
            break;
        }

        case DateFormatSymbols::PATTERN_MILLISECOND: // 'S'
        {
            // Fractional seconds must be treated specially. We must always convert the parsed
            // value into a fractional second [0, 1) and then widen it out to the appropriate
            // formatted size. For example, an initial value of 789 will be converted
            // 0.789 and then become ".7" (S) or ".78" (SS) or "0.789" (SSS) or "0.7890" (SSSS)
            // in the resulting formatted output.
            if (current.IsNull()) {
                value = (Integer)(((Double) value / 1000) * Math::Pow(10, count));
                ZeroPaddingNumber(value, count, count, buffer);
            }
            break;
        }

        default:
        // case DateFormatSymbols::PATTERN_DAY_OF_MONTH:         // 'd'
        // case DateFormatSymbols::PATTERN_HOUR_OF_DAY0:         // 'H' 0-based.  eg, 23:59 + 1 hour =>> 00:59
        // case DateFormatSymbols::PATTERN_MINUTE:               // 'm'
        // case DateFormatSymbols::PATTERN_SECOND:               // 's'
        // case DateFormatSymbols::PATTERN_DAY_OF_YEAR:          // 'D'
        // case DateFormatSymbols::PATTERN_DAY_OF_WEEK_IN_MONTH: // 'F'
        // case DateFormatSymbols::PATTERN_WEEK_OF_YEAR:         // 'w'
        // case DateFormatSymbols::PATTERN_WEEK_OF_MONTH:        // 'W'
        // case DateFormatSymbols::PATTERN_HOUR0:                // 'K' eg, 11PM + 1 hour =>> 0 AM
        // case DateFormatSymbols::PATTERN_ISO_DAY_OF_WEEK:      // 'u' pseudo field, Monday = 1, ..., Sunday = 7
        {
            if (current.IsNull()) {
                ZeroPaddingNumber(value, count, maxIntCount, buffer);
            }
            break;
        }
    } // switch (patternCharIndex)

    if (!current.IsNull()) {
        buffer->Append(current);
    }

    Integer fieldID = PATTERN_INDEX_TO_DATE_FORMAT_FIELD[patternCharIndex];
    IDateFormatField* f = const_cast<IDateFormatField*>(
            PATTERN_INDEX_TO_DATE_FORMAT_FIELD_ID[patternCharIndex]);

    Integer length;
    buffer->GetLength(&length);
    return delegate->Formatted(fieldID, IFormatField::Probe(f), f, beginOffset, length, buffer);
}

}
}
