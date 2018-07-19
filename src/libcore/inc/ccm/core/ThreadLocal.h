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

#ifndef __CCM_CORE_THREADLOCAL_H__
#define __CCM_CORE_THREADLOCAL_H__

#include "ccm/core/SyncObject.h"
#include "ccm.core.IThread.h"
#include "ccm.core.IThreadLocal.h"
#include <ccmautoptr.h>
#include <ccmobject.h>
#include <ccmrefbase.h>

namespace ccm {
namespace core {

class ThreadLocal
    : public SyncObject
    , public IThreadLocal
{
protected:
    class ThreadLocalMap
        : public Object
    {
    public:
        class Entry
            : public LightRefBase
        {
        public:
            Entry(
                /* [in] */ ThreadLocal* k,
                /* [in] */ IInterface* v);

            void Clear();

        public:
            AutoPtr<IWeakReference> mKey;
            AutoPtr<IInterface> mValue;
        };

    public:
        ThreadLocalMap(
            /* [in] */ ThreadLocal* key,
            /* [in] */ IInterface* firstValue);

    private:
        void SetThreshold(
            /* [in] */ Integer len);

        static Integer GetNextIndex(
            /* [in] */ Integer i,
            /* [in] */ Integer len);

        AutoPtr<Entry> GetEntry(
            /* [in] */ ThreadLocal* key);

        AutoPtr<Entry> GetEntryAfterMiss(
            /* [in] */ ThreadLocal* key,
            /* [in] */ Integer i,
            /* [in] */ Entry* e);

        void Set(
            /* [in] */ ThreadLocal* key,
            /* [in] */ IInterface* value);

        void Remove(
            /* [in] */ ThreadLocal* key);

        void ReplaceStaleEntry(
            /* [in] */ ThreadLocal* key,
            /* [in] */ IInterface* value,
            /* [in] */ Integer staleSlot);

        Integer ExpungeStaleEntry(
            /* [in] */ Integer staleSlot);

        Boolean CleanSomeSlots(
            /* [in] */ Integer i,
            /* [in] */ Integer n);

        void Rehash();

    private:
        static constexpr Integer INITIAL_CAPACITY = 16;

        Array<Entry*> mTable;

        Integer mSize = 0;

        Integer mThreshold = 0;

        friend class ThreadLocal;
    };

public:
    CCM_INTERFACE_DECL();

    ECode Constructor();

    ECode Get(
        /* [out] */ IInterface** value) override;

    ECode Set(
        /* [in] */ IInterface* value) override;

    ECode Remove() override;

    static ThreadLocal* From(
        /* [in] */ IThreadLocal* tl);

protected:
    virtual AutoPtr<IInterface> InitialValue();

    virtual AutoPtr<ThreadLocalMap> GetMap(
        /* [in] */ IThread* t);

    virtual void CreateMap(
        /* [in] */ IThread* t,
        /* [in] */ IInterface* firstValue);

private:
    static Integer GetNextHashCode();

    ECode SetInitialValue(
        /* [out] */ IInterface** value);

private:
    Integer mThreadLocalHashCode;

    static constexpr Integer HASH_INCREMENT = 0x61c88647;

    friend class Thread;
};

inline ThreadLocal* ThreadLocal::From(
    /* [in] */ IThreadLocal* tl)
{
    return (ThreadLocal*)tl;
}

}
}

#endif // __CCM_CORE_THREADLOCAL_H__
