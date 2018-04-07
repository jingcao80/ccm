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
        DOUBLE,                 // 5)
        FLOAT,                  // 6)
        HANDLE,                 // 7)
        INTEGER,                // 8)
        INTERFACEID,            // 9)
        LONG,                   // 10)
        SHORT,                  // 11)
        STRING,                 // 12)
        // other keyword
        ARRAY,                  // 13)
        CALLEE,                 // 14)
        COCLASS,                // 15)
        CONST,                  // 16)
        CONSTRUCTOR,            // 17)
        DESCRIPTION,            // 18)
        ENUM,                   // 19)
        FALSE,                  // 20)
        IN,                     // 21)
        INCLUDE,                // 22)
        INTERFACE,              // 23)
        MODULE,                 // 24)
        NAMESPACE,              // 25)
        OUT,                    // 26)
        TRUE,                   // 27)
        URL,                    // 28)
        UUID,                   // 29)
        VERSION,                // 30)
        // symbol
        AND,                    // 31)  '&'
        ANGLE_BRACKETS_OPEN,    // 32)  '<'
        ANGLE_BRACKETS_CLOSE,   // 33)  '>'
        ASSIGNMENT,             // 34)  '='
        ASTERISK,               // 35)  '*'
        BRACES_OPEN,            // 36)  '{'
        BRACES_CLOSE,           // 37)  '}'
        BRACKETS_OPEN,          // 38)  '['
        BRACKETS_CLOSE,         // 39)  ']'
        COLON,                  // 40)  ':'
        COMMA,                  // 41)  ','
        COMPLIMENT,             // 42)  '~'
        DIVIDE,                 // 43)  '/'
        END_OF_LINE,            // 44)  '\n'
        EXCLUSIVE_OR,           // 45)  '^'
        INCLUSIVE_OR,           // 46)  '|'
        MINUS,                  // 47)  '-'
        MODULO,                 // 48)  '%'
        NOT,                    // 49)  '!'
        PARENTHESES_OPEN,       // 50)  '('
        PARENTHESES_CLOSE,      // 51)  ')'
        PERIOD,                 // 52)  '.'
        PLUS,                   // 53)  '+'
        SEMICOLON,              // 54)  ';'
        // other
        CHARACTER,              // 55)
        COMMENT_BLOCK,          // 56)
        COMMENT_LINE,           // 57)
        END_OF_FILE,            // 58)
        IDENTIFIER,             // 59)
        NUMBER_INTEGRAL,        // 60)
        NUMBER_FLOATINGPOINT,   // 61)
        SHIFT_LEFT,             // 62)  "<<"
        SHIFT_RIGHT,            // 63)  ">>"
        SHIFT_RIGHT_UNSIGNED,   // 64)  ">>>"
        STRING_LITERAL,         // 65)
        UUID_NUMBER,            // 66)
        VERSION_NUMBER,         // 67)
    };

private:
    class FileNode
    {
    public:
        FileNode()
            : mFile(nullptr)
            , mNext(nullptr)
        {}

        ~FileNode()
        {
            if (mFile != nullptr) {
                delete mFile;
                mFile = nullptr;
            }
            mNext = nullptr;
        }

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

    inline int GetCharacter()
    { return mCharacter; }

    inline String GetIdentifier()
    { return mIdentifier; }

    inline String GetNumberString()
    { return mNumberString; }

    inline bool Is64Bit()
    { return mBit == 64; }

    inline int GetRadix()
    { return mRadix; }

    inline long long int GetIntegralValue()
    { return mIntegralValue; }

    inline double GetFloatingPointValue()
    { return mFloatingPointValue; }

    inline String GetString()
    { return mString; }

    inline int GetTokenColumnNo()
    { return mTokenColumnNo; }

    inline int GetTokenLineNo()
    { return mTokenLineNo; }

    void SkipCurrentLine();

    static inline bool IsPrimitiveType(
        /* [in] */ Token token)
    { return Token::BOOLEAN <= token && token <= Token::STRING; }

    static inline bool IsKeyword(
        /* [in] */ Token token)
    { return Token::BOOLEAN <= token && token <= Token::VERSION; }

    inline File* GetCurrentFile()
    { return mFile; }

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
};

}

#endif //__CCDL_TOKENIZER_H__
