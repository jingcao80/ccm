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

#ifndef __COMO_IO_BUFFER_H__
#define __COMO_IO_BUFFER_H__

#include "como/core/SyncObject.h"
#include "como.io.IBuffer.h"

using como::core::SyncObject;

namespace como {
namespace io {

class Buffer
    : public SyncObject
    , public IBuffer
{
public:
    COMO_INTERFACE_DECL();

    ECode Constructor(
        /* [in] */ Integer mark,
        /* [in] */ Integer pos,
        /* [in] */ Integer lim,
        /* [in] */ Integer cap,
        /* [in] */ Integer elementSizeShift);

    ECode GetCapacity(
        /* [out] */ Integer* capacity) override;

    ECode GetPosition(
        /* [out] */ Integer* position) override;

    ECode SetPosition(
        /* [in] */ Integer newPosition) override;

    ECode GetLimit(
        /* [out] */ Integer* limit) override;

    ECode SetLimit(
        /* [in] */ Integer newLimit) override;

    ECode Mark() override;

    ECode Reset() override;

    ECode Clear() override;

    ECode Flip() override;

    ECode Rewind() override;

    ECode Remaining(
        /* [out] */ Integer* remaining) override;

    ECode HasRemaining(
        /* [out] */ Boolean* result) override;

    ECode NextGetIndex(
        /* [out] */ Integer* value);

    ECode NextGetIndex(
        /* [in] */ Integer nb,
        /* [out] */ Integer* value);

    ECode NextPutIndex(
        /* [out] */ Integer* value);

    ECode NextPutIndex(
        /* [in] */ Integer nb,
        /* [out] */ Integer* value);

    ECode CheckIndex(
        /* [in] */ Integer i);

    ECode CheckIndex(
        /* [in] */ Integer i,
        /* [in] */ Integer nb);

    Integer MarkValue();

    void Truncate();

    void DiscardMark();

    static ECode CheckBounds(
        /* [in] */ Integer off,
        /* [in] */ Integer len,
        /* [in] */ Integer size);

public:
    Integer mPosition = 0;
    HANDLE mAddress = 0;
    Integer mElementSizeShift = 0;

private:
    Integer mMark = -1;
    Integer mLimit = 0;
    Integer mCapacity = 0;
};

}
}

#endif // __COMO_IO_BUFFER_H__
