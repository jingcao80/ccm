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
#include "como/core/CStringBuffer.h"
#include "como/text/CDateFormatField.h"
#include "como/text/CParsePosition.h"
#include "como/text/CSimpleDateFormat.h"
#include "como/text/DateFormat.h"
#include "como/text/DontCareFieldPosition.h"
#include "como/util/CDate.h"
#include "como/util/CHashMap.h"
#include "como/util/CLocale.h"
#include "como.core.INumber.h"
#include "libcore/icu/ICU.h"
#include <ccmlogger.h>

using como::core::CoreUtils;
using como::core::CStringBuffer;
using como::core::IID_ICloneable;
using como::core::IID_IStringBuffer;
using como::core::INumber;
using como::io::E_INVALID_OBJECT_EXCEPTION;
using como::io::IID_ISerializable;
using como::util::CDate;
using como::util::CHashMap;
using como::util::CLocale;
using como::util::IID_ICalendar;
using como::util::IID_IDate;
using como::util::IID_IMap;
using libcore::icu::ICU;

namespace como {
namespace text {

AutoPtr<IBoolean> DateFormat::sIs24Hour;

COMO_INTERFACE_IMPL_1(DateFormat, BaseFormat, IDateFormat);

ECode DateFormat::Format(
    /* [in] */ IInterface* obj,
    /* [in, out] */ IStringBuffer* toAppendTo,
    /* [in] */ IFieldPosition* pos)
{
    if (IDate::Probe(obj) != nullptr) {
        return Format(IDate::Probe(obj), toAppendTo, pos);
    }
    else if (INumber::Probe(obj) != nullptr) {
        Long value;
        INumber::Probe(obj)->LongValue(&value);
        AutoPtr<IDate> date;
        CDate::New(value, IID_IDate, (IInterface**)&date);
        return Format(date, toAppendTo, pos);
    }
    else {
        Logger::E("DateFormat", "Cannot format given Object as a Date");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
}

ECode DateFormat::Format(
    /* [in] */ IDate* date,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IStringBuffer> sb;
    CStringBuffer::New(IID_IStringBuffer, (IInterface**)&sb);
    FAIL_RETURN(Format(date, sb, DontCareFieldPosition::GetInstance()));
    return sb->ToString(result);
}

ECode DateFormat::Parse(
    /* [in] */ const String& source,
    /* [out] */ IDate** date)
{
    VALIDATE_NOT_NULL(date);

    AutoPtr<IParsePosition> pos;
    CParsePosition::New(0, IID_IParsePosition, (IInterface**)&pos);
    FAIL_RETURN(Parse(source, pos, date));
    Integer idx;
    pos->GetIndex(&idx);
    if (idx == 0) {
        Integer errIdx;
        pos->GetErrorIndex(&errIdx);
        Logger::E("DateFormat", "Unparseable date: \"%s\"%d", source.string(), errIdx);
        return E_PARSE_EXCEPTION;
    }
    return NOERROR;
}

ECode DateFormat::ParseObject(
    /* [in] */ const String& source,
    /* [in] */ IParsePosition* pos,
    /* [out] */ IInterface** object)
{
    return Parse(source, pos, (IDate**)object);
}

ECode DateFormat::GetTimeInstance(
    /* [out] */ IDateFormat** instance)
{
    VALIDATE_NOT_NULL(instance);

    return Get(DEFAULT, 0 ,1, CLocale::GetDefault(CLocale::Category::GetFORMAT()), instance);
}

ECode DateFormat::GetTimeInstance(
    /* [in] */ Integer style,
    /* [out] */ IDateFormat** instance)
{
    VALIDATE_NOT_NULL(instance);

    return Get(style, 0 ,1, CLocale::GetDefault(CLocale::Category::GetFORMAT()), instance);
}

ECode DateFormat::GetTimeInstance(
    /* [in] */ Integer style,
    /* [in] */ ILocale* locale,
    /* [out] */ IDateFormat** instance)
{
    VALIDATE_NOT_NULL(instance);

    return Get(style, 0, 1, locale, instance);
}

ECode DateFormat::GetDateInstance(
    /* [out] */ IDateFormat** instance)
{
    VALIDATE_NOT_NULL(instance);

    return Get(0, DEFAULT, 2, CLocale::GetDefault(CLocale::Category::GetFORMAT()), instance);
}

ECode DateFormat::GetDateInstance(
    /* [in] */ Integer style,
    /* [out] */ IDateFormat** instance)
{
    VALIDATE_NOT_NULL(instance);

    return Get(0, style, 2, CLocale::GetDefault(CLocale::Category::GetFORMAT()), instance);
}

ECode DateFormat::GetDateInstance(
    /* [in] */ Integer style,
    /* [in] */ ILocale* locale,
    /* [out] */ IDateFormat** instance)
{
    VALIDATE_NOT_NULL(instance);

    return Get(0, style, 2, locale, instance);
}

ECode DateFormat::GetDateTimeInstance(
    /* [out] */ IDateFormat** instance)
{
    VALIDATE_NOT_NULL(instance);

    return Get(DEFAULT, DEFAULT, 3, CLocale::GetDefault(CLocale::Category::GetFORMAT()), instance);
}

ECode DateFormat::GetDateTimeInstance(
    /* [in] */ Integer dateStyle,
    /* [in] */ Integer timeStyle,
    /* [out] */ IDateFormat** instance)
{
    VALIDATE_NOT_NULL(instance);

    return Get(timeStyle, dateStyle, 3, CLocale::GetDefault(CLocale::Category::GetFORMAT()), instance);
}

ECode DateFormat::GetDateTimeInstance(
    /* [in] */ Integer dateStyle,
    /* [in] */ Integer timeStyle,
    /* [in] */ ILocale* locale,
    /* [out] */ IDateFormat** instance)
{
    VALIDATE_NOT_NULL(instance);

    return Get(timeStyle, dateStyle, 3, locale, instance);
}

ECode DateFormat::GetInstance(
    /* [out] */ IDateFormat** instance)
{
    return GetDateTimeInstance(SHORT, SHORT, instance);
}

ECode DateFormat::Set24HourTimePref(
    /* [in] */ IBoolean* is24Hour)
{
    sIs24Hour = is24Hour;
    return NOERROR;
}

ECode DateFormat::GetAvailableLocales(
    /* [out, callee] */ Array<ILocale*>* locales)
{
    VALIDATE_NOT_NULL(locales);

    *locales = ICU::GetAvailableLocales();
    return NOERROR;
}

ECode DateFormat::SetCalendar(
    /* [in] */ ICalendar* newCalendar)
{
    mCalendar = newCalendar;
    return NOERROR;
}

ECode DateFormat::GetCalendar(
    /* [out] */ ICalendar** calendar)
{
    VALIDATE_NOT_NULL(calendar);

    *calendar = mCalendar;
    REFCOUNT_ADD(*calendar);
    return NOERROR;
}

ECode DateFormat::SetNumberFormat(
    /* [in] */ INumberFormat* newNumberFormat)
{
    mNumberFormat = newNumberFormat;
    return NOERROR;
}

ECode DateFormat::GetNumberFormat(
    /* [out] */ INumberFormat** format)
{
    VALIDATE_NOT_NULL(format);

    *format = mNumberFormat;
    REFCOUNT_ADD(*format);
    return NOERROR;
}

ECode DateFormat::SetTimeZone(
    /* [in] */ ITimeZone* timezone)
{
    mCalendar->SetTimeZone(timezone);
    return NOERROR;
}

ECode DateFormat::GetTimeZone(
    /* [out] */ ITimeZone** timezone)
{
    return mCalendar->GetTimeZone(timezone);
}

ECode DateFormat::SetLenient(
    /* [in] */ Boolean lenient)
{
    return mCalendar->SetLenient(lenient);
}

ECode DateFormat::IsLenient(
    /* [out] */ Boolean* lenient)
{
    return mCalendar->IsLenient(lenient);
}

ECode DateFormat::GetHashCode(
    /* [out] */ Integer* hash)
{
    VALIDATE_NOT_NULL(hash);

    *hash = Object::GetHashCode(mNumberFormat);
    return NOERROR;
}

ECode DateFormat::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* same)
{
    VALIDATE_NOT_NULL(same);

    DateFormat* other = (DateFormat*)IDateFormat::Probe(obj);
    if (other == this) {
        *same = true;
        return NOERROR;
    }
    if (obj == nullptr ||
            Object::GetCoclass((IObject*)this) != Object::GetCoclass(obj)) {
        *same = false;
        return NOERROR;
    }

    Integer thisDow, otherDow;
    Integer thisDfw, otherDfw;
    Boolean thisLenient, otherLenient;
    AutoPtr<ITimeZone> thisTz, otherTz;

    *same = (mCalendar->GetFirstDayOfWeek(&thisDow), other->mCalendar->GetFirstDayOfWeek(&otherDow), thisDow == otherDow) &&
            (mCalendar->GetMinimalDaysInFirstWeek(&thisDfw), other->mCalendar->GetMinimalDaysInFirstWeek(&otherDfw), thisDfw == otherDfw) &&
            (mCalendar->IsLenient(&thisLenient), other->mCalendar->IsLenient(&otherLenient), thisLenient == otherLenient) &&
            (mCalendar->GetTimeZone(&thisTz), other->mCalendar->GetTimeZone(&otherTz), Object::Equals(thisTz, otherTz)) &&
            (Object::Equals(mNumberFormat, other->mNumberFormat));
    return NOERROR;
}

ECode DateFormat::CloneImpl(
    /* [in] */ DateFormat* newObj)
{
    newObj->mCalendar = (ICalendar*)CoreUtils::Clone(mCalendar, IID_ICalendar).Get();
    newObj->mNumberFormat = (INumberFormat*)CoreUtils::Clone(mNumberFormat, IID_INumberFormat).Get();
    return NOERROR;
}

ECode DateFormat::Get(
    /* [in] */ Integer timeStyle,
    /* [in] */ Integer dateStyle,
    /* [in] */ Integer flags,
    /* [in] */ ILocale* loc,
    /* [out] */ IDateFormat** instance)
{
    VALIDATE_NOT_NULL(instance);

    if ((flags & 1) != 0) {
        if (timeStyle < 0 || timeStyle > 3) {
            Logger::E("DateFormat", "Illegal time style %d", timeStyle);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    else {
        timeStyle = -1;
    }
    if ((flags & 2) != 0) {
        if (dateStyle < 0 || dateStyle > 3) {
            Logger::E("DateFormat", "Illegal date style %d", dateStyle);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    else {
        dateStyle = -1;
    }
    ECode ec = CSimpleDateFormat::New(timeStyle, dateStyle, loc, IID_IDateFormat, (IInterface**)instance);
    if (SUCCEEDED(ec)) {
        return NOERROR;
    }
    return CSimpleDateFormat::New(String("M/d/yy h:mm a"), IID_IDateFormat, (IInterface**)instance);
}


//--------------------------------------------------------------------

Array<IDateFormatField*> DateFormat::Field::sCalendarToFieldMapping(ICalendar::FIELD_COUNT);

static AutoPtr<IMap> CreateMap()
{
    AutoPtr<IMap> map;
    CHashMap::New(18, IID_IMap, (IInterface**)&map);
    return map;
}

AutoPtr<IMap> DateFormat::Field:: GetInstanceMap()
{
    static const AutoPtr<IMap> sInstanceMap = CreateMap();
    return sInstanceMap;
}

COMO_INTERFACE_IMPL_2(DateFormat::Field, AttributedCharacterIteratorAttribute, IDateFormatField, IFormatField);

ECode DateFormat::Field::Constructor(
    /* [in] */ const String& name,
    /* [in] */ Integer calendarField)
{
    AttributedCharacterIteratorAttribute::Constructor(name);
    mCalendarField = calendarField;
    CoclassID cid;
    GetCoclassID(&cid);
    if (cid == CID_CDateFormatField) {
        GetInstanceMap()->Put(CoreUtils::Box(name), (IDateFormatField*)this);
        if (calendarField >= 0) {
            sCalendarToFieldMapping.Set(calendarField, (IDateFormatField*)this);
        }
    }
    return NOERROR;
}

ECode DateFormat::Field::OfCalendarField(
    /* [in] */ Integer calendarField,
    /* [out] */ IDateFormatField** field)
{
    VALIDATE_NOT_NULL(field);

    if (calendarField < 0 || calendarField >= sCalendarToFieldMapping.GetLength()) {
        Logger::E("DateFormat::Field", "Unknown Calendar constant %d", calendarField);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *field = sCalendarToFieldMapping[calendarField];
    REFCOUNT_ADD(*field);
    return NOERROR;
}

ECode DateFormat::Field::GetCalendarField(
    /* [out] */ Integer* calendarField)
{
    VALIDATE_NOT_NULL(calendarField);

    *calendarField = mCalendarField;
    return NOERROR;
}

ECode DateFormat::Field::ReadResolve(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    CoclassID cid;
    GetCoclassID(&cid);
    if (cid != CID_CDateFormatField) {
        Logger::E("DateFormat::Field", "subclass didn't correctly implement readResolve");
        return E_INVALID_OBJECT_EXCEPTION;
    }

    GetInstanceMap()->Get(CoreUtils::Box(GetName()), obj);
    if (*obj == nullptr) {
        Logger::E("DateFormat::Field", "unknown attribute name");
        return E_INVALID_OBJECT_EXCEPTION;
    }
    return NOERROR;
}

static AutoPtr<IDateFormatField> CreateField(
    /* [in] */ const String& name,
    /* [in] */ Integer calendarField)
{
    AutoPtr<IDateFormatField> field;
    CDateFormatField::New(name, calendarField, IID_IDateFormatField, (IInterface**)&field);
    return field;
}

const AutoPtr<IDateFormatField> DateFormat::Field::GetERA()
{
    static const AutoPtr<IDateFormatField> ERA =
            CreateField(String("era"), ICalendar::ERA);
    return ERA;
}

const AutoPtr<IDateFormatField> DateFormat::Field::GetYEAR()
{
    static const AutoPtr<IDateFormatField> YEAR =
            CreateField(String("year"), ICalendar::YEAR);
    return YEAR;
}

const AutoPtr<IDateFormatField> DateFormat::Field::GetMONTH()
{
    static const AutoPtr<IDateFormatField> MONTH =
            CreateField(String("month"), ICalendar::MONTH);
    return MONTH;
}

const AutoPtr<IDateFormatField> DateFormat::Field::GetDAY_OF_MONTH()
{
    static const AutoPtr<IDateFormatField> DAY_OF_MONTH =
            CreateField(String("day of month"), ICalendar::DAY_OF_MONTH);
    return DAY_OF_MONTH;
}

const AutoPtr<IDateFormatField> DateFormat::Field::GetHOUR_OF_DAY1()
{
    static const AutoPtr<IDateFormatField> HOUR_OF_DAY1 =
            CreateField(String("hour of day 1"), -1);
    return HOUR_OF_DAY1;
}

const AutoPtr<IDateFormatField> DateFormat::Field::GetHOUR_OF_DAY0()
{
    static const AutoPtr<IDateFormatField> HOUR_OF_DAY0 =
            CreateField(String("hour of day"), ICalendar::HOUR_OF_DAY);
    return HOUR_OF_DAY0;
}

const AutoPtr<IDateFormatField> DateFormat::Field::GetMINUTE()
{
    static const AutoPtr<IDateFormatField> MINUTE =
            CreateField(String("minute"), ICalendar::MINUTE);
    return MINUTE;
}

const AutoPtr<IDateFormatField> DateFormat::Field::GetSECOND()
{
    static const AutoPtr<IDateFormatField> SECOND =
            CreateField(String("second"), ICalendar::SECOND);
    return SECOND;
}

const AutoPtr<IDateFormatField> DateFormat::Field::GetMILLISECOND()
{
    static const AutoPtr<IDateFormatField> MILLISECOND =
            CreateField(String("millisecond"), ICalendar::MILLISECOND);
    return MILLISECOND;
}

const AutoPtr<IDateFormatField> DateFormat::Field::GetDAY_OF_WEEK()
{
    static const AutoPtr<IDateFormatField> DAY_OF_WEEK =
            CreateField(String("day of week"), ICalendar::DAY_OF_WEEK);
    return DAY_OF_WEEK;
}

const AutoPtr<IDateFormatField> DateFormat::Field::GetDAY_OF_YEAR()
{
    static const AutoPtr<IDateFormatField> DAY_OF_YEAR =
            CreateField(String("day of year"), ICalendar::DAY_OF_YEAR);
    return DAY_OF_YEAR;
}

const AutoPtr<IDateFormatField> DateFormat::Field::GetDAY_OF_WEEK_IN_MONTH()
{
    static const AutoPtr<IDateFormatField> DAY_OF_WEEK_IN_MONTH =
            CreateField(String("day of week in month"), ICalendar::DAY_OF_WEEK_IN_MONTH);
    return DAY_OF_WEEK_IN_MONTH;
}

const AutoPtr<IDateFormatField> DateFormat::Field::GetWEEK_OF_YEAR()
{
    static const AutoPtr<IDateFormatField> WEEK_OF_YEAR =
            CreateField(String("week of year"), ICalendar::WEEK_OF_YEAR);
    return WEEK_OF_YEAR;
}

const AutoPtr<IDateFormatField> DateFormat::Field::GetWEEK_OF_MONTH()
{
    static const AutoPtr<IDateFormatField> WEEK_OF_MONTH =
            CreateField(String("week of month"), ICalendar::WEEK_OF_MONTH);
    return WEEK_OF_MONTH;
}

const AutoPtr<IDateFormatField> DateFormat::Field::GetAM_PM()
{
    static const AutoPtr<IDateFormatField> AM_PM =
            CreateField(String("am pm"), ICalendar::AM_PM);
    return AM_PM;
}

const AutoPtr<IDateFormatField> DateFormat::Field::GetHOUR1()
{
    static const AutoPtr<IDateFormatField> HOUR1 =
            CreateField(String("hour 1"), -1);
    return HOUR1;
}

const AutoPtr<IDateFormatField> DateFormat::Field::GetHOUR0()
{
    static const AutoPtr<IDateFormatField> HOUR0 =
            CreateField(String("hour"), ICalendar::HOUR);
    return HOUR0;
}

const AutoPtr<IDateFormatField> DateFormat::Field::GetTIME_ZONE()
{
    static const AutoPtr<IDateFormatField> TIME_ZONE =
            CreateField(String("time zone"), -1);
    return TIME_ZONE;
}

}
}
