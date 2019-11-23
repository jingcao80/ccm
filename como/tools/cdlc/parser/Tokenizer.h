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

#include "parser/TokenInfo.h"
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

    inline AutoPtr<Reader> GetReader();

    inline void SetReader(
        /* [in] */ Reader* reader);

    TokenInfo PeekToken(
        /* [in] */ Token expectedToken = Token::UNKNOWN);

    TokenInfo GetToken(
        /* [in] */ Token expectedToken = Token::UNKNOWN);

    void SkipCurrentLine();

    inline TokenInfo GetUuidNumberToken();

    inline TokenInfo GetVersionNumberToken();

private:
    void SetupKeywords();

    TokenInfo ReadToken(
        /* [in] */ Token expectedToken);

    TokenInfo ReadUuidNumberToken();

    TokenInfo ReadVersionNumberToken();

    TokenInfo ReadIdentifier(
        /* [in] */ char c);

    TokenInfo ReadNumber(
        /* [in] */ char c);

    TokenInfo ReadCharacter(
        /* [in] */ char c);

    TokenInfo ReadStringLiteral(
        /* [in] */ char c);

    TokenInfo ReadLineComment(
        /* [in] */ char c);

    TokenInfo ReadBlockComment(
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
    TokenInfo mCurrentTokenInfo;
    bool mPeeked = false;
    Token mExpectedToken = Token::UNKNOWN;
};

Tokenizer::Tokenizer()
{
    SetupKeywords();
}

AutoPtr<Reader> Tokenizer::GetReader()
{
    return mReader;
}

void Tokenizer::SetReader(
    /* [in] */ Reader* reader)
{
    mReader = reader;
    mPeeked = false;
}

TokenInfo Tokenizer::GetUuidNumberToken()
{
    return ReadUuidNumberToken();
}

TokenInfo Tokenizer::GetVersionNumberToken()
{
    return ReadVersionNumberToken();
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
