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

#include "util/String.h"
#include "util/Logger.h"
#include "util/StringBuilder.h"
#include <atomic>
#include <cctype>
#include <cstdarg>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <new>

namespace cdlc {

static const char* TAG = "String";

struct SharedBuffer
{
    SharedBuffer(
        /* [in] */ int refCount,
        /* [in] */ int size);

    static void* Alloc(
        /* [in] */ int size);

    static int AddRef(
        /* [in] */ void* data);

    static int Release(
        /* [in] */ void* data);

    static int GetSize(
        /* [in] */ void* data);

    mutable std::atomic<int> mRefCount;
    int mSize;
};

SharedBuffer::SharedBuffer(
    /* [in] */ int refCount,
    /* [in] */ int size)
    : mRefCount(refCount)
    , mSize(size)
{}

void* SharedBuffer::Alloc(
    /* [in] */ int size)
{
    if (size < 0) {
        Logger::E(TAG, "The size is minus.");
        return nullptr;
    }
    if (size > String::MAX_SIZE) {
        Logger::E(TAG, "The size is too large.");
        return nullptr;
    }

    void* handle = malloc(sizeof(SharedBuffer) + size + 1);
    if (handle == nullptr) {
        Logger::E(TAG, "Fail to malloc memory, size is %d.", size);
        return nullptr;
    }

    new (handle)SharedBuffer(1, size);
    return reinterpret_cast<SharedBuffer*>(handle) + 1;
}

int SharedBuffer::AddRef(
    /* [in] */ void* data)
{
    if (data == nullptr) {
        return 0;
    }

    SharedBuffer* buffer = reinterpret_cast<SharedBuffer*>(data) - 1;
    int count = buffer->mRefCount.fetch_add(1, std::memory_order_relaxed);
    if (count < 1) {
        Logger::E(TAG, "The refCount %d of %p is error in AddRef.", count, buffer);
    }
    return count + 1;
}

int SharedBuffer::Release(
    /* [in] */ void* data)
{
    if (data == nullptr) {
        return 0;
    }

    SharedBuffer* buffer = reinterpret_cast<SharedBuffer*>(data) - 1;
    int count = buffer->mRefCount.fetch_sub(1, std::memory_order_relaxed);
    if (count == 1) {
        std::atomic_thread_fence(std::memory_order_acquire);
        free(buffer);
    }
    else if (count < 1) {
        Logger::E(TAG, "The refCount %d of %p is error in Release.", count, buffer);
    }
    return count - 1;
}

int SharedBuffer::GetSize(
    /* [in] */ void* data)
{
    if (data == nullptr) {
        return 0;
    }

    SharedBuffer* buffer = reinterpret_cast<SharedBuffer*>(data) - 1;
    return buffer->mSize;
}

//-----------------------------------------------------------------------------

String::String(
    /* [in] */ const char* string)
{
    if (string == nullptr) {
        return;
    }

    mString = reinterpret_cast<char*>(SharedBuffer::Alloc(strlen(string)));
    if (mString != nullptr) {
        strcpy(mString, string);
    }
}

String::String(
    /* [in] */ const char* string,
    /* [in] */ int size)
{
    if (string == nullptr) {
        return;
    }

    if (size <= 0 || size > strlen(string)) {
        return;
    }

    mString = reinterpret_cast<char*>(SharedBuffer::Alloc(size));
    if (mString != nullptr) {
        memcpy(mString, string, size);
        mString[size] = '\0';
    }
}

String::String(
    /* [in] */ const String& other)
    : mString(other.mString)
    , mHashCode(other.mHashCode)
{
    SharedBuffer::AddRef(mString);
}

String::String(
    /* [in] */ String&& other)
    : mString(other.mString)
    , mHashCode(other.mHashCode)
{
    other.mString = nullptr;
}

String::String(
    /* [in] */ int size)
{
    mString = reinterpret_cast<char*>(SharedBuffer::Alloc(size));
    if (mString != nullptr) {
        *mString = '\0';
    }
}

String::~String()
{
    SharedBuffer::Release(mString);
}

String& String::operator=(
    /* [in] */ const char* string)
{
    SharedBuffer::Release(mString);

    if (string == nullptr) {
        mString = nullptr;
    }
    else {
        mString = reinterpret_cast<char*>(SharedBuffer::Alloc(strlen(string)));
        if (mString != nullptr) {
            strcpy(mString, string);
        }
    }
    mHashCode = -1;
    return *this;
}

String& String::operator=(
    /* [in] */ const String& other)
{
    if (mString == other.mString) {
        return *this;
    }

    SharedBuffer::Release(mString);
    mString = other.mString;
    mHashCode = other.mHashCode;
    SharedBuffer::AddRef(mString);
    return *this;
}

String& String::operator=(
    /* [in] */ String&& other)
{
    SharedBuffer::Release(mString);
    mString = other.mString;
    mHashCode = other.mHashCode;
    other.mString = nullptr;
    return *this;
}

String& String::operator+=(
    /* [in] */ const char* string)
{
    if (string == nullptr || string[0] == '\0') {
        return *this;
    }

    int oldLen = GetLength();
    int newLen = oldLen + strlen(string);
    char* newString = reinterpret_cast<char*>(SharedBuffer::Alloc(newLen));
    if (newString == nullptr) {
        return *this;
    }

    memcpy(newString, mString, oldLen);
    strcpy(newString + oldLen, string);
    SharedBuffer::Release(mString);
    mString = newString;
    mHashCode = -1;
    return *this;
}

String& String::operator+=(
    /* [in] */ const String& other)
{
    if (other.IsEmpty()) {
        return *this;
    }

    int oldLen = GetLength();
    int newLen = oldLen + other.GetLength();
    char* newString = reinterpret_cast<char*>(SharedBuffer::Alloc(newLen));
    if (newString == nullptr) {
        return *this;
    }

    memcpy(newString, mString, oldLen);
    strcpy(newString + oldLen, other.mString);
    SharedBuffer::Release(mString);
    mString = newString;
    mHashCode = -1;
    return *this;
}

String String::Format(
    /* [in] */ const char* format, ...)
{
    va_list args, argss;

    va_start(args, format);
    va_copy(argss, args);
    int len = vsnprintf(nullptr, 0, format, args);
    va_end(args);

    String string(len);
    vsnprintf(string.mString, len + 1, format, argss);
    va_end(argss);

    return string;
}

int String::GetLength() const
{
    return SharedBuffer::GetSize(mString);
}

int String::GetHashCode() const
{
    if (mHashCode != -1) {
        return mHashCode;
    }

    // BKDR Hash Function
    unsigned int seed = 31; // 31 131 1313 13131 131313 etc..
    unsigned int hash = 0;

    const char* string = mString;
    if (string) {
        for ( ; *string; ++string) {
            hash = hash * seed + (*string);
        }
    }
    mHashCode = (hash & 0x7FFFFFFF);
    return mHashCode;
}

char String::GetChar(
    /* [in] */ int index) const
{
    if (index < 0 || index >= GetLength()) {
        return '\0';
    }
    return mString[index];
}

int String::Compare(
    /* [in] */ const char* string) const
{
    if (mString == string) {
        return 0;
    }
    if (mString == nullptr) {
        return -1;
    }
    if (string == nullptr) {
        return 1;
    }

    return strcmp(mString, string);
}

bool String::Equals(
    /* [in] */ const char* string) const
{
    if (mString == nullptr && string == nullptr) {
        return true;
    }

    if (string == nullptr) {
        return false;
    }
    if (GetLength() != strlen(string)) {
        return false;
    }
    return strcmp(mString, string) == 0;
}

bool String::Equals(
    /* [in] */ const String& other) const
{
    if (mString == nullptr && other.mString == nullptr) {
        return true;
    }

    if (GetLength() != other.GetLength()) {
        return false;
    }
    if (GetHashCode() != other.GetHashCode()) {
        return false;
    }
    return strcmp(mString, other.mString) == 0;
}

bool String::EqualsIgnoreCase(
    /* [in] */ const char* string) const
{
    if (mString == nullptr && string == nullptr) {
        return true;
    }

    if (string == nullptr) {
        return false;
    }
    if (GetLength() != strlen(string)) {
        return false;
    }
    return strcasecmp(mString, string) == 0;
}

bool String::EqualsIgnoreCase(
    /* [in] */ const String& other) const
{
    if (mString == nullptr && other.mString == nullptr) {
        return true;
    }

    if (GetLength() != other.GetLength()) {
        return false;
    }
    if (GetHashCode() != other.GetHashCode()) {
        return false;
    }
    return strcasecmp(mString, other.mString) == 0;
}

int String::IndexOf(
    /* [in] */ const char* string,
    /* [in] */ int fromIndex) const
{
    if (string == nullptr || string[0] == '\0') {
        return -1;
    }

    if (IsEmpty() || fromIndex >= GetLength()) {
        return -1;
    }

    char* ci = strstr(mString + fromIndex, string);
    return ci != nullptr ? ci - mString : -1;
}

int String::IndexOf(
    /* [in] */ int c,
    /* [in] */ int fromIndex) const
{
    if (c <= 0 || c > 255) {
        return -1;
    }
    if (IsEmpty() || fromIndex >= GetLength()) {
        return -1;
    }

    char* ci = strchr(mString + fromIndex, c);
    return ci != nullptr ? ci - mString : -1;
}

int String::LastIndexOf(
    /* [in] */ const char* string,
    /* [in] */ int fromIndex) const
{
    if (string == nullptr || string[0] == '\0') {
        return -1;
    }

    if (IsEmpty() || fromIndex < 0) {
        return -1;
    }

    return LastIndexOfInternal(mString, GetLength(),
            string, strlen(string), fromIndex);
}

int String::LastIndexOf(
    /* [in] */ const String& other,
    /* [in] */ int fromIndex) const
{
    if (other.IsEmpty()) {
        return -1;
    }

    if (IsEmpty() || fromIndex < 0) {
        return -1;
    }

    return LastIndexOfInternal(mString, GetLength(),
            other.mString, other.GetLength(), fromIndex);
}

int String::LastIndexOf(
    /* [in] */ int c,
    /* [in] */ int fromIndex) const
{
    if (c <= 0 || c > 255) {
        return -1;
    }
    if (IsEmpty() || fromIndex < 0) {
        return -1;
    }

    int pos = fromIndex < GetLength() ? fromIndex : GetLength() - 1;
    char* p = mString + pos;
    while (p != mString) {
        if (*p == c) {
            return p - mString;
        }
        p--;
    }
    return -1;
}

int String::LastIndexOfInternal(
    /* [in] */ const char* source,
    /* [in] */ int sourceLength,
    /* [in] */ const char* target,
    /* [in] */ int targetLength,
    /* [in] */ int fromIndex)
{
    int rightIndex = sourceLength - targetLength;
    if (fromIndex > rightIndex) {
        fromIndex = rightIndex;
    }

    int strLastIndex = targetLength - 1;
    char strLastChar = target[strLastIndex];
    int min = targetLength - 1;
    int i = min + fromIndex;

startSearchForLastChar:
    while (true) {
        while (i >= min && source[i] != strLastChar) {
            i--;
        }
        if (i < min) {
            return -1;
        }
        int j = i - 1;
        int start = j - (targetLength - 1);
        int k = strLastIndex - 1;

        while (j > start) {
            if (source[j--] != target[k--]) {
                i--;
                goto startSearchForLastChar;
            }
        }
        return start + 1;
    }
}

bool String::EndsWith(
    /* [in] */ const char* suffix) const
{
    int idx = LastIndexOf(suffix);
    if (idx == -1) {
        return false;
    }
    return idx + strlen(suffix) == GetLength();
}

bool String::EndsWith(
    /* [in] */ const String& suffix) const
{
    int idx = LastIndexOf(suffix);
    if (idx == -1) {
        return false;
    }
    return idx + suffix.GetLength() == GetLength();
}

String String::Trim() const
{
    if (IsEmpty()) {
        return nullptr;
    }

    int len = GetLength();

    const char* start = mString;
    while (*start && isspace(*start)) {
        ++start;
    }
    if (start - mString >= len) {
        return nullptr;
    }

    const char* end = mString + len - 1;
    while (isspace(*end) && end >= mString) {
        --end;
    }
    if (end < mString) {
        return nullptr;
    }
    return String(start, end - start + 1);
}

String String::TrimStart() const
{
    if (IsEmpty()) {
        return nullptr;
    }

    int len = GetLength();

    const char* start = mString;
    while (*start && isspace(*start)) {
        ++start;
    }
    return start - mString >= len ? nullptr :
            String(start, mString + len - start);
}

String String::TrimEnd() const
{
    if (IsEmpty()) {
        return nullptr;
    }

    const char* end = mString + GetLength() - 1;
    while (isspace(*end) && end >= mString) {
        --end;
    }
    return end < mString ? nullptr :
            String(mString, end - mString + 1);
}

String String::ToLowerCase() const
{
    if (IsEmpty()) {
        return *this;
    }

    int len = GetLength();
    String str(len);
    for (int i = 0; i < len; i++) {
        str.mString[i] = tolower(mString[i]);
    }
    str.mString[len] = '\0';
    return str;
}

String String::ToUpperCase() const
{
    if (IsEmpty()) {
        return *this;
    }

    int len = GetLength();
    String str(len);
    for (int i = 0; i < len; i++) {
        str.mString[i] = toupper(mString[i]);
    }
    str.mString[len] = '\0';
    return str;
}

String String::Substring(
    /* [in] */ int begin) const
{
    if (begin < 0 || begin >= GetLength()) {
        return nullptr;
    }

    return String(mString + begin);
}

String String::Substring(
    /* [in] */ int begin,
    /* [in] */ int end) const
{
    if (begin < 0 || begin > end || begin >= GetLength()) {
        return nullptr;
    }

    int len = end < GetLength() ? end - begin : GetLength() - begin;
    String str(len);
    memcpy(str.mString, mString + begin, len);
    str.mString[len] = '\0';

    return str;
}

String String::Replace(
    /* [in] */ const char* target,
    /* [in] */ const char* replacement) const
{
    if (target == nullptr || target[0] == '\0') {
        return *this;
    }
    if (replacement == nullptr) {
        return *this;
    }

    int index = IndexOf(target);
    if (index == -1) {
        return *this;
    }

    StringBuilder builder;
    int begin = 0, step = strlen(target);
    while (index != -1) {
        builder.Append(mString, begin, index);
        builder.Append(replacement);
        begin = index + step;
        index = IndexOf(target, begin);
    }
    if (begin < GetLength()) {
        builder.Append(mString, begin);
    }
    return builder.ToString();
}

String String::Replace(
    /* [in] */ int oldChar,
    /* [in] */ int newChar) const
{
    if (oldChar == newChar) {
        return *this;
    }

    int len = GetLength();
    for (int i = 0; i < len; i++) {
        if (mString[i] == oldChar) {
            String str(mString);
            for (int j = i; j < len; j++) {
                if (str.mString[j] == oldChar) {
                    str.mString[j] = newChar;
                }
            }
            return str;
        }
    }
    return *this;
}

}
