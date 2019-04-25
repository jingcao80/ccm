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

#ifndef __CCDL_TOKENIZER_H__
#define __CCDL_TOKENIZER_H__

#include "../util/File.h"
#include "../util/StringMap.h"
#include "../util/StringBuilder.h"

namespace ccdl {

class Tokenizer
{
public:
    enum class Token
    {
        ILLEGAL_TOKEN = 0,      // 0)
        // primitive type
        BOOLEAN,                // 1)
        BYTE,                   // 2)
        CHAR,                   // 3)
        COCLASSID,              // 4)
        COMPONENTID,            // 5)
        DOUBLE,                 // 6)
        ECODE,                  // 7)
        FLOAT,                  // 8)
        HANDLE,                 // 9)
        INTEGER,                // 10)
        INTERFACEID,            // 11)
        LONG,                   // 12)
        SHORT,                  // 13)
        STRING,                 // 14)
        TRIPLE,                 // 15)
        // other keyword
        ARRAY,                  // 16)
        CALLEE,                 // 17)
        COCLASS,                // 18)
        CONST,                  // 19)
        CONSTRUCTOR,            // 20)
        DELETE,                 // 21)
        DESCRIPTION,            // 22)
        ENUM,                   // 23)
        FALSE,                  // 24)
        IN,                     // 25)
        INCLUDE,                // 26)
        INTERFACE,              // 27)
        MODULE,                 // 28)
        NAMESPACE,              // 29)
        NULLPTR,                // 30)
        OUT,                    // 31)
        TRUE,                   // 32)
        URL,                    // 33)
        UUID,                   // 34)
        VERSION,                // 35)
        // symbol
        AND,                    // 36)  '&'
        ANGLE_BRACKETS_OPEN,    // 37)  '<'
        ANGLE_BRACKETS_CLOSE,   // 38)  '>'
        ASSIGNMENT,             // 39)  '='
        ASTERISK,               // 40)  '*'
        BRACES_OPEN,            // 41)  '{'
        BRACES_CLOSE,           // 42)  '}'
        BRACKETS_OPEN,          // 43)  '['
        BRACKETS_CLOSE,         // 44)  ']'
        COLON,                  // 45)  ':'
        COMMA,                  // 46)  ','
        COMPLIMENT,             // 47)  '~'
        DIVIDE,                 // 48)  '/'
        END_OF_LINE,            // 49)  '\n'
        EXCLUSIVE_OR,           // 50)  '^'
        INCLUSIVE_OR,           // 51)  '|'
        MINUS,                  // 52)  '-'
        MODULO,                 // 53)  '%'
        NOT,                    // 54)  '!'
        PARENTHESES_OPEN,       // 55)  '('
        PARENTHESES_CLOSE,      // 56)  ')'
        PERIOD,                 // 57)  '.'
        PLUS,                   // 58)  '+'
        SEMICOLON,              // 59)  ';'
        // other
        CHARACTER,              // 60)
        COMMENT_BLOCK,          // 61)
        COMMENT_LINE,           // 62)
        END_OF_FILE,            // 63)
        IDENTIFIER,             // 64)
        NUMBER_INTEGRAL,        // 65)
        NUMBER_FLOATINGPOINT,   // 66)
        SHIFT_LEFT,             // 67)  "<<"
        SHIFT_RIGHT,            // 68)  ">>"
        SHIFT_RIGHT_UNSIGNED,   // 69)  ">>>"
        STRING_LITERAL,         // 70)
        UUID_NUMBER,            // 71)
        VERSION_NUMBER,         // 72)
    };

private:
    class FileNode
    {
    public:
        FileNode();

        ~FileNode();

    public:
        File* mFile;
        FileNode* mNext;
    };

public:
    Tokenizer();

    ~Tokenizer();

    bool PushInputFile(
        /* [in] */ const String& file);

    void PopInputFileAndRemove();

    Token PeekToken();

    Token GetToken();

    Token GetUuidNumberToken();

    Token GetVersionNumberToken();

    inline int GetCharacter();

    inline String GetIdentifier();

    inline String GetNumberString();

    inline bool Is64Bit();

    inline int GetRadix();

    inline bool IsScientificNotation();

    inline long long int GetIntegralValue();

    inline double GetFloatingPointValue();

    inline String GetString();

    inline int GetTokenColumnNo();

    inline int GetTokenLineNo();

    void SkipCurrentLine();

    static bool IsPrimitiveType(
        /* [in] */ Token token);

    static bool IsKeyword(
        /* [in] */ Token token);

    inline File* GetCurrentFile();

    const char* DumpToken(
        /* [in] */ Token token);

private:
    void InitializeKeyword();

    Token ReadToken();

    Token ReadUuidNumberToken();

    Token ReadVersionNumberToken();

    Token ReadCharacter(
        /* [in] */ int c);

    Token ReadIdentifier(
        /* [in] */ int c);

    Token ReadNumber(
        /* [in] */ int c);

    Token ReadStringLiteral(
        /* [in] */ int c);

    Token ReadLineComment(
        /* [in] */ int c);

    Token ReadBlockComment(
        /* [in] */ int c);

    bool IsAlphabet(
        /* [in] */ int c);

    bool IsOctalDigital(
        /* [in] */ int c);

    bool IsDecimalDigital(
        /* [in] */ int c);

    bool IsHexDigital(
        /* [in] */ int c);

    bool IsEscape(
        /* [in] */ int c);

private:
    static const String TAG;
    File* mFile;
    FileNode* mFileStack;
    StringMap<Token> mKeywords;
    Token mCurrToken;
    bool mHasAPeek;
    int mTokenLineNo;
    int mTokenColumnNo;
    int mCharacter;
    String mIdentifier;
    String mComment;
    String mNumberString;
    String mString;
    long long int mIntegralValue;
    double mFloatingPointValue;
    int mBit;
    int mRadix;
    bool mScientificNotation;
};

int Tokenizer::GetCharacter()
{
    return mCharacter;
}

String Tokenizer::GetIdentifier()
{
    return mIdentifier;
}

String Tokenizer::GetNumberString()
{
    return mNumberString;
}

bool Tokenizer::Is64Bit()
{
    return mBit == 64;
}

int Tokenizer::GetRadix()
{
    return mRadix;
}

bool Tokenizer::IsScientificNotation()
{
    return mScientificNotation;
}

long long int Tokenizer::GetIntegralValue()
{
    return mIntegralValue;
}

double Tokenizer::GetFloatingPointValue()
{
    return mFloatingPointValue;
}

String Tokenizer::GetString()
{
    return mString;
}

int Tokenizer::GetTokenColumnNo()
{
    return mTokenColumnNo;
}

int Tokenizer::GetTokenLineNo()
{
    return mTokenLineNo;
}

File* Tokenizer::GetCurrentFile()
{
    return mFile;
}

}

#endif //__CCDL_TOKENIZER_H__
