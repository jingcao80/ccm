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

#ifndef __CCM_CORE_NATIVEOBJECT_H__
#define __CCM_CORE_NATIVEOBJECT_H__

#include "core/NativeLockWord.h"
#include "core/NativeMonitor.h"
#include "core/NativeThread.h"

namespace ccm {
namespace core {

class NativeObject
{
public:
    NativeLockWord GetLockWord(
        /* [in] */ Boolean asVolatile);

    void SetLockWord(
        /* [in] */ NativeLockWord newVal,
        /* [in] */ Boolean asVolatile);

    Boolean CasLockWordWeakAcquire(
        /* [in] */ NativeLockWord oldVal,
        /* [in] */ NativeLockWord newVal);

    Boolean CasLockWordWeakRelease(
        /* [in] */ NativeLockWord oldVal,
        /* [in] */ NativeLockWord newVal);

    ECode MonitorEnter(
        /* [in] */ NativeThread* self);

    ECode MonitorExit(
        /* [in] */ NativeThread* self);

    ECode Notify(
        /* [in] */ NativeThread* self);

    ECode NotifyAll(
        /* [in] */ NativeThread* self);

    // Returns a human-readable form of the name of the *class* of the given object.
    // So given an instance of java.lang.String, the output would
    // be "java.lang.String". Given an array of int, the output would be "int[]".
    // Given String.class, the output would be "java.lang.Class<java.lang.String>".
    static String PrettyTypeOf(
        /* [in] */ NativeObject* obj);

    String PrettyTypeOf();

private:
    // Monitor and hash code information.
    Atomic<uint32_t> mMonitor;

    HANDLE mCcmObject;
};

inline Boolean NativeObject::CasLockWordWeakAcquire(
    /* [in] */ NativeLockWord oldVal,
    /* [in] */ NativeLockWord newVal)
{
    return mMonitor.CompareExchangeWeakAcquire(
            oldVal.GetValue(), newVal.GetValue());
}

inline Boolean NativeObject::CasLockWordWeakRelease(
    /* [in] */ NativeLockWord oldVal,
    /* [in] */ NativeLockWord newVal)
{
    return mMonitor.CompareExchangeWeakRelease(
            oldVal.GetValue(), newVal.GetValue());
}

inline ECode NativeObject::MonitorEnter(
    /* [in] */ NativeThread* self)
{
    return NativeMonitor::MonitorEnter(self, this, /*trylock*/true);
}

inline ECode NativeObject::MonitorExit(
    /* [in] */ NativeThread* self)
{
    return NativeMonitor::MonitorExit(self, this);
}

inline ECode NativeObject::Notify(
    /* [in] */ NativeThread* self)
{
    return NativeMonitor::Notify(self, this);
}

inline ECode NativeObject::NotifyAll(
    /* [in] */ NativeThread* self)
{
    return NativeMonitor::NotifyAll(self, this);
}

}
}

#endif // __CCM_CORE_NATIVEOBJECT_H__
