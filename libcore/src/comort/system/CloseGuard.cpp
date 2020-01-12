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

#include "como/core/CStackTrace.h"
#include "como/core/System.h"
#include "comort/system/CloseGuard.h"
#include <comolog.h>

using como::core::CStackTrace;
using como::core::IID_IStackTrace;
using como::core::System;

namespace comort {
namespace system {

Boolean CloseGuard::ENABLED = true;

COMO_INTERFACE_IMPL_1(CloseGuard, SyncObject, ICloseGuard);

AutoPtr<ICloseGuard> CloseGuard::GetNOOP()
{
    static AutoPtr<ICloseGuard> NOOP = new CloseGuard();
    return NOOP;
}

AutoPtr<ICloseGuardReporter> CloseGuard::GetOrSetREPORTER(
    /* [in] */ ICloseGuardReporter* reporter)
{
    static AutoPtr<ICloseGuardReporter> REPORTER = new DefaultReporter();
    if (reporter != nullptr) {
        REPORTER = reporter;
    }
    return REPORTER;
}

AutoPtr<ICloseGuardTracker> CloseGuard::GetOrSetTRACKER(
    /* [in] */ ICloseGuardTracker* tracker)
{
    static AutoPtr<ICloseGuardTracker> DEFAULT_TRACKER = new DefaultTracker();
    static AutoPtr<ICloseGuardTracker> TRACKER = DEFAULT_TRACKER;
    if (tracker != nullptr) {
        TRACKER = tracker;
    }
    return TRACKER;
}

AutoPtr<ICloseGuard> CloseGuard::Get()
{
    if (!ENABLED) {
        return GetNOOP();
    }
    return new CloseGuard();
}

ECode CloseGuard::SetReporter(
    /* [in] */ ICloseGuardReporter* reporter)
{
    if (reporter == nullptr) {
        Logger::E("CloseGuard", "reporter == null");
        return como::core::E_NULL_POINTER_EXCEPTION;
    }
    GetOrSetREPORTER(reporter);
    return NOERROR;
}

ECode CloseGuard::SetTracker(
    /* [in] */ ICloseGuardTracker* tracker)
{
    if (tracker == nullptr) {
        Logger::E("CloseGuard", "tracker == null");
        return como::core::E_NULL_POINTER_EXCEPTION;
    }
    GetOrSetTRACKER(tracker);
    return NOERROR;
}

ECode CloseGuard::Open(
    /* [in] */ const String& closer)
{
    if (closer.IsNull()) {
        Logger::E("CloseGuard", "closer == null");
        return como::core::E_NULL_POINTER_EXCEPTION;
    }
    if ((ICloseGuard*)this == GetNOOP() || !ENABLED) {
        return NOERROR;
    }
    String message = String::Format("Explicit termination method '%s' not called", closer.string());
    CStackTrace::New(message, IID_IStackTrace, (IInterface**)&mAllocationSite);
    GetOrSetTRACKER(nullptr)->Open(mAllocationSite);
    return NOERROR;
}

ECode CloseGuard::Close()
{
    GetOrSetTRACKER(nullptr)->Close(mAllocationSite);
    mAllocationSite = nullptr;
    return NOERROR;
}

ECode CloseGuard::WarnIfOpen()
{
    if (mAllocationSite == nullptr || !ENABLED) {
        return NOERROR;
    }

    String message("A resource was acquired at attached stack trace but never released. "
                   "See como.io.ICloseable for information on avoiding resource leaks.");

    GetOrSetREPORTER(nullptr)->Report(message, mAllocationSite);
    return NOERROR;
}

//-------------------------------------------------------------------------

COMO_INTERFACE_IMPL_1(CloseGuard::DefaultReporter, SyncObject, ICloseGuardReporter);

ECode CloseGuard::DefaultReporter::Report(
    /* [in] */ const String& message,
    /* [in] */ IStackTrace* allocationSite)
{
    return System::LogW(message, allocationSite);
}

//-------------------------------------------------------------------------

COMO_INTERFACE_IMPL_1(CloseGuard::DefaultTracker, SyncObject, ICloseGuardTracker);

ECode CloseGuard::DefaultTracker::Open(
    /* [in] */ IStackTrace* allocationSite)
{
    return NOERROR;
}

ECode CloseGuard::DefaultTracker::Close(
    /* [in] */ IStackTrace* allocationSite)
{
    return NOERROR;
}

}
}
