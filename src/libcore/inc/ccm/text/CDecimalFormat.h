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

#ifndef __CCM_TEXT_CDECIMALFORMAT_H__
#define __CCM_TEXT_CDECIMALFORMAT_H__

#include "ccm/core/SyncObject.h"
#include "_ccm_text_CDecimalFormat.h"
#include "ccm.text.IDecimalFormat.h"
#include "ccm.text.IDecimalFormatSymbols.h"

using ccm::core::SyncObject;

namespace ccm {
namespace text {

Coclass(CDecimalFormat)
    , public SyncObject
{
public:
    // CCM_OBJECT_DECL();

    ECode Constructor()
    {
        return NOERROR;
    }

    ECode Constructor(
        /* [in] */ const String& pattern)
    {
        return NOERROR;
    }

    ECode Constructor(
        /* [in] */ const String& pattern,
        /* [in] */ IDecimalFormatSymbols* symbols)
    {
        return NOERROR;
    }

    void AdjustForCurrencyDefaultFractionDigits()
    {}

    static CDecimalFormat* From(
        /* [in] */ IDecimalFormat* obj)
    {
        return nullptr;
    }
};

}
}

#endif // __CCM_TEXT_CDECIMALFORMAT_H__
