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

#ifndef __CCM_TOKENIZER_H__
#define __CCM_TOKENIZER_H__

#include "../util/File.h"
#include "../util/HashMap.h"
#include "../util/StringBuilder.h"

namespace ccm {

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
        DOUBLE,                 // 4)
        FLOAT,                  // 5)
        HANDLE,                 // 6)
        INTEGER,                // 7)
        LONG,                   // 8)
        SHORT,                  // 9)
        STRING,                 // 10)
        // other keyword
        ARRAY,                  // 11)
        CALLEE,                 // 12)
        COCLASS,                // 13)
        CONST,                  // 14)
        CONSTRUCTOR,            // 15)
        DESCRIPTION,            // 16)
        ENUM,                   // 17)
        FALSE,                  // 18)
        IN,                     // 19)
        INCLUDE,                // 20)
        INTERFACE,              // 21)
        MODULE,                 // 22)
        NAMESPACE,              // 23)
        OUT,                    // 24)
        TRUE,                   // 25)
        URL,                    // 26)
        UUID,                   // 27)
        VERSION,                // 28)
        // symbol
        AND,                    // 29)  '&'
        ANGLE_BRACKETS_OPEN,    // 30)  '<'
        ANGLE_BRACKETS_CLOSE,   // 31)  '>'
        ASSIGNMENT,             // 32)  '='
        ASTERISK,               // 33)  '*'
        BRACES_OPEN,            // 34)  '{'
        BRACES_CLOSE,           // 35)  '}'
        BRACKETS_OPEN,          // 36)  '['
        BRACKETS_CLOSE,         // 37)  ']'
        COLON,                  // 38)  ':'
        COMMA,                  // 39)  ','
        COMPLIMENT,             // 40)  '~'
        DIVIDE,                 // 41)  '/'
        END_OF_LINE,            // 42)  '\n'
        EXCLUSIVE_OR,           // 43)  '^'
        INCLUSIVE_OR,           // 44)  '|'
        MINUS,                  // 45)  '-'
        MODULO,                 // 46)  '%'
        NOT,                    // 47)  '!'
        PARENTHESES_OPEN,       // 48)  '('
        PARENTHESES_CLOSE,      // 49)  ')'
        PERIOD,                 // 50)  '.'
        PLUS,                   // 51)  '+'
        SEMICOLON,              // 52)  ';'
        // other
        CHARACTER,              //
        COMMENT_BLOCK,          // 52)
        COMMENT_LINE,           // 53)
        END_OF_FILE,            // 54)
        IDENTIFIER,             // 55)
        NUMBER_INTEGRAL,        // 56)
        NUMBER_FLOATINGPOINT,   // 57)
        SHIFT_LEFT,             // 58)  "<<"
        SHIFT_RIGHT,            // 59)  ">>"
        SHIFT_RIGHT_UNSIGNED,   // 60)  ">>>"
        STRING_LITERAL,         // 61)
        UUID_NUMBER,            // 62)
        VERSION_NUMBER,         // 63)
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
    HashMap<Token> mKeywords;
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
};

}

#endif //__CCM_TOKENIZER_H__
