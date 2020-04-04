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
#include "como/core/StringUtils.h"
#include "como/io/ByteArrayOutputStream.h"
#include "como/util/Arrays.h"
#include <comolog.h>

using como::core::AutoLock;
using como::core::E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
using como::core::StringUtils;
using como::util::Arrays;

namespace como {
namespace io {

COMO_INTERFACE_IMPL_1(ByteArrayOutputStream, OutputStream, IByteArrayOutputStream);

ECode ByteArrayOutputStream::Constructor()
{
    return Constructor(32);
}

ECode ByteArrayOutputStream::Constructor(
    /* [in] */ Integer size)
{
    if (size < 0) {
        Logger::E("ByteArrayOutputStream", "Negative initial size: %d", size);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mBuf = Array<Byte>::Allocate(size);
    return NOERROR;
}

void ByteArrayOutputStream::EnsureCapacity(
    /* [in] */ Integer minCapacity)
{
    if (minCapacity - mBuf.GetLength() > 0) {
        Grow(minCapacity);
    }
}

void ByteArrayOutputStream::Grow(
    /* [in] */ Integer minCapacity)
{
    Integer oldCapacity = mBuf.GetLength();
    Integer newCapacity = oldCapacity << 1;
    if (newCapacity - minCapacity < 0) {
        newCapacity = minCapacity;
    }
    if (newCapacity - MAX_ARRAY_SIZE > 0) {
        newCapacity = HugeCapacity(minCapacity);
    }
    Array<Byte> newBuf;
    Arrays::CopyOf(mBuf, newCapacity, &newBuf);
    mBuf = newBuf;
}

Integer ByteArrayOutputStream::HugeCapacity(
    /* [in] */ Integer minCapacity)
{
    return (minCapacity > MAX_ARRAY_SIZE) ?
            IInteger::MAX_VALUE : MAX_ARRAY_SIZE;
}

ECode ByteArrayOutputStream::Write(
    /* [in] */ Integer byte)
{
    AutoLock lock(this);
    EnsureCapacity(mCount + 1);
    mBuf[mCount] = byte;
    mCount += 1;
    return NOERROR;
}

ECode ByteArrayOutputStream::Write(
    /* [in] */ const Array<Byte>& buffer,
    /* [in] */ Integer offset,
    /* [in] */ Integer size)
{
    if ((offset < 0) || (offset > buffer.GetLength()) || (size < 0) ||
            ((offset + size) - buffer.GetLength() > 0)) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    AutoLock lock(this);
    EnsureCapacity(mCount + size);
    mBuf.Copy(mCount, buffer, offset, size);
    mCount += size;
    return NOERROR;
}

ECode ByteArrayOutputStream::WriteTo(
    /* [in] */ IOutputStream* outs)
{
    AutoLock lock(this);
    return outs->Write(mBuf, 0, mCount);
}

ECode ByteArrayOutputStream::Reset()
{
    AutoLock lock(this);
    mCount = 0;
    return NOERROR;
}

ECode ByteArrayOutputStream::ToByteArray(
    /* [out, callee] */ Array<Byte>* array)
{
    AutoLock lock(this);
    return Arrays::CopyOf(mBuf, mCount, array);
}

ECode ByteArrayOutputStream::GetSize(
    /* [out] */ Integer& size)
{
    AutoLock lock(this);
    size = mCount;
    return NOERROR;
}

ECode ByteArrayOutputStream::ToString(
    /* [out] */ String& desc)
{
    AutoLock lock(this);
    desc = String(mBuf, 0, mCount);
    return NOERROR;
}

ECode ByteArrayOutputStream::ToString(
    /* [in] */ const String& charsetName,
    /* [out] */ String& desc)
{
    AutoLock lock(this);
    return StringUtils::ToString(String(mBuf, 0, mCount), charsetName, &desc);
}

ECode ByteArrayOutputStream::Close()
{
    return NOERROR;
}

}
}
