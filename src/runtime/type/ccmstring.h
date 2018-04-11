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

#ifndef __CCM_STRING_H__
#define __CCM_STRING_H__

#include "ccmdef.h"

namespace ccm {

template<class T> class Array;

class COM_PUBLIC String
{
public:
    inline String();

    explicit String(
        /* [in] */ const char* string);

    explicit String(
        /* [in] */ const char* string,
        /* [in] */ Integer byteSize);

    String(
        /* [in] */ const String& other);

    String(
        /* [in] */ String&& other);

    ~String();

    inline Boolean IsNull() const;

    inline Boolean IsEmpty() const;

    inline Boolean IsNullOrEmpty() const;

    Integer GetLength() const;

    Integer GetUTF16Length(
        /* [in] */ Integer start = 0) const;

    Integer GetByteLength() const;

    inline const char* string() const;

    inline operator const char*() const;

    Char GetChar(
        /* [in] */ Integer index) const;

    Array<Char> GetChars(
        /* [in] */ Integer start = 0) const;

    Array<Short> GetUTF16Chars(
        /* [in] */ Integer start = 0) const;

    inline Integer Compare(
        /* [in] */ const String& other) const;

    Integer Compare(
        /* [in] */ const char* string) const;

    Integer CompareIgnoreCase(
        /* [in] */ const char* string) const;

    inline Integer CompareIgnoreCase(
        /* [in] */ const String& other) const;

    inline Boolean Equals(
        /* [in] */ const String& other) const;

    inline Boolean Equals(
        /* [in] */ const char* string) const;

    inline Boolean EqualsIgnoreCase(
        /* [in] */ const String& other) const;

    inline Boolean EqualsIgnoreCase(
        /* [in] */ const char* string) const;

    inline String Substring(
        /* [in] */ Integer charStart) const;

    String Substring(
        /* [in] */ Integer charStart,
        /* [in] */ Integer charEnd) const;

    inline Boolean Contains(
        /* [in] */ const String& other) const;

    inline Boolean Contains(
        /* [in] */ const char* string) const;

    Integer IndexOf(
        /* [in] */ Char c,
        /* [in] */ Integer fromCharIndex = 0) const;

    inline Integer IndexOf(
        /* [in] */ const String& other,
        /* [in] */ Integer fromCharIndex = 0) const;

    Integer IndexOf(
        /* [in] */ const char* string,
        /* [in] */ Integer fromCharIndex = 0) const;

    Integer LastIndexOf(
        /* [in] */ Char c) const;

    Integer LastIndexOf(
        /* [in] */ Char c,
        /* [in] */ Integer fromCharIndex) const;

    inline Integer LastIndexOf(
        /* [in] */ const String& other) const;

    inline Integer LastIndexOf(
        /* [in] */ const String& other,
        /* [in] */ Integer fromCharIndex) const;

    Integer LastIndexOf(
        /* [in] */ const char* string) const;

    Integer LastIndexOf(
        /* [in] */ const char* string,
        /* [in] */ Integer fromCharIndex) const;

    Integer ToByteIndex(
        /* [in] */ Integer charIndex,
        /* [in] */ Integer* charByteSize = nullptr) const;

    Integer ToCharIndex(
        /* [in] */ Integer byteIndex,
        /* [in] */ Integer* charByteSize = nullptr) const;

    String& operator=(
        /* [in] */ const String& other);

    String& operator=(
        /* [in] */ String&& other);

    String& operator=(
        /* [in] */ const char* string);

    String& operator+=(
        /* [in] */ const String& other);

    String& operator+=(
        /* [in] */ const char* string);

    static String Format(
        /* [in] */ const char* format ...);

    inline static Boolean IsASCII(
        /* [in] */ char c);

    static Integer UTF8SequenceLengthNonASCII(
        /* [in] */ char b0);

    inline static Integer UTF8SequenceLength(
        /* [in] */ char b0);

    static Integer GetByteSize(
        /* [in] */ Char c);

private:
    Integer LastByteIndexOfInternal(
        /* [in] */ const char* string,
        /* [in] */ Integer fromByteIndex) const;

    char* LockBuffer(
        /* [in] */ Integer byteSize);

    ECode UnlockBuffer(
        /* [in] */ Integer byteSize);

    void SetCharCount(
        /* [in] */ Integer charCount) const;

    Integer GetCharCount() const;

    Boolean IsCounted() const;

    void ClearCounted();

    static Char GetCharInternal(
        /* [in] */ const char* cur,
        /* [in] */ Integer* byteSize);

    static void WriteUTF8Bytes(
        /* [in] */ char* dst,
        /* [in] */ Char c,
        /* [in] */ Integer bytes);

public:
    static constexpr Char INVALID_CHAR = 0x110000;

private:
    static constexpr Integer MIN_CODE_POINT = 0x000000;
    static constexpr Integer MAX_CODE_POINT = 0x10FFFF;
    static constexpr Integer MIN_HIGH_SURROGATE = 0xD800;
    static constexpr Integer MAX_HIGH_SURROGATE = 0xDBFF;
    static constexpr Integer MIN_LOW_SURROGATE = 0xDC00;
    static constexpr Integer MAX_LOW_SURROGATE = 0xDFFF;

    char* mString;
    mutable Integer mCharCount;
};

String::String()
    : mString(nullptr)
    , mCharCount(0)
{}

Boolean String::IsNull() const
{
    return mString == nullptr;
}

Boolean String::IsEmpty() const
{
    return mString[0] == '\0';
}

Boolean String::IsNullOrEmpty() const
{
    return mString == nullptr || mString[0] == '\0';
}

const char* String::string() const
{
    return mString;
}

String::operator const char*() const
{
    return mString;
}

Integer String::Compare(
    /* [in] */ const String& other) const
{
    return Compare(other.mString);
}

Integer String::CompareIgnoreCase(
    /* [in] */ const String& other) const
{
    return CompareIgnoreCase(other.mString);
}

Boolean String::Equals(
    /* [in] */ const String& other) const
{
    return Compare(other.mString) == 0;
}

Boolean String::Equals(
    /* [in] */ const char* string) const
{
    return Compare(string) == 0;
}

Boolean String::EqualsIgnoreCase(
    /* [in] */ const String& other) const
{
    return CompareIgnoreCase(other.mString) == 0;
}

Boolean String::EqualsIgnoreCase(
    /* [in] */ const char* string) const
{
    return CompareIgnoreCase(string) == 0;
}

String String::Substring(
    /* [in] */ Integer charStart) const
{
    return Substring(charStart, GetLength());
}

Boolean String::Contains(
    /* [in] */ const String& other) const
{
    return IndexOf(other) != -1;
}

Boolean String::Contains(
    /* [in] */ const char* string) const
{
    return IndexOf(string) != -1;
}

Integer String::IndexOf(
    /* [in] */ const String& other,
    /* [in] */ Integer fromCharIndex) const
{
    return IndexOf(other.string(), fromCharIndex);
}

Integer String::LastIndexOf(
    /* [in] */ const String& other) const
{
    return LastIndexOf(other.string());
}

Integer String::LastIndexOf(
    /* [in] */ const String& other,
    /* [in] */ Integer fromCharIndex) const
{
    return LastIndexOf(other.string(), fromCharIndex);
}

Boolean String::IsASCII(
    /* [in] */ char c)
{
    return (c & 0x80) == 0;
}

Integer String::UTF8SequenceLength(
    /* [in] */ char b0)
{
    return IsASCII(b0) ? 1 : UTF8SequenceLengthNonASCII(b0);
}

inline String operator+(
    /* [in] */ const String& string1,
    /* [in] */ const String& string2)
{
    String ret = string1;
    return ret += string2;
}

inline String operator+(
    /* [in] */ const String& string1,
    /* [in] */ const char* string2)
{
    String ret = string1;
    return ret += string2;
}

}

#endif //__CCM_STRING_H__
