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

#include "como/core/ThreadLocal.h"
#include "comort/system/BlockGuard.h"
#include <comolog.h>

using como::core::ThreadLocal;

namespace comort {
namespace system {

AutoPtr<IBlockGuardPolicy> BlockGuard::GetLAX_POLICY()
{
    static AutoPtr<IBlockGuardPolicy> LAX_POLICY = new Policy();
    return LAX_POLICY;
}

static AutoPtr<IThreadLocal> CreateThreadLocal()
{
    class _ThreadLocal
        : public ThreadLocal
    {
    protected:
        AutoPtr<IInterface> InitialValue() override
        {
            return BlockGuard::GetLAX_POLICY().Get();
        }
    };

    AutoPtr<_ThreadLocal> tl = new _ThreadLocal();
    tl->Constructor();
    return tl.Get();
}

AutoPtr<IThreadLocal> BlockGuard::GetPolicyThreadLocal()
{
    static AutoPtr<IThreadLocal> sThreadPolicy = CreateThreadLocal();
    return sThreadPolicy;
}

ECode BlockGuard::GetThreadPolicy(
    /* [out] */ IBlockGuardPolicy** policy)
{
    VALIDATE_NOT_NULL(policy);

    return GetPolicyThreadLocal()->Get((IInterface**)policy);
}

ECode BlockGuard::SetThreadPolicy(
    /* [in] */ IBlockGuardPolicy* policy)
{
    if (policy == nullptr) {
        Logger::E("BlockGuard", "policy == null");
        return como::core::E_NULL_POINTER_EXCEPTION;
    }
    return GetPolicyThreadLocal()->Set(policy);
}

//-------------------------------------------------------------------------

COMO_INTERFACE_IMPL_1(BlockGuard::Policy, Object, IBlockGuardPolicy);

ECode BlockGuard::Policy::OnWriteToDisk()
{
    return NOERROR;
}

ECode BlockGuard::Policy::OnReadFromDisk()
{
    return NOERROR;
}

ECode BlockGuard::Policy::OnNetwork()
{
    return NOERROR;
}

ECode BlockGuard::Policy::OnUnbufferedIO()
{
    return NOERROR;
}

ECode BlockGuard::Policy::GetPolicyMask(
    /* [out] */ Integer* mask)
{
    VALIDATE_NOT_NULL(mask);

    *mask = 0;
    return NOERROR;
}

}
}
