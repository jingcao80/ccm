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

#include "parser/Tokenizer.h"
#include "util/StringBuilder.h"

namespace cdlc {

void Tokenizer::SetupKeywords()
{
    mKeywords["Array"] = Token::ARRAY;
    mKeywords["Boolean"] = Token::BOOLEAN;
    mKeywords["Byte"] = Token::BYTE;
    mKeywords["callee"] = Token::CALLEE;
    mKeywords["Char"] = Token::CHAR;
    mKeywords["coclass"] = Token::COCLASS;
    mKeywords["CoclassID"] = Token::COCLASSID;
    mKeywords["ComponentID"] = Token::COMPONENTID;
    mKeywords["const"] = Token::CONST;
    mKeywords["Constructor"] = Token::CONSTRUCTOR;
    mKeywords["delete"] = Token::DELETE;
    mKeywords["description"] = Token::DESCRIPTION;
    mKeywords["Double"] = Token::DOUBLE;
    mKeywords["ECode"] = Token::ECODE;
    mKeywords["enum"] = Token::ENUM;
    mKeywords["false"] = Token::FALSE;
    mKeywords["Float"] = Token::FLOAT;
    mKeywords["HANDLE"] = Token::HANDLE;
    mKeywords["in"] = Token::IN;
    mKeywords["include"] = Token::INCLUDE;
    mKeywords["Integer"] = Token::INTEGER;
    mKeywords["interface"] = Token::INTERFACE;
    mKeywords["InterfaceID"] = Token::INTERFACEID;
    mKeywords["Long"] = Token::LONG;
    mKeywords["module"] = Token::MODULE;
    mKeywords["namespace"] = Token::NAMESPACE;
    mKeywords["nullptr"] = Token::NULLPTR;
    mKeywords["out"] = Token::OUT;
    mKeywords["Short"] = Token::SHORT;
    mKeywords["String"] = Token::STRING;
    mKeywords["Triple"] = Token::TRIPLE;
    mKeywords["true"] = Token::TRUE;
    mKeywords["uri"] = Token::URI;
    mKeywords["uuid"] = Token::UUID;
    mKeywords["version"] = Token::VERSION;
}

TokenInfo Tokenizer::PeekToken(
    /* [in] */ Token expectedToken)
{
    if (mPeeked) {
        if (mExpectedToken == expectedToken) {
            return mCurrentTokenInfo;
        }
        mReader->Reset();
    }

    mReader->Mark();
    mPeeked = true;
    mExpectedToken = expectedToken;
    return ReadToken(expectedToken);
}

TokenInfo Tokenizer::GetToken(
    /* [in] */ Token expectedToken)
{
    if (mPeeked) {
        mPeeked = false;
        if (mExpectedToken == expectedToken) {
            return mCurrentTokenInfo;
        }
        mReader->Reset();
    }

    return ReadToken(expectedToken);
}

TokenInfo Tokenizer::ReadToken(
    /* [in] */ Token expectedToken)
{
    while (!mReader->IsEof() && IsSpace(mReader->PeekChar())) {
        mReader->GetChar();
    }

    int lineNo = mReader->GetCurrentLineNumber();
    int columnNo = mReader->GetCurrentColumnNumber();

    while (!mReader->IsEof()) {
        lineNo = mReader->GetCurrentLineNumber();
        columnNo = mReader->GetCurrentColumnNumber();
        char c = mReader->GetChar();
        if (c == Token2Char(expectedToken)) {
            TokenInfo tokenInfo(expectedToken,
                                mReader->GetCurrentFilePath(),
                                lineNo, columnNo);
            mCurrentTokenInfo = std::move(tokenInfo);
            return mCurrentTokenInfo;
        }
        else if (IsSpace(c)) {
            continue;
        }
        else if (c == '_' || IsAlphabet(c)) {
            TokenInfo tokenInfo = ReadIdentifier(c);
            tokenInfo.mTokenFilePath = mReader->GetCurrentFilePath();
            tokenInfo.mTokenLineNo = lineNo;
            tokenInfo.mTokenColumnNo = columnNo;
            mCurrentTokenInfo = std::move(tokenInfo);
            return mCurrentTokenInfo;
        }
        else if (IsDecimalDigital(c)) {
            TokenInfo tokenInfo = ReadNumber(c);
            tokenInfo.mTokenFilePath = mReader->GetCurrentFilePath();
            tokenInfo.mTokenLineNo = lineNo;
            tokenInfo.mTokenColumnNo = columnNo;
            mCurrentTokenInfo = std::move(tokenInfo);
            return mCurrentTokenInfo;
        }
        switch (c) {
            case '<': {
                if (mReader->PeekChar() == '<') {
                    mReader->GetChar();
                    TokenInfo tokenInfo(Token::SHIFT_LEFT,
                                        mReader->GetCurrentFilePath(),
                                        lineNo, columnNo);
                    mCurrentTokenInfo = std::move(tokenInfo);
                    return mCurrentTokenInfo;
                }
                TokenInfo tokenInfo(Token::ANGLE_BRACKETS_OPEN,
                                    mReader->GetCurrentFilePath(),
                                    lineNo, columnNo);
                mCurrentTokenInfo = std::move(tokenInfo);
                return mCurrentTokenInfo;
            }
            case '>': {
                if (mReader->PeekChar() == '>') {
                    mReader->GetChar();
                    if (mReader->PeekChar() == '>') {
                        mReader->GetChar();
                        TokenInfo tokenInfo(Token::SHIFT_RIGHT_UNSIGNED,
                                            mReader->GetCurrentFilePath(),
                                            lineNo, columnNo);
                        mCurrentTokenInfo = std::move(tokenInfo);
                        return mCurrentTokenInfo;
                    }
                    TokenInfo tokenInfo(Token::SHIFT_RIGHT,
                                        mReader->GetCurrentFilePath(),
                                        lineNo, columnNo);
                    mCurrentTokenInfo = std::move(tokenInfo);
                    return mCurrentTokenInfo;
                }
                TokenInfo tokenInfo(Token::ANGLE_BRACKETS_CLOSE,
                                    mReader->GetCurrentFilePath(),
                                    lineNo, columnNo);
                mCurrentTokenInfo = std::move(tokenInfo);
                return mCurrentTokenInfo;
            }
            case '=': {
                TokenInfo tokenInfo(Token::ASSIGNMENT,
                                    mReader->GetCurrentFilePath(),
                                    lineNo, columnNo);
                mCurrentTokenInfo = std::move(tokenInfo);
                return mCurrentTokenInfo;
            }
            case '*': {
                TokenInfo tokenInfo(Token::ASTERISK,
                                    mReader->GetCurrentFilePath(),
                                    lineNo, columnNo);
                mCurrentTokenInfo = std::move(tokenInfo);
                return mCurrentTokenInfo;
            }
            case '{': {
                TokenInfo tokenInfo(Token::BRACES_OPEN,
                                    mReader->GetCurrentFilePath(),
                                    lineNo, columnNo);
                mCurrentTokenInfo = std::move(tokenInfo);
                return mCurrentTokenInfo;
            }
            case '}': {
                TokenInfo tokenInfo(Token::BRACES_CLOSE,
                                    mReader->GetCurrentFilePath(),
                                    lineNo, columnNo);
                mCurrentTokenInfo = std::move(tokenInfo);
                return mCurrentTokenInfo;
            }
            case '[': {
                TokenInfo tokenInfo(Token::BRACKETS_OPEN,
                                    mReader->GetCurrentFilePath(),
                                    lineNo, columnNo);
                mCurrentTokenInfo = std::move(tokenInfo);
                return mCurrentTokenInfo;
            }
            case ']': {
                TokenInfo tokenInfo(Token::BRACKETS_CLOSE,
                                    mReader->GetCurrentFilePath(),
                                    lineNo, columnNo);
                mCurrentTokenInfo = std::move(tokenInfo);
                return mCurrentTokenInfo;
            }
            case ':': {
                TokenInfo tokenInfo(Token::COLON,
                                    mReader->GetCurrentFilePath(),
                                    lineNo, columnNo);
                mCurrentTokenInfo = std::move(tokenInfo);
                return mCurrentTokenInfo;
            }
            case ',': {
                TokenInfo tokenInfo(Token::COMMA,
                                    mReader->GetCurrentFilePath(),
                                    lineNo, columnNo);
                mCurrentTokenInfo = std::move(tokenInfo);
                return mCurrentTokenInfo;
            }
            case '/': {
                if (mReader->PeekChar() == '/') {
                    TokenInfo tokenInfo = ReadLineComment(c);
                    tokenInfo.mTokenFilePath = mReader->GetCurrentFilePath();
                    tokenInfo.mTokenLineNo = lineNo;
                    tokenInfo.mTokenColumnNo = columnNo;
                    mCurrentTokenInfo = std::move(tokenInfo);
                    continue;
                }
                else if (mReader->PeekChar() == '*') {
                    TokenInfo tokenInfo = ReadBlockComment(c);
                    tokenInfo.mTokenFilePath = mReader->GetCurrentFilePath();
                    tokenInfo.mTokenLineNo = lineNo;
                    tokenInfo.mTokenColumnNo = columnNo;
                    mCurrentTokenInfo = std::move(tokenInfo);
                    continue;
                }
                TokenInfo tokenInfo(Token::DIVIDE,
                                    mReader->GetCurrentFilePath(),
                                    lineNo, columnNo);
                mCurrentTokenInfo = std::move(tokenInfo);
                return mCurrentTokenInfo;
            }
            case '"': {
                TokenInfo tokenInfo = ReadStringLiteral(c);
                tokenInfo.mTokenFilePath = mReader->GetCurrentFilePath();
                tokenInfo.mTokenLineNo = lineNo;
                tokenInfo.mTokenColumnNo = columnNo;
                mCurrentTokenInfo = std::move(tokenInfo);
                return mCurrentTokenInfo;
            }
            case '-': {
                TokenInfo tokenInfo(Token::MINUS,
                                    mReader->GetCurrentFilePath(),
                                    lineNo, columnNo);
                mCurrentTokenInfo = std::move(tokenInfo);
                return mCurrentTokenInfo;
            }
            case '(': {
                TokenInfo tokenInfo(Token::PARENTHESES_OPEN,
                                    mReader->GetCurrentFilePath(),
                                    lineNo, columnNo);
                mCurrentTokenInfo = std::move(tokenInfo);
                return mCurrentTokenInfo;
            }
            case ')': {
                TokenInfo tokenInfo(Token::PARENTHESES_CLOSE,
                                    mReader->GetCurrentFilePath(),
                                    lineNo, columnNo);
                mCurrentTokenInfo = std::move(tokenInfo);
                return mCurrentTokenInfo;
            }
            case '.': {
                TokenInfo tokenInfo(Token::PERIOD,
                                    mReader->GetCurrentFilePath(),
                                    lineNo, columnNo);
                mCurrentTokenInfo = std::move(tokenInfo);
                return mCurrentTokenInfo;
            }
            case '\'': {
                TokenInfo tokenInfo = ReadCharacter(c);
                tokenInfo.mTokenFilePath = mReader->GetCurrentFilePath();
                tokenInfo.mTokenLineNo = lineNo;
                tokenInfo.mTokenColumnNo = columnNo;
            }
            case ';': {
                TokenInfo tokenInfo(Token::SEMICOLON,
                                    mReader->GetCurrentFilePath(),
                                    lineNo, columnNo);
                mCurrentTokenInfo = std::move(tokenInfo);
                return mCurrentTokenInfo;
            }
            case '&': {
                TokenInfo tokenInfo(Token::AMPERSAND,
                                    mReader->GetCurrentFilePath(),
                                    lineNo, columnNo);
                mCurrentTokenInfo = std::move(tokenInfo);
                return mCurrentTokenInfo;
            }
            case '~': {
                TokenInfo tokenInfo(Token::COMPLIMENT,
                                    mReader->GetCurrentFilePath(),
                                    lineNo, columnNo);
                mCurrentTokenInfo = std::move(tokenInfo);
                return mCurrentTokenInfo;
            }
            case '^': {
                TokenInfo tokenInfo(Token::EXCLUSIVE_OR,
                                    mReader->GetCurrentFilePath(),
                                    lineNo, columnNo);
                mCurrentTokenInfo = std::move(tokenInfo);
                return mCurrentTokenInfo;
            }
            case '|': {
                TokenInfo tokenInfo(Token::INCLUSIVE_OR,
                                    mReader->GetCurrentFilePath(),
                                    lineNo, columnNo);
                mCurrentTokenInfo = std::move(tokenInfo);
                return mCurrentTokenInfo;
            }
            case '%': {
                TokenInfo tokenInfo(Token::MODULO,
                                    mReader->GetCurrentFilePath(),
                                    lineNo, columnNo);
                mCurrentTokenInfo = std::move(tokenInfo);
                return mCurrentTokenInfo;
            }
            case '!': {
                TokenInfo tokenInfo(Token::NOT,
                                    mReader->GetCurrentFilePath(),
                                    lineNo, columnNo);
                mCurrentTokenInfo = std::move(tokenInfo);
                return mCurrentTokenInfo;
            }
            case '+': {
                TokenInfo tokenInfo(Token::PLUS,
                                    mReader->GetCurrentFilePath(),
                                    lineNo, columnNo);
                mCurrentTokenInfo = std::move(tokenInfo);
                return mCurrentTokenInfo;
            }
            default: {
                TokenInfo tokenInfo(Token::UNKNOWN,
                                    mReader->GetCurrentFilePath(),
                                    lineNo, columnNo);
                mCurrentTokenInfo = std::move(tokenInfo);
                return mCurrentTokenInfo;
            }
        }
    }
    TokenInfo tokenInfo(Token::END_OF_FILE,
                        mReader->GetCurrentFilePath(),
                        lineNo, columnNo);
    mCurrentTokenInfo = std::move(tokenInfo);
    return mCurrentTokenInfo;
}

TokenInfo Tokenizer::ReadUuidNumberToken()
{
    static constexpr int SEGMENT_1 = 0;
    static constexpr int SEGMENT_2 = 1;
    static constexpr int SEGMENT_3 = 2;
    static constexpr int SEGMENT_4 = 3;
    static constexpr int SEGMENT_5 = 4;

    StringBuilder builder;

    while (!mReader->IsEof() && IsSpace(mReader->PeekChar())) {
        mReader->GetChar();
    }

    int lineNo = mReader->GetCurrentLineNumber();
    int columnNo = mReader->GetCurrentColumnNumber();

    int state = SEGMENT_1;
    int index = 0;
    while (!mReader->IsEof()) {
        char c = mReader->PeekChar();
        index++;
        if (state == SEGMENT_1) {
            if (c == '-') {
                if (index == 9) {
                    mReader->GetChar();
                    builder.Append(c);
                    state = SEGMENT_2;
                    index = 0;
                    continue;
                }
                TokenInfo tokenInfo(Token::UNKNOWN,
                                    mReader->GetCurrentFilePath(),
                                    lineNo, columnNo);
                mCurrentTokenInfo = std::move(tokenInfo);
                return mCurrentTokenInfo;
            }
            else if (IsHexDigital(c)) {
                if (index <= 8) {
                    mReader->GetChar();
                    builder.Append(c);
                    continue;
                }
                TokenInfo tokenInfo(Token::UNKNOWN,
                                    mReader->GetCurrentFilePath(),
                                    lineNo, columnNo);
                mCurrentTokenInfo = std::move(tokenInfo);
                return mCurrentTokenInfo;
            }
            else {
                TokenInfo tokenInfo(Token::UNKNOWN,
                                    mReader->GetCurrentFilePath(),
                                    lineNo, columnNo);
                mCurrentTokenInfo = std::move(tokenInfo);
                return mCurrentTokenInfo;
            }
        }
        else if (state == SEGMENT_2) {
            if (c == '-') {
                if (index == 5) {
                    mReader->GetChar();
                    builder.Append(c);
                    state = SEGMENT_3;
                    index = 0;
                    continue;
                }
                TokenInfo tokenInfo(Token::UNKNOWN,
                                    mReader->GetCurrentFilePath(),
                                    lineNo, columnNo);
                mCurrentTokenInfo = std::move(tokenInfo);
                return mCurrentTokenInfo;
            }
            else if (IsHexDigital(c)) {
                if (index <= 4) {
                    mReader->GetChar();
                    builder.Append(c);
                    continue;
                }
                TokenInfo tokenInfo(Token::UNKNOWN,
                                    mReader->GetCurrentFilePath(),
                                    lineNo, columnNo);
                mCurrentTokenInfo = std::move(tokenInfo);
                return mCurrentTokenInfo;
            }
            else {
                TokenInfo tokenInfo(Token::UNKNOWN,
                                    mReader->GetCurrentFilePath(),
                                    lineNo, columnNo);
                mCurrentTokenInfo = std::move(tokenInfo);
                return mCurrentTokenInfo;
            }
        }
        else if (state == SEGMENT_3) {
            if (c == '-') {
                if (index == 5) {
                    mReader->GetChar();
                    builder.Append(c);
                    state = SEGMENT_4;
                    index = 0;
                    continue;
                }
                TokenInfo tokenInfo(Token::UNKNOWN,
                                    mReader->GetCurrentFilePath(),
                                    lineNo, columnNo);
                mCurrentTokenInfo = std::move(tokenInfo);
                return mCurrentTokenInfo;
            }
            else if (IsHexDigital(c)) {
                if (index <= 4) {
                    mReader->GetChar();
                    builder.Append(c);
                    continue;
                }
                TokenInfo tokenInfo(Token::UNKNOWN,
                                    mReader->GetCurrentFilePath(),
                                    lineNo, columnNo);
                mCurrentTokenInfo = std::move(tokenInfo);
                return mCurrentTokenInfo;
            }
            else {
                TokenInfo tokenInfo(Token::UNKNOWN,
                                    mReader->GetCurrentFilePath(),
                                    lineNo, columnNo);
                mCurrentTokenInfo = std::move(tokenInfo);
                return mCurrentTokenInfo;
            }
        }
        else if (state == SEGMENT_4) {
            if (c == '-') {
                if (index == 5) {
                    mReader->GetChar();
                    builder.Append(c);
                    state = SEGMENT_5;
                    index = 0;
                    continue;
                }
                TokenInfo tokenInfo(Token::UNKNOWN,
                                    mReader->GetCurrentFilePath(),
                                    lineNo, columnNo);
                mCurrentTokenInfo = std::move(tokenInfo);
                return mCurrentTokenInfo;
            }
            else if (IsHexDigital(c)) {
                if (index <= 4) {
                    mReader->GetChar();
                    builder.Append(c);
                    continue;
                }
                TokenInfo tokenInfo(Token::UNKNOWN,
                                    mReader->GetCurrentFilePath(),
                                    lineNo, columnNo);
                mCurrentTokenInfo = std::move(tokenInfo);
                return mCurrentTokenInfo;
            }
            else {
                TokenInfo tokenInfo(Token::UNKNOWN,
                                    mReader->GetCurrentFilePath(),
                                    lineNo, columnNo);
                mCurrentTokenInfo = std::move(tokenInfo);
                return mCurrentTokenInfo;
            }
        }
        else if (state == SEGMENT_5) {
            if (IsHexDigital(c)) {
                if (index <= 12) {
                    mReader->GetChar();
                    builder.Append(c);
                    continue;
                }
                TokenInfo tokenInfo(Token::UNKNOWN,
                                    mReader->GetCurrentFilePath(),
                                    lineNo, columnNo);
                mCurrentTokenInfo = std::move(tokenInfo);
                return mCurrentTokenInfo;
            }
            else {
                if (index >= 13) {
                    TokenInfo tokenInfo(Token::UUID_NUMBER,
                                        mReader->GetCurrentFilePath(),
                                        lineNo, columnNo);
                    tokenInfo.mStringValue = builder.ToString();
                    mCurrentTokenInfo = std::move(tokenInfo);
                    return mCurrentTokenInfo;
                }
                TokenInfo tokenInfo(Token::UNKNOWN,
                                    mReader->GetCurrentFilePath(),
                                    lineNo, columnNo);
                mCurrentTokenInfo = std::move(tokenInfo);
                return mCurrentTokenInfo;
            }
        }
    }
    TokenInfo tokenInfo(Token::END_OF_FILE,
                        mReader->GetCurrentFilePath(),
                        lineNo, columnNo);
    mCurrentTokenInfo = std::move(tokenInfo);
    return mCurrentTokenInfo;
}

TokenInfo Tokenizer::ReadVersionNumberToken()
{
    static constexpr int SEGMENT_1 = 0;
    static constexpr int SEGMENT_2 = 1;
    static constexpr int SEGMENT_3 = 2;
    static constexpr int SEGMENT_4 = 3;
    static constexpr int SEGMENT_5 = 4;

    StringBuilder builder;

    while (!mReader->IsEof() && IsSpace(mReader->PeekChar())) {
        mReader->GetChar();
    }

    int lineNo = mReader->GetCurrentLineNumber();
    int columnNo = mReader->GetCurrentColumnNumber();

    int state = SEGMENT_1;
    while (!mReader->IsEof()) {
        char c = mReader->PeekChar();
        if (state == SEGMENT_1) {
            if (IsDecimalDigital(c)) {
                mReader->GetChar();
                builder.Append(c);
                continue;
            }
            else if (c == '.') {
                mReader->GetChar();
                builder.Append(c);
                state = SEGMENT_2;
                continue;
            }
            else {
                TokenInfo tokenInfo(Token::UNKNOWN,
                                    mReader->GetCurrentFilePath(),
                                    lineNo, columnNo);
                mCurrentTokenInfo = std::move(tokenInfo);
                return mCurrentTokenInfo;
            }
        }
        else if (state == SEGMENT_2) {
            if (IsDecimalDigital(c)) {
                mReader->GetChar();
                builder.Append(c);
                continue;
            }
            else if (c == '.') {
                mReader->GetChar();
                builder.Append(c);
                state = SEGMENT_3;
                continue;
            }
            else {
                TokenInfo tokenInfo(Token::UNKNOWN,
                                    mReader->GetCurrentFilePath(),
                                    lineNo, columnNo);
                mCurrentTokenInfo = std::move(tokenInfo);
                return mCurrentTokenInfo;
            }
        }
        else if (state == SEGMENT_3) {
            if (IsDecimalDigital(c)) {
                mReader->GetChar();
                builder.Append(c);
                continue;
            }
            else {
                TokenInfo tokenInfo(Token::VERSION_NUMBER,
                                    mReader->GetCurrentFilePath(),
                                    lineNo, columnNo);
                tokenInfo.mStringValue = builder.ToString();
                mCurrentTokenInfo = std::move(tokenInfo);
                return mCurrentTokenInfo;
            }
        }
    }
    TokenInfo tokenInfo(Token::END_OF_FILE,
                        mReader->GetCurrentFilePath(),
                        lineNo, columnNo);
    mCurrentTokenInfo = std::move(tokenInfo);
    return mCurrentTokenInfo;
}

TokenInfo Tokenizer::ReadIdentifier(
    /* [in] */ char c)
{
    StringBuilder builder;

    builder.Append(c);
    while (!mReader->IsEof()) {
        c = mReader->PeekChar();
        if (c == '_' || IsAlphabet(c) || IsDecimalDigital(c)) {
            mReader->GetChar();
            builder.Append(c);
            continue;
        }
        else if (c == ':') {
            mReader->GetChar();
            if (mReader->PeekChar() == ':') {
                mReader->GetChar();
                builder.Append("::");
                continue;
            }
            else {
                mReader->UnGetChar();
            }
        }
        break;
    }
    String id = builder.ToString();
    Token token = mKeywords[id];
    if (token == Token::UNKNOWN) {
        TokenInfo tokenInfo(Token::IDENTIFIER,
                            mReader->GetCurrentFilePath());
        tokenInfo.mStringValue = id;
        return tokenInfo;
    }
    TokenInfo tokenInfo(token,
                        mReader->GetCurrentFilePath());
    return tokenInfo;
}

TokenInfo Tokenizer::ReadNumber(
    /* [in] */ char c)
{
    static constexpr int NUMBER_INT = 0;
    static constexpr int NUMBER_INT_0 = 1;
    static constexpr int NUMBER_FP = 2;
    static constexpr int NUMBER_FP_EXP = 3;

    StringBuilder builder;

    builder.Append(c);
    int bit = 32;
    int radix = c == '0' ? 8 : 10;
    bool scientificNotation = false;
    int state = c == '0' ? NUMBER_INT_0 : NUMBER_INT;
    while (!mReader->IsEof()) {
        c = mReader->PeekChar();
        if (state == NUMBER_INT_0) {
            if (c == 'x' || c =='X') {
                mReader->GetChar();
                builder.Append(c);
                radix = 16;
                state = NUMBER_INT;
                continue;
            }
            else if (IsOctalDigital(c)) {
                mReader->GetChar();
                builder.Append(c);
                state = NUMBER_INT;
                continue;
            }
            else if (c == '.') {
                mReader->GetChar();
                builder.Append(c);
                state = NUMBER_FP;
                continue;
            }
            break;
        }
        else if (state == NUMBER_INT) {
            if (radix == 10 && IsDecimalDigital(c)) {
                mReader->GetChar();
                builder.Append(c);
                continue;
            }
            else if (radix == 16 && IsHexDigital(c)) {
                mReader->GetChar();
                builder.Append(c);
                continue;
            }
            else if (c == '.') {
                mReader->GetChar();
                builder.Append(c);
                state = NUMBER_FP;
                continue;
            }
            else if (c == 'l') {
                mReader->GetChar();
                if (mReader->PeekChar() == 'l') {
                    mReader->GetChar();
                    bit = 64;
                }
                break;
            }
            else if (c == 'L') {
                mReader->GetChar();
                if (mReader->PeekChar() == 'L') {
                    mReader->GetChar();
                    bit = 64;
                }
                break;
            }
            else if (c == 'e' || c == 'E') {
                mReader->GetChar();
                builder.Append(c);
                state = NUMBER_FP_EXP;
                scientificNotation = true;
                continue;
            }
            break;
        }
        else if (state == NUMBER_FP) {
            if (IsDecimalDigital(c)) {
                mReader->GetChar();
                builder.Append(c);
                continue;
            }
            else if (c == 'e' || c == 'E') {
                mReader->GetChar();
                builder.Append(c);
                state = NUMBER_FP_EXP;
                scientificNotation = true;
                continue;
            }
            else if (c == 'f' || c == 'F') {
                mReader->GetChar();
                break;
            }
            else if (c == 'd' || c == 'D') {
                mReader->GetChar();
                bit = 64;
                break;
            }
            break;
        }
        else if (state == NUMBER_FP_EXP) {
            if (c == '+' || c == '-') {
                mReader->GetChar();
                builder.Append(c);
                continue;
            }
            else if (IsDecimalDigital(c)) {
                mReader->GetChar();
                builder.Append(c);
                continue;
            }
            else if (c == 'f' || c == 'F') {
                mReader->GetChar();
                break;
            }
            else if (c == 'd' || c == 'D') {
                mReader->GetChar();
                bit = 64;
                break;
            }
            break;
        }
    }
    String number = builder.ToString();
    if (state == NUMBER_INT_0 || state == NUMBER_INT) {
        if (number.Equals("0x8000000000000000")) {
            number = String("-") + number;
        }
        TokenInfo tokenInfo(Token::NUMBER_INTEGRAL,
                            mReader->GetCurrentFilePath());
        tokenInfo.mStringValue = number;
        tokenInfo.mIntegralValue = strtoll(number.string(), nullptr, radix);
        tokenInfo.mBit = bit;
        tokenInfo.mRadix = radix;
        return tokenInfo;
    }
    else {
        TokenInfo tokenInfo(Token::NUMBER_FLOATINGPOINT,
                            mReader->GetCurrentFilePath());
        tokenInfo.mStringValue = number;
        tokenInfo.mIntegralValue = atof(number.string());
        tokenInfo.mScientificNotation = scientificNotation;
        return tokenInfo;
    }
}

TokenInfo Tokenizer::ReadCharacter(
    /* [in] */ char c)
{
    c = mReader->GetChar();
    if (c == '\'') {
        TokenInfo tokenInfo(Token::CHARACTER,
                            mReader->GetCurrentFilePath());
        tokenInfo.mCharValue = 0;
        tokenInfo.mStringValue = "";
        return tokenInfo;
    }

    if (mReader->PeekChar() != '\'') {
        TokenInfo tokenInfo(Token::UNKNOWN,
                            mReader->GetCurrentFilePath());
        return tokenInfo;
    }

    mReader->GetChar();
    TokenInfo tokenInfo(Token::CHARACTER,
                        mReader->GetCurrentFilePath());
    tokenInfo.mCharValue = c;
    tokenInfo.mStringValue = String::Format("%c", c);
    return tokenInfo;
}

TokenInfo Tokenizer::ReadStringLiteral(
    /* [in] */ char c)
{
    StringBuilder builder;

    while (!mReader->IsEof()) {
        c = mReader->GetChar();
        if (c != '"') {
            builder.Append(c);
        }
        else {
            TokenInfo tokenInfo(Token::STRING_LITERAL,
                                mReader->GetCurrentFilePath());
            tokenInfo.mStringValue = builder.ToString();
            return tokenInfo;
        }
    }
    TokenInfo tokenInfo(Token::END_OF_FILE,
                        mReader->GetCurrentFilePath());
    return tokenInfo;
}

TokenInfo Tokenizer::ReadLineComment(
    /* [in] */ char c)
{
    StringBuilder builder;

    builder.Append(c);
    while (!mReader->IsEof()) {
        c = mReader->GetChar();
        if (c == '\n') {
            break;
        }
        builder.Append(c);
    }
    TokenInfo tokenInfo(Token::COMMENT_LINE,
                        mReader->GetCurrentFilePath());
    tokenInfo.mStringValue = builder.ToString();
    return tokenInfo;
}

TokenInfo Tokenizer::ReadBlockComment(
    /* [in] */ char c)
{
    StringBuilder builder;

    builder.Append(c);
    while (!mReader->IsEof()) {
        c = mReader->GetChar();
        if (c == '*' && mReader->PeekChar() == '/') {
            mReader->GetChar();
            builder.Append("*/");
            break;
        }
        builder.Append(c);
    }
    TokenInfo tokenInfo(Token::COMMENT_BLOCK,
                        mReader->GetCurrentFilePath());
    tokenInfo.mStringValue = builder.ToString();
    return tokenInfo;
}

void Tokenizer::SkipCurrentLine()
{
    while (!mReader->IsEof()) {
        if (mReader->GetChar() == '\n') {
            return;
        }
    }
}

}
