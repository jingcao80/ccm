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

#ifndef __CCM_UTIL_CALENDAR_H__
#define __CCM_UTIL_CALENDAR_H__

#include "ccm/core/SyncObject.h"
#include "ccm.core.ICloneable.h"
#include "ccm.core.IComparable.h"
#include "ccm.io.ISerializable.h"
#include "ccm.util.ICalendar.h"
#include "ccm.util.IDate.h"
#include "ccm.util.ILocale.h"
#include "ccm.util.ITimeZone.h"

using ccm::core::ICloneable;
using ccm::core::IComparable;
using ccm::core::SyncObject;
using ccm::io::ISerializable;

namespace ccm {
namespace util {

class Calendar
    : public SyncObject
    , public ICalendar
    , public ISerializable
    , public ICloneable
    , public IComparable
{
public:
    CCM_INTERFACE_DECL();

protected:
    ECode Constructor();

    ECode Constructor(
        /* [in] */ ITimeZone* zone,
        /* [in] */ ILocale* locale);

    static AutoPtr<ICalendar> GetInstance();

    static AutoPtr<ICalendar> GetInstance(
        /* [in] */ ITimeZone* zone);

    static AutoPtr<ICalendar> GetInstance(
        /* [in] */ ILocale* aLocale);

    static AutoPtr<ICalendar> GetInstance(
        /* [in] */ ITimeZone* zone,
        /* [in] */ ILocale* aLocale);

    static AutoPtr<ICalendar> GetJapaneseImperialInstance(
        /* [in] */ ITimeZone* zone,
        /* [in] */ ILocale* aLocale);

    static Array<ILocale*> GetAvailableLocales();

    ECode GetTime(
        /* [out] */ IDate** date) override final;

    ECode SetTime(
        /* [in] */ IDate* date) override final;

    ECode GetTimeInMillis(
        /* [out] */ Long* time) override;

    ECode SetTimeInMillis(
        /* [in] */ Long millis) override;

    ECode Get(
        /* [in] */ Integer field,
        /* [out] */ Integer* value) override;

    ECode Set(
        /* [in] */ Integer field,
        /* [in] */ Integer value) override;

    ECode Set(
        /* [in] */ Integer year,
        /* [in] */ Integer month,
        /* [in] */ Integer date) override final;

    ECode Set(
        /* [in] */ Integer year,
        /* [in] */ Integer month,
        /* [in] */ Integer date,
        /* [in] */ Integer hourOfDay,
        /* [in] */ Integer minute) override final;

    ECode Set(
        /* [in] */ Integer year,
        /* [in] */ Integer month,
        /* [in] */ Integer date,
        /* [in] */ Integer hourOfDay,
        /* [in] */ Integer minute,
        /* [in] */ Integer second) override final;

    ECode Clear() override final;

    ECode Clear(
        /* [in] */ Integer field) override final;

    ECode IsSet(
        /* [in] */ Integer field,
        /* [out] */ Boolean* result) override final;

protected:
    virtual ECode ComputeTime() = 0;

    virtual ECode ComputeFields() = 0;

    Integer InternalGet(
        /* [in] */ Integer field);

    void InternalSet(
        /* [in] */ Integer field,
        /* [in] */ Integer value);

    virtual ECode Complete()
    {
        return NOERROR;
    }

private:
    static AutoPtr<ICalendar> CreateCalendar(
        /* [in] */ ITimeZone* zone,
        /* [in] */ ILocale* aLocale);

    void SetWeekCountData(
        /* [in] */ ILocale* desiredLocale)
    {}

    void UpdateTime()
    {}

    void AdjustStamp()
    {}

protected:
    Array<Integer> mFields;

    Array<Boolean> mIsSet;

    Long mTime = 0;

    Boolean mIsTimeSet = false;

    Boolean mAreFieldsSet = false;

    Boolean mAreAllFieldsSet = false;

private:
    Array<Integer> mStamp;

    AutoPtr<ITimeZone> mZone;

    Boolean mSharedZone = false;

    static constexpr Integer UNSET = 0;

    static constexpr Integer MINIMUM_USER_STAMP = 2;

    Integer mNextStamp = MINIMUM_USER_STAMP;
};

inline Integer Calendar::InternalGet(
    /* [in] */ Integer field)
{
    return mFields[field];
}

inline void Calendar::InternalSet(
    /* [in] */ Integer field,
    /* [in] */ Integer value)
{
    mFields[field] = value;
}

}
}

#endif // __CCM_UTIL_CALENDAR_H__
