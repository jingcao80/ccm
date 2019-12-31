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

#ifndef __CCM_IO_STRINGWRITER_H__
#define __CCM_IO_STRINGWRITER_H__

#include "ccm/io/Writer.h"
#include "ccm.core.IStringBuffer.h"
#include "ccm.io.IStringWriter.h"
#include <ccmautoptr.h>

using ccm::core::IStringBuffer;

namespace ccm {
namespace io {

class StringWriter
    : public Writer
    , public IStringWriter
{
public:
    CCM_INTERFACE_DECL();

    ECode Constructor();

    ECode Constructor(
        /* [in] */ Integer initialSize);

    ECode Write(
        /* [in] */ Integer c) override;

    ECode Write(
        /* [in] */ const Array<Char>& buffer,
        /* [in] */ Integer off,
        /* [in] */ Integer len) override;

    ECode Write(
        /* [in] */ const String& str) override;

    ECode Write(
        /* [in] */ const String& str,
        /* [in] */ Integer off,
        /* [in] */ Integer len) override;

    ECode ToString(
        /* [out] */ String* str) override;

    ECode GetBuffer(
        /* [out] */ IStringBuffer** buf) override;

    ECode Flush() override;

    ECode Close() override;

private:
    AutoPtr<IStringBuffer> mBuf;
};

}
}

#endif // __CCM_IO_STRINGWRITER_H__
