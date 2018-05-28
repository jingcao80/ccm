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

#ifndef __CCM_CORE_GLOBALS_H__
#define __CCM_CORE_GLOBALS_H__

#include <stddef.h>
#include <stdint.h>

#if !defined(__has_feature)
#define __has_feature(x) 0
#endif

#if __has_feature(address_sanitizer)

#include <sanitizer/asan_interface.h>

#else

// #include <valgrind.h>
// #include <memcheck/memcheck.h>

#endif

namespace ccm {
namespace core {

static constexpr size_t KB = 1024;
static constexpr size_t MB = KB * KB;
static constexpr size_t GB = KB * KB * KB;

// System page size. We check this against sysconf(_SC_PAGE_SIZE) at runtime, but use a simple
// compile-time constant so the compiler can generate better code.
static constexpr int kPageSize = 4096;

enum InstructionSet {
    kNone,
    kArm64,
    kX86_64
};

#if defined(__aarch64__)
static constexpr InstructionSet kRuntimeISA = kArm64;
#elif defined(__x86_64__)
static constexpr InstructionSet kRuntimeISA = kX86_64;
#else
static constexpr InstructionSet kRuntimeISA = kNone;
#endif

static constexpr size_t kArm64StackOverflowReservedBytes  = 8192;
static constexpr size_t kX86_64StackOverflowReservedBytes = 8192;

#if __has_feature(address_sanitizer)

constexpr size_t kMemoryToolStackGuardSizeScale = 2;

#else

constexpr size_t kMemoryToolStackGuardSizeScale = 1;

#endif

}
}

#endif // __CCM_CORE_GLOBALS_H__
