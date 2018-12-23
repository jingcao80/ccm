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

#include "ccm/core/CoreUtils.h"
#include "ccm/core/CStringBuffer.h"
#include "ccm/text/CParsePosition.h"
#include "ccm/text/CSimpleDateFormat.h"
#include "ccm/text/DateFormat.h"
#include "ccm/text/DontCareFieldPosition.h"
#include "ccm/util/CDate.h"
#include "ccm/util/CLocale.h"
#include "ccm.core.INumber.h"
#include "libcore/icu/ICU.h"
#include <ccmlogger.h>

using ccm::core::CoreUtils;
using ccm::core::CStringBuffer;
using ccm::core::IID_IStringBuffer;
using ccm::core::INumber;
using ccm::util::CDate;
using ccm::util::CLocale;
using ccm::util::IID_ICalendar;
using ccm::util::IID_IDate;
using libcore::icu::ICU;

namespace ccm {
namespace text {

Boolean DateFormat::sIs24Hour = false;

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
    /* [in] */ Boolean is24Hour)
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

}
}
