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

#ifndef __CCM_STRINGBUILDER_H__
#define __CCM_STRINGBUILDER_H__

#include "String.h"

namespace ccm {

class StringBuilder
{
public:
    StringBuilder()
        : mCapacity(0)
        , mPosition(0)
        , mBuffer(nullptr)
    {}

    ~StringBuilder();

    StringBuilder& Append(
        /* [in] */ char c);

    StringBuilder& Append(
        /* [in] */ const char* string);

    StringBuilder& Append(
        /* [in] */ const String& string);

    String ToString();

private:
    bool Enlarge(
        /* [in] */ int size);

private:
    static const String TAG;
    static constexpr int MAX_SIZE = 67108863; // 2^26 - 1;
    int mCapacity;
    int mPosition;
    char* mBuffer;
};

}

#endif // __CCM_STRINGBUILDER_H__
