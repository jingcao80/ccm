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

#ifndef __CCM_IO_OUTPUTSTREAMWRITER_H__
#define __CCM_IO_OUTPUTSTREAMWRITER_H__

#include "ccm/io/Writer.h"
#include "ccm/io/charset/StreamEncoder.h"
#include "ccm.io.IOutputStream.h"
#include "ccm.io.IOutputStreamWriter.h"
#include "ccm.io.charset.ICharset.h"
#include "ccm.io.charset.ICharsetEncoder.h"
#include "ccm.io.charset.IStreamEncoder.h"
#include <ccmautoptr.h>

using ccm::io::charset::ICharset;
using ccm::io::charset::ICharsetEncoder;
using ccm::io::charset::IStreamEncoder;
using ccm::io::charset::StreamEncoder;

namespace ccm {
namespace io {

class OutputStreamWriter
    : public Writer
    , public IOutputStreamWriter
{
public:
    CCM_INTERFACE_DECL();

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

#endif // __CCM_IO_OUTPUTSTREAMWRITER_H__
