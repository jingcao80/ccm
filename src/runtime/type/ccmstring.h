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

namespace ccm {

template<class T> class Array;

class String
{
public:
    inline String();

    explicit String(
        /* [in] */ const char* string);

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

    String& operator=(
        /* [in] */ const String& other);

    String& operator=(
        /* [in] */ const char* string);

    inline static Boolean IsASCII(
        /* [in] */ char c);

    static Integer UTF8SequenceLengthNonASCII(
        /* [in] */ char b0);

    inline static Integer UTF8SequenceLength(
        /* [in] */ char b0);

private:
    void SetCharCount(
        /* [in] */ Integer charCount) const;

    Integer GetCharCount() const;

    Boolean IsCounted() const;

    void ClearCounted();

    static Char GetCharInternal(
        /* [in] */ const char* cur,
        /* [in] */ Integer* byteSize);

public:
    static constexpr Char INVALID_CHAR = 0x110000;

private:
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

}

#endif //__CCM_STRING_H__
