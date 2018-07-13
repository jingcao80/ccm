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

#include "ccm/core/AutoLock.h"
#include "ccm/core/ClassLoader.h"
#include "ccm/core/CoreUtils.h"
#include "ccm/util/CHashMap.h"
#include "ccm/util/CProperties.h"
#include "ccm/util/calendar/CalendarSystem.h"
#include "ccm/util/calendar/LocalGregorianCalendar.h"
#include "ccm.util.IMap.h"
#include "ccm.util.calendar.ILocalGregorianCalendar.h"
#include <ccmlogger.h>

using ccm::core::AutoLock;
using ccm::core::ClassLoader;
using ccm::core::CoreUtils;

namespace ccm {
namespace util {
namespace calendar {

INIT_PROI_3 AutoPtr<IMap> CalendarSystem::sNames;
INIT_PROI_3 AutoPtr<IConcurrentMap> CalendarSystem::sCalendars;
INIT_PROI_3 AutoPtr<IGregorian> CalendarSystem::GREGORIAN_INSTANCE;

CCM_INTERFACE_IMPL_1(CalendarSystem, SyncObject, ICalendarSystem);

ECode CalendarSystem::StaticInitialize()
{
    static Boolean sInitialized = false;
    static SyncObject sLock;

    {
        AutoLock lock(sLock);
        if (!sInitialized) {
            AutoPtr<IClassLoader> loader = ClassLoader::GetSystemClassLoader();
            AutoPtr<IMetaCoclass> gregorianClass;
            loader->LoadCoclass(String("ccm::util::calendar::CGregorian"), (IMetaCoclass**)&gregorianClass);
            AutoPtr<IMetaCoclass> japaneseClass;
            loader->LoadCoclass(String("ccm::util::calendar::CLocalGregorianCalendar"), (IMetaCoclass**)&japaneseClass);
            AutoPtr<IMetaCoclass> julianClass;
            loader->LoadCoclass(String("ccm::util::calendar::CJulianCalendar"), (IMetaCoclass**)&julianClass);
            CHashMap::New(IID_IMap, (IInterface**)&sNames);
            sNames->Put(CoreUtils::Box(String("gregorian")), gregorianClass);
            sNames->Put(CoreUtils::Box(String("japanese")), japaneseClass);
            sNames->Put(CoreUtils::Box(String("julian")), julianClass);

            sInitialized = true;
        }
    }

    return NOERROR;
}

AutoPtr<IGregorian> CalendarSystem::GetGregorianCalendar()
{
    StaticInitialize();
    return GREGORIAN_INSTANCE;
}

ECode CalendarSystem::ForName(
    /* [in] */ const String& calendarName,
    /* [out] */ ICalendarSystem** system)
{
    VALIDATE_NOT_NULL(system);

    StaticInitialize();

    if (String("gregorian").Equals(calendarName)) {
        *system = ICalendarSystem::Probe(GREGORIAN_INSTANCE);
        REFCOUNT_ADD(*system);
        return NOERROR;
    }

    AutoPtr<ICalendarSystem> cal;
    IMap::Probe(sCalendars)->Get(CoreUtils::Box(calendarName),
            (IInterface**)&cal);
    if (cal != nullptr) {
        *system = cal;
        REFCOUNT_ADD(*system);
        return NOERROR;
    }

    AutoPtr<IMetaCoclass> calendarClass;
    sNames->Get(CoreUtils::Box(calendarName), (IInterface**)&calendarClass);
    if (calendarClass == nullptr) {
        *system = nullptr;
        return NOERROR;
    }

    Boolean contains;
    if (calendarClass->ContainsInterface(String("ccm::util::calendar::ILocalGregorianCalendar"),
            &contains), contains) {
        AutoPtr<ILocalGregorianCalendar> lgCalendar;
        LocalGregorianCalendar::GetLocalGregorianCalendar(calendarName, (ILocalGregorianCalendar**)&lgCalendar);
        cal = ICalendarSystem::Probe(lgCalendar);
    }
    else {
        ECode ec = calendarClass->CreateObject(IID_ICalendarSystem,
                (IInterface**)&cal);
        if (FAILED(ec)) {
            Logger::E("CalendarSystem", "internal error");
            return E_RUNTIME_EXCEPTION;
        }
    }
    if (cal == nullptr) {
        *system = nullptr;
        return NOERROR;
    }

    AutoPtr<ICalendarSystem> cs;
    sCalendars->PutIfAbsent(CoreUtils::Box(calendarName), cal, (IInterface**)&cs);
    *system = cs == nullptr ? cal : cs;
    REFCOUNT_ADD(*system);
    return NOERROR;
}

ECode CalendarSystem::GetCalendarProperties(
    /* [out] */ IProperties** prop)
{
    VALIDATE_NOT_NULL(prop);

    StaticInitialize();

    AutoPtr<IProperties> calendarProps;
    CProperties::New(IID_IProperties, (IInterface**)&calendarProps);
    AutoPtr<IInputStream> is = ClassLoader::GetSystemResourceAsStream(
            String("calendars.properties"));
    ECode ec = calendarProps->Load(is);
    is->Close();
    if (FAILED(ec)) return ec;
    *prop = calendarProps;
    REFCOUNT_ADD(*prop);
    return NOERROR;
}

}
}
}
