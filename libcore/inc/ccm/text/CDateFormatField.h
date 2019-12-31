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

#ifndef __CCM_TEXT_CDATEFORMATFIELD_H__
#define __CCM_TEXT_CDATEFORMATFIELD_H__

#include "ccm/text/DateFormat.h"
#include "_ccm_text_CDateFormatField.h"

namespace ccm {
namespace text {

Coclass(CDateFormatField)
    , public DateFormat::Field
{
public:
    CCM_OBJECT_DECL();

    static ECode New(
        /* [in] */ const String& name,
        /* [in] */ Integer calendarField,
        /* [in] */ const InterfaceID& iid,
        /* [out] */ ccm::IInterface** object);
};

}
}

#endif // __CCM_TEXT_CDATEFORMATFIELD_H__
