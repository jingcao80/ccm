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

#ifndef __COMO_CORE_STACKTRACEELEMENT_H__
#define __COMO_CORE_STACKTRACEELEMENT_H__

#include "como/core/SyncObject.h"
#include "como.core.IStackTraceElement.h"
#include "como.io.ISerializable.h"

using como::io::ISerializable;

namespace como {
namespace core {

class StackTraceElement
    : public SyncObject
    , public IStackTraceElement
    , public ISerializable
{
public:
    COMO_INTERFACE_DECL();

    ECode Constructor(
        /* [in] */ const String& no,
        /* [in] */ const String& pc,
        /* [in] */ const String& soname,
        /* [in] */ const String& symbol);

    ECode ToString(
        /* [out] */ String& desc) override;

private:
    String mNo;
    String mPC;
    String mSoname;
    String mSymbol;
};

}
}

#endif // __COMO_CORE_CSTACKTRACEELEMENT_H__