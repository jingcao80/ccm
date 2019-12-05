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

// EXTERN_C ECode invoke(
//     /* [in] */ HANDLE func,
//     /* [in] */ Byte* params,
//     /* [in] */ Integer paramNum,
//     /* [in] */ Integer stackParamNum,
//     /* [in] */ struct ParameterInfo* paramInfos);

    .text
    .align 8;
    .global invoke;

invoke:
    pushq   %r10;
    pushq   %r11;
    pushq   %r12;
    pushq   %rbx;
    pushq   %rbp;
    movq    %rsp, %rbp;
    jmp     main;

get_next_param:
    addq    $20, %r10;              // jump to next paramInfos, sizeof(struct ParameterInfo) is #20
    movl    (%r10), %r11d;          // paramInfos->mSize
    cmpl    $8, %r11d;              // paramInfos->mSize == 8 ?
    je      eight_bytes_alignment;
    jmp     set_params;
eight_bytes_alignment:
    movq    %rax, %r11;
    andq    $7, %r11;
    cmpq    $0, %r11;
    je      do_not_adjust
    addq    $4, %rax;
do_not_adjust:
    jmp     set_params;

main:
    subq    $64, %rsp;
    movq    %rdi, -8(%rbp);         // push "func" value
    movq    %rsi, -16(%rbp);        // push "params" value
    movl    %edx, -20(%rbp);        // push "paramNum" value
    movq    %r8, -32(%rbp);         // push "paramInfos" value
    movl    $0, -40(%rbp);          // integral paramNum
    movl    $0, -44(%rbp);          // floating point paramNum
    movl    %ecx, -48(%rbp);        // push "stackParamNum" value

alloc_stack:
    cmpl    $0, %ecx;
    je      set_this;
    movq    $16, %rax;
    mulq    %rcx;
    subq    %rax, %rsp;
    movq    %rsp, %r12;

set_this:
    movq    -16(%rbp), %rax;        // "params"
    movl    -20(%rbp), %ebx;        // "paramNum"
    movq    -32(%rbp), %r10;        // "paramInfos"
    movq    (%rax), %rdi;           // "params[0]"
    addq    $8, %rax;               // "params + 8"
    movl    $1, -40(%rbp);          // next integral paramNum = 1
    subl    $1, %ebx;               // paramNum -= 1

set_params:
    testl   %ebx, %ebx;             // paramNum == 0 ?
    jz      call_func;
    movl    4(%r10), %r11d;         // paramInfos->mNumberType
    cmpl    $0, %r11d;              // paramInfos->mNumberType == NUMBER_TYPE_INTEGER ?
    je      set_integral_param;
    jmp     set_float_point_param;

set_integral_param:
    movl    -40(%rbp), %r11d;       // next integral paramNum
    cmpl    $1, %r11d;              // next integral paramNum == 1 ?
    je      set_reg_rsi;
    cmpl    $2, %r11d;
    je      set_reg_rdx;
    cmpl    $3, %r11d;
    je      set_reg_rcx;
    cmpl    $4, %r11d;
    je      set_reg_r8;
    cmpl    $5, %r11d;
    je      set_reg_r9;
    jmp     save_param_to_stack;

set_reg_rsi:
    movl    (%r10), %r11d;          // paramInfos->mSize
    cmpl    $8, %r11d;              // paramInfos->mSize == 8 ?
    je      rsi_eight_bytes;
    movl    (%rax), %esi;           // params[0] -> %esi
    addq    $4, %rax;               // next params
    movl    $2, -40(%rbp);          // next integral paramNum = 2
    subl    $1, %ebx;               // paramNum -= 1
    jmp     get_next_param;
rsi_eight_bytes:
    movq    (%rax), %rsi;           // params[0] -> %rsi
    addq    $8, %rax;               // next params
    movl    $2, -40(%rbp);          // next integral paramNum = 2
    subl    $1, %ebx;               // paramNum -= 1
    jmp     get_next_param;

set_reg_rdx:
    movl    (%r10), %r11d;
    cmpl    $8, %r11d;
    je      rdx_eight_bytes;
    movl    (%rax), %edx;
    addq    $4, %rax;
    movl    $3, -40(%rbp);
    subl    $1, %ebx;
    jmp     get_next_param;
rdx_eight_bytes:
    movq    (%rax), %rdx;
    addq    $8, %rax;
    movl    $3, -40(%rbp);
    subl    $1, %ebx;
    jmp     get_next_param;

set_reg_rcx:
    movl    (%r10), %r11d;
    cmpl    $8, %r11d;
    je      rcx_eight_bytes;
    movl    (%rax), %ecx;
    addq    $4, %rax;
    movl    $4, -40(%rbp);
    subl    $1, %ebx;
    jmp     get_next_param;
rcx_eight_bytes:
    movq    (%rax), %rcx;
    addq    $8, %rax;
    movl    $4, -40(%rbp);
    subl    $1, %ebx;
    jmp     get_next_param;

set_reg_r8:
    movl    (%r10), %r11d;
    cmpl    $8, %r11d;
    je      r8_eight_bytes;
    movl    (%rax), %r8d;
    addq    $4, %rax;
    movl    $5, -40(%rbp);
    subl    $1, %ebx;
    jmp     get_next_param;
r8_eight_bytes:
    movq    (%rax), %r8;
    addq    $8, %rax;
    movl    $5, -40(%rbp);
    subl    $1, %ebx;
    jmp     get_next_param;

set_reg_r9:
    movl    (%r10), %r11d;
    cmpl    $8, %r11d;
    je      r9_eight_bytes;
    movl    (%rax), %r9d;
    addq    $4, %rax;
    movl    $6, -40(%rbp);
    subl    $1, %ebx;
    jmp     get_next_param;
r9_eight_bytes:
    movq    (%rax), %r9;
    addq    $8, %rax;
    movl    $6, -40(%rbp);
    subl    $1, %ebx;
    jmp     get_next_param;

set_float_point_param:
    movl    -44(%rbp), %r11d;       // floating point paramNum
    cmpl    $0, %r11d;
    je      set_reg_xmm0;           // next floating point paramNum == 1 ?
    cmpl    $1, %r11d;
    je      set_reg_xmm1;
    cmpl    $2, %r11d;
    je      set_reg_xmm2;
    cmpl    $3, %r11d;
    je      set_reg_xmm3;
    cmpl    $4, %r11d;
    je      set_reg_xmm4;
    cmpl    $5, %r11d;
    je      set_reg_xmm5;
    cmpl    $6, %r11d;
    je      set_reg_xmm6;
    cmpl    $7, %r11d;
    je      set_reg_xmm7;
    jmp     save_param_to_stack;

set_reg_xmm0:
    movl    (%r10), %r11d;          // paramInfos->mSize
    cmpl    $8, %r11d;              // paramInfos->mSize == 8 ?
    je      xmm0_eight_bytes;
    movss   (%rax), %xmm0;          // params[0] -> %xmm0
    addq    $4, %rax;               // next params
    movl    $1, -44(%rbp);          // next floating point paramNum = 2
    subl    $1, %ebx;               // paramNum -= 1
    jmp     get_next_param;
xmm0_eight_bytes:
    movsd   (%rax), %xmm0;          // params[0] -> %xmm0
    addq    $8, %rax;               // next params
    movl    $1, -44(%rbp);          // next floating point paramNum = 2
    subl    $1, %ebx;               // paramNum -= 1
    jmp     get_next_param;

set_reg_xmm1:
    movl    (%r10), %r11d;
    cmpl    $8, %r11d;
    je      xmm1_eight_bytes;
    movss   (%rax), %xmm1;
    addq    $4, %rax;
    movl    $2, -44(%rbp);
    subl    $1, %ebx;
    jmp     get_next_param;
xmm1_eight_bytes:
    movsd   (%rax), %xmm1;
    addq    $8, %rax;
    movl    $2, -44(%rbp);
    subl    $1, %ebx;
    jmp     get_next_param;

set_reg_xmm2:
    movl    (%r10), %r11d;
    cmpl    $8, %r11d;
    je      xmm2_eight_bytes;
    movss   (%rax), %xmm2;
    addq    $4, %rax;
    movl    $3, -44(%rbp);
    subl    $1, %ebx;
    jmp     get_next_param;
xmm2_eight_bytes:
    movsd   (%rax), %xmm2;
    addq    $8, %rax;
    movl    $3, -44(%rbp);
    subl    $1, %ebx;
    jmp     get_next_param;

set_reg_xmm3:
    movl    (%r10), %r11d;
    cmpl    $8, %r11d;
    je      xmm3_eight_bytes;
    movss   (%rax), %xmm3;
    addq    $4, %rax;
    movl    $4, -44(%rbp);
    subl    $1, %ebx;
    jmp     get_next_param;
xmm3_eight_bytes:
    movsd   (%rax), %xmm3;
    addq    $8, %rax;
    movl    $4, -44(%rbp);
    subl    $1, %ebx;
    jmp     get_next_param;

set_reg_xmm4:
    movl    (%r10), %r11d;
    cmpl    $8, %r11d;
    je      xmm4_eight_bytes;
    movss   (%rax), %xmm4;
    addq    $4, %rax;
    movl    $5, -44(%rbp);
    subl    $1, %ebx;
    jmp     get_next_param;
xmm4_eight_bytes:
    movsd   (%rax), %xmm4;
    addq    $8, %rax;
    movl    $5, -44(%rbp);
    subl    $1, %ebx;
    jmp     get_next_param;

set_reg_xmm5:
    movl    (%r10), %r11d;
    cmpl    $8, %r11d;
    je      xmm5_eight_bytes;
    movss   (%rax), %xmm5;
    addq    $4, %rax;
    movl    $6, -44(%rbp);
    subl    $1, %ebx;
    jmp     get_next_param;
xmm5_eight_bytes:
    movsd   (%rax), %xmm5;
    addq    $8, %rax;
    movl    $6, -44(%rbp);
    subl    $1, %ebx;
    jmp     get_next_param;

set_reg_xmm6:
    movl    (%r10), %r11d;
    cmpl    $8, %r11d;
    je      xmm6_eight_bytes;
    movss   (%rax), %xmm6;
    addq    $4, %rax;
    movl    $7, -44(%rbp);
    subl    $1, %ebx;
    jmp     get_next_param;
xmm6_eight_bytes:
    movsd   (%rax), %xmm6;
    addq    $8, %rax;
    movl    $7, -44(%rbp);
    subl    $1, %ebx;
    jmp     get_next_param;

set_reg_xmm7:
    movl    (%r10), %r11d;
    cmpl    $8, %r11d;
    je      xmm7_eight_bytes;
    movss   (%rax), %xmm7;
    addq    $4, %rax;
    movl    $8, -44(%rbp);
    subl    $1, %ebx;
    jmp     get_next_param;
xmm7_eight_bytes:
    movsd   (%rax), %xmm7;
    addq    $8, %rax;
    movl    $8, -44(%rbp);
    subl    $1, %ebx;
    jmp     get_next_param;

save_param_to_stack:
    movl    (%r10), %r11d;
    cmpl    $8, %r11d;
    je      stack_eight_bytes;
    movl    (%rax), %r11d;
    movl    %r11d, (%r12);
    addq    $8, %r12;
    addq    $4, %rax;
    subl    $1, %ebx;
    jmp     get_next_param;
stack_eight_bytes:
    movq    (%rax), %r11;
    movq    %r11, (%r12);
    addq    $8, %r12;
    addq    $8, %rax;
    subl    $1, %ebx;
    jmp     get_next_param;

call_func:
    movq    -8(%rbp), %rax;
    call    *%rax;

return:
    movq    %rbp, %rsp;
    popq    %rbp;
    popq    %rbx;
    popq    %r12;
    popq    %r11;
    popq    %r10;
    ret
