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

#include "comotypes.h"

namespace como {

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

#if !defined(NDEBUG)
    __asm__ __volatile__(
        "pushq  %%rbx;\n"
        ::
    );
#endif

    __asm__ __volatile__(
    "push_int_parames:\n"
        "movq   %2, %%rax;\n"
        "movl   %3, %%ebx;\n"
        "testl  %%ebx, %%ebx;\n"
        "jz     push_fp_params;\n"
    "set_reg_rdi:\n"
        "movq   (%%rax), %%rdi;\n"
        "addq   $8, %%rax;\n"
        "subl   $1, %%ebx;\n"
        "testl  %%ebx, %%ebx;\n"
        "jz     push_fp_params;\n"
    "set_reg_rsi:\n"
        "movq   (%%rax), %%rsi;\n"
        "addq   $8, %%rax;\n"
        "subl   $1, %%ebx;\n"
        "testl  %%ebx, %%ebx;\n"
        "jz     push_fp_params;\n"
    "set_reg_rdx:\n"
        "movq   (%%rax), %%rdx;\n"
        "addq   $8, %%rax;\n"
        "subl   $1, %%ebx;\n"
        "testl  %%ebx, %%ebx;\n"
        "jz     push_fp_params;\n"
    "set_reg_rcx:\n"
        "movq   (%%rax), %%rcx;\n"
        "addq   $8, %%rax;\n"
        "subl   $1, %%ebx;\n"
        "testl  %%ebx, %%ebx;\n"
        "jz     push_fp_params;\n"
    "set_reg_r8:\n"
        "movq   (%%rax), %%r8;\n"
        "addq   $8, %%rax;\n"
        "subl   $1, %%ebx;\n"
        "testl  %%ebx, %%ebx;\n"
        "jz     push_fp_params;\n"
    "set_reg_r9:\n"
        "movq   (%%rax), %%r9;\n"
    "push_fp_params:\n"
        "movq   %4, %%rax;\n"
        "movl   %5, %%ebx;\n"
        "testl  %%ebx, %%ebx;\n"
        "jz     push_stk_params;\n"
    "set_reg_xmm0:\n"
        "movsd  (%%rax), %%xmm0;\n"
        "addq   $8, %%rax;\n"
        "subl   $1, %%ebx;\n"
        "testl  %%ebx, %%ebx;\n"
        "jz     push_stk_params;\n"
    "set_reg_xmm1:\n"
        "movsd  (%%rax), %%xmm1;\n"
        "addq   $8, %%rax;\n"
        "subl   $1, %%ebx;\n"
        "testl  %%ebx, %%ebx;\n"
        "jz     push_stk_params;\n"
    "set_reg_xmm2:\n"
        "movsd  (%%rax), %%xmm2;\n"
        "addq   $8, %%rax;\n"
        "subl   $1, %%ebx;\n"
        "testl  %%ebx, %%ebx;\n"
        "jz     push_stk_params;\n"
    "set_reg_xmm3:\n"
        "movsd  (%%rax), %%xmm3;\n"
        "addq   $8, %%rax;\n"
        "subl   $1, %%ebx;\n"
        "testl  %%ebx, %%ebx;\n"
        "jz     push_stk_params;\n"
    "set_reg_xmm4:\n"
        "movsd  (%%rax), %%xmm4;\n"
        "addq   $8, %%rax;\n"
        "subl   $1, %%ebx;\n"
        "testl  %%ebx, %%ebx;\n"
        "jz     push_stk_params;\n"
    "set_reg_xmm5:\n"
        "movsd  (%%rax), %%xmm5;\n"
        "addq   $8, %%rax;\n"
        "subl   $1, %%ebx;\n"
        "testl  %%ebx, %%ebx;\n"
        "jz     push_stk_params;\n"
    "set_reg_xmm6:\n"
        "movsd  (%%rax), %%xmm6;\n"
        "addq   $8, %%rax;\n"
        "subl   $1, %%ebx;\n"
        "testl  %%ebx, %%ebx;\n"
        "jz     push_stk_params;\n"
    "set_reg_xmm7:\n"
        "movsd  (%%rax), %%xmm7;\n"
    "push_stk_params:\n"
        "movq   %6, %%rax;\n"
        "movl   %7, %%ebx;\n"
    "push_param:\n"
        "testl  %%ebx, %%ebx;\n"
        "jz     do_call;\n"
        "pushq  (%%rax);\n"
        "addq   $8, %%rax;\n"
        "subl   $1, %%ebx;\n"
        "jmp    push_param;\n"
    "do_call:\n"
        "movq   %1, %%rax;\n"
        "call   *%%rax;\n"
        "movl   %%eax, %0;\n"
        : "=r" (ret)
        : "m" (func)
        , "m" (intData)
        , "m" (intDataSize)
        , "m" (fpData)
        , "m" (fpDataSize)
        , "m" (stkData)
        , "m" (stkDataSize)
    );

#if !defined(NDEBUG)
    __asm__ __volatile__(
        "popq   %%rbx;\n"
        "movq   %%rbp, %%rsp;\n"
        ::
    );
#endif

    return ret;
}

} // namespace como
