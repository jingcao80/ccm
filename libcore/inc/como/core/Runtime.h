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

#ifndef __COMO_CORE_RUNTIME_H__
#define __COMO_CORE_RUNTIME_H__

#include "como/core/SyncObject.h"
#include "como.core.IRuntime.h"
#include "como.util.IList.h"
#include <comosp.h>

using como::util::IList;

namespace como {
namespace core {

class Runtime
    : public SyncObject
    , public IRuntime
{
public:
    COMO_INTERFACE_DECL();

    static ECode GetRuntime(
        /* [out] */ IRuntime** runtime);

    ECode Exit(
        /* [in] */ Integer status) override;

    ECode AddShutdownHook(
        /* [in] */ IThread* hook) override;

    ECode AvailableProcessors(
        /* [out] */ Integer* ncpu) override;

private:
    Runtime();

    static void NativeExit(
        /* [in] */ Integer status);

private:
    AutoPtr<IList> mShutdownHooks;

    Boolean mShuttingDown = false;
};

}
}

#endif // __COMO_CORE_RUNTIME_H__
