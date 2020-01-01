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

#include "como/core/AutoLock.h"
#include "como/core/System.h"
#include "como/io/BufferedWriter.h"
#include <ccmlogger.h>

using como::core::AutoLock;
using como::core::System;

namespace como {
namespace io {

COMO_INTERFACE_IMPL_1(BufferedWriter, Writer, IBufferedWriter);

ECode BufferedWriter::Constructor(
    /* [in] */ IWriter* out)
{
    return Constructor(out, sDefaultCharBufferSize);
}

ECode BufferedWriter::Constructor(
    /* [in] */ IWriter* out,
    /* [in] */ Integer sz)
{
    Writer::Constructor(ISynchronize::Probe(out));
    if (sz <= 0) {
        Logger::E("BufferedWriter", "Buffer size <= 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mOut = out;
    mCb = Array<Char>(sz);
    mNChars = sz;
    mNextChar = 0;

    System::GetProperty(String("line.separator"), &mLineSeparator);
    return NOERROR;
}

ECode BufferedWriter::EnsureOpen()
{
    if (mOut == nullptr) {
        Logger::E("BufferedWriter", "Stream closed");
        return E_IO_EXCEPTION;
    }
    return NOERROR;
}

ECode BufferedWriter::FlushBuffer()
{
    AutoLock lock(mLock);
    FAIL_RETURN(EnsureOpen());
    if (mNextChar == 0) return NOERROR;
    FAIL_RETURN(mOut->Write(mCb, 0, mNextChar));
    mNextChar = 0;
    return NOERROR;
}

ECode BufferedWriter::Write(
    /* [in] */ Integer c)
{
    AutoLock lock(mLock);
    FAIL_RETURN(EnsureOpen());
    if (mNextChar >= mNChars) {
        FAIL_RETURN(FlushBuffer());
    }
    mCb[mNextChar++] = (Char)c;
    return NOERROR;
}

ECode BufferedWriter::Write(
    /* [in] */ const Array<Char>& buffer,
    /* [in] */ Integer off,
    /* [in] */ Integer len)
{
    AutoLock lock(mLock);
    FAIL_RETURN(EnsureOpen());
    if ((off < 0) || (off > buffer.GetLength()) || (len < 0) ||
        ((off + len) > buffer.GetLength()) || ((off + len) < 0)) {
        return como::core::E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    else if (len == 0) {
        return NOERROR;
    }

    if (len >= mNChars) {
        /* If the request length exceeds the size of the output buffer,
           flush the buffer and then write the data directly.  In this
           way buffered streams will cascade harmlessly. */
        FAIL_RETURN(FlushBuffer());
        return mOut->Write(buffer, off, len);
    }

    Integer b = off, t = off + len;
    while (b < t) {
        Integer d = MIN(mNChars - mNextChar, t - b);
        mCb.Copy(mNextChar, buffer, b, d);
        b += d;
        mNextChar += d;
        if (mNextChar >= mNChars) {
            FAIL_RETURN(FlushBuffer());
        }
    }
    return NOERROR;
}

ECode BufferedWriter::Write(
    /* [in] */ const String& str,
    /* [in] */ Integer off,
    /* [in] */ Integer len)
{
    AutoLock lock(mLock);
    FAIL_RETURN(EnsureOpen());

    Integer b = off, t = off + len;
    while (b < t) {
        Integer d = MIN(mNChars - mNextChar, t - b);
        str.GetChars(b, b + d, mCb, mNextChar);
        b += d;
        mNextChar += d;
        if (mNextChar >= mNChars) {
            FAIL_RETURN(FlushBuffer());
        }
    }
    return NOERROR;
}

ECode BufferedWriter::NewLine()
{
    return Write(mLineSeparator);
}

ECode BufferedWriter::Flush()
{
    AutoLock lock(mLock);
    FAIL_RETURN(FlushBuffer());
    return mOut->Flush();
}

ECode BufferedWriter::Close()
{
    AutoLock lock(mLock);
    if (mOut == nullptr) {
        return NOERROR;
    }
    FlushBuffer();
    mOut = nullptr;
    mCb.Clear();
    return NOERROR;
}

}
}
