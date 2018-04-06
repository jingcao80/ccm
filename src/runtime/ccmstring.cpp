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

//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
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

#include "ccmsharedbuffer.h"
#include "ccmtypes.h"
#include "util/logger.h"

#include <limits.h>
#include <string.h>

namespace ccm {

char* EMPTY_STRING = nullptr;

static void Init_EMPTY_STRING()
{
    SharedBuffer* buf = SharedBuffer::Alloc(1);
    EMPTY_STRING = (char*)buf->GetData();
    EMPTY_STRING[0] = '\0';
}

static void Uninit_EMPTY_STRING()
{
    SharedBuffer::GetBufferFromData(EMPTY_STRING)->Release();
    EMPTY_STRING = NULL;
}

static char* GetEmptyString()
{
    SharedBuffer::GetBufferFromData(EMPTY_STRING)->AddRef();
    return EMPTY_STRING;
}

static char* AllocFromUTF8(
    /* [in] */ const char* string, size_t byteSize)
{
    if (byteSize == 0) return GetEmptyString();
    if (byteSize > INT_MAX) {
        Logger::E("String", "Invalid buffer size %zu", byteSize);
        return nullptr;
    }

    SharedBuffer* buf = SharedBuffer::Alloc(byteSize + 1);
    if (buf == nullptr) {
        Logger::E("String", "Malloc string which size is %zu failed.", byteSize);
        return nullptr;
    }

    char* str = (char*)buf->GetData();
    memcpy(str, string, byteSize);
    str[byteSize] = '\0';
    return str;
}

String::String(
    /* [in] */ const char* string)
    : mString(nullptr)
    , mCharCount(0)
{
    if (string != nullptr) {
        mString = AllocFromUTF8(string, strlen(string));
    }
}

String::String(
    /* [in] */ const String& other)
{
    mString = other.mString;
    mCharCount = other.mCharCount;
    SharedBuffer::GetBufferFromData(mString)->AddRef();
}

String::String(
    /* [in] */ String&& other)
{
    mString = other.mString;
    mCharCount = other.mCharCount;
    other.mString = nullptr;
    other.mCharCount = 0;
}

String::~String()
{
    if (mString != nullptr) {
        SharedBuffer::GetBufferFromData(mString)->Release();
        mString = nullptr;
        mCharCount = 0;
    }
}

Integer String::GetLength() const
{
    if (IsNullOrEmpty()) return 0;
    if (IsCounted()) return GetCharCount();

    Integer charCount = 0;
    Integer byteSize;
    const char* p = mString;
    const char* end = mString + GetByteLength() + 1;
    while (*p != '\0' && p < end) {
        byteSize = UTF8SequenceLength(*p);
        if (byteSize == 0 || p + byteSize >= end) break;
        p += byteSize;
        ++charCount;
    }

    SetCharCount(charCount);
    return charCount;
}

Integer String::GetByteLength() const
{
    if (mString == nullptr || mString[0] == '\0') return 0;

    return (Integer)SharedBuffer::GetBufferFromData(mString)->GetSize() - 1;
}

Char String::GetChar(
    /* [in] */ Integer index) const
{
    if (IsNullOrEmpty() || index < 0) return INVALID_CHAR;

    Integer byteSize;
    const char* p = mString;
    const char* end = mString + GetByteLength() + 1;
    while (*p && p < end) {
        byteSize = UTF8SequenceLength(*p);
        if (byteSize == 0 || p + byteSize >= end) break;
        if (index == 0) {
            return GetCharInternal(p, &byteSize);
        }
        p += byteSize;
        index -= 1;
    }

    return INVALID_CHAR;
}

Array<Char> String::GetChars(
    /* [in] */ Integer start) const
{
    Integer charCount = GetLength();
    if (start >= charCount) {
        return Array<Char>();
    }

    Array<Char> charArray(charCount - start);

    Integer byteSize, i = 0;
    const char* p = mString;
    const char* end = mString + GetByteLength() + 1;
    while (*p && p < end) {
        byteSize = UTF8SequenceLength(*p);
        if (byteSize == 0 || p + byteSize >= end) break;
        if (i >= start) {
            charArray[i - start] = GetCharInternal(p, &byteSize);
        }
        p += byteSize;
        i++;
    }

    return charArray;
}

Integer String::Compare(
    /* [in] */ const char* string) const
{
    if (mString == string) return 0;
    if (mString == nullptr) return -1;
    if (string == nullptr) return 1;

    if (mString[0] == '\0' && string[0] == '\0') {
        return 0;
    }

    return strcmp(mString, string);
}

Integer String::CompareIgnoreCase(
    /* [in] */ const char* string) const
{
    if (mString == string) return 0;
    if (mString == nullptr) return -1;
    if (string == nullptr) return 1;

    if (mString[0] == '\0' && string[0] == '\0') {
        return 0;
    }

    return strcasecmp(mString, string);
}

String& String::operator=(
    /* [in] */ const String& other)
{
    if (other.mString != nullptr) {
        SharedBuffer::GetBufferFromData(other.mString)->AddRef();
    }
    if (mString != nullptr) {
        SharedBuffer::GetBufferFromData(mString)->Release();
    }
    mString = other.mString;
    mCharCount = other.mCharCount;
    return *this;
}

String& String::operator=(
    /* [in] */ const char* string)
{
    if (mString != nullptr) {
        SharedBuffer::GetBufferFromData(mString)->Release();
    }
    mString = string != nullptr ?
        AllocFromUTF8(string, strlen(string)) : nullptr;
    mCharCount = 0;
    return *this;
}

Integer String::UTF8SequenceLengthNonASCII(
    /* [in] */ char b0)
{
    if ((b0 & 0xC0) != 0xC0) {
        return 0;
    }
    if ((b0 & 0xE0) == 0xC0) {
        return 2;
    }
    if ((b0 & 0xF0) == 0xE0) {
        return 3;
    }
    if ((b0 & 0xF8) == 0xF0){
        return 4;
    }
    return 0;
}

void String::SetCharCount(
    /* [in] */ Integer charCount) const
{
    mCharCount = (0x7FFFFFFF & charCount);
    mCharCount |= (1 << 31);
}

Integer String::GetCharCount() const
{
    return (0x7FFFFFFF & mCharCount);
}

Boolean String::IsCounted() const
{
    return (mCharCount & (1 << 31)) != 0;
}

void String::ClearCounted()
{
    mCharCount = 0;
}

Char String::GetCharInternal(
    /* [in] */ const char* cur,
    /* [in] */ Integer* byteSize)
{
    if (IsASCII(*cur)) {
        *byteSize = 1;
        return *cur;
    }

    const char first_char = *cur++;
    Char result = first_char;
    Char mask, to_ignore_mask;
    Integer num_to_read = 0;
    for (num_to_read = 1, mask = 0x40, to_ignore_mask = 0xFFFFFF80;
         (first_char & mask);
         num_to_read++, to_ignore_mask |= mask, mask >>= 1) {
        // 0x3F == 00111111
        result = (result << 6) + (*cur++ & 0x3F);
    }
    to_ignore_mask |= mask;
    result &= ~(to_ignore_mask << (6 * (num_to_read - 1)));
    *byteSize = num_to_read;
    return result;
}

}
