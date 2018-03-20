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

#ifndef __CCM_STRING_H__
#define __CCM_STRING_H__

#include <stdarg.h>
#include <string.h>

namespace ccm {

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

    inline bool IsNull() const
    { return mString == nullptr; }

    inline bool IsNullOrEmpty() const
    { return mString == nullptr || mString[0] == '\0'; }

    int GetLength() const;

    bool Equals(
        /* [in] */ const String& other) const;

    bool Equals(
        /* [in] */ const char* string) const;

    bool EqualsIgnoreCase(
        /* [in] */ const String& other) const;

    bool EqualsIgnoreCase(
        /* [in] */ const char* string) const;

    inline char* string() const
    { return mString; }

    inline operator char*() const
    { return mString; }

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
        /* [in] */ const String& other) const;

    int IndexOf(
        /* [in] */ const char* string) const;

    int LastIndexOf(
        /* [in] */ int c) const;

    int LastIndexOf(
        /* [in] */ const String& other) const;

    int LastIndexOf(
        /* [in] */ const char* string) const;

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

private:
    char* mString;
};

inline String operator+(
    /* [in] */ const String& string1,
    /* [in] */ const char* string2)
{
    String ret = string1;
    return ret += string2;
}

} // namespace ccm

#endif // __CCM_STRING_H__
