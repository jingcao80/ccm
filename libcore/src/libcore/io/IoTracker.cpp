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

#include "ccmrt/system/BlockGuard.h"
#include "libcore/io/IoTracker.h"
#include "ccmrt.system.IBlockGuardPolicy.h"

using ccmrt::system::BlockGuard;
using ccmrt::system::IBlockGuardPolicy;

namespace libcore {
namespace io {

ECode IoTracker::TrackIo(
    /* [in] */ Integer byteCount)
{
    ++mOpCount;
    mTotalByteCount += byteCount;
    if (mIsOpen && mOpCount > 10 && mTotalByteCount < 10 * 512) {
        AutoPtr<IBlockGuardPolicy> policy;
        BlockGuard::GetThreadPolicy(&policy);
        FAIL_RETURN(policy->OnUnbufferedIO());
        mIsOpen = false;
    }
    return NOERROR;
}

ECode IoTracker::TrackIo(
    /* [in] */ Integer byteCount,
    /* [in] */ Mode mode)
{
    if (mMode != mode) {
        Reset();
        mMode = mode;
    }
    return TrackIo(byteCount);
}

void IoTracker::Reset()
{
    mOpCount = 0;
    mTotalByteCount = 0;
}

}
}
