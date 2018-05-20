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

#include "core/NativeThread.h"
#include <ccmlogger.h>

namespace ccm {
namespace core {

Boolean NativeThread::sIsStarted = false;
pthread_key_t NativeThread::sPthreadKeySelf;

NativeThread* NativeThread::Current()
{
    if (!sIsStarted) {
        return nullptr;
    }
    else {
        void* thread = pthread_getspecific(NativeThread::sPthreadKeySelf);
        return reinterpret_cast<NativeThread*>(thread);
    }
}

String NativeThread::ShortDump() const
{
    return String();
}

void NativeThread::ThreadExitCallback(
    /* [in] */ void* arg)
{
    NativeThread* self = reinterpret_cast<NativeThread*>(arg);
    if (self->mTls32.mThreadExitCheckCount == 0) {
        Logger::W("NativeThread", "Native thread exiting without having called DetachCurrentThread (maybe it's "
                "going to use a pthread_key_create destructor?): %s", self->ShortDump().string());
        CHECK(sIsStarted);
        pthread_setspecific(NativeThread::sPthreadKeySelf, self);
        self->mTls32.mThreadExitCheckCount = 1;
    }
    else {
        Logger::E("NativeThread", "Native thread exited without calling DetachCurrentThread: %s",
                self->ShortDump().string());
    }
}

void NativeThread::Startup()
{
    CHECK(!sIsStarted);
    sIsStarted = true;

    pthread_key_create(&sPthreadKeySelf, NativeThread::ThreadExitCallback);

    if (pthread_getspecific(sPthreadKeySelf) != nullptr) {
        Logger::E("NativeThread", "Newly created pthread TLS slot is not nullptr");
    }
}

void NativeThread::Notify()
{
    NativeThread* self = Current();
    NativeMutex::AutoLock lock(self, *mWaitMutex);
    NotifyLocked(self);
}

void NativeThread::NotifyLocked(
    /* [in] */ NativeThread* self)
{
    if (mWaitMonitor != nullptr) {
        mWaitCond->Signal(self);
    }
}

void NativeThread::SetHeldMutex(
    /* [in] */ LockLevel level,
    /* [in] */ BaseMutex* mutex)
{
    mTlsPtr.mHeldMutexes[level] = mutex;
}

}
}
