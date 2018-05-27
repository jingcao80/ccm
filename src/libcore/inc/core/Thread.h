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

#ifndef __CCM_CORE_THREAD_H__
#define __CCM_CORE_THREAD_H__

#include "core/Runnable.h"
#include "ccm.core.IThread.h"

namespace ccm {
namespace core {

class COM_PUBLIC Thread
    : public Runnable
    , public IThread
{
public:
    CCM_INTERFACE_DECL();

private:
    ECode NativeCreate(
        /* [in] */ Thread* t,
        /* [in] */ Long stackSize,
        /* [in] */ Boolean daemon);

private:
    friend class NativeThread;

    HANDLE mNative = 0;

    /**
     * The synchronization object responsible for this thread's join/sleep/park operations.
     */
    SyncObject* mLock;

    String mName;

    Integer mPriority;
};

}
}

#endif // __CCM_CORE_THREAD_H__
