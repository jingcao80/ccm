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

#include "ccm/core/Character.h"
#include <unicode/uchar.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>

namespace ccm {
namespace core {

constexpr Byte Character::DIRECTIONALITY[];

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

Integer Character::Digit(
    /* [in] */ Char c,
    /* [in] */ Integer radix)
{
    if (radix < MIN_RADIX || radix > MAX_RADIX) {
        return -1;
    }
    if (c < 128) {
        // Optimized for ASCII
        Integer result = -1;
        if ('0' <= c && c <= '9') {
            result = c - '0';
        }
        else if ('a' <= c && c <= 'z') {
            result = 10 + (c - 'a');
        }
        else if ('A' <= c && c <= 'Z') {
            result = 10 + (c - 'A');
        }
        return result < radix ? result : -1;
    }
    return DigitImpl(c, radix);
}

Integer Character::DigitImpl(
    /* [in] */ Char c,
    /* [in] */ Integer radix)
{
    return u_digit(c, radix);
}

Integer Character::GetNumericValue(
    /* [in] */ Char c)
{
    // This is both an optimization and papers over differences between Java and ICU.
    if (c < 128) {
        if (c >= '0' && c <= '9') {
            return c - '0';
        }
        if (c >= 'a' && c <= 'z') {
            return c - ('a' - 10);
        }
        if (c >= 'A' && c <= 'Z') {
            return c - ('A' - 10);
        }
        return -1;
    }
    // Full-width uppercase A-Z.
    if (c >= 0xff21 && c <= 0xff3a) {
        return c - 0xff17;
    }
    // Full-width lowercase a-z.
    if (c >= 0xff41 && c <= 0xff5a) {
        return c - 0xff37;
    }
    return GetNumericValueImpl(c);
}

Integer Character::GetNumericValueImpl(
    /* [in] */ Char c)
{
    double result = u_getNumericValue(c);
    if (result == U_NO_NUMERIC_VALUE) {
        return -1;
    }
    else if (result < 0 || floor(result + 0.5) != result) {
        return -2;
    }
    return static_cast<Integer>(result);
}

Boolean Character::IsSpaceChar(
    /* [in] */ Char c)
{
    // SPACE or NO-BREAK SPACE?
    if (c == 0x20 || c == 0xa0) {
        return true;
    }
    if (c < 0x1000) {
        return false;
    }
    // OGHAM SPACE MARK or MONGOLIAN VOWEL SEPARATOR?
    if (c == 0x1680 || c == 0x180e) {
        return true;
    }
    if (c < 0x2000) {
        return false;
    }
    if (c <= 0xffff) {
        // Other whitespace from General Punctuation...
        return c <= 0x200a || c == 0x2028 || c == 0x2029 || c == 0x202f ||
                c == 0x205f ||  c == 0x3000; // ...or CJK Symbols and Punctuation?
    }
    // Let icu4c worry about non-BMP code points.
    return IsSpaceCharImpl(c);
}

Boolean Character::IsSpaceCharImpl(
    /* [in] */ Char c)
{
    return u_isJavaSpaceChar(c);
}

Boolean Character::IsWhitespace(
    /* [in] */ Char c)
{
    // Any ASCII whitespace character?
    if ((c >= 0x1c && c <= 0x20) || (c >= 0x09 && c <= 0x0d)) {
        return true;
    }
    if (c < 0x1000) {
        return false;
    }
    // OGHAM SPACE MARK or MONGOLIAN VOWEL SEPARATOR?
    if (c == 0x1680 || c == 0x180e) {
        return true;
    }
    if (c < 0x2000) {
        return false;
    }
    // Exclude General Punctuation's non-breaking spaces (which includes FIGURE SPACE).
    if (c == 0x2007 || c == 0x202f) {
        return false;
    }
    if (c <= 0xffff) {
        // Other whitespace from General Punctuation...
        return c <= 0x200a || c == 0x2028 || c == 0x2029 || c == 0x205f ||
            c == 0x3000; // ...or CJK Symbols and Punctuation?
    }
    // Let icu4c worry about non-BMP code points.
    return IsWhitespaceImpl(c);
}

Boolean Character::IsWhitespaceImpl(
    /* [in] */ Char c)
{
    return u_isWhitespace(c);
}

Integer Character::GetType(
    /* [in] */ Char c)
{
    Integer type = GetTypeImpl(c);
    // The type values returned by ICU are not RI-compatible. The RI skips the value 17.
    if (type <= FORMAT) {
        return type;
    }
    return (type + 1);
}

Integer Character::GetTypeImpl(
    /* [in] */ Char c)
{
    return u_charType(c);
}

Char Character::ForDigit(
    /* [in] */ Integer digit,
    /* [in] */ Integer radix)
{
    if ((digit >= radix) || (digit < 0)) {
        return '\0';
    }
    if ((radix < MIN_RADIX) || (radix > MAX_RADIX)) {
        return '\0';
    }
    if (digit < 10) {
        return (Char)('0' + digit);
    }
    return (Char)('a' - 10 + digit);
}

Byte Character::GetDirectionality(
    /* [in] */ Char c)
{
    if (GetType(c) == UNASSIGNED) {
        return DIRECTIONALITY_UNDEFINED;
    }

    Byte directionality = GetDirectionalityImpl(c);
    if (directionality >= 0 && directionality < ArrayLength(DIRECTIONALITY)) {
        return DIRECTIONALITY[directionality];
    }
    return DIRECTIONALITY_UNDEFINED;
}

Byte Character::GetDirectionalityImpl(
    /* [in] */ Char c)
{
    return u_charDirection(c);
}

Boolean Character::IsMirroredImpl(
    /* [in] */ Char c)
{
    return u_isMirrored(c);
}

String Character::GetName(
    /* [in] */ Char c)
{
    if (!IsValidCodePoint(c)) {
        return String();
    }
    String name = GetNameImpl(c);
    if (!name.IsNull()) {
        return name;
    }
    if (GetType(c) == UNASSIGNED) {
        return String();
    }
    AutoPtr<UnicodeBlock> block = UnicodeBlock::Of(c);
    if (block != nullptr) {
        // return block.toString().replace('_', ' ') + " "
        //        + Integer.toHexString(codePoint).toUpperCase(Locale.ENGLISH);
    }
    // should never come here
    // return Integer.toHexString(codePoint).toUpperCase(Locale.ENGLISH);
}

String Character::GetNameImpl(
    /* [in] */ Char c)
{
    // U_UNICODE_CHAR_NAME gives us the modern names for characters. For control characters,
    // we need U_EXTENDED_CHAR_NAME to get "NULL" rather than "BASIC LATIN 0" and so on.
    // We could just use U_EXTENDED_CHAR_NAME except that it returns strings for characters
    // that aren't unassigned but that don't have names, and those strings aren't in the form
    // Java specifies.
    Boolean isControl = (c <= 0x1f || (c >= 0x7f && c <= 0x9f));
    UCharNameChoice nameType = isControl ? U_EXTENDED_CHAR_NAME : U_UNICODE_CHAR_NAME;
    UErrorCode status = U_ZERO_ERROR;
    char buf[BUFSIZ]; // TODO: is there a more sensible upper bound?
    int32_t byteCount = u_charName(c, nameType, &buf[0], sizeof(buf), &status);
    return (U_FAILURE(status) || byteCount == 0) ? String() : String(buf);
}

//--------------------------------------------------------------------------------

Character::UnicodeBlock::UnicodeBlock(
    /* [in] */ const String& idName)
    : Subset(idName)
{}

Character::UnicodeBlock::UnicodeBlock(
    /* [in] */ const String& idName,
    /* [in] */ Boolean isMap)
    : Subset(idName)
{}

Character::UnicodeBlock::UnicodeBlock(
    /* [in] */ const String& idName,
    /* [in] */ const String& alias)
    : Subset(idName)
{}

Character::UnicodeBlock::UnicodeBlock(
    /* [in] */ const String& idName,
    /* [in] */ const String& alias1,
    /* [in] */ const String& alias2)
    : Subset(idName)
{}

Character::UnicodeBlock::UnicodeBlock(
    /* [in] */ const String& idName,
    /* [in] */ const String& alias1,
    /* [in] */ const String& alias2,
    /* [in] */ const String& alias3,
    /* [in] */ const String& alias4)
    : Subset(idName)
{}

AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::BASIC_LATIN =
        new UnicodeBlock(String("BASIC_LATIN"),
                         String("BASIC LATIN"),
                         String("BASICLATIN"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::LATIN_1_SUPPLEMENT =
        new UnicodeBlock(String("LATIN_1_SUPPLEMENT"),
                         String("LATIN-1 SUPPLEMENT"),
                         String("LATIN-1SUPPLEMENT"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::LATIN_EXTENDED_A =
        new UnicodeBlock(String("LATIN_EXTENDED_A"),
                         String("LATIN EXTENDED-A"),
                         String("LATINEXTENDED-A"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::LATIN_EXTENDED_B =
        new UnicodeBlock(String("LATIN_EXTENDED_B"),
                         String("LATIN EXTENDED-B"),
                         String("LATINEXTENDED-B"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::IPA_EXTENSIONS =
        new UnicodeBlock(String("IPA_EXTENSIONS"),
                         String("IPA EXTENSIONS"),
                         String("IPAEXTENSIONS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::SPACING_MODIFIER_LETTERS =
        new UnicodeBlock(String("SPACING_MODIFIER_LETTERS"),
                         String("SPACING MODIFIER LETTERS"),
                         String("SPACINGMODIFIERLETTERS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::COMBINING_DIACRITICAL_MARKS =
        new UnicodeBlock(String("COMBINING_DIACRITICAL_MARKS"),
                         String("COMBINING DIACRITICAL MARKS"),
                         String("COMBININGDIACRITICALMARKS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::GREEK =
        new UnicodeBlock(String("GREEK"),
                         String("GREEK AND COPTIC"),
                         String("GREEKANDCOPTIC"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::CYRILLIC =
        new UnicodeBlock(String("CYRILLIC"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::ARMENIAN =
        new UnicodeBlock(String("ARMENIAN"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::HEBREW =
        new UnicodeBlock(String("HEBREW"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::ARABIC =
        new UnicodeBlock(String("ARABIC"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::DEVANAGARI =
        new UnicodeBlock(String("DEVANAGARI"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::BENGALI =
        new UnicodeBlock(String("BENGALI"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::GURMUKHI =
        new UnicodeBlock(String("GURMUKHI"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::GUJARATI =
        new UnicodeBlock(String("GUJARATI"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::ORIYA =
        new UnicodeBlock(String("ORIYA"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::TAMIL =
        new UnicodeBlock(String("TAMIL"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::TELUGU =
        new UnicodeBlock(String("TELUGU"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::KANNADA =
        new UnicodeBlock(String("KANNADA"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::MALAYALAM =
        new UnicodeBlock(String("MALAYALAM"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::THAI =
        new UnicodeBlock(String("THAI"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::LAO =
        new UnicodeBlock(String("LAO"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::TIBETAN =
        new UnicodeBlock(String("TIBETAN"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::GEORGIAN =
        new UnicodeBlock(String("GEORGIAN"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::HANGUL_JAMO =
        new UnicodeBlock(String("HANGUL_JAMO"),
                         String("HANGUL JAMO"),
                         String("HANGULJAMO"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::LATIN_EXTENDED_ADDITIONAL =
        new UnicodeBlock(String("LATIN_EXTENDED_ADDITIONAL"),
                         String("LATIN EXTENDED ADDITIONAL"),
                         String("LATINEXTENDEDADDITIONAL"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::GREEK_EXTENDED =
        new UnicodeBlock(String("GREEK_EXTENDED"),
                         String("GREEK EXTENDED"),
                         String("GREEKEXTENDED"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::GENERAL_PUNCTUATION =
        new UnicodeBlock(String("GENERAL_PUNCTUATION"),
                         String("GENERAL PUNCTUATION"),
                         String("GENERALPUNCTUATION"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::SUPERSCRIPTS_AND_SUBSCRIPTS =
        new UnicodeBlock(String("SUPERSCRIPTS_AND_SUBSCRIPTS"),
                         String("SUPERSCRIPTS AND SUBSCRIPTS"),
                         String("SUPERSCRIPTSANDSUBSCRIPTS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::CURRENCY_SYMBOLS =
        new UnicodeBlock(String("CURRENCY_SYMBOLS"),
                         String("CURRENCY SYMBOLS"),
                         String("CURRENCYSYMBOLS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::COMBINING_MARKS_FOR_SYMBOLS =
        new UnicodeBlock(String("COMBINING_MARKS_FOR_SYMBOLS"),
                         String("COMBINING DIACRITICAL MARKS FOR SYMBOLS"),
                         String("COMBININGDIACRITICALMARKSFORSYMBOLS"),
                         String("COMBINING MARKS FOR SYMBOLS"),
                         String("COMBININGMARKSFORSYMBOLS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::LETTERLIKE_SYMBOLS =
        new UnicodeBlock(String("LETTERLIKE_SYMBOLS"),
                         String("LETTERLIKE SYMBOLS"),
                         String("LETTERLIKESYMBOLS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::NUMBER_FORMS =
        new UnicodeBlock(String("NUMBER_FORMS"),
                         String("NUMBER FORMS"),
                         String("NUMBERFORMS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::ARROWS =
        new UnicodeBlock(String("ARROWS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::MATHEMATICAL_OPERATORS =
        new UnicodeBlock(String("MATHEMATICAL_OPERATORS"),
                         String("MATHEMATICAL OPERATORS"),
                         String("MATHEMATICALOPERATORS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::MISCELLANEOUS_TECHNICAL =
        new UnicodeBlock(String("MISCELLANEOUS_TECHNICAL"),
                         String("MISCELLANEOUS TECHNICAL"),
                         String("MISCELLANEOUSTECHNICAL"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::CONTROL_PICTURES =
        new UnicodeBlock(String("CONTROL_PICTURES"),
                         String("CONTROL PICTURES"),
                         String("CONTROLPICTURES"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::OPTICAL_CHARACTER_RECOGNITION =
        new UnicodeBlock(String("OPTICAL_CHARACTER_RECOGNITION"),
                         String("OPTICAL CHARACTER RECOGNITION"),
                         String("OPTICALCHARACTERRECOGNITION"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::ENCLOSED_ALPHANUMERICS =
        new UnicodeBlock(String("ENCLOSED_ALPHANUMERICS"),
                         String("ENCLOSED ALPHANUMERICS"),
                         String("ENCLOSEDALPHANUMERICS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::BOX_DRAWING =
        new UnicodeBlock(String("BOX_DRAWING"),
                         String("BOX DRAWING"),
                         String("BOXDRAWING"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::BLOCK_ELEMENTS =
        new UnicodeBlock(String("BLOCK_ELEMENTS"),
                         String("BLOCK ELEMENTS"),
                         String("BLOCKELEMENTS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::GEOMETRIC_SHAPES =
        new UnicodeBlock(String("GEOMETRIC_SHAPES"),
                         String("GEOMETRIC SHAPES"),
                         String("GEOMETRICSHAPES"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::MISCELLANEOUS_SYMBOLS =
        new UnicodeBlock(String("MISCELLANEOUS_SYMBOLS"),
                         String("MISCELLANEOUS SYMBOLS"),
                         String("MISCELLANEOUSSYMBOLS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::DINGBATS =
        new UnicodeBlock(String("DINGBATS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::CJK_SYMBOLS_AND_PUNCTUATION =
        new UnicodeBlock(String("CJK_SYMBOLS_AND_PUNCTUATION"),
                         String("CJK SYMBOLS AND PUNCTUATION"),
                         String("CJKSYMBOLSANDPUNCTUATION"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::HIRAGANA =
        new UnicodeBlock(String("HIRAGANA"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::KATAKANA =
        new UnicodeBlock(String("KATAKANA"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::BOPOMOFO =
        new UnicodeBlock(String("BOPOMOFO"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::HANGUL_COMPATIBILITY_JAMO =
        new UnicodeBlock(String("HANGUL_COMPATIBILITY_JAMO"),
                         String("HANGUL COMPATIBILITY JAMO"),
                         String("HANGULCOMPATIBILITYJAMO"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::KANBUN =
        new UnicodeBlock(String("KANBUN"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::ENCLOSED_CJK_LETTERS_AND_MONTHS =
        new UnicodeBlock(String("ENCLOSED_CJK_LETTERS_AND_MONTHS"),
                         String("ENCLOSED CJK LETTERS AND MONTHS"),
                         String("ENCLOSEDCJKLETTERSANDMONTHS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::CJK_COMPATIBILITY =
        new UnicodeBlock(String("CJK_COMPATIBILITY"),
                         String("CJK COMPATIBILITY"),
                         String("CJKCOMPATIBILITY"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::CJK_UNIFIED_IDEOGRAPHS =
        new UnicodeBlock(String("CJK_UNIFIED_IDEOGRAPHS"),
                         String("CJK UNIFIED IDEOGRAPHS"),
                         String("CJKUNIFIEDIDEOGRAPHS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::HANGUL_SYLLABLES =
        new UnicodeBlock(String("HANGUL_SYLLABLES"),
                         String("HANGUL SYLLABLES"),
                         String("HANGULSYLLABLES"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::PRIVATE_USE_AREA =
        new UnicodeBlock(String("PRIVATE_USE_AREA"),
                         String("PRIVATE USE AREA"),
                         String("PRIVATEUSEAREA"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::CJK_COMPATIBILITY_IDEOGRAPHS =
        new UnicodeBlock(String("CJK_COMPATIBILITY_IDEOGRAPHS"),
                         String("CJK COMPATIBILITY IDEOGRAPHS"),
                         String("CJKCOMPATIBILITYIDEOGRAPHS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::ALPHABETIC_PRESENTATION_FORMS =
        new UnicodeBlock(String("ALPHABETIC_PRESENTATION_FORMS"),
                         String("ALPHABETIC PRESENTATION FORMS"),
                         String("ALPHABETICPRESENTATIONFORMS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::ARABIC_PRESENTATION_FORMS_A =
        new UnicodeBlock(String("ARABIC_PRESENTATION_FORMS_A"),
                         String("ARABIC PRESENTATION FORMS-A"),
                         String("ARABICPRESENTATIONFORMS-A"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::COMBINING_HALF_MARKS =
        new UnicodeBlock(String("COMBINING_HALF_MARKS"),
                         String("COMBINING HALF MARKS"),
                         String("COMBININGHALFMARKS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::CJK_COMPATIBILITY_FORMS =
        new UnicodeBlock(String("CJK_COMPATIBILITY_FORMS"),
                         String("CJK COMPATIBILITY FORMS"),
                         String("CJKCOMPATIBILITYFORMS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::SMALL_FORM_VARIANTS =
        new UnicodeBlock(String("SMALL_FORM_VARIANTS"),
                         String("SMALL FORM VARIANTS"),
                         String("SMALLFORMVARIANTS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::ARABIC_PRESENTATION_FORMS_B =
        new UnicodeBlock(String("ARABIC_PRESENTATION_FORMS_B"),
                         String("ARABIC PRESENTATION FORMS-B"),
                         String("ARABICPRESENTATIONFORMS-B"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::HALFWIDTH_AND_FULLWIDTH_FORMS =
        new UnicodeBlock(String("HALFWIDTH_AND_FULLWIDTH_FORMS"),
                         String("HALFWIDTH AND FULLWIDTH FORMS"),
                         String("HALFWIDTHANDFULLWIDTHFORMS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::SPECIALS =
        new UnicodeBlock(String("SPECIALS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::SURROGATES_AREA =
        new UnicodeBlock(String("SURROGATES_AREA"), false);
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::SYRIAC =
        new UnicodeBlock(String("SYRIAC"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::THAANA =
        new UnicodeBlock(String("THAANA"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::SINHALA =
        new UnicodeBlock(String("SINHALA"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::MYANMAR =
        new UnicodeBlock(String("MYANMAR"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::ETHIOPIC =
        new UnicodeBlock(String("ETHIOPIC"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::CHEROKEE =
        new UnicodeBlock(String("CHEROKEE"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS =
        new UnicodeBlock(String("UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS"),
                         String("UNIFIED CANADIAN ABORIGINAL SYLLABICS"),
                         String("UNIFIEDCANADIANABORIGINALSYLLABICS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::OGHAM =
        new UnicodeBlock(String("OGHAM"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::RUNIC =
        new UnicodeBlock(String("RUNIC"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::KHMER =
        new UnicodeBlock(String("KHMER"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::MONGOLIAN =
        new UnicodeBlock(String("MONGOLIAN"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::BRAILLE_PATTERNS =
        new UnicodeBlock(String("BRAILLE_PATTERNS"),
                         String("BRAILLE PATTERNS"),
                         String("BRAILLEPATTERNS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::CJK_RADICALS_SUPPLEMENT =
        new UnicodeBlock(String("CJK_RADICALS_SUPPLEMENT"),
                         String("CJK RADICALS SUPPLEMENT"),
                         String("CJKRADICALSSUPPLEMENT"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::KANGXI_RADICALS =
        new UnicodeBlock(String("KANGXI_RADICALS"),
                         String("KANGXI RADICALS"),
                         String("KANGXIRADICALS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::IDEOGRAPHIC_DESCRIPTION_CHARACTERS =
        new UnicodeBlock(String("IDEOGRAPHIC_DESCRIPTION_CHARACTERS"),
                         String("IDEOGRAPHIC DESCRIPTION CHARACTERS"),
                         String("IDEOGRAPHICDESCRIPTIONCHARACTERS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::BOPOMOFO_EXTENDED =
        new UnicodeBlock(String("BOPOMOFO_EXTENDED"),
                         String("BOPOMOFO EXTENDED"),
                         String("BOPOMOFOEXTENDED"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::CJK_UNIFIED_IDEOGRAPHS_EXTENSION_A =
        new UnicodeBlock(String("CJK_UNIFIED_IDEOGRAPHS_EXTENSION_A"),
                         String("CJK UNIFIED IDEOGRAPHS EXTENSION A"),
                         String("CJKUNIFIEDIDEOGRAPHSEXTENSIONA"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::YI_SYLLABLES =
        new UnicodeBlock(String("YI_SYLLABLES"),
                         String("YI SYLLABLES"),
                         String("YISYLLABLES"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::YI_RADICALS =
        new UnicodeBlock(String("YI_RADICALS"),
                         String("YI RADICALS"),
                         String("YIRADICALS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::CYRILLIC_SUPPLEMENTARY =
        new UnicodeBlock(String("CYRILLIC_SUPPLEMENTARY"),
                         String("CYRILLIC SUPPLEMENTARY"),
                         String("CYRILLICSUPPLEMENTARY"),
                         String("CYRILLIC SUPPLEMENT"),
                         String("CYRILLICSUPPLEMENT"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::TAGALOG =
        new UnicodeBlock(String("TAGALOG"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::HANUNOO =
        new UnicodeBlock(String("HANUNOO"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::BUHID =
        new UnicodeBlock(String("BUHID"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::TAGBANWA =
        new UnicodeBlock(String("TAGBANWA"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::LIMBU =
        new UnicodeBlock(String("LIMBU"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::TAI_LE =
        new UnicodeBlock(String("TAI_LE"),
                         String("TAI LE"),
                         String("TAILE"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::KHMER_SYMBOLS =
        new UnicodeBlock(String("KHMER_SYMBOLS"),
                         String("KHMER SYMBOLS"),
                         String("KHMERSYMBOLS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::PHONETIC_EXTENSIONS =
        new UnicodeBlock(String("PHONETIC_EXTENSIONS"),
                         String("PHONETIC EXTENSIONS"),
                         String("PHONETICEXTENSIONS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::MISCELLANEOUS_MATHEMATICAL_SYMBOLS_A =
        new UnicodeBlock(String("MISCELLANEOUS_MATHEMATICAL_SYMBOLS_A"),
                         String("MISCELLANEOUS MATHEMATICAL SYMBOLS-A"),
                         String("MISCELLANEOUSMATHEMATICALSYMBOLS-A"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::SUPPLEMENTAL_ARROWS_A =
        new UnicodeBlock(String("SUPPLEMENTAL_ARROWS_A"),
                         String("SUPPLEMENTAL ARROWS-A"),
                         String("SUPPLEMENTALARROWS-A"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::SUPPLEMENTAL_ARROWS_B =
        new UnicodeBlock(String("SUPPLEMENTAL_ARROWS_B"),
                         String("SUPPLEMENTAL ARROWS-B"),
                         String("SUPPLEMENTALARROWS-B"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::MISCELLANEOUS_MATHEMATICAL_SYMBOLS_B =
        new UnicodeBlock(String("MISCELLANEOUS_MATHEMATICAL_SYMBOLS_B"),
                         String("MISCELLANEOUS MATHEMATICAL SYMBOLS-B"),
                         String("MISCELLANEOUSMATHEMATICALSYMBOLS-B"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::SUPPLEMENTAL_MATHEMATICAL_OPERATORS =
        new UnicodeBlock(String("SUPPLEMENTAL_MATHEMATICAL_OPERATORS"),
                         String("SUPPLEMENTAL MATHEMATICAL OPERATORS"),
                         String("SUPPLEMENTALMATHEMATICALOPERATORS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::MISCELLANEOUS_SYMBOLS_AND_ARROWS =
        new UnicodeBlock(String("MISCELLANEOUS_SYMBOLS_AND_ARROWS"),
                         String("MISCELLANEOUS SYMBOLS AND ARROWS"),
                         String("MISCELLANEOUSSYMBOLSANDARROWS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::KATAKANA_PHONETIC_EXTENSIONS =
        new UnicodeBlock(String("KATAKANA_PHONETIC_EXTENSIONS"),
                         String("KATAKANA PHONETIC EXTENSIONS"),
                         String("KATAKANAPHONETICEXTENSIONS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::YIJING_HEXAGRAM_SYMBOLS =
        new UnicodeBlock(String("YIJING_HEXAGRAM_SYMBOLS"),
                         String("YIJING HEXAGRAM SYMBOLS"),
                         String("YIJINGHEXAGRAMSYMBOLS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::VARIATION_SELECTORS =
        new UnicodeBlock(String("VARIATION_SELECTORS"),
                         String("VARIATION SELECTORS"),
                         String("VARIATIONSELECTORS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::LINEAR_B_SYLLABARY =
        new UnicodeBlock(String("LINEAR_B_SYLLABARY"),
                         String("LINEAR B SYLLABARY"),
                         String("LINEARBSYLLABARY"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::LINEAR_B_IDEOGRAMS =
        new UnicodeBlock(String("LINEAR_B_IDEOGRAMS"),
                         String("LINEAR B IDEOGRAMS"),
                         String("LINEARBIDEOGRAMS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::AEGEAN_NUMBERS =
        new UnicodeBlock(String("AEGEAN_NUMBERS"),
                         String("AEGEAN NUMBERS"),
                         String("AEGEANNUMBERS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::OLD_ITALIC =
        new UnicodeBlock(String("OLD_ITALIC"),
                         String("OLD ITALIC"),
                         String("OLDITALIC"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::GOTHIC =
        new UnicodeBlock(String("GOTHIC"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::UGARITIC =
        new UnicodeBlock(String("UGARITIC"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::DESERET =
        new UnicodeBlock(String("DESERET"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::SHAVIAN =
        new UnicodeBlock(String("SHAVIAN"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::OSMANYA =
        new UnicodeBlock(String("OSMANYA"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::CYPRIOT_SYLLABARY =
        new UnicodeBlock(String("CYPRIOT_SYLLABARY"),
                         String("CYPRIOT SYLLABARY"),
                         String("CYPRIOTSYLLABARY"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::BYZANTINE_MUSICAL_SYMBOLS =
        new UnicodeBlock(String("BYZANTINE_MUSICAL_SYMBOLS"),
                         String("BYZANTINE MUSICAL SYMBOLS"),
                         String("BYZANTINEMUSICALSYMBOLS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::MUSICAL_SYMBOLS =
        new UnicodeBlock(String("MUSICAL_SYMBOLS"),
                         String("MUSICAL SYMBOLS"),
                         String("MUSICALSYMBOLS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::TAI_XUAN_JING_SYMBOLS =
        new UnicodeBlock(String("TAI_XUAN_JING_SYMBOLS"),
                         String("TAI XUAN JING SYMBOLS"),
                         String("TAIXUANJINGSYMBOLS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::MATHEMATICAL_ALPHANUMERIC_SYMBOLS =
        new UnicodeBlock(String("MATHEMATICAL_ALPHANUMERIC_SYMBOLS"),
                         String("MATHEMATICAL ALPHANUMERIC SYMBOLS"),
                         String("MATHEMATICALALPHANUMERICSYMBOLS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::CJK_UNIFIED_IDEOGRAPHS_EXTENSION_B =
        new UnicodeBlock(String("CJK_UNIFIED_IDEOGRAPHS_EXTENSION_B"),
                         String("CJK UNIFIED IDEOGRAPHS EXTENSION B"),
                         String("CJKUNIFIEDIDEOGRAPHSEXTENSIONB"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::CJK_COMPATIBILITY_IDEOGRAPHS_SUPPLEMENT =
        new UnicodeBlock(String("CJK_COMPATIBILITY_IDEOGRAPHS_SUPPLEMENT"),
                         String("CJK COMPATIBILITY IDEOGRAPHS SUPPLEMENT"),
                         String("CJKCOMPATIBILITYIDEOGRAPHSSUPPLEMENT"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::TAGS =
        new UnicodeBlock(String("TAGS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::VARIATION_SELECTORS_SUPPLEMENT =
        new UnicodeBlock(String("VARIATION_SELECTORS_SUPPLEMENT"),
                         String("VARIATION SELECTORS SUPPLEMENT"),
                         String("VARIATIONSELECTORSSUPPLEMENT"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::SUPPLEMENTARY_PRIVATE_USE_AREA_A =
        new UnicodeBlock(String("SUPPLEMENTARY_PRIVATE_USE_AREA_A"),
                         String("SUPPLEMENTARY PRIVATE USE AREA-A"),
                         String("SUPPLEMENTARYPRIVATEUSEAREA-A"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::SUPPLEMENTARY_PRIVATE_USE_AREA_B =
        new UnicodeBlock(String("SUPPLEMENTARY_PRIVATE_USE_AREA_B"),
                         String("SUPPLEMENTARY PRIVATE USE AREA-B"),
                         String("SUPPLEMENTARYPRIVATEUSEAREA-B"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::HIGH_SURROGATES =
        new UnicodeBlock(String("HIGH_SURROGATES"),
                         String("HIGH SURROGATES"),
                         String("HIGHSURROGATES"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::HIGH_PRIVATE_USE_SURROGATES =
        new UnicodeBlock(String("HIGH_PRIVATE_USE_SURROGATES"),
                         String("HIGH PRIVATE USE SURROGATES"),
                         String("HIGHPRIVATEUSESURROGATES"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::LOW_SURROGATES =
        new UnicodeBlock(String("LOW_SURROGATES"),
                         String("LOW SURROGATES"),
                         String("LOWSURROGATES"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::ARABIC_SUPPLEMENT =
        new UnicodeBlock(String("ARABIC_SUPPLEMENT"),
                         String("ARABIC SUPPLEMENT"),
                         String("ARABICSUPPLEMENT"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::NKO =
        new UnicodeBlock(String("NKO"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::SAMARITAN =
        new UnicodeBlock(String("SAMARITAN"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::MANDAIC =
        new UnicodeBlock(String("MANDAIC"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::ETHIOPIC_SUPPLEMENT =
        new UnicodeBlock(String("ETHIOPIC_SUPPLEMENT"),
                         String("ETHIOPIC SUPPLEMENT"),
                         String("ETHIOPICSUPPLEMENT"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS_EXTENDED =
        new UnicodeBlock(String("UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS_EXTENDED"),
                         String("UNIFIED CANADIAN ABORIGINAL SYLLABICS EXTENDED"),
                         String("UNIFIEDCANADIANABORIGINALSYLLABICSEXTENDED"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::NEW_TAI_LUE =
        new UnicodeBlock(String("NEW_TAI_LUE"),
                         String("NEW TAI LUE"),
                         String("NEWTAILUE"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::BUGINESE =
        new UnicodeBlock(String("BUGINESE"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::TAI_THAM =
        new UnicodeBlock(String("TAI_THAM"),
                         String("TAI THAM"),
                         String("TAITHAM"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::BALINESE =
        new UnicodeBlock(String("BALINESE"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::SUNDANESE =
        new UnicodeBlock(String("SUNDANESE"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::BATAK =
        new UnicodeBlock(String("BATAK"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::LEPCHA =
        new UnicodeBlock(String("LEPCHA"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::OL_CHIKI =
        new UnicodeBlock(String("OL_CHIKI"),
                         String("OL CHIKI"),
                         String("OLCHIKI"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::VEDIC_EXTENSIONS =
        new UnicodeBlock(String("VEDIC_EXTENSIONS"),
                         String("VEDIC EXTENSIONS"),
                         String("VEDICEXTENSIONS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::PHONETIC_EXTENSIONS_SUPPLEMENT =
        new UnicodeBlock(String("PHONETIC_EXTENSIONS_SUPPLEMENT"),
                         String("PHONETIC EXTENSIONS SUPPLEMENT"),
                         String("PHONETICEXTENSIONSSUPPLEMENT"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::COMBINING_DIACRITICAL_MARKS_SUPPLEMENT =
        new UnicodeBlock(String("COMBINING_DIACRITICAL_MARKS_SUPPLEMENT"),
                         String("COMBINING DIACRITICAL MARKS SUPPLEMENT"),
                         String("COMBININGDIACRITICALMARKSSUPPLEMENT"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::GLAGOLITIC =
        new UnicodeBlock(String("GLAGOLITIC"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::LATIN_EXTENDED_C =
        new UnicodeBlock(String("LATIN_EXTENDED_C"),
                         String("LATIN EXTENDED-C"),
                         String("LATINEXTENDED-C"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::COPTIC =
        new UnicodeBlock(String("COPTIC"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::GEORGIAN_SUPPLEMENT =
        new UnicodeBlock(String("GEORGIAN_SUPPLEMENT"),
                         String("GEORGIAN SUPPLEMENT"),
                         String("GEORGIANSUPPLEMENT"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::TIFINAGH =
        new UnicodeBlock(String("TIFINAGH"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::ETHIOPIC_EXTENDED =
        new UnicodeBlock(String("ETHIOPIC_EXTENDED"),
                         String("ETHIOPIC EXTENDED"),
                         String("ETHIOPICEXTENDED"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::CYRILLIC_EXTENDED_A =
        new UnicodeBlock(String("CYRILLIC_EXTENDED_A"),
                         String("CYRILLIC EXTENDED-A"),
                         String("CYRILLICEXTENDED-A"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::SUPPLEMENTAL_PUNCTUATION =
        new UnicodeBlock(String("SUPPLEMENTAL_PUNCTUATION"),
                         String("SUPPLEMENTAL PUNCTUATION"),
                         String("SUPPLEMENTALPUNCTUATION"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::CJK_STROKES =
        new UnicodeBlock(String("CJK_STROKES"),
                         String("CJK STROKES"),
                         String("CJKSTROKES"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::LISU =
        new UnicodeBlock(String("LISU"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::VAI =
        new UnicodeBlock(String("VAI"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::CYRILLIC_EXTENDED_B =
        new UnicodeBlock(String("CYRILLIC_EXTENDED_B"),
                         String("CYRILLIC EXTENDED-B"),
                         String("CYRILLICEXTENDED-B"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::BAMUM =
        new UnicodeBlock(String("BAMUM"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::MODIFIER_TONE_LETTERS =
        new UnicodeBlock(String("MODIFIER_TONE_LETTERS"),
                         String("MODIFIER TONE LETTERS"),
                         String("MODIFIERTONELETTERS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::LATIN_EXTENDED_D =
        new UnicodeBlock(String("LATIN_EXTENDED_D"),
                         String("LATIN EXTENDED-D"),
                         String("LATINEXTENDED-D"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::SYLOTI_NAGRI =
        new UnicodeBlock(String("SYLOTI_NAGRI"),
                         String("SYLOTI NAGRI"),
                         String("SYLOTINAGRI"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::COMMON_INDIC_NUMBER_FORMS =
        new UnicodeBlock(String("COMMON_INDIC_NUMBER_FORMS"),
                         String("COMMON INDIC NUMBER FORMS"),
                         String("COMMONINDICNUMBERFORMS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::PHAGS_PA =
        new UnicodeBlock(String("PHAGS_PA"),
                         String("PHAGS-PA"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::SAURASHTRA =
        new UnicodeBlock(String("SAURASHTRA"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::DEVANAGARI_EXTENDED =
        new UnicodeBlock(String("DEVANAGARI_EXTENDED"),
                         String("DEVANAGARI EXTENDED"),
                         String("DEVANAGARIEXTENDED"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::KAYAH_LI =
        new UnicodeBlock(String("KAYAH_LI"),
                         String("KAYAH LI"),
                         String("KAYAHLI"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::REJANG =
        new UnicodeBlock(String("REJANG"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::HANGUL_JAMO_EXTENDED_A =
        new UnicodeBlock(String("HANGUL_JAMO_EXTENDED_A"),
                         String("HANGUL JAMO EXTENDED-A"),
                         String("HANGULJAMOEXTENDED-A"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::JAVANESE =
        new UnicodeBlock(String("JAVANESE"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::CHAM =
        new UnicodeBlock(String("CHAM"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::MYANMAR_EXTENDED_A =
        new UnicodeBlock(String("MYANMAR_EXTENDED_A"),
                         String("MYANMAR EXTENDED-A"),
                         String("MYANMAREXTENDED-A"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::TAI_VIET =
        new UnicodeBlock(String("TAI_VIET"),
                         String("TAI VIET"),
                         String("TAIVIET"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::ETHIOPIC_EXTENDED_A =
        new UnicodeBlock(String("ETHIOPIC_EXTENDED_A"),
                         String("ETHIOPIC EXTENDED-A"),
                         String("ETHIOPICEXTENDED-A"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::MEETEI_MAYEK =
        new UnicodeBlock(String("MEETEI_MAYEK"),
                         String("MEETEI MAYEK"),
                         String("MEETEIMAYEK"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::HANGUL_JAMO_EXTENDED_B =
        new UnicodeBlock(String("HANGUL_JAMO_EXTENDED_B"),
                         String("HANGUL JAMO EXTENDED-B"),
                         String("HANGULJAMOEXTENDED-B"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::VERTICAL_FORMS =
        new UnicodeBlock(String("VERTICAL_FORMS"),
                         String("VERTICAL FORMS"),
                         String("VERTICALFORMS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::ANCIENT_GREEK_NUMBERS =
        new UnicodeBlock(String("ANCIENT_GREEK_NUMBERS"),
                         String("ANCIENT GREEK NUMBERS"),
                         String("ANCIENTGREEKNUMBERS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::ANCIENT_SYMBOLS =
        new UnicodeBlock(String("ANCIENT_SYMBOLS"),
                         String("ANCIENT SYMBOLS"),
                         String("ANCIENTSYMBOLS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::PHAISTOS_DISC =
        new UnicodeBlock(String("PHAISTOS_DISC"),
                         String("PHAISTOS DISC"),
                         String("PHAISTOSDISC"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::LYCIAN =
        new UnicodeBlock(String("LYCIAN"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::CARIAN =
        new UnicodeBlock(String("CARIAN"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::OLD_PERSIAN =
        new UnicodeBlock(String("OLD_PERSIAN"),
                         String("OLD PERSIAN"),
                         String("OLDPERSIAN"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::IMPERIAL_ARAMAIC =
        new UnicodeBlock(String("IMPERIAL_ARAMAIC"),
                         String("IMPERIAL ARAMAIC"),
                         String("IMPERIALARAMAIC"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::PHOENICIAN =
        new UnicodeBlock(String("PHOENICIAN"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::LYDIAN =
        new UnicodeBlock(String("LYDIAN"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::KHAROSHTHI =
        new UnicodeBlock(String("KHAROSHTHI"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::OLD_SOUTH_ARABIAN =
        new UnicodeBlock(String("OLD_SOUTH_ARABIAN"),
                         String("OLD SOUTH ARABIAN"),
                         String("OLDSOUTHARABIAN"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::AVESTAN =
        new UnicodeBlock(String("AVESTAN"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::INSCRIPTIONAL_PARTHIAN =
        new UnicodeBlock(String("INSCRIPTIONAL_PARTHIAN"),
                         String("INSCRIPTIONAL PARTHIAN"),
                         String("INSCRIPTIONALPARTHIAN"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::INSCRIPTIONAL_PAHLAVI =
        new UnicodeBlock(String("INSCRIPTIONAL_PAHLAVI"),
                         String("INSCRIPTIONAL PAHLAVI"),
                         String("INSCRIPTIONALPAHLAVI"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::OLD_TURKIC =
        new UnicodeBlock(String("OLD_TURKIC"),
                         String("OLD TURKIC"),
                         String("OLDTURKIC"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::RUMI_NUMERAL_SYMBOLS =
        new UnicodeBlock(String("RUMI_NUMERAL_SYMBOLS"),
                         String("RUMI NUMERAL SYMBOLS"),
                         String("RUMINUMERALSYMBOLS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::BRAHMI =
        new UnicodeBlock(String("BRAHMI"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::KAITHI =
        new UnicodeBlock(String("KAITHI"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::CUNEIFORM =
        new UnicodeBlock(String("CUNEIFORM"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::CUNEIFORM_NUMBERS_AND_PUNCTUATION =
        new UnicodeBlock(String("CUNEIFORM_NUMBERS_AND_PUNCTUATION"),
                         String("CUNEIFORM NUMBERS AND PUNCTUATION"),
                         String("CUNEIFORMNUMBERSANDPUNCTUATION"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::EGYPTIAN_HIEROGLYPHS =
        new UnicodeBlock(String("EGYPTIAN_HIEROGLYPHS"),
                         String("EGYPTIAN HIEROGLYPHS"),
                         String("EGYPTIANHIEROGLYPHS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::BAMUM_SUPPLEMENT =
        new UnicodeBlock(String("BAMUM_SUPPLEMENT"),
                         String("BAMUM SUPPLEMENT"),
                         String("BAMUMSUPPLEMENT"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::KANA_SUPPLEMENT =
        new UnicodeBlock(String("KANA_SUPPLEMENT"),
                         String("KANA SUPPLEMENT"),
                         String("KANASUPPLEMENT"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::ANCIENT_GREEK_MUSICAL_NOTATION =
        new UnicodeBlock(String("ANCIENT_GREEK_MUSICAL_NOTATION"),
                         String("ANCIENT GREEK MUSICAL NOTATION"),
                         String("ANCIENTGREEKMUSICALNOTATION"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::COUNTING_ROD_NUMERALS =
        new UnicodeBlock(String("COUNTING_ROD_NUMERALS"),
                         String("COUNTING ROD NUMERALS"),
                         String("COUNTINGRODNUMERALS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::MAHJONG_TILES =
        new UnicodeBlock(String("MAHJONG_TILES"),
                         String("MAHJONG TILES"),
                         String("MAHJONGTILES"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::DOMINO_TILES =
        new UnicodeBlock(String("DOMINO_TILES"),
                         String("DOMINO TILES"),
                         String("DOMINOTILES"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::PLAYING_CARDS =
        new UnicodeBlock(String("PLAYING_CARDS"),
                         String("PLAYING CARDS"),
                         String("PLAYINGCARDS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::ENCLOSED_ALPHANUMERIC_SUPPLEMENT =
        new UnicodeBlock(String("ENCLOSED_ALPHANUMERIC_SUPPLEMENT"),
                         String("ENCLOSED ALPHANUMERIC SUPPLEMENT"),
                         String("ENCLOSEDALPHANUMERICSUPPLEMENT"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::ENCLOSED_IDEOGRAPHIC_SUPPLEMENT =
        new UnicodeBlock(String("ENCLOSED_IDEOGRAPHIC_SUPPLEMENT"),
                         String("ENCLOSED IDEOGRAPHIC SUPPLEMENT"),
                         String("ENCLOSEDIDEOGRAPHICSUPPLEMENT"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::MISCELLANEOUS_SYMBOLS_AND_PICTOGRAPHS =
        new UnicodeBlock(String("MISCELLANEOUS_SYMBOLS_AND_PICTOGRAPHS"),
                         String("MISCELLANEOUS SYMBOLS AND PICTOGRAPHS"),
                         String("MISCELLANEOUSSYMBOLSANDPICTOGRAPHS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::EMOTICONS =
        new UnicodeBlock(String("EMOTICONS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::TRANSPORT_AND_MAP_SYMBOLS =
        new UnicodeBlock(String("TRANSPORT_AND_MAP_SYMBOLS"),
                         String("TRANSPORT AND MAP SYMBOLS"),
                         String("TRANSPORTANDMAPSYMBOLS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::ALCHEMICAL_SYMBOLS =
        new UnicodeBlock(String("ALCHEMICAL_SYMBOLS"),
                         String("ALCHEMICAL SYMBOLS"),
                         String("ALCHEMICALSYMBOLS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::CJK_UNIFIED_IDEOGRAPHS_EXTENSION_C =
        new UnicodeBlock(String("CJK_UNIFIED_IDEOGRAPHS_EXTENSION_C"),
                         String("CJK UNIFIED IDEOGRAPHS EXTENSION C"),
                         String("CJKUNIFIEDIDEOGRAPHSEXTENSIONC"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::CJK_UNIFIED_IDEOGRAPHS_EXTENSION_D =
        new UnicodeBlock(String("CJK_UNIFIED_IDEOGRAPHS_EXTENSION_D"),
                         String("CJK UNIFIED IDEOGRAPHS EXTENSION D"),
                         String("CJKUNIFIEDIDEOGRAPHSEXTENSIOND"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::ARABIC_EXTENDED_A =
        new UnicodeBlock(String("ARABIC_EXTENDED_A"),
                         String("ARABIC EXTENDED-A"),
                         String("ARABICEXTENDED-A"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::SUNDANESE_SUPPLEMENT =
        new UnicodeBlock(String("SUNDANESE_SUPPLEMENT"),
                         String("SUNDANESE SUPPLEMENT"),
                         String("SUNDANESESUPPLEMENT"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::MEETEI_MAYEK_EXTENSIONS =
        new UnicodeBlock(String("MEETEI_MAYEK_EXTENSIONS"),
                         String("MEETEI MAYEK EXTENSIONS"),
                         String("MEETEIMAYEKEXTENSIONS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::MEROITIC_HIEROGLYPHS =
        new UnicodeBlock(String("MEROITIC_HIEROGLYPHS"),
                         String("MEROITIC HIEROGLYPHS"),
                         String("MEROITICHIEROGLYPHS"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::MEROITIC_CURSIVE =
        new UnicodeBlock(String("MEROITIC_CURSIVE"),
                         String("MEROITIC CURSIVE"),
                         String("MEROITICCURSIVE"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::SORA_SOMPENG =
        new UnicodeBlock(String("SORA_SOMPENG"),
                         String("SORA SOMPENG"),
                         String("SORASOMPENG"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::CHAKMA =
        new UnicodeBlock(String("CHAKMA"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::SHARADA =
        new UnicodeBlock(String("SHARADA"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::TAKRI =
        new UnicodeBlock(String("TAKRI"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::MIAO =
        new UnicodeBlock(String("MIAO"));
AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::ARABIC_MATHEMATICAL_ALPHABETIC_SYMBOLS =
        new UnicodeBlock(String("ARABIC_MATHEMATICAL_ALPHABETIC_SYMBOLS"),
                         String("ARABIC MATHEMATICAL ALPHABETIC SYMBOLS"),
                         String("ARABICMATHEMATICALALPHABETICSYMBOLS"));

constexpr Integer Character::UnicodeBlock::sBlockStarts[];

Character::UnicodeBlock* const Character::UnicodeBlock::sBlocks[] = {
    BASIC_LATIN,
    LATIN_1_SUPPLEMENT,
    LATIN_EXTENDED_A,
    LATIN_EXTENDED_B,
    IPA_EXTENSIONS,
    SPACING_MODIFIER_LETTERS,
    COMBINING_DIACRITICAL_MARKS,
    GREEK,
    CYRILLIC,
    CYRILLIC_SUPPLEMENTARY,
    ARMENIAN,
    HEBREW,
    ARABIC,
    SYRIAC,
    ARABIC_SUPPLEMENT,
    THAANA,
    NKO,
    SAMARITAN,
    MANDAIC,
    nullptr,
    ARABIC_EXTENDED_A,
    DEVANAGARI,
    BENGALI,
    GURMUKHI,
    GUJARATI,
    ORIYA,
    TAMIL,
    TELUGU,
    KANNADA,
    MALAYALAM,
    SINHALA,
    THAI,
    LAO,
    TIBETAN,
    MYANMAR,
    GEORGIAN,
    HANGUL_JAMO,
    ETHIOPIC,
    ETHIOPIC_SUPPLEMENT,
    CHEROKEE,
    UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS,
    OGHAM,
    RUNIC,
    TAGALOG,
    HANUNOO,
    BUHID,
    TAGBANWA,
    KHMER,
    MONGOLIAN,
    UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS_EXTENDED,
    LIMBU,
    TAI_LE,
    NEW_TAI_LUE,
    KHMER_SYMBOLS,
    BUGINESE,
    TAI_THAM,
    nullptr,
    BALINESE,
    SUNDANESE,
    BATAK,
    LEPCHA,
    OL_CHIKI,
    nullptr,
    SUNDANESE_SUPPLEMENT,
    VEDIC_EXTENSIONS,
    PHONETIC_EXTENSIONS,
    PHONETIC_EXTENSIONS_SUPPLEMENT,
    COMBINING_DIACRITICAL_MARKS_SUPPLEMENT,
    LATIN_EXTENDED_ADDITIONAL,
    GREEK_EXTENDED,
    GENERAL_PUNCTUATION,
    SUPERSCRIPTS_AND_SUBSCRIPTS,
    CURRENCY_SYMBOLS,
    COMBINING_MARKS_FOR_SYMBOLS,
    LETTERLIKE_SYMBOLS,
    NUMBER_FORMS,
    ARROWS,
    MATHEMATICAL_OPERATORS,
    MISCELLANEOUS_TECHNICAL,
    CONTROL_PICTURES,
    OPTICAL_CHARACTER_RECOGNITION,
    ENCLOSED_ALPHANUMERICS,
    BOX_DRAWING,
    BLOCK_ELEMENTS,
    GEOMETRIC_SHAPES,
    MISCELLANEOUS_SYMBOLS,
    DINGBATS,
    MISCELLANEOUS_MATHEMATICAL_SYMBOLS_A,
    SUPPLEMENTAL_ARROWS_A,
    BRAILLE_PATTERNS,
    SUPPLEMENTAL_ARROWS_B,
    MISCELLANEOUS_MATHEMATICAL_SYMBOLS_B,
    SUPPLEMENTAL_MATHEMATICAL_OPERATORS,
    MISCELLANEOUS_SYMBOLS_AND_ARROWS,
    GLAGOLITIC,
    LATIN_EXTENDED_C,
    COPTIC,
    GEORGIAN_SUPPLEMENT,
    TIFINAGH,
    ETHIOPIC_EXTENDED,
    CYRILLIC_EXTENDED_A,
    SUPPLEMENTAL_PUNCTUATION,
    CJK_RADICALS_SUPPLEMENT,
    KANGXI_RADICALS,
    nullptr,
    IDEOGRAPHIC_DESCRIPTION_CHARACTERS,
    CJK_SYMBOLS_AND_PUNCTUATION,
    HIRAGANA,
    KATAKANA,
    BOPOMOFO,
    HANGUL_COMPATIBILITY_JAMO,
    KANBUN,
    BOPOMOFO_EXTENDED,
    CJK_STROKES,
    KATAKANA_PHONETIC_EXTENSIONS,
    ENCLOSED_CJK_LETTERS_AND_MONTHS,
    CJK_COMPATIBILITY,
    CJK_UNIFIED_IDEOGRAPHS_EXTENSION_A,
    YIJING_HEXAGRAM_SYMBOLS,
    CJK_UNIFIED_IDEOGRAPHS,
    YI_SYLLABLES,
    YI_RADICALS,
    LISU,
    VAI,
    CYRILLIC_EXTENDED_B,
    BAMUM,
    MODIFIER_TONE_LETTERS,
    LATIN_EXTENDED_D,
    SYLOTI_NAGRI,
    COMMON_INDIC_NUMBER_FORMS,
    PHAGS_PA,
    SAURASHTRA,
    DEVANAGARI_EXTENDED,
    KAYAH_LI,
    REJANG,
    HANGUL_JAMO_EXTENDED_A,
    JAVANESE,
    nullptr,
    CHAM,
    MYANMAR_EXTENDED_A,
    TAI_VIET,
    MEETEI_MAYEK_EXTENSIONS,
    ETHIOPIC_EXTENDED_A,
    nullptr,
    MEETEI_MAYEK,
    HANGUL_SYLLABLES,
    HANGUL_JAMO_EXTENDED_B,
    HIGH_SURROGATES,
    HIGH_PRIVATE_USE_SURROGATES,
    LOW_SURROGATES,
    PRIVATE_USE_AREA,
    CJK_COMPATIBILITY_IDEOGRAPHS,
    ALPHABETIC_PRESENTATION_FORMS,
    ARABIC_PRESENTATION_FORMS_A,
    VARIATION_SELECTORS,
    VERTICAL_FORMS,
    COMBINING_HALF_MARKS,
    CJK_COMPATIBILITY_FORMS,
    SMALL_FORM_VARIANTS,
    ARABIC_PRESENTATION_FORMS_B,
    HALFWIDTH_AND_FULLWIDTH_FORMS,
    SPECIALS,
    LINEAR_B_SYLLABARY,
    LINEAR_B_IDEOGRAMS,
    AEGEAN_NUMBERS,
    ANCIENT_GREEK_NUMBERS,
    ANCIENT_SYMBOLS,
    PHAISTOS_DISC,
    nullptr,
    LYCIAN,
    CARIAN,
    nullptr,
    OLD_ITALIC,
    GOTHIC,
    nullptr,
    UGARITIC,
    OLD_PERSIAN,
    nullptr,
    DESERET,
    SHAVIAN,
    OSMANYA,
    nullptr,
    CYPRIOT_SYLLABARY,
    IMPERIAL_ARAMAIC,
    nullptr,
    PHOENICIAN,
    LYDIAN,
    nullptr,
    MEROITIC_HIEROGLYPHS,
    MEROITIC_CURSIVE,
    KHAROSHTHI,
    OLD_SOUTH_ARABIAN,
    nullptr,
    AVESTAN,
    INSCRIPTIONAL_PARTHIAN,
    INSCRIPTIONAL_PAHLAVI,
    nullptr,
    OLD_TURKIC,
    nullptr,
    RUMI_NUMERAL_SYMBOLS,
    nullptr,
    BRAHMI,
    KAITHI,
    SORA_SOMPENG,
    CHAKMA,
    nullptr,
    SHARADA,
    nullptr,
    TAKRI,
    nullptr,
    CUNEIFORM,
    CUNEIFORM_NUMBERS_AND_PUNCTUATION,
    nullptr,
    EGYPTIAN_HIEROGLYPHS,
    nullptr,
    BAMUM_SUPPLEMENT,
    nullptr,
    MIAO,
    nullptr,
    KANA_SUPPLEMENT,
    nullptr,
    BYZANTINE_MUSICAL_SYMBOLS,
    MUSICAL_SYMBOLS,
    ANCIENT_GREEK_MUSICAL_NOTATION,
    nullptr,
    TAI_XUAN_JING_SYMBOLS,
    COUNTING_ROD_NUMERALS,
    nullptr,
    MATHEMATICAL_ALPHANUMERIC_SYMBOLS,
    nullptr,
    ARABIC_MATHEMATICAL_ALPHABETIC_SYMBOLS,
    nullptr,
    MAHJONG_TILES,
    DOMINO_TILES,
    PLAYING_CARDS,
    ENCLOSED_ALPHANUMERIC_SUPPLEMENT,
    ENCLOSED_IDEOGRAPHIC_SUPPLEMENT,
    MISCELLANEOUS_SYMBOLS_AND_PICTOGRAPHS,
    EMOTICONS,
    nullptr,
    TRANSPORT_AND_MAP_SYMBOLS,
    ALCHEMICAL_SYMBOLS,
    nullptr,
    CJK_UNIFIED_IDEOGRAPHS_EXTENSION_B,
    nullptr,
    CJK_UNIFIED_IDEOGRAPHS_EXTENSION_C,
    CJK_UNIFIED_IDEOGRAPHS_EXTENSION_D,
    nullptr,
    CJK_COMPATIBILITY_IDEOGRAPHS_SUPPLEMENT,
    nullptr,
    TAGS,
    nullptr,
    VARIATION_SELECTORS_SUPPLEMENT,
    nullptr,
    SUPPLEMENTARY_PRIVATE_USE_AREA_A,
    SUPPLEMENTARY_PRIVATE_USE_AREA_B
};

AutoPtr<Character::UnicodeBlock> Character::UnicodeBlock::Of(
    /* [in] */ Char c)
{
    if (!IsValidCodePoint(c)) {
        return nullptr;
    }

    Integer top, bottom, current;
    bottom = 0;
    top = ArrayLength(sBlockStarts);
    current = top / 2;

    // invariant: top > current >= bottom && codePoint >= unicodeBlockStarts[bottom]
    while (top - bottom > 1) {
        if (c >= sBlockStarts[current]) {
            bottom = current;
        }
        else {
            top = current;
        }
        current = (top + bottom) / 2;
    }
    return sBlocks[current];
}

}
}
