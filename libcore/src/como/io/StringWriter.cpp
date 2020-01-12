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

#include "como/core/CStringBuffer.h"
#include "como/io/StringWriter.h"
#include <comolog.h>

using como::core::CStringBuffer;
using como::core::IID_IStringBuffer;

namespace como {
namespace io {

COMO_INTERFACE_IMPL_1(StringWriter, Writer, IStringWriter);

ECode StringWriter::Constructor()
{
    CStringBuffer::New(IID_IStringBuffer, (IInterface**)&mBuf);
    mLock = ISynchronize::Probe(mBuf);
    REFCOUNT_ADD(mLock);
    return NOERROR;
}

ECode StringWriter::Constructor(
    /* [in] */ Integer initialSize)
{
    if (initialSize < 0) {
        Logger::E("StringWriter", "Negative buffer size");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    CStringBuffer::New(IID_IStringBuffer, (IInterface**)&mBuf);
    mLock = ISynchronize::Probe(mBuf);
    REFCOUNT_ADD(mLock);
    return NOERROR;
}

ECode StringWriter::Write(
    /* [in] */ Integer c)
{
    return mBuf->Append((Char)c);
}

ECode StringWriter::Write(
    /* [in] */ const Array<Char>& buffer,
    /* [in] */ Integer off,
    /* [in] */ Integer len)
{
    if ((off < 0) || (off > buffer.GetLength()) || (len < 0) ||
        ((off + len) > buffer.GetLength()) || ((off + len) < 0)) {
        return como::core::E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    else if (len == 0) {
        return NOERROR;
    }
    return mBuf->Append(buffer, off, len);
}

ECode StringWriter::Write(
    /* [in] */ const String& str)
{
    return mBuf->Append(str);
}

ECode StringWriter::Write(
    /* [in] */ const String& str,
    /* [in] */ Integer off,
    /* [in] */ Integer len)
{
    return mBuf->Append(str.Substring(off, off + len));
}

ECode StringWriter::ToString(
    /* [out] */ String& str)
{
    return mBuf->ToString(str);
}

ECode StringWriter::GetBuffer(
    /* [out] */ IStringBuffer** buf)
{
    VALIDATE_NOT_NULL(buf);

    *buf = mBuf;
    REFCOUNT_ADD(*buf);
    return NOERROR;
}

ECode StringWriter::Flush()
{
    return NOERROR;
}

ECode StringWriter::Close()
{
    return NOERROR;
}

}
}
