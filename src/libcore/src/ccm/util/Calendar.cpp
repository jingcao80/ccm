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

#include "ccm/util/Calendar.h"
#include "ccm/util/CLocale.h"
#include "ccm/util/TimeZone.h"
#include <ccmautoptr.h>

using ccm::core::IID_ICloneable;
using ccm::core::IID_IComparable;
using ccm::io::IID_ISerializable;

namespace ccm {
namespace util {

CCM_INTERFACE_IMPL_4(Calendar, SyncObject, ICalendar, ISerializable, ICloneable, IComparable);

ECode Calendar::Constructor()
{
    Constructor(TimeZone::GetDefaultRef(),
            CLocale::GetDefault(CLocale::Category::GetFORMAT()));
    mSharedZone = true;
    return NOERROR;
}

ECode Calendar::Constructor(
    /* [in] */ ITimeZone* zone,
    /* [in] */ ILocale* locale)
{
    AutoPtr<ILocale> aLocale = locale;
    if (aLocale == nullptr) {
        aLocale = CLocale::GetDefault();
    }
    mFields = Array<Integer>(FIELD_COUNT);
    mIsSet = Array<Boolean>(FIELD_COUNT);
    mStamp = Array<Integer>(FIELD_COUNT);

    mZone = zone;
    SetWeekCountData(aLocale);
    return NOERROR;
}

}
}
