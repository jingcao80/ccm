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

#include "como/core/AutoLock.h"
#include "como/core/ClassLoader.h"
#include "como/core/CoreUtils.h"
#include "como/util/CHashMap.h"
#include "como/util/CProperties.h"
#include "como/util/calendar/CalendarSystem.h"
#include "como/util/calendar/CGregorian.h"
#include "como/util/calendar/LocalGregorianCalendar.h"
#include "como.util.IMap.h"
#include "como.util.calendar.ILocalGregorianCalendar.h"
#include <comolog.h>

using como::core::AutoLock;
using como::core::ClassLoader;
using como::core::CoreUtils;

namespace como {
namespace util {
namespace calendar {

INIT_PROI_3 AutoPtr<IMap> CalendarSystem::sNames;
INIT_PROI_3 AutoPtr<IConcurrentMap> CalendarSystem::sCalendars;
INIT_PROI_3 AutoPtr<IGregorian> CalendarSystem::GREGORIAN_INSTANCE;

COMO_INTERFACE_IMPL_1(CalendarSystem, SyncObject, ICalendarSystem);

ECode CalendarSystem::StaticInitialize()
{
    static Boolean sInitialized = false;
    static SyncObject sLock;

    if (!sInitialized) {
        AutoLock lock(sLock);
        if (!sInitialized) {
            AutoPtr<IClassLoader> loader = ClassLoader::GetSystemClassLoader();
            AutoPtr<IMetaCoclass> gregorianClass;
            loader->LoadCoclass(String("como::util::calendar::CGregorian"), gregorianClass);
            AutoPtr<IMetaCoclass> japaneseClass;
            loader->LoadCoclass(String("como::util::calendar::CLocalGregorianCalendar"), japaneseClass);
            AutoPtr<IMetaCoclass> julianClass;
            loader->LoadCoclass(String("como::util::calendar::CJulianCalendar"), julianClass);
            CHashMap::New(IID_IMap, (IInterface**)&sNames);
            sNames->Put(CoreUtils::Box(String("gregorian")), gregorianClass);
            sNames->Put(CoreUtils::Box(String("japanese")), japaneseClass);
            sNames->Put(CoreUtils::Box(String("julian")), julianClass);

            CGregorian::New(IID_IGregorian, (IInterface**)&GREGORIAN_INSTANCE);

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
        *system = ICalendarSystem::Probe(GetGregorianCalendar());
        REFCOUNT_ADD(*system);
        return NOERROR;
    }

    AutoPtr<ICalendarSystem> cal;
    IMap::Probe(sCalendars)->Get(CoreUtils::Box(calendarName),
            (IInterface**)&cal);
    if (cal != nullptr) {
        cal.MoveTo(system);
        return NOERROR;
    }

    AutoPtr<IMetaCoclass> calendarClass;
    sNames->Get(CoreUtils::Box(calendarName), (IInterface**)&calendarClass);
    if (calendarClass == nullptr) {
        *system = nullptr;
        return NOERROR;
    }

    Boolean contains;
    if (calendarClass->ContainsInterface(String("como::util::calendar::ILocalGregorianCalendar"),
            contains), contains) {
        AutoPtr<ILocalGregorianCalendar> lgCalendar;
        LocalGregorianCalendar::GetLocalGregorianCalendar(calendarName, &lgCalendar);
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
    if (cs == nullptr) {
        cal.MoveTo(system);
    }
    else {
        cs.MoveTo(system);
    }
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
    calendarProps.MoveTo(prop);
    return NOERROR;
}

}
}
}
