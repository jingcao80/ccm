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

#include "comologger.h"
#include "comorefbase.h"
#include "mutex.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

namespace como {

// compile with refcounting debugging enabled
#define DEBUG_REFS                      0
// #define DEBUG_REFS_FATAL_SANITY_CHECKS  0
#define DEBUG_REFS_ENABLED_BY_DEFAULT   1
#define DEBUG_REFS_CALLSTACK_ENABLED    0

// folder where stack traces are saved when DEBUG_REFS is enabled
// this folder needs to exist and be writable
#define DEBUG_REFS_CALLSTACK_PATH       "./debug"

// log all reference counting operations
#define PRINT_REFS                      0

#define INITIAL_STRONG_VALUE (1<<28)

#define MAX_COUNT 0xfffff

#define BAD_STRONG(c) \
        ((c) == 0 || ((c) & (~(MAX_COUNT | INITIAL_STRONG_VALUE))) != 0)

#define BAD_WEAK(c) ((c) == 0 || ((c) & (~MAX_COUNT)) != 0)


class COM_LOCAL RefBase::WeakRefImpl : public RefBase::WeakRef
{
#if !DEBUG_REFS

public:
    explicit WeakRefImpl(
        /* [in] */ RefBase* base);

    void AddStrongRef(
        /* [in] */ const void* /*id*/);

    void RemoveStrongRef(
        /* [in] */ const void* /*id*/);

    void RenameStrongRefId(
        /* [in] */ const void* /*old_id*/,
        /* [in] */ const void* /*new_id*/);

    void AddWeakRef(
        /* [in] */ const void* /*id*/);

    void RemoveWeakRef(
        /* [in] */ const void* /*id*/);

    void RenameWeakRefId(
        /* [in] */ const void* /*old_id*/,
        /* [in] */ const void* /*new_id*/);

    void PrintRefs() const;

    void TrackMe(
        /* [in] */ Boolean,
        /* [in] */ Boolean);

#else

private:
    struct RefEntry
    {
        RefEntry* mNext;
        const void* mId;
#if DEBUG_REFS_CALLSTACK_ENABLED
        CallStack mStack;
#endif
        Integer mRef;
    };

public:
    explicit WeakRefImpl(
        /* [in] */ RefBase* base);

    ~WeakRefImpl();

    void AddStrongRef(
        /* [in] */ const void* id);

    void RemoveStrongRef(
        /* [in] */ const void* id);

    void RenameStrongRefId(
        /* [in] */ const void* oldId,
        /* [in] */ const void* newId);

    void AddWeakRef(
        /* [in] */ const void* id);

    void RemoveWeakRef(
        /* [in] */ const void* id);

    void RenameWeakRefId(
        /* [in] */ const void* oldId,
        /* [in] */ const void* newId);

    void PrintRefs() const;

    void TrackMe(
        /* [in] */ Boolean,
        /* [in] */ Boolean);

private:
    void AddRef(
        /* [in] */ RefEntry** refs,
        /* [in] */ const void* id,
        /* [in] */ Integer ref);

    void RemoveRef(
        /* [in] */ RefEntry** refs,
        /* [in] */ const void* id);

    void RenameRefsId(
        /* [in] */ RefEntry* r,
        /* [in] */ const void* oldId,
        /* [in] */ const void* newId);

    void PrintRefsLocked(
        /* [out] */ String* out,
        /* [in] */ const RefEntry* refs) const;

#endif // !DEBUG_REFS

public:
    std::atomic<Integer> mStrong;
    std::atomic<Integer> mWeak;
    RefBase* const mBase;
    std::atomic<Integer> mFlags;

#if DEBUG_REFS

private:
    mutable Mutex mMutex;
    RefEntry* mStrongRefs;
    RefEntry* mWeakRefs;

    Boolean mTrackEnabled;
    Boolean mRetain;

#endif
};

#if !DEBUG_REFS

RefBase::WeakRefImpl::WeakRefImpl(
    /* [in] */ RefBase* base)
    : mStrong(INITIAL_STRONG_VALUE)
    , mWeak(0)
    , mBase(base)
    , mFlags(0)
{}

void RefBase::WeakRefImpl::AddStrongRef(
    /* [in] */ const void* /*id*/)
{}

void RefBase::WeakRefImpl::RemoveStrongRef(
    /* [in] */ const void* /*id*/)
{}

void RefBase::WeakRefImpl::RenameStrongRefId(
    /* [in] */ const void* /*old_id*/,
    /* [in] */ const void* /*new_id*/)
{}

void RefBase::WeakRefImpl::AddWeakRef(
    /* [in] */ const void* /*id*/)
{}

void RefBase::WeakRefImpl::RemoveWeakRef(
    /* [in] */ const void* /*id*/)
{}

void RefBase::WeakRefImpl::RenameWeakRefId(
    /* [in] */ const void* /*old_id*/,
    /* [in] */ const void* /*new_id*/)
{}

void RefBase::WeakRefImpl::PrintRefs() const
{}

void RefBase::WeakRefImpl::TrackMe(
    /* [in] */ Boolean track,
    /* [in] */ Boolean retain)
{}

#else

RefBase::WeakRefImpl::WeakRefImpl(
    /* [in] */ RefBase* base)
    : mStrong(INITIAL_STRONG_VALUE)
    , mWeak(0)
    , mBase(base)
    , mFlags(0)
    , mStrongRefs(nullptr)
    , mWeakRefs(nullptr)
    , mTrackEnabled(!!DEBUG_REFS_ENABLED_BY_DEFAULT)
    , mRetain(false)
{
}

RefBase::WeakRefImpl::~WeakRefImpl()
{
    Boolean dumpStack = false;
    if (!mRetain && mStrongRefs != nullptr) {
        dumpStack = true;
        Logger::E("RefBase", "Strong references remain:");
        RefEntry* refs = mStrongRefs;
        while (refs) {
            char inc = refs->mRef >= 0 ? '+' : '-';
            Logger::D("RefBase", "\t%c ID %p (ref %d):", inc, refs->mId, refs->mRef);
#if DEBUG_REFS_CALLSTACK_ENABLED
            refs->mStack.log("RefBase");
#endif
            refs = refs->mNext;
        }
    }

    if (!mRetain && mWeakRefs != nullptr) {
        dumpStack = true;
        Logger::E("RefBase", "Weak references remain!");
        RefEntry* refs = mWeakRefs;
        while (refs) {
            char inc = refs->mRef >= 0 ? '+' : '-';
            Logger::D("RefBase", "\t%c ID %p (ref %d):", inc, refs->mId, refs->mRef);
#if DEBUG_REFS_CALLSTACK_ENABLED
            refs->mStack.log("RefBase");
#endif
            refs = refs->mNext;
        }
    }
    if (dumpStack) {
#if DEBUG_REFS_CALLSTACK_ENABLED
        Logger::E("RefBase", "above errors at:");
        CallStack stack("RefBase");
#endif
    }
}

void RefBase::WeakRefImpl::AddStrongRef(
    /* [in] */ const void* id)
{
    AddRef(&mStrongRefs, id, mStrong.load(std::memory_order_relaxed));
}

void RefBase::WeakRefImpl::RemoveStrongRef(
    /* [in] */ const void* id)
{
    if (!mRetain) {
        RemoveRef(&mStrongRefs, id);
    }
    else {
        AddRef(&mStrongRefs, id,
                -mStrong.load(std::memory_order_relaxed));
    }
}

void RefBase::WeakRefImpl::RenameStrongRefId(
    /* [in] */ const void* oldId,
    /* [in] */ const void* newId)
{
    RenameRefsId(mStrongRefs, oldId, newId);
}

void RefBase::WeakRefImpl::AddWeakRef(
    /* [in] */ const void* id)
{
    AddRef(&mWeakRefs, id,
            mWeak.load(std::memory_order_relaxed));
}

void RefBase::WeakRefImpl::RemoveWeakRef(
    /* [in] */ const void* id)
{
    if (!mRetain) {
        RemoveRef(&mWeakRefs, id);
    }
    else {
        AddRef(&mWeakRefs, id,
                -mWeak.load(std::memory_order_relaxed));
    }
}

void RefBase::WeakRefImpl::RenameWeakRefId(
    /* [in] */ const void* oldId,
    /* [in] */ const void* newId)
{
    RenameRefsId(mWeakRefs, oldId, newId);
}

void RefBase::WeakRefImpl::PrintRefs() const
{
    String text;

    {
        Mutex::AutoLock lock(mMutex);
        char buf[128];
        snprintf(buf, sizeof(buf),
                "Strong references on RefBase %p (WeakRef %p):\n",
                mBase, this);
        text += buf;
        PrintRefsLocked(&text, mStrongRefs);
        snprintf(buf, sizeof(buf),
                 "Weak references on RefBase %p (WeakRef %p):\n",
                 mBase, this);
        text += buf;
        PrintRefsLocked(&text, mWeakRefs);
    }

    char name[100];
    snprintf(name, sizeof(name), DEBUG_REFS_CALLSTACK_PATH "/%p.stack",
             this);
    Integer rc = open(name, O_RDWR | O_CREAT | O_APPEND, 644);
    if (rc >= 0) {
        write(rc, text.string(), text.GetByteLength());
        close(rc);
        Logger::D("RefBase", "STACK TRACE for %p saved in %s", this, name);
    }
    else Logger::E("RefBase", "FAILED TO PRINT STACK TRACE for %p in %s: %s", this,
            name, strerror(errno));
}

void RefBase::WeakRefImpl::TrackMe(
    /* [in] */ Boolean track,
    /* [in] */ Boolean retain)
{
    mTrackEnabled = track;
    mRetain = retain;
}

void RefBase::WeakRefImpl::AddRef(
    /* [in] */ RefEntry** refs,
    /* [in] */ const void* id,
    /* [in] */ Integer refCount)
{
    if (mTrackEnabled) {
        Mutex::AutoLock lock(mMutex);

        RefEntry* ref = new RefEntry();
        ref->mRef = refCount;
        ref->mId = id;
#if DEBUG_REFS_CALLSTACK_ENABLED
        ref->mStack.update(2);
#endif
        ref->mNext = *refs;
        *refs = ref;
    }
}

void RefBase::WeakRefImpl::RemoveRef(
    /* [in] */ RefEntry** refs,
    /* [in] */ const void* id)
{
    if (mTrackEnabled) {
        Mutex::AutoLock lock(mMutex);

        RefEntry* const head = *refs;
        RefEntry* ref = head;
        while (ref != nullptr) {
            if (ref->mId == id) {
                *refs = ref->mNext;
                delete ref;
                return;
            }
            refs = &ref->mNext;
            ref = *refs;
        }

        Logger::E("RefBase", "removing id %p on RefBase %p"
                "(WeakRef %p) that doesn't exist!",
                id, mBase, this);

        ref = head;
        while (ref) {
            char inc = ref->mRef >= 0 ? '+' : '-';
            Logger::D("RefBase", "\t%c ID %p (ref %d):",
                    inc, ref->mId, ref->mRef);
            ref = ref->mNext;
        }

#if DEBUG_REFS_CALLSTACK_ENABLED
        CallStack stack(LOG_TAG);
#endif
    }
}

void RefBase::WeakRefImpl::RenameRefsId(
    /* [in] */ RefEntry* r,
    /* [in] */ const void* oldId,
    /* [in] */ const void* newId)
{
    if (mTrackEnabled) {
        Mutex::AutoLock lock(mMutex);
        RefEntry* ref = r;
        while (ref != nullptr) {
            if (ref->mId == oldId) {
                ref->mId = newId;
            }
            ref = ref->mNext;
        }
    }
}

void RefBase::WeakRefImpl::PrintRefsLocked(
    /* [out] */ String* out,
    /* [in] */ const RefEntry* refs) const
{
    char buf[128];
    while (refs) {
        char inc = refs->mRef >= 0 ? '+' : '-';
        snprintf(buf, sizeof(buf), "\t%c ID %p (ref %d):\n",
                 inc, refs->mId, refs->mRef);
        *out += buf;
#if DEBUG_REFS_CALLSTACK_ENABLED
        *out += refs->stack.toString("\t\t");
#else
        *out += "\t\t(call stacks disabled)";
#endif
        refs = refs->mNext;
    }
}

#endif // !DEBUG_REFS


// ---------------------------------------------------------------------------

Integer RefBase::IncStrong(
    /* [in] */ const void* id) const
{
    WeakRefImpl* const refs = mRefs;
    refs->IncWeak(id);

    refs->AddStrongRef(id);
    const Integer c = refs->mStrong.fetch_add(1, std::memory_order_relaxed);
    if (c <= 0) {
        Logger::E("RefBase", "IncStrong() called on %p after last strong ref",
                refs);
        assert(0);
    }
#if PRINT_REFS
    Logger::D("RefBase", "IncStrong of %p from %p: cnt=%d\n", this, id, c);
#endif
    if (c != INITIAL_STRONG_VALUE)  {
        return c + 1;
    }

    Integer old = refs->mStrong.fetch_sub(INITIAL_STRONG_VALUE,
            std::memory_order_relaxed);
    if (old <= INITIAL_STRONG_VALUE) {
        Logger::E("RefBase", "0x%x too small", old);
        assert(0);
    }
    refs->mBase->OnFirstRef();
    return 1;
}

Integer RefBase::DecStrong(
    /* [in] */ const void* id) const
{
    WeakRefImpl* const refs = mRefs;
    refs->RemoveStrongRef(id);
    const Integer c = refs->mStrong.fetch_sub(1, std::memory_order_release);
#if PRINT_REFS
    Logger::D("RefBase", "DecStrong of %p from %p: cnt=%d\n", this, id, c);
#endif
    if (BAD_STRONG(c)) {
        Logger::E("RefBase", "DecStrong() called on %p too many times",
                refs);
        assert(0);
    }
    if (c == 1) {
        std::atomic_thread_fence(std::memory_order_acquire);
        refs->mBase->OnLastStrongRef(id);
        Integer flags = refs->mFlags.load(std::memory_order_relaxed);
        if ((flags & OBJECT_LIFETIME_MASK) == OBJECT_LIFETIME_STRONG) {
            delete this;
        }
    }
    refs->DecWeak(id);
    return c - 1;
}

Integer RefBase::ForceIncStrong(
    /* [in] */ const void* id) const
{
    // Allows initial mStrong of 0 in addition to INITIAL_STRONG_VALUE.
    WeakRefImpl* const refs = mRefs;
    refs->IncWeak(id);

    refs->AddStrongRef(id);
    const Integer c = refs->mStrong.fetch_add(1, std::memory_order_relaxed);
    if (c < 0) {
        Logger::E("RefBase", "forceIncStrong called on %p after ref count underflow",
                refs);
        assert(0);
    }
#if PRINT_REFS
    Logger::D("RefBase", "ForceIncStrong of %p from %p: cnt=%d\n", this, id, c);
#endif

    switch (c) {
        case INITIAL_STRONG_VALUE:
            refs->mStrong.fetch_sub(INITIAL_STRONG_VALUE,
                    std::memory_order_relaxed);
            // fall through...
        case 0:
            refs->mBase->OnFirstRef();
            return 1;
        default:
            return c + 1;
    }

}

Integer RefBase::GetStrongCount() const
{
    return mRefs->mStrong.load(std::memory_order_relaxed);
}

RefBase* RefBase::WeakRef::GetRefBase() const
{
    return static_cast<const WeakRefImpl*>(this)->mBase;
}

void RefBase::WeakRef::IncWeak(
    /* [in] */ const void* id)
{
    WeakRefImpl* const impl = static_cast<WeakRefImpl*>(this);
    impl->AddWeakRef(id);
    const Integer c = impl->mWeak.fetch_add(1,
            std::memory_order_relaxed);
    if (c < 0) {
        Logger::E("RefBase", "IncWeak called on %p after last weak ref", this);
        assert(0);
    }
}

void RefBase::WeakRef::DecWeak(
    /* [in] */ const void* id)
{
    WeakRefImpl* const impl = static_cast<WeakRefImpl*>(this);
    impl->RemoveWeakRef(id);
    const Integer c = impl->mWeak.fetch_sub(1, std::memory_order_release);
    if (BAD_WEAK(c)) {
        Logger::E("RefBase", "decWeak called on %p too many times",
                this);
        assert(0);
    }
    if (c != 1) return;
    atomic_thread_fence(std::memory_order_acquire);

    Integer flags = impl->mFlags.load(std::memory_order_relaxed);
    if ((flags & OBJECT_LIFETIME_MASK) == OBJECT_LIFETIME_STRONG) {
        // This is the regular lifetime case. The object is destroyed
        // when the last strong reference goes away. Since weakref_impl
        // outlives the object, it is not destroyed in the dtor, and
        // we'll have to do it here.
        if (impl->mStrong.load(std::memory_order_relaxed)
                == INITIAL_STRONG_VALUE) {
            // Decrementing a weak count to zero when object never had a strong
            // reference.  We assume it acquired a weak reference early, e.g.
            // in the constructor, and will eventually be properly destroyed,
            // usually via incrementing and decrementing the strong count.
            // Thus we no longer do anything here.  We log this case, since it
            // seems to be extremely rare, and should not normally occur. We
            // used to deallocate mBase here, so this may now indicate a leak.
            Logger::W("RefBase", "Object at %p lost last weak reference "
                    "before it had a strong reference", impl->mBase);
        }
        else {
            // ALOGV("Freeing refs %p of old RefBase %p\n", this, impl->mBase);
            delete impl;
        }
    }
    else {
        // This is the OBJECT_LIFETIME_WEAK case. The last weak-reference
        // is gone, we can destroy the object.
        impl->mBase->OnLastWeakRef(id);
        delete impl->mBase;
    }
}

Boolean RefBase::WeakRef::AttemptIncStrong(
    /* [in] */ const void* id)
{
    IncWeak(id);

    WeakRefImpl* const impl = static_cast<WeakRefImpl*>(this);
    Integer curCount = impl->mStrong.load(std::memory_order_relaxed);

    if (curCount < 0) {
        Logger::E("RefBase", "AttemptIncStrong called on %p after underflow",
                this);
        assert(0);
    }

    while (curCount > 0 && curCount != INITIAL_STRONG_VALUE) {
        // we're in the easy/common case of promoting a weak-reference
        // from an existing strong reference.
        if (impl->mStrong.compare_exchange_weak(curCount, curCount + 1,
                std::memory_order_relaxed)) {
            break;
        }
        // the strong count has changed on us, we need to re-assert our
        // situation. curCount was updated by compare_exchange_weak.
    }

    if (curCount <= 0 || curCount == INITIAL_STRONG_VALUE) {
        // we're now in the harder case of either:
        // - there never was a strong reference on us
        // - or, all strong references have been released
        Integer flags = impl->mFlags.load(std::memory_order_relaxed);
        if ((flags & OBJECT_LIFETIME_MASK) == OBJECT_LIFETIME_STRONG) {
            // this object has a "normal" life-time, i.e.: it gets destroyed
            // when the last strong reference goes away
            if (curCount <= 0) {
                // the last strong-reference got released, the object cannot
                // be revived.
                DecWeak(id);
                return false;
            }

            // here, curCount == INITIAL_STRONG_VALUE, which means
            // there never was a strong-reference, so we can try to
            // promote this object; we need to do that atomically.
            while (curCount > 0) {
                if (impl->mStrong.compare_exchange_weak(curCount, curCount + 1,
                        std::memory_order_relaxed)) {
                    break;
                }
                // the strong count has changed on us, we need to re-assert our
                // situation (e.g.: another thread has inc/decStrong'ed us)
                // curCount has been updated.
            }

            if (curCount <= 0) {
                // promote() failed, some other thread destroyed us in the
                // meantime (i.e.: strong count reached zero).
                DecWeak(id);
                return false;
            }
        }
        else {
            // this object has an "extended" life-time, i.e.: it can be
            // revived from a weak-reference only.
            // Ask the object's implementation if it agrees to be revived
            if (!impl->mBase->OnIncStrongAttempted(FIRST_INC_STRONG, id)) {
                // it didn't so give-up.
                DecWeak(id);
                return false;
            }
            // grab a strong-reference, which is always safe due to the
            // extended life-time.
            curCount = impl->mStrong.fetch_add(1, std::memory_order_relaxed);
            // If the strong reference count has already been incremented by
            // someone else, the implementor of onIncStrongAttempted() is holding
            // an unneeded reference.  So call onLastStrongRef() here to remove it.
            // (No, this is not pretty.)  Note that we MUST NOT do this if we
            // are in fact acquiring the first reference.
            if (curCount != 0 && curCount != INITIAL_STRONG_VALUE) {
                impl->mBase->OnLastStrongRef(id);
            }
        }
    }

    impl->AddStrongRef(id);

#if PRINT_REFS
    Logger::D("RefBase", "attemptIncStrong of %p from %p: cnt=%d\n", this, id, curCount);
#endif

    // curCount is the value of mStrong before we incremented it.
    // Now we need to fix-up the count if it was INITIAL_STRONG_VALUE.
    // This must be done safely, i.e.: handle the case where several threads
    // were here in attemptIncStrong().
    // curCount > INITIAL_STRONG_VALUE is OK, and can happen if we're doing
    // this in the middle of another incStrong.  The subtraction is handled
    // by the thread that started with INITIAL_STRONG_VALUE.
    if (curCount == INITIAL_STRONG_VALUE) {
        impl->mStrong.fetch_sub(INITIAL_STRONG_VALUE,
                std::memory_order_relaxed);
    }

    return true;
}

Boolean RefBase::WeakRef::AttemptIncWeak(
    /* [in] */ const void* id)
{
    WeakRefImpl* const impl = static_cast<WeakRefImpl*>(this);

    Integer curCount = impl->mWeak.load(std::memory_order_relaxed);
    if (curCount < 0) {
        Logger::E("RefBase", "AttemptIncWeak called on %p after underflow",
               this);
        assert(0);
    }
    while (curCount > 0) {
        if (impl->mWeak.compare_exchange_weak(curCount, curCount + 1,
                std::memory_order_relaxed)) {
            break;
        }
        // curCount has been updated.
    }

    if (curCount > 0) {
        impl->AddWeakRef(id);
    }

    return curCount > 0;
}

Integer RefBase::WeakRef::GetWeakCount() const
{
    return static_cast<const WeakRefImpl*>(this)->mWeak.load(
            std::memory_order_relaxed);
}

void RefBase::WeakRef::PrintRefs() const
{
    static_cast<const WeakRefImpl*>(this)->PrintRefs();
}

void RefBase::WeakRef::TrackMe(
    /* [in] */ Boolean enable,
    /* [in] */ Boolean retain)
{
    static_cast<WeakRefImpl*>(this)->TrackMe(enable, retain);
}

RefBase::WeakRef* RefBase::CreateWeak(
    /* [in] */ const void* id) const
{
    mRefs->IncWeak(id);
    return mRefs;
}

RefBase::WeakRef* RefBase::GetWeakRefs() const
{
    return mRefs;
}

RefBase::RefBase()
    : mRefs(new WeakRefImpl(this))
{
}

RefBase::~RefBase()
{
    Integer flags = mRefs->mFlags.load(std::memory_order_relaxed);
    // Life-time of this object is extended to WEAK, in
    // which case weakref_impl doesn't out-live the object and we
    // can free it now.
    if ((flags & OBJECT_LIFETIME_MASK) == OBJECT_LIFETIME_WEAK) {
        // It's possible that the weak count is not 0 if the object
        // re-acquired a weak reference in its destructor
        if (mRefs->mWeak.load(std::memory_order_relaxed) == 0) {
            delete mRefs;
        }
    }
    else if (mRefs->mStrong.load(std::memory_order_relaxed)
            == INITIAL_STRONG_VALUE) {
        // We never acquired a strong reference on this object.
        delete mRefs;
    }
    const_cast<WeakRefImpl*&>(mRefs) = nullptr;
}

void RefBase::ExtendObjectLifetime(
    /* [in] */ Integer mode)
{
    // Must be happens-before ordered with respect to construction or any
    // operation that could destroy the object.
    mRefs->mFlags.fetch_or(mode, std::memory_order_relaxed);
}

void RefBase::OnFirstRef()
{
}

void RefBase::OnLastStrongRef(
    /* [in] */ const void* /*id*/)
{
}

Boolean RefBase::OnIncStrongAttempted(
    /* [in] */ Integer flags,
    /* [in] */ const void* /*id*/)
{
    return (flags & FIRST_INC_STRONG) ? true : false;
}

void RefBase::OnLastWeakRef(
    /* [in] */ const void* /*id*/)
{
}

// ---------------------------------------------------------------------------

WeakReferenceImpl::WeakReferenceImpl(
    /* [in] */ IInterface* object,
    /* [in] */ RefBase::WeakRef* ref)
    : mObject(object)
    , mRef(ref)
{}

WeakReferenceImpl::~WeakReferenceImpl()
{
    if (mRef != nullptr) {
        mRef->DecWeak(this);
        mRef = nullptr;
    }
}

Integer WeakReferenceImpl::AddRef(
    /* [in] */ HANDLE id)
{
    return LightRefBase::AddRef(id);
}

Integer WeakReferenceImpl::Release(
    /* [in] */ HANDLE id)
{
    return LightRefBase::Release(id);
}

IInterface* WeakReferenceImpl::Probe(
    /* [in] */ const InterfaceID& iid)
{
    if (iid == IID_IInterface) {
        return (IInterface*)(IWeakReference*)this;
    }
    else if (iid == IID_IWeakReference) {
        return (IWeakReference*)this;
    }

    return nullptr;
}

ECode WeakReferenceImpl::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [in] */ InterfaceID& iid)
{
    if (object == (IInterface*)(IWeakReference*)this) {
        iid = IID_IWeakReference;
    }
    else {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode WeakReferenceImpl::Resolve(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ AutoPtr<IInterface>& object)
{
    if (mObject && mRef->AttemptIncStrong(&object)) {
        *reinterpret_cast<IInterface**>(&object) = mObject->Probe(iid);
        if (object == nullptr) {
            mObject->Release();
        }
    }
    else {
        object = nullptr;
    }
    return NOERROR;
}

} // namespace como
