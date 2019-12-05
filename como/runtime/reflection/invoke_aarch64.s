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

// x9:   subroutine return address
// x10:  holds "func" value
// x11:  holds "params" value
// w12:  holds "paramNum" value
// w13:  holds "stackParamNum" value
// x14:  holds "paramInfos" value

invoke:
    sub     sp, sp, #16
    stp     fp, lr, [sp]
    mov     fp, sp
    b       main

get_next_param:
    add     x14, x14, #20               // jump to next paramInfos, sizeof(struct ParameterInfo) is #20
    ldr     w15, [x14]
    cmp     w15, #8
    beq     eight_bytes_alignment
    b       set_params
eight_bytes_alignment:
    and     x15, x11, #0x7
    cmp     x15, #0
    beq     do_not_adjust
    add     x11, x11, #4
do_not_adjust:
    b       set_params

main:
    mov     x10, x0                     // x0 holds "func" value
    mov     x11, x1                     // x1 holds "params" value
    mov     w12, w2                     // w2 holds "paramNum" value
    mov     w13, w3                     // w3 holds "stackParamNum" value
    mov     x14, x4                     // x4 holds "paramInfos" value
    sub     sp, sp, #16
    mov     x15, #0
    str     x15, [sp, #8]               // integral paramNum
    str     x15, [sp]                   // floating point paramNum

alloc_stack:
    cmp     w13, #0
    beq     set_this;
    mov     x15, #16                    // sp should be 16 bytes aligned
    mul     x15, x13, x15
    sub     sp, sp, x15
    mov     x8, sp

set_this:
    ldr     x0, [x11]
    add     x11, x11, #8
    mov     x15, #1
    str     x15, [fp, #-8]              // next integral paramNum = 1
    sub     w12, w12, #1

set_params:
    cmp     w12, #0
    beq     call_func
    ldr     w15, [x14, #4]              // paramInfos->mNumberType
    cmp     w15, #0                     // paramInfos->mNumberType == NUMBER_TYPE_INTEGER ?
    beq     set_integral_param
    b       set_float_point_param

set_integral_param:
    ldr     x15, [fp, #-8]              // next integral paramNum
    cmp     x15, #1                     // next integral paramNum == 1 ?
    beq     set_reg_x1
    cmp     x15, #2
    beq     set_reg_x2
    cmp     x15, #3
    beq     set_reg_x3
    cmp     x15, #4
    beq     set_reg_x4
    cmp     x15, #5
    beq     set_reg_x5
    cmp     x15, #6
    beq     set_reg_x6
    cmp     x15, #7
    beq     set_reg_x7
    b       save_param_to_stack

set_reg_x1:
    ldr     w9, [x14]
    cmp     w9, #4
    ble     set_reg_w1
    ldr     x1, [x11]
    add     x11, x11, #8
    b       set_reg_x1_end
set_reg_w1:
    ldr     w1, [x11]
    add     x11, x11, #4
set_reg_x1_end:
    mov     x15, #2
    str     x15, [fp, #-8]              // next integral paramNum = 2
    sub     w12, w12, #1
    b       get_next_param

set_reg_x2:
    ldr     w9, [x14]
    cmp     w9, #4
    ble     set_reg_w2
    ldr     x2, [x11]
    add     x11, x11, #8
    b       set_reg_x2_end
set_reg_w2:
    ldr     w2, [x11]
    add     x11, x11, #4
set_reg_x2_end:
    mov     x15, #3
    str     x15, [fp, #-8]              // next integral paramNum = 3
    sub     w12, w12, #1
    b       get_next_param

set_reg_x3:
    ldr     w9, [x14]
    cmp     w9, #4
    ble     set_reg_w3
    ldr     x3, [x11]
    add     x11, x11, #8
    b       set_reg_x3_end
set_reg_w3:
    ldr     w3, [x11]
    add     x11, x11, #4
set_reg_x3_end:
    mov     x15, #4
    str     x15, [fp, #-8]              // next integral paramNum = 4
    sub     w12, w12, #1
    b       get_next_param

set_reg_x4:
    ldr     w9, [x14]
    cmp     w9, #4
    ble     set_reg_w4
    ldr     x4, [x11]
    add     x11, x11, #8
    b       set_reg_x4_end
set_reg_w4:
    ldr     w4, [x11]
    add     x11, x11, #4
set_reg_x4_end:
    mov     x15, #5
    str     x15, [fp, #-8]              // next integral paramNum = 5
    sub     w12, w12, #1
    b       get_next_param

set_reg_x5:
    ldr     w9, [x14]
    cmp     w9, #4
    ble     set_reg_w5
    ldr     x5, [x11]
    add     x11, x11, #8
    b       set_reg_x5_end
set_reg_w5:
    ldr     w5, [x11]
    add     x11, x11, #4
set_reg_x5_end:
    mov     x15, #6
    str     x15, [fp, #-8]              // next integral paramNum = 6
    sub     w12, w12, #1
    b       get_next_param

set_reg_x6:
    ldr     w9, [x14]
    cmp     w9, #4
    ble     set_reg_w6
    ldr     x6, [x11]
    add     x11, x11, #8
    b       set_reg_x6_end
set_reg_w6:
    ldr     w6, [x11]
    add     x11, x11, #4
set_reg_x6_end:
    mov     x15, #7
    str     x15, [fp, #-8]              // next integral paramNum = 7
    sub     w12, w12, #1
    b       get_next_param

set_reg_x7:
    ldr     w9, [x14]
    cmp     w9, #4
    ble     set_reg_w7
    ldr     x7, [x11]
    add     x11, x11, #8
    b       set_reg_x7_end
set_reg_w7:
    ldr     w7, [x11]
    add     x11, x11, #4
set_reg_x7_end:
    mov     x15, #8
    str     x15, [fp, #-8]              // next integral paramNum = 8
    sub     w12, w12, #1
    b       get_next_param

set_float_point_param:
    ldr     x15, [fp, #-16]             // next floating point paramNum
    cmp     x15, #0                     // next floating point paramNum == 0 ?
    beq     set_reg_d0
    cmp     x15, #1
    beq     set_reg_d1
    cmp     x15, #2
    beq     set_reg_d2
    cmp     x15, #3
    beq     set_reg_d3
    cmp     x15, #4
    beq     set_reg_d4
    cmp     x15, #5
    beq     set_reg_d5
    cmp     x15, #6
    beq     set_reg_d6
    cmp     x15, #7
    beq     set_reg_d7
    b       save_param_to_stack

set_reg_d0:
    ldr     w9, [x14]
    cmp     w9, #4
    ble     set_reg_s0
    ldr     d0, [x11]
    add     x11, x11, #8
    b       set_reg_d0_end
set_reg_s0:
    ldr     s0, [x11]
    add     x11, x11, #4
set_reg_d0_end:
    mov     x15, #1
    str     x15, [fp, #-16]             // next floating point paramNum = 1
    sub     w12, w12, #1
    b       get_next_param

set_reg_d1:
    ldr     w9, [x14]
    cmp     w9, #4
    ble     set_reg_s1
    ldr     d1, [x11]
    add     x11, x11, #8
    b       set_reg_d1_end
set_reg_s1:
    ldr     s1, [x11]
    add     x11, x11, #4
set_reg_d1_end:
    mov     x15, #2
    str     x15, [fp, #-16]             // next floating point paramNum = 2
    sub     w12, w12, #1
    b       get_next_param

set_reg_d2:
    ldr     w9, [x14]
    cmp     w9, #4
    ble     set_reg_s2
    ldr     d2, [x11]
    add     x11, x11, #8
    b       set_reg_d2_end
set_reg_s2:
    ldr     s2, [x11]
    add     x11, x11, #4
set_reg_d2_end:
    mov     x15, #3
    str     x15, [fp, #-16]             // next floating point paramNum = 3
    sub     w12, w12, #1
    b       get_next_param

set_reg_d3:
    ldr     w9, [x14]
    cmp     w9, #4
    ble     set_reg_s3
    ldr     d3, [x11]
    add     x11, x11, #8
    b       set_reg_d3_end
set_reg_s3:
    ldr     s3, [x11]
    add     x11, x11, #4
set_reg_d3_end:
    mov     x15, #4
    str     x15, [fp, #-16]             // next floating point paramNum = 4
    sub     w12, w12, #1
    b       get_next_param

set_reg_d4:
    ldr     w9, [x14]
    cmp     w9, #4
    ble     set_reg_s4
    ldr     d4, [x11]
    add     x11, x11, #8
    b       set_reg_d4_end
set_reg_s4:
    ldr     s4, [x11]
    add     x11, x11, #4
set_reg_d4_end:
    mov     x15, #5
    str     x15, [fp, #-16]             // next floating point paramNum = 5
    sub     w12, w12, #1
    b       get_next_param

set_reg_d5:
    ldr     w9, [x14]
    cmp     w9, #4
    ble     set_reg_s5
    ldr     d5, [x11]
    add     x11, x11, #8
    b       set_reg_d5_end
set_reg_s5:
    ldr     s5, [x11]
    add     x11, x11, #4
set_reg_d5_end:
    mov     x15, #6
    str     x15, [fp, #-16]             // next floating point paramNum = 6
    sub     w12, w12, #1
    b       get_next_param

set_reg_d6:
    ldr     w9, [x14]
    cmp     w9, #4
    ble     set_reg_s6
    ldr     d6, [x11]
    add     x11, x11, #8
    b       set_reg_d6_end
set_reg_s6:
    ldr     s6, [x11]
    add     x11, x11, #4
set_reg_d6_end:
    mov     x15, #7
    str     x15, [fp, #-16]             // next floating point paramNum = 7
    sub     w12, w12, #1
    b       get_next_param

set_reg_d7:
    ldr     w9, [x14]
    cmp     w9, #4
    ble     set_reg_s7
    ldr     d7, [x11]
    add     x11, x11, #8
    b       set_reg_d7_end
set_reg_s7:
    ldr     s7, [x11]
    add     x11, x11, #4
set_reg_d7_end:
    mov     x15, #8
    str     x15, [fp, #-16]             // next floating point paramNum = 8
    sub     w12, w12, #1
    b       get_next_param

save_param_to_stack:
    ldr     w9, [x14]
    cmp     w9, #4
    ble     four_bytes
x8_need_eight_bytes_alignment:
    and     x15, x8, #0x7
    cmp     x15, #0
    beq     not_adjust_x8
    add     x8, x8, #4
not_adjust_x8:
    ldr     x15, [x11]
    str     x15, [x8]
    add     x11, x11, #8
    add     x8, x8, #8
    sub     w12, w12, #1
    b       get_next_param
four_bytes:
    ldr     w15, [x11]
    str     w15, [x8]
    add     x11, x11, #4
    add     x8, x8, #4
    sub     w12, w12, #1
    b       get_next_param

call_func:
    adr     lr, return_from_func
    br      x10

return_from_func:
    mov     sp, fp
    ldp     fp, lr, [sp]
    add     sp, sp, #16
    ret
