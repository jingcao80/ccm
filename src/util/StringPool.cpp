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

#include "Logger.h"
#include "StringPool.h"

namespace ccm {

template<>
HashMap<char*>::~HashMap<char*>()
{
    for (int i = 0; i < mBucketSize; i++) {
        if (mBuckets[i] != nullptr) {
            Bucket* curr = mBuckets[i];
            while (curr != nullptr) {
                Bucket* next = curr->mNext;
                free(curr->mValue);
                delete curr;
                curr = next;
            }
        }
    }
}

StringPool::StringPool()
    : mData(nullptr)
    , mDataCapacity(256)
    , mDataOffset(0)
    , mPool(3000)
{
    mData = (char*)calloc(1, mDataCapacity);
    Add(String(""));
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
    if (string.IsNull() || mPool.ContainsKey(string)) {
        return;
    }

    ptrdiff_t offset = AddInternal(string);
    if (offset == 0) return;
    mPool.Put(string, offset);
}

const char* StringPool::FindAddress(
    /* [in] */ const String& string)
{
    ptrdiff_t offset = mPool.Get(string);
    return offset != 0 ? mData + offset : nullptr;
}

ptrdiff_t StringPool::FindOffset(
    /* [in] */ const String& string)
{
    return mPool.Get(string);
}

ptrdiff_t StringPool::AddInternal(
    /* [in] */ const String& string)
{
    if (!EnsureCapacity(string.GetLength() + 1)) {
        return 0;
    }

    char* target = mData + mDataOffset;
    strcpy(target, string.string());
    mDataOffset += string.GetLength() + 1;
    return target - mData;
}

bool StringPool::EnsureCapacity(
    /* [in] */ size_t expand)
{
    int newSize = mDataOffset + expand;

    if (newSize < mDataCapacity) {
        return true;
    }

    newSize = 10 * mDataCapacity > newSize ?
            10 * mDataCapacity : 10 * mDataCapacity + newSize;
    char* newData = (char*)calloc(1, newSize);
    if (newData == nullptr) {
        Logger::E("StringPool", "Out of memory.");
        return false;
    }
    memcpy(newData, mData, mDataOffset);
    free(mData);
    mData = newData;
    mDataCapacity = newSize;
    return true;
}

}
