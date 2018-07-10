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

#include "ccm/core/System.h"
#include "ccm/util/calendar/AbstractCalendar.h"
#include <ccmlogger.h>

using ccm::core::System;

namespace ccm {
namespace util {
namespace calendar {

ECode AbstractCalendar::GetEra(
    /* [in] */ const String& eraName,
    /* [out] */ IEra** era)
{
    VALIDATE_NOT_NULL(era);

    if (!mEras.IsNull()) {
        for (Integer i = 0; i < mEras.GetLength(); i++) {
            String name;
            if (mEras[i]->GetName(&name), name.Equals(eraName)) {
                *era = mEras[i];
                REFCOUNT_ADD(*era);
                return NOERROR;
            }
        }
    }
    *era = nullptr;
    return NOERROR;
}

ECode AbstractCalendar::GetEras(
    /* [out, callee] */ Array<IEra*>* eras)
{
    VALIDATE_NOT_NULL(eras);

    if (!mEras.IsNull()) {
        *eras = Array<IEra*>(mEras.GetLength());
        eras->Copy(mEras, mEras.GetLength());
        return NOERROR;
    }
    else {
        *eras = Array<IEra*>::Null();
        return NOERROR;
    }
}

ECode AbstractCalendar::SetEra(
    /* [in] */ ICalendarDate* date,
    /* [in] */ const String& eraName)
{
    if (mEras.IsNull()) {
        return NOERROR;
    }
    for (Integer i = 0; i < mEras.GetLength(); i++) {
        IEra* e = mEras[i];
        String name;
        if (e != nullptr && (e->GetName(&name), name.Equals(eraName))) {
            date->SetEra(e);
            return NOERROR;
        }
    }
    Logger::E("AbstractCalendar", "unknown era name: %s", eraName.string());
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

void AbstractCalendar::SetEras(
    /* [in] */ const Array<IEra*>& eras)
{
    mEras = eras;
}

ECode AbstractCalendar::GetCalendarDate(
    /* [out] */ ICalendarDate** date)
{
    VALIDATE_NOT_NULL(date);

    AutoPtr<ICalendarDate> inDate;
    FAIL_RETURN(NewCalendarDate((ICalendarDate**)&inDate));
    return GetCalendarDate(System::GetCurrentTimeMillis(), inDate, date);
}

ECode AbstractCalendar::GetCalendarDate(
    /* [in] */ Long millis,
    /* [out] */ ICalendarDate** date)
{
    VALIDATE_NOT_NULL(date);

    AutoPtr<ICalendarDate> inDate;
    FAIL_RETURN(NewCalendarDate((ICalendarDate**)&inDate));
    return GetCalendarDate(millis, inDate, date);
}

ECode AbstractCalendar::GetCalendarDate(
    /* [in] */ Long millis,
    /* [in] */ ITimeZone* zone,
    /* [out] */ ICalendarDate** date)
{
    VALIDATE_NOT_NULL(date);

    AutoPtr<ICalendarDate> inDate;
    FAIL_RETURN(NewCalendarDate(zone, (ICalendarDate**)&inDate));
    return GetCalendarDate(millis, inDate, date);
}

ECode AbstractCalendar::GetCalendarDate(
    /* [in] */ Long millis,
    /* [in] */ ICalendarDate* inDate,
    /* [out] */ ICalendarDate** outDate)
{
    VALIDATE_NOT_NULL(outDate);

    Integer ms = 0;             // time of day
    Integer zoneOffset = 0;
    Integer saving = 0;
    Long days = 0;              // fixed date

    // adjust to local time if `date' has time zone.
    AutoPtr<ITimeZone> zi;
    inDate->GetZone((ITimeZone**)&zi);
    if (zi != nullptr) {
        Array<Integer> offsets(2);
        zi->GetOffset(millis, &zoneOffset);
        zi->GetRawOffset(&offsets[0]);
        offsets[1] = zoneOffset - offsets[0];

        // We need to calculate the given millis and time zone
        // offset separately for ccm::util::GregorianCalendar
        // compatibility. (i.e., millis + zoneOffset could cause
        // overflow or underflow, which must be avoided.) Usually
        // days should be 0 and ms is in the range of -13:00 to
        // +14:00. However, we need to deal with extreme cases.
        days = zoneOffset / DAY_IN_MILLIS;
        ms = zoneOffset % DAY_IN_MILLIS;
        saving = offsets[1];
    }
    inDate->SetZoneOffset(zoneOffset);
    inDate->SetDaylightSaving(saving);

    days += millis / DAY_IN_MILLIS;
    ms += (Integer) (millis % DAY_IN_MILLIS);
    if (ms >= DAY_IN_MILLIS) {
        // at most ms is (DAY_IN_MILLIS - 1) * 2.
        ms -= DAY_IN_MILLIS;
        ++days;
    }
    else {
        // at most ms is (1 - DAY_IN_MILLIS) * 2. Adding one
        // DAY_IN_MILLIS results in still negative.
        while (ms < 0) {
            ms += DAY_IN_MILLIS;
            --days;
        }
    }

    // convert to fixed date (offset from Jan. 1, 1 (Gregorian))
    days += EPOCH_OFFSET;

    // calculate date fields from the fixed date
    GetCalendarDateFromFixedDate(inDate, days);

    // calculate time fields from the time of day
    SetTimeOfDay(inDate, ms);
    inDate->SetLeapYear(IsLeapYear(inDate));
    inDate->SetNormalized(true);
    *outDate = inDate;
    REFCOUNT_ADD(*outDate);
    return NOERROR;
}

}
}
}
