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
#include "ccm/core/CoreUtils.h"
#include "ccm/io/Writer.h"

using ccm::core::AutoLock;
using ccm::core::CoreUtils;
using ccm::core::IID_IAppendable;

namespace ccm {
namespace io {

CCM_INTERFACE_IMPL_4(Writer, SyncObject, IWriter, IAppendable, ICloseable, IFlushable);

Writer::~Writer()
{
    if (mLock != nullptr && mLock != (ISynchronize*)this) {
        REFCOUNT_RELEASE(mLock);
    }
}

ECode Writer::Constructor()
{
    mLock = (ISynchronize*)this;
    return NOERROR;
}

ECode Writer::Constructor(
    /* [in] */ ISynchronize* lock)
{
    if (lock == nullptr) {
        return ccm::core::E_NULL_POINTER_EXCEPTION;
    }
    mLock = lock;
    if (mLock != (ISynchronize*)this) {
        REFCOUNT_ADD(mLock);
    }
    return NOERROR;
}

ECode Writer::Write(
    /* [in] */ Integer c)
{
    AutoLock lock(mLock);
    if (mWriteBuffer.IsNull()) {
        mWriteBuffer = Array<Char>(WRITE_BUFFER_SIZE);
    }
    mWriteBuffer[0] = (Char)c;
    return Write(mWriteBuffer, 0, 1);
}

ECode Writer::Write(
    /* [in] */ const Array<Char>& buffer)
{
    return Write(buffer, 0, buffer.GetLength());
}

ECode Writer::Write(
    /* [in] */ const String& str)
{
    return Write(str, 0, str.GetLength());
}

ECode Writer::Write(
    /* [in] */ const String& str,
    /* [in] */ Integer off,
    /* [in] */ Integer len)
{
    AutoLock lock(mLock);
    Array<Char> buffer;
    if (len < WRITE_BUFFER_SIZE) {
        if (mWriteBuffer.IsNull()) {
            mWriteBuffer = Array<Char>(WRITE_BUFFER_SIZE);
        }
        buffer = mWriteBuffer;
    }
    else {
        buffer = Array<Char>(len);
    }
    str.GetChars(off, off + len, buffer, 0);
    return Write(buffer, 0, len);
}

ECode Writer::Append(
    /* [in] */ ICharSequence* csq)
{
    if (csq == nullptr) {
        return Write(String("null"));
    }
    else {
        return Write(CoreUtils::Unbox(csq));
    }
}

ECode Writer::Append(
    /* [in] */ ICharSequence* csq,
    /* [in] */ Integer start,
    /* [in] */ Integer end)
{
    AutoPtr<ICharSequence> cs = csq;
    if (cs == nullptr) {
        cs = CoreUtils::Box(String("null"));
    }
    AutoPtr<ICharSequence> subcs;
    cs->SubSequence(start, end, (ICharSequence**)&subcs);
    return Write(CoreUtils::Unbox(subcs));
}

ECode Writer::Append(
    /* [in] */ Char c)
{
    return Write(c);
}

}
}
