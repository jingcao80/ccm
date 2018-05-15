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

#ifndef __CCM_CONDITION_H__
#define __CCM_CONDITION_H__

#include "mutex.h"

namespace ccm {

class Condition
{
public:
    Condition();

    ~Condition();

    Integer Wait(
        /* [in] */ Mutex& mutex);

    void Signal();

    void SignalAll();

private:
    pthread_cond_t mCond;
};

inline Condition::Condition()
{
    pthread_condattr_t attr;
    pthread_condattr_init(&attr);
    pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
    pthread_cond_init(&mCond, &attr);
    pthread_condattr_destroy(&attr);
}

inline Condition::~Condition()
{
    pthread_cond_destroy(&mCond);
}

inline Integer Condition::Wait(
    /* [in] */ Mutex& mutex)
{
    return pthread_cond_wait(&mCond, &mutex.mMutex);
}

inline void Condition::Signal()
{
    pthread_cond_signal(&mCond);
}

inline void Condition::SignalAll()
{
    pthread_cond_broadcast(&mCond);
}

}

#endif // __CCM_CONDITION_H__
