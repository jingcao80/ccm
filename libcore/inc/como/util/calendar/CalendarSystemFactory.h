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

#ifndef __COMO_UTIL_CALENDAR_CALENDARSYSTEMFACTORY_H__
#define __COMO_UTIL_CALENDAR_CALENDARSYSTEMFACTORY_H__

#include "como.util.IProperties.h"
#include "como.util.calendar.ICalendarSystem.h"
#include "como.util.calendar.IGregorian.h"

namespace como {
namespace util {
namespace calendar {

class COM_PUBLIC CalendarSystemFactory
{
public:
    static ECode ForName(
        /* [in] */ const String& calendarName,
        /* [out] */ AutoPtr<ICalendarSystem>& system);

    static ECode GetCalendarProperties(
        /* [out] */ AutoPtr<IProperties>& prop);

    static ECode GetGregorianCalendar(
        /* [out] */ AutoPtr<IGregorian>& gcal);

private:
    CalendarSystemFactory();
};

}
}
}

#endif // __COMO_UTIL_CALENDAR_CALENDARSYSTEMFACTORY_H__
