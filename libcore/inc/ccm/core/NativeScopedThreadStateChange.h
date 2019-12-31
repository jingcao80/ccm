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

#ifndef __COMO_CORE_NATIVESCOPEDTHREADSTATECHANGE_H__
#define __COMO_CORE_NATIVESCOPEDTHREADSTATECHANGE_H__

#include "como/core/NativeThreadState.h"

namespace como {
namespace core {

class NativeThread;

// Scoped change into and out of a particular state. Handles Runnable transitions that require
// more complicated suspension checking. The subclasses ScopedObjectAccessUnchecked and
// ScopedObjectAccess are used to handle the change into Runnable to Get direct access to objects,
// the unchecked variant doesn't aid annotalysis.
class ScopedThreadStateChange
{
public:
    ScopedThreadStateChange(
        /* [in] */ NativeThread* self,
        /* [in] */ NativeThreadState newThreadState);

    ~ScopedThreadStateChange();

protected:
    NativeThread* const mSelf;

    const NativeThreadState mThreadState;

private:
    friend class ScopedObjectAccessUnchecked;

    NativeThreadState mOldThreadState = kTerminated;

    const Boolean mExpectedHasNoThread;
};

// Assumes we are already runnable.
class ScopedObjectAccessAlreadyRunnable
{
public:
    NativeThread* Self() const;

protected:
    explicit ScopedObjectAccessAlreadyRunnable(
        /* [in] */ NativeThread* self);

    ~ScopedObjectAccessAlreadyRunnable();

protected:
    // Self thread, can be null.
    NativeThread* const mSelf;
};

inline ScopedObjectAccessAlreadyRunnable::ScopedObjectAccessAlreadyRunnable(
    /* [in] */ NativeThread* self)
    : mSelf(self)
{}

inline ScopedObjectAccessAlreadyRunnable::~ScopedObjectAccessAlreadyRunnable()
{}

inline NativeThread* ScopedObjectAccessAlreadyRunnable::Self() const
{
    return mSelf;
}

// Entry/exit processing for transitions from Native to Runnable (ie within JNI functions).
//
// This class performs the necessary thread state switching to and from Runnable and lets us
// amortize the cost of working out the current thread. Additionally it lets us check (and repair)
// apps that are using a JNIEnv on the wrong thread. The class also decodes and encodes Objects
// into jobjects via methods of this class. Performing this here enforces the Runnable thread state
// for use of Object, thereby inhibiting the Object being modified by GC whilst native or VM code
// is also manipulating the Object.
//
// The destructor transitions back to the previous thread state, typically Native. In this state
// GC and thread suspension may occur.
//
// For annotalysis the subclass ScopedObjectAccess (below) makes it explicit that a shared of
// the mutator_lock_ will be acquired on construction.
class ScopedObjectAccessUnchecked : public ScopedObjectAccessAlreadyRunnable
{
public:
    explicit ScopedObjectAccessUnchecked(
        /* [in] */ NativeThread* self);

    ~ScopedObjectAccessUnchecked();

private:
    // The scoped thread state change makes sure that we are runnable and restores the thread state
    // in the destructor.
    const ScopedThreadStateChange mTsc;
};

inline ScopedObjectAccessUnchecked::ScopedObjectAccessUnchecked(
    /* [in] */ NativeThread* self)
    : ScopedObjectAccessAlreadyRunnable(self)
    , mTsc(self, kRunnable)
{}

inline ScopedObjectAccessUnchecked::~ScopedObjectAccessUnchecked()
{}


// Annotalysis helping variant of the above.
class ScopedObjectAccess : public ScopedObjectAccessUnchecked
{
public:
    explicit ScopedObjectAccess(
        /* [in] */ NativeThread* self);

    ~ScopedObjectAccess();
};

inline ScopedObjectAccess::ScopedObjectAccess(
    /* [in] */ NativeThread* self)
    : ScopedObjectAccessUnchecked(self)
{}

inline ScopedObjectAccess::~ScopedObjectAccess()
{}

}
}

#endif // __COMO_CORE_NATIVESCOPEDTHREADSTATECHANGE_H__
