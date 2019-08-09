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

#ifndef __CDLC_TOKENIZER_H__
#define __CDLC_TOKENIZER_H__

#include "parser/Token.h"
#include "util/AutoPtr.h"
#include "util/Reader.h"
#include "util/String.h"
#include <cctype>
#include <unordered_map>

namespace cdlc {

class Tokenizer
{
public:
    inline Tokenizer();

    inline void SetReader(
        /* [in] */ Reader* reader);

    Token PeekToken(
        /* [in] */ Token expectedToken = Token::UNKNOW);

    Token GetToken(
        /* [in] */ Token expectedToken = Token::UNKNOW);

    inline Token GetUuidNumberToken();

    inline Token GetVersionNumberToken();

    inline String GetUuid();

    inline String GetVersionNumber();

    inline String GetTokenFilePath();

    inline int GetTokenLineNo();

    inline int GetTokenColumnNo();

    String DumpCurrentToken();

private:
    void SetupKeywords();

    Token ReadToken(
        /* [in] */ Token expectedToken);

    Token ReadUuidNumberToken();

    Token ReadVersionNumberToken();

    Token ReadIdentifier(
        /* [in] */ char c);

    Token ReadNumber(
        /* [in] */ char c);

    Token ReadCharacter(
        /* [in] */ char c);

    Token ReadStringLiteral(
        /* [in] */ char c);

    Token ReadLineComment(
        /* [in] */ char c);

    Token ReadBlockComment(
        /* [in] */ char c);

    inline static bool IsAlphabet(
        /* [in] */ char c);

    inline static bool IsOctalDigital(
        /* [in] */ char c);

    inline static bool IsDecimalDigital(
        /* [in] */ char c);

    inline static bool IsHexDigital(
        /* [in] */ char c);

    inline static bool IsSpace(
        /* [in] */ char c);

private:
    AutoPtr<Reader> mReader;
    std::unordered_map<String, Token, StringHashFunc, StringEqualsFunc> mKeywords;
    Token mCurrentToken;
    int mTokenLineNo = 0;
    int mTokenColumnNo = 0;
    String mIdentifier;
    String mNumberString;
    int mCharacter = 0;
    String mString;
    String mComment;
    long long int mIntegralValue = 0;
    double mFloatingPointValue = 0;
    int mBit = 0;
    int mRadix = 0;
    bool mScientificNotation = false;
};

Tokenizer::Tokenizer()
{
    SetupKeywords();
}

void Tokenizer::SetReader(
    /* [in] */ Reader* reader)
{
    mReader = reader;
}

Token Tokenizer::GetUuidNumberToken()
{
    return ReadUuidNumberToken();
}

Token Tokenizer::GetVersionNumberToken()
{
    return ReadVersionNumberToken();
}

String Tokenizer::GetUuid()
{
    return mString;
}

String Tokenizer::GetVersionNumber()
{
    return mString;
}

String Tokenizer::GetTokenFilePath()
{
    return mReader->GetCurrentFilePath();
}

int Tokenizer::GetTokenLineNo()
{
    return mTokenLineNo;
}

int Tokenizer::GetTokenColumnNo()
{
    return mTokenColumnNo;
}

bool Tokenizer::IsAlphabet(
    /* [in] */ char c)
{
    return isalpha(c);
}

bool Tokenizer::IsOctalDigital(
    /* [in] */ char c)
{
    return ('0' <= c && c <= '7');
}

bool Tokenizer::IsDecimalDigital(
    /* [in] */ char c)
{
    return isdigit(c);
}

bool Tokenizer::IsHexDigital(
    /* [in] */ char c)
{
    return isxdigit(c);
}

bool Tokenizer::IsSpace(
    /* [in] */ char c)
{
    return isspace(c);
}

}

#endif // __CDLC_TOKENIZER_H__
