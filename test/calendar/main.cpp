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

#include "ccm.core.CSystem.h"
#include "ccm.core.ISystem.h"
#include "ccm.util.calendar.CGregorian.h"
#include "ccm.util.calendar.ICalendarDate.h"
#include "ccm.util.calendar.ICalendarSystem.h"
#include "ccm.util.calendar.IGregorian.h"
#include <ccmautoptr.h>
#include <ccmobject.h>

#include <stdio.h>

using namespace ccm;
using ccm::core::CSystem;
using ccm::core::ISystem;
using ccm::core::IID_ISystem;
using ccm::util::calendar::CGregorian;
using ccm::util::calendar::ICalendarDate;
using ccm::util::calendar::ICalendarSystem;
using ccm::util::calendar::IGregorian;
using ccm::util::calendar::IID_ICalendarSystem;

void TestCGregorian()
{
    AutoPtr<ISystem> sys;
    CSystem::New(IID_ISystem, (IInterface**)&sys);
    Long millis;
    sys->GetCurrentTimeMillis(&millis);
    AutoPtr<ICalendarSystem> gcal;
    CGregorian::New(IID_ICalendarSystem, (IInterface**)&gcal);
    AutoPtr<ICalendarDate> date;
    gcal->GetCalendarDate(millis, (ICalendarDate**)&date);
    printf("==== date: %s ====\n", Object::ToString(date).string());
    date = nullptr;
    gcal->GetCalendarDate((ICalendarDate**)&date);
    printf("==== date: %s ====\n", Object::ToString(date).string());
}

int main(int argc, char **argv)
{
    TestCGregorian();
    return 0;
}