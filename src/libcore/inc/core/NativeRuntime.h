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

#ifndef __CCM_CORE_NATIVERUNTIME_H__
#define __CCM_CORE_NATIVERUNTIME_H__

#include <ccmtypes.h>

namespace ccm {
namespace core {

class NativeMonitorList;
class NativeMonitorPool;
class NativeThreadList;

class NativeRuntime
{
public:
    static Boolean Create();

    static NativeRuntime* Current();

    size_t GetMaxSpinsBeforeThinLockInflation() const;

    NativeMonitorList* GetMonitorList() const;

    NativeMonitorPool* GetMonitorPool() const;

    NativeThreadList* GetThreadList() const;

private:
    Boolean Init();

private:
    static NativeRuntime* sInstance;

    // The number of spins that are done before thread suspension is used to forcibly inflate.
    size_t mMaxSpinsBeforeThinLockInflation;

    NativeMonitorList* mMonitorList;

    NativeMonitorPool* mMonitorPool;

    NativeThreadList* mThreadList;
};

inline NativeRuntime* NativeRuntime::Current()
{
    return sInstance;
}

inline size_t NativeRuntime::GetMaxSpinsBeforeThinLockInflation() const
{
    return mMaxSpinsBeforeThinLockInflation;
}

inline NativeMonitorList* NativeRuntime::GetMonitorList() const
{
    return mMonitorList;
}

inline NativeMonitorPool* NativeRuntime::GetMonitorPool() const
{
    return mMonitorPool;
}

inline NativeThreadList* NativeRuntime::GetThreadList() const
{
    return mThreadList;
}

}
}

#endif // __CCM_CORE_NATIVERUNTIME_H__
