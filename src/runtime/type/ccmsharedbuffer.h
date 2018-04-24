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
 * Copyright (C) 2005 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __CCM_SHAREDBUFFER_H__
#define __CCM_SHAREDBUFFER_H__

#include "ccmdef.h"
#include <atomic>
#include <stddef.h>

namespace ccm {

class COM_PUBLIC SharedBuffer
{
public:
    enum {
        eKeepStorage = 0x00000001
    };

    static SharedBuffer* Alloc(
        /* [in] */ size_t size);

    static void Dealloc(
        /* [in] */ const SharedBuffer* released);

    inline const void* GetData() const;

    inline void* GetData();

    inline size_t GetSize() const;

    inline static SharedBuffer* GetBufferFromData(
        /* [in] */ void* data);

    inline static const SharedBuffer* GetBufferFromData(
        /* [in] */ const void* data);

    inline static size_t GetSizeFromData(
        /* [in] */ const void* data);

    SharedBuffer* Edit() const;

    SharedBuffer* EditResize(
        /* [in] */ size_t size) const;

    SharedBuffer* AttemptEdit() const;

    SharedBuffer* Reset(
        /* [in] */ size_t size) const;

    int32_t AddRef() const;

    int32_t Release(
        /* [in] */ uint32_t flags = 0) const;

    inline bool OnlyOwner() const;

private:
    inline SharedBuffer() {}
    inline ~SharedBuffer() {}
    SharedBuffer(const SharedBuffer&);
    SharedBuffer& operator = (const SharedBuffer&);

private:
    mutable std::atomic<int32_t> mRefs;
    size_t mSize;
    uint32_t mReserved[2];
};

const void* SharedBuffer::GetData() const
{
    return this + 1;
}

void* SharedBuffer::GetData()
{
    return this + 1;
}

size_t SharedBuffer::GetSize() const
{
    return mSize;
}

SharedBuffer* SharedBuffer::GetBufferFromData(
    /* [in] */ void* data)
{
    return data != nullptr ? static_cast<SharedBuffer *>(data) - 1 : 0;
}

const SharedBuffer* SharedBuffer::GetBufferFromData(
    /* [in] */ const void* data)
{
    return data !=  nullptr ? static_cast<const SharedBuffer *>(data) - 1 : 0;
}

size_t SharedBuffer::GetSizeFromData(
    /* [in] */ const void* data)
{
    return data != nullptr ? GetBufferFromData(data)->mSize : 0;
}

bool SharedBuffer::OnlyOwner() const
{
    return (mRefs.load(std::memory_order_acquire) == 1);
}

}

#endif // __CCM_SHAREDBUFFER_H__
