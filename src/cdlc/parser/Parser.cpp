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

#include "parser/Parser.h"
#include "parser/Token.h"
#include "util/AutoPtr.h"
#include "util/Logger.h"
#include "util/MemoryFileReader.h"
#include "util/Properties.h"
#include <cstdlib>
#include <unistd.h>

namespace cdlc {

const char* Parser::TAG = "Parser";

bool Parser::Parse(
    /* [in] */ const String& filePath)
{
    char* cwd = getcwd(nullptr, 0);
    Properties::Get().AddSearchPath(cwd);
    free(cwd);

    AutoPtr<MemoryFileReader> reader = new MemoryFileReader(filePath);
    if (!reader->ReadIn(true)) {
        return false;
    }

    mTokenizer.SetReader(reader);

    return ParseModule();
}

bool Parser::ParseFile(
    /* [in] */ const String& filePath)
{
    AutoPtr<MemoryFileReader> reader = new MemoryFileReader(filePath);
    if (!reader->ReadIn(false)) {
        return false;
    }

    mTokenizer.SetReader(reader);

    bool result = true;
    Token token = Token::UNKNOW;
    while ((token = mTokenizer.PeekToken()) != Token::END_OF_FILE) {
        switch (token) {
            case Token::BRACKETS_OPEN:
                result = ParseDeclarationWithAttributes() && result;
                continue;
            default: {
                String message = String::Format("%s is not expected.",
                        mTokenizer.DumpCurrentToken().string());
                LogError(token, message);
                mTokenizer.GetToken();
                continue;
            }
        }
    }

    return result;
}

bool Parser::ParseDeclarationWithAttributes()
{
    Attributes attrs;
    bool result = ParseAttributes(attrs);
}

bool Parser::ParseAttributes(
    /* [out] */ Attributes& attrs)
{
    Token token = Token::UNKNOW;
    bool result = true;

    // read '['
    token = mTokenizer.PeekToken();
    if (token == Token::BRACKETS_OPEN) {
        mTokenizer.GetToken();
        token = mTokenizer.PeekToken();
        while (token != Token::BRACKETS_CLOSE &&
                token != Token::END_OF_FILE) {
            switch (token) {
                case Token::UUID: {
                    result = ParseUuid(attrs) && result;
                    break;
                }
                case Token::VERSION: {
                    result = ParseVersion(attrs) && result;
                    break;
                }
                case Token::DESCRIPTION: {
                    break;
                }
                case Token::URL: {
                    break;
                }
                default: {
                    String message = String::Format("\"%s\" is not expected.",
                            mTokenizer.DumpCurrentToken().string());
                    LogError(token, message);
                    // jump to ',' or ']'
                    while (token != Token::COMMA && token != Token::BRACKETS_CLOSE &&
                            token != Token::END_OF_FILE) {
                        mTokenizer.GetToken();
                        token = mTokenizer.PeekToken();
                    }
                    break;
                }
            }
            token = mTokenizer.PeekToken();
            if (token == Token::COMMA) {
                mTokenizer.GetToken();
                token = mTokenizer.PeekToken();
            }
            else if (token != Token::BRACKETS_CLOSE) {
                LogError(token, "\",\" or \"]\" is expected.");
                return false;
            }
        }
        if (token == Token::END_OF_FILE) {
            LogError(token, "\"]\" is expected.");
            return false;
        }
        // read ']'
        mTokenizer.GetToken();
        return result;
    }
    else {
        LogError(token, "\"[\" is expected.");
        return false;
    }
}

bool Parser::ParseUuid(
    /* [out] */ Attributes& attrs)
{
    Token token = Token::UNKNOW;

    // read "uuid"
    mTokenizer.GetToken();
    token = mTokenizer.PeekToken();
    if (token != Token::PARENTHESES_OPEN) {
        LogError(token, "\"(\" is expected.");
        return false;
    }
    mTokenizer.GetToken();
    token = mTokenizer.GetUuidNumberToken();
    if (token != Token::UUID_NUMBER) {
        LogError(token, "uuid number is expected.");
        return false;
    }
    attrs.mUuid = mTokenizer.GetUuid();
    token = mTokenizer.PeekToken();
    if (token != Token::PARENTHESES_CLOSE) {
        LogError(token, "\")\" is expected.");
        return false;
    }
    return true;
}

bool Parser::ParseVersion(
    /* [out] */ Attributes& attrs)
{
    Token token = Token::UNKNOW;

    // read "version"
    mTokenizer.GetToken();
    token = mTokenizer.PeekToken();
    if (token != Token::PARENTHESES_OPEN) {
        LogError(token, "\"(\" is expected.");
        return false;
    }
    mTokenizer.GetToken();
    token = mTokenizer.GetVersionNumberToken();
    if (token != Token::VERSION_NUMBER) {
        LogError(token, "version number is expected.");
        return false;
    }
    attrs.mVersion = mTokenizer.GetVersionNumber();
    token = mTokenizer.PeekToken();
    if (token != Token::PARENTHESES_CLOSE) {
        LogError(token, "\")\" is expected.");
        return false;
    }
    return true;
}


bool Parser::ParseModule()
{
    Attributes attrs;
    bool result = ParseAttributes(attrs);


    return result;
}



void Parser::LogError(
    /* [in] */ Token token,
    /* [in] */ const String& message)
{
    Error error;
    String file = mTokenizer.GetTokenFilePath();
    error.mFile = file.Substring(file.LastIndexOf('/') + 1);
    error.mLineNo = mTokenizer.GetTokenLineNo();
    error.mColumnNo = mTokenizer.GetTokenColumnNo();
    error.mMessage = message;

    mErrors.push_back(std::move(error));
}

void Parser::ShowErrors()
{
    for (Error& error : mErrors) {
        Logger::E(TAG, "%s[line %d, column %d] %s",
                error.mFile.string(),
                error.mLineNo,
                error.mColumnNo,
                error.mMessage.string());
    }
}

}
