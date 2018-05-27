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
#include "core/Thread.h"

namespace ccm {
namespace core {

CCM_INTERFACE_IMPL_1(Thread, Runnable, IThread);

ECode Thread::NativeCreate(
    /* [in] */ Thread* t,
    /* [in] */ Long stackSize,
    /* [in] */ Boolean daemon)
{
    return NativeThread::CreateNativeThread(t, stackSize, daemon);
}

}
}
