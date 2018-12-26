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
        // other keyword
        ARRAY,                  // 15)
        CALLEE,                 // 16)
        COCLASS,                // 17)
        CONST,                  // 18)
        CONSTRUCTOR,            // 19)
        DELETE,                 // 20)
        DESCRIPTION,            // 21)
        ENUM,                   // 22)
        FALSE,                  // 23)
        IN,                     // 24)
        INCLUDE,                // 25)
        INTERFACE,              // 26)
        MODULE,                 // 27)
        NAMESPACE,              // 28)
        NULLPTR,                // 29)
        OUT,                    // 30)
        TRUE,                   // 31)
        URL,                    // 32)
        UUID,                   // 33)
        VERSION,                // 34)
        // symbol
        AND,                    // 35)  '&'
        ANGLE_BRACKETS_OPEN,    // 36)  '<'
        ANGLE_BRACKETS_CLOSE,   // 37)  '>'
        ASSIGNMENT,             // 38)  '='
        ASTERISK,               // 39)  '*'
        BRACES_OPEN,            // 40)  '{'
        BRACES_CLOSE,           // 41)  '}'
        BRACKETS_OPEN,          // 42)  '['
        BRACKETS_CLOSE,         // 43)  ']'
        COLON,                  // 44)  ':'
        COMMA,                  // 45)  ','
        COMPLIMENT,             // 46)  '~'
        DIVIDE,                 // 47)  '/'
        END_OF_LINE,            // 48)  '\n'
        EXCLUSIVE_OR,           // 49)  '^'
        INCLUSIVE_OR,           // 50)  '|'
        MINUS,                  // 51)  '-'
        MODULO,                 // 52)  '%'
        NOT,                    // 53)  '!'
        PARENTHESES_OPEN,       // 54)  '('
        PARENTHESES_CLOSE,      // 55)  ')'
        PERIOD,                 // 56)  '.'
        PLUS,                   // 57)  '+'
        SEMICOLON,              // 58)  ';'
        // other
        CHARACTER,              // 59)
        COMMENT_BLOCK,          // 60)
        COMMENT_LINE,           // 61)
        END_OF_FILE,            // 62)
        IDENTIFIER,             // 63)
        NUMBER_INTEGRAL,        // 64)
        NUMBER_FLOATINGPOINT,   // 65)
        SHIFT_LEFT,             // 66)  "<<"
        SHIFT_RIGHT,            // 67)  ">>"
        SHIFT_RIGHT_UNSIGNED,   // 68)  ">>>"
        STRING_LITERAL,         // 69)
        UUID_NUMBER,            // 70)
        VERSION_NUMBER,         // 71)
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
