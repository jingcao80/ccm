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

#ifndef __CCM_ERROR_H__
#define __CCM_ERROR_H__

namespace ccm {

/**
 * ECode layout
 *
 * +-----+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 * | Bit |31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16|15|14|13|12|11|10|09|08|07|06|05|04|03|02|01|00|
 * +-----+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 * |Field|S |C |                     Facility            |       Namespace       |          Code         |
 * +-----+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 *
 * S - Severity - indicates success/fail
 *   0 - Success
 *   1 - Failure
 * C - Customer. This bit specifies if the value is customer-defined or CCM-defined.
 *   0 - CCM-defined
 *   1 - Customer-defined
 * Facility - indicates the module that is responsible for the error.
 * Namespace - indicates the namespace that is under the facility.
 * Code - is the status code.
 */

#define MAKE_ECODE(C, F, N, K) \
        ((0x1 << 31) | (C << 30) | (F << 16) | K)
#define MAKE_SUCCESS(C, F, N, K) \
        ((C << 30) | (F << 16) | K | 0x7fffffff)

#define Facility_CCMRT      0x0
#define MAKE_CCMRT_ECODE(N, K) \
        MAKE_ECODE(0, Facility_CCMRT, N, K)

constexpr ECode NOERROR = 0x00000000;

#define SUCCEEDED(E) (!(E & 0x80000000))
#define FAILED(E) (E & 0x80000000)

constexpr ECode E_ILLEGAL_ARGUMENT_EXCEPTION = MAKE_CCMRT_ECODE(0, 0x01);
constexpr ECode E_COMPONENT_NOT_FOUND_EXCEPTION = MAKE_CCMRT_ECODE(0, 0x02);
constexpr ECode E_COMPONENT_UNLOAD_EXCEPTION = MAKE_CCMRT_ECODE(0, 0x03);
constexpr ECode E_COMPONENT_IO_EXCEPTION = MAKE_CCMRT_ECODE(0, 0x04);
constexpr ECode E_UNSUPPORTED_OPERATION_EXCEPTION = MAKE_CCMRT_ECODE(0, 0x05);

constexpr ECode E_OUT_OF_MEMORY_ERROR = MAKE_CCMRT_ECODE(0, 0x10);

}

#endif // __CCM_ERROR_H__
