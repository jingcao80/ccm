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

#ifndef __CCM_UTIL_CALENDAR_CERA_H__
#define __CCM_UTIL_CALENDAR_CERA_H__

#include "ccm/util/calendar/Era.h"
#include "_ccm_util_calendar_CEra.h"

namespace ccm {
namespace util {
namespace calendar {

Coclass(CEra)
    , public Era
{
public:
    CCM_OBJECT_DECL();
};

}
}
}

#endif // __CCM_UTIL_CALENDAR_CERA_H__
