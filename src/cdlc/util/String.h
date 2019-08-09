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

#ifndef __CDLC_STRING_H__
#define __CDLC_STRING_H__

#include <cstddef>

namespace cdlc {

class String
{
public:
    inline String();

    String(
        /* [in] */ const char* string);

    String(
        /* [in] */ const char* string,
        /* [in] */ int size);

    String(
        /* [in] */ const String& other);

    String(
        /* [in] */ String&& other);

    ~String();

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
        /* [in] */ const char* format, ...);

    inline bool IsNull() const;

    inline bool IsEmpty() const;

    int GetLength() const;

    int GetHashCode() const;

    inline const char* string() const;

    inline operator const char*() const;

    char GetChar(
        /* [in] */ int index) const;

    int Compare(
        /* [in] */ const char* string) const;

    inline int Compare(
        /* [in] */ const String& other) const;

    bool Equals(
        /* [in] */ const char* string) const;

    bool Equals(
        /* [in] */ const String& other) const;

    bool EqualsIgnoreCase(
        /* [in] */ const char* string) const;

    bool EqualsIgnoreCase(
        /* [in] */ const String& other) const;

    inline bool Contains(
        /* [in] */ const char* string) const;

    inline bool Contains(
        /* [in] */ const String& other) const;

    inline int IndexOf(
        /* [in] */ const char* string) const;

    int IndexOf(
        /* [in] */ const char* string,
        /* [in] */ int fromIndex) const;

    inline int IndexOf(
        /* [in] */ const String& other) const;

    inline int IndexOf(
        /* [in] */ const String& other,
        /* [in] */ int fromIndex) const;

    inline int IndexOf(
        /* [in] */ int c) const;

    int IndexOf(
        /* [in] */ int c,
        /* [in] */ int fromIndex) const;

    inline int LastIndexOf(
        /* [in] */ const char* string) const;

    int LastIndexOf(
        /* [in] */ const char* string,
        /* [in] */ int fromIndex) const;

    inline int LastIndexOf(
        /* [in] */ const String& other) const;

    int LastIndexOf(
        /* [in] */ const String& other,
        /* [in] */ int fromIndex) const;

    inline int LastIndexOf(
        /* [in] */ int c) const;

    int LastIndexOf(
        /* [in] */ int c,
        /* [in] */ int fromIndex) const;

    inline bool StartsWith(
        /* [in] */ const char* prefix) const;

    inline bool StartsWith(
        /* [in] */ const String& prefix) const;

    bool EndsWith(
        /* [in] */ const char* suffix) const;

    bool EndsWith(
        /* [in] */ const String& suffix) const;

    String Trim() const;

    String TrimStart() const;

    String TrimEnd() const;

    String ToLowerCase() const;

    String ToUpperCase() const;

    String Substring(
        /* [in] */ int begin) const;

    String Substring(
        /* [in] */ int begin,
        /* [in] */ int end) const;

    String Replace(
        /* [in] */ const char* target,
        /* [in] */ const char* replacement) const;

    inline String Replace(
        /* [in] */ const String& target,
        /* [in] */ const String& replacement) const;

    String Replace(
        /* [in] */ int oldChar,
        /* [in] */ int newChar) const;

private:
    String(
        /* [in] */ int size);

    static int LastIndexOfInternal(
        /* [in] */ const char* source,
        /* [in] */ int sourceLength,
        /* [in] */ const char* target,
        /* [in] */ int targetLength,
        /* [in] */ int fromIndex);

public:
    static constexpr int MAX_SIZE = 67108863; // 2^26 - 1;

private:
    char* mString = nullptr;
    mutable int mHashCode = -1;
};

String::String()
{}

bool String::IsNull() const
{
    return mString == nullptr;
}

bool String::IsEmpty() const
{
    return IsNull() || mString[0] == '\0';
}

const char* String::string() const
{
    return mString;
}

String::operator const char*() const
{
    return mString;
}

int String::Compare(
    /* [in] */ const String& other) const
{
    return Compare(other.mString);
}

bool String::Contains(
    /* [in] */ const char* string) const
{
    return IndexOf(string) != -1;
}

bool String::Contains(
    /* [in] */ const String& other) const
{
    return IndexOf(other) != -1;
}

int String::IndexOf(
    /* [in] */ const char* string) const
{
    return IndexOf(string, 0);
}

int String::IndexOf(
    /* [in] */ const String& other) const
{
    return IndexOf(other.mString, 0);
}

int String::IndexOf(
    /* [in] */ const String& other,
    /* [in] */ int fromIndex) const
{
    return IndexOf(other.mString, fromIndex);
}

int String::IndexOf(
    /* [in] */ int c) const
{
    return IndexOf(c, 0);
}

int String::LastIndexOf(
    /* [in] */ const char* string) const
{
    return LastIndexOf(string, GetLength() - 1);
}

int String::LastIndexOf(
    /* [in] */ const String& other) const
{
    return LastIndexOf(other, GetLength() - 1);
}

int String::LastIndexOf(
    /* [in] */ int c) const
{
    return LastIndexOf(c, GetLength() - 1);
}

bool String::StartsWith(
    /* [in] */ const char* prefix) const
{
    return IndexOf(prefix) == 0;
}

bool String::StartsWith(
    /* [in] */ const String& prefix) const
{
    return IndexOf(prefix) == 0;
}

String String::Replace(
    /* [in] */ const String& target,
    /* [in] */ const String& replacement) const
{
    return Replace(target.mString, replacement.mString);
}

inline String operator+(
    /* [in] */ const String& lvalue,
    /* [in] */ const String& rvalue)
{
    String ret = lvalue;
    return ret += rvalue;
}

struct StringHashFunc
{
    size_t operator()(
        /* [in] */ const String& string) const
    {
        return string.GetHashCode();
    }
};

struct StringCompareFunc
{
    bool operator()(
        /* [in] */ const String& lvalue,
        /* [in] */ const String& rvalue) const
    {
        return lvalue.Compare(rvalue) == -1;
    }
};

struct StringEqualsFunc
{
    bool operator()(
        /* [in] */ const String& lvalue,
        /* [in] */ const String& rvalue) const
    {
        return lvalue.Equals(rvalue);
    }
};

}

#endif // __CDLC_STRING_H__
