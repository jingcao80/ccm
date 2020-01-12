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

#ifndef __COMO_UTIL_LOCALE_PARSESTATUS_H__
#define __COMO_UTIL_LOCALE_PARSESTATUS_H__

#include <comoref.h>
#include <comotypes.h>

namespace como {
namespace util {
namespace locale {

class ParseStatus
    : public LightRefBase
{
public:
    ParseStatus();

    void Reset();

    Boolean IsError();

    Integer GetErrorIndex();

    Integer GetParseLength();

    String GetErrorMessage();

public:
    Integer mParseLength;
    Integer mErrorIndex;
    String mErrorMsg;
};

inline ParseStatus::ParseStatus()
{
    Reset();
}

inline void ParseStatus::Reset()
{
    mParseLength = 0;
    mErrorIndex = -1;
    mErrorMsg = nullptr;
}

inline Boolean ParseStatus::IsError()
{
    return mErrorIndex >= 0;
}

inline Integer ParseStatus::GetErrorIndex()
{
    return mErrorIndex;
}

inline Integer ParseStatus::GetParseLength()
{
    return mParseLength;
}

inline String ParseStatus::GetErrorMessage()
{
    return mErrorMsg;
}

}
}
}

#endif // __COMO_UTIL_LOCALE_PARSESTATUS_H__
