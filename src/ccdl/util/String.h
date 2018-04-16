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

#ifndef __CCDL_STRING_H__
#define __CCDL_STRING_H__

#include <stdarg.h>
#include <string.h>

namespace ccdl {

class String
{
public:
    String()
        : mString(nullptr)
    {}

    explicit String(
        /* [in] */ const char* string);

    String(
        /* [in] */ const String& other);

    String(
        /* [in] */ String&& other);

    ~String();

    inline bool IsNull() const;

    inline bool IsEmpty() const;

    inline bool IsNullOrEmpty() const;

    int GetLength() const;

    bool Equals(
        /* [in] */ const String& other) const;

    bool Equals(
        /* [in] */ const char* string) const;

    bool EqualsIgnoreCase(
        /* [in] */ const String& other) const;

    bool EqualsIgnoreCase(
        /* [in] */ const char* string) const;

    inline const char* string() const;

    inline operator const char*() const;

    char GetChar(
        /* [in] */ int index) const;

    String Substring(
        /* [in] */ int begin) const;

    String Substring(
        /* [in] */ int begin,
        /* [in] */ int end) const;

    bool Contains(
        /* [in] */ const String& other) const;

    bool Contains(
        /* [in] */ const char* string) const;

    int IndexOf(
        /* [in] */ int c) const;

    int IndexOf(
        /* [in] */ int c,
        /* [in] */ int fromIndex) const;

    int IndexOf(
        /* [in] */ const String& other) const;

    int IndexOf(
        /* [in] */ const String& other,
        /* [in] */ int fromIndex) const;

    int IndexOf(
        /* [in] */ const char* string) const;

    int IndexOf(
        /* [in] */ const char* string,
        /* [in] */ int fromIndex) const;

    int LastIndexOf(
        /* [in] */ int c) const;

    int LastIndexOf(
        /* [in] */ int c,
        /* [in] */ int fromIndex) const;

    int LastIndexOf(
        /* [in] */ const String& other) const;

    int LastIndexOf(
        /* [in] */ const String& other,
        /* [in] */ int fromIndex) const;

    int LastIndexOf(
        /* [in] */ const char* string) const;

    int LastIndexOf(
        /* [in] */ const char* string,
        /* [in] */ int fromIndex) const;

    bool StartsWith(
        /* [in] */ const String& prefix) const;

    bool StartsWith(
        /* [in] */ const char* prefix) const;

    String ToLowerCase() const;

    String ToUpperCase() const;

    String Replace(
        /* [in] */ int oldChar,
        /* [in] */ int newChar) const;

    String Replace(
        /* [in] */ const char* target,
        /* [in] */ const char* replacement) const;

    String Replace(
        /* [in] */ const String& target,
        /* [in] */ const String& replacement) const;

    String& operator=(
        /* [in] */ const char* string);

    String& operator=(
        /* [in] */ const String& other);

    String& operator=(
        /* [in] */ String&& other);

    String& operator+=(
        /* [in] */ const char* string);

    String& operator+=(
        /* [in] */ const String& other);

    static String Format(
        /* [in] */ const char* format ...);

private:
    String(
        /* [in] */ int size);

    int LastIndexOfInternal(
        /* [in] */ const char* string,
        /* [in] */ int fromIndex) const;

private:
    char* mString;
};

bool String::IsNull() const
{
    return mString == nullptr;
}

bool String::IsEmpty() const
{
    return mString[0] == '\0';
}

bool String::IsNullOrEmpty() const
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

} // namespace ccdl

#endif // __CCDL_STRING_H__
