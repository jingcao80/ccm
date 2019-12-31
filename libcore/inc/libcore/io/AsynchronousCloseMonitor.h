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
 * Copyright (C) 2010 The Android Open Source Project
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

#ifndef __LIBCORE_IO_ASYNCHRONOUSCLOSEMONITOR_H__
#define __LIBCORE_IO_ASYNCHRONOUSCLOSEMONITOR_H__

#include <ccmtypes.h>
#include <pthread.h>

using namespace ccm;

namespace libcore {
namespace io {

class AsynchronousCloseMonitor
{
public:
    AsynchronousCloseMonitor(
        /* [in] */ Integer fd);

    ~AsynchronousCloseMonitor();

    Boolean WasSignaled();

    static void Init();

    static void SignalBlockedThreads(
        /* [in] */ Integer fd);

private:
    AsynchronousCloseMonitor* mPrev;
    AsynchronousCloseMonitor* mNext;
    pthread_t mThread;
    Integer mFd;
    Boolean mSignaled;
};

inline Boolean AsynchronousCloseMonitor::WasSignaled()
{
    return mSignaled;
}

}
}

#endif // __LIBCORE_IO_ASYNCHRONOUSCLOSEMONITOR_H__
