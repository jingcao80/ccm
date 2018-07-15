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
#include "util/ccmlogger.h"

#include <ctype.h>
#include <limits.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

namespace ccm {

char* EMPTY_STRING = nullptr;

extern void Init_EMPTY_STRING()
{
    SharedBuffer* buf = SharedBuffer::Alloc(1);
    EMPTY_STRING = (char*)buf->GetData();
    EMPTY_STRING[0] = '\0';
}

extern void Uninit_EMPTY_STRING()
{
    SharedBuffer::GetBufferFromData(EMPTY_STRING)->Release();
    EMPTY_STRING = nullptr;
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
    /* [in] */ const char* string,
    /* [in] */ Integer byteSize)
    : mString(nullptr)
    , mCharCount(0)
{
    if (string != nullptr && byteSize >= 0 && byteSize <= strlen(string)) {
        mString = AllocFromUTF8(string, byteSize);
    }
}

String::String(
    /* [in] */ const String& other)
{
    mString = other.mString;
    mCharCount = other.mCharCount;
    if (mString != nullptr) {
        SharedBuffer::GetBufferFromData(mString)->AddRef();
    }
}

String::String(
    /* [in] */ String&& other)
{
    mString = other.mString;
    mCharCount = other.mCharCount;
    other.mString = nullptr;
    other.mCharCount = 0;
}

String::String(
    /* [in] */ const Array<Char>& charArray,
    /* [in] */ Integer start)
    : mString(nullptr)
    , mCharCount(0)
{
    WriteCharArray(charArray, start, charArray.GetLength());
}

String::String(
    /* [in] */ const Array<Char>& charArray,
    /* [in] */ Integer start,
    /* [in] */ Integer length)
    : mString(nullptr)
    , mCharCount(0)
{
    WriteCharArray(charArray, start, length);
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
        charCount++;
    }

    SetCharCount(charCount);
    return charCount;
}

Integer String::GetUTF16Length(
    /* [in] */ Integer start) const
{
    if (IsNullOrEmpty()) return 0;

    Integer utf16Count = 0, charCount = 0;
    Integer byteSize;
    const char* p = mString;
    const char* end = mString + GetByteLength() + 1;
    while (*p != '\0' && p < end) {
        Char unicode = GetCharInternal(p, &byteSize);
        if (byteSize == 0 || p + byteSize >= end) break;
        p += byteSize;
        if (charCount >= start) {
            utf16Count++;
            if (unicode > 0xFFFF) utf16Count++; // this will be a surrogate pair in utf16
        }
        charCount++;
    }
    return utf16Count;
}

Integer String::GetByteLength() const
{
    if (mString == nullptr || mString[0] == '\0') return 0;

    return (Integer)SharedBuffer::GetBufferFromData(mString)->GetSize() - 1;
}

Integer String::GetHashCode() const
{
    // BKDR Hash Function
    unsigned int seed = 31; // 31 131 1313 13131 131313 etc..
    unsigned int hash = 0;

    const char* string = mString;
    if (string) {
        for ( ; *string; ++string) {
            hash = hash * seed + (*string);
        }
    }
    return (hash & 0x7FFFFFFF);
}

Char String::GetChar(
    /* [in] */ Integer index) const
{
    if (IsNullOrEmpty() || index < 0) return INVALID_CHAR;

    Integer byteSize;
    const char* p = mString;
    const char* end = mString + GetByteLength() + 1;
    while (*p && p < end) {
        Char unicode = GetCharInternal(p, &byteSize);
        if (byteSize == 0 || p + byteSize >= end) break;
        if (index == 0) {
            return unicode;
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
        Char unicode = GetCharInternal(p, &byteSize);
        if (byteSize == 0 || p + byteSize >= end) break;
        if (i >= start) {
            charArray[i - start] = unicode;
        }
        p += byteSize;
        i++;
    }

    return charArray;
}

ECode String::GetChars(
    /* [in] */ Integer srcBegin,
    /* [in] */ Integer srcEnd,
    /* [out] */ Array<Char>& dst,
    /* [in] */ Integer dstBegin) const
{
    if (srcBegin < 0 || srcEnd > GetLength() ||
            srcEnd < srcBegin || dst.IsNull() || dstBegin < 0 ||
            dstBegin > dst.GetLength() ||
            (srcEnd - srcBegin > dst.GetLength() - dstBegin)) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer byteSize, i = 0;
    const char* p = mString;
    const char* end = mString + GetByteLength() + 1;
    while (*p && p < end) {
        Char unicode = GetCharInternal(p, &byteSize);
        if (byteSize == 0 || p + byteSize >= end) break;
        if (i >= srcBegin && i < srcEnd) {
            dst[dstBegin + i - srcBegin] = unicode;
            if (i == srcEnd - 1) break;
        }
        p += byteSize;
        i++;
    }
    return NOERROR;
}

Array<Short> String::GetUTF16Chars(
    /* [in] */ Integer start) const
{
    Integer utf16Count = GetUTF16Length(start);
    if (start > utf16Count) {
        return Array<Short>();
    }

    Array<Short> utf16Array(utf16Count);

    Integer byteSize, count = 0, i = 0;
    const char* p = mString;
    const char* end = mString + GetByteLength() + 1;
    while (*p && p < end) {
        Char unicode = GetCharInternal(p, &byteSize);
        if (byteSize == 0 || p + byteSize >= end) break;
        if (count >= start) {
            if (unicode <= 0xFFFF) {
                utf16Array[i++] = unicode;
            }
            else {
                // Multiple UTF16 characters with surrogates
                unicode =  unicode - 0x10000;
                utf16Array[i++] = (Short)((unicode >> 10) + 0xD800);
                utf16Array[i++] = (Short)((unicode & 0x3FF) + 0xDC00);
            }
        }
        p += byteSize;
        count++;
    }

    return utf16Array;
}

ECode String::GetUTF16Chars(
    /* [in] */ Integer srcBegin,
    /* [in] */ Integer srcEnd,
    /* [out] */ Array<Short>& dst,
    /* [in] */ Integer dstBegin) const
{
    if (srcBegin < 0 || srcEnd > GetLength() ||
            srcEnd < srcBegin || dst.IsNull() || dstBegin < 0 ||
            dstBegin > dst.GetLength() ||
            (srcEnd - srcBegin > dst.GetLength() - dstBegin)) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer byteSize, count = 0, i = 0;
    const char* p = mString;
    const char* end = mString + GetByteLength() + 1;
    while (*p && p < end) {
        Char unicode = GetCharInternal(p, &byteSize);
        if (byteSize == 0 || p + byteSize >= end) break;
        if (count >= srcBegin && count < srcEnd) {
            if (unicode <= 0xFFFF) {
                dst[i++] = unicode;
            }
            else {
                // Multiple UTF16 characters with surrogates
                unicode =  unicode - 0x10000;
                dst[i++] = (Short)((unicode >> 10) + 0xD800);
                dst[i++] = (Short)((unicode & 0x3FF) + 0xDC00);
            }
            if (count == srcEnd - 1) break;
        }
        p += byteSize;
        count++;
    }
    return NOERROR;
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

String String::Substring(
    /* [in] */ Integer charStart,
    /* [in] */ Integer charEnd) const
{
    if (mString == nullptr) return String();
    if (charStart < 0 || charEnd < 0 ||
        charStart > charEnd || charStart >= GetLength()) {
        return String("");
    }

    Integer charCount = 0;
    Integer byteSize;
    const char* p = mString;
    const char* end = mString + GetByteLength() + 1;
    const char* p1 = p;
    const char* p2 = end;
    while (*p != '\0' && p < end) {
        byteSize = UTF8SequenceLength(*p);
        if (byteSize == 0 || p + byteSize >= end) break;
        if (charCount == charStart) {
            p1 = p;
        }
        p += byteSize;
        if (charCount == charEnd) {
            p2 = p;
            break;
        }
        charCount++;
    }

    return String(p1, (p2 - 1) - p1);
}

Integer String::IndexOf(
    /* [in] */ Char c,
    /* [in] */ Integer fromCharIndex) const
{
    if (fromCharIndex < 0) return -1;

    Integer byteSize, i = 0;
    const char* p = mString;
    const char* end = mString + GetByteLength() + 1;
    while (*p && p < end) {
        Char unicode = GetCharInternal(p, &byteSize);
        if (byteSize == 0 || p + byteSize >= end) break;
        if (i >= fromCharIndex) {
            if (c == unicode) return i;
        }
        p += byteSize;
        i++;
    }
    return -1;
}

Integer String::IndexOf(
    /* [in] */ const char* string,
    /* [in] */ Integer fromCharIndex) const
{
    if (string == nullptr || string[0] == '\0') {
        return -1;
    }

    Integer i = 0;
    Integer byteSize;
    const char* p = mString;
    const char* end = mString + GetByteLength() + 1;
    const char* psub = nullptr;
    while (*p && p < end) {
        byteSize = UTF8SequenceLength(*p);
        if (byteSize == 0 || p + byteSize >= end) break;
        if (i >= fromCharIndex && psub == nullptr) {
            psub = strstr(p, string);
            if (psub == nullptr) {
                return -1;
            }
        }
        if (p == psub) {
            return i;
        }
        p += byteSize;
        i++;
    }
    return -1;
}

Integer String::LastIndexOf(
    /* [in] */ Char c) const
{
    Integer byteSize = GetByteSize(c);
    char buf[5];
    WriteUTF8Bytes(buf, c, byteSize);
    buf[byteSize] = '\0';
    return LastIndexOf(buf);
}

Integer String::LastIndexOf(
    /* [in] */ Char c,
    /* [in] */ Integer fromCharIndex) const
{
    Integer byteSize = GetByteSize(c);
    char buf[5];
    WriteUTF8Bytes(buf, c, byteSize);
    buf[byteSize] = '\0';
    return LastIndexOf(buf, fromCharIndex);
}

Integer String::LastIndexOf(
    /* [in] */ const char* string) const
{
    if (string == nullptr || string[0] == '\0') {
        return -1;
    }
    Integer byteIndex = LastByteIndexOfInternal(
            string, GetByteLength() - 1);
    return ToCharIndex(byteIndex);
}

Integer String::LastIndexOf(
    /* [in] */ const char* string,
    /* [in] */ Integer fromCharIndex) const
{
    if (string == nullptr || string[0] == '\0') {
        return -1;
    }
    Integer charByteSize;
    Integer fromByteIndex = ToByteIndex(
            fromCharIndex, &charByteSize);
    Integer byteIndex = LastByteIndexOfInternal(
            string, fromByteIndex + charByteSize);
    return ToCharIndex(byteIndex);
}

Boolean String::EndsWith(
    /* [in] */ const String& prefix) const
{
    Integer fromCharIndex = GetLength() - prefix.GetLength();
    return IndexOf(prefix, fromCharIndex) == fromCharIndex;
}

String& String::operator=(
    /* [in] */ const String& other)
{
    if (mString == other.mString) {
        return *this;
    }

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
    /* [in] */ String&& other)
{
    if (mString != nullptr) {
        SharedBuffer::GetBufferFromData(mString)->Release();
    }
    mString = other.mString;
    mCharCount = other.mCharCount;
    other.mString = nullptr;
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

String& String::operator+=(
    /* [in] */ const String& other)
{
    if (other.IsNullOrEmpty()) {
        return *this;
    }

    Integer origByteSize = GetByteLength();
    Integer newByteSize = origByteSize + other.GetByteLength();
    char* buf = LockBuffer(newByteSize);
    if (buf == nullptr) {
        return *this;
    }

    memcpy(buf + origByteSize, other.string(), other.GetByteLength());
    buf[newByteSize] = '\0';
    UnlockBuffer(newByteSize);
    return *this;
}

String& String::operator+=(
    /* [in] */ const char* string)
{
    if (string == nullptr || string[0] == '\0') {
        return *this;
    }

    Integer origByteSize = GetByteLength();
    Integer stringByteSize = strlen(string);
    Integer newByteSize = origByteSize + stringByteSize;
    char* buf = LockBuffer(newByteSize);
    if (buf == nullptr) {
        return *this;
    }

    memcpy(buf + origByteSize, string, stringByteSize);
    buf[newByteSize] = '\0';
    UnlockBuffer(newByteSize);
    return *this;
}

String String::ToLowerCase() const
{
    String lowerStr(nullptr);
    for (Integer i = 0; i < GetByteLength(); i++) {
        char l = tolower(mString[i]);
        if (l != mString[i]) {
            if (lowerStr.IsNull()) {
                lowerStr = String(mString, GetByteLength());
            }
            lowerStr.mString[i] = l;
        }
    }
    return lowerStr.IsNull() ? *this : lowerStr;
}

String String::ToUpperCase() const
{
    String upperStr(nullptr);
    for (Integer i = 0; i < GetByteLength(); i++) {
        char l = toupper(mString[i]);
        if (l != mString[i]) {
            if (upperStr.IsNull()) {
                upperStr = String(mString, GetByteLength());
            }
            upperStr.mString[i] = l;
        }
    }
    return upperStr.IsNull() ? *this : upperStr;
}

String String::Replace(
    /* [in] */ Char oldChar,
    /* [in] */ Char newChar) const
{
    if (oldChar == newChar) return *this;

    String newStr(nullptr);

    Boolean found = false;
    Array<Char> charArray = GetChars();
    for (Integer i = 0; i < charArray.GetLength(); i++) {
        if (charArray[i] == oldChar) {
            charArray[i] = newChar;
            found = true;
        }
    }
    if (found) {
        newStr = String(charArray);
    }
    return newStr.IsNull() ? *this : newStr;
}

String String::Replace(
    /* [in] */ const char* target,
    /* [in] */ const char* replacement) const
{
    if (target == nullptr || target[0] == '\0' ||
            replacement == nullptr) {
        return *this;
    }

    char* index = strstr(mString, target);
    if (index == nullptr) {
        return *this;
    }

    String newStr(nullptr);
    char* p = mString;
    Integer step = strlen(target);
    while (index != nullptr) {
        newStr.AppendBytes(p, index - p - 1);
        newStr.AppendBytes(replacement, step);
        p = index + step;
        index = strstr(p, target);
    }
    return newStr;
}

String String::Trim() const
{
    if (mString == nullptr) {
        return String(nullptr);
    }

    Integer byteSize = GetByteLength();

    const char* start = mString;
    while (*start && isspace(*start)) {
        ++start;
    }
    if (start - mString >= byteSize) {
        return String(nullptr);
    }

    const char* end = mString + byteSize - 1;
    while (isspace(*end) && end >= mString) {
        --end;
    }
    if (end < mString) {
        return String(nullptr);
    }
    return String(start, end - start + 1);
}

String String::TrimStart() const
{
    if (mString == nullptr) {
        return String(nullptr);
    }

    Integer byteSize = GetByteLength();

    const char* start = mString;
    while (*start && isspace(*start)) {
        ++start;
    }
    return start - mString >= byteSize ? String(nullptr) :
            String(start, mString + byteSize - start);
}

String String::TrimEnd() const
{
    if (mString == nullptr) {
        return String(nullptr);
    }

    const char* end = mString + GetByteLength() - 1;
    while (isspace(*end) && end >= mString) {
        --end;
    }
    return end < mString ? String(nullptr) :
            String(mString, end - mString + 1);
}

Integer String::ToByteIndex(
    /* [in] */ Integer charIndex,
    /* [in] */ Integer* charByteSize) const
{
    if (charIndex < 0 || charIndex >= GetLength()) {
        return -1;
    }

    Integer charCount = 0;
    Integer byteSize;
    const char* p = mString;
    const char* end = mString + GetByteLength() + 1;
    while (*p != '\0' && p < end) {
        byteSize = UTF8SequenceLength(*p);
        if (byteSize == 0 || p + byteSize >= end) break;
        if (charCount == charIndex) {
            if (charByteSize != nullptr) {
                *charByteSize = byteSize;
            }
            return p - mString;
        }
        p += byteSize;
        charCount++;
    }

    return -1;
}

Integer String::ToCharIndex(
    /* [in] */ Integer byteIndex,
    /* [in] */ Integer* charByteSize) const
{
    if (byteIndex < 0 || byteIndex > GetByteLength()) {
        return -1;
    }

    Integer charIndex = 0;
    Integer byteSize;
    const char* p = mString;
    const char* end = mString + GetByteLength() + 1;
    while (*p != '\0' && p < end) {
        byteSize = UTF8SequenceLength(*p);
        if (byteSize == 0 || p + byteSize >= end) break;
        if (byteIndex >= p - mString && byteIndex < p - mString + byteSize) {
            if (charByteSize != nullptr) {
                *charByteSize = byteSize;
            }
            return charIndex;
        }
        p += byteSize;
        charIndex++;
    }

    return -1;
}

String String::Format(
    /* [in] */ const char* format ...)
{
    va_list args, args1;

    va_start(args, format);
    va_copy(args1, args);

    int len = vsnprintf(nullptr, 0, format, args);
    va_end(args);

    String str("");
    char* buf = str.LockBuffer(len);
    if (buf == nullptr) {
        Logger::E("String", "Lock %d bytes buffer failed", len);
        return str;
    }
    vsnprintf(buf, len + 1, format, args1);
    str.UnlockBuffer(len);
    va_end(args1);

    return str;
}

String String::ValueOfChar(
    /* [in] */ Char c)
{
    Integer byteSize = GetByteSize(c);
    char buf[5];
    WriteUTF8Bytes(buf, c, byteSize);
    buf[byteSize] = '\0';
    return String(buf);
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

Integer String::GetByteSize(
    /* [in] */ Char c)
{
    if ((c > MAX_CODE_POINT) ||
            (MIN_HIGH_SURROGATE <= c && c <= MAX_LOW_SURROGATE)) {
        return 0;
    }

    Integer byteSize = 4;

    // Figure out how many bytes the result will require.
    if (c < 0x00000080) {
        byteSize = 1;
    }
    else if (c < 0x00000800) {
        byteSize = 2;
    }
    else if (c < 0x00010000) {
        byteSize = 3;
    }

    return byteSize;
}

void String::WriteCharArray(
    /* [in] */ const Array<Char>& charArray,
    /* [in] */ Integer start,
    /* [in] */ Integer length)
{
    if (start < 0 || start > charArray.GetLength() || length < 0) {
        return;
    }
    length = start + length <= charArray.GetLength() ?
            length : charArray.GetLength() - start;

    Integer totalByteSize = 0;
    for (Integer i = start; i < length; i++) {
        totalByteSize += GetByteSize(charArray[i]);
    }

    char* buf = LockBuffer(totalByteSize);
    if (buf == nullptr) {
        return;
    }

    for (Integer i = start; i < length; i++) {
        Integer byteSize = GetByteSize(charArray[i]);
        WriteUTF8Bytes(buf, charArray[i], byteSize);
        buf += byteSize;
    }
    UnlockBuffer(totalByteSize);
    buf[totalByteSize] = '\0';
}

void String::AppendBytes(
    /* [in] */ const char* string,
    /* [in] */ Integer byteSize)
{
    Integer oldSize = GetByteLength();
    Integer newByteSize = oldSize + byteSize;
    char* buf = LockBuffer(newByteSize);
    if (buf == nullptr) return;
    memcpy(buf + oldSize, string, byteSize);
    UnlockBuffer(newByteSize);
    buf[newByteSize] = '\0';
}

Integer String::LastByteIndexOfInternal(
    /* [in] */ const char* string,
    /* [in] */ Integer fromByteIndex) const
{
    Integer fromIndex = fromByteIndex;
    Integer sourceLength = GetByteLength();
    Integer targetLength = strlen(string);
    Integer rightIndex = sourceLength - targetLength;
    if (fromIndex > rightIndex) {
        fromIndex = rightIndex;
    }

    Integer strLastIndex = targetLength - 1;
    char strLastChar = string[strLastIndex];
    Integer min = targetLength - 1;
    Integer i = min + fromIndex;

startSearchForLastChar:
    while (true) {
        while (i >= min && mString[i] != strLastChar) {
            i--;
        }
        if (i < min) {
            return -1;
        }
        Integer j = i - 1;
        Integer start = j - (targetLength - 1);
        Integer k = strLastIndex - 1;

        while (j > start) {
            if (mString[j--] != string[k--]) {
                i--;
                goto startSearchForLastChar;
            }
        }
        return start + 1;
    }
}

char* String::LockBuffer(
    /* [in] */ Integer byteSize)
{
    if (byteSize < 0) return nullptr;

    SharedBuffer* buf;
    if (mString != nullptr) {
        buf = SharedBuffer::GetBufferFromData(mString)->EditResize(
                byteSize + 1);
    }
    else {
        buf = SharedBuffer::Alloc(byteSize + 1);
    }
    if (buf == nullptr) return nullptr;

    mString = (char*)buf->GetData();
    return mString;
}

ECode String::UnlockBuffer(
    /* [in] */ Integer byteSize)
{
    if (byteSize < 0) return E_ILLEGAL_ARGUMENT_EXCEPTION;

    if (byteSize != GetByteLength()) {
        SharedBuffer* buf;
        if (mString != nullptr) {
            buf = SharedBuffer::GetBufferFromData(mString)->EditResize(
                    byteSize + 1);
        }
        else {
            buf = SharedBuffer::Alloc(byteSize + 1);
        }
        if (buf == nullptr) {
            Logger::E("String", "Unlock %d bytes buffer failed",
                    byteSize);
            return E_OUT_OF_MEMORY_ERROR;
        }

        mString = (char*)buf->GetData();
        mString[byteSize] = 0;
    }

    return NOERROR;
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

static const Char kByteMask               = 0x000000BF;
static const Char kByteMark               = 0x00000080;

// Mask used to set appropriate bits in first byte of UTF-8 sequence,
// indexed by number of bytes in the sequence.
// 0xxxxxxx
// -> (00-7f) 7bit. Bit mask for the first byte is 0x00000000
// 110yyyyx 10xxxxxx
// -> (c0-df)(80-bf) 11bit. Bit mask is 0x000000C0
// 1110yyyy 10yxxxxx 10xxxxxx
// -> (e0-ef)(80-bf)(80-bf) 16bit. Bit mask is 0x000000E0
// 11110yyy 10yyxxxx 10xxxxxx 10xxxxxx
// -> (f0-f7)(80-bf)(80-bf)(80-bf) 21bit. Bit mask is 0x000000F0
static const Char kFirstByteMark[] = {
    0x00000000, 0x00000000, 0x000000C0, 0x000000E0, 0x000000F0
};

void String::WriteUTF8Bytes(
    /* [in] */ char* dst,
    /* [in] */ Char c,
    /* [in] */ Integer bytes)
{
    dst += bytes;
    switch (bytes) {
        /* note: everything falls through. */
        case 4: *--dst = (Byte)((c | kByteMark) & kByteMask); c >>= 6;
        case 3: *--dst = (Byte)((c | kByteMark) & kByteMask); c >>= 6;
        case 2: *--dst = (Byte)((c | kByteMark) & kByteMask); c >>= 6;
        case 1: *--dst = (Byte)(c | kFirstByteMark[bytes]);
    }
}

}
