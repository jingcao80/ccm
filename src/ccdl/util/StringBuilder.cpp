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

#include "StringBuilder.h"
#include "Logger.h"

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

namespace ccdl {

const String StringBuilder::TAG("StringBuilder");

StringBuilder::~StringBuilder()
{
    if (mBuffer != nullptr) {
        free(mBuffer);
        mBuffer = nullptr;
    }
    mCapacity = 0;
    mPosition = 0;
}

StringBuilder& StringBuilder::Append(
    /* [in] */ char c)
{
    if (mPosition + 1 > MAX_SIZE) {
        Logger::E(TAG, "The StringBuilder is full and it can't be enlarged.");
        return *this;
    }

    if (mPosition + 1 >= mCapacity) {
        if (!Enlarge(2)) return *this;
    }

    mBuffer[mPosition++] = c;
    return *this;
}

StringBuilder& StringBuilder::Append(
    /* [in] */ const char* string)
{
    if (string == nullptr || string[0] == '\0') {
        return *this;
    }

    if (mPosition >= MAX_SIZE) {
        Logger::E(TAG, "The StringBuilder is full and it can't be enlarged.");
        return *this;
    }

    int len = strlen(string);
    if (mPosition + len + 1 >= mCapacity) {
        if (!Enlarge(len + 1)) return *this;
    }

    memcpy(mBuffer + mPosition, string, len);
    mPosition += len;
    return *this;
}

StringBuilder& StringBuilder::Append(
    /* [in] */ const String& string)
{
    if (string.IsNullOrEmpty()) return *this;

    if (mPosition >= MAX_SIZE) {
        Logger::E(TAG, "The StringBuilder is full and it can't be enlarged.");
        return *this;
    }

    int len = string.GetLength();
    if (mPosition + len + 1 >= mCapacity) {
        if (!Enlarge(len + 1)) return *this;
    }

    memcpy(mBuffer + mPosition, string.string(), len);
    mPosition += len;
    return *this;
}

StringBuilder& StringBuilder::AppendFormat(
        /* [in] */ const char* format ...)
{
    va_list args, args1;

    va_start(args, format);
    va_copy(args1, args);

    int len = vsnprintf(nullptr, 0, format, args);
    va_end(args);

    if (mPosition + len + 1 >= mCapacity) {
        if (!Enlarge(len + 1)) return *this;
    }

    vsnprintf(mBuffer + mPosition, len + 1, format, args1);
    mPosition += len;
    va_end(args1);

    return *this;
}

String StringBuilder::ToString()
{
    return String(mBuffer);
}

bool StringBuilder::Enlarge(
    /* [in] */ int size)
{
    int newSize = mCapacity == 0 ? 256 : mCapacity;
    newSize = newSize > size ? newSize : size;
    char* newBuffer = (char*)calloc(mCapacity + newSize, 1);
    if (newBuffer == nullptr) {
        Logger::E(TAG, "Fail to malloc %d byte memory.", mCapacity + newSize);
        return false;
    }

    if (mBuffer != nullptr) {
        memcpy(newBuffer, mBuffer, mCapacity);
        free(mBuffer);
    }
    mBuffer = newBuffer;
    mCapacity += newSize;
    return true;
}

}
