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

#include "Parser.h"
#include "util/Logger.h"

namespace ccm {

const String Parser::TAG("Parser");

int Parser::Parse(
    /* [in] */ const std::shared_ptr<File>& file)
{
    mFile = file;

    if (!mFile->IsValid()) {
        Logger::E(TAG, "File %s is invalid.", mFile->GetPath().string());
        return E_FILE_NOT_FOUND_EXCEPTION;
    }

    mTokenizer.SetInputFile(file);

    mComponent = new Component(mFile->GetPath());
    mCurrNamespace = new Namespace(String("__global__"));
    mComponent->AddNamespace(mCurrNamespace);
    return ParseFile();
}

Parser::~Parser()
{
    if (mComponent != nullptr) {
        delete mComponent;
        mComponent = nullptr;
    }
    mCurrNamespace = nullptr;

    mCurrError = mErrorHeader;
    while (mCurrError != nullptr) {
        mErrorHeader = mCurrError->mNext;
        delete mCurrError;
        mCurrError = mErrorHeader;
    }
    mErrorHeader = mCurrError = nullptr;
}

int Parser::ParseFile()
{
    Tokenizer::Token token;
    while ((token = mTokenizer.GetToken()) != Tokenizer::Token::END_OF_FILE) {
        switch (token) {
            case Tokenizer::Token::BRACKETS_OPEN:
                ParseDeclarationWithAttribute();
                continue;
            case Tokenizer::Token::COCLASS:
                ParseCoclass();
                continue;
            case Tokenizer::Token::ENUM:
                ParseEnumeration();
                continue;
            case Tokenizer::Token::INCLUDE:
                ParseInclude();
                continue;
            case Tokenizer::Token::INTERFACE:
                ParseInterface();
                continue;
            case Tokenizer::Token::NAMESPACE:
                ParseNamespace();
                continue;
            default: {
                String message = String::Format("%s is not expected.", mTokenizer.DumpToken(token));
                LogError(token, message);
                continue;
            }
        }
    }
    return 0;
}

bool Parser::ParseAttribute(
    /* [in] */ String* uuid,
    /* [in] */ String* version,
    /* [in] */ String* description)
{
    bool parseResult = true;
    Tokenizer::Token token = mTokenizer.GetToken();
    while (token != Tokenizer::Token::BRACKETS_CLOSE &&
            token != Tokenizer::Token::END_OF_FILE) {
        if (token == Tokenizer::Token::UUID) {
            token = mTokenizer.GetToken();
            if (token != Tokenizer::Token::PARENTHESES_OPEN) {
                LogError(token, String("\"(\" is expected."));
                mTokenizer.UngetToken(token);
                parseResult = false;
            }
            token = mTokenizer.GetUuidNumberToken();
            if (token != Tokenizer::Token::UUID_NUMBER) {
                LogError(token, String("uuid number is expected."));
                mTokenizer.UngetToken(token);
                parseResult = false;
            }
            else {
                *uuid = mTokenizer.GetNumberString();
            }
            token = mTokenizer.GetToken();
            if (token != Tokenizer::Token::PARENTHESES_OPEN) {
                LogError(token, String("\")\" is expected."));
                mTokenizer.UngetToken(token);
                parseResult = false;
            }
        }
        else if (token == Tokenizer::Token::VERSION) {
            token = mTokenizer.GetToken();
            if (token != Tokenizer::Token::PARENTHESES_OPEN) {
                LogError(token, String("\"(\" is expected."));
                mTokenizer.UngetToken(token);
                parseResult = false;
            }
            token = mTokenizer.GetVersionNumberToken();
            if (token != Tokenizer::Token::VERSION_NUMBER) {
                LogError(token, String("version number is expected."));
                mTokenizer.UngetToken(token);
                parseResult = false;
            }
            else {
                *version = mTokenizer.GetString();
            }
            token = mTokenizer.GetToken();
            if (token != Tokenizer::Token::PARENTHESES_OPEN) {
                LogError(token, String("\")\" is expected."));
                mTokenizer.UngetToken(token);
                parseResult = false;
            }
        }
        else if (token == Tokenizer::Token::DESCRIPTION) {
            token = mTokenizer.GetToken();
            if (token != Tokenizer::Token::PARENTHESES_OPEN) {
                LogError(token, String("\"(\" is expected."));
                mTokenizer.UngetToken(token);
                parseResult = false;
            }
            token = mTokenizer.GetStringLiteralToken();
            if (token != Tokenizer::Token::STRING_LITERAL) {
                LogError(token, String("version number is expected."));
                mTokenizer.UngetToken(token);
                parseResult = false;
            }
            else {
                *description = mTokenizer.GetString();
            }
            token = mTokenizer.GetToken();
            if (token != Tokenizer::Token::PARENTHESES_OPEN) {
                LogError(token, String("\")\" is expected."));
                mTokenizer.UngetToken(token);
                parseResult = false;
            }
        }
        token = mTokenizer.GetToken();
        if (token == Tokenizer::Token::COMMA) {
            token = mTokenizer.GetToken();
        }
        else if (token != Tokenizer::Token::BRACKETS_CLOSE) {
            LogError(token, String("\",\" is expected."));
            mTokenizer.UngetToken(token);
            parseResult = false;
            break;
        }
    }
    return parseResult;
}

bool Parser::ParseDeclarationWithAttribute()
{
    String uuid, version, description;
    bool parseResult = ParseAttribute(&uuid, &version, &description);

    Tokenizer::Token token = mTokenizer.GetToken();

    return parseResult;
}

int Parser::ParseCoclass()
{
    // todo:
    return 0;
}

bool Parser::ParseEnumeration()
{
    Tokenizer::Token token;
    bool parseResult = true;

    Enumeration* enumeration = new Enumeration();

    token = mTokenizer.GetToken();
    if (token == Tokenizer::Token::IDENTIFIER) {
        enumeration->SetName(mTokenizer.GetIdentifier());
        enumeration->SetNamespace(mCurrNamespace);
    }
    else {
        LogError(token, String("Identifier as enumeration name is expected."));
        mTokenizer.UngetToken(token);
        parseResult = false;
    }

    parseResult = parseResult && ParseEnumerationBody(enumeration);

    if (parseResult) {
        mComponent->AddEnumeration(enumeration);
    }
    else {
        delete enumeration;
    }

    return parseResult;
}

bool Parser::ParseEnumerationBody(
    /* [in] */ Enumeration* enumeration)
{
    Tokenizer::Token token;
    bool parseResult = true;

    token = mTokenizer.GetToken();
    if (token != Tokenizer::Token::BRACES_OPEN) {
        LogError(token, String("\" { \" is expected."));
        mTokenizer.UngetToken(token);
        parseResult = false;
    }

    token = mTokenizer.GetToken();
    int value = 0;
    while (token != Tokenizer::Token::BRACES_CLOSE &&
            token != Tokenizer::Token::END_OF_FILE ) {
        String name;
        if (token == Tokenizer::Token::IDENTIFIER) {
            name = mTokenizer.GetIdentifier();
        }
        else {
            LogError(token, String("Identifier as enumeration name is expected."));
            mTokenizer.UngetToken(token);
            parseResult = false;
        }
        token = mTokenizer.GetToken();
        if (token == Tokenizer::Token::ASSIGNMENT) {
            parseResult = parseResult && ParseExpression(&value);
            token = mTokenizer.GetToken();
        }
        if (token == Tokenizer::Token::COMMA) {
            token = mTokenizer.GetToken();
        }
        else if (token != Tokenizer::Token::BRACES_CLOSE) {
            LogError(token, String("\",\" is expected."));
            mTokenizer.UngetToken(token);
            parseResult = false;
            break;
        }
        enumeration->AddEnumerator(name, value++);
    }
    return parseResult;
}

bool Parser::ParseExpression(
    /* [out] */ int* value)
{
    // todo:
    return false;
}

int Parser::ParseInclude()
{
    // todo:
    return 0;
}

int Parser::ParseInterface()
{
    // todo:
    return 0;
}

int Parser::ParseNamespace()
{
    // todo:
    return 0;
}

void Parser::LogError(
    /* [in] */ Tokenizer::Token token,
    /* [in] */ const String& message)
{
    int lineNo = mTokenizer.GetTokenLineNo();
    int columeNo = mTokenizer.GetTokenColumnNo();

    Logger::E(TAG, "[Line %d, Column %d] %s", lineNo, columeNo, message.string());

    if (mErrorHeader == nullptr) {
        mErrorHeader = new Error();
        mErrorHeader->mErrorToken = token;
        mErrorHeader->mLineNo = lineNo;
        mErrorHeader->mColumnNo = columeNo;
        mErrorHeader->mMessage = message;
        mCurrError = mErrorHeader;
        return;
    }

    Error* error = new Error();
    error->mErrorToken = token;
    error->mLineNo = lineNo;
    error->mColumnNo = columeNo;
    error->mMessage = message;
    mCurrError->mNext = error;
    mCurrError = error;
}

}
