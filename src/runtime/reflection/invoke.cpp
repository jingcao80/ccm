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
        "push   %%rbx\n"
    "push_int_parames:\n"
        "mov    %2, %%rax\n"
        "mov    %3, %%ebx\n"
        "test   %%ebx, %%ebx\n"
        "jz     push_fp_params\n"
        "mov    (%%rax), %%rdi\n"
        "add    $8, %%rax\n"
        "sub    $1, %%ebx\n"
        "test   %%ebx, %%ebx\n"
        "jz     push_fp_params\n"
        "mov    (%%rax), %%rsi\n"
        "add    $8, %%rax\n"
        "sub    $1, %%ebx\n"
        "test   %%ebx, %%ebx\n"
        "jz     push_fp_params\n"
        "mov    (%%rax), %%rdx\n"
        "add    $8, %%rax\n"
        "sub    $1, %%ebx\n"
        "test   %%ebx, %%ebx\n"
        "jz     push_fp_params\n"
        "mov    (%%rax), %%rcx\n"
        "add    $8, %%rax\n"
        "sub    $1, %%ebx\n"
        "test   %%ebx, %%ebx\n"
        "jz     push_fp_params\n"
        "mov    (%%rax), %%r8\n"
        "add    $8, %%rax\n"
        "sub    $1, %%ebx\n"
        "test   %%ebx, %%ebx\n"
        "jz     push_fp_params\n"
        "mov    (%%rax), %%r9\n"
    "push_fp_params:\n"
        "mov    %4, %%rax\n"
        "mov    %5, %%ebx\n"
        "test   %%ebx, %%ebx\n"
        "jz     push_stk_params\n"
        "movss  (%%rax), %%xmm0\n"
        "add    $8, %%rax\n"
        "sub    $1, %%ebx\n"
        "test   %%ebx, %%ebx\n"
        "jz     push_stk_params\n"
        "movss  (%%rax), %%xmm1\n"
        "add    $8, %%rax\n"
        "sub    $1, %%ebx\n"
        "test   %%ebx, %%ebx\n"
        "jz     push_stk_params\n"
        "movss  (%%rax), %%xmm2\n"
        "add    $8, %%rax\n"
        "sub    $1, %%ebx\n"
        "test   %%ebx, %%ebx\n"
        "jz     push_stk_params\n"
        "movss  (%%rax), %%xmm3\n"
        "add    $8, %%rax\n"
        "sub    $1, %%ebx\n"
        "test   %%ebx, %%ebx\n"
        "jz     push_stk_params\n"
        "movss  (%%rax), %%xmm4\n"
        "add    $8, %%rax\n"
        "sub    $1, %%ebx\n"
        "test   %%ebx, %%ebx\n"
        "jz     push_stk_params\n"
        "movss  (%%rax), %%xmm5\n"
        "add    $8, %%rax\n"
        "sub    $1, %%ebx\n"
        "test   %%ebx, %%ebx\n"
        "jz     push_stk_params\n"
        "movss  (%%rax), %%xmm6\n"
        "add    $8, %%rax\n"
        "sub    $1, %%ebx\n"
        "test   %%ebx, %%ebx\n"
        "jz     push_stk_params\n"
        "movss  (%%rax), %%xmm7\n"
    "push_stk_params:\n"
        "mov    %6, %%rax\n"
        "mov    %7, %%ebx\n"
    "push_param:\n"
        "test   %%ebx, %%ebx\n"
        "jz     do_call\n"
        "push   (%%rax)\n"
        "add    $8, %%rax\n"
        "sub    $1, %%ebx\n"
        "jmp    push_param\n"
    "do_call:\n"
        "mov    %1, %%rax\n"
        "call   *%%rax\n"
        "pop    %%rbx\n"
        "mov    %%rbp, %%rsp\n"
        "mov    %%eax, %0\n"
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
