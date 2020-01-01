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

#ifndef __COMO_CORE_NATIVEMAPDATA_H__
#define __COMO_CORE_NATIVEMAPDATA_H__

#include "como/core/SyncObject.h"
#include <set>

namespace como {
namespace core {

struct MapEntry
{
    MapEntry(
        /* [in] */ uintptr_t start,
        /* [in] */ uintptr_t end,
        /* [in] */ uintptr_t offset,
        /* [in] */ const char* name,
        /* [in] */ size_t nameLen)
        : mStart(start)
        , mEnd(end)
        , mOffset(offset)
        , mName(name, nameLen)
    {}

    MapEntry(
        /* [in] */ uintptr_t pc)
        : mStart(pc)
        , mEnd(pc)
    {}

    uintptr_t mStart;
    uintptr_t mEnd;
    uintptr_t mOffset;
    uintptr_t mLoadBase;
    Boolean mLoadBaseRead = false;
    String mName;
};


// Ordering comparator that returns equivalence for overlapping entries
struct CompareEntries
{
    bool operator()(const MapEntry* a, const MapEntry* b) const
    {
        return a->mEnd <= b->mStart;
    }
};

class MapData
{
public:
    MapData() = default;
    ~MapData();

    const MapEntry* Find(
        /* [in] */ uintptr_t pc,
        /* [in] */ uintptr_t* relPc = nullptr);

private:
    Boolean ReadMaps();

    SyncObject mLock;
    std::set<MapEntry*, CompareEntries> mEntries;
};

}
}

#endif  // __COMO_CORE_NATIVEMAPDATA_H__
