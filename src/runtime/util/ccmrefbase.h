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

/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __CCM_REFBASE_H__
#define __CCM_REFBASE_H__

#include "ccmtypes.h"
#include <atomic>

namespace ccm {

class COM_PUBLIC RefBase
{
public:
    class WeakRef
    {
    public:
        RefBase* GetRefBase() const;

        void IncWeak(
            /* [in] */ const void* id);

        void DecWeak(
            /* [in] */ const void* id);

        Boolean AttemptIncStrong(
            /* [in] */ const void* id);

        Boolean AttemptIncWeak(
            /* [in] */ const void* id);

        Integer GetWeakCount() const;

        void PrintRefs() const;

        void TrackMe(
            /* [in] */ Boolean enable,
            /* [in] */ Boolean retain);
    };

public:
    Integer IncStrong(
        /* [in] */ const void* id) const;

    Integer DecStrong(
        /* [in] */ const void* id) const;

    Integer ForceIncStrong(
        /* [in] */ const void* id) const;

    Integer GetStrongCount() const;

    WeakRef* CreateWeak(
        /* [in] */ const void* id) const;

    WeakRef* GetWeakRefs() const;

    inline void PrintRefs() const;

    inline void TrackMe(
        /* [in] */ Boolean enable,
        /* [in] */ Boolean retain);

    inline Integer AddRef(
        /* [in] */ HANDLE id = 0);

    inline Integer Release(
        /* [in] */ HANDLE id = 0);

protected:
    RefBase();

    virtual ~RefBase();

    enum {
        OBJECT_LIFETIME_STRONG  = 0x0000,
        OBJECT_LIFETIME_WEAK    = 0x0001,
        OBJECT_LIFETIME_MASK    = 0x0001
    };

    void ExtendObjectLifetime(
        /* [in] */ Integer mode);

    enum {
        FIRST_INC_STRONG = 0x0001
    };

    virtual void OnFirstRef();

    virtual void OnLastStrongRef(
        /* [in] */ const void* id);

    virtual Boolean OnIncStrongAttempted(
        /* [in] */ Integer flags,
        /* [in] */ const void* id);

    virtual void OnLastWeakRef(
        /* [in] */ const void* id);

private:
    friend class WeakRef;
    class WeakRefImpl;

    RefBase(
        /* [in] */ const RefBase& o);

    RefBase& operator=(
        /* [in] */ const RefBase& o);

    WeakRefImpl* const mRefs;
};

void RefBase::PrintRefs() const
{
    GetWeakRefs()->PrintRefs();
}

void RefBase::TrackMe(
    /* [in] */ Boolean enable,
    /* [in] */ Boolean retain)
{
    GetWeakRefs()->TrackMe(enable, retain);
}

Integer RefBase::AddRef(
    /* [in] */ HANDLE id)
{
    return IncStrong(reinterpret_cast<const void*>(id));
}

Integer RefBase::Release(
    /* [in] */ HANDLE id)
{
    return DecStrong(reinterpret_cast<const void*>(id));
}

// LightRefBase
class COM_PUBLIC LightRefBase
{
public:
    inline LightRefBase();

    inline Integer AddRef(
        /* [in] */ HANDLE id = 0) const;

    inline Integer Release(
        /* [in] */ HANDLE id = 0) const;

    inline Integer GetStrongCount() const;

protected:
    inline virtual ~LightRefBase();

private:
    mutable std::atomic<Integer> mCount;
};

LightRefBase::LightRefBase()
    : mCount(0)
{}

LightRefBase::~LightRefBase()
{}

Integer LightRefBase::AddRef(
    /* [in] */ HANDLE id) const
{
    Integer c = mCount.fetch_add(1, std::memory_order_relaxed);
    return c + 1;
}

Integer LightRefBase::Release(
    /* [in] */ HANDLE id) const
{
    Integer c = mCount.fetch_sub(1, std::memory_order_release);
    if (c == 1) {
        std::atomic_thread_fence(std::memory_order_acquire);
        delete this;
    }
    return c - 1;
}

Integer LightRefBase::GetStrongCount() const
{
    return mCount.load(std::memory_order_relaxed);
}

// WeakReferenceImpl
class COM_PUBLIC WeakReferenceImpl
    : public LightRefBase
    , public IWeakReference
{
public:
    WeakReferenceImpl(
        /* [in] */ IInterface* object,
        /* [in] */ RefBase::WeakRef* ref);

    ~WeakReferenceImpl();

    CCM_INTERFACE_DECL();

    ECode Resolve(
        /* [in] */ const InterfaceID& iid,
        /* [out] */ IInterface** object);

private:
    IInterface* mObject;
    RefBase::WeakRef* mRef;
};

}

#endif // __CCM_REFBASE_H__
