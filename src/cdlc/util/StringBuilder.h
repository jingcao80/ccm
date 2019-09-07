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

#ifndef __CDLC_STRINGBUILDER_H__
#define __CDLC_STRINGBUILDER_H__

#include "util/String.h"

namespace cdlc {

class StringBuilder
{
public:
    ~StringBuilder();

    inline StringBuilder& Append(
        /* [in] */ const char* string);

    inline StringBuilder& Append(
        /* [in] */ const char* string,
        /* [in] */ int begin);

    StringBuilder& Append(
        /* [in] */ const char* string,
        /* [in] */ int begin,
        /* [in] */ int end);

    inline StringBuilder& Append(
        /* [in] */ const String& string);

    inline StringBuilder& Append(
        /* [in] */ const String& string,
        /* [in] */ int begin);

    StringBuilder& Append(
        /* [in] */ const String& string,
        /* [in] */ int begin,
        /* [in] */ int end);

    StringBuilder& Append(
        /* [in] */ char c);

    StringBuilder& AppendFormat(
        /* [in] */ const char* format ...);

    inline String ToString();

private:
    bool Enlarge(
        /* [in] */ int size);

private:
    static const char* TAG;
    static constexpr int MAX_SIZE = String::MAX_SIZE;

    char* mBuffer = nullptr;
    int mPosition = 0;
    int mCapacity = 0;
};

StringBuilder& StringBuilder::Append(
    /* [in] */ const char* string)
{
    return Append(string, 0, MAX_SIZE);
}

StringBuilder& StringBuilder::Append(
    /* [in] */ const char* string,
    /* [in] */ int begin)
{
    return Append(string, begin, MAX_SIZE);
}

StringBuilder& StringBuilder::Append(
    /* [in] */ const String& string)
{
    return Append(string, 0, MAX_SIZE);
}

StringBuilder& StringBuilder::Append(
    /* [in] */ const String& string,
    /* [in] */ int begin)
{
    return Append(string, 0, MAX_SIZE);
}

String StringBuilder::ToString()
{
    return String(mBuffer);
}

}

#endif // __CDLC_STRINGBUILDER_H__
