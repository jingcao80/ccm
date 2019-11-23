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

#include "util/MemoryBuffer.h"
#include "util/Logger.h"
#include <cstdlib>
#include <cstring>

namespace cdlc {

const char* MemoryBuffer::TAG = "MemoryBuffer";

size_t MemoryBuffer::Put(
    /* [in] */ const String& string)
{
    size_t pos = mPosition;

    if (string.IsEmpty()) {
        return pos;
    }

    int size = string.GetLength();
    if (!EnLarge(size)) {
        return pos;
    }

    memcpy(mHb + mPosition, string.string(), size);
    mPosition += size;
    mHb[mPosition] = '\0';
    return pos;
}

bool MemoryBuffer::EnLarge(
    /* [in] */ size_t size)
{
    if (mPosition + size + 1 >= MAX_SIZE) {
        Logger::E(TAG, "The size is too large.");
        return false;
    }

    if (mPosition + size + 1 < mCapacity) {
        return true;
    }

    size_t newSize = (mCapacity == 0) ? 0xFFFF : mCapacity * 5;
    if (newSize < mCapacity + size + 1) {
        newSize = mCapacity + size + 1;
    }
    if (newSize > MAX_SIZE) {
        newSize = MAX_SIZE;
    }

    char* newBuffer = (char*)malloc(newSize);
    if (newBuffer == nullptr) {
        Logger::E(TAG, "Fail to malloc %lu bytes memory.", newSize);
        return false;
    }

    if (mHb != nullptr) {
        memcpy(newBuffer, mHb, mCapacity);
        free(mHb);
    }
    mHb = newBuffer;
    mCapacity = newSize;
    return true;
}

void MemoryBuffer::Dump(
    /* [in] */ FILE* stream) const
{
    fprintf(stream, "==== Dump MemoryBuffer[%p] ====\n", this);
    fprintf(stream, "%s", mHb);
    fprintf(stream, "==== Dump End ====\n");
}

}
