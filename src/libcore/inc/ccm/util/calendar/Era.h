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

#ifndef __CCM_UTIL_CALENDAR_ERA_H__
#define __CCM_UTIL_CALENDAR_ERA_H__

#include "ccm/core/SyncObject.h"
#include "ccm.util.calendar.ICalendarDate.h"
#include "ccm.util.calendar.IEra.h"
#include <ccmautoptr.h>

using ccm::core::SyncObject;

namespace ccm {
namespace util {
namespace calendar {

class Era
    : public SyncObject
    , public IEra
{
public:
    CCM_INTERFACE_DECL();

    ECode Constructor(
        /* [in] */ const String& name,
        /* [in] */ const String& abbr,
        /* [in] */ Long since,
        /* [in] */ Boolean localTime);

    ECode GetName(
        /* [out] */ String* name) override;

    ECode GetDisplayName(
        /* [in] */ ILocale* locale,
        /* [out] */ String* name) override;

    ECode GetAbbreviation(
        /* [out] */ String* abbr) override;

    ECode GetDiaplayAbbreviation(
        /* [in] */ ILocale* locale,
        /* [out] */ String* abbr) override;

    ECode GetSince(
        /* [in] */ ITimeZone* zone,
        /* [out] */ Long* time) override;

    ECode GetSinceDate(
        /* [out] */ ICalendarDate** date) override;

    ECode IsLocalTime(
        /* [out] */ Boolean* result) override;

    ECode Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* same) override;

    ECode GetHashCode(
        /* [out] */ Integer* hash) override;

    ECode ToString(
        /* [out] */ String* desc) override;

private:
    String mName;
    String mAbbr;
    Long mSince = 0;
    AutoPtr<ICalendarDate> mSinceDate;
    Boolean mLocalTime = false;
    Integer mHash = 0;
};

}
}
}

#endif // __CCM_UTIL_CALENDAR_ERA_H__
