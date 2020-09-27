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

#ifndef __COMO_UTIL_DATE_H__
#define __COMO_UTIL_DATE_H__

#include "como/core/SyncObject.h"
#include "como.core.ICloneable.h"
#include "como.core.IComparable.h"
#include "como.core.IStringBuilder.h"
#include "como.io.ISerializable.h"
#include "como.util.IDate.h"
#include "como.util.calendar.IBaseCalendar.h"
#include "como.util.calendar.IBaseCalendarDate.h"
#include <comosp.h>

using como::core::ICloneable;
using como::core::IComparable;
using como::core::IStringBuilder;
using como::core::SyncObject;
using como::io::ISerializable;
using como::util::calendar::IBaseCalendar;
using como::util::calendar::IBaseCalendarDate;

namespace como {
namespace util {

class Date
    : public SyncObject
    , public IDate
    , public ICloneable
    , public IComparable
    , public ISerializable
{
public:
    COMO_INTERFACE_DECL();

    ECode Constructor();

    ECode Constructor(
        /* [in] */ Long date);

    ECode Constructor(
        /* [in] */ Integer year,
        /* [in] */ Integer month,
        /* [in] */ Integer date);

    ECode Constructor(
        /* [in] */ Integer year,
        /* [in] */ Integer month,
        /* [in] */ Integer date,
        /* [in] */ Integer hrs,
        /* [in] */ Integer min);

    ECode Constructor(
        /* [in] */ Integer year,
        /* [in] */ Integer month,
        /* [in] */ Integer date,
        /* [in] */ Integer hrs,
        /* [in] */ Integer min,
        /* [in] */ Integer sec);

    ECode Constructor(
        /* [in] */ const String& s);

    static Long UTC(
        /* [in] */ Integer year,
        /* [in] */ Integer month,
        /* [in] */ Integer date,
        /* [in] */ Integer hrs,
        /* [in] */ Integer min,
        /* [in] */ Integer sec);

    static ECode Parse(
        /* [in] */ const String& s,
        /* [out] */ Long* date);

    ECode GetYear(
        /* [out] */ Integer& year) override;

    ECode SetYear(
        /* [in] */ Integer year) override;

    ECode GetMonth(
        /* [out] */ Integer& month) override;

    ECode SetMonth(
        /* [in] */ Integer month) override;

    ECode GetDate(
        /* [out] */ Integer& date) override;

    ECode SetDate(
        /* [in] */ Integer date) override;

    ECode GetDay(
        /* [out] */ Integer& day) override;

    ECode GetHours(
        /* [out] */ Integer& hours) override;

    ECode SetHours(
        /* [in] */ Integer hours) override;

    ECode GetMinutes(
        /* [out] */ Integer& minutes) override;

    ECode SetMinutes(
        /* [in] */ Integer minutes) override;

    ECode GetSeconds(
        /* [out] */ Integer& seconds) override;

    ECode SetSeconds(
        /* [in] */ Integer seconds) override;

    ECode GetTime(
        /* [out] */ Long& time) override;

    ECode SetTime(
        /* [in] */ Long time) override;

    ECode Before(
        /* [in] */ IDate* when,
        /* [out] */ Boolean& before) override;

    ECode After(
        /* [in] */ IDate* when,
        /* [out] */ Boolean& after) override;

    ECode Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean& same) override;

    static Long GetMillisOf(
        /* [in] */ IDate* date);

    ECode CompareTo(
        /* [in] */ IInterface* other,
        /* [out] */ Integer& result) override;

    ECode GetHashCode(
        /* [out] */ Integer& hash) override;

    ECode ToString(
        /* [out] */ String& desc) override;

    ECode ToLocaleString(
        /* [out] */ String& str) override;

    ECode ToGMTString(
        /* [out] */ String& str) override;

    ECode GetTimezoneOffset(
        /* [out] */ Integer& tzOffset) override;

    static Date* From(
        /* [in] */ IDate* date);

protected:
    ECode CloneImpl(
        /* [in] */ IDate* newObj);

private:
    static SyncObject& GetClassLock();

    static AutoPtr<IBaseCalendar> GetGcal();

    static Array<String>& GetWtb();

    Long GetTimeImpl();

    void ConvertToAbbr(
        /* [in] */ IStringBuilder* sb,
        /* [in] */ const String& name);

    AutoPtr<IBaseCalendarDate> GetCalendarDate();

    AutoPtr<IBaseCalendarDate> Normalize();

    AutoPtr<IBaseCalendarDate> Normalize(
        /* [in] */ IBaseCalendarDate* date);

    static AutoPtr<IBaseCalendar> GetCalendarSystem(
        /* [in] */ Integer year);

    static AutoPtr<IBaseCalendar> GetCalendarSystem(
        /* [in] */ Long utc);

    static AutoPtr<IBaseCalendar> GetCalendarSystem(
        /* [in] */ IBaseCalendarDate* cdate);

    static AutoPtr<IBaseCalendar> GetJulianCalendar();

private:
    static AutoPtr<IBaseCalendar> sJcal;

    Long mFastTime;

    AutoPtr<IBaseCalendarDate> mCdate;

    static Integer sDefaultCenturyStart;

    static constexpr Integer sTtb[] = {
        14, 1, 0, 0, 0, 0, 0, 0, 0,
        2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
        10000 + 0, 10000 + 0, 10000 + 0,    // GMT/UT/UTC
        10000 + 5 * 60, 10000 + 4 * 60,     // EST/EDT
        10000 + 6 * 60, 10000 + 5 * 60,     // CST/CDT
        10000 + 7 * 60, 10000 + 6 * 60,     // MST/MDT
        10000 + 8 * 60, 10000 + 7 * 60      // PST/PDT
    };
};

inline Date* Date::From(
    /* [in] */ IDate* date)
{
    return (Date*)date;
}

}
}

#endif // __COMO_UTIL_DATE_H__
