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

#include "util/File.h"
#include "util/HashMap.h"
#include "util/StringBuilder.h"

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
        DESCRIPTION,            // 15)
        ENUM,                   // 16)
        FALSE,
        IN,                     // 17)
        INCLUDE,                // 18)
        INTERFACE,              // 19)
        MODULE,                 // 20)
        NAMESPACE,              // 21)
        OUT,                    // 22)
        TRUE,
        UUID,                   // 23)
        VERSION,                // 24)
        // symbol
        AND,                    //      '&'
        ANGLE_BRACKETS_OPEN,    // 25)  '<'
        ANGLE_BRACKETS_CLOSE,   // 26)  '>'
        ASSIGNMENT,             // 27)  '='
        ASTERISK,               // 28)  '*'
        BRACES_OPEN,            // 29)  '{'
        BRACES_CLOSE,           // 30)  '}'
        BRACKETS_OPEN,          // 31)  '['
        BRACKETS_CLOSE,         // 32)  ']'
        COLON,                  // 33)  ':'
        COMMA,                  // 34)  ','
        COMMENT_BLOCK,          // 35)
        COMMENT_LINE,           // 36)
        COMPLIMENT,             //      '~'
        DIVIDE,                 // 37)  '/'
        DOUBLE_QUOTES,          // 38)  '"'
        END_OF_FILE,            // 39)
        END_OF_LINE,            // 40)  '\n'
        EXCLUSIVE_OR,           //      '^'
        INCLUSIVE_OR,           // 43)  '|'
        MINUS,                  //      '-'
        MODULO,                 //      '%'
        NOT,                    //      '!'
        PARENTHESES_OPEN,       // 44)  '('
        PARENTHESES_CLOSE,      // 45)  ')'
        PERIOD,                 // 46)  '.'
        PLUS,                   //        '+'
        SEMICOLON,              // 47)  ';'
        SHIFT_LEFT,
        SHIFT_RIGHT,
        SHIFT_RIGHT_UNSIGNED,
        STRING_LITERAL,         // 48)
        UUID_NUMBER,            // 49)
        VERSION_NUMBER,         // 50)
        // other
        CHARACTER,
        IDENTIFIER,             // 42)
        NUMBER_INTEGER,         // 43)
        NUMBER_FLOAT,
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
    Token GetStringLiteralToken();
    Token GetUuidNumberToken();
    Token GetVersionNumberToken();

    inline String GetIdentifier() { return mIdentifier; }
    inline String GetNumberString() { return mNumberString; }
    inline String GetString() { return mString; }
    inline int GetTokenColumnNo() { return mTokenColumnNo; }
    inline int GetTokenLineNo() { return mTokenLineNo; }

    void SkipCurrentLine();

    static inline bool IsPrimitiveType(
        /* [in] */ Token token) { return Token::BOOLEAN <= token && token <= Token::STRING; }
    static inline bool IsKeyword(
        /* [in] */ Token token) { return Token::BOOLEAN <= token && token <= Token::VERSION; }

    inline File* GetCurrentFile() { return mFile; }

    const char* DumpToken(
        /* [in] */ Token token);

private:
    void InitializeKeyword();

    Token ReadToken();
    Token ReadStringLiteralToken();
    Token ReadUuidNumberToken();
    Token ReadVersionNumberToken();

    Token ReadIdentifier(
        /* [in] */ int c);

    Token ReadNumber(
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
    String mIdentifier;
    String mComment;
    String mNumberString;
    String mString;
    long long mNumber;
    int mBit;
};

}

#endif //__CCM_TOKENIZER_H__
