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

#ifndef __CCM_IO_CHARSET_CHARSETENCODERICU_H__
#define __CCM_IO_CHARSET_CHARSETENCODERICU_H__

#include "ccm.io.charset.ICharset.h"
#include "ccm.io.charset.ICharsetEncoder.h"

namespace ccm {
namespace io {
namespace charset {

class CharsetEncoderICU
    : public ICharsetEncoder
{
public:
    static AutoPtr<CharsetEncoderICU> NewInstance(
        /* [in] */ ICharset* cs,
        /* [in] */ const String& icuCanonicalName)
    {
        return nullptr;
    }
};

}
}
}

#endif // __CCM_IO_CHARSET_CHARSETENCODERICU_H__
