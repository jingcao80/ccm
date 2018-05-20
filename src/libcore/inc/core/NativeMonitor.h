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

#ifndef __CCM_CORE_NATIVEMONITOR_H__
#define __CCM_CORE_NATIVEMONITOR_H__

#include "core/NativeMutex.h"
#include <ccmtypes.h>

namespace ccm {
namespace core {

class NativeThread;
class NativeObject;

class NativeMonitor
{
public:
    static ECode NotifyAll(
        /* [in] */ NativeThread* self,
        /* [in] */ NativeObject* obj);

private:
    static ECode DoNotify(
        /* [in] */ NativeThread* self,
        /* [in] */ NativeObject* obj,
        /* [in] */ Boolean notifyAll);

    ECode Notify(
        /* [in] */ NativeThread* self);

    ECode NotifyAll(
        /* [in] */ NativeThread* self);

private:
    NativeMutex mMonitorLock;

    // Which thread currently owns the lock?
    NativeThread* volatile mOwner;

    // Threads currently waiting on this monitor.
    NativeThread* mWaitSet;
};

inline ECode NativeMonitor::NotifyAll(
    /* [in] */ NativeThread* self,
    /* [in] */ NativeObject* obj)
{
    return DoNotify(self, obj, true);
}

}
}

#endif // __CCM_CORE_NATIVEMONITOR_H__
