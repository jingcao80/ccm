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
        FLOAT,                  // 7)
        HANDLE,                 // 8)
        INTEGER,                // 9)
        INTERFACEID,            // 10)
        LONG,                   // 11)
        SHORT,                  // 12)
        STRING,                 // 13)
        // other keyword
        ARRAY,                  // 14)
        CALLEE,                 // 15)
        COCLASS,                // 16)
        CONST,                  // 17)
        CONSTRUCTOR,            // 18)
        DESCRIPTION,            // 19)
        ENUM,                   // 20)
        FALSE,                  // 21)
        IN,                     // 22)
        INCLUDE,                // 23)
        INTERFACE,              // 24)
        MODULE,                 // 25)
        NAMESPACE,              // 26)
        OUT,                    // 27)
        TRUE,                   // 28)
        URL,                    // 29)
        UUID,                   // 30)
        VERSION,                // 31)
        // symbol
        AND,                    // 32)  '&'
        ANGLE_BRACKETS_OPEN,    // 33)  '<'
        ANGLE_BRACKETS_CLOSE,   // 34)  '>'
        ASSIGNMENT,             // 35)  '='
        ASTERISK,               // 36)  '*'
        BRACES_OPEN,            // 37)  '{'
        BRACES_CLOSE,           // 38)  '}'
        BRACKETS_OPEN,          // 39)  '['
        BRACKETS_CLOSE,         // 40)  ']'
        COLON,                  // 41)  ':'
        COMMA,                  // 42)  ','
        COMPLIMENT,             // 43)  '~'
        DIVIDE,                 // 44)  '/'
        END_OF_LINE,            // 45)  '\n'
        EXCLUSIVE_OR,           // 46)  '^'
        INCLUSIVE_OR,           // 47)  '|'
        MINUS,                  // 48)  '-'
        MODULO,                 // 40)  '%'
        NOT,                    // 50)  '!'
        PARENTHESES_OPEN,       // 51)  '('
        PARENTHESES_CLOSE,      // 52)  ')'
        PERIOD,                 // 53)  '.'
        PLUS,                   // 54)  '+'
        SEMICOLON,              // 55)  ';'
        // other
        CHARACTER,              // 56)
        COMMENT_BLOCK,          // 57)
        COMMENT_LINE,           // 58)
        END_OF_FILE,            // 59)
        IDENTIFIER,             // 60)
        NUMBER_INTEGRAL,        // 61)
        NUMBER_FLOATINGPOINT,   // 62)
        SHIFT_LEFT,             // 63)  "<<"
        SHIFT_RIGHT,            // 64)  ">>"
        SHIFT_RIGHT_UNSIGNED,   // 65)  ">>>"
        STRING_LITERAL,         // 66)
        UUID_NUMBER,            // 67)
        VERSION_NUMBER,         // 68)
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
