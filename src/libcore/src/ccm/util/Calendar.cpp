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

#include "ccm/util/Calendar.h"
#include "ccm/util/CLocale.h"
#include "ccm/util/CDate.h"
#include "ccm/util/TimeZone.h"
#include "ccm.core.IInteger.h"
#include <ccmautoptr.h>

using ccm::core::IID_ICloneable;
using ccm::core::IID_IComparable;
using ccm::core::IInteger;
using ccm::io::IID_ISerializable;

namespace ccm {
namespace util {

CCM_INTERFACE_IMPL_4(Calendar, SyncObject, ICalendar, ISerializable, ICloneable, IComparable);

ECode Calendar::Constructor()
{
    Constructor(TimeZone::GetDefaultRef(),
            CLocale::GetDefault(CLocale::Category::GetFORMAT()));
    mSharedZone = true;
    return NOERROR;
}

ECode Calendar::Constructor(
    /* [in] */ ITimeZone* zone,
    /* [in] */ ILocale* locale)
{
    AutoPtr<ILocale> aLocale = locale;
    if (aLocale == nullptr) {
        aLocale = CLocale::GetDefault();
    }
    mFields = Array<Integer>(FIELD_COUNT);
    mIsSet = Array<Boolean>(FIELD_COUNT);
    mStamp = Array<Integer>(FIELD_COUNT);

    mZone = zone;
    SetWeekCountData(aLocale);
    return NOERROR;
}

AutoPtr<ICalendar> Calendar::GetInstance()
{
    return CreateCalendar(TimeZone::GetDefault(),
            CLocale::GetDefault(CLocale::Category::GetFORMAT()));
}

AutoPtr<ICalendar> Calendar::GetInstance(
    /* [in] */ ITimeZone* zone)
{
    return CreateCalendar(zone,
            CLocale::GetDefault(CLocale::Category::GetFORMAT()));
}

AutoPtr<ICalendar> Calendar::GetInstance(
    /* [in] */ ILocale* aLocale)
{
    return CreateCalendar(TimeZone::GetDefault(), aLocale);
}

AutoPtr<ICalendar> Calendar::GetInstance(
    /* [in] */ ITimeZone* zone,
    /* [in] */ ILocale* aLocale)
{
    return CreateCalendar(zone, aLocale);
}

AutoPtr<ICalendar> Calendar::GetJapaneseImperialInstance(
    /* [in] */ ITimeZone* zone,
    /* [in] */ ILocale* aLocale)
{
    return nullptr;
}

AutoPtr<ICalendar> Calendar::CreateCalendar(
    /* [in] */ ITimeZone* zone,
    /* [in] */ ILocale* aLocale)
{
    return nullptr;
}

Array<ILocale*> Calendar::GetAvailableLocales()
{
    return Array<ILocale*>::Null();
}

ECode Calendar::GetTime(
    /* [out] */ IDate** date)
{
    VALIDATE_NOT_NULL(date);

    Long time;
    GetTimeInMillis(&time);
    return CDate::New(time, IID_IDate, (IInterface**)date);
}

ECode Calendar::SetTime(
    /* [in] */ IDate* date)
{
    Long time;
    date->GetTime(&time);
    return SetTimeInMillis(time);
}

ECode Calendar::GetTimeInMillis(
    /* [out] */ Long* time)
{
    VALIDATE_NOT_NULL(time);

    if (!mIsTimeSet) {
        UpdateTime();
    }
    *time = mTime;
    return NOERROR;
}

ECode Calendar::SetTimeInMillis(
    /* [in] */ Long millis)
{
    if (mTime = millis && mIsTimeSet && mAreFieldsSet && mAreAllFieldsSet) {
        return NOERROR;
    }
    mTime = millis;
    mIsTimeSet = true;
    mAreFieldsSet = false;
    ComputeFields();
    mAreAllFieldsSet = mAreFieldsSet = true;
    return NOERROR;
}

ECode Calendar::Get(
    /* [in] */ Integer field,
    /* [out] */ Integer* value)
{
    VALIDATE_NOT_NULL(value);

    if (field < 0 || field >= FIELD_COUNT) {
        return ccm::core::E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    Complete();
    *value = InternalGet(field);
    return NOERROR;
}

ECode Calendar::Set(
    /* [in] */ Integer field,
    /* [in] */ Integer value)
{
    if (field < 0 || field >= FIELD_COUNT) {
        return ccm::core::E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    if (mAreFieldsSet && ! mAreAllFieldsSet) {
        ComputeFields();
    }
    InternalSet(field, value);
    mIsTimeSet = false;
    mAreFieldsSet = false;
    mIsSet[field] = true;
    mStamp[field] = mNextStamp++;
    if (mNextStamp == IInteger::MAX_VALUE) {
        AdjustStamp();
    }
    return NOERROR;
}

ECode Calendar::Set(
    /* [in] */ Integer year,
    /* [in] */ Integer month,
    /* [in] */ Integer date)
{
    Set(YEAR, year);
    Set(MONTH, month);
    Set(DATE, date);
    return NOERROR;
}

ECode Calendar::Set(
    /* [in] */ Integer year,
    /* [in] */ Integer month,
    /* [in] */ Integer date,
    /* [in] */ Integer hourOfDay,
    /* [in] */ Integer minute)
{
    Set(YEAR, year);
    Set(MONTH, month);
    Set(DATE, date);
    Set(HOUR_OF_DAY, hourOfDay);
    Set(MINUTE, minute);
    return NOERROR;
}

ECode Calendar::Set(
    /* [in] */ Integer year,
    /* [in] */ Integer month,
    /* [in] */ Integer date,
    /* [in] */ Integer hourOfDay,
    /* [in] */ Integer minute,
    /* [in] */ Integer second)
{
    Set(YEAR, year);
    Set(MONTH, month);
    Set(DATE, date);
    Set(HOUR_OF_DAY, hourOfDay);
    Set(MINUTE, minute);
    Set(SECOND, second);
    return NOERROR;
}

ECode Calendar::Clear()
{
    for (Integer i = 0; i < mFields.GetLength(); ) {
        mStamp[i] = mFields[i] = 0;
        mIsSet[i++] = false;
    }
    mAreAllFieldsSet = mAreFieldsSet = false;
    mIsTimeSet = false;
    return NOERROR;
}

ECode Calendar::Clear(
    /* [in] */ Integer field)
{
    if (field < 0 || field >= FIELD_COUNT) {
        return ccm::core::E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    mFields[field] = 0;
    mStamp[field] = UNSET;
    mIsSet[field] = false;

    mAreAllFieldsSet = mAreFieldsSet = false;
    mIsTimeSet = false;
    return NOERROR;
}

ECode Calendar::IsSet(
    /* [in] */ Integer field,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (field < 0 || field >= FIELD_COUNT) {
        return ccm::core::E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    *result = mStamp[field] != UNSET;
    return NOERROR;
}

}
}
