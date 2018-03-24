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

#include "Tokenizer.h"
#include "../util/Logger.h"

namespace ccm {

struct KV
{
    String mKey;
    Tokenizer::Token mValue;
}
sKeywords[28] =
{
    { String("Array"), Tokenizer::Token::ARRAY },
    { String("Boolean"), Tokenizer::Token::BOOLEAN },
    { String("Byte"), Tokenizer::Token::BYTE },
    { String("callee"), Tokenizer::Token::CALLEE },
    { String("Char"), Tokenizer::Token::CHAR },
    { String("coclass"), Tokenizer::Token::COCLASS },
    { String("const"), Tokenizer::Token::CONST },
    { String("constructor"), Tokenizer::Token::CONSTRUCTOR },
    { String("description"), Tokenizer::Token::DESCRIPTION },
    { String("Double"), Tokenizer::Token::DOUBLE },
    { String("enum"), Tokenizer::Token::ENUM },
    { String("false"), Tokenizer::Token::FALSE },
    { String("Float"), Tokenizer::Token::FLOAT },
    { String("HANDLE"), Tokenizer::Token::HANDLE },
    { String("in"), Tokenizer::Token::IN },
    { String("include"), Tokenizer::Token::INCLUDE },
    { String("Integer"), Tokenizer::Token::INTEGER },
    { String("interface"), Tokenizer::Token::INTERFACE },
    { String("Long"), Tokenizer::Token::LONG },
    { String("module"), Tokenizer::Token::MODULE },
    { String("namespace"), Tokenizer::Token::NAMESPACE },
    { String("out"), Tokenizer::Token::OUT },
    { String("Short"), Tokenizer::Token::SHORT },
    { String("String"), Tokenizer::Token::STRING },
    { String("true"), Tokenizer::Token::TRUE },
    { String("url"), Tokenizer::Token::URL },
    { String("uuid"), Tokenizer::Token::UUID },
    { String("version"), Tokenizer::Token::VERSION },
};

const String Tokenizer::TAG("Tokenizer");

Tokenizer::Tokenizer()
    : mFile(nullptr)
    , mFileStack(nullptr)
    , mCurrToken(Token::ILLEGAL_TOKEN)
    , mHasAPeek(false)
    , mTokenLineNo(0)
    , mTokenColumnNo(0)
    , mIntegralValue(0)
    , mFloatingPointValue(0)
    , mBit(0)
{
    InitializeKeyword();
}

Tokenizer::~Tokenizer()
{
    mFile = nullptr;
    FileNode* fn = mFileStack;
    while (fn != nullptr) {
        mFileStack = fn->mNext;
        delete fn;
        fn = mFileStack;
    }
    mFileStack = nullptr;
}

bool Tokenizer::PushInputFile(
    /* [in] */ const String& filePath)
{
    File* file = new File(filePath);
    if (!file->IsValid()) return false;

    if (mFileStack == nullptr) {
        mFileStack = new FileNode();
        mFileStack->mFile = file;
        mFile = file;
        return true;
    }

    FileNode* fn = new FileNode();
    fn->mFile = file;
    fn->mNext = mFileStack;
    mFileStack = fn;
    mFile = file;
    return true;
}

void Tokenizer::PopInputFileAndRemove()
{
    if (mFileStack == nullptr) return;
    FileNode* fn = mFileStack;
    mFileStack = mFileStack->mNext;
    delete fn;
    mFile = mFileStack != nullptr ? mFileStack->mFile : nullptr;
}

void Tokenizer::InitializeKeyword()
{
    for (int i = 0; i < sizeof(sKeywords) / sizeof(KV); i++) {
        mKeywords.Put(sKeywords[i].mKey, sKeywords[i].mValue);
    }
}

Tokenizer::Token Tokenizer::PeekToken()
{
    if (mHasAPeek) return mCurrToken;

    mCurrToken = ReadToken();
    mHasAPeek = true;
    return mCurrToken;
}

Tokenizer::Token Tokenizer::GetToken()
{
    if (mHasAPeek) {
        mHasAPeek = false;
        return mCurrToken;
    }
    return ReadToken();
}

Tokenizer::Token Tokenizer::GetUuidNumberToken()
{
    if (mHasAPeek) {
        mHasAPeek = false;
        return mCurrToken;
    }
    return ReadUuidNumberToken();
}

Tokenizer::Token Tokenizer::GetVersionNumberToken()
{
    if (mHasAPeek) {
        mHasAPeek = false;
        return mCurrToken;
    }
    return ReadVersionNumberToken();
}

void Tokenizer::SkipCurrentLine()
{
    int c = mFile->Read();
    while (c != -1 && c != '\n') {
        c = mFile->Read();
    }
}

Tokenizer::Token Tokenizer::ReadToken()
{
    int c;
    while ((c = mFile->Read()) != -1) {
        mTokenLineNo = mFile->GetLineNo();
        mTokenColumnNo = mFile->GetColumnNo();
        if (IsEscape(c)) {
            continue;
        }
        else if (IsAlphabet(c) || c == '_' || c == '-') {
            return ReadIdentifier(c);
        }
        else if (IsDecimalDigital(c)) {
            return ReadNumber(c);
        }
        switch (c) {
            case '<': {
                if (mFile->Peek() == '<') {
                    mFile->Read();
                    return Token::SHIFT_LEFT;
                }
                return Token::ANGLE_BRACKETS_OPEN;
            }
            case '>': {
                if (mFile->Peek() == '>') {
                    mFile->Read();
                    if (mFile->Peek() == '>') {
                        mFile->Read();
                        return Token::SHIFT_RIGHT_UNSIGNED;
                    }
                    else return Token::SHIFT_RIGHT;
                }
                return Token::ANGLE_BRACKETS_CLOSE;
            }
            case '=':
                return Token::ASSIGNMENT;
            case '*':
                return Token::ASTERISK;
            case '{':
                return Token::BRACES_OPEN;
            case '}':
                return Token::BRACES_CLOSE;
            case '[':
                return Token::BRACKETS_OPEN;
            case ']':
                return Token::BRACKETS_CLOSE;
            case ':':
                return Token::COLON;
            case ',':
                return Token::COMMA;
            case '/':
                if (mFile->Peek() == '/') {
                    ReadLineComment(c);
                    continue;
                }
                else if (mFile->Peek() == '*') {
                    ReadBlockComment(c);
                    continue;
                }
                return Token::DIVIDE;
            case '"':
                return ReadStringLiteral(c);
            case '-':
                return Token::MINUS;
            case '(':
                return Token::PARENTHESES_OPEN;
            case ')':
                return Token::PARENTHESES_CLOSE;
            case '.':
                return Token::PERIOD;
            case '\'':
                return ReadCharacter(c);
            case ';':
                return Token::SEMICOLON;
            default:
                // todo:
                continue;
        }
    }
    return Token::END_OF_FILE;
}

Tokenizer::Token Tokenizer::ReadUuidNumberToken()
{
    static constexpr int SEGMENT_1 = 0;
    static constexpr int SEGMENT_2 = 1;
    static constexpr int SEGMENT_3 = 2;
    static constexpr int SEGMENT_4 = 3;
    static constexpr int SEGMENT_5 = 4;

    StringBuilder builder;

    int state = SEGMENT_1;
    int c, index = 0;
    while ((c = mFile->Read()) != -1) {
        index++;
        if (state == SEGMENT_1) {
            if (index == 1) {
                mTokenLineNo = mFile->GetLineNo();
                mTokenColumnNo = mFile->GetColumnNo();
            }
            if (c == '-') {
                if (index == 9) {
                    builder.Append((char)c);
                    state = SEGMENT_2;
                    index = 0;
                    continue;
                }
                else return Token::ILLEGAL_TOKEN;
            }
            else if (IsHexDigital(c)) {
                if (index > 8) return Token::ILLEGAL_TOKEN;
                builder.Append((char)c);
                continue;
            }
            else {
                mFile->Unread(c);
                return Token::ILLEGAL_TOKEN;
            }
        }
        else if (state == SEGMENT_2) {
            if (c == '-') {
                if (index == 5) {
                    builder.Append((char)c);
                    state = SEGMENT_3;
                    index = 0;
                    continue;
                }
                else return Token::ILLEGAL_TOKEN;
            }
            else if (IsHexDigital(c)) {
                if (index > 4) return Token::ILLEGAL_TOKEN;
                builder.Append((char)c);
                continue;
            }
            else {
                mFile->Unread(c);
                return Token::ILLEGAL_TOKEN;
            }
        }
        else if (state == SEGMENT_3) {
            if (c == '-') {
                if (index == 5) {
                    builder.Append((char)c);
                    state = SEGMENT_4;
                    index = 0;
                    continue;
                }
                else return Token::ILLEGAL_TOKEN;
            }
            else if (IsHexDigital(c)) {
                if (index > 4) return Token::ILLEGAL_TOKEN;
                builder.Append((char)c);
                continue;
            }
            else {
                mFile->Unread(c);
                return Token::ILLEGAL_TOKEN;
            }
        }
        else if (state == SEGMENT_4) {
            if (c == '-') {
                if (index == 5) {
                    builder.Append((char)c);
                    state = SEGMENT_5;
                    index = 0;
                    continue;
                }
                else return Token::ILLEGAL_TOKEN;
            }
            else if (IsHexDigital(c)) {
                if (index > 4) return Token::ILLEGAL_TOKEN;
                builder.Append((char)c);
                continue;
            }
            else {
                mFile->Unread(c);
                return Token::ILLEGAL_TOKEN;
            }
        }
        else if (state == SEGMENT_5) {
            if (IsHexDigital(c)) {
                if (index > 12) return Token::ILLEGAL_TOKEN;
                builder.Append((char)c);
                continue;
            }
            else {
                mFile->Unread(c);
                if (index >= 13) {
                    mString = builder.ToString();
                    return Token::UUID_NUMBER;
                }
                else return Token::ILLEGAL_TOKEN;
            }
        }

    }
}

Tokenizer::Token Tokenizer::ReadVersionNumberToken()
{
    static constexpr int START = 0;
    static constexpr int SEGMENT_1 = 1;
    static constexpr int SEGMENT_2 = 2;
    static constexpr int SEGMENT_3 = 3;
    static constexpr int END = 4;

    StringBuilder builder;

    int c, state = START;
    while ((c = mFile->Read()) != -1) {
        if (state == START) {
            mTokenLineNo = mFile->GetLineNo();
            mTokenColumnNo = mFile->GetColumnNo();
            if (IsEscape(c)) {
                continue;
            }
            else if (IsDecimalDigital(c)) {
                builder.Append((char)c);
                state = SEGMENT_1;
                continue;
            }
            else {
                mFile->Unread(c);
                return Token::ILLEGAL_TOKEN;
            }
        }
        else if (state == SEGMENT_1) {
            if (IsDecimalDigital(c)) {
                builder.Append((char)c);
                state = SEGMENT_1;
                continue;
            }
            else if (c == '.') {
                builder.Append((char)c);
                state = SEGMENT_2;
                continue;
            }
            else {
                if (!IsEscape(c)) {
                    mFile->Unread(c);
                }
                return Token::ILLEGAL_TOKEN;
            }
        }
        else if (state == SEGMENT_2) {
            if (IsDecimalDigital(c)) {
                builder.Append((char)c);
                state = SEGMENT_2;
                continue;
            }
            else if (c == '.') {
                builder.Append((char)c);
                state = SEGMENT_3;
                continue;
            }
            else {
                if (!IsEscape(c)) {
                    mFile->Unread(c);
                }
                return Token::ILLEGAL_TOKEN;
            }
        }
        else if (state == SEGMENT_3) {
            if (IsDecimalDigital(c)) {
                builder.Append((char)c);
                state = END;
                continue;
            }
            else {
                if (!IsEscape(c)) {
                    mFile->Unread(c);
                }
                return Token::ILLEGAL_TOKEN;
            }
        }
        else if (state == END) {
            if (IsDecimalDigital(c)) {
                builder.Append((char)c);
                state = END;
                continue;
            }
            else {
                if (!IsEscape(c)) {
                    mFile->Unread(c);
                }
                mString = builder.ToString();
                break;
            }
        }
    }

    return state == END ? Token::VERSION_NUMBER : Token::ILLEGAL_TOKEN;
}

Tokenizer::Token Tokenizer::ReadCharacter(
    /* [in] */ int c)
{
    c = mFile->Read();
    if (c == -1) return Token::END_OF_LINE;

    mCharacter = c;

    if (mFile->Peek() != '\'') {
        if (mCharacter == '\'') {
            mCharacter = 0;
            mString = "";
            return Token::CHARACTER;
        }
        return Token::ILLEGAL_TOKEN;
    }
    mFile->Read();
    mString = String::Format("%c", mCharacter);
    return Token::CHARACTER;
}

Tokenizer::Token Tokenizer::ReadIdentifier(
    /* [in] */ int c)
{
    StringBuilder builder;

    builder.Append((char)c);
    while ((c = mFile->Read()) != -1) {
        if (IsAlphabet(c) || c == '_' || c== '-' || IsDecimalDigital(c)) {
            builder.Append((char)c);
            continue;
        }
        else {
            if (c == ':' && mFile->Peek() == ':') {
                mFile->Read();
                builder.Append("::");
                continue;
            }
            if (!IsEscape(c)) {
                mFile->Unread(c);
            }
            break;
        }
    }
    String key = builder.ToString();
    Token token = mKeywords.Get(key);
    if (token == Token::ILLEGAL_TOKEN) {
        mIdentifier = key;
        token = Token::IDENTIFIER;
    }
    return token;
}

Tokenizer::Token Tokenizer::ReadNumber(
    /* [in] */ int c)
{
    static constexpr int NUMBER_INT = 0;
    static constexpr int NUMBER_INT_0 = 1;
    static constexpr int NUMBER_FP = 2;
    static constexpr int NUMBER_FP_EXP = 3;

    StringBuilder builder;

    builder.Append((char)c);
    mBit = 32;
    int radix = c == '0' ? 8 : 10;
    int state = c == '0' ? NUMBER_INT_0 : NUMBER_INT;
    while ((c = mFile->Read()) != -1) {
        if (state == NUMBER_INT_0) {
            if (c == 'x' || c == 'X') {
                builder.Append((char)c);
                radix = 16;
                state = NUMBER_INT;
                continue;
            }
            else if (IsOctalDigital(c)) {
                builder.Append((char)c);
                state = NUMBER_INT;
                continue;
            }
            else if (c == '.') {
                builder.Append((char)c);
                state = NUMBER_FP;
                continue;
            }
            else if (!IsEscape(c)) {
                mFile->Unread(c);
            }
            break;
        }
        else if (state == NUMBER_INT) {
            if ((radix == 10 && IsDecimalDigital(c)) ||
                (radix == 16 && IsHexDigital(c))) {
                builder.Append((char)c);
                continue;
            }
            else if (c == '.') {
                builder.Append((char)c);
                state = NUMBER_FP;
                continue;
            }
            else if (c == 'l') {
                if (mFile->Peek() == 'l') {
                    mFile->Read();
                    mBit = 64;
                }
                break;
            }
            else if (c == 'L') {
                if (mFile->Peek() == 'L') {
                    mFile->Read();
                    mBit = 64;
                }
                break;
            }
            else if (c == 'e' || c == 'E') {
                builder.Append((char)c);
                state = NUMBER_FP_EXP;
                continue;
            }
            else if (!IsEscape(c)) {
                mFile->Unread(c);
            }
            break;
        }
        else if (state == NUMBER_FP) {
            if (IsDecimalDigital(c)) {
                builder.Append((char)c);
                continue;
            }
            else if (c == 'e' || c == 'E') {
                builder.Append((char)c);
                state = NUMBER_FP_EXP;
                continue;
            }
            break;
        }
        else if (state == NUMBER_FP_EXP) {
            if (c == '+' || c == '-') {
                builder.Append((char)c);
                continue;
            }
            else if (IsDecimalDigital(c)) {
                builder.Append((char)c);
                continue;
            }
            else if (c == 'f' || c == 'F') {
                break;
            }
            else if (c == 'd' || c == 'D') {
                mBit = 64;
                break;
            }
            else if (!IsEscape(c)) {
                mFile->Unread(c);
            }
            break;
        }
    }
    mNumberString = builder.ToString();
    if (state == NUMBER_INT_0 || state == NUMBER_INT) {
        mIntegralValue = strtoll(mNumberString.string(), NULL, radix);
        return Token::NUMBER_INTEGRAL;
    }
    else {
        mFloatingPointValue = atof(mNumberString.string());
        return Token::NUMBER_FLOATINGPOINT;
    }
}

Tokenizer::Token Tokenizer::ReadStringLiteral(
    /* [in] */ int c)
{
    StringBuilder builder;

    while ((c = mFile->Read()) != -1) {
        if (c != '"') {
            builder.Append((char)c);
        }
        else {
            mString = builder.ToString();
            return Token::STRING_LITERAL;
        }
    }
    return Token::END_OF_FILE;
}

Tokenizer::Token Tokenizer::ReadLineComment(
    /* [in] */ int c)
{
    StringBuilder builder;

    builder.Append((char)c);
    c = mFile->Read();
    while (c != '\n' && c != -1) {
        builder.Append((char)c);
        c = mFile->Read();
    }
    mComment = builder.ToString();
    return Token::COMMENT_LINE;
}

Tokenizer::Token Tokenizer::ReadBlockComment(
    /* [in] */ int c)
{
    StringBuilder builder;

    builder.Append((char)c);
    while ((c = mFile->Read()) != -1) {
        builder.Append((char)c);
        if (c == '*' && mFile->Peek() == '/') {
            mFile->Read();
            builder.Append('/');
            break;
        }
    }

    mComment = builder.ToString();
    return Token::COMMENT_BLOCK;
}

bool Tokenizer::IsAlphabet(
    /* [in] */ int c)
{
    return (('a' <= c && c <= 'z') ||
            ('A' <= c && c <= 'Z'));
}

bool Tokenizer::IsOctalDigital(
        /* [in] */ int c)
{
    return ('0' <= c && c <= '7');
}

bool Tokenizer::IsDecimalDigital(
    /* [in] */ int c)
{
    return ('0' <= c && c <= '9');
}

bool Tokenizer::IsHexDigital(
    /* [in] */ int c)
{
    return (('0' <= c && c <= '9') ||
            ('a' <= c && c <= 'f') ||
            ('A' <= c && c <= 'F'));
}

bool Tokenizer::IsEscape(
    /* [in] */ int c)
{
    return (c == ' ' || c == '\n' || c == '\t');
}

const char* Tokenizer::DumpToken(
    /* [in] */ Token token)
{
    switch (token) {
        case Token::AND:
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
            return mString.string();
        case Token::COCLASS:
            return "coclass";
        case Token::COLON:
            return ":";
        case Token::COMMA:
            return ",";
        case Token::COMMENT_BLOCK:
            return mComment.string();
        case Token::COMMENT_LINE:
            return mComment.string();
        case Token::COMPLIMENT:
            return "~";
        case Token::CONST:
            return "const";
        case Token::CONSTRUCTOR:
            return "constructor";
        case Token::DESCRIPTION:
            return "description";
        case Token::DIVIDE:
            return "/";
        case Token::DOUBLE:
            return "Double";
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
            return mIdentifier.string();
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
        case Token::NUMBER_INTEGRAL:
            return mNumberString.string();
        case Token::NUMBER_FLOATINGPOINT:
            return mNumberString.string();
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
            return mString.string();
        case Token::TRUE:
            return "true";
        case Token::URL:
            return "url";
        case Token::UUID:
            return "uuid";
        case Token::UUID_NUMBER:
            return mString.string();
        case Token::VERSION:
            return "version";
        case Token::VERSION_NUMBER:
            return mString.string();
        default:
            return "";
    }
}

}
