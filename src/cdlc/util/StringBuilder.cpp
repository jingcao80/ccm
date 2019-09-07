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

#include "util/StringBuilder.h"
#include "util/Logger.h"
#include <cstdarg>
#include <cstdlib>
#include <cstring>

namespace cdlc {

const char* StringBuilder::TAG = "StringBuilder";

StringBuilder::~StringBuilder()
{
    if (mBuffer != nullptr) {
        free(mBuffer);
    }
}

StringBuilder& StringBuilder::Append(
    /* [in] */ const char* string,
    /* [in] */ int begin,
    /* [in] */ int end)
{
    if (string == nullptr || string[0] == '\0') {
        return *this;
    }

    if (begin < 0 || begin > end || end < 0) {
        return *this;
    }

    int len = strlen(string);
    if (begin >= len) {
        return *this;
    }

    if (mPosition >= MAX_SIZE) {
        Logger::E(TAG, "The StringBuilder is full.");
        return *this;
    }

    len = end < len ? end - begin : len - begin;
    if (!Enlarge(len)) {
        return *this;
    }

    memcpy(mBuffer + mPosition, string + begin, len);
    mPosition += len;
    return *this;
}

StringBuilder& StringBuilder::Append(
    /* [in] */ const String& string,
    /* [in] */ int begin,
    /* [in] */ int end)
{
    if (string.IsEmpty()) {
        return *this;
    }

    if (begin < 0 || begin > end || end < 0) {
        return *this;
    }

    int len = string.GetLength();
    if (begin >= len) {
        return *this;
    }

    if (mPosition >= MAX_SIZE) {
        Logger::E(TAG, "The StringBuilder is full.");
        return *this;
    }

    len = end < len ? end - begin : len - begin;
    if (!Enlarge(len)) {
        return *this;
    }

    memcpy(mBuffer + mPosition, string.string() + begin, len);
    mPosition += len;
    return *this;
}

StringBuilder& StringBuilder::Append(
    /* [in] */ char c)
{
    if (!Enlarge(1)) {
        return *this;
    }

    mBuffer[mPosition++] = c;
    return *this;
}

StringBuilder& StringBuilder::AppendFormat(
    /* [in] */ const char* format ...)
{
    va_list args, argss;

    va_start(args, format);
    va_copy(argss, args);
    int len = vsnprintf(nullptr, 0, format, args);
    va_end(args);

    if (!Enlarge(len)) {
        return *this;
    }

    vsnprintf(mBuffer + mPosition, len + 1, format, argss);
    mPosition += len;
    va_end(argss);

    return *this;
}

bool StringBuilder::Enlarge(
    /* [in] */ int size)
{
    if (mPosition + size + 1 >= MAX_SIZE || mPosition + size + 1 < 0) {
        Logger::E(TAG, "The size is too large.");
        return false;
    }

    if (mPosition + size + 1 < mCapacity) {
        return true;
    }

    int newSize = (mCapacity == 0) ? 256 : mCapacity * 2;
    if (newSize < mCapacity + size + 1) {
        newSize = mCapacity + size + 1;
    }
    if (newSize > MAX_SIZE) {
        newSize = MAX_SIZE;
    }

    char* newBuffer = (char*)calloc(newSize, 1);
    if (newBuffer == nullptr) {
        Logger::E(TAG, "Fail to malloc %d bytes memory.", newSize);
        return false;
    }

    if (mBuffer != nullptr) {
        memcpy(newBuffer, mBuffer, mCapacity);
        free(mBuffer);
    }
    mBuffer = newBuffer;
    mCapacity = newSize;
    return true;
}

}
