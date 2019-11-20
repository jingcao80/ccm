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

#ifndef __CDLC_STRINGPOOL_H__
#define __CDLC_STRINGPOOL_H__

#include "util/AutoPtr.h"
#include "util/String.h"
#include <unordered_map>

namespace cdlc {

class StringPool
{
public:
    StringPool();

    ~StringPool();

    void Add(
        /* [in] */ const String& string);

    char* FindAddress(
        /* [in] */ const String& string);

    ptrdiff_t FindOffset(
        /* [in] */ const String& string);

    inline size_t GetSize();

    inline char* GetData();

    void Dump();

private:
    ptrdiff_t AddInternal(
        /* [in] */ const String& string);

    bool Enlarge(
        /* [in] */ int expand);

private:
    char* mData = nullptr;
    size_t mCapacity = 256;
    ptrdiff_t mOffset = 0;
    std::unordered_map<String, ptrdiff_t, StringHashFunc, StringEqualsFunc> mStringOffsets;
};

size_t StringPool::GetSize()
{
    return mOffset;
}

char* StringPool::GetData()
{
    return mData;
}

}

#endif // __CDLC_STRINGPOOL_H__
