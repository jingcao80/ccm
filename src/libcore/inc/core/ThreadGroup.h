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
#include "ccm.io.IPrintStream.h"
#include <ccmarray.h>
#include <ccmautoptr.h>

using ccm::io::IPrintStream;

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
        /* [out] */ String* name) override;

    ECode GetParent(
        /* [out] */ IThreadGroup** parent) override;

    ECode GetMaxPriority(
        /* [out] */ Integer* priority) override;

    ECode IsDaemon(
        /* [out] */ Boolean* daemon) override;

    ECode IsDestroyed(
        /* [out] */ Boolean* destroyed) override;

    ECode SetDaemon(
        /* [in] */ Boolean daemon) override;

    ECode SetMaxPriority(
        /* [in] */ Integer pri) override;

    ECode ParentOf(
        /* [in] */ IThreadGroup* g,
        /* [out] */ Boolean* result) override;

    ECode CheckAccess() override;

    ECode ActiveCount(
        /* [out] */ Integer* count) override;

    ECode Enumerate(
        /* [out] */ Array<IThread*>& list,
        /* [out] */ Integer* count) override;

    ECode Enumerate(
        /* [out] */ Array<IThread*>& list,
        /* [in] */ Boolean recurse,
        /* [out] */ Integer* count) override;

    ECode ActiveGroupCount(
        /* [out] */ Integer* count) override;

    ECode Enumerate(
        /* [out] */ Array<IThreadGroup*>& list,
        /* [out] */ Integer* count) override;

    ECode Enumerate(
        /* [out] */ Array<IThreadGroup*>& list,
        /* [in] */ Boolean recurse,
        /* [out] */ Integer* count) override;

    ECode Stop() override;

    ECode Interrupt() override;

    ECode Suspend() override;

    ECode Resume() override;

    ECode Destroy() override;

    ECode AddUnstarted() override;

    ECode Add(
        /* [in] */ IThread* t) override;

    ECode ThreadStartFailed(
        /* [in] */ IThread* t) override;

    ECode ThreadTerminated(
        /* [in] */ IThread* t) override;

    ECode List() override;

    ECode ToString(
        /* [out] */ String* desc) override;

    static ThreadGroup* From(
        /* [in] */ IThreadGroup* group);

private:
    Integer Enumerate(
        /* [out] */ Array<IThread*>& list,
        /* [in] */ Integer n,
        /* [in] */ Boolean recurse);

    Integer Enumerate(
        /* [in] */ Array<IThreadGroup*>& list,
        /* [in] */ Integer n,
        /* [in] */ Boolean recurse);

    Boolean StopOrSuspend(
        /* [in] */ Boolean suspend);

    void Remove(
        /* [in] */ IThreadGroup* g);

    ECode Add(
        /* [in] */ IThreadGroup* g);

    void Remove(
        /* [in] */ IThread* t);

    void List(
        /* [in] */ IPrintStream* out,
        /* [in] */ Integer indent);

public:
    String mName;

    Integer mMaxPriority;

    Boolean mDestroyed = false;

    Boolean mDaemon = false;

    Integer mNUnstartedThreads = 0;

    Integer mNthreads = 0;

    Array<IThread*> mThreads;

    Integer mNgroups = 0;
    Array<IThreadGroup*> mGroups;

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
