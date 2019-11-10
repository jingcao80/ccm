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
 * Copyright (C) 2007 The Android Open Source Project
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

#ifndef __COMO_MUTEX_H__
#define __COMO_MUTEX_H__

#include "comotypes.h"
#include <atomic>

namespace como {

class Mutex
{
public:
    class AutoLock
    {
    public:
        explicit AutoLock(
            /* [in] */ Mutex& mutex);

        explicit AutoLock(
            /* [in] */ Mutex* mutex);

        ~AutoLock();

    private:
        Mutex& mLock;
    };

public:
    inline Mutex(
        /* [in] */ Boolean recursive = false);

    void Lock();

    void Unlock();

private:
    friend class Condition;

    // 0 is unheld, 1 is held.
    std::atomic<int32_t> mState { 0 };
    // Exclusive owner.
    volatile uint64_t mExclusiveOwner = 0;
    // Number of waiting contenders.
    std::atomic<int32_t> mNumContenders { 0 };
    const Boolean mRecursive;
    unsigned int mRecursionCount = 0;
};

inline Mutex::AutoLock::AutoLock(
    /* [in] */ Mutex& mutex)
    : mLock(mutex)
{
    mLock.Lock();
}

inline Mutex::AutoLock::AutoLock(
    /* [in] */ Mutex* mutex)
    : mLock(*mutex)
{
    mLock.Lock();
}

inline Mutex::AutoLock::~AutoLock()
{
    mLock.Unlock();
}

inline Mutex::Mutex(
    /* [in] */ Boolean recursive)
    : mRecursive(recursive)
{}

//----------------------------------------------------------

class Condition
{
public:
    explicit Condition(
        /* [in] */ Mutex& mutex);

    void Wait();

    Boolean TimedWait(
        /* [in] */ int64_t ms,
        /* [in] */ int32_t ns);

    void Signal();

    void SignalAll();

private:
    // The Mutex being used by waiters. It is an error to mix condition variables between different
    // Mutexes.
    Mutex& mGuard;
    // A counter that is modified by signals and broadcasts. This ensures that when a waiter gives up
    // their Mutex and another thread takes it and signals, the waiting thread observes that sequence_
    // changed and doesn't enter the wait. Modified while holding guard_, but is read by futex wait
    // without guard_ held.
    std::atomic<int32_t> mSequence;
    // Number of threads that have come into to wait, not the length of the waiters on the futex as
    // waiters may have been requeued onto guard_. Guarded by guard_.
    volatile int32_t mNumWaiters;
};

inline Condition::Condition(
    /* [in] */ Mutex& mutex)
    : mGuard(mutex)
    , mNumWaiters(0)
{}

} // namespace como

#endif // __COMO_MUTEX_H__
