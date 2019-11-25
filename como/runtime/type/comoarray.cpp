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

#include "comotypes.h"

namespace como {

Triple::Triple(
    /* [in] */ const Triple& other)
    : mData(other.mData)
    , mSize(other.mSize)
{
    if (other.mData != nullptr) {
        SharedBuffer::GetBufferFromData(other.mData)->AddRef();
    }
}

Triple::Triple(
    /* [in] */ Triple&& other)
    : mData(other.mData)
    , mSize(other.mSize)
{
    other.mData = nullptr;
    other.mSize = 0;
}

void Triple::AllocData(
    /* [in] */ Long dataSize)
{
    SharedBuffer* buf = SharedBuffer::Alloc(dataSize);
    if (buf == nullptr) {
        Logger::E("Triple", "Malloc data which size is %lld failed.", dataSize);
        mData = nullptr;
        mSize = 0;
        return;
    }
    void* data = buf->GetData();
    memset(data, 0, dataSize);
    mData = data;
}

void Triple::FreeData()
{
    if (mData != nullptr) {
        SharedBuffer::GetBufferFromData(mData)->Release();
        mData = nullptr;
    }
    mSize = 0;
}

Triple& Triple::operator=(
    /* [in] */ const Triple& other)
{
    if (mData == other.mData) {
        return *this;
    }

    if (other.mData != nullptr) {
        SharedBuffer::GetBufferFromData(other.mData)->AddRef();
    }
    if (mData != nullptr) {
        SharedBuffer::GetBufferFromData(mData)->Release();
    }
    mData = other.mData;
    mSize = other.mSize;
    return *this;
}

Triple& Triple::operator=(
    /* [in] */ Triple&& other)
{
    if (mData != nullptr) {
        SharedBuffer::GetBufferFromData(mData)->Release();
    }
    mData = other.mData;
    mSize = other.mSize;
    other.mData = nullptr;
    other.mSize = 0;
    return *this;
}

} // namespace como
