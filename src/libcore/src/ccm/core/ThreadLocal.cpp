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

#include "ccm/core/Thread.h"
#include "ccm/core/ThreadLocal.h"

namespace ccm {
namespace core {

CCM_INTERFACE_IMPL_1(ThreadLocal, SyncObject, IThreadLocal);

ECode ThreadLocal::Constructor()
{
    mThreadLocalHashCode = GetNextHashCode();
    return NOERROR;
}

Integer ThreadLocal::GetNextHashCode()
{
    return 0;
}

AutoPtr<IInterface> ThreadLocal::InitialValue()
{
    return nullptr;
}

ECode ThreadLocal::Get(
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value);

    AutoPtr<IThread> t;
    Thread::GetCurrentThread((IThread**)&t);
    AutoPtr<ThreadLocalMap> map = GetMap(t);
    if (map != nullptr) {
        AutoPtr<ThreadLocalMap::Entry> e = map->GetEntry(this);
        if (e != nullptr) {
            *value = e->mValue;
            REFCOUNT_ADD(*value);
            return NOERROR;
        }
    }
    return SetInitialValue(value);
}

ECode ThreadLocal::SetInitialValue(
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value);

    AutoPtr<IInterface> v = InitialValue();
    AutoPtr<IThread> t;
    Thread::GetCurrentThread((IThread**)&t);
    AutoPtr<ThreadLocalMap> map = GetMap(t);
    if (map != nullptr) {
        map->Set(this, v);
    }
    else {
        CreateMap(t, v);
    }
    *value = v;
    REFCOUNT_ADD(*value);
    return NOERROR;
}

ECode ThreadLocal::Set(
    /* [in] */ IInterface* value)
{
    AutoPtr<IThread> t;
    Thread::GetCurrentThread((IThread**)&t);
    AutoPtr<ThreadLocalMap> map = GetMap(t);
    if (map != nullptr) {
        map->Set(this, value);
    }
    else {
        CreateMap(t, value);
    }
    return NOERROR;
}

ECode ThreadLocal::Remove()
{
    AutoPtr<IThread> t;
    Thread::GetCurrentThread((IThread**)&t);
    AutoPtr<ThreadLocalMap> map = GetMap(t);
    if (map != nullptr) {
        map->Remove(this);
    }
    return NOERROR;
}

AutoPtr<ThreadLocal::ThreadLocalMap> ThreadLocal::GetMap(
    /* [in] */ IThread* t)
{
    return Thread::From(t)->mThreadLocals;
}

void ThreadLocal::CreateMap(
    /* [in] */ IThread* t,
    /* [in] */ IInterface* firstValue)
{
    Thread::From(t)->mThreadLocals = new ThreadLocalMap(this, firstValue);
}

//-------------------------------------------------------------------------

ThreadLocal::ThreadLocalMap::ThreadLocalMap(
    /* [in] */ ThreadLocal* key,
    /* [in] */ IInterface* firstValue)
    : mTable(INITIAL_CAPACITY)
{
    Integer i = key->mThreadLocalHashCode & (INITIAL_CAPACITY - 1);
    mTable.Set(i, new Entry(key, firstValue));
    mSize = 1;
    SetThreshold(INITIAL_CAPACITY);
}

void ThreadLocal::ThreadLocalMap::SetThreshold(
    /* [in] */ Integer len)
{
    mThreshold = len * 2 / 3;
}

Integer ThreadLocal::ThreadLocalMap::GetNextIndex(
    /* [in] */ Integer i,
    /* [in] */ Integer len)
{
    return ((i + 1 < len) ? i + 1 : 0);
}

AutoPtr<ThreadLocal::ThreadLocalMap::Entry>
ThreadLocal::ThreadLocalMap::GetEntry(
    /* [in] */ ThreadLocal* key)
{
    Integer i = key->mThreadLocalHashCode & (mTable.GetLength() - 1);
    AutoPtr<Entry> e = mTable[i];
    AutoPtr<IThreadLocal> tl;
    if (e != nullptr && e->mKey != nullptr &&
            (e->mKey->Resolve(IID_IThreadLocal, (IInterface**)&tl),
            ThreadLocal::From(tl) == key)) {
        return e;
    }
    else {
        return GetEntryAfterMiss(key, i, e);
    }
}

AutoPtr<ThreadLocal::ThreadLocalMap::Entry>
ThreadLocal::ThreadLocalMap::GetEntryAfterMiss(
    /* [in] */ ThreadLocal* key,
    /* [in] */ Integer i,
    /* [in] */ Entry* e)
{
    Integer len = mTable.GetLength();

    while (e != nullptr) {
        AutoPtr<IThreadLocal> tl;
        if (e->mKey != nullptr) {
            e->mKey->Resolve(IID_IThreadLocal, (IInterface**)&tl);
        }
        ThreadLocal* k = ThreadLocal::From(tl);
        if (k == key) {
            return e;
        }
        if (k == nullptr) {
            ExpungeStaleEntry(i);
        }
        else {
            i = GetNextIndex(i, len);
        }
        e = mTable[i];
    }
    return nullptr;
}

void ThreadLocal::ThreadLocalMap::Set(
    /* [in] */ ThreadLocal* key,
    /* [in] */ IInterface* value)
{
    Array<Entry*>& tab = mTable;
    Integer len = tab.GetLength();
    Integer i = key->mThreadLocalHashCode & (len - 1);

    for (Entry* e = tab[i]; e != nullptr;
            e = tab[i = GetNextIndex(i, len)]) {
        AutoPtr<IThreadLocal> tl;
        if (e->mKey != nullptr) {
            e->mKey->Resolve(IID_IThreadLocal, (IInterface**)&tl);
        }
        ThreadLocal* k = ThreadLocal::From(tl);

        if (k == key) {
            e->mValue = value;
            return;
        }

        if (k == nullptr) {
            ReplaceStaleEntry(key, value, i);
            return;
        }
    }

    tab.Set(i, new Entry(key, value));
    Integer sz = ++mSize;
    if (!CleanSomeSlots(i, sz) && sz >= mThreshold) {
        Rehash();
    }
}

void ThreadLocal::ThreadLocalMap::Remove(
    /* [in] */ ThreadLocal* key)
{
    Integer len = mTable.GetLength();
    Integer i = key->mThreadLocalHashCode & (len - 1);
    for (Entry* e = mTable[i]; e != nullptr;
            e = mTable[i = GetNextIndex(i, len)]) {
        AutoPtr<IThreadLocal> tl;
        if (e->mKey != nullptr) {
            e->mKey->Resolve(IID_IThreadLocal, (IInterface**)&tl);
        }
        ThreadLocal* k = ThreadLocal::From(tl);
        if (k == key) {
            e->Clear();
            ExpungeStaleEntry(i);
            return;
        }
    }
}

void ThreadLocal::ThreadLocalMap::ReplaceStaleEntry(
    /* [in] */ ThreadLocal* key,
    /* [in] */ IInterface* value,
    /* [in] */ Integer staleSlot)
{}

Integer ThreadLocal::ThreadLocalMap::ExpungeStaleEntry(
    /* [in] */ Integer staleSlot)
{
    return 0;
}

Boolean ThreadLocal::ThreadLocalMap::CleanSomeSlots(
    /* [in] */ Integer i,
    /* [in] */ Integer n)
{
    return false;
}

void ThreadLocal::ThreadLocalMap::Rehash()
{}

//-------------------------------------------------------------------------

ThreadLocal::ThreadLocalMap::Entry::Entry(
    /* [in] */ ThreadLocal* k,
    /* [in] */ IInterface* v)
{
    k->GetWeakReference((IWeakReference**)&mKey);
    mValue = v;
}

void ThreadLocal::ThreadLocalMap::Entry::Clear()
{
    mKey = nullptr;
    mValue = nullptr;
}

}
}
