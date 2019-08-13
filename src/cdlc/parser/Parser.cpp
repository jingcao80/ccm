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
#include "parser/TokenInfo.h"
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

    bool ret = ParseFile(filePath);
    if (!ret) {
        ShowErrors();
    }

    return ret;
}

bool Parser::ParseFile(
    /* [in] */ const String& filePath)
{
    TokenInfo tokenInfo;
    tokenInfo.mStringValue = filePath;
    return ParseFile(tokenInfo);
}

bool Parser::ParseFile(
    /* [in] */ TokenInfo tokenInfo)
{
    String filePath = tokenInfo.mStringValue;
    AutoPtr<MemoryFileReader> reader = new MemoryFileReader(filePath);
    if (!reader->ReadIn(false)) {
        String message = String::Format("Fail to open the file \"%s\".",
                filePath.string());
        LogError(tokenInfo, message);
        return false;
    }

    mTokenizer.SetReader(reader);

    bool result = true;
    while (tokenInfo = mTokenizer.PeekToken(), tokenInfo.mToken != Token::END_OF_FILE) {
        switch (tokenInfo.mToken) {
            case Token::BRACKETS_OPEN: {
                result = ParseDeclarationWithAttributes(false) && result;
                continue;
            }
            default: {
                String message = String::Format("%s is not expected.",
                        TokenInfo::Dump(tokenInfo).string());
                LogError(tokenInfo, message);
                mTokenizer.GetToken();
                result = false;
                continue;
            }
        }
    }

    return result;
}

bool Parser::ParseDeclarationWithAttributes(
    /* [in] */ bool excludeModule)
{
    Attributes attrs;
    bool result = ParseAttributes(attrs);

    TokenInfo tokenInfo = mTokenizer.PeekToken();
    switch (tokenInfo.mToken) {
        case Token::COCLASS: {
            result = ParseCoclass(attrs) && result;
            break;
        }
        case Token::INTERFACE: {
            result = ParseInterface(attrs) && result;
            break;
        }
        case Token::MODULE: {
            if (excludeModule) {
                String message = String::Format("%s is not expected.",
                        TokenInfo::Dump(tokenInfo).string());
                LogError(tokenInfo, message);
                result = false;
                break;
            }
            result = ParseModule(attrs) && result;
            break;
        }
    }

    return result;
}

bool Parser::ParseAttributes(
    /* [out] */ Attributes& attrs)
{
    bool result = true;

    // read '['
    TokenInfo tokenInfo = mTokenizer.PeekToken();
    if (tokenInfo.mToken == Token::BRACKETS_OPEN) {
        mTokenizer.GetToken();
        tokenInfo = mTokenizer.PeekToken();
        while (tokenInfo.mToken != Token::BRACKETS_CLOSE &&
                tokenInfo.mToken != Token::END_OF_FILE) {
            switch (tokenInfo.mToken) {
                case Token::UUID: {
                    result = ParseUuid(attrs) && result;
                    break;
                }
                case Token::VERSION: {
                    result = ParseVersion(attrs) && result;
                    break;
                }
                case Token::DESCRIPTION: {
                    result = ParseDescription(attrs) && result;
                    break;
                }
                case Token::URI: {
                    result = ParseUri(attrs) && result;
                    break;
                }
                default: {
                    String message = String::Format("\"%s\" is not expected.",
                            TokenInfo::Dump(tokenInfo).string());
                    LogError(tokenInfo, message);
                    result = false;
                    break;
                }
            }
            tokenInfo = mTokenizer.PeekToken();
            if (tokenInfo.mToken == Token::COMMA) {
                mTokenizer.GetToken();
                tokenInfo = mTokenizer.PeekToken();
            }
            else if (tokenInfo.mToken != Token::BRACKETS_CLOSE) {
                LogError(tokenInfo, "\",\" or \"]\" is expected.");
                return false;
            }
        }
        if (tokenInfo.mToken == Token::END_OF_FILE) {
            LogError(tokenInfo, "\"]\" is expected.");
            return false;
        }
        // read ']'
        mTokenizer.GetToken();
        return result;
    }
    else {
        LogError(tokenInfo, "\"[\" is expected.");
        return false;
    }
}

bool Parser::ParseUuid(
    /* [out] */ Attributes& attrs)
{
    // read "uuid"
    mTokenizer.GetToken();
    TokenInfo tokenInfo = mTokenizer.PeekToken();
    if (tokenInfo.mToken != Token::PARENTHESES_OPEN) {
        LogError(tokenInfo, "\"(\" is expected.");
        return false;
    }
    mTokenizer.GetToken();
    tokenInfo = mTokenizer.GetUuidNumberToken();
    if (tokenInfo.mToken != Token::UUID_NUMBER) {
        LogError(tokenInfo, "uuid number is expected.");
        return false;
    }
    attrs.mUuid = tokenInfo.mStringValue;
    tokenInfo = mTokenizer.PeekToken();
    if (tokenInfo.mToken != Token::PARENTHESES_CLOSE) {
        LogError(tokenInfo, "\")\" is expected.");
        return false;
    }
    mTokenizer.GetToken();
    return true;
}

bool Parser::ParseVersion(
    /* [out] */ Attributes& attrs)
{
    // read "version"
    mTokenizer.GetToken();
    TokenInfo tokenInfo = mTokenizer.PeekToken();
    if (tokenInfo.mToken != Token::PARENTHESES_OPEN) {
        LogError(tokenInfo, "\"(\" is expected.");
        return false;
    }
    mTokenizer.GetToken();
    tokenInfo = mTokenizer.GetVersionNumberToken();
    if (tokenInfo.mToken != Token::VERSION_NUMBER) {
        LogError(tokenInfo, "version number is expected.");
        return false;
    }
    attrs.mVersion = tokenInfo.mStringValue;
    tokenInfo = mTokenizer.PeekToken();
    if (tokenInfo.mToken != Token::PARENTHESES_CLOSE) {
        LogError(tokenInfo, "\")\" is expected.");
        return false;
    }
    mTokenizer.GetToken();
    return true;
}

bool Parser::ParseDescription(
    /* [out] */ Attributes& attrs)
{
    // read "description"
    mTokenizer.GetToken();
    TokenInfo tokenInfo = mTokenizer.PeekToken();
    if (tokenInfo.mToken != Token::PARENTHESES_OPEN) {
        LogError(tokenInfo, "\"(\" is expected.");
        return false;
    }
    mTokenizer.GetToken();
    tokenInfo = mTokenizer.PeekToken();
    if (tokenInfo.mToken != Token::STRING_LITERAL) {
        LogError(tokenInfo, "string literal is expected.");
        return false;
    }
    mTokenizer.GetToken();
    attrs.mDescription = tokenInfo.mStringValue;
    tokenInfo = mTokenizer.PeekToken();
    if (tokenInfo.mToken != Token::PARENTHESES_CLOSE) {
        LogError(tokenInfo, "\")\" is expected.");
        return false;
    }
    mTokenizer.GetToken();
    return true;
}

bool Parser::ParseUri(
    /* [out] */ Attributes& attrs)
{
    // read "uri"
    mTokenizer.GetToken();
    TokenInfo tokenInfo = mTokenizer.PeekToken();
    if (tokenInfo.mToken != Token::PARENTHESES_OPEN) {
        LogError(tokenInfo, "\"(\" is expected.");
        return false;
    }
    mTokenizer.GetToken();
    tokenInfo = mTokenizer.PeekToken();
    if (tokenInfo.mToken != Token::STRING_LITERAL) {
        LogError(tokenInfo, "string literal is expected.");
        return false;
    }
    mTokenizer.GetToken();
    attrs.mUri = tokenInfo.mStringValue;
    tokenInfo = mTokenizer.PeekToken();
    if (tokenInfo.mToken != Token::PARENTHESES_CLOSE) {
        LogError(tokenInfo, "\")\" is expected.");
        return false;
    }
    mTokenizer.GetToken();
    return true;
}

bool Parser::ParseModule(
    /* [in] */ Attributes& attrs)
{
    bool result = true;
    String moduleName;

    // read "module"
    mTokenizer.GetToken();
    TokenInfo tokenInfo = mTokenizer.PeekToken();
    if (tokenInfo.mToken == Token::IDENTIFIER) {
        mTokenizer.GetToken();
        moduleName = tokenInfo.mStringValue;
        if (attrs.mUuid.IsEmpty() || attrs.mUri.IsEmpty()) {
            String message = String::Format("Module %s should have attributes.",
                    moduleName.string());
            LogError(tokenInfo, message);
            result = false;
        }
        tokenInfo = mTokenizer.PeekToken();
    }
    else {
        LogError(tokenInfo, "Identifier as the module name is expected.");
        result = false;
    }

    if (tokenInfo.mToken != Token::BRACES_OPEN) {
        LogError(tokenInfo, "\"{\" is expected.");
        return false;
    }

    // read '{'
    mTokenizer.GetToken();

    tokenInfo = mTokenizer.PeekToken();
    while (tokenInfo.mToken != Token::BRACES_CLOSE &&
            tokenInfo.mToken != Token::END_OF_FILE) {
        switch (tokenInfo.mToken) {
            case Token::BRACKETS_OPEN: {
                result = ParseDeclarationWithAttributes(true) && result;
                break;
            }
            case Token::INCLUDE: {
                result = ParseInclude() && result;
                break;
            }
            case Token::NAMESPACE: {
                result = ParseNamespace() && result;
                break;
            }
            default: {
                String message = String::Format("%s is not expected.",
                        TokenInfo::Dump(tokenInfo).string());
                LogError(tokenInfo, message);
                mTokenizer.GetToken();
                result = false;
                break;
            }
        }
        tokenInfo = mTokenizer.PeekToken();
    }
    if (tokenInfo.mToken == Token::END_OF_FILE) {
        LogError(tokenInfo, "\"}\" is expected.");
        return false;
    }
    // read '}'
    mTokenizer.GetToken();

    return result;
}

bool Parser::ParseNamespace()
{
    bool result = true;
    String namespaceName;

    // read "namespace"
    mTokenizer.GetToken();
    TokenInfo tokenInfo = mTokenizer.PeekToken();
    if (tokenInfo.mToken == Token::IDENTIFIER) {
        mTokenizer.GetToken();
        namespaceName = tokenInfo.mStringValue;
        tokenInfo = mTokenizer.PeekToken();
    }
    else {
        LogError(tokenInfo, "Identifier as the namespace name is expected.");
        result = false;
    }

    if (tokenInfo.mToken != Token::BRACES_OPEN) {
        LogError(tokenInfo, "\"{\" is expected.");
        return false;
    }

    // read '{'
    mTokenizer.GetToken();

    tokenInfo = mTokenizer.PeekToken();
    while (tokenInfo.mToken != Token::BRACES_CLOSE &&
            tokenInfo.mToken != Token::END_OF_FILE) {
        switch (tokenInfo.mToken) {
            case Token::BRACKETS_OPEN: {
                result = ParseDeclarationWithAttributes(true) && result;
                break;
            }
            case Token::NAMESPACE: {
                result = ParseNamespace() && result;
                break;
            }
            default: {
                String message = String::Format("%s is not expected.",
                        TokenInfo::Dump(tokenInfo).string());
                LogError(tokenInfo, message);
                mTokenizer.GetToken();
                result = false;
                break;
            }
        }
        tokenInfo = mTokenizer.PeekToken();
    }
    if (tokenInfo.mToken == Token::END_OF_FILE) {
        LogError(tokenInfo, "\"}\" is expected.");
        return false;
    }
    // read '}'
    mTokenizer.GetToken();

    return result;
}

bool Parser::ParseInterface(
    /* [in] */ Attributes& attrs)
{
    bool result = true;
    String interfaceName;

    // read "interface"
    mTokenizer.GetToken();
    TokenInfo tokenInfo = mTokenizer.PeekToken();
    if (tokenInfo.mToken == Token::IDENTIFIER) {
        mTokenizer.GetToken();
        interfaceName = tokenInfo.mStringValue;
    }
    else {
        LogError(tokenInfo, "An interface name is expected.");
        result = false;
    }

    if (mTokenizer.PeekToken().mToken == Token::SEMICOLON) {
        // interface forward declaration
        mTokenizer.GetToken();

        return result;
    }

    // interface definition
    if (attrs.mUuid.IsEmpty() || attrs.mVersion.IsEmpty()) {
        String message = String::Format("Interface %s should have attributes.", interfaceName.string());
        LogError(tokenInfo, message);
        result = false;
    }

    if (mTokenizer.PeekToken().mToken == Token::COLON) {
        // parent interface
    }
    else {

    }

    result = ParseInterfaceBody() && result;

    return result;
}

bool Parser::ParseInterfaceBody()
{
    bool result = true;

    TokenInfo tokenInfo = mTokenizer.PeekToken();
    if (tokenInfo.mToken != Token::BRACES_OPEN) {
        LogError(tokenInfo, "\"{\" is expected.");
        result = false;
    }
    mTokenizer.GetToken();

    tokenInfo = mTokenizer.PeekToken();
    while (tokenInfo.mToken != Token::BRACES_CLOSE &&
            tokenInfo.mToken != Token::END_OF_FILE) {
        switch (tokenInfo.mToken) {
            case Token::BRACKETS_OPEN: {
                result = ParseNestedInterface() && result;
                break;
            }
            case Token::IDENTIFIER: {
                result = ParseMethod() && result;
                break;
            }
            default: {
                LogError(tokenInfo, "Methods or constants are expected.");
                mTokenizer.GetToken();
                result = false;
                break;
            }
        }
        tokenInfo = mTokenizer.PeekToken();
    }
    if (tokenInfo.mToken == Token::END_OF_FILE) {
        LogError(tokenInfo, "\"}\" is expected.");
        return false;
    }
    // read '}'
    mTokenizer.GetToken();

    return result;
}

bool Parser::ParseMethod()
{
    bool result = true;

    TokenInfo tokenInfo = mTokenizer.GetToken();

    String methodName = tokenInfo.mStringValue;

    tokenInfo = mTokenizer.PeekToken();
    if (tokenInfo.mToken != Token::PARENTHESES_OPEN) {
        LogError(tokenInfo, "\"(\" is expected.");
        result = false;
    }
    mTokenizer.GetToken();

    tokenInfo = mTokenizer.PeekToken();
    while (tokenInfo.mToken != Token::PARENTHESES_CLOSE &&
            tokenInfo.mToken != Token::END_OF_FILE) {
        result = ParseParameter() && result;
        if (!result) {
            break;
        }
        tokenInfo = mTokenizer.PeekToken();
        if (tokenInfo.mToken == Token::COMMA) {
            mTokenizer.GetToken();
            tokenInfo = mTokenizer.PeekToken();
        }
        else if (tokenInfo.mToken != Token::PARENTHESES_CLOSE) {
            LogError(tokenInfo, "\",\" or \")\" is expected.");
            break;
        }
    }
    if (tokenInfo.mToken == Token::END_OF_FILE) {
        LogError(tokenInfo, "\")\" is expected.");
        return false;
    }
    // read ')'
    mTokenizer.GetToken();

    tokenInfo = mTokenizer.PeekToken();
    if (tokenInfo.mToken != Token::SEMICOLON) {
        LogError(tokenInfo, "\";\" is expected.");
        return false;
    }
    mTokenizer.GetToken();

    return result;
}

bool Parser::ParseParameter()
{
    bool result = true;

    TokenInfo tokenInfo = mTokenizer.PeekToken();
    if (tokenInfo.mToken != Token::BRACKETS_OPEN) {
        LogError(tokenInfo, "\"[\" is expected.");
        result = false;
    }
    mTokenizer.GetToken();

    tokenInfo = mTokenizer.PeekToken();
    while (tokenInfo.mToken != Token::BRACKETS_CLOSE &&
            tokenInfo.mToken != Token::END_OF_FILE) {
        switch (tokenInfo.mToken) {
            case Token::IN: {
                mTokenizer.GetToken();
                break;
            }
            case Token::OUT: {
                mTokenizer.GetToken();
                break;
            }
            case Token::CALLEE: {
                mTokenizer.GetToken();
                break;
            }
            default: {
                String message = String::Format("%s is not expected.",
                        TokenInfo::Dump(tokenInfo).string());
                LogError(tokenInfo, message);
                result = false;
                break;
            }
        }
        tokenInfo = mTokenizer.PeekToken();
        if (tokenInfo.mToken == Token::COMMA) {
            mTokenizer.GetToken();
            tokenInfo = mTokenizer.PeekToken();
        }
        else if (tokenInfo.mToken != Token::BRACKETS_CLOSE) {
            LogError(tokenInfo, "\",\" or \"]\" is expected.");
            break;
        }
    }
    if (tokenInfo.mToken == Token::END_OF_FILE) {
        LogError(tokenInfo, "\"]\" is expected.");
        return false;
    }
    // read ']'
    mTokenizer.GetToken();

    ParseType();

    tokenInfo = mTokenizer.PeekToken();
    if (tokenInfo.mToken != Token::IDENTIFIER) {
        LogError(tokenInfo, "Parameter name is expected.");
        result = false;
    }
    mTokenizer.GetToken();

    String parameterName = tokenInfo.mStringValue;

    if (mTokenizer.PeekToken().mToken == Token::ASSIGNMENT) {
        mTokenizer.GetToken();
    }

    return result;
}

void Parser::ParseType()
{
    TokenInfo tokenInfo = mTokenizer.PeekToken();
    if (tokenInfo.IsBuildinType()) {
        mTokenizer.GetToken();
    }
    else if (tokenInfo.mToken == Token::IDENTIFIER) {
        mTokenizer.GetToken();
    }
    else if (tokenInfo.mToken == Token::ARRAY) {
        ParseArray();
    }

    tokenInfo = mTokenizer.PeekToken();
    while (tokenInfo.mToken == Token::ASTERISK) {
        mTokenizer.GetToken();
        tokenInfo = mTokenizer.PeekToken();
    }

    if (tokenInfo.mToken == Token::AMPERSAND) {
        mTokenizer.GetToken();
    }
}

void Parser::ParseArray()
{
    // read "Array"
    mTokenizer.GetToken();

    TokenInfo tokenInfo = mTokenizer.PeekToken();
    if (tokenInfo.mToken != Token::ANGLE_BRACKETS_OPEN) {
        LogError(tokenInfo, "\"<\" is expected.");
        return;
    }
    mTokenizer.GetToken();

    ParseType();

    tokenInfo = mTokenizer.PeekToken(Token::ANGLE_BRACKETS_CLOSE);
    if (tokenInfo.mToken != Token::ANGLE_BRACKETS_CLOSE) {
        LogError(tokenInfo, "\">\" is expected.");
        return;
    }
    mTokenizer.GetToken();
}

bool Parser::ParseNestedInterface()
{
    Attributes attrs;
    bool result = ParseAttributes(attrs);

    TokenInfo tokenInfo = mTokenizer.PeekToken();
    if (tokenInfo.mToken != Token::INTERFACE) {
        String message = String::Format("%s is not expected.", TokenInfo::Dump(tokenInfo).string());
        LogError(tokenInfo, message);
        result = false;
    }

    result = ParseInterface(attrs) && result;

    return result;
}

bool Parser::ParseCoclass(
    /* [in] */ Attributes& attrs)
{
    bool result = true;
    String className;

    // read "coclass"
    mTokenizer.GetToken();
    TokenInfo tokenInfo = mTokenizer.PeekToken();
    if (tokenInfo.mToken == Token::IDENTIFIER) {
        mTokenizer.GetToken();
        className = tokenInfo.mStringValue;
    }
    else {
        LogError(tokenInfo, "A coclass name is expected.");
        result = false;
    }

    if (attrs.mUuid.IsEmpty() || attrs.mVersion.IsEmpty()) {
        String message = String::Format("Coclass %s should have attributes.", className.string());
        LogError(tokenInfo, message);
        result = false;
    }

    result = ParseCoclassBody() && result;

    return result;
}

bool Parser::ParseCoclassBody()
{
    bool result = true;

    TokenInfo tokenInfo = mTokenizer.PeekToken();
    if (tokenInfo.mToken != Token::BRACES_OPEN) {
        LogError(tokenInfo, "\"{\" is expected.");
        result = false;
    }
    mTokenizer.GetToken();

    tokenInfo = mTokenizer.PeekToken();
    while (tokenInfo.mToken != Token::BRACES_CLOSE &&
            tokenInfo.mToken != Token::END_OF_FILE) {
        switch (tokenInfo.mToken) {
            case Token::CONSTRUCTOR: {
                result = ParseConstructor() && result;
                break;
            }
            case Token::INTERFACE: {
                result = ParseInterface(nullptr) && result;
                break;
            }
            default: {
                LogError(tokenInfo, "Constructors or interfaces are expected.");
                mTokenizer.GetToken();
                result = false;
                break;
            }
        }
        tokenInfo = mTokenizer.PeekToken();
    }
    if (tokenInfo.mToken == Token::END_OF_FILE) {
        LogError(tokenInfo, "\"}\" is expected.");
        return false;
    }
    // read '}'
    mTokenizer.GetToken();

    return result;
}

bool Parser::ParseConstructor()
{
    bool result = true;

    // read "Constructor"
    mTokenizer.GetToken();
    TokenInfo tokenInfo = mTokenizer.PeekToken();
    if (tokenInfo.mToken != Token::PARENTHESES_OPEN) {
        LogError(tokenInfo, "\"(\" is expected.");
        result = false;
    }
    mTokenizer.GetToken();

    tokenInfo = mTokenizer.PeekToken();
    while (tokenInfo.mToken != Token::PARENTHESES_CLOSE &&
            tokenInfo.mToken != Token::END_OF_FILE) {
        result = ParseParameter() && result;
        if (!result) {
            break;
        }
        tokenInfo = mTokenizer.PeekToken();
        if (tokenInfo.mToken == Token::COMMA) {
            mTokenizer.GetToken();
            tokenInfo = mTokenizer.PeekToken();
        }
        else if (tokenInfo.mToken != Token::PARENTHESES_CLOSE) {
            LogError(tokenInfo, "\",\" or \")\" is expected.");
            break;
        }
    }
    if (tokenInfo.mToken == Token::END_OF_FILE) {
        LogError(tokenInfo, "\")\" is expected.");
        return false;
    }
    // read ')'
    mTokenizer.GetToken();

    tokenInfo = mTokenizer.PeekToken();
    if (tokenInfo.mToken == Token::ASSIGNMENT) {
        mTokenizer.GetToken();
        tokenInfo = mTokenizer.PeekToken();
        if (tokenInfo.mToken == Token::DELETE) {
            mTokenizer.GetToken();

            tokenInfo = mTokenizer.PeekToken();
        }
        else {
            String message = String::Format("%s is not expected.",
                    TokenInfo::Dump(tokenInfo).string());
            LogError(tokenInfo, message);
            result = false;
        }
    }

    if (tokenInfo.mToken == Token::AMPERSAND) {
        mTokenizer.GetToken();

        tokenInfo = mTokenizer.PeekToken();
    }

    if (tokenInfo.mToken != Token::SEMICOLON) {
        LogError(tokenInfo, "\";\" is expected.");
        return false;
    }
    mTokenizer.GetToken();

    return result;
}

bool Parser::ParseInterface(
    /* [in] */ void* arg)
{
    bool result = true;

    // read "interface"
    mTokenizer.GetToken();
    TokenInfo tokenInfo = mTokenizer.PeekToken();
    if (tokenInfo.mToken != Token::IDENTIFIER) {
        LogError(tokenInfo, "An interface name is expected.");
        result = false;
    }
    mTokenizer.GetToken();

    String interfaceName = tokenInfo.mStringValue;

    tokenInfo = mTokenizer.PeekToken();
    if (tokenInfo.mToken != Token::SEMICOLON) {
        LogError(tokenInfo, "\";\" is expected.");
        result = false;
    }
    mTokenizer.GetToken();

    return result;
}

bool Parser::ParseInclude()
{
    // read "include"
    mTokenizer.GetToken();
    TokenInfo tokenInfo = mTokenizer.PeekToken();
    if (tokenInfo.mToken != Token::STRING_LITERAL) {
        LogError(tokenInfo, "A file path is expected.");
        return false;
    }
    mTokenizer.GetToken();

    bool ret = ParseFile(tokenInfo);

    return ret;
}

void Parser::LogError(
    /* [in] */ TokenInfo& tokenInfo,
    /* [in] */ const String& message)
{
    Error error;
    String file = tokenInfo.mTokenFilePath;
    error.mFile = file.Substring(file.LastIndexOf('/') + 1);
    error.mLineNo = tokenInfo.mTokenLineNo;
    error.mColumnNo = tokenInfo.mTokenColumnNo;
    error.mMessage = message;

    mErrors.push_back(std::move(error));
}

void Parser::ShowErrors()
{
    for (Error& error : mErrors) {
        if (!error.mFile.IsEmpty()) {
            Logger::E(TAG, "%s[line %d, column %d] %s",
                    error.mFile.string(),
                    error.mLineNo,
                    error.mColumnNo,
                    error.mMessage.string());
        }
        else {
            Logger::E(TAG, "%s", error.mMessage.string());
        }
    }
}

}
