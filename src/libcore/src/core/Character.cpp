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

#include "core/Character.h"
#include <unicode/uchar.h>

namespace ccm {
namespace core {

Boolean Character::IsLowerCaseImpl(
    /* [in] */ Char c)
{
    return u_islower(c);
}

Boolean Character::IsUpperCaseImpl(
    /* [in] */ Char c)
{
    return u_isupper(c);
}

Boolean Character::IsTitleCaseImpl(
    /* [in] */ Char c)
{
    return u_istitle(c);
}

Boolean Character::IsDigitImpl(
    /* [in] */ Char c)
{
    return u_isdigit(c);
}

Boolean Character::IsDefinedImpl(
    /* [in] */ Char c)
{
    return u_isdefined(c);
}

Boolean Character::IsLetterImpl(
    /* [in] */ Char c)
{
    return u_isalpha(c);
}

Boolean Character::IsLetterOrDigitImpl(
    /* [in] */ Char c)
{
    return u_isalnum(c);
}

Boolean Character::IsAlphabeticImpl(
    /* [in] */ Char c)
{
    return u_hasBinaryProperty(c, UCHAR_ALPHABETIC);
}

Boolean Character::IsIdeographicImpl(
    /* [in] */ Char c)
{
    return u_hasBinaryProperty(c, UCHAR_IDEOGRAPHIC);
}

Boolean Character::IsUnicodeIdentifierStartImpl(
    /* [in] */ Char c)
{
    return u_isIDStart(c);
}

Boolean Character::IsUnicodeIdentifierPartImpl(
    /* [in] */ Char c)
{
    return u_isIDPart(c);
}

Boolean Character::IsIdentifierIgnorableImpl(
    /* [in] */ Char c)
{
    return u_isIDIgnorable(c);
}

Char Character::ToLowerCase(
    /* [in] */ Char c)
{
    if (c >= 'A' && c <= 'Z') {
        return c + ('a' - 'A');
    }

    // All ASCII codepoints except the ones above remain unchanged.
    if (c < 0x80) {
        return c;
    }

    return ToLowerCaseImpl(c);
}

Char Character::ToLowerCaseImpl(
    /* [in] */ Char c)
{
    return u_tolower(c);
}

Char Character::ToUpperCase(
    /* [in] */ Char c)
{
    if (c >= 'a' && c <= 'z') {
        return c - ('a' - 'A');
    }

    // All ASCII codepoints except the ones above remain unchanged.
    if (c < 0x80) {
        return c;
    }

    return ToUpperCaseImpl(c);
}

Char Character::ToUpperCaseImpl(
    /* [in] */ Char c)
{
    return u_toupper(c);
}

Char Character::ToTitleCaseImpl(
    /* [in] */ Char c)
{
    return u_totitle(c);
}

}
}
