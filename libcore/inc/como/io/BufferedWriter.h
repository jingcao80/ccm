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

#ifndef __COMO_IO_BUFFEREDWRITER_H__
#define __COMO_IO_BUFFEREDWRITER_H__

#include "como/io/Writer.h"
#include "como.io.IBufferedWriter.h"
#include <ccmautoptr.h>

namespace como {
namespace io {

class BufferedWriter
    : public Writer
    , public IBufferedWriter
{
public:
    COMO_INTERFACE_DECL();

    ECode Constructor(
        /* [in] */ IWriter* out);

    ECode Constructor(
        /* [in] */ IWriter* out,
        /* [in] */ Integer sz);

    ECode Write(
        /* [in] */ Integer c) override;

    ECode Write(
        /* [in] */ const Array<Char>& buffer,
        /* [in] */ Integer off,
        /* [in] */ Integer len) override;

    ECode Write(
        /* [in] */ const String& str,
        /* [in] */ Integer off,
        /* [in] */ Integer len) override;

    ECode NewLine() override;

    ECode Flush() override;

    ECode Close() override;

    using Writer::Write;

protected:
    ECode FlushBuffer();

private:
    ECode EnsureOpen();

private:
    AutoPtr<IWriter> mOut;

    Array<Char> mCb;
    Integer mNChars;
    Integer mNextChar;

    static constexpr Integer sDefaultCharBufferSize = 8192;

    String mLineSeparator;
};

}
}

#endif // __COMO_IO_BUFFEREDWRITER_H__
