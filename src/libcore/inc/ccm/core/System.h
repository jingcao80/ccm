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

#ifndef __CCM_CORE_SYSTEM_H__
#define __CCM_CORE_SYSTEM_H__

#include "ccm.io.IPrintStream.h"
#include <ccmautoptr.h>
#include <ccmtypes.h>

using ccm::io::IPrintStream;

namespace ccm {
namespace core {

class System
{
public:
    static AutoPtr<IPrintStream> GetOut();

    static Long GetCurrentTimeMillis();

    static Long GetNanoTime();

    static ECode GetProperty(
        /* [in] */ const String& key,
        /* [out] */ String* value);

    static ECode GetProperty(
        /* [in] */ const String& key,
        /* [in] */ const String& def,
        /* [out] */ String* value);

private:
    System();
};

}
}

#endif // __CCM_CORE_SYSTEM_H__
