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

#ifndef __CCM_CORE_STRINGUTILS_H__
#define __CCM_CORE_STRINGUTILS_H__

#include <ccmtypes.h>

namespace ccm {
namespace core {

class StringUtils
{
public:
    static ECode ParseByte(
        /* [in] */ const String& s,
        /* [out] */ Byte* value);

    static ECode ParseByte(
        /* [in] */ const String& s,
        /* [in] */ Integer radix,
        /* [out] */ Byte* value);

    static ECode ParseShort(
        /* [in] */ const String& s,
        /* [out] */ Short* value);

    static ECode ParseShort(
        /* [in] */ const String& s,
        /* [in] */ Integer radix,
        /* [out] */ Short* value);

    static ECode ParseInt(
        /* [in] */ const String& s,
        /* [out] */ Integer* value);

    static ECode ParseInt(
        /* [in] */ const String& s,
        /* [in] */ Integer radix,
        /* [out] */ Integer* value);

    static ECode ParseLong(
        /* [in] */ const String& s,
        /* [out] */ Long* value);

    static ECode ParseLong(
        /* [in] */ const String& s,
        /* [in] */ Integer radix,
        /* [out] */ Long* value);

    static ECode ParseFloat(
        /* [in] */ const String& s,
        /* [out] */ Float* value);

    static ECode ParseDouble(
        /* [in] */ const String& s,
        /* [out] */ Double* value);

    static String ToString(
        /* [in] */ Byte b);

    static String ToString(
        /* [in] */ Integer i);

    static String ToString(
        /* [in] */ Integer i,
        /* [in] */ Integer radix);

private:
    StringUtils();
    StringUtils(const StringUtils&);
    ~StringUtils();
};

inline ECode StringUtils::ParseByte(
    /* [in] */ const String& s,
    /* [out] */ Byte* value)
{
    return ParseByte(s, 10, value);
}

inline ECode StringUtils::ParseShort(
    /* [in] */ const String& s,
    /* [out] */ Short* value)
{
    return ParseShort(s, 10, value);
}

inline ECode StringUtils::ParseInt(
    /* [in] */ const String& s,
    /* [out] */ Integer* value)
{
    return ParseInt(s, 10, value);
}

inline ECode StringUtils::ParseLong(
    /* [in] */ const String& s,
    /* [out] */ Long* value)
{
    return ParseLong(s, 10, value);
}

}
}

#endif // __CCM_CORE_STRINGUTILS_H__
