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

#include "String.h"
#include "Logger.h"

#include <new>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

namespace ccm {

static const char* TAG = "String";
static const int MAX_SIZE = 67108863; // 2^26 - 1;

struct SharedBuffer
{
    SharedBuffer(
        /* [in] */ int refCount,
        /* [in] */ int length)
        : mRefCount(refCount)
        , mLength(length)
    {}

    static void* Alloc(
        /* [in] */ size_t size);

    static int AddRef(
        /* [in] */ void* handle);

    static int Release(
        /* [in] */ void* handle);

    volatile int mRefCount;
    int mLength;
};

void* SharedBuffer::Alloc(
    /* [in] */ size_t size)
{
    if (size > MAX_SIZE) {
        Logger::E(TAG, "The string is too large to alloc.");
        return nullptr;
    }

    void* handle = malloc(sizeof(struct SharedBuffer) + size + 1);
    if (handle == NULL) {
        Logger::E(TAG, "Fail to malloc %d memory", size);
        return nullptr;
    }

    new (handle)SharedBuffer(1, size);
    return reinterpret_cast<struct SharedBuffer*>(handle) + 1;
}

int SharedBuffer::AddRef(
    /* [in] */ void* handle)
{
    if (handle == nullptr) return 0;

    struct SharedBuffer* buffer =
            reinterpret_cast<struct SharedBuffer*>(handle) - 1;
    int before = __sync_fetch_and_add(&buffer->mRefCount, (int)1);
    if (before < 0) {
        Logger::E(TAG, "The refCount %d of %p is error in AddRef.", before);
    }
    return before + 1;
}

int SharedBuffer::Release(
    /* [in] */ void* handle)
{
    if (handle == nullptr) return 0;

    struct SharedBuffer* buffer =
            reinterpret_cast<struct SharedBuffer*>(handle) - 1;
    int before = __sync_fetch_and_sub(&buffer->mRefCount, (int)1);
    if (before == 1) {
        free(buffer);
    }
    else if (before < 1) {
        Logger::E(TAG, "The refCount %d of %p is error in AddRef.", before);
    }
    return before - 1;
}

//========================================================================
// String
//========================================================================

String::String(
    /* [in] */ const char* string)
{
    if (string == nullptr) {
        mString = nullptr;
    }
    else {
        mString = reinterpret_cast<char*>(SharedBuffer::Alloc(strlen(string)));
        if (mString == nullptr) return;
        strcpy(mString, string);
    }
}

String::String(
    /* [in] */ const String& other)
{
    mString = other.mString;
    SharedBuffer::AddRef(mString);
}

String::String(
    /* [in] */ String&& other)
{
    mString = other.mString;
    other.mString = nullptr;
}

String::String(
    /* [in] */ int size)
{
    mString = reinterpret_cast<char*>(SharedBuffer::Alloc(size));
    memset(mString, 0, size + 1);
}

String::~String()
{
    SharedBuffer::Release(mString);
    mString = nullptr;
}

int String::GetLength() const
{
    if (mString == nullptr) return 0;

    struct SharedBuffer* buffer =
            reinterpret_cast<struct SharedBuffer*>(mString) - 1;
    return buffer->mLength;
}

bool String::Equals(
    /* [in] */ const String& other) const
{
    if (mString == nullptr &&
            other.mString == nullptr) return true;

    if (GetLength() != other.GetLength()) return false;

    return strcmp(mString, other.mString) == 0;
}

bool String::Equals(
    /* [in] */ const char* string) const
{
    if (string == nullptr) {
        return mString == nullptr? true : false;
    }

    if (GetLength() != strlen(string)) return false;

    return strcmp(mString, string) == 0;
}

bool String::EqualsIgnoreCase(
    /* [in] */ const String& other) const
{
    if (mString == nullptr &&
            other.mString == nullptr) return true;

    if (GetLength() != other.GetLength()) return false;

    return strcasecmp(mString, other.mString) == 0;
}

bool String::EqualsIgnoreCase(
    /* [in] */ const char* string) const
{
    if (string == nullptr) {
        return mString == nullptr? true : false;
    }

    if (GetLength() != strlen(string)) return false;

    return strcasecmp(mString, string) == 0;
}

char String::GetChar(
    /* [in] */ int index) const
{
    if (index < 0 || index >= GetLength()) return '\0';

    return mString[index];
}

String String::Substring(
    /* [in] */ int begin) const
{
    if (begin < 0 || begin >= GetLength()) return String();

    return String(mString + begin);
}

String String::Substring(
    /* [in] */ int begin,
    /* [in] */ int end) const
{
    if (begin < 0 || begin > end || end >= GetLength()) return String();

    int len = end - begin + 1;
    String string(len);
    memcpy(string.mString, mString + begin, len);
    string.mString[len] = '\0';

    return string;
}

bool String::Contains(
    /* [in] */ const String& other) const
{
    return IndexOf(other) != -1;
}

bool String::Contains(
    /* [in] */ const char* string) const
{
    return IndexOf(string) != -1;
}

int String::IndexOf(
    /* [in] */ int c) const
{
    if (c == '\0' || IsNullOrEmpty()) return -1;

    char* ci = strchr(mString, c);
    return ci != nullptr ? ci - mString : -1;
}

int String::IndexOf(
    /* [in] */ const String& other) const
{
    if (IsNullOrEmpty() || other.IsNullOrEmpty()) return -1;

    char* ci = strstr(mString, other.mString);
    return ci != nullptr ? ci - mString : -1;
}

int String::IndexOf(
    /* [in] */ const char* string) const
{
    if (IsNullOrEmpty() || string == nullptr || string[0] == '\0') {
        return -1;
    }

    char* ci = strstr(mString, string);
    return ci != nullptr ? ci - mString : -1;
}

int String::LastIndexOf(
    /* [in] */ int c) const
{
    if (c == '\0' || IsNullOrEmpty()) return -1;

    char* ci = strrchr(mString, c);
    return ci != nullptr ? ci - mString : -1;
}

int String::LastIndexOf(
    /* [in] */ const String& other) const
{
    if (IsNullOrEmpty() || other.IsNullOrEmpty()) return -1;

    int sourceLength = GetLength();
    int targetLength = other.GetLength();
    int fromIndex = sourceLength;
    int rightIndex = sourceLength - targetLength;
    if (fromIndex < 0) {
        return -1;
    }
    if (fromIndex > rightIndex) {
        fromIndex = rightIndex;
    }

    int strLastIndex = targetLength - 1;
    char strLastChar = other.GetChar(strLastIndex);
    int min = targetLength - 1;
    int i = min + fromIndex;

startSearchForLastChar:
    while (true) {
        while (i >= min && GetChar(i) != strLastChar) {
            i--;
        }
        if (i < min) {
            return -1;
        }
        int j = i - 1;
        int start = j - (targetLength - 1);
        int k = strLastIndex - 1;

        while (j > start) {
            if (GetChar(j--) != other.GetChar(k--)) {
                i--;
                goto startSearchForLastChar;
            }
        }
        return start + 1;
    }
}

int String::LastIndexOf(
    /* [in] */ const char* string) const
{
    return LastIndexOf(String(string));
}

String& String::operator=(
    /* [in] */ const char* string)
{
    SharedBuffer::Release(mString);

    if (string == nullptr) {
        mString = nullptr;
        return *this;
    }
    else {
        mString = reinterpret_cast<char*>(SharedBuffer::Alloc(strlen(string)));
        if (mString == nullptr) return *this;
        strcpy(mString, string);
        return *this;
    }
}

String& String::operator=(
    /* [in] */ const String& other)
{
    if (mString == other.mString) return *this;

    SharedBuffer::Release(mString);
    mString = other.mString;
    SharedBuffer::AddRef(mString);
    return *this;
}

String& String::operator=(
    /* [in] */ String&& other)
{
    if (mString == other.mString) {
        other.mString = nullptr;
        return *this;
    }

    SharedBuffer::Release(mString);
    mString = other.mString;
    other.mString = nullptr;
    return *this;
}

String& String::operator+=(
    /* [in] */ const char* string)
{
    if (string == nullptr || string[0] == '\0') return *this;

    int oldSize = GetLength();
    int newSize = oldSize + strlen(string);
    char* str = reinterpret_cast<char*>(SharedBuffer::Alloc(newSize));
    if (str == nullptr) return *this;

    memcpy(str, mString, oldSize);
    strcpy(str + oldSize, string);

    SharedBuffer::Release(mString);
    mString = str;
    return *this;
}

String& String::operator+=(
    /* [in] */ const String& other)
{
    if (other.IsNullOrEmpty()) return *this;

    int oldSize = GetLength();
    int newSize = oldSize + other.GetLength();
    char* str = reinterpret_cast<char*>(SharedBuffer::Alloc(newSize));
    if (str == nullptr) return *this;

    memcpy(str, mString, oldSize);
    strcpy(str + oldSize, other.mString);

    SharedBuffer::Release(mString);
    mString = str;
    return *this;
}

String String::Format(
    /* [in] */ const char* format ...)
{
    va_list args, args1;

    va_start(args, format);
    va_copy(args1, args);

    int len = vsnprintf(nullptr, 0, format, args);
    va_end(args);

    String string(len);
    vsnprintf(string.mString, len + 1, format, args1);
    va_end(args1);

    return string;
}

} // namespace ccm
