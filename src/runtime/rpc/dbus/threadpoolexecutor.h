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

//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
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

#ifndef __THREADPOOLEXECUTOR_H__
#define __THREADPOOLEXECUTOR_H__

#include "comoptr.h"
#include "comoref.h"
#include "util/arraylist.h"
#include "util/mutex.h"

namespace ccm {

class ThreadPoolExecutor
    : public LightRefBase
{
public:
    class Runnable
        : public LightRefBase
    {
    public:
        virtual ECode Run() = 0;
    };

private:
    class Worker
        : public LightRefBase
    {
    public:
        Worker(
            /* [in] */ Runnable* task,
            /* [in] */ ThreadPoolExecutor* owner);

        ECode Run();

    public:
        pthread_t mThread;
        AutoPtr<Runnable> mTask;
        ThreadPoolExecutor* mOwner;
        Mutex mLock;
    };

public:
    static AutoPtr<ThreadPoolExecutor> GetInstance();

    ECode RunTask(
        /* [in] */ Runnable* task);

    ECode StopTask(
        /* [in] */ Runnable* task);

private:
    static void* ThreadEntry(void* arg);

private:
    static AutoPtr<ThreadPoolExecutor> sInstance;
    static Mutex sInstanceLock;

    Mutex mMainLock;
    ArrayList<Worker*> mWorkers;
};

}

#endif // __THREADPOOLEXECUTOR_H__
