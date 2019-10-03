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

#include "comologger.h"
#include <cstdarg>
#include <cstdio>

namespace como {

int Logger::sLevel = DEBUG;

void Logger::D(
    /* [in] */ const char* tag,
    /* [in] */ const char* format, ...)
{
    if (DEBUG < sLevel) return;

    va_list argList;

    printf("[%s]: ", tag);
    va_start(argList, format);
    vprintf(format, argList);
    va_end(argList);
    printf("\n");
}

void Logger::E(
    /* [in] */ const char* tag,
    /* [in] */ const char* format, ...)
{
    if (ERROR < sLevel) return;

    va_list argList;

    printf("[%s]: ", tag);
    va_start(argList, format);
    vprintf(format, argList);
    va_end(argList);
    printf("\n");
}

void Logger::V(
    /* [in] */ const char* tag,
    /* [in] */ const char* format, ...)
{
    if (VERBOSE < sLevel) return;

    va_list argList;

    printf("[%s]: ", tag);
    va_start(argList, format);
    vprintf(format, argList);
    va_end(argList);
    printf("\n");
}

void Logger::W(
    /* [in] */ const char* tag,
    /* [in] */ const char* format, ...)
{
    if (WARNING < sLevel) return;

    va_list argList;

    printf("[%s]: ", tag);
    va_start(argList, format);
    vprintf(format, argList);
    va_end(argList);
    printf("\n");
}

void Logger::Log(
    /* [in] */ int level,
    /* [in] */ const char* tag,
    /* [in] */ const char* format, ...)
{
    if (level < sLevel) return;

    va_list argList;

    printf("[%s]: ", tag);
    va_start(argList, format);
    vprintf(format, argList);
    va_end(argList);
    printf("\n");
}

void Logger::SetLevel(
    /* [in] */ int level)
{
    sLevel = level;
}

} // namespace como
