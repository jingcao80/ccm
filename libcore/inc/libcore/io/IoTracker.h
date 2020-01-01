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

#ifndef __LIBCORE_IO_IOTRACKER_H__
#define __LIBCORE_IO_IOTRACKER_H__

#include "como/core/SyncObject.h"

using como::core::SyncObject;

namespace libcore {
namespace io {

class IoTracker final
    : public SyncObject
{
public:
    enum class Mode
    {
        READ,
        WRITE
    };

public:
    ECode TrackIo(
        /* [in] */ Integer byteCount);

    ECode TrackIo(
        /* [in] */ Integer byteCount,
        /* [in] */ Mode mode);

    void Reset();

private:
    Integer mOpCount = 0;
    Integer mTotalByteCount = 0;
    Boolean mIsOpen = true;
    Mode mMode = Mode::READ;
};

}
}

#endif // __LIBCORE_IO_IOTRACKER_H__
