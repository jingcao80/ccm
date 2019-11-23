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

#ifndef __CDLC_LOGGER_H__
#define __CDLC_LOGGER_H__

#include <cstdio>

namespace cdlc {

class Logger
{
public:
    static void D(
        /* [in] */ const char* tag,
        /* [in] */ const char* format, ...);

    static void E(
        /* [in] */ const char* tag,
        /* [in] */ const char* format, ...);

    static void V(
        /* [in] */ const char* tag,
        /* [in] */ const char* format, ...);

    inline static void SetLevel(
        /* [in] */ int level);

    inline static void SetStream(
        /* [in] */ FILE* stream);

public:
    static constexpr int VERBOSE = 0;
    static constexpr int DEBUG = 1;
    static constexpr int ERROR = 2;

private:
    static int sLevel;
    static FILE* sStream;
};

void Logger::SetLevel(
    /* [in] */ int level)
{
    sLevel = level;
}

void Logger::SetStream(
    /* [in] */ FILE* stream)
{
    sStream = stream;
}

}

#endif // __CDLC_LOGGER_H__
