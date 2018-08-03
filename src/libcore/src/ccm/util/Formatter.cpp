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

#include "ccm/core/CStringBuilder.h"
#include "ccm/io/CBufferedWriter.h"
#include "ccm/io/CFileOutputStream.h"
#include "ccm/io/COutputStreamWriter.h"
#include "ccm/io/charset/Charset.h"
#include "ccm/util/Formatter.h"
#include "ccm.io.IOutputStream.h"
#include "ccm.io.IWriter.h"

using ccm::core::CStringBuilder;
using ccm::core::IID_IAppendable;
using ccm::core::IID_IAutoCloseable;
using ccm::io::CBufferedWriter;
using ccm::io::CFileOutputStream;
using ccm::io::COutputStreamWriter;
using ccm::io::IID_ICloseable;
using ccm::io::IID_IFlushable;
using ccm::io::IID_IOutputStream;
using ccm::io::IID_IWriter;
using ccm::io::IOutputStream;
using ccm::io::IWriter;
using ccm::io::charset::Charset;

namespace ccm {
namespace util {

CCM_INTERFACE_IMPL_4(Formatter, SyncObject, IFormatter, ICloseable, IFlushable, IAutoCloseable);

ECode Formatter::ToCharset(
    /* [in] */ const String& csn,
    /* [out] */ ICharset** cs)
{
    if (csn.IsNull()) {
        Logger::E("Formatter", "charsetName is null");
        return ccm::core::E_NULL_POINTER_EXCEPTION;
    }
    ECode ec = Charset::ForName(csn, cs);
    return SUCCEEDED(ec) ? NOERROR : ccm::io::E_UNSUPPORTED_ENCODING_EXCEPTION;
}

AutoPtr<IAppendable> Formatter::NonNullAppendable(
    /* [in] */ IAppendable* a)
{
    if (a == nullptr) {
        AutoPtr<IAppendable> sb;
        CStringBuilder::New(IID_IAppendable, (IInterface**)&sb);
        return sb;
    }
    return a;
}

ECode Formatter::Constructor(
    /* [in] */ ILocale* l,
    /* [in] */ IAppendable* a)
{
    mA = a;
    mL = l;
    mZero = GetZero(l);
    return NOERROR;
}

ECode Formatter::Constructor(
    /* [in] */ ICharset* charset,
    /* [in] */ ILocale* l,
    /* [in] */ IFile* file)
{
    AutoPtr<IOutputStream> fos;
    CFileOutputStream::New(file, IID_IOutputStream, (IInterface**)&fos);
    AutoPtr<IWriter> osw;
    COutputStreamWriter::New(fos, charset, IID_IWriter, (IInterface**)&osw);
    AutoPtr<IAppendable> a;
    CBufferedWriter::New(osw, IID_IAppendable, (IInterface**)&a);
    return Constructor(l, a);
}

ECode Formatter::Constructor()
{
    return NOERROR;
}

}
}
