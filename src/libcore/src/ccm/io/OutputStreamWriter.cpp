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

#include "ccm/core/AutoLock.h"
#include "ccm/io/OutputStreamWriter.h"
#include "ccm.core.ISynchronize.h"

using ccm::core::AutoLock;
using ccm::core::ISynchronize;

namespace ccm {
namespace io {

CCM_INTERFACE_IMPL_1(OutputStreamWriter, Writer, IOutputStreamWriter);

ECode OutputStreamWriter::Constructor(
    /* [in] */ IOutputStream* out,
    /* [in] */ const String& charsetName)
{
    Writer::Constructor(ISynchronize::Probe(out));
    if (charsetName.IsNull()) {
        return ccm::core::E_NULL_POINTER_EXCEPTION;
    }
    return StreamEncoder::ForOutputStreamWriter(out, this, charsetName, &mSe);
}

ECode OutputStreamWriter::Constructor(
    /* [in] */ IOutputStream* out)
{
    Writer::Constructor(ISynchronize::Probe(out));
    return StreamEncoder::ForOutputStreamWriter(out, this, String(), &mSe);
}

ECode OutputStreamWriter::Constructor(
    /* [in] */ IOutputStream* out,
    /* [in] */ ICharset* cs)
{
    Writer::Constructor(ISynchronize::Probe(out));
    if (cs == nullptr) {
        return ccm::core::E_NULL_POINTER_EXCEPTION;
    }
    return StreamEncoder::ForOutputStreamWriter(out, this, cs, &mSe);
}

ECode OutputStreamWriter::Constructor(
    /* [in] */ IOutputStream* out,
    /* [in] */ ICharsetEncoder* enc)
{
    Writer::Constructor(ISynchronize::Probe(out));
    if (enc == nullptr) {
        return ccm::core::E_NULL_POINTER_EXCEPTION;
    }
    return StreamEncoder::ForOutputStreamWriter(out, this, enc, &mSe);
}

ECode OutputStreamWriter::GetEncoding(
    /* [out] */ String* name)
{
    return mSe->GetEncoding(name);
}

ECode OutputStreamWriter::FlushBuffer()
{
    return mSe->FlushBuffer();
}

ECode OutputStreamWriter::Write(
    /* [in] */ Integer c)
{
    return IWriter::Probe(mSe)->Write(c);
}

ECode OutputStreamWriter::Write(
    /* [in] */ const Array<Char>& buffer,
    /* [in] */ Integer off,
    /* [in] */ Integer len)
{
    return IWriter::Probe(mSe)->Write(buffer, off, len);
}

ECode OutputStreamWriter::Write(
    /* [in] */ const String& str,
    /* [in] */ Integer off,
    /* [in] */ Integer len)
{
    return IWriter::Probe(mSe)->Write(str, off, len);
}

ECode OutputStreamWriter::Flush()
{
    return IWriter::Probe(mSe)->Flush();
}

ECode OutputStreamWriter::Close()
{
    return IWriter::Probe(mSe)->Close();
}

}
}
