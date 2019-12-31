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

#ifndef __CCM_IO_PRINTWRITER_H__
#define __CCM_IO_PRINTWRITER_H__

#include "ccm/io/Writer.h"
#include "ccm.io.IFile.h"
#include "ccm.io.IPrintStream.h"
#include "ccm.io.IPrintWriter.h"
#include "ccm.io.charset.ICharset.h"
#include "ccm.util.IFormatter.h"
#include "ccm.util.ILocale.h"
#include <ccmautoptr.h>

using ccm::io::charset::ICharset;
using ccm::util::IFormatter;
using ccm::util::ILocale;

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

    ECode Constructor(
        /* [in] */ const String& fileName,
        /* [in] */ const String& csn);

    ECode Constructor(
        /* [in] */ IFile* file);

    ECode Constructor(
        /* [in] */ IFile* file,
        /* [in] */ const String& csn);

    ECode Flush() override;

    ECode Close() override;

    ECode CheckError(
        /* [out] */ Boolean* hasErrors) override;

    ECode Write(
        /* [in] */ Integer c) override;

    ECode Write(
        /* [in] */ const Array<Char>& buffer,
        /* [in] */ Integer off,
        /* [in] */ Integer len) override;

    ECode Write(
        /* [in] */ const Array<Char>& buffer) override;

    ECode Write(
        /* [in] */ const String& str,
        /* [in] */ Integer off,
        /* [in] */ Integer len) override;

    ECode Print(
        /* [in] */ Boolean b) override;

    ECode PrintChar(
        /* [in] */ Char c) override;

    ECode Print(
        /* [in] */ Integer i) override;

    ECode Print(
        /* [in] */ Long l) override;

    ECode Print(
        /* [in] */ Float f) override;

    ECode Print(
        /* [in] */ Double d) override;

    ECode Print(
        /* [in] */ const Array<Char>& s) override;

    ECode Print(
        /* [in] */ const String& s) override;

    ECode Print(
        /* [in] */ IInterface* obj) override;

    ECode Println() override;

    ECode Println(
        /* [in] */ Boolean b) override;

    ECode PrintCharln(
        /* [in] */ Char c) override;

    ECode Println(
        /* [in] */ Integer i) override;

    ECode Println(
        /* [in] */ Long l) override;

    ECode Println(
        /* [in] */ Float f) override;

    ECode Println(
        /* [in] */ Double d) override;

    ECode Println(
        /* [in] */ const Array<Char>& s) override;

    ECode Println(
        /* [in] */ const String& s) override;

    ECode Println(
        /* [in] */ IInterface* obj) override;

    ECode Printf(
        /* [in] */ const String& format,
        /* [in] */ const Array<IInterface*>* args) override;

    ECode Printf(
        /* [in] */ ILocale* l,
        /* [in] */ const String& format,
        /* [in] */ const Array<IInterface*>* args) override;

    ECode Format(
        /* [in] */ const String& format,
        /* [in] */ const Array<IInterface*>* args) override;

    ECode Format(
        /* [in] */ ILocale* l,
        /* [in] */ const String& format,
        /* [in] */ const Array<IInterface*>* args) override;

    using Writer::Write;

protected:
    virtual void SetError();

    virtual void ClearError();

private:
    static ECode ToCharset(
        /* [in] */ const String& csn,
        /* [out] */ ICharset** cs);

    ECode Constructor(
        /* [in] */ ICharset* charset,
        /* [in] */ IFile* file);

    ECode EnsureOpen();

    void NewLine();

protected:
    AutoPtr<IWriter> mOut;

private:
    Boolean mAutoFlush;
    Boolean mTrouble = false;
    AutoPtr<IFormatter> mFormatter;
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
