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

#ifndef __COMO_IO_STRINGWRITER_H__
#define __COMO_IO_STRINGWRITER_H__

#include "como/io/Writer.h"
#include "como.core.IStringBuffer.h"
#include "como.io.IStringWriter.h"
#include <comosp.h>

using como::core::IStringBuffer;

namespace como {
namespace io {

class StringWriter
    : public Writer
    , public IStringWriter
{
public:
    COMO_INTERFACE_DECL();

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
        /* [out] */ String& str) override;

    ECode GetBuffer(
        /* [out] */ IStringBuffer** buf) override;

    ECode Flush() override;

    ECode Close() override;

private:
    AutoPtr<IStringBuffer> mBuf;
};

}
}

#endif // __COMO_IO_STRINGWRITER_H__
