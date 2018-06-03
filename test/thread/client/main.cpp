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

#include "ccm.core.CThread.h"
#include "ccm.core.IThread.h"
#include "ccm.io.IInterruptible.h"
#include <ccmautoptr.h>
#include <stdio.h>

using ccm::core::CThread;
using ccm::core::IThread;
using ccm::core::IID_IThread;

int main(int argv, char** argc)
{
    AutoPtr<IThread> t;
    CThread::New(IID_IThread, (IInterface**)&t);
    printf("==== Create thread %p ====\n", t.Get());

    return 0;
}
