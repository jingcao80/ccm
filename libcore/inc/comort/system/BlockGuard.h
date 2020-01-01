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

#ifndef __COMORT_SYSTEM_BLOCKGUARD_H__
#define __COMORT_SYSTEM_BLOCKGUARD_H__

#include "como.core.IThreadLocal.h"
#include "comort.system.IBlockGuardPolicy.h"
#include <ccmautoptr.h>
#include <ccmobject.h>

using namespace como;
using como::core::IThreadLocal;

namespace comort {
namespace system {

class BlockGuard
{
public:
    class Policy
        : public Object
        , public IBlockGuardPolicy
    {
    public:
        CCM_INTERFACE_DECL();

        ECode OnWriteToDisk() override;

        ECode OnReadFromDisk() override;

        ECode OnNetwork() override;

        ECode OnUnbufferedIO() override;

        ECode GetPolicyMask(
            /* [out] */ Integer* mask) override;
    };

public:
    static ECode GetThreadPolicy(
        /* [out] */ IBlockGuardPolicy** policy);

    static ECode SetThreadPolicy(
        /* [in] */ IBlockGuardPolicy* policy);

    static AutoPtr<IBlockGuardPolicy> GetLAX_POLICY();

private:
    static AutoPtr<IThreadLocal> GetPolicyThreadLocal();

private:
    BlockGuard();
};

}
}

#endif // __COMORT_SYSTEM_BLOCKGUARD_H__
