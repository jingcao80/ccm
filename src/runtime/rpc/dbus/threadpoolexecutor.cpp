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

#include "threadpoolexecutor.h"
#include "util/ccmlogger.h"
#include <assert.h>
#include <errno.h>
#include <signal.h>

namespace ccm {

//-------------------------------------------------------------------------

ThreadPoolExecutor::Worker::Worker(
    /* [in] */ Runnable* task,
    /* [in] */ ThreadPoolExecutor* owner)
    : mThread(0)
    , mTask(task)
    , mOwner(owner)
{}

ECode ThreadPoolExecutor::Worker::Run()
{
    return mTask->Run();
}

//-------------------------------------------------------------------------

AutoPtr<ThreadPoolExecutor> ThreadPoolExecutor::sInstance;
Mutex ThreadPoolExecutor::sInstanceLock;

AutoPtr<ThreadPoolExecutor> ThreadPoolExecutor::GetInstance()
{
    {
        Mutex::AutoLock lock(sInstanceLock);
        if (sInstance == NULL) {
            sInstance = new ThreadPoolExecutor();
        }
    }
    return sInstance;
}

ECode ThreadPoolExecutor::RunTask(
    /* [in] */ Runnable* task)
{
    AutoPtr<Worker> w = new Worker(task, this);
    {
        Mutex::AutoLock lock(w->mLock);

        pthread_attr_t threadAddr;
        pthread_attr_init(&threadAddr);
        pthread_attr_setdetachstate(&threadAddr, PTHREAD_CREATE_DETACHED);

        pthread_t thread;
        int ret = pthread_create(&thread, &threadAddr, ThreadPoolExecutor::ThreadEntry, (void*)w);
        if (ret != 0) {
            return E_RUNTIME_EXCEPTION;
        }

        w->mThread = thread;
        {
            Mutex::AutoLock lock(mMainLock);
            mWorkers.Add(w);
        }
    }
    return NOERROR;
}

ECode ThreadPoolExecutor::StopTask(
    /* [in] */ Runnable* task)
{
    Mutex::AutoLock _l(mMainLock);

    for (Long i = 0; i < mWorkers.GetSize(); i++) {
        Worker* w = mWorkers.Get(i);
        if (w->mTask == task) {
            if (pthread_kill(w->mThread, SIGUSR2) == ESRCH) {
                Logger::E("ThreadPoolExecutor", "pthread_kill failed");
            }
            mWorkers.Remove(i);
            break;
        }
    }
    return NOERROR;
}

void* ThreadPoolExecutor::ThreadEntry(void* arg)
{
    AutoPtr<Worker> w = (Worker*)arg;
    {
        Mutex::AutoLock lock(w->mLock);
        assert(w->mThread != 0);
    }

    ECode ec = w->Run();

    {
        ThreadPoolExecutor* executor = w->mOwner;
        Mutex::AutoLock lock(executor->mMainLock);
        for (Long i = 0; i < executor->mWorkers.GetSize(); ++i) {
            if (executor->mWorkers.Get(i) == w) {
                executor->mWorkers.Remove(i);
                break;
            }
        }
    }

    return reinterpret_cast<void*>(ec);
}

}
