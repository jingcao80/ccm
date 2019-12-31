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

/*
 * Copyright (C) 2012 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "ccm/core/AutoLock.h"
#include "ccm/core/NativeMapData.h"
#include <inttypes.h>
#include <link.h>
#include <stdio.h>

namespace ccm {
namespace core {

// Format of /proc/<PID>/maps:
//   6f000000-6f01e000 rwxp 00000000 00:0c 16389419   /system/lib/libcomposer.so
static MapEntry* ParseLine(char* line)
{
    uintptr_t start;
    uintptr_t end;
    uintptr_t offset;
    char permissions[5];
    int namePos;
    if (sscanf(line, "%" PRIxPTR "-%" PRIxPTR " %4s %" PRIxPTR " %*x:%*x %*d %n", &start,
            &end, permissions, &offset, &namePos) < 2) {
        return nullptr;
    }

    const char* name = line + namePos;
    size_t nameLen = strlen(name);
    if (nameLen && name[nameLen - 1] == '\n') {
        nameLen -= 1;
    }

    MapEntry* entry = new MapEntry(start, end, offset, name, nameLen);
    if (permissions[0] != 'r') {
        // Any unreadable map will just get a zero load base.
        entry->mLoadBase = 0;
        entry->mLoadBaseRead = true;
    }
    return entry;
}

template<typename T>
static inline bool GetVal(MapEntry* entry, uintptr_t addr, T* store)
{
    if (addr < entry->mStart || addr + sizeof(T) > entry->mEnd) {
        return false;
    }
    // Make sure the address is aligned properly.
    if (addr & (sizeof(T)-1)) {
        return false;
    }
    *store = *reinterpret_cast<T*>(addr);
    return true;
}

static void ReadLoadbase(MapEntry* entry)
{
    entry->mLoadBase = 0;
    entry->mLoadBaseRead = true;
    uintptr_t addr = entry->mStart;
    ElfW(Ehdr) ehdr;
    if (!GetVal<ElfW(Half)>(entry, addr + offsetof(ElfW(Ehdr), e_phnum), &ehdr.e_phnum)) {
        return;
    }
    if (!GetVal<ElfW(Off)>(entry, addr + offsetof(ElfW(Ehdr), e_phoff), &ehdr.e_phoff)) {
        return;
    }
    addr += ehdr.e_phoff;
    for (size_t i = 0; i < ehdr.e_phnum; i++) {
        ElfW(Phdr) phdr;
        if (!GetVal<ElfW(Word)>(entry, addr + offsetof(ElfW(Phdr), p_type), &phdr.p_type)) {
            return;
        }
        if (!GetVal<ElfW(Off)>(entry, addr + offsetof(ElfW(Phdr), p_offset), &phdr.p_offset)) {
            return;
        }
        if (phdr.p_type == PT_LOAD && phdr.p_offset == entry->mOffset) {
            if (!GetVal<ElfW(Addr)>(entry, addr + offsetof(ElfW(Phdr), p_vaddr), &phdr.p_vaddr)) {
                return;
            }
            entry->mLoadBase = phdr.p_vaddr;
            return;
        }
        addr += sizeof(phdr);
    }
}

MapData::~MapData()
{
    for (auto* entry : mEntries) {
        delete entry;
    }
    mEntries.clear();
}

// Find the containing map info for the PC.
const MapEntry* MapData::Find(
    /* [in] */ uintptr_t pc,
    /* [in] */ uintptr_t* relPc)
{
    MapEntry pcEntry(pc);

    AutoLock lock(mLock);

    auto it = mEntries.find(&pcEntry);
    if (it == mEntries.end()) {
        ReadMaps();
    }
    it = mEntries.find(&pcEntry);
    if (it == mEntries.end()) {
        return nullptr;
    }

    MapEntry *entry = *it;
    if (!entry->mLoadBaseRead) {
        ReadLoadbase(entry);
    }
    if (relPc) {
        *relPc = pc - entry->mStart + entry->mLoadBase;
    }
    return entry;
}

bool MapData::ReadMaps()
{
    FILE* fp = fopen("/proc/self/maps", "re");
    if (fp == nullptr) {
        return false;
    }

    Array<char> buffer(1024);
    while (fgets(buffer.GetPayload(), buffer.GetLength(), fp) != nullptr) {
        MapEntry* entry = ParseLine(buffer.GetPayload());
        if (entry == nullptr) {
            fclose(fp);
            return false;
        }

        auto it = mEntries.find(entry);
        if (it == mEntries.end()) {
            mEntries.insert(entry);
        }
        else {
            delete entry;
        }
    }
    fclose(fp);
    return true;
}

}
}
