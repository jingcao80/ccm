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

#include "util/Logger.h"
#include <cstdarg>

namespace cdlc {

int Logger::sLevel = DEBUG;
FILE* Logger::sStream = stdout;

void Logger::D(
    /* [in] */ const char* tag,
    /* [in] */ const char* format, ...)
{
    if (DEBUG < sLevel) {
        return;
    }

    va_list args;

    fprintf(sStream, "[DEBUG] %s : ", tag);
    va_start(args, format);
    vfprintf(sStream, format, args);
    va_end(args);
    fprintf(sStream, "\n");
}

void Logger::E(
    /* [in] */ const char* tag,
    /* [in] */ const char* format, ...)
{
    if (ERROR < sLevel) {
        return;
    }

    va_list args;

    fprintf(sStream, "[ERROR] %s : ", tag);
    va_start(args, format);
    vfprintf(sStream, format, args);
    va_end(args);
    fprintf(sStream, "\n");
}

void Logger::V(
    /* [in] */ const char* tag,
    /* [in] */ const char* format, ...)
{
    if (VERBOSE < sLevel) {
        return;
    }

    va_list args;

    fprintf(sStream, "[VERBOSE] %s : ", tag);
    va_start(args, format);
    vfprintf(sStream, format, args);
    va_end(args);
    fprintf(sStream, "\n");
}

}
