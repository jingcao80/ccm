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

#ifndef __COMO_CORE_CHARACTER_H__
#define __COMO_CORE_CHARACTER_H__

#include <ccmautoptr.h>
#include <ccmrefbase.h>
#include <ccmtypes.h>

namespace como {
namespace core {

class Character final
{
public:
    class Subset
        : public LightRefBase
    {
    protected:
        Subset(
            /* [in] */ const String& name)
            : mName(name)
        {}

    private:
        String mName;
    };

    class UnicodeBlock
        : public Subset
    {
    public:
        static AutoPtr<UnicodeBlock> Of(
            /* [in] */ Char c);

    private:
        UnicodeBlock(
            /* [in] */ const String& idName);

        UnicodeBlock(
            /* [in] */ const String& idName,
            /* [in] */ Boolean isMap);

        UnicodeBlock(
            /* [in] */ const String& idName,
            /* [in] */ const String& alias);

        UnicodeBlock(
            /* [in] */ const String& idName,
            /* [in] */ const String& alias1,
            /* [in] */ const String& alias2);

        UnicodeBlock(
            /* [in] */ const String& idName,
            /* [in] */ const String& alias1,
            /* [in] */ const String& alias2,
            /* [in] */ const String& alias3,
            /* [in] */ const String& alias4);

    private:
        static AutoPtr<UnicodeBlock> BASIC_LATIN;

        static AutoPtr<UnicodeBlock> LATIN_1_SUPPLEMENT;

        static AutoPtr<UnicodeBlock> LATIN_EXTENDED_A;

        static AutoPtr<UnicodeBlock> LATIN_EXTENDED_B;

        static AutoPtr<UnicodeBlock> IPA_EXTENSIONS;

        static AutoPtr<UnicodeBlock> SPACING_MODIFIER_LETTERS;

        static AutoPtr<UnicodeBlock> COMBINING_DIACRITICAL_MARKS;

        static AutoPtr<UnicodeBlock> GREEK;

        static AutoPtr<UnicodeBlock> CYRILLIC;

        static AutoPtr<UnicodeBlock> ARMENIAN;

        static AutoPtr<UnicodeBlock> HEBREW;

        static AutoPtr<UnicodeBlock> ARABIC;

        static AutoPtr<UnicodeBlock> DEVANAGARI;

        static AutoPtr<UnicodeBlock> BENGALI;

        static AutoPtr<UnicodeBlock> GURMUKHI;

        static AutoPtr<UnicodeBlock> GUJARATI;

        static AutoPtr<UnicodeBlock> ORIYA;

        static AutoPtr<UnicodeBlock> TAMIL;

        static AutoPtr<UnicodeBlock> TELUGU;

        static AutoPtr<UnicodeBlock> KANNADA;

        static AutoPtr<UnicodeBlock> MALAYALAM;

        static AutoPtr<UnicodeBlock> THAI;

        static AutoPtr<UnicodeBlock> LAO;

        static AutoPtr<UnicodeBlock> TIBETAN;

        static AutoPtr<UnicodeBlock> GEORGIAN;

        static AutoPtr<UnicodeBlock> HANGUL_JAMO;

        static AutoPtr<UnicodeBlock> LATIN_EXTENDED_ADDITIONAL;

        static AutoPtr<UnicodeBlock> GREEK_EXTENDED;

        static AutoPtr<UnicodeBlock> GENERAL_PUNCTUATION;

        static AutoPtr<UnicodeBlock> SUPERSCRIPTS_AND_SUBSCRIPTS;

        static AutoPtr<UnicodeBlock> CURRENCY_SYMBOLS;

        static AutoPtr<UnicodeBlock> COMBINING_MARKS_FOR_SYMBOLS;

        static AutoPtr<UnicodeBlock> LETTERLIKE_SYMBOLS;

        static AutoPtr<UnicodeBlock> NUMBER_FORMS;

        static AutoPtr<UnicodeBlock> ARROWS;

        static AutoPtr<UnicodeBlock> MATHEMATICAL_OPERATORS;

        static AutoPtr<UnicodeBlock> MISCELLANEOUS_TECHNICAL;

        static AutoPtr<UnicodeBlock> CONTROL_PICTURES;

        static AutoPtr<UnicodeBlock> OPTICAL_CHARACTER_RECOGNITION;

        static AutoPtr<UnicodeBlock> ENCLOSED_ALPHANUMERICS;

        static AutoPtr<UnicodeBlock> BOX_DRAWING;

        static AutoPtr<UnicodeBlock> BLOCK_ELEMENTS;

        static AutoPtr<UnicodeBlock> GEOMETRIC_SHAPES;

        static AutoPtr<UnicodeBlock> MISCELLANEOUS_SYMBOLS;

        static AutoPtr<UnicodeBlock> DINGBATS;

        static AutoPtr<UnicodeBlock> CJK_SYMBOLS_AND_PUNCTUATION;

        static AutoPtr<UnicodeBlock> HIRAGANA;

        static AutoPtr<UnicodeBlock> KATAKANA;

        static AutoPtr<UnicodeBlock> BOPOMOFO;

        static AutoPtr<UnicodeBlock> HANGUL_COMPATIBILITY_JAMO;

        static AutoPtr<UnicodeBlock> KANBUN;

        static AutoPtr<UnicodeBlock> ENCLOSED_CJK_LETTERS_AND_MONTHS;

        static AutoPtr<UnicodeBlock> CJK_COMPATIBILITY;

        static AutoPtr<UnicodeBlock> CJK_UNIFIED_IDEOGRAPHS;

        static AutoPtr<UnicodeBlock> HANGUL_SYLLABLES;

        static AutoPtr<UnicodeBlock> PRIVATE_USE_AREA;

        static AutoPtr<UnicodeBlock> CJK_COMPATIBILITY_IDEOGRAPHS;

        static AutoPtr<UnicodeBlock> ALPHABETIC_PRESENTATION_FORMS;

        static AutoPtr<UnicodeBlock> ARABIC_PRESENTATION_FORMS_A;

        static AutoPtr<UnicodeBlock> COMBINING_HALF_MARKS;

        static AutoPtr<UnicodeBlock> CJK_COMPATIBILITY_FORMS;

        static AutoPtr<UnicodeBlock> SMALL_FORM_VARIANTS;

        static AutoPtr<UnicodeBlock> ARABIC_PRESENTATION_FORMS_B;

        static AutoPtr<UnicodeBlock> HALFWIDTH_AND_FULLWIDTH_FORMS;

        static AutoPtr<UnicodeBlock> SPECIALS;

        static AutoPtr<UnicodeBlock> SURROGATES_AREA;

        static AutoPtr<UnicodeBlock> SYRIAC;

        static AutoPtr<UnicodeBlock> THAANA;

        static AutoPtr<UnicodeBlock> SINHALA;

        static AutoPtr<UnicodeBlock> MYANMAR;

        static AutoPtr<UnicodeBlock> ETHIOPIC;

        static AutoPtr<UnicodeBlock> CHEROKEE;

        static AutoPtr<UnicodeBlock> UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS;

        static AutoPtr<UnicodeBlock> OGHAM;

        static AutoPtr<UnicodeBlock> RUNIC;

        static AutoPtr<UnicodeBlock> KHMER;

        static AutoPtr<UnicodeBlock> MONGOLIAN;

        static AutoPtr<UnicodeBlock> BRAILLE_PATTERNS;

        static AutoPtr<UnicodeBlock> CJK_RADICALS_SUPPLEMENT;

        static AutoPtr<UnicodeBlock> KANGXI_RADICALS;

        static AutoPtr<UnicodeBlock> IDEOGRAPHIC_DESCRIPTION_CHARACTERS;

        static AutoPtr<UnicodeBlock> BOPOMOFO_EXTENDED;

        static AutoPtr<UnicodeBlock> CJK_UNIFIED_IDEOGRAPHS_EXTENSION_A;

        static AutoPtr<UnicodeBlock> YI_SYLLABLES;

        static AutoPtr<UnicodeBlock> YI_RADICALS;

        static AutoPtr<UnicodeBlock> CYRILLIC_SUPPLEMENTARY;

        static AutoPtr<UnicodeBlock> TAGALOG;

        static AutoPtr<UnicodeBlock> HANUNOO;

        static AutoPtr<UnicodeBlock> BUHID;

        static AutoPtr<UnicodeBlock> TAGBANWA;

        static AutoPtr<UnicodeBlock> LIMBU;

        static AutoPtr<UnicodeBlock> TAI_LE;

        static AutoPtr<UnicodeBlock> KHMER_SYMBOLS;

        static AutoPtr<UnicodeBlock> PHONETIC_EXTENSIONS;

        static AutoPtr<UnicodeBlock> MISCELLANEOUS_MATHEMATICAL_SYMBOLS_A;

        static AutoPtr<UnicodeBlock> SUPPLEMENTAL_ARROWS_A;

        static AutoPtr<UnicodeBlock> SUPPLEMENTAL_ARROWS_B;

        static AutoPtr<UnicodeBlock> MISCELLANEOUS_MATHEMATICAL_SYMBOLS_B;

        static AutoPtr<UnicodeBlock> SUPPLEMENTAL_MATHEMATICAL_OPERATORS;

        static AutoPtr<UnicodeBlock> MISCELLANEOUS_SYMBOLS_AND_ARROWS;

        static AutoPtr<UnicodeBlock> KATAKANA_PHONETIC_EXTENSIONS;

        static AutoPtr<UnicodeBlock> YIJING_HEXAGRAM_SYMBOLS;

        static AutoPtr<UnicodeBlock> VARIATION_SELECTORS;

        static AutoPtr<UnicodeBlock> LINEAR_B_SYLLABARY;

        static AutoPtr<UnicodeBlock> LINEAR_B_IDEOGRAMS;

        static AutoPtr<UnicodeBlock> AEGEAN_NUMBERS;

        static AutoPtr<UnicodeBlock> OLD_ITALIC;

        static AutoPtr<UnicodeBlock> GOTHIC;

        static AutoPtr<UnicodeBlock> UGARITIC;

        static AutoPtr<UnicodeBlock> DESERET;

        static AutoPtr<UnicodeBlock> SHAVIAN;

        static AutoPtr<UnicodeBlock> OSMANYA;

        static AutoPtr<UnicodeBlock> CYPRIOT_SYLLABARY;

        static AutoPtr<UnicodeBlock> BYZANTINE_MUSICAL_SYMBOLS;

        static AutoPtr<UnicodeBlock> MUSICAL_SYMBOLS;

        static AutoPtr<UnicodeBlock> TAI_XUAN_JING_SYMBOLS;

        static AutoPtr<UnicodeBlock> MATHEMATICAL_ALPHANUMERIC_SYMBOLS;

        static AutoPtr<UnicodeBlock> CJK_UNIFIED_IDEOGRAPHS_EXTENSION_B;

        static AutoPtr<UnicodeBlock> CJK_COMPATIBILITY_IDEOGRAPHS_SUPPLEMENT;

        static AutoPtr<UnicodeBlock> TAGS;

        static AutoPtr<UnicodeBlock> VARIATION_SELECTORS_SUPPLEMENT;

        static AutoPtr<UnicodeBlock> SUPPLEMENTARY_PRIVATE_USE_AREA_A;

        static AutoPtr<UnicodeBlock> SUPPLEMENTARY_PRIVATE_USE_AREA_B;

        static AutoPtr<UnicodeBlock> HIGH_SURROGATES;

        static AutoPtr<UnicodeBlock> HIGH_PRIVATE_USE_SURROGATES;

        static AutoPtr<UnicodeBlock> LOW_SURROGATES;

        static AutoPtr<UnicodeBlock> ARABIC_SUPPLEMENT;

        static AutoPtr<UnicodeBlock> NKO;

        static AutoPtr<UnicodeBlock> SAMARITAN;

        static AutoPtr<UnicodeBlock> MANDAIC;

        static AutoPtr<UnicodeBlock> ETHIOPIC_SUPPLEMENT;

        static AutoPtr<UnicodeBlock> UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS_EXTENDED;

        static AutoPtr<UnicodeBlock> NEW_TAI_LUE;

        static AutoPtr<UnicodeBlock> BUGINESE;

        static AutoPtr<UnicodeBlock> TAI_THAM;

        static AutoPtr<UnicodeBlock> BALINESE;

        static AutoPtr<UnicodeBlock> SUNDANESE;

        static AutoPtr<UnicodeBlock> BATAK;

        static AutoPtr<UnicodeBlock> LEPCHA;

        static AutoPtr<UnicodeBlock> OL_CHIKI;

        static AutoPtr<UnicodeBlock> VEDIC_EXTENSIONS;

        static AutoPtr<UnicodeBlock> PHONETIC_EXTENSIONS_SUPPLEMENT;

        static AutoPtr<UnicodeBlock> COMBINING_DIACRITICAL_MARKS_SUPPLEMENT;

        static AutoPtr<UnicodeBlock> GLAGOLITIC;

        static AutoPtr<UnicodeBlock> LATIN_EXTENDED_C;

        static AutoPtr<UnicodeBlock> COPTIC;

        static AutoPtr<UnicodeBlock> GEORGIAN_SUPPLEMENT;

        static AutoPtr<UnicodeBlock> TIFINAGH;

        static AutoPtr<UnicodeBlock> ETHIOPIC_EXTENDED;

        static AutoPtr<UnicodeBlock> CYRILLIC_EXTENDED_A;

        static AutoPtr<UnicodeBlock> SUPPLEMENTAL_PUNCTUATION;

        static AutoPtr<UnicodeBlock> CJK_STROKES;

        static AutoPtr<UnicodeBlock> LISU;

        static AutoPtr<UnicodeBlock> VAI;

        static AutoPtr<UnicodeBlock> CYRILLIC_EXTENDED_B;

        static AutoPtr<UnicodeBlock> BAMUM;

        static AutoPtr<UnicodeBlock> MODIFIER_TONE_LETTERS;

        static AutoPtr<UnicodeBlock> LATIN_EXTENDED_D;

        static AutoPtr<UnicodeBlock> SYLOTI_NAGRI;

        static AutoPtr<UnicodeBlock> COMMON_INDIC_NUMBER_FORMS;

        static AutoPtr<UnicodeBlock> PHAGS_PA;

        static AutoPtr<UnicodeBlock> SAURASHTRA;

        static AutoPtr<UnicodeBlock> DEVANAGARI_EXTENDED;

        static AutoPtr<UnicodeBlock> KAYAH_LI;

        static AutoPtr<UnicodeBlock> REJANG;

        static AutoPtr<UnicodeBlock> HANGUL_JAMO_EXTENDED_A;

        static AutoPtr<UnicodeBlock> JAVANESE;

        static AutoPtr<UnicodeBlock> CHAM;

        static AutoPtr<UnicodeBlock> MYANMAR_EXTENDED_A;

        static AutoPtr<UnicodeBlock> TAI_VIET;

        static AutoPtr<UnicodeBlock> ETHIOPIC_EXTENDED_A;

        static AutoPtr<UnicodeBlock> MEETEI_MAYEK;

        static AutoPtr<UnicodeBlock> HANGUL_JAMO_EXTENDED_B;

        static AutoPtr<UnicodeBlock> VERTICAL_FORMS;

        static AutoPtr<UnicodeBlock> ANCIENT_GREEK_NUMBERS;

        static AutoPtr<UnicodeBlock> ANCIENT_SYMBOLS;

        static AutoPtr<UnicodeBlock> PHAISTOS_DISC;

        static AutoPtr<UnicodeBlock> LYCIAN;

        static AutoPtr<UnicodeBlock> CARIAN;

        static AutoPtr<UnicodeBlock> OLD_PERSIAN;

        static AutoPtr<UnicodeBlock> IMPERIAL_ARAMAIC;

        static AutoPtr<UnicodeBlock> PHOENICIAN;

        static AutoPtr<UnicodeBlock> LYDIAN;

        static AutoPtr<UnicodeBlock> KHAROSHTHI;

        static AutoPtr<UnicodeBlock> OLD_SOUTH_ARABIAN;

        static AutoPtr<UnicodeBlock> AVESTAN;

        static AutoPtr<UnicodeBlock> INSCRIPTIONAL_PARTHIAN;

        static AutoPtr<UnicodeBlock> INSCRIPTIONAL_PAHLAVI;

        static AutoPtr<UnicodeBlock> OLD_TURKIC;

        static AutoPtr<UnicodeBlock> RUMI_NUMERAL_SYMBOLS;

        static AutoPtr<UnicodeBlock> BRAHMI;

        static AutoPtr<UnicodeBlock> KAITHI;

        static AutoPtr<UnicodeBlock> CUNEIFORM;

        static AutoPtr<UnicodeBlock> CUNEIFORM_NUMBERS_AND_PUNCTUATION;

        static AutoPtr<UnicodeBlock> EGYPTIAN_HIEROGLYPHS;

        static AutoPtr<UnicodeBlock> BAMUM_SUPPLEMENT;

        static AutoPtr<UnicodeBlock> KANA_SUPPLEMENT;

        static AutoPtr<UnicodeBlock> ANCIENT_GREEK_MUSICAL_NOTATION;

        static AutoPtr<UnicodeBlock> COUNTING_ROD_NUMERALS;

        static AutoPtr<UnicodeBlock> MAHJONG_TILES;

        static AutoPtr<UnicodeBlock> DOMINO_TILES;

        static AutoPtr<UnicodeBlock> PLAYING_CARDS;

        static AutoPtr<UnicodeBlock> ENCLOSED_ALPHANUMERIC_SUPPLEMENT;

        static AutoPtr<UnicodeBlock> ENCLOSED_IDEOGRAPHIC_SUPPLEMENT;

        static AutoPtr<UnicodeBlock> MISCELLANEOUS_SYMBOLS_AND_PICTOGRAPHS;

        static AutoPtr<UnicodeBlock> EMOTICONS;

        static AutoPtr<UnicodeBlock> TRANSPORT_AND_MAP_SYMBOLS;

        static AutoPtr<UnicodeBlock> ALCHEMICAL_SYMBOLS;

        static AutoPtr<UnicodeBlock> CJK_UNIFIED_IDEOGRAPHS_EXTENSION_C;

        static AutoPtr<UnicodeBlock> CJK_UNIFIED_IDEOGRAPHS_EXTENSION_D;

        static AutoPtr<UnicodeBlock> ARABIC_EXTENDED_A;

        static AutoPtr<UnicodeBlock> SUNDANESE_SUPPLEMENT;

        static AutoPtr<UnicodeBlock> MEETEI_MAYEK_EXTENSIONS;

        static AutoPtr<UnicodeBlock> MEROITIC_HIEROGLYPHS;

        static AutoPtr<UnicodeBlock> MEROITIC_CURSIVE;

        static AutoPtr<UnicodeBlock> SORA_SOMPENG;

        static AutoPtr<UnicodeBlock> CHAKMA;

        static AutoPtr<UnicodeBlock> SHARADA;

        static AutoPtr<UnicodeBlock> TAKRI;

        static AutoPtr<UnicodeBlock> MIAO;

        static AutoPtr<UnicodeBlock> ARABIC_MATHEMATICAL_ALPHABETIC_SYMBOLS;

        static constexpr Integer sBlockStarts[] = {
            0x0000,   // 0000..007F; Basic Latin
            0x0080,   // 0080..00FF; Latin-1 Supplement
            0x0100,   // 0100..017F; Latin Extended-A
            0x0180,   // 0180..024F; Latin Extended-B
            0x0250,   // 0250..02AF; IPA Extensions
            0x02B0,   // 02B0..02FF; Spacing Modifier Letters
            0x0300,   // 0300..036F; Combining Diacritical Marks
            0x0370,   // 0370..03FF; Greek and Coptic
            0x0400,   // 0400..04FF; Cyrillic
            0x0500,   // 0500..052F; Cyrillic Supplement
            0x0530,   // 0530..058F; Armenian
            0x0590,   // 0590..05FF; Hebrew
            0x0600,   // 0600..06FF; Arabic
            0x0700,   // 0700..074F; Syriac
            0x0750,   // 0750..077F; Arabic Supplement
            0x0780,   // 0780..07BF; Thaana
            0x07C0,   // 07C0..07FF; NKo
            0x0800,   // 0800..083F; Samaritan
            0x0840,   // 0840..085F; Mandaic
            0x0860,   //             unassigned
            0x08A0,   // 08A0..08FF; Arabic Extended-A
            0x0900,   // 0900..097F; Devanagari
            0x0980,   // 0980..09FF; Bengali
            0x0A00,   // 0A00..0A7F; Gurmukhi
            0x0A80,   // 0A80..0AFF; Gujarati
            0x0B00,   // 0B00..0B7F; Oriya
            0x0B80,   // 0B80..0BFF; Tamil
            0x0C00,   // 0C00..0C7F; Telugu
            0x0C80,   // 0C80..0CFF; Kannada
            0x0D00,   // 0D00..0D7F; Malayalam
            0x0D80,   // 0D80..0DFF; Sinhala
            0x0E00,   // 0E00..0E7F; Thai
            0x0E80,   // 0E80..0EFF; Lao
            0x0F00,   // 0F00..0FFF; Tibetan
            0x1000,   // 1000..109F; Myanmar
            0x10A0,   // 10A0..10FF; Georgian
            0x1100,   // 1100..11FF; Hangul Jamo
            0x1200,   // 1200..137F; Ethiopic
            0x1380,   // 1380..139F; Ethiopic Supplement
            0x13A0,   // 13A0..13FF; Cherokee
            0x1400,   // 1400..167F; Unified Canadian Aboriginal Syllabics
            0x1680,   // 1680..169F; Ogham
            0x16A0,   // 16A0..16FF; Runic
            0x1700,   // 1700..171F; Tagalog
            0x1720,   // 1720..173F; Hanunoo
            0x1740,   // 1740..175F; Buhid
            0x1760,   // 1760..177F; Tagbanwa
            0x1780,   // 1780..17FF; Khmer
            0x1800,   // 1800..18AF; Mongolian
            0x18B0,   // 18B0..18FF; Unified Canadian Aboriginal Syllabics Extended
            0x1900,   // 1900..194F; Limbu
            0x1950,   // 1950..197F; Tai Le
            0x1980,   // 1980..19DF; New Tai Lue
            0x19E0,   // 19E0..19FF; Khmer Symbols
            0x1A00,   // 1A00..1A1F; Buginese
            0x1A20,   // 1A20..1AAF; Tai Tham
            0x1AB0,   //             unassigned
            0x1B00,   // 1B00..1B7F; Balinese
            0x1B80,   // 1B80..1BBF; Sundanese
            0x1BC0,   // 1BC0..1BFF; Batak
            0x1C00,   // 1C00..1C4F; Lepcha
            0x1C50,   // 1C50..1C7F; Ol Chiki
            0x1C80,   //             unassigned
            0x1CC0,   // 1CC0..1CCF; Sundanese Supplement
            0x1CD0,   // 1CD0..1CFF; Vedic Extensions
            0x1D00,   // 1D00..1D7F; Phonetic Extensions
            0x1D80,   // 1D80..1DBF; Phonetic Extensions Supplement
            0x1DC0,   // 1DC0..1DFF; Combining Diacritical Marks Supplement
            0x1E00,   // 1E00..1EFF; Latin Extended Additional
            0x1F00,   // 1F00..1FFF; Greek Extended
            0x2000,   // 2000..206F; General Punctuation
            0x2070,   // 2070..209F; Superscripts and Subscripts
            0x20A0,   // 20A0..20CF; Currency Symbols
            0x20D0,   // 20D0..20FF; Combining Diacritical Marks for Symbols
            0x2100,   // 2100..214F; Letterlike Symbols
            0x2150,   // 2150..218F; Number Forms
            0x2190,   // 2190..21FF; Arrows
            0x2200,   // 2200..22FF; Mathematical Operators
            0x2300,   // 2300..23FF; Miscellaneous Technical
            0x2400,   // 2400..243F; Control Pictures
            0x2440,   // 2440..245F; Optical Character Recognition
            0x2460,   // 2460..24FF; Enclosed Alphanumerics
            0x2500,   // 2500..257F; Box Drawing
            0x2580,   // 2580..259F; Block Elements
            0x25A0,   // 25A0..25FF; Geometric Shapes
            0x2600,   // 2600..26FF; Miscellaneous Symbols
            0x2700,   // 2700..27BF; Dingbats
            0x27C0,   // 27C0..27EF; Miscellaneous Mathematical Symbols-A
            0x27F0,   // 27F0..27FF; Supplemental Arrows-A
            0x2800,   // 2800..28FF; Braille Patterns
            0x2900,   // 2900..297F; Supplemental Arrows-B
            0x2980,   // 2980..29FF; Miscellaneous Mathematical Symbols-B
            0x2A00,   // 2A00..2AFF; Supplemental Mathematical Operators
            0x2B00,   // 2B00..2BFF; Miscellaneous Symbols and Arrows
            0x2C00,   // 2C00..2C5F; Glagolitic
            0x2C60,   // 2C60..2C7F; Latin Extended-C
            0x2C80,   // 2C80..2CFF; Coptic
            0x2D00,   // 2D00..2D2F; Georgian Supplement
            0x2D30,   // 2D30..2D7F; Tifinagh
            0x2D80,   // 2D80..2DDF; Ethiopic Extended
            0x2DE0,   // 2DE0..2DFF; Cyrillic Extended-A
            0x2E00,   // 2E00..2E7F; Supplemental Punctuation
            0x2E80,   // 2E80..2EFF; CJK Radicals Supplement
            0x2F00,   // 2F00..2FDF; Kangxi Radicals
            0x2FE0,   //             unassigned
            0x2FF0,   // 2FF0..2FFF; Ideographic Description Characters
            0x3000,   // 3000..303F; CJK Symbols and Punctuation
            0x3040,   // 3040..309F; Hiragana
            0x30A0,   // 30A0..30FF; Katakana
            0x3100,   // 3100..312F; Bopomofo
            0x3130,   // 3130..318F; Hangul Compatibility Jamo
            0x3190,   // 3190..319F; Kanbun
            0x31A0,   // 31A0..31BF; Bopomofo Extended
            0x31C0,   // 31C0..31EF; CJK Strokes
            0x31F0,   // 31F0..31FF; Katakana Phonetic Extensions
            0x3200,   // 3200..32FF; Enclosed CJK Letters and Months
            0x3300,   // 3300..33FF; CJK Compatibility
            0x3400,   // 3400..4DBF; CJK Unified Ideographs Extension A
            0x4DC0,   // 4DC0..4DFF; Yijing Hexagram Symbols
            0x4E00,   // 4E00..9FFF; CJK Unified Ideographs
            0xA000,   // A000..A48F; Yi Syllables
            0xA490,   // A490..A4CF; Yi Radicals
            0xA4D0,   // A4D0..A4FF; Lisu
            0xA500,   // A500..A63F; Vai
            0xA640,   // A640..A69F; Cyrillic Extended-B
            0xA6A0,   // A6A0..A6FF; Bamum
            0xA700,   // A700..A71F; Modifier Tone Letters
            0xA720,   // A720..A7FF; Latin Extended-D
            0xA800,   // A800..A82F; Syloti Nagri
            0xA830,   // A830..A83F; Common Indic Number Forms
            0xA840,   // A840..A87F; Phags-pa
            0xA880,   // A880..A8DF; Saurashtra
            0xA8E0,   // A8E0..A8FF; Devanagari Extended
            0xA900,   // A900..A92F; Kayah Li
            0xA930,   // A930..A95F; Rejang
            0xA960,   // A960..A97F; Hangul Jamo Extended-A
            0xA980,   // A980..A9DF; Javanese
            0xA9E0,   //             unassigned
            0xAA00,   // AA00..AA5F; Cham
            0xAA60,   // AA60..AA7F; Myanmar Extended-A
            0xAA80,   // AA80..AADF; Tai Viet
            0xAAE0,   // AAE0..AAFF; Meetei Mayek Extensions
            0xAB00,   // AB00..AB2F; Ethiopic Extended-A
            0xAB30,   //             unassigned
            0xABC0,   // ABC0..ABFF; Meetei Mayek
            0xAC00,   // AC00..D7AF; Hangul Syllables
            0xD7B0,   // D7B0..D7FF; Hangul Jamo Extended-B
            0xD800,   // D800..DB7F; High Surrogates
            0xDB80,   // DB80..DBFF; High Private Use Surrogates
            0xDC00,   // DC00..DFFF; Low Surrogates
            0xE000,   // E000..F8FF; Private Use Area
            0xF900,   // F900..FAFF; CJK Compatibility Ideographs
            0xFB00,   // FB00..FB4F; Alphabetic Presentation Forms
            0xFB50,   // FB50..FDFF; Arabic Presentation Forms-A
            0xFE00,   // FE00..FE0F; Variation Selectors
            0xFE10,   // FE10..FE1F; Vertical Forms
            0xFE20,   // FE20..FE2F; Combining Half Marks
            0xFE30,   // FE30..FE4F; CJK Compatibility Forms
            0xFE50,   // FE50..FE6F; Small Form Variants
            0xFE70,   // FE70..FEFF; Arabic Presentation Forms-B
            0xFF00,   // FF00..FFEF; Halfwidth and Fullwidth Forms
            0xFFF0,   // FFF0..FFFF; Specials
            0x10000,  // 10000..1007F; Linear B Syllabary
            0x10080,  // 10080..100FF; Linear B Ideograms
            0x10100,  // 10100..1013F; Aegean Numbers
            0x10140,  // 10140..1018F; Ancient Greek Numbers
            0x10190,  // 10190..101CF; Ancient Symbols
            0x101D0,  // 101D0..101FF; Phaistos Disc
            0x10200,  //               unassigned
            0x10280,  // 10280..1029F; Lycian
            0x102A0,  // 102A0..102DF; Carian
            0x102E0,  //               unassigned
            0x10300,  // 10300..1032F; Old Italic
            0x10330,  // 10330..1034F; Gothic
            0x10350,  //               unassigned
            0x10380,  // 10380..1039F; Ugaritic
            0x103A0,  // 103A0..103DF; Old Persian
            0x103E0,  //               unassigned
            0x10400,  // 10400..1044F; Deseret
            0x10450,  // 10450..1047F; Shavian
            0x10480,  // 10480..104AF; Osmanya
            0x104B0,  //               unassigned
            0x10800,  // 10800..1083F; Cypriot Syllabary
            0x10840,  // 10840..1085F; Imperial Aramaic
            0x10860,  //               unassigned
            0x10900,  // 10900..1091F; Phoenician
            0x10920,  // 10920..1093F; Lydian
            0x10940,  //               unassigned
            0x10980,  // 10980..1099F; Meroitic Hieroglyphs
            0x109A0,  // 109A0..109FF; Meroitic Cursive
            0x10A00,  // 10A00..10A5F; Kharoshthi
            0x10A60,  // 10A60..10A7F; Old South Arabian
            0x10A80,  //               unassigned
            0x10B00,  // 10B00..10B3F; Avestan
            0x10B40,  // 10B40..10B5F; Inscriptional Parthian
            0x10B60,  // 10B60..10B7F; Inscriptional Pahlavi
            0x10B80,  //               unassigned
            0x10C00,  // 10C00..10C4F; Old Turkic
            0x10C50,  //               unassigned
            0x10E60,  // 10E60..10E7F; Rumi Numeral Symbols
            0x10E80,  //               unassigned
            0x11000,  // 11000..1107F; Brahmi
            0x11080,  // 11080..110CF; Kaithi
            0x110D0,  // 110D0..110FF; Sora Sompeng
            0x11100,  // 11100..1114F; Chakma
            0x11150,  //               unassigned
            0x11180,  // 11180..111DF; Sharada
            0x111E0,  //               unassigned
            0x11680,  // 11680..116CF; Takri
            0x116D0,  //               unassigned
            0x12000,  // 12000..123FF; Cuneiform
            0x12400,  // 12400..1247F; Cuneiform Numbers and Punctuation
            0x12480,  //               unassigned
            0x13000,  // 13000..1342F; Egyptian Hieroglyphs
            0x13430,  //               unassigned
            0x16800,  // 16800..16A3F; Bamum Supplement
            0x16A40,  //               unassigned
            0x16F00,  // 16F00..16F9F; Miao
            0x16FA0,  //               unassigned
            0x1B000,  // 1B000..1B0FF; Kana Supplement
            0x1B100,  //               unassigned
            0x1D000,  // 1D000..1D0FF; Byzantine Musical Symbols
            0x1D100,  // 1D100..1D1FF; Musical Symbols
            0x1D200,  // 1D200..1D24F; Ancient Greek Musical Notation
            0x1D250,  //               unassigned
            0x1D300,  // 1D300..1D35F; Tai Xuan Jing Symbols
            0x1D360,  // 1D360..1D37F; Counting Rod Numerals
            0x1D380,  //               unassigned
            0x1D400,  // 1D400..1D7FF; Mathematical Alphanumeric Symbols
            0x1D800,  //               unassigned
            0x1EE00,  // 1EE00..1EEFF; Arabic Mathematical Alphabetic Symbols
            0x1EF00,  //               unassigned
            0x1F000,  // 1F000..1F02F; Mahjong Tiles
            0x1F030,  // 1F030..1F09F; Domino Tiles
            0x1F0A0,  // 1F0A0..1F0FF; Playing Cards
            0x1F100,  // 1F100..1F1FF; Enclosed Alphanumeric Supplement
            0x1F200,  // 1F200..1F2FF; Enclosed Ideographic Supplement
            0x1F300,  // 1F300..1F5FF; Miscellaneous Symbols And Pictographs
            0x1F600,  // 1F600..1F64F; Emoticons
            0x1F650,  //               unassigned
            0x1F680,  // 1F680..1F6FF; Transport And Map Symbols
            0x1F700,  // 1F700..1F77F; Alchemical Symbols
            0x1F780,  //               unassigned
            0x20000,  // 20000..2A6DF; CJK Unified Ideographs Extension B
            0x2A6E0,  //               unassigned
            0x2A700,  // 2A700..2B73F; CJK Unified Ideographs Extension C
            0x2B740,  // 2B740..2B81F; CJK Unified Ideographs Extension D
            0x2B820,  //               unassigned
            0x2F800,  // 2F800..2FA1F; CJK Compatibility Ideographs Supplement
            0x2FA20,  //               unassigned
            0xE0000,  // E0000..E007F; Tags
            0xE0080,  //               unassigned
            0xE0100,  // E0100..E01EF; Variation Selectors Supplement
            0xE01F0,  //               unassigned
            0xF0000,  // F0000..FFFFF; Supplementary Private Use Area-A
            0x100000  // 100000..10FFFF; Supplementary Private Use Area-B
        };

        static UnicodeBlock* const sBlocks[];
    };

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

    COM_PUBLIC static Integer Digit(
        /* [in] */ Char c,
        /* [in] */ Integer radix);

    COM_PUBLIC static Integer GetNumericValue(
        /* [in] */ Char c);

    COM_PUBLIC static Boolean IsSpaceChar(
        /* [in] */ Char c);

    COM_PUBLIC static Boolean IsWhitespace(
        /* [in] */ Char c);

    static Boolean IsISOControl(
        /* [in] */ Char c);

    COM_PUBLIC static Integer GetType(
        /* [in] */ Char c);

    COM_PUBLIC static Char ForDigit(
        /* [in] */ Integer digit,
        /* [in] */ Integer radix);

    COM_PUBLIC static Byte GetDirectionality(
        /* [in] */ Char c);

    static Boolean IsMirrored(
        /* [in] */ Char c);

    static Integer Compare(
        /* [in] */ Char x,
        /* [in] */ Char y);

    COM_PUBLIC static String GetName(
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

    static Integer DigitImpl(
        /* [in] */ Char c,
        /* [in] */ Integer radix);

    static Integer GetNumericValueImpl(
        /* [in] */ Char c);

    static Boolean IsSpaceCharImpl(
        /* [in] */ Char c);

    static Boolean IsWhitespaceImpl(
        /* [in] */ Char c);

    static Integer GetTypeImpl(
        /* [in] */ Char c);

    static Byte GetDirectionalityImpl(
        /* [in] */ Char c);

    static Boolean IsMirroredImpl(
        /* [in] */ Char c);

    static String GetNameImpl(
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
    return String::ValueOf(c);
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

inline Boolean Character::IsISOControl(
    /* [in] */ Char c)
{
    return c <= 0x9F &&
            (c >= 0x7F || (c >> 5 == 0));
}

inline Boolean Character::IsMirrored(
    /* [in] */ Char c)
{
    return IsMirroredImpl(c);
}

inline Integer Character::Compare(
    /* [in] */ Char x,
    /* [in] */ Char y)
{
    return x - y;
}

}
}

#endif // __COMO_CORE_CHARACTER_H__
