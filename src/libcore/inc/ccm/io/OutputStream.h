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

#ifndef __CCM_IO_OUTPUTSTREAM_H__
#define __CCM_IO_OUTPUTSTREAM_H__

#include "ccm/core/SyncObject.h"
#include "ccm.core.IAutoCloseable.h"
#include "ccm.io.IOutputStream.h"
#include "ccm.io.IFlushable.h"
#include "ccm.io.ICloseable.h"

using ccm::core::IAutoCloseable;
using ccm::core::SyncObject;

namespace ccm {
namespace io {

class OutputStream
    : public SyncObject
    , public IOutputStream
    , public IFlushable
    , public ICloseable
    , public IAutoCloseable
{
public:
    CCM_INTERFACE_DECL();

    ECode Write(
        /* [in] */ const Array<Byte>& buffer) override;

    ECode Write(
        /* [in] */ const Array<Byte>& buffer,
        /* [in] */ Integer offset,
        /* [in] */ Integer size) override;

    ECode Flush() override;

    ECode Close() override;
};

}
}

#endif // __CCM_IO_OUTPUTSTREAM_H__
