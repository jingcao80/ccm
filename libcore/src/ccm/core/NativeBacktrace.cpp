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

#include "ccm/core/NativeBacktrace.h"
#include "ccm/core/NativeMapData.h"
#include <dlfcn.h>
#include <inttypes.h>
#include <stdio.h>
#include <unwind.h>

#define PAD_PTR "016" PRIxPTR

extern "C" char* __cxa_demangle(const char*, char*, size_t*, int*);

namespace ccm {
namespace core {

static MapData sMapData;
// static const MapEntry* sCurrentCodeMap = nullptr;

struct stack_crawl_state_t
{
    uintptr_t* frames;
    size_t frame_count;
    size_t cur_frame = 0;

    stack_crawl_state_t(uintptr_t* frames, size_t frame_count)
        : frames(frames), frame_count(frame_count)
    {}
};

static _Unwind_Reason_Code trace_function(_Unwind_Context* context, void* arg)
{
    stack_crawl_state_t* state = static_cast<stack_crawl_state_t*>(arg);

    uintptr_t ip = _Unwind_GetIP(context);

    // The instruction pointer is pointing at the instruction after the return
    // call on all architectures.
    // Modify the pc to point at the real function.
    if (ip != 0) {
#if defined(__arm__)
        // If the ip is suspiciously low, do nothing to avoid a segfault trying
        // to access this memory.
        if (ip >= 4096) {
            // Check bits [15:11] of the first halfword assuming the instruction
            // is 32 bits long. If the bits are any of these values, then our
            // assumption was correct:
            //  b11101
            //  b11110
            //  b11111
            // Otherwise, this is a 16 bit instruction.
            uint16_t value = (*reinterpret_cast<uint16_t*>(ip - 2)) >> 11;
            if (value == 0x1f || value == 0x1e || value == 0x1d) {
                ip -= 4;
            }
            else {
                ip -= 2;
            }
        }
#elif defined(__aarch64__)
        // All instructions are 4 bytes long, skip back one instruction.
        ip -= 4;
#elif defined(__i386__) || defined(__x86_64__)
        // It's difficult to decode exactly where the previous instruction is,
        // so subtract 1 to estimate where the instruction lives.
        ip--;
#endif
    }

    state->frames[state->cur_frame++] = ip;
    return (state->cur_frame >= state->frame_count) ? _URC_END_OF_STACK : _URC_NO_REASON;
}

size_t GetBacktrace(
    /* [in] */ uintptr_t* frames,
    /* [in] */ size_t frameCount)
{
    stack_crawl_state_t state(frames, frameCount);
    _Unwind_Backtrace(trace_function, &state);
    return state.cur_frame;
}

String DumpBacktrace(
    /* [in] */ const uintptr_t* frames,
    /* [in] */ size_t frame_count)
{
    String str;

    for (size_t frame_num = 0; frame_num < frame_count; frame_num++) {
        uintptr_t offset = 0;
        const char* symbol = nullptr;

        Dl_info info;
        if (dladdr(reinterpret_cast<void*>(frames[frame_num]), &info) != 0) {
            offset = reinterpret_cast<uintptr_t>(info.dli_saddr);
            symbol = info.dli_sname;
        }
        else {
            info.dli_fname = nullptr;
        }

        uintptr_t rel_pc = offset;
        const MapEntry* entry = sMapData.Find(frames[frame_num], &rel_pc);

        const char* soname = (entry != nullptr) ? entry->mName.string() : info.dli_fname;
        if (soname == nullptr) {
            soname = "<unknown>";
        }
        char buf[1024];
        if (symbol != nullptr) {
            char* demangled_symbol = __cxa_demangle(symbol, nullptr, nullptr, nullptr);
            const char* best_name = (demangled_symbol != nullptr) ? demangled_symbol : symbol;

            snprintf(buf, sizeof(buf),
                    "          #%02zd  pc %" PAD_PTR "  %s (%s+%" PRIuPTR ")\n", frame_num,
                    rel_pc, soname, best_name, frames[frame_num] - offset);
            free(demangled_symbol);
        }
        else {
            snprintf(buf, sizeof(buf),
                    "          #%02zd  pc %" PAD_PTR "  %s\n", frame_num, rel_pc, soname);
        }
        str += buf;
    }

    return str;
}

}
}
