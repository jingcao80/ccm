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

#ifndef __CCM_TEXT_DATEFORMATSYMBOLS_H__
#define __CCM_TEXT_DATEFORMATSYMBOLS_H__

#include "ccm/core/SyncObject.h"
#include "ccm.core.ICloneable.h"
#include "ccm.io.ISerializable.h"
#include "ccm.text.IDateFormatSymbols.h"
#include "ccm.util.ILocale.h"
#include <ccmautoptr.h>

using ccm::core::SyncObject;
using ccm::core::ICloneable;
using ccm::io::ISerializable;
using ccm::util::ILocale;

namespace ccm {
namespace text {

class DateFormatSymbols
    : public SyncObject
    , public IDateFormatSymbols
    , public ISerializable
    , public ICloneable
{
public:
    CCM_INTERFACE_DECL();

    static AutoPtr<IDateFormatSymbols> GetInstance(
        /* [in] */ ILocale* locale)
    {
        return nullptr;
    }
};

}
}

#endif // __CCM_TEXT_DATEFORMATSYMBOLS_H__