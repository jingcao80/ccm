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

#ifndef __CCM_MUTEX_H__
#define __CCM_MUTEX_H__

#include "ccmtypes.h"
#include <pthread.h>

namespace ccm {

class Mutex
{
public:
    enum {
        PRIVATE = 0,
        SHARED = 1
    };

    class AutoLock
    {
    public:
        inline explicit AutoLock(
            /* [in] */ Mutex& mutex);

        inline explicit AutoLock(
            /* [in] */ Mutex* mutex);

        inline ~AutoLock();

    private:
        Mutex& mLock;
    };

public:
    inline Mutex();

    inline Mutex(
        /* [in] */ Integer type);

    inline ~Mutex();

    inline Integer Lock();

    inline Integer Unlock();

    inline Integer TryLock();

private:
    Mutex(
        /* [in] */ const Mutex&);

    Mutex& operator=(
        /* [in] */ const Mutex&);

private:
    friend class Condition;

    pthread_mutex_t mMutex;
};

Mutex::AutoLock::AutoLock(
    /* [in] */ Mutex& mutex)
    : mLock(mutex)
{
    mLock.Lock();
}

Mutex::AutoLock::AutoLock(
    /* [in] */ Mutex* mutex)
    : mLock(*mutex)
{
    mLock.Lock();
}

Mutex::AutoLock::~AutoLock()
{
    mLock.Unlock();
}

Mutex::Mutex()
{
    pthread_mutex_init(&mMutex, nullptr);
}

Mutex::Mutex(
    /* [in] */ Integer type)
{
    if (type == SHARED) {
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
        pthread_mutex_init(&mMutex, &attr);
        pthread_mutexattr_destroy(&attr);
    }
    else {
        pthread_mutex_init(&mMutex, nullptr);
    }
}

Mutex::~Mutex()
{
    pthread_mutex_destroy(&mMutex);
}

Integer Mutex::Lock()
{
    return pthread_mutex_lock(&mMutex);
}

Integer Mutex::Unlock()
{
    return pthread_mutex_unlock(&mMutex);
}

Integer Mutex::TryLock()
{
    return pthread_mutex_trylock(&mMutex);
}

}

#endif // __CCM_MUTEX_H__
