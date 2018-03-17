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
#include "util/Logger.h"

namespace ccm {

struct KV
{
    String mKey;
    Tokenizer::Token mValue;
}
sKeywords[24] =
{
    { String("Array"), Tokenizer::Token::ARRAY },
    { String("Boolean"), Tokenizer::Token::BOOLEAN },
    { String("Byte"), Tokenizer::Token::BYTE },
    { String("callee"), Tokenizer::Token::CALLEE },
    { String("Char"), Tokenizer::Token::CHAR },
    { String("coclass"), Tokenizer::Token::COCLASS },
    { String("const"), Tokenizer::Token::CONST },
    { String("description"), Tokenizer::Token::DESCRIPTION },
    { String("Double"), Tokenizer::Token::DOUBLE },
    { String("enum"), Tokenizer::Token::ENUM },
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
    , mNumber(0)
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
    mFile = mFileStack != nullptr? mFileStack->mFile : nullptr;
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

Tokenizer::Token Tokenizer::GetEndOfLineToken()
{
    if (mHasAPeek) {
        mHasAPeek = false;
        return mCurrToken;
    }
    return ReadEndOfLineToken();
}

Tokenizer::Token Tokenizer::GetStringLiteralToken()
{
    if (mHasAPeek) {
        mHasAPeek = false;
        return mCurrToken;
    }
    return ReadStringLiteralToken();
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

Tokenizer::Token Tokenizer::ReadToken()
{
    int c;
    while ((c = mFile->Read()) != -1) {
        mTokenLineNo = mFile->GetLineNo();
        mTokenColumnNo = mFile->GetColumnNo();
        if (IsEscape(c)) {
            continue;
        }
        else if (IsAlphabet(c) || c == '_') {
            return ReadIdentifier(c);
        }
        else if (IsDecimalDigital(c)) {
            return ReadNumber(c);
        }
        switch (c) {
            case '<':
                return Token::ANGLE_BRACKETS_OPEN;
            case '>':
                return Token::ANGLE_BRACKETS_CLOSE;
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
                return Token::DOUBLE_QUOTES;
            case '-':
                return Token::HYPHEN;
            case '(':
                return Token::PARENTHESES_OPEN;
            case ')':
                return Token::PARENTHESES_CLOSE;
            case '.':
                return Token::PERIOD;
            case ';':
                return Token::SEMICOLON;
            default:
                // todo:
                continue;
        }
    }
    return Token::END_OF_FILE;
}

Tokenizer::Token Tokenizer::ReadEndOfLineToken()
{
    if (mFile->Read() == '\n') return Token::END_OF_LINE;
    return Token::ILLEGAL_TOKEN;
}

Tokenizer::Token Tokenizer::ReadStringLiteralToken()
{
    int c = mFile->Read();
    if (c != '"') return Token::ILLEGAL_TOKEN;

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

Tokenizer::Token Tokenizer::ReadUuidNumberToken()
{
    static const int START = 0;
    static const int SEGMENT_1 = 1;
    static const int SEGMENT_2 = 2;
    static const int SEGMENT_3 = 3;
    static const int SEGMENT_4 = 4;

    StringBuilder builder;

    int state = START;
    int c, index = 0;
    while ((c = mFile->Read()) != -1) {
        index++;
        if (state == START) {
            if (index == 1) {
                mTokenLineNo = mFile->GetLineNo();
                mTokenColumnNo = mFile->GetColumnNo();
            }
            if (c == '-') {
                if (index == 9) {
                    builder.Append((char)c);
                    state = SEGMENT_1;
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
        else if (state == SEGMENT_1) {
            if (c == '-') {
                if (index == 5) {
                    builder.Append((char)c);
                    state = SEGMENT_2;
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
    StringBuilder builder;

    if (PeekToken() != Token::NUMBER) {
        return Token::ILLEGAL_TOKEN;
    }
    Tokenizer::Token token = GetToken();
    builder.Append(mNumberString);

    int c = mFile->Read();
    if (c != '.') {
        mFile->Unread(c);
        return Token::ILLEGAL_TOKEN;
    }
    builder.Append('.');

    if (PeekToken() != Token::NUMBER) {
        return Token::ILLEGAL_TOKEN;
    }
    token = GetToken();
    builder.Append(mNumberString);

    c = mFile->Read();
    if (c != '.') {
        mFile->Unread(c);
        return Token::ILLEGAL_TOKEN;
    }
    builder.Append('.');

    if (PeekToken() != Token::NUMBER) {
        return Token::ILLEGAL_TOKEN;
    }
    token = GetToken();
    builder.Append(mNumberString);

    mString = builder.ToString();
    return Token::VERSION_NUMBER;
}

Tokenizer::Token Tokenizer::ReadIdentifier(
    /* [in] */ int c)
{
    StringBuilder builder;

    builder.Append((char)c);
    while ((c = mFile->Read()) != -1) {
        if (IsAlphabet(c) || c == '_' || IsDecimalDigital(c)) {
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
    static const int START = 0;
    static const int NUMBER = 1;
    static const int NUMBER_PREFIX_0 = 2;
    static const int NUMBER_POSTFIX_l = 3;

    StringBuilder builder;

    builder.Append((char)c);
    mBit = 32;
    int radix = c == '0'? 8 : 10;
    int state = c == '0'? NUMBER_PREFIX_0 : NUMBER;
    while ((c = mFile->Read()) != -1) {
        if (state == NUMBER_PREFIX_0) {
            if (c == 'x' || c == 'X') {
                builder.Append((char)c);
                radix = 16;
                state = NUMBER;
                continue;
            }
            else if (IsOctalDigital(c)) {
                builder.Append((char)c);
                state = NUMBER;
                continue;
            }
            if (!IsEscape(c)) {
                mFile->Unread(c);
            }
            break;
        }
        else if (state = NUMBER) {
            if ((radix == 10 && IsDecimalDigital(c)) ||
                (radix == 16 && IsHexDigital(c))) {
                builder.Append((char)c);
                continue;
            }
            else if (c == 'l' || c == 'L') {
                state = NUMBER_POSTFIX_l;
                continue;
            }
            else {
                if (!IsEscape(c)) {
                    mFile->Unread(c);
                }
                break;
            }
        }
        else if (state == NUMBER_POSTFIX_l) {
            if (c == 'l' || c == 'L') {
                mBit = 64;
            }
            else if (!IsEscape(c)) {
                mFile->Unread(c);
            }
            break;
        }
    }
    mNumberString = builder.ToString();
    mNumber = strtoll(mNumberString.string(), NULL, radix);
    return Token::NUMBER;
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
        case Token::CONST:
            return "const";
        case Token::DESCRIPTION:
            return "description";
        case Token::DIVIDE:
            return "/";
        case Token::DOUBLE:
            return "Double";
        case Token::DOUBLE_QUOTES:
            return "\"";
        case Token::END_OF_LINE:
            return "\n";
        case Token::ENUM:
            return "enum";
        case Token::FLOAT:
            return "Float";
        case Token::HANDLE:
            return "HANDLE";
        case Token::HYPHEN:
            return "-";
        case Token::IDENTIFIER:
            return mIdentifier.string();
        case Token::IN:
            return "in";
        case Token::INCLUDE:
            return "include";
        case Token::INTEGER:
            return "Integer";
        case Token::INTERFACE:
            return "interface";
        case Token::LONG:
            return "Long";
        case Token::MODULE:
            return "Module";
        case Token::NAMESPACE:
            return "namespace";
        case Token::NUMBER:
            return mNumberString.string();
        case Token::OUT:
            return "out";
        case Token::PARENTHESES_OPEN:
            return "(";
        case Token::PARENTHESES_CLOSE:
            return ")";
        case Token::PERIOD:
            return ".";
        case Token::SEMICOLON:
            return ";";
        case Token::SHORT:
            return "Short";
        case Token::STRING:
            return "String";
        case Token::STRING_LITERAL:
            return mString.string();
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
