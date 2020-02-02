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

#include "como/core/AutoLock.h"
#include "como/core/CThreadGroup.h"
#include "como/core/Math.h"
#include "como/core/System.h"
#include "como/core/Thread.h"

namespace como {
namespace core {

COMO_INTERFACE_IMPL_1(ThreadGroup, SyncObject, IThreadGroup);

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

ECode ThreadGroup::Constructor()
{
    mName = "system";
    mMaxPriority = IThread::MAX_PRIORITY;
    return NOERROR;
}

ECode ThreadGroup::Constructor(
    /* [in] */ const String& name)
{
    AutoPtr<IThread> t;
    Thread::GetCurrentThread(&t);
    AutoPtr<IThreadGroup> tg;
    t->GetThreadGroup(tg);
    return Constructor(tg, name);
}

ECode ThreadGroup::Constructor(
    /* [in] */ IThreadGroup* parent,
    /* [in] */ const String& name)
{
    FAIL_RETURN(parent->CheckAccess());
    mName = name;
    mMaxPriority = From(parent)->mMaxPriority;
    mDaemon = From(parent)->mDaemon;
    mParent = parent;
    From(parent)->Add(this);
    return NOERROR;
}

ECode ThreadGroup::GetName(
    /* [out] */ String& name)
{
    name = mName;
    return NOERROR;
}

ECode ThreadGroup::GetParent(
    /* [out] */ AutoPtr<IThreadGroup>& parent)
{
    if (mParent != nullptr) {
        FAIL_RETURN(mParent->CheckAccess());
    }
    parent = mParent;
    return NOERROR;
}

ECode ThreadGroup::GetMaxPriority(
    /* [out] */ Integer& priority)
{
    priority = mMaxPriority;
    return NOERROR;
}

ECode ThreadGroup::IsDaemon(
    /* [out] */ Boolean& daemon)
{
    daemon = mDaemon;
    return NOERROR;
}

ECode ThreadGroup::IsDestroyed(
    /* [out] */ Boolean& destroyed)
{
    destroyed = mDestroyed;
    return NOERROR;
}

ECode ThreadGroup::SetDaemon(
    /* [in] */ Boolean daemon)
{
    FAIL_RETURN(CheckAccess());
    mDaemon = daemon;
    return NOERROR;
}

ECode ThreadGroup::SetMaxPriority(
    /* [in] */ Integer pri)
{
    Integer ngroupsSnapshot;
    Array<IThreadGroup*> groupsSnapshot;
    {
        AutoLock lock(this);
        FAIL_RETURN(CheckAccess());
        if (pri < IThread::MIN_PRIORITY) {
            pri = IThread::MIN_PRIORITY;
        }
        if (pri > IThread::MAX_PRIORITY) {
            pri = IThread::MAX_PRIORITY;
        }

        mMaxPriority = pri;
        if (mParent != nullptr) {
            Integer pprior;
            mParent->GetMaxPriority(pprior);
            mMaxPriority = Math::Min(pri, pprior);
        }
        ngroupsSnapshot = mNgroups;
        if (!mGroups.IsEmpty()) {
            groupsSnapshot = Array<IThreadGroup*>(ngroupsSnapshot);
            groupsSnapshot.Copy(mGroups, ngroupsSnapshot);
        }
    }
    for (Integer i = 0; i < ngroupsSnapshot; i++) {
        groupsSnapshot[i]->SetMaxPriority(pri);
    }
    return NOERROR;
}

ECode ThreadGroup::ParentOf(
    /* [in] */ IThreadGroup* g,
    /* [out] */ Boolean& result)
{
    AutoPtr<IThreadGroup> group = g;
    while (group != nullptr) {
        if (group == (IThreadGroup*)this) {
            result = true;
            return NOERROR;
        }
        AutoPtr<IThreadGroup> p;
        group->GetParent(p);
        group = p;
    }
    result = false;
    return NOERROR;
}

ECode ThreadGroup::CheckAccess()
{
    return NOERROR;
}

ECode ThreadGroup::ActiveCount(
    /* [out] */ Integer& count)
{
    Integer result;
    Integer ngroupsSnapshot;
    Array<IThreadGroup*> groupsSnapshot;
    {
        AutoLock lock(this);
        if (mDestroyed) {
            count = 0;
            return NOERROR;
        }
        result = mNthreads;
        ngroupsSnapshot = mNgroups;
        if (!mGroups.IsEmpty()) {
            groupsSnapshot = Array<IThreadGroup*>(ngroupsSnapshot);
            groupsSnapshot.Copy(mGroups, ngroupsSnapshot);
        }
    }
    for (Integer i = 0; i < ngroupsSnapshot; i++) {
        Integer ac;
        groupsSnapshot[i]->ActiveCount(ac);
        result += ac;
    }
    count = result;
    return NOERROR;
}

ECode ThreadGroup::Enumerate(
    /* [out] */ Array<IThread*>& list,
    /* [out] */ Integer& count)
{
    FAIL_RETURN(CheckAccess());
    count = Enumerate(list, 0, true);
    return NOERROR;
}

ECode ThreadGroup::Enumerate(
    /* [out] */ Array<IThread*>& list,
    /* [in] */ Boolean recurse,
    /* [out] */ Integer& count)
{
    FAIL_RETURN(CheckAccess());
    count = Enumerate(list, 0, recurse);
    return NOERROR;
}

Integer ThreadGroup::Enumerate(
    /* [out] */ Array<IThread*>& list,
    /* [in] */ Integer n,
    /* [in] */ Boolean recurse)
{
    Integer ngroupsSnapshot = 0;
    Array<IThreadGroup*> groupsSnapshot;
    {
        AutoLock lock(this);
        if (mDestroyed) {
            return 0;
        }
        Integer nt = mNthreads;
        if (nt > list.GetLength() - n) {
            nt = list.GetLength() - n;
        }
        for (Integer i = 0; i < nt; i++) {
            Boolean alive;
            if (mThreads[i]->IsAlive(alive), alive) {
                list.Set(n++, mThreads[i]);
            }
        }
        if (recurse) {
            ngroupsSnapshot = mNgroups;
            if (!mGroups.IsEmpty()) {
                groupsSnapshot = Array<IThreadGroup*>(ngroupsSnapshot);
                groupsSnapshot.Copy(mGroups, ngroupsSnapshot);
            }
        }
    }
    if (recurse) {
        for (Integer i = 0; i < ngroupsSnapshot; i++) {
            n = From(groupsSnapshot[i])->Enumerate(list, n, true);
        }
    }
    return n;
}

ECode ThreadGroup::ActiveGroupCount(
    /* [out] */ Integer& count)
{
    Integer ngroupsSnapshot;
    Array<IThreadGroup*> groupsSnapshot;
    {
        AutoLock lock(this);
        if (mDestroyed) {
            count = 0;
            return NOERROR;
        }
        ngroupsSnapshot = mNgroups;
        if (!mGroups.IsEmpty()) {
            groupsSnapshot = Array<IThreadGroup*>(ngroupsSnapshot);
            groupsSnapshot.Copy(mGroups, ngroupsSnapshot);
        }
    }
    Integer n = ngroupsSnapshot;
    for (Integer i = 0; i < ngroupsSnapshot; i++) {
        Integer agc;
        groupsSnapshot[i]->ActiveGroupCount(agc);
        n += agc;
    }
    count = n;
    return NOERROR;
}

ECode ThreadGroup::Enumerate(
    /* [out] */ Array<IThreadGroup*>& list,
    /* [out] */ Integer& count)
{
    FAIL_RETURN(CheckAccess());
    count = Enumerate(list, 0, true);
    return NOERROR;
}

ECode ThreadGroup::Enumerate(
    /* [out] */ Array<IThreadGroup*>& list,
    /* [in] */ Boolean recurse,
    /* [out] */ Integer& count)
{
    FAIL_RETURN(CheckAccess());
    count = Enumerate(list, 0, recurse);
    return NOERROR;
}

Integer ThreadGroup::Enumerate(
    /* [in] */ Array<IThreadGroup*>& list,
    /* [in] */ Integer n,
    /* [in] */ Boolean recurse)
{
    Integer ngroupsSnapshot = 0;
    Array<IThreadGroup*> groupsSnapshot;
    {
        AutoLock lock(this);
        if (mDestroyed) {
            return 0;
        }
        Integer ng = mNgroups;
        if (ng > list.GetLength() - n) {
            ng = list.GetLength() - n;
        }
        if (ng > 0) {
            list.Copy(n, mGroups, 0, ng);
            n += ng;
        }
        if (recurse) {
            ngroupsSnapshot = mNgroups;
            if (!mGroups.IsEmpty()) {
                groupsSnapshot = Array<IThreadGroup*>(ngroupsSnapshot);
                groupsSnapshot.Copy(mGroups, ngroupsSnapshot);
            }
        }
    }
    if (recurse) {
        for (Integer i = 0; i < ngroupsSnapshot; i++) {
            n = From(groupsSnapshot[i])->Enumerate(list, n, true);
        }
    }
    return n;
}

ECode ThreadGroup::Stop()
{
    if (StopOrSuspend(false)) {
        AutoPtr<IThread> t;
        Thread::GetCurrentThread(&t);
        return t->Stop();
    }
    return NOERROR;
}

ECode ThreadGroup::Interrupt()
{
    Integer ngroupsSnapshot = 0;
    Array<IThreadGroup*> groupsSnapshot;
    {
        AutoLock lock(this);
        FAIL_RETURN(CheckAccess());
        for (Integer i = 0; i < mNthreads; i++) {
            mThreads[i]->Interrupt();
        }
        ngroupsSnapshot = mNgroups;
        if (!mGroups.IsEmpty()) {
            groupsSnapshot = Array<IThreadGroup*>(ngroupsSnapshot);
            groupsSnapshot.Copy(mGroups, ngroupsSnapshot);
        }
    }
    for (Integer i = 0; i < ngroupsSnapshot; i++) {
        groupsSnapshot[i]->Interrupt();
    }
    return NOERROR;
}

ECode ThreadGroup::Suspend()
{
    if (StopOrSuspend(true)) {
        AutoPtr<IThread> t;
        Thread::GetCurrentThread(&t);
        return t->Suspend();
    }
    return NOERROR;
}

Boolean ThreadGroup::StopOrSuspend(
    /* [in] */ Boolean suspend)
{
    Boolean suicide = false;
    AutoPtr<IThread> us;
    Thread::GetCurrentThread(&us);
    Integer ngroupsSnapshot = 0;
    Array<IThreadGroup*> groupsSnapshot;
    {
        AutoLock lock(this);
        FAIL_RETURN(CheckAccess());
        for (Integer i = 0 ; i < mNthreads ; i++) {
            if (mThreads[i] == us) {
                suicide = true;
            }
            else if (suspend) {
                mThreads[i]->Suspend();
            }
            else {
                mThreads[i]->Stop();
            }
        }
        ngroupsSnapshot = mNgroups;
        if (!mGroups.IsEmpty()) {
            groupsSnapshot = Array<IThreadGroup*>(ngroupsSnapshot);
            groupsSnapshot.Copy(mGroups, ngroupsSnapshot);
        }
    }
    for (Integer i = 0 ; i < ngroupsSnapshot ; i++) {
        suicide = From(groupsSnapshot[i])->StopOrSuspend(suspend) || suicide;
    }

    return suicide;
}

ECode ThreadGroup::Resume()
{
    Integer ngroupsSnapshot = 0;
    Array<IThreadGroup*> groupsSnapshot;
    {
        AutoLock lock(this);
        FAIL_RETURN(CheckAccess());
        for (Integer i = 0; i < mNthreads; i++) {
            mThreads[i]->Resume();
        }
        ngroupsSnapshot = mNgroups;
        if (!mGroups.IsEmpty()) {
            groupsSnapshot = Array<IThreadGroup*>(ngroupsSnapshot);
            groupsSnapshot.Copy(mGroups, ngroupsSnapshot);
        }
    }
    for (Integer i = 0; i < ngroupsSnapshot; i++) {
        groupsSnapshot[i]->Resume();
    }
    return NOERROR;
}

ECode ThreadGroup::Destroy()
{
    Integer ngroupsSnapshot = 0;
    Array<IThreadGroup*> groupsSnapshot;
    {
        AutoLock lock(this);
        FAIL_RETURN(CheckAccess());
        if (mDestroyed || mNthreads > 0) {
            return E_ILLEGAL_THREAD_STATE_EXCEPTION;
        }
        ngroupsSnapshot = mNgroups;
        if (!mGroups.IsEmpty()) {
            groupsSnapshot = Array<IThreadGroup*>(ngroupsSnapshot);
            groupsSnapshot.Copy(mGroups, ngroupsSnapshot);
        }
        if (mParent != nullptr) {
            mDestroyed = true;
            mNgroups = 0;
            mGroups.Clear();
            mNthreads = 0;
            mThreads.Clear();
        }
    }
    for (Integer i = 0; i < ngroupsSnapshot; i++) {
        groupsSnapshot[i]->Destroy();
    }
    if (mParent != nullptr) {
        From(mParent)->Remove(this);
    }
    return NOERROR;
}

ECode ThreadGroup::Add(
    /* [in] */ IThreadGroup* g)
{
    AutoLock lock(this);
    if (mDestroyed) {
        return E_ILLEGAL_THREAD_STATE_EXCEPTION;
    }
    if (mGroups.IsEmpty()) {
        mGroups = Array<IThreadGroup*>(4);
    }
    else if (mNgroups == mGroups.GetLength()) {
        Array<IThreadGroup*> oldGroups = mGroups;
        mGroups = Array<IThreadGroup*>(mNgroups * 2);
        mGroups.Copy(oldGroups, oldGroups.GetLength());
    }
    mGroups.Set(mNgroups, g);

    // This is done last so it doesn't matter in case the
    // thread is killed
    mNgroups++;
    return NOERROR;
}

void ThreadGroup::Remove(
    /* [in] */ IThreadGroup* g)
{
    AutoLock lock(this);
    if (mDestroyed) {
        return;
    }
    for (Integer i = 0; i < mNgroups; i++) {
        if (mGroups[i] == g) {
            mGroups.Set(i, nullptr);
            mNgroups -= 1;
            for (Integer j = i; j < mNgroups; j++) {
                mGroups[j] = mGroups[j + 1];
            }
            mGroups[mNgroups] = nullptr;
            break;
        }
    }
    if (mNthreads == 0) {
        NotifyAll();
    }
    if (mDaemon && (mNthreads == 0) &&
            (mNUnstartedThreads == 0) && (mNgroups == 0)) {
        Destroy();
    }
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

ECode ThreadGroup::ThreadStartFailed(
    /* [in] */ IThread* t)
{
    AutoLock lock(this);
    Remove(t);
    mNUnstartedThreads++;
    return NOERROR;
}

ECode ThreadGroup::ThreadTerminated(
    /* [in] */ IThread* t)
{
    AutoLock lock(this);
    Remove(t);

    if (mNthreads == 0) {
        NotifyAll();
    }
    if (mDaemon && (mNthreads == 0) &&
            (mNUnstartedThreads == 0) && (mNgroups == 0)) {
        Destroy();
    }
    return NOERROR;
}

void ThreadGroup::Remove(
    /* [in] */ IThread* t)
{
    AutoLock lock(this);
    if (mDestroyed) {
        return;
    }
    for (Integer i = 0; i < mNthreads; i++) {
        if (mThreads[i] == t) {
            mThreads.Set(i, nullptr);
            mNthreads -= 1;
            for (Integer j = i; j < mNthreads; j++) {
                mThreads[j] = mThreads[j + 1];
            }
            mThreads[mNthreads] = nullptr;
            break;
        }
    }
}

ECode ThreadGroup::List()
{
    List(System::GetOut(), 0);
    return NOERROR;
}

void ThreadGroup::List(
    /* [in] */ IPrintStream* out,
    /* [in] */ Integer indent)
{
    Integer ngroupsSnapshot = 0;
    Array<IThreadGroup*> groupsSnapshot;
    {
        AutoLock lock(this);
        for (Integer j = 0 ; j < indent ; j++) {
            out->Print(String(" "));
        }
        out->Println((IThreadGroup*)this);
        indent += 4;
        for (Integer i = 0 ; i < mNthreads ; i++) {
            for (Integer j = 0 ; j < indent ; j++) {
                out->Print(String(" "));
            }
            out->Println(mThreads[i]);
        }
        ngroupsSnapshot = mNgroups;
        if (!mGroups.IsEmpty()) {
            groupsSnapshot = Array<IThreadGroup*>(ngroupsSnapshot);
            groupsSnapshot.Copy(mGroups, ngroupsSnapshot);
        }
    }
    for (Integer i = 0; i < ngroupsSnapshot; i++) {
        From(groupsSnapshot[i])->List(out, indent);
    }
}

ECode ThreadGroup::ToString(
    /* [out] */ String& desc)
{
    AutoPtr<IMetaCoclass> klass;
    GetCoclass(klass);
    String cName;
    klass->GetName(cName);
    String name;
    GetName(name);
    desc = String::Format("%s[name=%s,maxpri=%d]",
            cName.string(), name.string(), mMaxPriority);
    return NOERROR;
}

}
}
