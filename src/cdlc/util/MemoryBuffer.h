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

#ifndef __CDLC_MEMORYBUFFER_H__
#define __CDLC_MEMORYBUFFER_H__

#include "util/LightRefBase.h"
#include "util/String.h"
#include <cstddef>
#include <cstdio>

namespace cdlc {

class MemoryBuffer
    : public LightRefBase
{
public:
    size_t Put(
        /* [in] */ const String& string);

    inline const char* GetArray() const;

    inline size_t GetPosition() const;

    void Dump(
        /* [in] */ FILE* stream) const;

private:
    bool EnLarge(
        /* [in] */ size_t size);

private:
    static const char* TAG;
    static constexpr size_t MAX_SIZE = 0xFFFFFFFF - 1;

    char* mHb = nullptr;
    size_t mCapacity = 0;
    size_t mPosition = 0;
};

const char* MemoryBuffer::GetArray() const
{
    return mHb;
}

size_t MemoryBuffer::GetPosition() const
{
    return mPosition;
}

}

#endif // __CDLC_MEMORYBUFFER_H__
