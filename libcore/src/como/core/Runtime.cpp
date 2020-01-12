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

#include "como/core/AutoLock.h"
#include "como/core/Runtime.h"
#include "como/core/Thread.h"
#include "como/util/CArrayList.h"
#include "libcore/io/Libcore.h"
#include "jing/system/OsConstants.h"
#include <comolog.h>

using como::util::CArrayList;
using como::util::IID_IList;
using libcore::io::Libcore;
using jing::system::OsConstants;

namespace como {
namespace core {

COMO_INTERFACE_IMPL_1(Runtime, SyncObject, IRuntime);

Runtime::Runtime()
{
    CArrayList::New(IID_IList, (IInterface**)&mShutdownHooks);
}

void Runtime::NativeExit(
    /* [in] */ Integer status)
{
    exit(status);
}

ECode Runtime::GetRuntime(
    /* [out] */ IRuntime** runtime)
{
    VALIDATE_NOT_NULL(runtime);

    static AutoPtr<IRuntime> sCurrentRuntime = new Runtime();
    *runtime = sCurrentRuntime;
    REFCOUNT_ADD(*runtime);
    return NOERROR;
}

ECode Runtime::Exit(
    /* [in] */ Integer status)
{
    AutoLock lock(this);

    if (!mShuttingDown) {
        mShuttingDown = true;

        Array<IThread*> hooks;
        {
            AutoLock lock(ISynchronize::Probe(mShutdownHooks));
            mShutdownHooks->ToArray((Array<IInterface*>*)&hooks);
        }

        for (Integer i = 0; i < hooks.GetLength(); i++) {
            hooks[i]->Start();
        }

        for (Integer i = 0; i < hooks.GetLength(); i++) {
            hooks[i]->Join();
        }

        NativeExit(status);
    }

    return NOERROR;
}

ECode Runtime::AddShutdownHook(
    /* [in] */ IThread* hook)
{
    if (hook == nullptr) {
        Logger::E("Runtime", "hook == null");
        return E_NULL_POINTER_EXCEPTION;
    }

    if (mShuttingDown) {
        Logger::E("Runtime", "Process already shutting down");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    if (Thread::From(hook)->mStarted) {
        Logger::E("Runtime", "Hook has already been started");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoLock lock(ISynchronize::Probe(mShutdownHooks));

    Boolean constains;
    if (mShutdownHooks->Contains(hook, &constains), constains) {
        Logger::E("Runtime", "Hook already registered.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mShutdownHooks->Add(hook);

    return NOERROR;
}

ECode Runtime::AvailableProcessors(
    /* [out] */ Integer* ncpu)
{
    VALIDATE_NOT_NULL(ncpu);

    Long result;
    Libcore::GetOs()->Sysconf(OsConstants::_SC_NPROCESSORS_CONF_, &result);
    *ncpu = result;
    return NOERROR;
}

}
}
