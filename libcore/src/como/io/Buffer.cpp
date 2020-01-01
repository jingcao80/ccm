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

#include "como/io/Buffer.h"
#include <ccmlogger.h>

using como::core::E_INDEX_OUT_OF_BOUNDS_EXCEPTION;

namespace como {
namespace io {

COMO_INTERFACE_IMPL_1(Buffer, SyncObject, IBuffer);

ECode Buffer::Constructor(
    /* [in] */ Integer mark,
    /* [in] */ Integer pos,
    /* [in] */ Integer lim,
    /* [in] */ Integer cap,
    /* [in] */ Integer elementSizeShift)
{
    if (cap < 0) {
        Logger::E("Buffer", "Negative capacity: %d", cap);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mCapacity = cap;
    SetLimit(lim);
    SetPosition(pos);
    if (mark >= 0) {
        if (mark > pos) {
            Logger::E("Buffer", "mark > position: (%d > %d)", mark, pos);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        mMark = mark;
    }
    mElementSizeShift = elementSizeShift;
    return NOERROR;
}

ECode Buffer::GetCapacity(
    /* [out] */ Integer* capacity)
{
    VALIDATE_NOT_NULL(capacity);

    *capacity = mCapacity;
    return NOERROR;
}

ECode Buffer::GetPosition(
    /* [out] */ Integer* position)
{
    VALIDATE_NOT_NULL(position);

    *position = mPosition;
    return NOERROR;
}

ECode Buffer::SetPosition(
    /* [in] */ Integer newPosition)
{
    if ((newPosition > mLimit) || (newPosition < 0)) {
        Logger::E("Buffer", "Bad position %d/%d", newPosition, mLimit);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mPosition = newPosition;
    if (mMark > mPosition) {
        mMark = -1;
    }
    return NOERROR;
}

ECode Buffer::GetLimit(
    /* [out] */ Integer* limit)
{
    VALIDATE_NOT_NULL(limit);

    *limit = mLimit;
    return NOERROR;
}

ECode Buffer::SetLimit(
    /* [in] */ Integer newLimit)
{
    if ((newLimit > mCapacity) || (newLimit < 0)) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mLimit = newLimit;
    if (mPosition > mLimit) {
        mPosition = mLimit;
    }
    if (mMark > mLimit) {
        mMark = -1;
    }
    return NOERROR;
}

ECode Buffer::Mark()
{
    mMark = mPosition;
    return NOERROR;
}

ECode Buffer::Reset()
{
    if (mMark < 0) {
        return E_INVALID_MARK_EXCEPTION;
    }
    mPosition = mMark;
    return NOERROR;
}

ECode Buffer::Clear()
{
    mPosition = 0;
    mLimit = mCapacity;
    mMark = -1;
    return NOERROR;
}

ECode Buffer::Flip()
{
    mLimit = mPosition;
    mPosition = 0;
    mMark = -1;
    return NOERROR;
}

ECode Buffer::Rewind()
{
    mPosition = 0;
    mMark = -1;
    return NOERROR;
}

ECode Buffer::Remaining(
    /* [out] */ Integer* remaining)
{
    VALIDATE_NOT_NULL(remaining);

    *remaining = mLimit - mPosition;
    return NOERROR;
}

ECode Buffer::HasRemaining(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mPosition < mLimit;
    return NOERROR;
}

ECode Buffer::NextGetIndex(
    /* [out] */ Integer* value)
{
    if (mPosition >= mLimit) {
        return E_BUFFER_UNDERFLOW_EXCEPTION;
    }
    *value = mPosition++;
    return NOERROR;
}

ECode Buffer::NextGetIndex(
    /* [in] */ Integer nb,
    /* [out] */ Integer* value)
{
    if (mLimit - mPosition < nb) {
        return E_BUFFER_UNDERFLOW_EXCEPTION;
    }
    *value = mPosition;
    mPosition += nb;
    return NOERROR;
}

ECode Buffer::NextPutIndex(
    /* [out] */ Integer* value)
{
    if (mPosition >= mLimit) {
        return E_BUFFER_OVERFLOW_EXCEPTION;
    }
    *value = mPosition++;
    return NOERROR;
}

ECode Buffer::NextPutIndex(
    /* [in] */ Integer nb,
    /* [out] */ Integer* value)
{
    if (mLimit - mPosition < nb) {
        return E_BUFFER_OVERFLOW_EXCEPTION;
    }
    *value = mPosition;
    mPosition += nb;
    return NOERROR;
}

ECode Buffer::CheckIndex(
    /* [in] */ Integer i)
{
    if ((i < 0) || (i >= mLimit)) {
        Logger::E("Buffer", "index=%d out of bounds (limit=%d)", i, mLimit);
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    return NOERROR;
}

ECode Buffer::CheckIndex(
    /* [in] */ Integer i,
    /* [in] */ Integer nb)
{
    if ((i < 0) || (nb > mLimit - i)) {
        Logger::E("Buffer", "index=%d out of bounds (limit=%d, nb=%d)", i, mLimit, nb);
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    return NOERROR;
}

Integer Buffer::MarkValue()
{
    return mMark;
}

void Buffer::Truncate()
{
    mMark = -1;
    mPosition = 0;
    mLimit = 0;
    mCapacity = 0;
}

void Buffer::DiscardMark()
{
    mMark = -1;
}

ECode Buffer::CheckBounds(
    /* [in] */ Integer off,
    /* [in] */ Integer len,
    /* [in] */ Integer size)
{
    if ((off | len | (off + len) | (size - (off + len))) < 0) {
        Logger::E("Buffer", "off=%d, len=%d out of bounds (size=%d)", off, len, size);
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    return NOERROR;
}

}
}