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

#include "ccm/core/CoreUtils.h"
#include "ccm/io/CBufferedWriter.h"
#include "ccm/io/CFileOutputStream.h"
#include "ccm/io/COutputStreamWriter.h"
#include "ccm/io/PrintWriter.h"
#include "ccm/io/charset/Charset.h"
#include "ccm/security/AccessController.h"
#include "ccm/security/action/CGetPropertyAction.h"
#include "ccm.security.IPrivilegedAction.h"
#include <ccmlogger.h>

using ccm::core::CoreUtils;
using ccm::io::charset::Charset;
using ccm::security::AccessController;
using ccm::security::IPrivilegedAction;
using ccm::security::IID_IPrivilegedAction;
using ccm::security::action::CGetPropertyAction;

namespace ccm {
namespace io {

CCM_INTERFACE_IMPL_1(PrintWriter, Writer, IPrintWriter);

ECode PrintWriter::ToCharset(
    /* [in] */ const String& csn,
    /* [out] */ ICharset** cs)
{
    VALIDATE_NOT_NULL(cs);

    if (csn.IsNull()) {
        Logger::E("Charset", "charsetName is null");
        return ccm::core::E_NULL_POINTER_EXCEPTION;
    }
    ECode ec = Charset::ForName(csn, cs);
    return SUCCEEDED(ec) ? NOERROR : E_UNSUPPORTED_ENCODING_EXCEPTION;
}

ECode PrintWriter::Constructor(
    /* [in] */ IWriter* outstream)
{
    return Constructor(outstream, false);
}

ECode PrintWriter::Constructor(
    /* [in] */ IWriter* outstream,
    /* [in] */ Boolean autoFlush)
{
    FAIL_RETURN(Writer::Constructor(ISynchronize::Probe(outstream)));
    mOut = outstream;
    mAutoFlush = autoFlush;
    AutoPtr<IPrivilegedAction> lsAction;
    CGetPropertyAction::New(String("line.separator"),
            IID_IPrivilegedAction, (IInterface**)&lsAction);
    AutoPtr<IInterface> lsRet;
    FAIL_RETURN(AccessController::DoPrivileged(lsAction, (IInterface**)&lsRet));
    mLineSeparator = CoreUtils::Unbox(ICharSequence::Probe(lsRet));
    return NOERROR;
}

ECode PrintWriter::Constructor(
    /* [in] */ IOutputStream* outstream)
{
    return Constructor(outstream, false);
}

ECode PrintWriter::Constructor(
    /* [in] */ IOutputStream* outstream,
    /* [in] */ Boolean autoFlush)
{
    AutoPtr<IWriter> osw, bw;
    COutputStreamWriter::New(outstream, IID_IWriter, (IInterface**)&osw);
    CBufferedWriter::New(osw, IID_IWriter, (IInterface**)&bw);
    FAIL_RETURN(Constructor(bw, autoFlush));

    // save print stream for error propagation
    if (IPrintStream::Probe(outstream) != nullptr) {
        mPsOut = IPrintStream::Probe(outstream);
    }
    return NOERROR;
}

ECode PrintWriter::Constructor(
    /* [in] */ const String& fileName)
{
    AutoPtr<IOutputStream> fos;
    CFileOutputStream::New(fileName, IID_IOutputStream, (IInterface**)&fos);
    AutoPtr<IWriter> osw, bw;
    COutputStreamWriter::New(fos, IID_IWriter, (IInterface**)&osw);
    CBufferedWriter::New(osw, IID_IWriter, (IInterface**)&bw);
    return Constructor(bw, false);
}

ECode PrintWriter::Flush()
{
    return NOERROR;
}

ECode PrintWriter::Close()
{
    return NOERROR;
}

ECode PrintWriter::Write(
    /* [in] */ const Array<Char>& buffer,
    /* [in] */ Integer off,
    /* [in] */ Integer len)
{
    return NOERROR;
}

ECode PrintWriter::Println(
    /* [in] */ const String& s)
{
    return NOERROR;
}

}
}
