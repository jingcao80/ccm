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

#ifndef __CCM_UTIL_CALENDAR_H__
#define __CCM_UTIL_CALENDAR_H__

#include "ccm/core/SyncObject.h"
#include "ccm.core.ICloneable.h"
#include "ccm.core.IComparable.h"
#include "ccm.io.ISerializable.h"
#include "ccm.util.ICalendar.h"
#include "ccm.util.ILocale.h"
#include "ccm.util.ITimeZone.h"

using ccm::core::ICloneable;
using ccm::core::IComparable;
using ccm::core::SyncObject;
using ccm::io::ISerializable;

namespace ccm {
namespace util {

class Calendar
    : public SyncObject
    , public ICalendar
    , public ISerializable
    , public ICloneable
    , public IComparable
{
public:
    CCM_INTERFACE_DECL();

protected:
    ECode Constructor();

    ECode Constructor(
        /* [in] */ ITimeZone* zone,
        /* [in] */ ILocale* alocale);

private:
    void SetWeekCountData(
        /* [in] */ ILocale* desiredLocale)
    {}

protected:
    Array<Integer> mFields;

    Array<Boolean> mIsSet;

private:
    Array<Integer> mStamp;

    AutoPtr<ITimeZone> mZone;

    Boolean mSharedZone = false;
};

}
}

#endif // __CCM_UTIL_CALENDAR_H__
