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

#include "util/Logger.h"
#include "util/StringPool.h"
#include <cstring>

namespace cdlc {

StringPool::StringPool()
{
    mData = (char*)calloc(mCapacity, 1);
}

StringPool::~StringPool()
{
    if (mData != nullptr) {
        free(mData);
    }
}

void StringPool::Add(
    /* [in] */ const String& string)
{
    if (string.IsEmpty() || mStringOffsets.find(string) != mStringOffsets.end()) {
        return;
    }

    ptrdiff_t offset = AddInternal(string);
    if (offset != -1) {
        mStringOffsets[string] = offset;
    }
}

char* StringPool::FindAddress(
    /* [in] */ const String& string)
{
    auto it = mStringOffsets.find(string);

    if (it != mStringOffsets.end()) {
        return mData + it->second;
    }
    return nullptr;
}

ptrdiff_t StringPool::FindOffset(
    /* [in] */ const String& string)
{
    auto it = mStringOffsets.find(string);

    if (it != mStringOffsets.end()) {
        return it->second;
    }
    return -1;
}

ptrdiff_t StringPool::AddInternal(
    /* [in] */ const String& string)
{
    if (!Enlarge(string.GetLength() + 1)) {
        return -1;
    }

    char* target = mData + mOffset;
    strcpy(target, string.string());
    mOffset += string.GetLength() + 1;
    return target - mData;
}

bool StringPool::Enlarge(
    /* [in] */ int expand)
{
    size_t newSize = mOffset + expand;

    if (newSize < mOffset) {
        return false;
    }

    if (newSize < mCapacity) {
        return true;
    }

    if (mCapacity == PTRDIFF_MAX - 4) {
        return false;
    }

    size_t addition = 2 * mCapacity;
    if (addition < PTRDIFF_MAX - 4) {
        if (newSize < addition) {
            newSize = addition;
        }
        else if (newSize + addition < PTRDIFF_MAX - 4) {
            newSize = newSize + addition;
        }
    }

    char* newData = (char*)calloc(1, newSize);
    if (newData == nullptr) {
        Logger::E("StringPool", "Out of memory.");
        return false;
    }

    memcpy(newData, mData, mOffset);
    free(mData);
    mData = newData;
    mCapacity = newSize;
    return true;
}

void StringPool::Dump()
{
    size_t i = 1;
    char* string = mData;
    while ((string - mData) <= mOffset - 1) {
        Logger::D("StringPool", "[%llu] %s", i++, string);
        string = strchr(string, '\0') + 1;
    }
}

}
