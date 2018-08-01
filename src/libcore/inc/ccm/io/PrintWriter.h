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

#ifndef __CCM_IO_PRINTWRITER_H__
#define __CCM_IO_PRINTWRITER_H__

#include "ccm/io/Writer.h"
#include "ccm.io.IPrintStream.h"
#include "ccm.io.IPrintWriter.h"
#include "ccm.io.charset.ICharset.h"
#include <ccmautoptr.h>

using ccm::io::charset::ICharset;

namespace ccm {
namespace io {

class PrintWriter
    : public Writer
    , public IPrintWriter
{
public:
    CCM_INTERFACE_DECL();

    ECode Constructor(
        /* [in] */ IWriter* outstream);

    ECode Constructor(
        /* [in] */ IWriter* outstream,
        /* [in] */ Boolean autoFlush);

    ECode Constructor(
        /* [in] */ IOutputStream* outstream);

    ECode Constructor(
        /* [in] */ IOutputStream* outstream,
        /* [in] */ Boolean autoFlush);

    ECode Constructor(
        /* [in] */ const String& fileName);

    ECode Flush() override;

    ECode Close() override;

    ECode Write(
        /* [in] */ const Array<Char>& buffer,
        /* [in] */ Integer off,
        /* [in] */ Integer len) override;

    ECode Println(
        /* [in] */ const String& s) override;

private:
    static ECode ToCharset(
        /* [in] */ const String& csn,
        /* [out] */ ICharset** cs);

protected:
    AutoPtr<IWriter> mOut;

private:
    Boolean mAutoFlush;
    AutoPtr<IPrintStream> mPsOut;

    /**
     * Line separator string.  This is the value of the line.separator
     * property at the moment that the stream was created.
     */
    String mLineSeparator;
};

}
}

#endif // __CCM_IO_PRINTWRITER_H__
