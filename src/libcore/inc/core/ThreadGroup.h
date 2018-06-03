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

#ifndef __CCM_CORE_THREADGROUP_H__
#define __CCM_CORE_THREADGROUP_H__

#include "core/SyncObject.h"
#include "ccm.core.IThread.h"
#include "ccm.core.IThreadGroup.h"
#include <ccmarray.h>
#include <ccmautoptr.h>

namespace ccm {
namespace core {

class COM_PUBLIC ThreadGroup
    : public SyncObject
    , public IThreadGroup
{
public:
    static AutoPtr<IThreadGroup> GetSystemThreadGroup();

    static AutoPtr<IThreadGroup> GetMainThreadGroup();

    CCM_INTERFACE_DECL();

    ECode constructor();

    ECode constructor(
        /* [in] */ const String& name);

    ECode constructor(
        /* [in] */ IThreadGroup* parent,
        /* [in] */ const String& name);

    ECode GetName(
        /* [out] */ String* name);

    ECode GetMaxPriority(
        /* [out] */ Integer* priority);

    ECode ActiveCount(
        /* [out] */ Integer* count);

    ECode Enumerate(
        /* [out] */ Array<IThread*>& list,
        /* [out] */ Integer* count);

    ECode AddUnstarted();

    ECode Add(
        /* [in] */ IThread* t);

    ECode ThreadStartFailed(
        /* [in] */ IThread* t);

    ECode ThreadTerminated(
        /* [in] */ IThread* t);

    static ThreadGroup* From(
        /* [in] */ IThreadGroup* group);

private:
    ECode Add(
        /* [in] */ IThreadGroup* g);

public:
    String mName;

    Integer mMaxPriority;

    Boolean mDestroyed = false;

    Integer mNUnstartedThreads = 0;

    Integer mNthreads = 0;

    Array<IThread*> mThreads;

private:
    IThreadGroup* mParent = nullptr;
};

inline ThreadGroup* ThreadGroup::From(
    /* [in] */ IThreadGroup* group)
{
    return (ThreadGroup*)group;
}

}
}

#endif // __CCM_CORE_THREADGROUP_H__
