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

#ifndef __COMO_IO_OUTPUTSTREAMWRITER_H__
#define __COMO_IO_OUTPUTSTREAMWRITER_H__

#include "como/io/Writer.h"
#include "como/io/charset/StreamEncoder.h"
#include "como.io.IOutputStream.h"
#include "como.io.IOutputStreamWriter.h"
#include "como.io.charset.ICharset.h"
#include "como.io.charset.ICharsetEncoder.h"
#include "como.io.charset.IStreamEncoder.h"
#include <comosp.h>

using como::io::charset::ICharset;
using como::io::charset::ICharsetEncoder;
using como::io::charset::IStreamEncoder;
using como::io::charset::StreamEncoder;

namespace como {
namespace io {

class OutputStreamWriter
    : public Writer
    , public IOutputStreamWriter
{
public:
    COMO_INTERFACE_DECL();

    ECode Constructor(
        /* [in] */ IOutputStream* out,
        /* [in] */ const String& charsetName);

    ECode Constructor(
        /* [in] */ IOutputStream* out);

    ECode Constructor(
        /* [in] */ IOutputStream* out,
        /* [in] */ ICharset* cs);

    ECode Constructor(
        /* [in] */ IOutputStream* out,
        /* [in] */ ICharsetEncoder* enc);

    ECode GetEncoding(
        /* [out] */ String* name) override;

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

    ECode Flush() override;

    ECode Close() override;

protected:
    ECode FlushBuffer();

private:
    AutoPtr<IStreamEncoder> mSe;
};

}
}

#endif // __COMO_IO_OUTPUTSTREAMWRITER_H__
