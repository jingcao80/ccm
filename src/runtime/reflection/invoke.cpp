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

#include "ccmtypes.h"

namespace ccm {

EXTERN_C ECode invoke(
    /* [in] */ HANDLE func,
    /* [in] */ Long* intData,
    /* [in] */ Integer intDataSize,
    /* [in] */ Double* fpData,
    /* [in] */ Integer fpDataSize,
    /* [in] */ Long* stkData,
    /* [in] */ Integer stkDataSize)
{
    ECode ret;

    __asm__ __volatile__(
        "push   %%rbx;"
    "push_int_parames:"
        "mov    %2, %%rax;"
        "mov    %3, %%ebx;"
        "test   %%ebx, %%ebx;"
        "jz     push_fp_params;"
        "mov    (%%rax), %%rdi;"
        "add    $8, %%rax;"
        "sub    $1, %%ebx;"
        "test   %%ebx, %%ebx;"
        "jz     push_fp_params;"
        "mov    (%%rax), %%rsi;"
        "add    $8, %%rax;"
        "sub    $1, %%ebx;"
        "test   %%ebx, %%ebx;"
        "jz     push_fp_params;"
        "mov    (%%rax), %%rdx;"
        "add    $8, %%rax;"
        "sub    $1, %%ebx;"
        "test   %%ebx, %%ebx;"
        "jz     push_fp_params;"
        "mov    (%%rax), %%rcx;"
        "add    $8, %%rax;"
        "sub    $1, %%ebx;"
        "test   %%ebx, %%ebx;"
        "jz     push_fp_params;"
        "mov    (%%rax), %%r8;"
        "add    $8, %%rax;"
        "sub    $1, %%ebx;"
        "test   %%ebx, %%ebx;"
        "jz     push_fp_params;"
        "mov    (%%rax), %%r9;"
    "push_fp_params:"
        "mov    %4, %%rax;"
        "mov    %5, %%ebx;"
        "test   %%ebx, %%ebx;"
        "jz     push_stk_params;"
        "movss  (%%rax), %%xmm0;"
        "add    $8, %%rax;"
        "sub    $1, %%ebx;"
        "test   %%ebx, %%ebx;"
        "jz     push_stk_params;"
        "movss  (%%rax), %%xmm1;"
        "add    $8, %%rax;"
        "sub    $1, %%ebx;"
        "test   %%ebx, %%ebx;"
        "jz     push_stk_params;"
        "movss  (%%rax), %%xmm2;"
        "add    $8, %%rax;"
        "sub    $1, %%ebx;"
        "test   %%ebx, %%ebx;"
        "jz     push_stk_params;"
        "movss  (%%rax), %%xmm3;"
        "add    $8, %%rax;"
        "sub    $1, %%ebx;"
        "test   %%ebx, %%ebx;"
        "jz     push_stk_params;"
        "movss  (%%rax), %%xmm4;"
        "add    $8, %%rax;"
        "sub    $1, %%ebx;"
        "test   %%ebx, %%ebx;"
        "jz     push_stk_params;"
        "movss  (%%rax), %%xmm5;"
        "add    $8, %%rax;"
        "sub    $1, %%ebx;"
        "test   %%ebx, %%ebx;"
        "jz     push_stk_params;"
        "movss  (%%rax), %%xmm6;"
        "add    $8, %%rax;"
        "sub    $1, %%ebx;"
        "test   %%ebx, %%ebx;"
        "jz     push_stk_params;"
        "movss  (%%rax), %%xmm7;"
    "push_stk_params:"
        "mov    %6, %%rax;"
        "mov    %7, %%ebx;"
    "push_param:"
        "test   %%ebx, %%ebx;"
        "jz     do_call;"
        "push   (%%rax);"
        "add    $8, %%rax;"
        "sub    $1, %%ebx;"
        "jmp    push_param;"
    "do_call:"
        "mov    %1, %%rax;"
        "call   *%%rax;"
        "pop    %%rbx;"
        "mov    %%rbp, %%rsp;"
        "mov    %%eax, %0;"
        : "=r" (ret)
        : "m" (func)
        , "m" (intData)
        , "m" (intDataSize)
        , "m" (fpData)
        , "m" (fpDataSize)
        , "m" (stkData)
        , "m" (stkDataSize)
    );

    return ret;
}

}
