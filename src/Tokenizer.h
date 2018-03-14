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
        ANGLE_BRACKETS_OPEN,    // 1)   '<'
        ANGLE_BRACKETS_CLOSE,   // 2)   '>'
        ARRAY,                  // 3)
        ASSIGNMENT,             // 4)   '='
        ASTERISK,               // 5)   '*'
        BOOLEAN,                // 6)
        BRACES_OPEN,            // 7)   '{'
        BRACES_CLOSE,           // 8)   '}'
        BRACKETS_OPEN,          // 9)   '['
        BRACKETS_CLOSE,         // 10)  ']'
        BYTE,                   // 11)
        CALLEE,                 // 12)
        COCLASS,                // 13)
        COLON,                  // 14)  ':'
        COMMA,                  // 15)  ','
        COMMENT_BLOCK,          // 16)
        COMMENT_LINE,           // 17)
        CONST,                  // 18)
        DESCRIPTION,            // 19)
        DIVIDE,                 // 20)  '/'
        DOUBLE_QUOTES,          // 21)  '"'
        END_OF_FILE,            // 22)
        ENUM,                   // 23)
        HANDLE,                 // 24)
        HYPHEN,                 // 25)  '-'
        IDENTIFIER,             // 26)
        IN,                     // 27)
        INCLUDE,                // 28)
        INTEGER,                // 29)
        INTERFACE,              // 30)
        LONG,                   // 31)
        MODULE,                 // 32)
        NAMESPACE,              // 33)
        NUMBER,                 // 34)
        OUT,                    // 35)
        PARENTHESES_OPEN,       // 36)  '('
        PARENTHESES_CLOSE,      // 37)  ')'
        PERIOD,                 // 38)  '.'
        SEMICOLON,              // 39)  ';'
        SHORT,                  // 40)
        STRING,                 // 41)
        STRING_LITERAL,         // 42)
        UUID,                   // 43)
        UUID_NUMBER,            // 44)
        VERSION,                // 45)
        VERSION_NUMBER,         // 46)
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
