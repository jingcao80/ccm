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

#include "como/core/CThread.h"
#include "como/core/volatile.h"
#include "como/util/concurrent/locks/LockSupport.h"

using como::core::CThread;

namespace como {
namespace util {
namespace concurrent {
namespace locks {

void LockSupport::SetBlocker(
    /* [in] */ IThread* t,
    /* [in] */ IInterface* arg)
{
    PUT_OBJECT(CThread::From(t), mParkBlocker, arg);
}

ECode LockSupport::Unpark(
    /* [in] */ IThread* thread)
{
    if (thread != nullptr) {
        return thread->Unpark();
    }
    return NOERROR;
}

ECode LockSupport::Park(
    /* [in] */ IInterface* blocker)
{
    AutoPtr<IThread> t;
    CThread::GetCurrentThread(&t);
    SetBlocker(t, blocker);
    t->ParkFor(0ll);
    SetBlocker(t, nullptr);
    return NOERROR;
}

}
}
}
}
