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

#ifndef __CCM_CORE_CHARACTER_H__
#define __CCM_CORE_CHARACTER_H__

#include <ccmtypes.h>

namespace ccm {
namespace core {

class Character final
{
public:
    static Integer HashCode(
        /* [in] */ Char value);

    static String ToString(
        /* [in] */ Char c);

    static Boolean IsValidCodePoint(
        /* [in] */ Char c);

    static Boolean IsBmpCodePoint(
        /* [in] */ Char c);

    static Boolean IsSupplementaryCodePoint(
        /* [in] */ Char c);

    static Boolean IsLowerCase(
        /* [in] */ Char c);

    static Boolean IsUpperCase(
        /* [in] */ Char c);

    static Boolean IsTitleCase(
        /* [in] */ Char c);

    static Boolean IsDigit(
        /* [in] */ Char c);

    static Boolean IsDefined(
        /* [in] */ Char c);

    static Boolean IsLetter(
        /* [in] */ Char c);

    static Boolean IsLetterOrDigit(
        /* [in] */ Char c);

    static Boolean IsAlphabetic(
        /* [in] */ Char c);

    static Boolean IsIdeographic(
        /* [in] */ Char c);

    static Boolean IsUnicodeIdentifierStart(
        /* [in] */ Char c);

    static Boolean IsUnicodeIdentifierPart(
        /* [in] */ Char c);

    static Boolean IsIdentifierIgnorable(
        /* [in] */ Char c);

    COM_PUBLIC static Char ToLowerCase(
        /* [in] */ Char c);

    COM_PUBLIC static Char ToUpperCase(
        /* [in] */ Char c);

    static Char ToTitleCase(
        /* [in] */ Char c);

private:
    Character();
    Character(const Character&);
    Character& operator= (const Character&);

    COM_PUBLIC static Boolean IsLowerCaseImpl(
        /* [in] */ Char c);

    COM_PUBLIC static Boolean IsUpperCaseImpl(
        /* [in] */ Char c);

    COM_PUBLIC static Boolean IsTitleCaseImpl(
        /* [in] */ Char c);

    COM_PUBLIC static Boolean IsDigitImpl(
        /* [in] */ Char c);

    COM_PUBLIC static Boolean IsDefinedImpl(
        /* [in] */ Char c);

    COM_PUBLIC static Boolean IsLetterImpl(
        /* [in] */ Char c);

    COM_PUBLIC static Boolean IsLetterOrDigitImpl(
        /* [in] */ Char c);

    COM_PUBLIC static Boolean IsAlphabeticImpl(
        /* [in] */ Char c);

    COM_PUBLIC static Boolean IsIdeographicImpl(
        /* [in] */ Char c);

    COM_PUBLIC static Boolean IsUnicodeIdentifierStartImpl(
        /* [in] */ Char c);

    COM_PUBLIC static Boolean IsUnicodeIdentifierPartImpl(
        /* [in] */ Char c);

    COM_PUBLIC static Boolean IsIdentifierIgnorableImpl(
        /* [in] */ Char c);

    static Char ToLowerCaseImpl(
        /* [in] */ Char c);

    static Char ToUpperCaseImpl(
        /* [in] */ Char c);

    COM_PUBLIC static Char ToTitleCaseImpl(
        /* [in] */ Char c);

public:
    static constexpr Integer MIN_RADIX = 2;

    static constexpr Integer MAX_RADIX = 36;

    /**
     * General category "Cn" in the Unicode specification.
     */
    static constexpr Byte UNASSIGNED = 0;

    /**
     * General category "Lu" in the Unicode specification.
     */
    static constexpr Byte UPPERCASE_LETTER = 1;

    /**
     * General category "Ll" in the Unicode specification.
     */
    static constexpr Byte LOWERCASE_LETTER = 2;

    /**
     * General category "Lt" in the Unicode specification.
     */
    static constexpr Byte TITLECASE_LETTER = 3;

    /**
     * General category "Lm" in the Unicode specification.
     */
    static constexpr Byte MODIFIER_LETTER = 4;

    /**
     * General category "Lo" in the Unicode specification.
     */
    static constexpr Byte OTHER_LETTER = 5;

    /**
     * General category "Mn" in the Unicode specification.
     */
    static constexpr Byte NON_SPACING_MARK = 6;

    /**
     * General category "Me" in the Unicode specification.
     */
    static constexpr Byte ENCLOSING_MARK = 7;

    /**
     * General category "Mc" in the Unicode specification.
     */
    static constexpr Byte COMBINING_SPACING_MARK = 8;

    /**
     * General category "Nd" in the Unicode specification.
     */
    static constexpr Byte DECIMAL_DIGIT_NUMBER = 9;

    /**
     * General category "Nl" in the Unicode specification.
     */
    static constexpr Byte LETTER_NUMBER = 10;

    /**
     * General category "No" in the Unicode specification.
     */
    static constexpr Byte OTHER_NUMBER = 11;

    /**
     * General category "Zs" in the Unicode specification.
     */
    static constexpr Byte SPACE_SEPARATOR = 12;

    /**
     * General category "Zl" in the Unicode specification.
     */
    static constexpr Byte LINE_SEPARATOR = 13;

    /**
     * General category "Zp" in the Unicode specification.
     */
    static constexpr Byte PARAGRAPH_SEPARATOR = 14;

    /**
     * General category "Cc" in the Unicode specification.
     */
    static constexpr Byte CONTROL = 15;

    /**
     * General category "Cf" in the Unicode specification.
     */
    static constexpr Byte FORMAT = 16;

    /**
     * General category "Co" in the Unicode specification.
     */
    static constexpr Byte PRIVATE_USE = 18;

    /**
     * General category "Cs" in the Unicode specification.
     */
    static constexpr Byte SURROGATE = 19;

    /**
     * General category "Pd" in the Unicode specification.
     */
    static constexpr Byte DASH_PUNCTUATION = 20;

    /**
     * General category "Ps" in the Unicode specification.
     */
    static constexpr Byte START_PUNCTUATION = 21;

    /**
     * General category "Pe" in the Unicode specification.
     */
    static constexpr Byte END_PUNCTUATION = 22;

    /**
     * General category "Pc" in the Unicode specification.
     */
    static constexpr Byte CONNECTOR_PUNCTUATION = 23;

    /**
     * General category "Po" in the Unicode specification.
     */
    static constexpr Byte OTHER_PUNCTUATION = 24;

    /**
     * General category "Sm" in the Unicode specification.
     */
    static constexpr Byte MATH_SYMBOL = 25;

    /**
     * General category "Sc" in the Unicode specification.
     */
    static constexpr Byte CURRENCY_SYMBOL = 26;

    /**
     * General category "Sk" in the Unicode specification.
     */
    static constexpr Byte MODIFIER_SYMBOL = 27;

    /**
     * General category "So" in the Unicode specification.
     */
    static constexpr Byte OTHER_SYMBOL = 28;

    /**
     * General category "Pi" in the Unicode specification.
     */
    static constexpr Byte INITIAL_QUOTE_PUNCTUATION = 29;

    /**
     * General category "Pf" in the Unicode specification.
     */
    static constexpr Byte FINAL_QUOTE_PUNCTUATION = 30;

    /**
     * Undefined bidirectional character type. Undefined {@code char}
     * values have undefined directionality in the Unicode specification.
     */
    static constexpr Byte DIRECTIONALITY_UNDEFINED = -1;

    /**
     * Strong bidirectional character type "L" in the Unicode specification.
     */
    static constexpr Byte DIRECTIONALITY_LEFT_TO_RIGHT = 0;

    /**
     * Strong bidirectional character type "R" in the Unicode specification.
     */
    static constexpr Byte DIRECTIONALITY_RIGHT_TO_LEFT = 1;

    /**
    * Strong bidirectional character type "AL" in the Unicode specification.
     */
    static constexpr Byte DIRECTIONALITY_RIGHT_TO_LEFT_ARABIC = 2;

    /**
     * Weak bidirectional character type "EN" in the Unicode specification.
     */
    static constexpr Byte DIRECTIONALITY_EUROPEAN_NUMBER = 3;

    /**
     * Weak bidirectional character type "ES" in the Unicode specification.
     */
    static constexpr Byte DIRECTIONALITY_EUROPEAN_NUMBER_SEPARATOR = 4;

    /**
     * Weak bidirectional character type "ET" in the Unicode specification.
     */
    static constexpr Byte DIRECTIONALITY_EUROPEAN_NUMBER_TERMINATOR = 5;

    /**
     * Weak bidirectional character type "AN" in the Unicode specification.
     */
    static constexpr Byte DIRECTIONALITY_ARABIC_NUMBER = 6;

    /**
     * Weak bidirectional character type "CS" in the Unicode specification.
     */
    static constexpr Byte DIRECTIONALITY_COMMON_NUMBER_SEPARATOR = 7;

    /**
     * Weak bidirectional character type "NSM" in the Unicode specification.
     */
    static constexpr Byte DIRECTIONALITY_NONSPACING_MARK = 8;

    /**
     * Weak bidirectional character type "BN" in the Unicode specification.
     */
    static constexpr Byte DIRECTIONALITY_BOUNDARY_NEUTRAL = 9;

    /**
     * Neutral bidirectional character type "B" in the Unicode specification.
     */
    static constexpr Byte DIRECTIONALITY_PARAGRAPH_SEPARATOR = 10;

    /**
     * Neutral bidirectional character type "S" in the Unicode specification.
     */
    static constexpr Byte DIRECTIONALITY_SEGMENT_SEPARATOR = 11;

    /**
     * Neutral bidirectional character type "WS" in the Unicode specification.
     */
    static constexpr Byte DIRECTIONALITY_WHITESPACE = 12;

    /**
     * Neutral bidirectional character type "ON" in the Unicode specification.
     */
    static constexpr Byte DIRECTIONALITY_OTHER_NEUTRALS = 13;

    /**
     * Strong bidirectional character type "LRE" in the Unicode specification.
     */
    static constexpr Byte DIRECTIONALITY_LEFT_TO_RIGHT_EMBEDDING = 14;

    /**
     * Strong bidirectional character type "LRO" in the Unicode specification.
     */
    static constexpr Byte DIRECTIONALITY_LEFT_TO_RIGHT_OVERRIDE = 15;

    /**
     * Strong bidirectional character type "RLE" in the Unicode specification.
     */
    static constexpr Byte DIRECTIONALITY_RIGHT_TO_LEFT_EMBEDDING = 16;

    /**
     * Strong bidirectional character type "RLO" in the Unicode specification.
     */
    static constexpr Byte DIRECTIONALITY_RIGHT_TO_LEFT_OVERRIDE = 17;

    /**
     * Weak bidirectional character type "PDF" in the Unicode specification.
     */
    static constexpr Byte DIRECTIONALITY_POP_DIRECTIONAL_FORMAT = 18;

    /**
     * The minimum value of a
     * <a href="http://www.unicode.org/glossary/#high_surrogate_code_unit">
     * Unicode high-surrogate code unit</a>
     * in the UTF-16 encoding, constant {@code '\u005CuD800'}.
     * A high-surrogate is also known as a <i>leading-surrogate</i>.
     */
    static constexpr Char MIN_HIGH_SURROGATE = 0xD800;

    /**
     * The maximum value of a
     * <a href="http://www.unicode.org/glossary/#high_surrogate_code_unit">
     * Unicode high-surrogate code unit</a>
     * in the UTF-16 encoding, constant {@code '\u005CuDBFF'}.
     * A high-surrogate is also known as a <i>leading-surrogate</i>.
     */
    static constexpr Char MAX_HIGH_SURROGATE = 0xDBFF;

    /**
     * The minimum value of a
     * <a href="http://www.unicode.org/glossary/#low_surrogate_code_unit">
     * Unicode low-surrogate code unit</a>
     * in the UTF-16 encoding, constant {@code '\u005CuDC00'}.
     * A low-surrogate is also known as a <i>trailing-surrogate</i>.
     */
    static constexpr Char MIN_LOW_SURROGATE  = 0xDC00;

    /**
     * The maximum value of a
     * <a href="http://www.unicode.org/glossary/#low_surrogate_code_unit">
     * Unicode low-surrogate code unit</a>
     * in the UTF-16 encoding, constant {@code '\u005CuDFFF'}.
     * A low-surrogate is also known as a <i>trailing-surrogate</i>.
     */
    static constexpr Char MAX_LOW_SURROGATE  = 0xDFFF;

    /**
     * The minimum value of a Unicode surrogate code unit in the
     * UTF-16 encoding, constant {@code '\u005CuD800'}.
     */
    static constexpr Char MIN_SURROGATE = MIN_HIGH_SURROGATE;

    /**
     * The maximum value of a Unicode surrogate code unit in the
     * UTF-16 encoding, constant {@code '\u005CuDFFF'}.
     */
    static constexpr Char MAX_SURROGATE = MAX_LOW_SURROGATE;

    /**
     * The minimum value of a
     * <a href="http://www.unicode.org/glossary/#supplementary_code_point">
     * Unicode supplementary code point</a>, constant {@code U+10000}.
     */
    static constexpr Integer MIN_SUPPLEMENTARY_CODE_POINT = 0x010000;

    /**
     * The minimum value of a
     * <a href="http://www.unicode.org/glossary/#code_point">
     * Unicode code point</a>, constant {@code U+0000}.
     */
    static constexpr Integer MIN_CODE_POINT = 0x000000;

    /**
     * The maximum value of a
     * <a href="http://www.unicode.org/glossary/#code_point">
     * Unicode code point</a>, constant {@code U+10FFFF}.
     */
    static constexpr Integer MAX_CODE_POINT = 0X10FFFF;


private:
    /**
     * Error flag. Use int (code point) to avoid confusion with U+FFFF.
     */
    static constexpr Integer ERROR = 0xFFFFFFFF;

    static constexpr Byte DIRECTIONALITY[] = {
        DIRECTIONALITY_LEFT_TO_RIGHT, DIRECTIONALITY_RIGHT_TO_LEFT,
        DIRECTIONALITY_EUROPEAN_NUMBER,
        DIRECTIONALITY_EUROPEAN_NUMBER_SEPARATOR,
        DIRECTIONALITY_EUROPEAN_NUMBER_TERMINATOR,
        DIRECTIONALITY_ARABIC_NUMBER,
        DIRECTIONALITY_COMMON_NUMBER_SEPARATOR,
        DIRECTIONALITY_PARAGRAPH_SEPARATOR,
        DIRECTIONALITY_SEGMENT_SEPARATOR, DIRECTIONALITY_WHITESPACE,
        DIRECTIONALITY_OTHER_NEUTRALS,
        DIRECTIONALITY_LEFT_TO_RIGHT_EMBEDDING,
        DIRECTIONALITY_LEFT_TO_RIGHT_OVERRIDE,
        DIRECTIONALITY_RIGHT_TO_LEFT_ARABIC,
        DIRECTIONALITY_RIGHT_TO_LEFT_EMBEDDING,
        DIRECTIONALITY_RIGHT_TO_LEFT_OVERRIDE,
        DIRECTIONALITY_POP_DIRECTIONAL_FORMAT,
        DIRECTIONALITY_NONSPACING_MARK, DIRECTIONALITY_BOUNDARY_NEUTRAL};
};

inline Integer Character::HashCode(
    /* [in] */ Char value)
{
    return (Integer)value;
}

inline String Character::ToString(
    /* [in] */ Char c)
{
    return String::ValueOfChar(c);
}

inline Boolean Character::IsValidCodePoint(
    /* [in] */ Char c)
{
    Integer plane = c >> 16;
    return plane < ((unsigned int)(MAX_CODE_POINT + 1) >> 16);
}

inline Boolean Character::IsBmpCodePoint(
    /* [in] */ Char c)
{
    return c >> 16 == 0;
}

inline Boolean Character::IsSupplementaryCodePoint(
    /* [in] */ Char c)
{
    return c >= MIN_SUPPLEMENTARY_CODE_POINT
            && c <  MAX_CODE_POINT + 1;
}

inline Boolean Character::IsLowerCase(
    /* [in] */ Char c)
{
    return IsLowerCaseImpl(c);
}

inline Boolean Character::IsUpperCase(
    /* [in] */ Char c)
{
    return IsUpperCaseImpl(c);
}

inline Boolean Character::IsTitleCase(
    /* [in] */ Char c)
{
    return IsTitleCaseImpl(c);
}

inline Boolean Character::IsDigit(
    /* [in] */ Char c)
{
    return IsDigitImpl(c);
}

inline Boolean Character::IsDefined(
    /* [in] */ Char c)
{
    return IsDefinedImpl(c);
}

inline Boolean Character::IsLetter(
    /* [in] */ Char c)
{
    return IsLetterImpl(c);
}

inline Boolean Character::IsLetterOrDigit(
    /* [in] */ Char c)
{
    return IsLetterOrDigitImpl(c);
}

inline Boolean Character::IsAlphabetic(
    /* [in] */ Char c)
{
    return IsAlphabeticImpl(c);
}

inline Boolean Character::IsIdeographic(
    /* [in] */ Char c)
{
    return IsIdeographicImpl(c);
}

inline Boolean Character::IsUnicodeIdentifierStart(
    /* [in] */ Char c)
{
    return IsUnicodeIdentifierStartImpl(c);
}

inline Boolean Character::IsUnicodeIdentifierPart(
    /* [in] */ Char c)
{
    return IsUnicodeIdentifierPartImpl(c);
}

inline Boolean Character::IsIdentifierIgnorable(
    /* [in] */ Char c)
{
    return IsIdentifierIgnorableImpl(c);
}

inline Char Character::ToTitleCase(
    /* [in] */ Char c)
{
    return ToTitleCaseImpl(c);
}

}
}

#endif // __CCM_CORE_CHARACTER_H__
