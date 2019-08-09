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
    mKeywords["url"] = Token::URL;
    mKeywords["uuid"] = Token::UUID;
    mKeywords["version"] = Token::VERSION;
}

Token Tokenizer::PeekToken(
    /* [in] */ Token expectedToken)
{
    mReader->Mark();
    Token token = ReadToken(expectedToken);
    mReader->Reset();
    return token;
}

Token Tokenizer::GetToken(
    /* [in] */ Token expectedToken)
{
    return ReadToken(expectedToken);
}

Token Tokenizer::ReadToken(
    /* [in] */ Token expectedToken)
{
    while (!mReader->IsEof()) {
        int lineNo = mReader->GetCurrentLineNumber();
        int columnNo = mReader->GetCurrentColumnNumber();
        char c = mReader->GetChar();
        if (c == Token2Char(expectedToken)) {
            mTokenLineNo = lineNo;
            mTokenColumnNo = columnNo;
            return expectedToken;
        }
        if (IsSpace(c)) {
            continue;
        }
        else if (c == '_' || IsAlphabet(c)) {
            return ReadIdentifier(c);
        }
        else if (IsDecimalDigital(c)) {
            return ReadNumber(c);
        }
        switch (c) {
            case '<': {
                if (mReader->PeekChar() == '<') {
                    mReader->GetChar();
                    mCurrentToken = Token::SHIFT_LEFT;
                    return mCurrentToken;
                }
                mCurrentToken = Token::ANGLE_BRACKETS_OPEN;
                return mCurrentToken;
            }
            case '>': {
                if (mReader->PeekChar() == '>') {
                    mReader->GetChar();
                    if (mReader->PeekChar() == '>') {
                        mReader->GetChar();
                        mCurrentToken = Token::SHIFT_RIGHT_UNSIGNED;
                        return mCurrentToken;
                    }
                    mCurrentToken = Token::SHIFT_RIGHT;
                    return mCurrentToken;
                }
                mCurrentToken = Token::ANGLE_BRACKETS_CLOSE;
                return mCurrentToken;
            }
            case '=':
                mCurrentToken = Token::ASSIGNMENT;
                return mCurrentToken;
            case '*':
                mCurrentToken = Token::ASTERISK;
                return mCurrentToken;
            case '{':
                mCurrentToken = Token::BRACES_OPEN;
                return mCurrentToken;
            case '}':
                mCurrentToken = Token::BRACES_CLOSE;
                return mCurrentToken;
            case '[':
                mCurrentToken = Token::BRACKETS_OPEN;
                return mCurrentToken;
            case ']':
                mCurrentToken = Token::BRACKETS_CLOSE;
                return mCurrentToken;
            case ':':
                mCurrentToken = Token::COLON;
                return mCurrentToken;
            case ',':
                mCurrentToken = Token::COMMA;
                return mCurrentToken;
            case '/': {
                if (mReader->PeekChar() == '/') {
                    ReadLineComment(c);
                    continue;
                }
                else if (mReader->PeekChar() == '*') {
                    ReadBlockComment(c);
                    continue;
                }
                mCurrentToken = Token::DIVIDE;
                return mCurrentToken;
            }
            case '"':
                return ReadStringLiteral(c);
            case '-':
                mCurrentToken = Token::MINUS;
                return mCurrentToken;
            case '(':
                mCurrentToken = Token::PARENTHESES_OPEN;
                return mCurrentToken;
            case ')':
                mCurrentToken = Token::PARENTHESES_CLOSE;
                return mCurrentToken;
            case '.':
                mCurrentToken = Token::PERIOD;
                return mCurrentToken;
            case '\'':
                return ReadCharacter(c);
            case ';':
                mCurrentToken = Token::SEMICOLON;
                return mCurrentToken;
            case '&':
                mCurrentToken = Token::AMPERSAND;
                return mCurrentToken;
            case '~':
                mCurrentToken = Token::COMPLIMENT;
                return mCurrentToken;
            case '^':
                mCurrentToken = Token::EXCLUSIVE_OR;
                return mCurrentToken;
            case '|':
                mCurrentToken = Token::INCLUSIVE_OR;
                return mCurrentToken;
            case '%':
                mCurrentToken = Token::MODULO;
                return mCurrentToken;
            case '!':
                mCurrentToken = Token::NOT;
                return mCurrentToken;
            case '+':
                mCurrentToken = Token::PLUS;
                return mCurrentToken;
            default:
                mCurrentToken = Token::UNKNOW;
                return mCurrentToken;
        }
    }
    mCurrentToken = Token::END_OF_FILE;
    return mCurrentToken;
}

Token Tokenizer::ReadUuidNumberToken()
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

    mTokenLineNo = mReader->GetCurrentLineNumber();
    mTokenColumnNo = mReader->GetCurrentColumnNumber();

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
                mCurrentToken = Token::UNKNOW;
                return mCurrentToken;
            }
            else if (IsHexDigital(c)) {
                if (index <= 8) {
                    mReader->GetChar();
                    builder.Append(c);
                    continue;
                }
                mCurrentToken = Token::UNKNOW;
                return mCurrentToken;
            }
            else {
                mCurrentToken = Token::UNKNOW;
                return mCurrentToken;
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
                mCurrentToken = Token::UNKNOW;
                return mCurrentToken;
            }
            else if (IsHexDigital(c)) {
                if (index <= 4) {
                    mReader->GetChar();
                    builder.Append(c);
                    continue;
                }
                mCurrentToken = Token::UNKNOW;
                return mCurrentToken;
            }
            else {
                mCurrentToken = Token::UNKNOW;
                return mCurrentToken;
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
                mCurrentToken = Token::UNKNOW;
                return mCurrentToken;
            }
            else if (IsHexDigital(c)) {
                if (index <= 4) {
                    mReader->GetChar();
                    builder.Append(c);
                    continue;
                }
                mCurrentToken = Token::UNKNOW;
                return mCurrentToken;
            }
            else {
                mCurrentToken = Token::UNKNOW;
                return mCurrentToken;
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
                mCurrentToken = Token::UNKNOW;
                return mCurrentToken;
            }
            else if (IsHexDigital(c)) {
                if (index <= 4) {
                    mReader->GetChar();
                    builder.Append(c);
                    continue;
                }
                mCurrentToken = Token::UNKNOW;
                return mCurrentToken;
            }
            else {
                mCurrentToken = Token::UNKNOW;
                return mCurrentToken;
            }
        }
        else if (state == SEGMENT_5) {
            if (IsHexDigital(c)) {
                if (index <= 12) {
                    mReader->GetChar();
                    builder.Append(c);
                    continue;
                }
                mCurrentToken = Token::UNKNOW;
                return mCurrentToken;
            }
            else {
                if (index >= 13) {
                    mString = builder.ToString();
                    mCurrentToken = Token::UUID_NUMBER;
                    return mCurrentToken;
                }
                mCurrentToken = Token::UNKNOW;
                return mCurrentToken;
            }
        }
    }
    mCurrentToken = Token::END_OF_FILE;
    return mCurrentToken;
}

Token Tokenizer::ReadVersionNumberToken()
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

    mTokenLineNo = mReader->GetCurrentLineNumber();
    mTokenColumnNo = mReader->GetCurrentColumnNumber();

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
                mCurrentToken = Token::UNKNOW;
                return mCurrentToken;
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
                mCurrentToken = Token::UNKNOW;
                return mCurrentToken;
            }
        }
        else if (state == SEGMENT_3) {
            if (IsDecimalDigital(c)) {
                mReader->GetChar();
                builder.Append(c);
                continue;
            }
            else {
                mString = builder.ToString();
                mCurrentToken = Token::VERSION_NUMBER;
                return mCurrentToken;
            }
        }
    }
    mCurrentToken = Token::END_OF_FILE;
    return mCurrentToken;
}

Token Tokenizer::ReadIdentifier(
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
    mCurrentToken = mKeywords[id];
    if (mCurrentToken == Token::UNKNOW) {
        mIdentifier = id;
        mCurrentToken = Token::IDENTIFIER;
    }
    return mCurrentToken;
}

Token Tokenizer::ReadNumber(
    /* [in] */ char c)
{
    static constexpr int NUMBER_INT = 0;
    static constexpr int NUMBER_INT_0 = 1;
    static constexpr int NUMBER_FP = 2;
    static constexpr int NUMBER_FP_EXP = 3;

    StringBuilder builder;

    builder.Append(c);
    mBit = 32;
    mRadix = c == '0' ? 8 : 10;
    mScientificNotation = false;
    int state = c == '0' ? NUMBER_INT_0 : NUMBER_INT;
    while (!mReader->IsEof()) {
        c = mReader->PeekChar();
        if (state == NUMBER_INT_0) {
            if (c == 'x' || c =='X') {
                mReader->GetChar();
                builder.Append(c);
                mRadix = 16;
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
            if (mRadix == 10 && IsDecimalDigital(c)) {
                mReader->GetChar();
                builder.Append(c);
                continue;
            }
            else if (mRadix == 16 && IsHexDigital(c)) {
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
                    mBit = 64;
                }
                break;
            }
            else if (c == 'L') {
                mReader->GetChar();
                if (mReader->PeekChar() == 'L') {
                    mReader->GetChar();
                    mBit = 64;
                }
                break;
            }
            else if (c == 'e' || c == 'E') {
                mReader->GetChar();
                builder.Append(c);
                state = NUMBER_FP_EXP;
                mScientificNotation = true;
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
                mScientificNotation = true;
                continue;
            }
            else if (c == 'f' || c == 'F') {
                mReader->GetChar();
                break;
            }
            else if (c == 'd' || c == 'D') {
                mReader->GetChar();
                mBit = 64;
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
                mBit = 64;
                break;
            }
            break;
        }
    }
    mNumberString = builder.ToString();
    if (state == NUMBER_INT_0 || state == NUMBER_INT) {
        if (mNumberString.Equals("0x8000000000000000")) {
            mNumberString = String("-") + mNumberString;
        }
        mIntegralValue = strtoll(mNumberString.string(), nullptr, mRadix);
        mCurrentToken = Token::NUMBER_INTEGRAL;
        return mCurrentToken;
    }
    else {
        mFloatingPointValue = atof(mNumberString.string());
        mCurrentToken = Token::NUMBER_FLOATINGPOINT;
        return mCurrentToken;
    }
}

Token Tokenizer::ReadCharacter(
    /* [in] */ char c)
{
    c = mReader->GetChar();
    if (c == '\'') {
        mCharacter = 0;
        mString = "";
        mCurrentToken = Token::CHARACTER;
        return mCurrentToken;
    }

    if (mReader->PeekChar() != '\'') {
        mCurrentToken = Token::UNKNOW;
        return mCurrentToken;
    }

    mCharacter = c;
    mReader->GetChar();
    mString = String::Format("%c", mCharacter);
    mCurrentToken = Token::CHARACTER;
    return mCurrentToken;
}

Token Tokenizer::ReadStringLiteral(
    /* [in] */ char c)
{
    StringBuilder builder;

    while (!mReader->IsEof()) {
        c = mReader->GetChar();
        if (c != '"') {
            builder.Append(c);
        }
        else {
            mString = builder.ToString();
            mCurrentToken = Token::STRING_LITERAL;
            return mCurrentToken;
        }
    }
    mCurrentToken = Token::END_OF_FILE;
    return mCurrentToken;
}

Token Tokenizer::ReadLineComment(
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
    mComment = builder.ToString();
    mCurrentToken = Token::COMMENT_LINE;
    return mCurrentToken;
}

Token Tokenizer::ReadBlockComment(
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
    mComment = builder.ToString();
    return Token::COMMENT_BLOCK;
}

String Tokenizer::DumpCurrentToken()
{
    switch (mCurrentToken) {
        case Token::AMPERSAND:
            return "&";
        case Token::ANGLE_BRACKETS_OPEN:
            return "<";
        case Token::ANGLE_BRACKETS_CLOSE:
            return ">";
        case Token::ARRAY:
            return "Array";
        case Token::ASSIGNMENT:
            return "=";
        case Token::ASTERISK:
            return "*";
        case Token::BOOLEAN:
            return "Boolean";
        case Token::BRACES_OPEN:
            return "{";
        case Token::BRACES_CLOSE:
            return "}";
        case Token::BRACKETS_OPEN:
            return "[";
        case Token::BRACKETS_CLOSE:
            return "]";
        case Token::BYTE:
            return "Byte";
        case Token::CALLEE:
            return "callee";
        case Token::CHAR:
            return "Char";
        case Token::CHARACTER:
            return mString;
        case Token::COCLASS:
            return "coclass";
        case Token::COCLASSID:
            return "CoclassID";
        case Token::COLON:
            return ":";
        case Token::COMMA:
            return ",";
        case Token::COMMENT_BLOCK:
        case Token::COMMENT_LINE:
            return mComment;
        case Token::COMPLIMENT:
            return "~";
        case Token::COMPONENTID:
            return "ComponentID";
        case Token::CONST:
            return "const";
        case Token::CONSTRUCTOR:
            return "Constructor";
        case Token::DELETE:
            return "Delete";
        case Token::DESCRIPTION:
            return "description";
        case Token::DIVIDE:
            return "/";
        case Token::DOUBLE:
            return "Double";
        case Token::ECODE:
            return "ECode";
        case Token::END_OF_LINE:
            return "\n";
        case Token::ENUM:
            return "enum";
        case Token::EXCLUSIVE_OR:
            return "^";
        case Token::FALSE:
            return "false";
        case Token::FLOAT:
            return "Float";
        case Token::HANDLE:
            return "HANDLE";
        case Token::IDENTIFIER:
            return mIdentifier;
        case Token::IN:
            return "in";
        case Token::INCLUDE:
            return "include";
        case Token::INCLUSIVE_OR:
            return "|";
        case Token::INTEGER:
            return "Integer";
        case Token::INTERFACE:
            return "interface";
        case Token::INTERFACEID:
            return "InterfaceID";
        case Token::LONG:
            return "Long";
        case Token::MINUS:
            return "-";
        case Token::MODULE:
            return "module";
        case Token::MODULO:
            return "%";
        case Token::NAMESPACE:
            return "namespace";
        case Token::NOT:
            return "!";
        case Token::NULLPTR:
            return "nullptr";
        case Token::NUMBER_INTEGRAL:
        case Token::NUMBER_FLOATINGPOINT:
            return mNumberString;
        case Token::OUT:
            return "out";
        case Token::PARENTHESES_OPEN:
            return "(";
        case Token::PARENTHESES_CLOSE:
            return ")";
        case Token::PERIOD:
            return ".";
        case Token::PLUS:
            return "+";
        case Token::SEMICOLON:
            return ";";
        case Token::SHIFT_LEFT:
            return "<<";
        case Token::SHIFT_RIGHT:
            return ">>";
        case Token::SHIFT_RIGHT_UNSIGNED:
            return ">>>";
        case Token::SHORT:
            return "Short";
        case Token::STRING:
            return "String";
        case Token::STRING_LITERAL:
            return mString;
        case Token::TRIPLE:
            return "Triple";
        case Token::TRUE:
            return "true";
        case Token::URL:
            return "url";
        case Token::UUID:
            return "uuid";
        case Token::UUID_NUMBER:
            return mString;
        case Token::VERSION:
            return "version";
        case Token::VERSION_NUMBER:
            return mString;
        default:
            return "";
    }
}

}
