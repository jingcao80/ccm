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
#include "ccm/util/concurrent/atomic/CAtomicInteger.h"

using ccm::util::concurrent::atomic::CAtomicInteger;
using ccm::util::concurrent::atomic::IID_IAtomicInteger;

namespace ccm {
namespace core {

CCM_INTERFACE_IMPL_1(ThreadLocal, SyncObject, IThreadLocal);

static AutoPtr<IAtomicInteger> CreateGenerator()
{
    AutoPtr<IAtomicInteger> atomic;
    CAtomicInteger::New(IID_IAtomicInteger, (IInterface**)&atomic);
    return atomic;
}

AutoPtr<IAtomicInteger> ThreadLocal::GetHashCodeGenerator()
{
    static AutoPtr<IAtomicInteger> GENERATOR = CreateGenerator();
    return GENERATOR;
}

ECode ThreadLocal::Constructor()
{
    mThreadLocalHashCode = GetNextHashCode();
    return NOERROR;
}

Integer ThreadLocal::GetNextHashCode()
{
    AutoPtr<IAtomicInteger> gen = GetHashCodeGenerator();
    Integer hash;
    gen->GetAndAdd(HASH_INCREMENT, &hash);
    return hash;
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
    Thread::GetCurrentThread(&t);
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
    Thread::GetCurrentThread(&t);
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
    Thread::GetCurrentThread(&t);
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
    Thread::GetCurrentThread(&t);
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
    return (i + 1 < len) ? i + 1 : 0;
}

Integer ThreadLocal::ThreadLocalMap::GetPrevIndex(
    /* [in] */ Integer i,
    /* [in] */ Integer len)
{
    return (i - 1 >= 0) ? i - 1 : len - 1;
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
{
    Array<Entry*>& tab = mTable;
    Integer len = tab.GetLength();
    AutoPtr<Entry> e;

    Integer slotToExpunge = staleSlot;
    for (Integer i = GetPrevIndex(staleSlot, len);
            (e = tab[i], e != nullptr);
            i = GetPrevIndex(i, len)) {
        AutoPtr<IThreadLocal> tl;
        if (e->mKey != nullptr) {
            e->mKey->Resolve(IID_IThreadLocal, (IInterface**)&tl);
        }
        ThreadLocal* k = ThreadLocal::From(tl);
        if (k == nullptr) {
            slotToExpunge = i;
        }
    }

    // Find either the key or trailing null slot of run, whichever
    // occurs first
    for (Integer i = GetNextIndex(staleSlot, len);
            (e = tab[i], e != nullptr);
            i = GetNextIndex(i, len)) {
        AutoPtr<IThreadLocal> tl;
        if (e->mKey != nullptr) {
            e->mKey->Resolve(IID_IThreadLocal, (IInterface**)&tl);
        }
        ThreadLocal* k = ThreadLocal::From(tl);

        // If we find key, then we need to swap it
        // with the stale entry to maintain hash table order.
        // The newly stale slot, or any other stale slot
        // encountered above it, can then be sent to expungeStaleEntry
        // to remove or rehash all of the other entries in run.
        if (k == key) {
            e->mValue = value;

            tab.Set(i, tab[staleSlot]);
            tab.Set(staleSlot, e);

            // Start expunge at preceding stale entry if it exists
            if (slotToExpunge == staleSlot) {
                slotToExpunge = i;
            }
            CleanSomeSlots(ExpungeStaleEntry(slotToExpunge), len);
            return;
        }

        // If we didn't find stale entry on backward scan, the
        // first stale entry seen while scanning for key is the
        // first still present in the run.
        if (k == nullptr && slotToExpunge == staleSlot) {
            slotToExpunge = i;
        }
    }

    // If key not found, put new entry in stale slot
    tab[staleSlot]->mValue = nullptr;
    tab.Set(staleSlot, new Entry(key, value));

    // If there are any other stale entries in run, expunge them
    if (slotToExpunge != staleSlot) {
        CleanSomeSlots(ExpungeStaleEntry(slotToExpunge), len);
    }
}

Integer ThreadLocal::ThreadLocalMap::ExpungeStaleEntry(
    /* [in] */ Integer staleSlot)
{
    Array<Entry*>& tab = mTable;
    Integer len = tab.GetLength();

    // expunge entry at staleSlot
    tab[staleSlot]->mValue = nullptr;
    tab.Set(staleSlot, nullptr);
    mSize--;

    // Rehash until we encounter null
    AutoPtr<Entry> e;
    Integer i;
    for (i = GetNextIndex(staleSlot, len);
            (e = tab[i], e != nullptr);
            i = GetNextIndex(i, len)) {
        AutoPtr<IThreadLocal> tl;
        if (e->mKey != nullptr) {
            e->mKey->Resolve(IID_IThreadLocal, (IInterface**)&tl);
        }
        ThreadLocal* k = ThreadLocal::From(tl);
        if (k == nullptr) {
            e->mValue = nullptr;
            tab.Set(i, nullptr);
            mSize--;
        }
        else {
            Integer h = k->mThreadLocalHashCode & (len - 1);
            if (h != i) {
                tab.Set(i, nullptr);

                // Unlike Knuth 6.4 Algorithm R, we must scan until
                // null because multiple entries could have been stale.
                while (tab[h] != nullptr) {
                    h = GetNextIndex(h, len);
                }
                tab.Set(h, e);
            }
        }
    }
    return i;
}

Boolean ThreadLocal::ThreadLocalMap::CleanSomeSlots(
    /* [in] */ Integer i,
    /* [in] */ Integer n)
{
    Boolean removed = false;
    Integer len = mTable.GetLength();
    do {
        i = GetNextIndex(i, len);
        Entry* e = mTable[i];
        AutoPtr<IThreadLocal> tl;
        if (e != nullptr && (e->mKey == nullptr ||
                (e->mKey->Resolve(IID_IThreadLocal, (IInterface**)&tl),
                tl == nullptr))) {
            n = len;
            removed = true;
            i = ExpungeStaleEntry(i);
        }
    } while ((n = ((unsigned Integer)n) >> 1) != 0);
    return removed;
}

void ThreadLocal::ThreadLocalMap::Rehash()
{
    ExpungeStaleEntries();

    // Use lower threshold for doubling to avoid hysteresis
    if (mSize >= mThreshold - mThreshold / 4) {
        Resize();
    }
}

void ThreadLocal::ThreadLocalMap::Resize()
{
    Array<Entry*> oldTab = mTable;
    Integer oldLen = oldTab.GetLength();
    Integer newLen = oldLen * 2;
    Array<Entry*> newTab(newLen);
    Integer count = 0;

    for (Integer j = 0; j < oldLen; ++j) {
        Entry* e = oldTab[j];
        if (e != nullptr) {
            AutoPtr<IThreadLocal> tl;
            if (e->mKey != nullptr) {
                e->mKey->Resolve(IID_IThreadLocal, (IInterface**)&tl);
            }
            ThreadLocal* k = ThreadLocal::From(tl);
            if (k == nullptr) {
                e->mValue = nullptr;
            }
            else {
                Integer h = k->mThreadLocalHashCode & (newLen - 1);
                while (newTab[h] != nullptr) {
                    h = GetNextIndex(h, newLen);
                }
                newTab.Set(h, e);
                count++;
            }
        }
    }

    SetThreshold(newLen);
    mSize = count;
    mTable = newTab;
}

void ThreadLocal::ThreadLocalMap::ExpungeStaleEntries()
{
    Integer len = mTable.GetLength();
    for (Integer j = 0; j < len; j++) {
        Entry* e = mTable[j];
        AutoPtr<IThreadLocal> tl;
        if (e != nullptr && (e->mKey == nullptr ||
                (e->mKey->Resolve(IID_IThreadLocal, (IInterface**)&tl),
                tl == nullptr))) {
            ExpungeStaleEntry(j);
        }
    }
}

//-------------------------------------------------------------------------

ThreadLocal::ThreadLocalMap::Entry::Entry(
    /* [in] */ ThreadLocal* k,
    /* [in] */ IInterface* v)
{
    k->GetWeakReference(&mKey);
    mValue = v;
}

void ThreadLocal::ThreadLocalMap::Entry::Clear()
{
    mKey = nullptr;
    mValue = nullptr;
}

}
}
