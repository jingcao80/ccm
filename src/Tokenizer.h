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
#include <memory>

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
        IN,                     // 17)
        INCLUDE,                // 18)
        INTERFACE,              // 19)
        MODULE,                 // 20)
        NAMESPACE,              // 21)
        OUT,                    // 22)
        UUID,                   // 23)
        VERSION,                // 24)
        // symbol
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
        DIVIDE,                 // 37)  '/'
        DOUBLE_QUOTES,          // 38)  '"'
        END_OF_FILE,            // 39)
        HYPHEN,                 // 40)  '-'
        IDENTIFIER,             // 41)
        NUMBER,                 // 42)
        PARENTHESES_OPEN,       // 43)  '('
        PARENTHESES_CLOSE,      // 44)  ')'
        PERIOD,                 // 45)  '.'
        SEMICOLON,              // 46)  ';'
        STRING_LITERAL,         // 47)
        UUID_NUMBER,            // 48)
        VERSION_NUMBER,         // 49)
    };

public:
    Tokenizer();

    inline void SetInputFile(
        /* [in] */ const std::shared_ptr<File>& file) { mFile = file; }

    Token GetToken();
    Token GetStringLiteralToken();
    Token GetUuidNumberToken();
    Token GetVersionNumberToken();
    inline void UngetToken(
        /* [in] */ Token token) { mPrevToken = token; mHasPrevToken = true; }

    inline String GetIdentifier() { return mIdentifier; }
    inline String GetNumberString() { return mNumberString; }
    inline String GetString() { return mString; }
    inline int GetTokenColumnNo() { return mTokenColumnNo; }
    inline int GetTokenLineNo() { return mTokenLineNo; }

    static inline bool IsPrimitiveType(
        /* [in] */ Token token) { return Token::BOOLEAN <= token && token <= Token::STRING; }
    static inline bool IsKeyword(
        /* [in] */ Token token) { return Token::BOOLEAN <= token && token <= Token::VERSION; }

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
    std::shared_ptr<File> mFile;
    HashMap<Token> mKeywords;
    Token mCurrToken;
    Token mPrevToken;
    bool mHasPrevToken;
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
