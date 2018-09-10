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

#ifndef __CCM_UTIL_GREGORIANCALENDAR_H__
#define __CCM_UTIL_GREGORIANCALENDAR_H__

#include "ccm/util/Calendar.h"
#include "ccm.util.IGregorianCalendar.h"
#include "ccm.util.ILocale.h"
#include "ccm.util.ITimeZone.h"
#include "ccm.util.calendar.IBaseCalendarDate.h"
#include "ccm.util.calendar.IGregorian.h"

using ccm::util::calendar::IBaseCalendarDate;
using ccm::util::calendar::IGregorian;

namespace ccm {
namespace util {

class GregorianCalendar
    : public Calendar
    , public IGregorianCalendar
{
public:
    CCM_INTERFACE_DECL();

    ECode Constructor();

    ECode Constructor(
        /* [in] */ ITimeZone* zone);

    ECode Constructor(
        /* [in] */ ILocale* locale);

    ECode Constructor(
        /* [in] */ ITimeZone* zone,
        /* [in] */ ILocale* locale);

    ECode Constructor(
        /* [in] */ Integer year,
        /* [in] */ Integer month,
        /* [in] */ Integer dayOfMonth);

    ECode Constructor(
        /* [in] */ Integer year,
        /* [in] */ Integer month,
        /* [in] */ Integer dayOfMonth,
        /* [in] */ Integer hourOfDay,
        /* [in] */ Integer minute);

    ECode Constructor(
        /* [in] */ Integer year,
        /* [in] */ Integer month,
        /* [in] */ Integer dayOfMonth,
        /* [in] */ Integer hourOfDay,
        /* [in] */ Integer minute,
        /* [in] */ Integer second);

    ECode Constructor(
        /* [in] */ Integer year,
        /* [in] */ Integer month,
        /* [in] */ Integer dayOfMonth,
        /* [in] */ Integer hourOfDay,
        /* [in] */ Integer minute,
        /* [in] */ Integer second,
        /* [in] */ Integer millis);

private:
    static AutoPtr<IGregorian> GetGcal();

private:
    AutoPtr<IBaseCalendarDate> mGdate;
};

}
}

#endif // __CCM_UTIL_GREGORIANCALENDAR_H__
