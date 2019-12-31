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

#include "ccm/util/calendar/CalendarSystem.h"
#include "ccm/util/calendar/CalendarSystemFactory.h"

namespace ccm {
namespace util {
namespace calendar {

ECode CalendarSystemFactory::ForName(
    /* [in] */ const String& calendarName,
    /* [out] */ ICalendarSystem** system)
{
    return CalendarSystem::ForName(calendarName, system);
}

ECode CalendarSystemFactory::GetCalendarProperties(
    /* [out] */ IProperties** prop)
{
    return CalendarSystem::GetCalendarProperties(prop);
}

ECode CalendarSystemFactory::GetGregorianCalendar(
    /* [out] */ IGregorian** gcal)
{
    VALIDATE_NOT_NULL(gcal);

    CalendarSystem::GetGregorianCalendar().MoveTo(gcal);
    return NOERROR;
}

}
}
}
