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
#include "ccm/core/Character.h"
#include "ccm/core/CoreUtils.h"
#include "ccm/core/CStringBuffer.h"
#include "ccm/core/CStringBuilder.h"
#include "ccm/core/Math.h"
#include "ccm/core/System.h"
#include "ccm/text/CharacterIteratorFieldDelegate.h"
#include "ccm/text/CParsePosition.h"
#include "ccm/text/DateFormatSymbols.h"
#include "ccm/text/DontCareFieldPosition.h"
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
#include "ccm.core.ICharSequence.h"
#include "ccm.core.INumber.h"
#include "ccm.text.IDecimalFormat.h"
#include "ccm.text.IDecimalFormatSymbols.h"
#include "ccm.util.ICollection.h"
#include "ccm.util.IGregorianCalendar.h"
#include "ccm.util.IIterator.h"
#include "ccm.util.IList.h"
#include "ccm.util.IMap.h"
#include "libcore/icu/LocaleData.h"
#include "libcore/icu/TimeZoneNames.h"
#include "libcore.icu.ILocaleData.h"
#include "pisces/icu/text/TimeZoneNames.h"
#include <ccmlogger.h>

using ccm::core::Character;
using ccm::core::CoreUtils;
using ccm::core::CStringBuffer;
using ccm::core::CStringBuilder;
using ccm::core::E_NULL_POINTER_EXCEPTION;
using ccm::core::ICharSequence;
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
using ccm::util::IIterator;
using ccm::util::IList;
using ccm::util::IMap;
using ccm::util::Locale;
using ccm::util::TimeZone;
using ccm::util::calendar::CalendarUtils;
using ccm::util::concurrent::CConcurrentHashMap;
using ccm::util::concurrent::IID_IConcurrentMap;
using libcore::icu::ILocaleData;
using libcore::icu::LocaleData;
using pisces::icu::text::TimeZoneNames;
using pisces::icu::text::ITimeZoneNamesNameType;

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

const String SimpleDateFormat::GMT("GMT");
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

        if (c == U'\'') {
            // '' is treated as a single quote regardless of being
            // in a quoted section.
            if ((i + 1) < length) {
                c = pattern.GetChar(i + 1);
                if (c == U'\'') {
                    i++;
                    if (count != 0) {
                        FAIL_RETURN(Encode(lastTag, count, compiledCode));
                        lastTag = -1;
                        count = 0;
                    }
                    if (inQuote) {
                        tmpBuffer->Append(c);
                    }
                    else {
                        compiledCode->Append((Char)(TAG_QUOTE_ASCII_CHAR << 8 | c));
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
                        compiledCode->Append((Char)(TAG_QUOTE_ASCII_CHAR << 8 | ch));
                    }
                    else {
                        compiledCode->Append((Char)(TAG_QUOTE_CHARS << 8 | 1));
                        compiledCode->Append(ch);
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
            tmpBuffer->Append(c);
            continue;
        }
        if (!(c >= U'a' && c <= U'z' || c >= U'A' && c <= U'Z')) {
            if (count != 0) {
                FAIL_RETURN(Encode(lastTag, count, compiledCode));
                lastTag = -1;
                count = 0;
            }
            if (c < 128) {
                // In most cases, c would be a delimiter, such as ':'.
                compiledCode->Append((Char)(TAG_QUOTE_ASCII_CHAR << 8 | c));
            }
            else {
                // Take any contiguous non-ASCII alphabet characters and
                // put them in a single TAG_QUOTE_CHARS.
                Integer j;
                for (j = i + 1; j < length; j++) {
                    Char d = pattern.GetChar(j);
                    if (d == U'\'' || (d >= U'a' && d <= U'z' || d >= U'A' && d <= U'Z')) {
                        break;
                    }
                }
                compiledCode->Append((Char)(TAG_QUOTE_CHARS << 8 | (j - i)));
                for (; i < j; i++) {
                    compiledCode->Append(pattern.GetChar(i));
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
        buffer->Append((Char)(tag << 8 | length));
    }
    else {
        buffer->Append((Char)((tag << 8) | 0xff));
        buffer->Append((Char)(((unsigned Integer)length) >> 16));
        buffer->Append((Char)(length & 0xffff));
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
                toAppendTo->Append((Char)count);
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
                    libcore::icu::TimeZoneNames::GetDisplayName(zoneStrings, tzID, daylight, tzstyle, &zoneString);
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
                buffer->Append(U'Z');
                break;
            }

            value /= 60000;
            if (value >= 0) {
                buffer->Append(U'+');
            }
            else {
                buffer->Append(U'-');
                value = -value;
            }

            CalendarUtils::Sprintf0d(buffer, value / 60, 2);
            if (count == 1) {
                break;
            }

            if (count == 3) {
                buffer->Append(U':');
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

String SimpleDateFormat::FormatWeekday(
    /* [in] */ Integer count,
    /* [in] */ Integer value,
    /* [in] */ Boolean useDateFormatSymbols,
    /* [in] */ Boolean standalone)
{
    if (useDateFormatSymbols) {
        Array<String> weekdays;
        if (count == 4) {
            if (standalone) {
                weekdays = DateFormatSymbols::From(mFormatData)->GetStandAloneWeekdays();
            }
            else {
                mFormatData->GetWeekdays(&weekdays);
            }
        }
        else if (count == 5) {
            if (standalone) {
                weekdays = DateFormatSymbols::From(mFormatData)->GetTinyStandAloneWeekdays();
            }
            else {
                weekdays = DateFormatSymbols::From(mFormatData)->GetTinyWeekdays();
            }
        }
        else {
            if (standalone) {
                weekdays = DateFormatSymbols::From(mFormatData)->GetShortStandAloneWeekdays();
            }
            else {
                mFormatData->GetShortWeekdays(&weekdays);
            }
        }

        return weekdays[value];
    }

    return String(nullptr);
}

String SimpleDateFormat::FormatMonth(
    /* [in] */ Integer count,
    /* [in] */ Integer value,
    /* [in] */ Integer maxIntCount,
    /* [in] */ IStringBuffer* buffer,
    /* [in] */ Boolean useDateFormatSymbols,
    /* [in] */ Boolean standalone)
{
    String current;
    if (useDateFormatSymbols) {
        Array<String> months;
        if (count == 4) {
            if (standalone) {
                months = DateFormatSymbols::From(mFormatData)->GetStandAloneMonths();
            }
            else {
                mFormatData->GetMonths(&months);
            }
        }
        else if (count == 5) {
            if (standalone) {
                months = DateFormatSymbols::From(mFormatData)->GetTinyStandAloneMonths();
            }
            else {
                months = DateFormatSymbols::From(mFormatData)->GetTinyMonths();
            }
        }
        else if (count == 3) {
            if (standalone) {
                months = DateFormatSymbols::From(mFormatData)->GetShortStandAloneMonths();
            }
            else {
                mFormatData->GetShortMonths(&months);
            }
        }
        else {
            months.Clear();
        }

        if (!months.IsNull()) {
            current = months[value];
        }
    }
    else {
        if (count < 3) {
            current = nullptr;
        }
    }

    if (current.IsNull()) {
        ZeroPaddingNumber(value + 1, count, maxIntCount, buffer);
    }

    return current;
}

void SimpleDateFormat::ZeroPaddingNumber(
    /* [in] */ Integer value,
    /* [in] */ Integer minDigits,
    /* [in] */ Integer maxDigits,
    /* [in] */ IStringBuffer* buffer)
{
    if (mZeroDigit == 0) {
        AutoPtr<IDecimalFormatSymbols> symbols;
        IDecimalFormat::Probe(mNumberFormat)->GetDecimalFormatSymbols(&symbols);
        symbols->GetZeroDigit(&mZeroDigit);
    }
    if (value >= 0) {
        if (value < 100 && minDigits >= 1 && minDigits <= 2) {
            if (value < 10) {
                if (minDigits == 2) {
                    buffer->Append(mZeroDigit);
                }
                buffer->Append((Char)(mZeroDigit + value));
            }
            else {
                buffer->Append((Char)(mZeroDigit + value / 10));
                buffer->Append((Char)(mZeroDigit + value % 10));
            }
            return;
        }
        else if (value >= 1000 && value < 10000) {
            if (minDigits == 4) {
                buffer->Append((Char)(mZeroDigit + value / 1000));
                value %= 1000;
                buffer->Append((Char)(mZeroDigit + value / 100));
                value %= 100;
                buffer->Append((Char)(mZeroDigit + value / 10));
                buffer->Append((Char)(mZeroDigit + value % 10));
                return;
            }
            if (minDigits == 2 && maxDigits == 2) {
                ZeroPaddingNumber(value % 100, 2, 2, buffer);
                return;
            }
        }
    }

    mNumberFormat->SetMinimumIntegerDigits(minDigits);
    mNumberFormat->SetMaximumIntegerDigits(maxDigits);
    mNumberFormat->Format((Long)value, buffer, DontCareFieldPosition::GetInstance());
}

ECode SimpleDateFormat::Parse(
    /* [in] */ const String& text,
    /* [in] */ IParsePosition* pos,
    /* [out] */ IDate** date)
{
    VALIDATE_NOT_NULL(date);

    // Make sure the timezone associated with this dateformat instance (set via
    // {@code setTimeZone} isn't change as a side-effect of parsing a date.
    AutoPtr<ITimeZone> tz;
    GetTimeZone(&tz);
    ECode ec = ParseInternal(text, pos, date);
    SetTimeZone(tz);
    return ec;
}

ECode SimpleDateFormat::ParseInternal(
    /* [in] */ const String& text,
    /* [in] */ IParsePosition* pos,
    /* [out] */ IDate** date)
{
    VALIDATE_NOT_NULL(date);

    CheckNegativeNumberExpression();

    Integer start;
    pos->GetIndex(&start);
    Integer oldStart = start;
    Integer textLength = text.GetLength();

    Array<Boolean> ambiguousYear(1);
    ambiguousYear[0] = false;

    AutoPtr<CalendarBuilder> calb = new CalendarBuilder();

    for (Integer i = 0; i < mCompiledPattern.GetLength(); ) {
        Integer tag = ((unsigned Char)mCompiledPattern[i]) >> 8;
        Integer count = mCompiledPattern[i++] & 0xff;
        if (count == 255) {
            count = mCompiledPattern[i++] << 16;
            count |= mCompiledPattern[i++];
        }

        switch (tag) {
            case TAG_QUOTE_ASCII_CHAR:
            {
                if (start >= textLength || text.GetChar(start) != (Char)count) {
                    pos->SetIndex(oldStart);
                    pos->SetErrorIndex(start);
                    *date = nullptr;
                    return NOERROR;
                }
                start++;
                break;
            }

            case TAG_QUOTE_CHARS:
            {
                while (count-- > 0) {
                    if (start >= textLength || text.GetChar(start) != mCompiledPattern[i++]) {
                        pos->SetIndex(oldStart);
                        pos->SetErrorIndex(start);
                        *date = nullptr;
                        return NOERROR;
                    }
                    start++;
                }
                break;
            }

            default:
            {
                // Peek the next pattern to determine if we need to
                // obey the number of pattern letters for
                // parsing. It's required when parsing contiguous
                // digit text (e.g., "20010704") with a pattern which
                // has no delimiters between fields, like "yyyyMMdd".
                Boolean obeyCount = false;

                // In Arabic, a minus sign for a negative number is put after
                // the number. Even in another locale, a minus sign can be
                // put after a number using DateFormat.setNumberFormat().
                // If both the minus sign and the field-delimiter are '-',
                // subParse() needs to determine whether a '-' after a number
                // in the given text is a delimiter or is a minus sign for the
                // preceding number. We give subParse() a clue based on the
                // information in compiledPattern.
                Boolean useFollowingMinusSignAsDelimiter = false;

                if (i < mCompiledPattern.GetLength()) {
                    Integer nextTag = ((unsigned Char)mCompiledPattern[i]) >> 8;
                    if (!(nextTag == TAG_QUOTE_ASCII_CHAR ||
                          nextTag == TAG_QUOTE_CHARS)) {
                        obeyCount = true;
                    }

                    if (mHasFollowingMinusSign &&
                        (nextTag == TAG_QUOTE_ASCII_CHAR ||
                         nextTag == TAG_QUOTE_CHARS)) {
                        Integer c;
                        if (nextTag == TAG_QUOTE_ASCII_CHAR) {
                            c = mCompiledPattern[i] & 0xff;
                        }
                        else {
                            c = mCompiledPattern[i+1];
                        }

                        if (c == mMinusSign) {
                            useFollowingMinusSignAsDelimiter = true;
                        }
                    }
                }
                start = SubParse(text, start, tag, count, obeyCount, ambiguousYear,
                        pos, useFollowingMinusSignAsDelimiter, calb);
                if (start < 0) {
                    pos->SetIndex(oldStart);
                    *date = nullptr;
                    return NOERROR;
                }
            }
        }
    }

    // At this point the fields of Calendar have been set.  Calendar
    // will fill in default values for missing fields when the time
    // is computed.

    pos->SetIndex(start);

    AutoPtr<IDate> parsedDate;
    calb->Establish(mCalendar);
    ECode ec = mCalendar->GetTime(&parsedDate);
    if (FAILED(ec)) {
        pos->SetErrorIndex(start);
        pos->SetIndex(oldStart);
        *date = nullptr;
        return NOERROR;
    }
    // If the year value is ambiguous,
    // then the two-digit year == the default start year
    if (ambiguousYear[0]) {
        Boolean before;
        if (parsedDate->Before(mDefaultCenturyStart, &before), before) {
            calb->AddYear(100);
            calb->Establish(mCalendar);
            ec = mCalendar->GetTime(&parsedDate);
            if (FAILED(ec)) {
                pos->SetErrorIndex(start);
                pos->SetIndex(oldStart);
                *date = nullptr;
                return NOERROR;
            }
        }
    }

    parsedDate.MoveTo(date);
    return NOERROR;
}

Integer SimpleDateFormat::MatchString(
    /* [in] */ const String& text,
    /* [in] */ Integer start,
    /* [in] */ Integer field,
    /* [in] */ Array<String>& data,
    /* [in] */ CalendarBuilder* calb)
{
    Integer i = 0;
    Integer count = data.GetLength();

    if (field == ICalendar::DAY_OF_WEEK) {
        i = 1;
    }

    // There may be multiple strings in the data[] array which begin with
    // the same prefix (e.g., Cerven and Cervenec (June and July) in Czech).
    // We keep track of the longest match, and return that.  Note that this
    // unfortunately requires us to test all array elements.
    Integer bestMatchLength = 0, bestMatch = -1;
    for (; i < count; ++i) {
        Integer length = data[i].GetLength();
        // Always compare if we have no match yet; otherwise only compare
        // against potentially better matches (longer strings).
        if (length > bestMatchLength &&
                text.RegionMatchesIgnoreCase(start, data[i], 0, length)) {
            bestMatch = i;
            bestMatchLength = length;
        }

        // When the input option ends with a period (usually an abbreviated form), attempt
        // to match all chars up to that period.
        if ((data[i].GetChar(length - 1) == U'.') &&
                ((length - 1) > bestMatchLength) &&
                text.RegionMatchesIgnoreCase(start, data[i], 0, length - 1)) {
            bestMatch = i;
            bestMatchLength = (length - 1);
        }
    }
    if (bestMatch >= 0) {
        calb->Set(field, bestMatch);
        return start + bestMatchLength;
    }
    return -start;
}

Integer SimpleDateFormat::MatchString(
    /* [in] */ const String& text,
    /* [in] */ Integer start,
    /* [in] */ Integer field,
    /* [in] */ IMap* data,
    /* [in] */ CalendarBuilder* calb)
{
    if (data != nullptr) {
        String bestMatch;

        AutoPtr<ISet> keys;
        data->GetKeySet(&keys);
        FOR_EACH_2(String, name, ICharSequence::Probe, CoreUtils::Unbox, keys) {
            Integer length = name.GetLength();
            if (bestMatch.IsNull() || length > bestMatch.GetLength()) {
                if (text.RegionMatchesIgnoreCase(start, name, 0, length)) {
                    bestMatch = name;
                }
            }
        } END_FOR_EACH();

        if (!bestMatch.IsNull()) {
            AutoPtr<IInterface> value;
            data->Get(CoreUtils::Box(bestMatch), &value);
            calb->Set(field, CoreUtils::Unbox(IInteger::Probe(value)));
            return start + bestMatch.GetLength();
        }
    }
    return -start;
}

Integer SimpleDateFormat::MatchZoneString(
    /* [in] */ const String& text,
    /* [in] */ Integer start,
    /* [in] */ Array<String>& zoneNames)
{
    for (Integer i = 1; i <= 4; ++i) {
        // Checking long and short zones [1 & 2],
        // and long and short daylight [3 & 4].
        String zoneName = zoneNames[i];
        if (text.RegionMatchesIgnoreCase(start, zoneName, 0, zoneName.GetLength())) {
            return i;
        }
    }
    return -1;
}

Integer SimpleDateFormat::SubParseZoneString(
    /* [in] */ const String& text,
    /* [in] */ Integer start,
    /* [in] */ CalendarBuilder* calb)
{
    if (DateFormatSymbols::From(mFormatData)->mIsZoneStringsSet) {
        return SubParseZoneStringFromSymbols(text, start, calb);
    }
    else {
        return SubParseZoneStringFromICU(text, start, calb);
    }
}

AutoPtr<ITimeZoneNames> SimpleDateFormat::GetTimeZoneNames()
{
    if (mTimeZoneNames == nullptr) {
        mTimeZoneNames = TimeZoneNames::GetInstance(mLocale);
    }
    return mTimeZoneNames;
}

Integer SimpleDateFormat::SubParseZoneStringFromICU(
    /* [in] */ const String& text,
    /* [in] */ Integer start,
    /* [in] */ CalendarBuilder* calb)
{
    // String currentTimeZoneID =
    CHECK(0);

    AutoPtr<ITimeZoneNames> tzNames = GetTimeZoneNames();
    // AutoPtr<ITimeZoneNamesMatchInfo> bestMatch;
    // The MetaZones associated with the current time zone are needed in two places, both of
    // which are avoided in some cases, so they are computed lazily.
    AutoPtr<ISet> currentTzMetaZoneIds;
}

Integer SimpleDateFormat::SubParseZoneStringFromSymbols(
    /* [in] */ const String& text,
    /* [in] */ Integer start,
    /* [in] */ CalendarBuilder* calb)
{
    Boolean useSameName = false; // true if standard and daylight time use the same abbreviation.
    AutoPtr<ITimeZone> currentTimeZone;
    GetTimeZone(&currentTimeZone);

    // At this point, check for named time zones by looking through
    // the locale data from the TimeZoneNames strings.
    // Want to be able to parse both short and long forms.
    String tzID;
    currentTimeZone->GetID(&tzID);
    Integer zoneIndex = DateFormatSymbols::From(mFormatData)->GetZoneIndex(tzID);
    AutoPtr<ITimeZone> tz;
    Array<Array<String>> zoneStrings = DateFormatSymbols::From(mFormatData)->GetZoneStringsWrapper();
    Array<String> zoneNames;
    Integer nameIndex = 0;
    if (zoneIndex != -1) {
        zoneNames = zoneStrings[zoneIndex];
        if ((nameIndex =  MatchZoneString(text, start, zoneNames)) > 0) {
            if (nameIndex <= 2) {
                // Check if the standard name (abbr) and the daylight name are the same.
                useSameName = zoneNames[nameIndex].EqualsIgnoreCase(zoneNames[nameIndex + 2]);
            }
            TimeZone::GetTimeZone(zoneNames[0], &tz);
        }
    }
    if (tz == nullptr) {
        TimeZone::GetDefault()->GetID(&tzID);
        zoneIndex = DateFormatSymbols::From(mFormatData)->GetZoneIndex(tzID);
        if (zoneIndex != -1) {
            zoneNames = zoneStrings[zoneIndex];
            if ((nameIndex = MatchZoneString(text, start, zoneNames)) > 0) {
                if (nameIndex <= 2) {
                    useSameName = zoneNames[nameIndex].EqualsIgnoreCase(zoneNames[nameIndex + 2]);
                }
                TimeZone::GetTimeZone(zoneNames[0], &tz);
            }
        }
    }

    if (tz == nullptr) {
        Integer len = zoneStrings.GetLength();
        for (Integer i = 0; i < len; i++) {
            zoneNames = zoneStrings[i];
            if ((nameIndex = MatchZoneString(text, start, zoneNames)) > 0) {
                if (nameIndex <= 2) {
                    useSameName = zoneNames[nameIndex].EqualsIgnoreCase(zoneNames[nameIndex + 2]);
                }
                TimeZone::GetTimeZone(zoneNames[0], &tz);
                break;
            }
        }
    }
    if (tz != nullptr) { // Matched any ?
        if (!Object::Equals(tz, currentTimeZone)) {
            SetTimeZone(tz);
        }
        // If the time zone matched uses the same name
        // (abbreviation) for both standard and daylight time,
        // let the time zone in the Calendar decide which one.
        //
        // Also if tz.getDSTSaving() returns 0 for DST, use tz to
        // determine the local time.
        Integer dstAmount = 0;
        if (nameIndex >= 3) {
            tz->GetDSTSavings(&dstAmount);
        }
        if (!(useSameName || (nameIndex >= 3 && dstAmount == 0))) {
            calb->Clear(ICalendar::ZONE_OFFSET);
            calb->Set(ICalendar::DST_OFFSET, dstAmount);
        }
        return (start + zoneNames[nameIndex].GetLength());
    }
    return -start;
}

Integer SimpleDateFormat::SubParseNumericZone(
    /* [in] */ const String& text,
    /* [in] */ Integer start,
    /* [in] */ Integer sign,
    /* [in] */ Integer count,
    /* [in] */ Boolean colonRequired,
    /* [in] */ CalendarBuilder* calb)
{
    Integer index = start;

    while (true) {
        Char c = text.GetChar(index++);
        // parse hh
        Integer hours;
        if (!IsDigit(c)) {
            break;
        }
        hours = c - U'0';
        c = text.GetChar(index++);
        if (IsDigit(c)) {
            hours = hours * 10 + (c - U'0');
        }
        else {
            --index;
        }
        if (hours > 23) {
            break;
        }
        Integer minutes = 0;
        if (count != 1) {
            // Proceed with parsing mm
            c = text.GetChar(index++);
            // Intentional change in behavior from OpenJDK. OpenJDK will return an error code
            // if a : is found and colonRequired is false, this will return an error code if
            // a : is not found and colonRequired is true.
            //
            // colonRequired | c == ':' | OpenJDK | this
            //   false       |  false   |   ok    |  ok
            //   false       |  true    |  error  |  ok
            //   true        |  false   |   ok    | error
            //   true        |  true    |   ok    |  ok
            if (c == U':') {
                c = text.GetChar(index++);
            }
            else if (colonRequired) {
                break;
            }
            if (!IsDigit(c)) {
                break;
            }
            minutes = c - U'0';
            c = text.GetChar(index++);
            if (!IsDigit(c)) {
                break;
            }
            minutes = minutes * 10 + (c - U'0');
            if (minutes > 59) {
                break;
            }
        }
        minutes += hours * 60;
        calb->Set(ICalendar::ZONE_OFFSET, minutes * MILLIS_PER_MINUTE * sign);
        calb->Set(ICalendar::DST_OFFSET, 0);
        return index;
    }
    return 1 - index; // -(index - 1);
}

Boolean SimpleDateFormat::IsDigit(
    /* [in] */ Char c)
{
    return c >= U'0' && c <= U'9';
}

Integer SimpleDateFormat::SubParse(
    /* [in] */ const String& text,
    /* [in] */ Integer start,
    /* [in] */ Integer patternCharIndex,
    /* [in] */ Integer count,
    /* [in] */ Boolean obeyCount,
    /* [in] */ Array<Boolean>& ambiguousYear,
    /* [in] */ IParsePosition* origPos,
    /* [in] */ Boolean useFollowingMinusSignAsDelimiter,
    /* [in] */ CalendarBuilder* calb)
{
    AutoPtr<INumber> number;
    Integer value = 0;
    AutoPtr<IParsePosition> pos;
    CParsePosition::New(0, IID_IParsePosition, (IInterface**)&pos);
    pos->SetIndex(start);
    Boolean supported;
    if (patternCharIndex == DateFormatSymbols::PATTERN_WEEK_YEAR && (mCalendar->IsWeekDateSupported(&supported), !supported)) {
        // use calendar year 'y' instead
        patternCharIndex = DateFormatSymbols::PATTERN_YEAR;
    }
    Integer field = PATTERN_INDEX_TO_CALENDAR_FIELD[patternCharIndex];

    // If there are any spaces here, skip over them.  If we hit the end
    // of the string, then fail.
    Integer index;
    for (;;) {
        pos->GetIndex(&index);
        if (index >= text.GetLength()) {
            origPos->SetErrorIndex(start);
            return -1;
        }
        Char c = text.GetChar(index);
        if (c != U' ' && c != U'\t') {
            break;
        }
        pos->SetIndex(++index);
    }

    {
        // We handle a few special cases here where we need to parse
        // a number value.  We handle further, more generic cases below.  We need
        // to handle some of them here because some fields require extra processing on
        // the parsed value.
        if (patternCharIndex == DateFormatSymbols::PATTERN_HOUR_OF_DAY1 ||
                patternCharIndex == DateFormatSymbols::PATTERN_HOUR1 ||
                (patternCharIndex == DateFormatSymbols::PATTERN_MONTH && count <= 2) ||
                patternCharIndex == DateFormatSymbols::PATTERN_YEAR ||
                patternCharIndex == DateFormatSymbols::PATTERN_WEEK_YEAR) {
            // It would be good to unify this with the obeyCount logic below,
            // but that's going to be difficult.
            if (obeyCount) {
                if ((start + count) > text.GetLength()) {
                    goto PARSING_FAILED;
                }
                mNumberFormat->Parse(text.Substring(0, start + count), pos, &number);
            }
            else {
                mNumberFormat->Parse(text, pos, &number);
            }
            if (number == nullptr) {
                if (patternCharIndex != DateFormatSymbols::PATTERN_YEAR || IGregorianCalendar::Probe(mCalendar) != nullptr) {
                    goto PARSING_FAILED;
                }
            }
            else {
                number->IntegerValue(&value);

                if (useFollowingMinusSignAsDelimiter && (value < 0) &&
                        (((pos->GetIndex(&index), index < text.GetLength()) &&
                        (text.GetChar(index) != mMinusSign)) ||
                        ((index == text.GetLength()) &&
                        (text.GetChar(index - 1) == mMinusSign)))) {
                    value = -value;
                    pos->SetIndex(--index);
                }
            }
        }

        Boolean useDateFormatSymbols = UseDateFormatSymbols();

        Integer index;
        switch (patternCharIndex) {
            case DateFormatSymbols::PATTERN_ERA: // 'G'
            {
                if (useDateFormatSymbols) {
                    Array<String> eras;
                    mFormatData->GetEras(&eras);
                    if ((index = MatchString(text, start, ICalendar::ERA, eras, calb)) > 0) {
                        return index;
                    }
                }
                else {
                    AutoPtr<IMap> map;
                    mCalendar->GetDisplayNames(field, ICalendar::ALL_STYLES, mLocale, &map);
                    if ((index = MatchString(text, start, field, map, calb)) > 0) {
                        return index;
                    }
                }
                goto PARSING_FAILED;
            }

            case DateFormatSymbols::PATTERN_WEEK_YEAR: // 'Y'
            case DateFormatSymbols::PATTERN_YEAR: // 'y'
            {
                if (IGregorianCalendar::Probe(mCalendar) == nullptr) {
                    // calendar might have text representations for year values,
                    // such as "\u5143" in JapaneseImperialCalendar.
                    Integer style = (count >= 4) ? ICalendar::LONG : ICalendar::SHORT;
                    AutoPtr<IMap> map;
                    mCalendar->GetDisplayNames(field, style, mLocale, &map);
                    if (map != nullptr) {
                        if ((index = MatchString(text, start, field, map, calb)) > 0) {
                            return index;
                        }
                    }
                    calb->Set(field, value);
                    pos->GetIndex(&index);
                    return index;
                }

                // If there are 3 or more YEAR pattern characters, this indicates
                // that the year value is to be treated literally, without any
                // two-digit year adjustments (e.g., from "01" to 2001).  Otherwise
                // we made adjustments to place the 2-digit year in the proper
                // century, for parsed strings from "00" to "99".  Any other string
                // is treated literally:  "2250", "-1", "1", "002".
                if (count <= 2 && (pos->GetIndex(&index), index - start == 2) &&
                        Character::IsDigit(text.GetChar(start)) &&
                        Character::IsDigit(text.GetChar(start + 1))) {
                    // Assume for example that the defaultCenturyStart is 6/18/1903.
                    // This means that two-digit years will be forced into the range
                    // 6/18/1903 to 6/17/2003.  As a result, years 00, 01, and 02
                    // correspond to 2000, 2001, and 2002.  Years 04, 05, etc. correspond
                    // to 1904, 1905, etc.  If the year is 03, then it is 2003 if the
                    // other fields specify a date before 6/18, or 1903 if they specify a
                    // date afterwards.  As a result, 03 is an ambiguous year.  All other
                    // two-digit years are unambiguous.
                    Integer ambiguousTwoDigitYear = mDefaultCenturyStartYear % 100;
                    ambiguousYear[0] = value == ambiguousTwoDigitYear;
                    value += (mDefaultCenturyStartYear / 100) * 100 +
                            (value < ambiguousTwoDigitYear ? 100 : 0);
                }
                calb->Set(field, value);
                return index;
            }

            case DateFormatSymbols::PATTERN_MONTH: // 'M'
            {
                Integer idx = ParseMonth(text, count, value, start, field, pos,
                        useDateFormatSymbols, false /* isStandalone */, calb);
                if (idx > 0) {
                    return idx;
                }

                goto PARSING_FAILED;
            }

            case DateFormatSymbols::PATTERN_MONTH_STANDALONE: // 'L'
            {
                Integer idx = ParseMonth(text, count, value, start, field, pos,
                        useDateFormatSymbols, true /* isStandalone */, calb);
                if (idx > 0) {
                    return idx;
                }

                goto PARSING_FAILED;
            }

            case DateFormatSymbols::PATTERN_HOUR_OF_DAY1: // 'k' 1-based.  eg, 23:59 + 1 hour =>> 24:59
            {
                Boolean lenient;
                if (IsLenient(&lenient), !lenient) {
                    // Validate the hour value in non-lenient
                    if (value < 1 || value > 24) {
                        goto PARSING_FAILED;
                    }
                }
                Integer maxValue;
                mCalendar->GetMaximum(ICalendar::HOUR_OF_DAY, &maxValue);
                if (value == maxValue + 1) {
                    value = 0;
                }
                calb->Set(ICalendar::HOUR_OF_DAY, value);
                pos->GetIndex(&index);
                return index;
            }

            case DateFormatSymbols::PATTERN_DAY_OF_WEEK: // 'E'
            {
                Integer idx = ParseWeekday(text, start, field, useDateFormatSymbols,
                        false /* standalone */, calb);
                if (idx > 0) {
                    return idx;
                }

                goto PARSING_FAILED;
            }

            case DateFormatSymbols::PATTERN_STANDALONE_DAY_OF_WEEK: // 'c'
            {
                Integer idx = ParseWeekday(text, start, field, useDateFormatSymbols,
                        true /* standalone */, calb);
                if (idx > 0) {
                    return idx;
                }

                goto PARSING_FAILED;
            }

            case DateFormatSymbols::PATTERN_AM_PM: // 'a'
            {
                if (useDateFormatSymbols) {
                    Array<String> ampm;
                    mFormatData->GetAmPmStrings(&ampm);
                    if ((index = MatchString(text, start, ICalendar::AM_PM,
                            ampm, calb)) > 0) {
                        return index;
                    }
                }
                else {
                    AutoPtr<IMap> map;
                    mCalendar->GetDisplayNames(field, ICalendar::ALL_STYLES, mLocale, &map);
                    if ((index = MatchString(text, start, field, map, calb)) > 0) {
                        return index;
                    }
                }

                goto PARSING_FAILED;
            }

            case DateFormatSymbols::PATTERN_HOUR1: // 'h' 1-based.  eg, 11PM + 1 hour =>> 12 AM
            {
                Boolean lenient;
                if (IsLenient(&lenient), !lenient) {
                    // Validate the hour value in non-lenient
                    if (value < 1 || value > 12) {
                        goto PARSING_FAILED;
                    }
                }
                Integer maxValue;
                mCalendar->GetLeastMaximum(ICalendar::HOUR, &maxValue);
                if (value == maxValue + 1) {
                    value = 0;
                }
                calb->Set(ICalendar::HOUR, value);
                pos->GetIndex(&index);
                return index;
            }

            case DateFormatSymbols::PATTERN_ZONE_NAME: // 'z'
            case DateFormatSymbols::PATTERN_ZONE_VALUE: // 'Z'
            {
                Integer sign = 0;
                pos->GetIndex(&index);
                Char c = text.GetChar(index);
                if (c == U'+') {
                    sign = 1;
                }
                else if (c == U'-') {
                    sign = -1;
                }
                if (sign == 0) {
                    // Try parsing a custom time zone "GMT+hh:mm" or "GMT".
                    if ((c == U'G' || c == U'g') &&
                            (text.GetLength() - start) >= GMT.GetLength() &&
                            text.RegionMatchesIgnoreCase(start, GMT, 0, GMT.GetLength())) {
                        index = start + GMT.GetLength();
                        pos->SetIndex(index);

                        if ((text.GetLength() - index) > 0) {
                            c = text.GetChar(index);
                            if (c == U'+') {
                                sign = 1;
                            }
                            else if (c == U'-') {
                                sign = -1;
                            }
                        }

                        if (sign == 0) {
                            calb->Set(ICalendar::ZONE_OFFSET, 0);
                            calb->Set(ICalendar::DST_OFFSET, 0);
                            return index;
                        }

                        // Parse the rest as "hh[:]?mm"
                        Integer i = SubParseNumericZone(text, ++index, sign, 0,
                                false, calb);
                        if (i > 0) {
                            return i;
                        }
                        pos->SetIndex(-i);
                    }
                    else {
                        // Try parsing the text as a time zone
                        // name or abbreviation.
                        Integer i = SubParseZoneString(text, index, calb);
                        if (i > 0) {
                            return i;
                        }
                        pos->SetIndex(-i);
                    }
                }
                else {
                    // Parse the rest as "hh[:]?mm" (RFC 822)
                    Integer i = SubParseNumericZone(text, ++index, sign, 0,
                            false, calb);
                    pos->SetIndex(index);
                    if (i > 0) {
                        return i;
                    }
                    pos->SetIndex(-i);
                }

                goto PARSING_FAILED;
            }

            case DateFormatSymbols::PATTERN_ISO_ZONE: // 'X'
            {
                pos->GetIndex(&index);
                if ((text.GetLength() - index) <= 0) {
                    goto PARSING_FAILED;
                }

                Integer sign;
                Char c = text.GetChar(index);
                if (c == U'Z') {
                    calb->Set(ICalendar::ZONE_OFFSET, 0);
                    calb->Set(ICalendar::DST_OFFSET, 0);
                    pos->SetIndex(++index);
                    return index;
                }

                // parse text as "+/-hh[[:]mm]" based on count
                if (c == U'+') {
                    sign = 1;
                }
                else if (c == U'-') {
                    sign = -1;
                }
                else {
                    pos->SetIndex(++index);
                    goto PARSING_FAILED;
                }
                Integer i = SubParseNumericZone(text, ++index, sign, count,
                        count == 3, calb);
                pos->SetIndex(index);
                if (i > 0) {
                    return i;
                }
                pos->SetIndex(-i);

                goto PARSING_FAILED;
            }

            default:
            // case PATTERN_DAY_OF_MONTH:         // 'd'
            // case PATTERN_HOUR_OF_DAY0:         // 'H' 0-based.  eg, 23:59 + 1 hour =>> 00:59
            // case PATTERN_MINUTE:               // 'm'
            // case PATTERN_SECOND:               // 's'
            // case PATTERN_MILLISECOND:          // 'S'
            // case PATTERN_DAY_OF_YEAR:          // 'D'
            // case PATTERN_DAY_OF_WEEK_IN_MONTH: // 'F'
            // case PATTERN_WEEK_OF_YEAR:         // 'w'
            // case PATTERN_WEEK_OF_MONTH:        // 'W'
            // case PATTERN_HOUR0:                // 'K' 0-based.  eg, 11PM + 1 hour =>> 0 AM
            // case PATTERN_ISO_DAY_OF_WEEK:      // 'u' (pseudo field);
            {

                // Handle "generic" fields
                Integer parseStart;
                pos->GetIndex(&parseStart);
                if (obeyCount) {
                    if ((start + count) > text.GetLength()) {
                        goto PARSING_FAILED;
                    }
                    mNumberFormat->Parse(text.Substring(0, start + count), pos, &number);
                }
                else {
                    mNumberFormat->Parse(text, pos, &number);
                }
                if (number != nullptr) {
                    if (patternCharIndex == DateFormatSymbols::PATTERN_MILLISECOND) {
                        // Fractional seconds must be treated specially. We must always
                        // normalize them to their fractional second value [0, 1) before we attempt
                        // to parse them.
                        //
                        // Case 1: 11.78 seconds is 11 seconds and 780 (not 78) milliseconds.
                        // Case 2: 11.7890567 seconds is 11 seconds and 789 (not 7890567) milliseconds.
                        Double doubleValue;
                        number->DoubleValue(&doubleValue);
                        pos->GetIndex(&index);
                        Integer width = index - parseStart;
                        Double divisor = Math::Pow(10, width);
                        value = (Integer)((doubleValue / divisor) * 1000);
                    }
                    else {
                        number->IntegerValue(&value);
                    }

                    pos->GetIndex(&index);
                    if (useFollowingMinusSignAsDelimiter && (value < 0) &&
                            (((index < text.GetLength()) &&
                            (text.GetChar(index) != mMinusSign)) ||
                            ((index == text.GetLength()) &&
                            (text.GetChar(index - 1) == mMinusSign)))) {
                        value = -value;
                        pos->SetIndex(--index);
                    }

                    calb->Set(field, value);
                    return index;
                }

                goto PARSING_FAILED;
            }
        }
    }

PARSING_FAILED:
    // Parsing failed.
    pos->GetIndex(&index);
    origPos->SetErrorIndex(index);
    return -1;
}

Integer SimpleDateFormat::ParseMonth(
    /* [in] */ const String& text,
    /* [in] */ Integer count,
    /* [in] */ Integer value,
    /* [in] */ Integer start,
    /* [in] */ Integer field,
    /* [in] */ IParsePosition* pos,
    /* [in] */ Boolean useDateFormatSymbols,
    /* [in] */ Boolean standalone,
    /* [in] */ CalendarBuilder* out)
{
    Integer index;
    if (count <= 2) { // i.e., M or MM.
        // Don't want to parse the month if it is a string
        // while pattern uses numeric style: M or MM.
        // [We computed 'value' above.]
        out->Set(ICalendar::MONTH, value - 1);
        pos->GetIndex(&index);
        return index;
    }

    index = -1;
    if (useDateFormatSymbols) {
        // count >= 3 // i.e., MMM or MMMM
        // Want to be able to parse both short and long forms.
        // Try count == 4 first:
        Array<String> months;
        if (standalone) {
            months = DateFormatSymbols::From(mFormatData)->GetStandAloneMonths();
        }
        else {
            mFormatData->GetMonths(&months);
        }
        if ((index = MatchString(text, start, ICalendar::MONTH, months, out)) > 0) {
            return index;
        }
        // count == 4 failed, now try count == 3
        if (standalone){
            months = DateFormatSymbols::From(mFormatData)->GetShortStandAloneMonths();
        }
        else {
            mFormatData->GetShortMonths(&months);
        }
        if ((index = MatchString(text, start, ICalendar::MONTH, months, out)) > 0) {
            return index;
        }
    }
    else {
        AutoPtr<IMap> map;
        mCalendar->GetDisplayNames(field, ICalendar::ALL_STYLES, mLocale, &map);
        if ((index = MatchString(text, start, field, map, out)) > 0) {
            return index;
        }
    }

    return index;
}

Integer SimpleDateFormat::ParseWeekday(
    /* [in] */ const String& text,
    /* [in] */ Integer start,
    /* [in] */ Integer field,
    /* [in] */ Boolean useDateFormatSymbols,
    /* [in] */ Boolean standalone,
    /* [in] */ CalendarBuilder* out)
{
    Integer index = -1;
    if (useDateFormatSymbols) {
        // Want to be able to parse both short and long forms.
        // Try count == 4 (DDDD) first:
        Array<String> weekdays;
        if (standalone) {
            weekdays = DateFormatSymbols::From(mFormatData)->GetStandAloneWeekdays();
        }
        else {
            mFormatData->GetWeekdays(&weekdays);
        }
        if ((index = MatchString(text, start, ICalendar::DAY_OF_WEEK,
                weekdays, out)) > 0) {
            return index;
        }

        // DDDD failed, now try DDD
        if (standalone) {
            weekdays = DateFormatSymbols::From(mFormatData)->GetShortStandAloneWeekdays();
        }
        else {
            mFormatData->GetShortWeekdays(&weekdays);
        }
        if ((index = MatchString(text, start, ICalendar::DAY_OF_WEEK,
                weekdays, out)) > 0) {
            return index;
        }
    }
    else {
        Array<Integer> styles(2);
        styles[0] = ICalendar::LONG;
        styles[1] = ICalendar::SHORT;
        for (Integer i = 0; i < styles.GetLength(); i++) {
            Integer style = styles[i];
            AutoPtr<IMap> map;
            mCalendar->GetDisplayNames(field, style, mLocale, &map);
            if ((index = MatchString(text, start, field, map, out)) > 0) {
                return index;
            }
        }
    }

    return index;
}

String SimpleDateFormat::GetCalendarName()
{
    return Object::GetCoclassName(mCalendar);
}

Boolean SimpleDateFormat::UseDateFormatSymbols()
{
    if (mUseDateFormatSymbols) {
        return true;
    }
    return IsGregorianCalendar() || mLocale == nullptr;
}

Boolean SimpleDateFormat::IsGregorianCalendar()
{
    return GetCalendarName().Equals("ccm.util.CGregorianCalendar");
}

ECode SimpleDateFormat::TranslatePattern(
    /* [in] */ const String& pattern,
    /* [in] */ const String& from,
    /* [in] */ const String& to,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IStringBuilder> sb;
    CStringBuilder::New(IID_IStringBuilder, (IInterface**)&sb);
    Boolean inQuote = false;
    for (Integer i = 0; i < pattern.GetLength(); ++i) {
        Char c = pattern.GetChar(i);
        if (inQuote) {
            if (c == U'\'') {
                inQuote = false;
            }
        }
        else {
            if (c == U'\'') {
                inQuote = true;
            }
            else if ((c >= U'a' && c <= U'z') || (c >= U'A' && c <= U'Z')) {
                Integer ci = from.IndexOf(c);
                if (ci >= 0) {
                    // patternChars is longer than localPatternChars due
                    // to serialization compatibility. The pattern letters
                    // unsupported by localPatternChars pass through.
                    if (ci < to.GetLength()) {
                        c = to.GetChar(ci);
                    }
                }
                else {
                    Logger::E("SimpleDateFormat", "Illegal pattern character '%c'", c);
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
                }
            }
        }
        sb->Append(c);
    }
    if (inQuote) {
        Logger::E("SimpleDateFormat", "Unfinished quote in pattern");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return sb->ToString(result);
}

ECode SimpleDateFormat::ToPattern(
    /* [out] */ String* pattern)
{
    VALIDATE_NOT_NULL(pattern);

    *pattern = mPattern;
    return NOERROR;
}

ECode SimpleDateFormat::ToLocalizedPattern(
    /* [out] */ String* pattern)
{
    String localPatternChars;
    mFormatData->GetLocalPatternChars(&localPatternChars);
    return TranslatePattern(mPattern, DateFormatSymbols::sPatternChars,
            localPatternChars, pattern);
}

ECode SimpleDateFormat::ApplyPattern(
    /* [in] */ const String& pattern)
{
    FAIL_RETURN(Compile(pattern, &mCompiledPattern));
    mPattern = pattern;
    return NOERROR;
}

ECode SimpleDateFormat::ApplyLocalizedPattern(
    /* [in] */ const String& pattern)
{
    String localPatternChars;
    mFormatData->GetLocalPatternChars(&localPatternChars);
    String p;
    FAIL_RETURN(TranslatePattern(pattern, localPatternChars,
            DateFormatSymbols::sPatternChars, &p));
    FAIL_RETURN(Compile(p, &mCompiledPattern));
    mPattern = p;
    return NOERROR;
}

ECode SimpleDateFormat::SetDateFormatSymbols(
    /* [in] */ IDateFormatSymbols* newFormatSymbols)
{
    mFormatData = (IDateFormatSymbols*)CoreUtils::Clone(
            newFormatSymbols, IID_IDateFormatSymbols).Get();
    mUseDateFormatSymbols = true;
    return NOERROR;
}

ECode SimpleDateFormat::CloneImpl(
    /* [in] */ SimpleDateFormat* newObj)
{
    DateFormat::CloneImpl(newObj);

    newObj->mUseDateFormatSymbols = mUseDateFormatSymbols;
    newObj->mPattern = mPattern;
    newObj->mOriginalNumberFormat = mOriginalNumberFormat;
    newObj->mOriginalNumberPattern = mOriginalNumberPattern;
    newObj->mMinusSign = mMinusSign;
    newObj->mHasFollowingMinusSign = mHasFollowingMinusSign;
    newObj->mCompiledPattern = mCompiledPattern;
    newObj->mZeroDigit = mZeroDigit;
    newObj->mDefaultCenturyStart = mDefaultCenturyStart;
    newObj->mDefaultCenturyStartYear = mDefaultCenturyStartYear;
    newObj->mLocale = mLocale;
    newObj->mTimeZoneNames = mTimeZoneNames;

    newObj->mFormatData = (IDateFormatSymbols*)CoreUtils::Clone(
            mFormatData, IID_IDateFormatSymbols).Get();
    return NOERROR;
}

ECode SimpleDateFormat::GetHashCode(
    /* [out] */ Integer* hash)
{
    VALIDATE_NOT_NULL(hash);

    *hash = mPattern.GetHashCode();
    return NOERROR;
}

ECode SimpleDateFormat::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* same)
{
    VALIDATE_NOT_NULL(same);

    if (DateFormat::Equals(obj, same), !*same) {
        return NOERROR;
    }
    SimpleDateFormat* that = (SimpleDateFormat*)ISimpleDateFormat::Probe(obj);
    *same = (mPattern.Equals(that->mPattern) && (Object::Equals(
            mFormatData, that->mFormatData)));
    return NOERROR;
}

void SimpleDateFormat::CheckNegativeNumberExpression()
{
    if ((IDecimalFormat::Probe(mNumberFormat) != nullptr) &&
            !Object::Equals(mNumberFormat, mOriginalNumberFormat)) {
        String numberPattern;
        IDecimalFormat::Probe(mNumberFormat)->ToPattern(&numberPattern);
        if (!numberPattern.Equals(mOriginalNumberPattern)) {
            mHasFollowingMinusSign = false;

            Integer separatorIndex = numberPattern.IndexOf(U';');
            // If the negative subpattern is not absent, we have to analayze
            // it in order to check if it has a following minus sign.
            if (separatorIndex > -1) {
                Integer minusIndex = numberPattern.IndexOf(U'-', separatorIndex);
                if ((minusIndex > numberPattern.LastIndexOf(U'0')) &&
                    (minusIndex > numberPattern.LastIndexOf(U'#'))) {
                    mHasFollowingMinusSign = true;
                    AutoPtr<IDecimalFormatSymbols> symbols;
                    IDecimalFormat::Probe(mNumberFormat)->GetDecimalFormatSymbols(&symbols);
                    symbols->GetMinusSign(&mMinusSign);
                }
            }
            mOriginalNumberPattern = numberPattern;
        }
        mOriginalNumberFormat = mNumberFormat;
    }
}

}
}
