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

#ifndef __CDLC_LIGHTREFBASE_H__
#define __CDLC_LIGHTREFBASE_H__

#include <atomic>

namespace cdlc {

class LightRefBase
{
public:
    inline LightRefBase();

    inline int AddRef() const;

    inline int Release() const;

    inline int GetRefCount() const;

protected:
    inline virtual ~LightRefBase();

private:
    mutable std::atomic<int> mRefCount;
};

LightRefBase::LightRefBase()
    : mRefCount(0)
{}

LightRefBase::~LightRefBase()
{}

int LightRefBase::AddRef() const
{
    int count = mRefCount.fetch_add(1, std::memory_order_relaxed);
    return count + 1;
}

int LightRefBase::Release() const
{
    int count = mRefCount.fetch_sub(1, std::memory_order_relaxed);
    if (count == 1) {
        std::atomic_thread_fence(std::memory_order_acquire);
        delete this;
    }
    return count - 1;
}

int LightRefBase::GetRefCount() const
{
    return mRefCount.load(std::memory_order_relaxed);
}

}

#endif // __CDLC_LIGHTREFBASE_H__

