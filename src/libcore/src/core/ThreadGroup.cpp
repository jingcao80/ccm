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

#include "core/AutoLock.h"
#include "core/CThreadGroup.h"
#include "core/Thread.h"

namespace ccm {
namespace core {

CCM_INTERFACE_IMPL_1(ThreadGroup, SyncObject, IThreadGroup);

static AutoPtr<IThreadGroup> CreateSystemThreadGroup()
{
    AutoPtr<IThreadGroup> tg;
    CThreadGroup::New(IID_IThreadGroup, (IInterface**)&tg);
    return tg;
}

static AutoPtr<IThreadGroup> CreateMainThreadGroup()
{
    AutoPtr<IThreadGroup> tg;
    CThreadGroup::New(ThreadGroup::GetSystemThreadGroup(),
            String("main"), IID_IThreadGroup, (IInterface**)&tg);
    return tg;
}

AutoPtr<IThreadGroup> ThreadGroup::GetSystemThreadGroup()
{
    static AutoPtr<IThreadGroup> sSystemThreadGroup =
            CreateSystemThreadGroup();
    return sSystemThreadGroup;
}

AutoPtr<IThreadGroup> ThreadGroup::GetMainThreadGroup()
{
    static AutoPtr<IThreadGroup> sMainThreadGroup =
            CreateMainThreadGroup();
    return sMainThreadGroup;
}

ECode ThreadGroup::constructor()
{
    mName = "system";
    mMaxPriority = IThread::MAX_PRIORITY;
    return NOERROR;
}

ECode ThreadGroup::constructor(
    /* [in] */ const String& name)
{
    AutoPtr<IThread> t;
    Thread::GetCurrentThread((IThread**)&t);
    AutoPtr<IThreadGroup> tg;
    t->GetThreadGroup((IThreadGroup**)&tg);
    return constructor(tg, name);
}

ECode ThreadGroup::constructor(
    /* [in] */ IThreadGroup* parent,
    /* [in] */ const String& name)
{
    mName = name;
    mMaxPriority = From(parent)->mMaxPriority;
    mParent = parent;
    From(parent)->Add(this);
    return NOERROR;
}

ECode ThreadGroup::Add(
    /* [in] */ IThreadGroup* g)
{
    return NOERROR;
}

ECode ThreadGroup::AddUnstarted()
{
    AutoLock lock(this);

    if (mDestroyed) {
        return E_ILLEGAL_THREAD_STATE_EXCEPTION;
    }
    mNUnstartedThreads++;

    return NOERROR;
}

ECode ThreadGroup::Add(
    /* [in] */ IThread* t)
{
    AutoLock lock(this);

    if (mDestroyed) {
        return E_ILLEGAL_THREAD_STATE_EXCEPTION;
    }
    if (mThreads.IsEmpty()) {
        mThreads = Array<IThread*>(4);
    }
    else if (mNthreads == mThreads.GetLength()) {
        Array<IThread*> oldArr = mThreads;
        mThreads = Array<IThread*>(mNthreads * 2);
        mThreads.Copy(oldArr, oldArr.GetLength());
    }
    mThreads.Set(mNthreads, t);

    // This is done last so it doesn't matter in case the
    // thread is killed
    mNthreads++;

    // The thread is now a fully fledged member of the group, even
    // though it may, or may not, have been started yet. It will prevent
    // the group from being destroyed so the unstarted Threads count is
    // decremented.
    mNUnstartedThreads--;

    return NOERROR;
}

ECode ThreadGroup::ThreadTerminated(
    /* [in] */ IThread* t)
{
    return NOERROR;
}

}
}
