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

#ifndef __COMO_UTIL_CALENDAR_CALENDARSYSTEM_H__
#define __COMO_UTIL_CALENDAR_CALENDARSYSTEM_H__

#include "como/core/SyncObject.h"
#include "como.util.IMap.h"
#include "como.util.IProperties.h"
#include "como.util.calendar.ICalendarSystem.h"
#include "como.util.calendar.IGregorian.h"
#include "como.util.concurrent.IConcurrentMap.h"
#include <comosp.h>

using como::core::SyncObject;
using como::util::concurrent::IConcurrentMap;

namespace como {
namespace util {
namespace calendar {

class CalendarSystem
    : public SyncObject
    , public ICalendarSystem
{
public:
    COMO_INTERFACE_DECL();

    static AutoPtr<IGregorian> GetGregorianCalendar();

    static ECode ForName(
        /* [in] */ const String& calendarName,
        /* [out] */ ICalendarSystem** system);

    static ECode GetCalendarProperties(
        /* [out] */ IProperties** prop);

private:
    static ECode StaticInitialize();

private:
    static AutoPtr<IMap> sNames;

    static AutoPtr<IConcurrentMap> sCalendars;

    static AutoPtr<IGregorian> GREGORIAN_INSTANCE;
};

}
}
}

#endif // __COMO_UTIL_CALENDAR_CALENDARSYSTEM_H__
