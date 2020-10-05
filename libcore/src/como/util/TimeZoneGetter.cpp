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

#include "como/core/System.h"
#include "como/util/TimeZoneGetter.h"

using como::core::System;

namespace como {
namespace util {

class DefaultTimeZoneGetter
    : public TimeZoneGetter
{
public:
    ECode GetId(
        /* [out] */ String& name) override
    {
        return System::GetProperty("persist.sys.timezone", name);
    }

private:
    String mId;
};

//-----------------------------------------------------------------------

AutoPtr<ITimeZoneGetter> TimeZoneGetter::sInstance;

COMO_INTERFACE_IMPL_1(TimeZoneGetter, SyncObject, ITimeZoneGetter);

AutoPtr<ITimeZoneGetter> TimeZoneGetter::GetInstance()
{
    if (sInstance == nullptr) {
        sInstance = new DefaultTimeZoneGetter();
    }
    return sInstance;
}

ECode TimeZoneGetter::SetInstance(
    /* [in] */ ITimeZoneGetter* getter)
{
    if (getter == nullptr) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    sInstance = getter;
    return NOERROR;
}

}
}
