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
#include "ast/EnumerationType.h"
#include "parser/TokenInfo.h"
#include "phase/BuildinTypeBuilder.h"
#include "util/AutoPtr.h"
#include "util/Logger.h"
#include "util/MemoryFileReader.h"
#include "util/Properties.h"
#include "util/UUID.h"
#include <cstdlib>
#include <unistd.h>

namespace cdlc {

const char* Parser::TAG = "Parser";

Parser::Parser()
{
    mBeforePhases.push_back(new BuildinTypeBuilder(mWorld));
}

void Parser::AddPhase(
    /* [in] */ Phase* phase)
{
    if (phase != nullptr) {
        mAfterPhases.push_back(phase);
    }
}

bool Parser::Parse(
    /* [in] */ const String& filePath)
{
    char* cwd = getcwd(nullptr, 0);
    Properties::Get().AddSearchPath(cwd);
    free(cwd);

    Prepare();

    bool ret = ParseFile(filePath);
    if (!ret) {
        ShowErrors();
    }

    return ret;
}

void Parser::Prepare()
{
    for (AutoPtr<Phase> phase : mBeforePhases) {
        phase->Process();
    }
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
    tokenInfo = mTokenizer.PeekToken();
    while (tokenInfo.mToken != Token::END_OF_FILE) {
        switch (tokenInfo.mToken) {
            case Token::BRACKETS_OPEN: {
                result = ParseDeclarationWithAttributes(false) && result;
                break;
            }
            case Token::COCLASS: {
                Attributes attrs;
                result = ParseCoclass(attrs) && result;
                break;
            }
            case Token::CONST: {
                ParseConstant();
                break;
            }
            case Token::ENUM: {
                result = ParseEnumeration() && result;
                break;
            }
            case Token::INCLUDE: {
                result = ParseInclude() && result;
                break;
            }
            case Token::INTERFACE: {
                Attributes attrs;
                result = ParseInterface(attrs) && result;
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
    mTokenizer.GetToken();

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
        default: {
            String message = String::Format("%s is not expected.",
                    TokenInfo::Dump(tokenInfo).string());
            LogError(tokenInfo, message);
            result = false;
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
            if (!result) {
                // jump to ',' or ']'
                while (tokenInfo.mToken != Token::COMMA &&
                        tokenInfo.mToken != Token::BRACKETS_CLOSE &&
                        tokenInfo.mToken != Token::END_OF_FILE) {
                    mTokenizer.GetToken();
                    tokenInfo = mTokenizer.PeekToken();
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
            mTokenizer.GetToken();
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
    bool result = true;

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
    if (!UUID::IsValid(attrs.mUuid)) {
        LogError(tokenInfo, "uuid number is not valid.");
        result = false;
    }
    tokenInfo = mTokenizer.PeekToken();
    if (tokenInfo.mToken != Token::PARENTHESES_CLOSE) {
        LogError(tokenInfo, "\")\" is expected.");
        return false;
    }
    mTokenizer.GetToken();
    return result;
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

    mModule = mWorld.GetWorkingModule();
    mModule->SetName(moduleName);
    mCurrentNamespace = mModule->FindNamespace(Namespace::GLOBAL_NAME);

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
            case Token::COCLASS: {
                Attributes attrs;
                result = ParseCoclass(attrs) && result;
                break;
            }
            case Token::CONST: {
                ParseConstant();
                break;
            }
            case Token::ENUM: {
                result = ParseEnumeration() && result;
                break;
            }
            case Token::INCLUDE: {
                result = ParseInclude() && result;
                break;
            }
            case Token::INTERFACE: {
                Attributes attrs;
                result = ParseInterface(attrs) && result;
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
        mTokenizer.GetToken();
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
            case Token::COCLASS: {
                Attributes attrs;
                result = ParseCoclass(attrs) && result;
                break;
            }
            case Token::CONST: {
                ParseConstant();
                break;
            }
            case Token::ENUM: {
                result = ParseEnumeration() && result;
                break;
            }
            case Token::INCLUDE: {
                result = ParseInclude() && result;
                break;
            }
            case Token::INTERFACE: {
                Attributes attrs;
                result = ParseInterface(attrs) && result;
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
        mTokenizer.GetToken();
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
            case Token::CONST: {
                ParseConstant();
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
        mTokenizer.GetToken();
        return false;
    }
    // read '}'
    mTokenizer.GetToken();

    return result;
}

AutoPtr<Constant> Parser::ParseConstant()
{
    AutoPtr<Type> type;

    // read "const"
    mTokenizer.GetToken();
    TokenInfo tokenInfo = mTokenizer.PeekToken();
    if (tokenInfo.IsBuildinType()) {
        mTokenizer.GetToken();
        type = mWorld.FindType(String::Format("como::%s", TokenInfo::Dump(tokenInfo).string()));
    }
    else {
        // enumeration
        mTokenizer.GetToken();
        AutoPtr<EnumerationType> enumeration;
        String typeName = tokenInfo.mStringValue;
        AutoPtr<Namespace> ns = mCurrentNamespace;
        while (ns != nullptr) {
            String fullTypeName = ns->ToString() + "::" + typeName;
            enumeration = mWorld.FindEnumeration(fullTypeName);
            if (enumeration != nullptr) {
                type = (Type*)enumeration.Get();
                break;
            }
            ns = ns->GetParent();
        }
        if (enumeration == nullptr) {
            String message = String::Format("Type \"%s\" is not declared.", typeName.string());
            LogError(tokenInfo, message);
            mTokenizer.SkipCurrentLine();
            return nullptr;
        }
    }

    AutoPtr<Constant> constant = new Constant();
    constant->SetType(type);

    tokenInfo = mTokenizer.GetToken();
    if (tokenInfo.mToken != Token::IDENTIFIER) {
        LogError(tokenInfo, "A constant name is expected.");
        mTokenizer.SkipCurrentLine();
        return nullptr;
    }

    constant->SetName(tokenInfo.mStringValue);

    tokenInfo = mTokenizer.GetToken();
    if (tokenInfo.mToken != Token::ASSIGNMENT) {
        LogError(tokenInfo, "\"=\" is expected.");
        mTokenizer.SkipCurrentLine();
        return nullptr;
    }

    AutoPtr<Expression> expr = ParseExpression(type);
    if (expr == nullptr) {
        return nullptr;
    }
    constant->SetValue(expr);

    tokenInfo = mTokenizer.GetToken();
    if (tokenInfo.mToken != Token::SEMICOLON) {
        LogError(tokenInfo, "\";\" is expected.");
        mTokenizer.SkipCurrentLine();
        return nullptr;
    }

    return constant;
}

AutoPtr<Expression> Parser::ParseExpression(
    /* [in] */ Type* type)
{
    return ParseInclusiveOrExpression(type);
}

AutoPtr<InclusiveOrExpression> Parser::ParseInclusiveOrExpression(
    /* [in] */ Type* type)
{
    AutoPtr<ExclusiveOrExpression> rightOperand = ParseExclusiveOrExpression(type);
    if (rightOperand == nullptr) {
        return nullptr;
    }

    AutoPtr<InclusiveOrExpression> expr = new InclusiveOrExpression();
    expr->SetRightOperand(rightOperand);
    expr->SetType(rightOperand->GetType());
    expr->SetRadix(rightOperand->GetRadix());
    expr->SetScientificNotation(rightOperand->IsScientificNotation());

    TokenInfo tokenInfo = mTokenizer.PeekToken();
    while (tokenInfo.mToken == Token::INCLUSIVE_OR) {
        mTokenizer.GetToken();

        rightOperand = ParseExclusiveOrExpression(type);
        if (rightOperand == nullptr) {
            return nullptr;
        }

        AutoPtr<InclusiveOrExpression> leftOperand = expr;

        if (!leftOperand->GetType()->IsIntegralType() ||
                !rightOperand->GetType()->IsIntegralType()) {
            LogError(tokenInfo, "Inclusive or operation can not be applied "
                    "to non-integral type.");
            return nullptr;
        }

        expr = new InclusiveOrExpression();
        expr->SetLeftOperand(leftOperand);
        expr->SetRightOperand(rightOperand);
        expr->SetType(ChooseType(leftOperand->GetType(), rightOperand->GetType()));

        tokenInfo = mTokenizer.PeekToken();
    }

    return expr;
}

AutoPtr<ExclusiveOrExpression> Parser::ParseExclusiveOrExpression(
    /* [in] */ Type* type)
{
    AutoPtr<AndExpression> rightOperand = ParseAndExpression(type);
    if (rightOperand == nullptr) {
        return nullptr;
    }

    AutoPtr<ExclusiveOrExpression> expr = new ExclusiveOrExpression();
    expr->SetRightOperand(rightOperand);
    expr->SetType(rightOperand->GetType());
    expr->SetRadix(rightOperand->GetRadix());
    expr->SetScientificNotation(rightOperand->IsScientificNotation());

    TokenInfo tokenInfo = mTokenizer.PeekToken();
    while (tokenInfo.mToken == Token::EXCLUSIVE_OR) {
        mTokenizer.GetToken();

        rightOperand = ParseAndExpression(type);
        if (rightOperand == nullptr) {
            return nullptr;
        }

        AutoPtr<ExclusiveOrExpression> leftOperand = expr;

        if (!leftOperand->GetType()->IsIntegralType() ||
                !rightOperand->GetType()->IsIntegralType()) {
            LogError(tokenInfo, "Exclusive or operation can not be applied "
                    "to non-integral type.");
            return nullptr;
        }

        expr = new ExclusiveOrExpression();
        expr->SetLeftOperand(leftOperand);
        expr->SetRightOperand(rightOperand);
        expr->SetType(ChooseType(leftOperand->GetType(), rightOperand->GetType()));

        tokenInfo = mTokenizer.PeekToken();
    }

    return expr;
}

AutoPtr<AndExpression> Parser::ParseAndExpression(
    /* [in] */ Type* type)
{
    AutoPtr<ShiftExpression> rightOperand = ParseShiftExpression(type);
    if (rightOperand == nullptr) {
        return nullptr;
    }

    AutoPtr<AndExpression> expr = new AndExpression();
    expr->SetRightOperand(rightOperand);
    expr->SetType(rightOperand->GetType());
    expr->SetRadix(rightOperand->GetRadix());
    expr->SetScientificNotation(rightOperand->IsScientificNotation());

    TokenInfo tokenInfo = mTokenizer.PeekToken();
    while (tokenInfo.mToken == Token::AMPERSAND) {
        mTokenizer.GetToken();

        rightOperand = ParseShiftExpression(type);
        if (rightOperand == nullptr) {
            return nullptr;
        }

        AutoPtr<AndExpression> leftOperand = expr;

        if (!leftOperand->GetType()->IsIntegralType() ||
                !rightOperand->GetType()->IsIntegralType()) {
            LogError(tokenInfo, "And operation can not be applied "
                    "to non-integral type.");
            return nullptr;
        }

        expr = new AndExpression();
        expr->SetLeftOperand(leftOperand);
        expr->SetRightOperand(rightOperand);
        expr->SetType(ChooseType(leftOperand->GetType(), rightOperand->GetType()));

        tokenInfo = mTokenizer.PeekToken();
    }

    return expr;
}

AutoPtr<ShiftExpression> Parser::ParseShiftExpression(
    /* [in] */ Type* type)
{
    AutoPtr<AdditiveExpression> rightOperand = ParseAdditiveExpression(type);
    if (rightOperand == nullptr) {
        return nullptr;
    }

    AutoPtr<ShiftExpression> expr = new ShiftExpression();
    expr->SetRightOperand(rightOperand);
    expr->SetType(rightOperand->GetType());
    expr->SetRadix(rightOperand->GetRadix());
    expr->SetScientificNotation(rightOperand->IsScientificNotation());

    TokenInfo tokenInfo = mTokenizer.PeekToken();
    while (tokenInfo.mToken == Token::SHIFT_LEFT ||
            tokenInfo.mToken == Token::SHIFT_RIGHT ||
            tokenInfo.mToken == Token::SHIFT_RIGHT_UNSIGNED) {
        mTokenizer.GetToken();

        rightOperand = ParseAdditiveExpression(type);
        if (rightOperand == nullptr) {
            return nullptr;
        }

        AutoPtr<ShiftExpression> leftOperand = expr;

        if (!leftOperand->GetType()->IsIntegralType() ||
                !rightOperand->GetType()->IsIntegralType()) {
            LogError(tokenInfo, "Shift operation can not be applied "
                    "to non-integral type.");
            return nullptr;
        }

        expr = new ShiftExpression();
        expr->SetLeftOperand(leftOperand);
        expr->SetRightOperand(rightOperand);
        expr->SetOperator(tokenInfo.mToken == Token::SHIFT_LEFT
                ? Expression::OPERATOR_LEFT_SHIFT
                : tokenInfo.mToken == Token::SHIFT_RIGHT
                    ? Expression::OPERATOR_RIGHT_SHIFT
                    : Expression::OPERATOR_UNSIGNED_RIGHT_SHIFT);
        expr->SetType(ChooseType(leftOperand->GetType(), rightOperand->GetType()));

        tokenInfo = mTokenizer.PeekToken();
    }

    return expr;
}

AutoPtr<AdditiveExpression> Parser::ParseAdditiveExpression(
    /* [in] */ Type* type)
{
    AutoPtr<MultiplicativeExpression> rightOperand = ParseMultiplicativeExpression(type);
    if (rightOperand == nullptr) {
        return nullptr;
    }

    AutoPtr<AdditiveExpression> expr = new AdditiveExpression();
    expr->SetRightOperand(rightOperand);
    expr->SetType(rightOperand->GetType());
    expr->SetRadix(rightOperand->GetRadix());
    expr->SetScientificNotation(rightOperand->IsScientificNotation());

    TokenInfo tokenInfo = mTokenizer.PeekToken();
    while (tokenInfo.mToken == Token::PLUS ||
            tokenInfo.mToken == Token::MINUS) {
        mTokenizer.GetToken();

        rightOperand = ParseMultiplicativeExpression(type);
        if (rightOperand == nullptr) {
            return nullptr;
        }

        AutoPtr<AdditiveExpression> leftOperand = expr;

        if (!leftOperand->GetType()->IsNumericType() ||
                !rightOperand->GetType()->IsNumericType()) {
            LogError(tokenInfo, "Additive operation can not be applied "
                    "to non-numeric type.");
            return nullptr;
        }

        expr = new AdditiveExpression();
        expr->SetLeftOperand(leftOperand);
        expr->SetRightOperand(rightOperand);
        expr->SetOperator(tokenInfo.mToken == Token::PLUS
                ? Expression::OPERATOR_PLUS
                : Expression::OPERATOR_MINUS);
        expr->SetType(ChooseType(leftOperand->GetType(), rightOperand->GetType()));

        tokenInfo = mTokenizer.PeekToken();
    }

    return expr;
}

AutoPtr<MultiplicativeExpression> Parser::ParseMultiplicativeExpression(
    /* [in] */ Type* type)
{
    AutoPtr<UnaryExpression> rightOperand = ParseUnaryExpression(type);
    if (rightOperand == nullptr) {
        return nullptr;
    }

    AutoPtr<MultiplicativeExpression> expr = new MultiplicativeExpression();
    expr->SetRightOperand(rightOperand);
    expr->SetType(rightOperand->GetType());
    expr->SetRadix(rightOperand->GetRadix());
    expr->SetScientificNotation(rightOperand->IsScientificNotation());

    TokenInfo tokenInfo = mTokenizer.PeekToken();
    while (tokenInfo.mToken == Token::ASTERISK ||
            tokenInfo.mToken == Token::DIVIDE ||
            tokenInfo.mToken == Token::MODULO) {
        mTokenizer.GetToken();

        rightOperand = ParseUnaryExpression(type);
        if (rightOperand == nullptr) {
            return nullptr;
        }

        AutoPtr<MultiplicativeExpression> leftOperand = expr;

        if (!leftOperand->GetType()->IsNumericType() ||
                !rightOperand->GetType()->IsNumericType()) {
            LogError(tokenInfo, "Multiplicative operation can not be applied "
                    "to non-numeric type.");
            return nullptr;
        }

        expr = new MultiplicativeExpression();
        expr->SetLeftOperand(leftOperand);
        expr->SetRightOperand(rightOperand);
        expr->SetOperator(tokenInfo.mToken == Token::ASTERISK
                ? Expression::OPERATOR_MULTIPLE
                : tokenInfo.mToken == Token::DIVIDE
                    ? Expression::OPERATOR_DIVIDE
                    : Expression::OPERATOR_MODULO);
        expr->SetType(ChooseType(leftOperand->GetType(), rightOperand->GetType()));

        tokenInfo = mTokenizer.PeekToken();
    }

    return expr;
}

AutoPtr<UnaryExpression> Parser::ParseUnaryExpression(
    /* [in] */ Type* type)
{
    TokenInfo tokenInfo = mTokenizer.PeekToken();
    if (tokenInfo.mToken == Token::PLUS ||
            tokenInfo.mToken == Token::MINUS ||
            tokenInfo.mToken == Token::COMPLIMENT ||
            tokenInfo.mToken == Token::NOT) {
        mTokenizer.GetToken();

        AutoPtr<UnaryExpression> rightOperand = ParseUnaryExpression(type);
        if (rightOperand == nullptr) {
            return nullptr;
        }

        if ((tokenInfo.mToken == Token::PLUS || tokenInfo.mToken == Token::MINUS ||
                tokenInfo.mToken == Token::NOT) && (!rightOperand->GetType()->IsNumericType())) {
            LogError(tokenInfo, "Plus, minus and not operation can not be applied to"
                    "non-numeric type.");
            return nullptr;
        }
        else if (tokenInfo.mToken == Token::COMPLIMENT && !rightOperand->GetType()->IsIntegralType()) {
            LogError(tokenInfo, "Compliment operation can not be applied to"
                    "non-integral type.");
            return nullptr;
        }

        AutoPtr<UnaryExpression> expr = new UnaryExpression();
        expr->SetRightOperand(rightOperand);
        expr->SetOperator(tokenInfo.mToken == Token::PLUS
                ? Expression::OPERATOR_POSITIVE
                : tokenInfo.mToken == Token::MINUS
                    ? Expression::OPERATOR_NEGATIVE
                    : tokenInfo.mToken == Token::COMPLIMENT
                        ? Expression::OPERATOR_COMPLIMENT
                        : Expression::OPERATOR_NOT);
        expr->SetType(rightOperand->GetType());

        return expr;
    }
    else {
        AutoPtr<PostfixExpression> leftOperand = ParsePostfixExpression(type);
        if (leftOperand == nullptr) {
            return nullptr;
        }

        AutoPtr<UnaryExpression> expr = new UnaryExpression();
        expr->SetLeftOperand(leftOperand);
        expr->SetType(leftOperand->GetType());
        expr->SetRadix(leftOperand->GetRadix());
        expr->SetScientificNotation(leftOperand->IsScientificNotation());

        return expr;
    }
}

AutoPtr<PostfixExpression> Parser::ParsePostfixExpression(
    /* [in] */ Type* type)
{
    TokenInfo tokenInfo = mTokenizer.PeekToken();
    switch (tokenInfo.mToken) {
        case Token::TRUE:
        case Token::FALSE: {
            return ParseBooleanLiteral(type);
        }
        case Token::CHARACTER: {
            return ParseCharacter(type);
        }
        case Token::NUMBER_INTEGRAL: {
            return ParseIntegralNumber(type);
        }
        case Token::NUMBER_FLOATINGPOINT: {
            return ParseFloatingPointNumber(type);
        }
        case Token::STRING_LITERAL: {
            return ParseStringLiteral(type);
        }
        case Token::IDENTIFIER: {
            return ParseIdentifier(type);
        }
        case Token::NULLPTR: {
            mTokenizer.GetToken();
            if (type->IsPointerType()) {
                AutoPtr<PostfixExpression> expr = new PostfixExpression();
                expr->SetType(type);
                expr->SetIntegralValue(0);
                expr->SetRadix(16);
                return expr;
            }

            String message = String::Format("\"nullptr\" can not be assigned to \"%s\" type.",
                    type->GetName().string());
            LogError(tokenInfo, message);
            return nullptr;
        }
        case Token::PARENTHESES_OPEN: {
            AutoPtr<Expression> nestedExpr = ParseExpression(type);

            tokenInfo = mTokenizer.PeekToken();
            if (tokenInfo.mToken != Token::PARENTHESES_CLOSE) {
                LogError(tokenInfo, "\")\" is expected.");
                return nullptr;
            }
            mTokenizer.GetToken();

            AutoPtr<PostfixExpression> expr = new PostfixExpression();
            expr->SetType(type);
            expr->SetExpression(nestedExpr);
            return expr;
        }
        default: {
            String message = String::Format("%s is not expected.",
                    TokenInfo::Dump(tokenInfo).string());
            LogError(tokenInfo, message);
            return nullptr;
        }
    }
}

AutoPtr<PostfixExpression> Parser::ParseBooleanLiteral(
    /* [in] */ Type* type)
{
    TokenInfo tokenInfo = mTokenizer.GetToken();
    if (type->IsBooleanType()) {
        AutoPtr<PostfixExpression> expr = new PostfixExpression();
        expr->SetType(type);
        expr->SetBooleanValue(tokenInfo.mToken == Token::TRUE
                ? true : false);
        return expr;
    }

    String message = String::Format("\"%s\" can not be assigned to \"%s\" type.",
            TokenInfo::Dump(tokenInfo).string(), type->GetName().string());
    LogError(tokenInfo, message);
    return nullptr;
}

AutoPtr<PostfixExpression> Parser::ParseCharacter(
    /* [in] */ Type* type)
{
    TokenInfo tokenInfo = mTokenizer.GetToken();
    if (type->IsNumericType()) {
        AutoPtr<PostfixExpression> expr = new PostfixExpression();
        expr->SetType(type);
        expr->SetIntegralValue(tokenInfo.mCharValue);
        return expr;
    }

    LogError(tokenInfo, "Character can not be assigned to non-numeric type.");
    return nullptr;
}

AutoPtr<PostfixExpression> Parser::ParseIntegralNumber(
    /* [in] */ Type* type)
{
    TokenInfo tokenInfo = mTokenizer.GetToken();
    if (type->IsNumericType() || type->IsEnumerationType() || type->IsHANDLEType()) {
        AutoPtr<PostfixExpression> expr = new PostfixExpression();
        expr->SetType(type);
        if (type->IsFloatingPointType()) {
            expr->SetFloatingPointValue(tokenInfo.mFloatingPointValue);
        }
        else {
            expr->SetIntegralValue(tokenInfo.mIntegralValue);
            if (type->IsIntegralType()) {
                expr->SetRadix(tokenInfo.mRadix);
            }
        }
        return expr;
    }

    String message = String::Format("Integral values can not be assigned to \"%s\" type.",
            type->GetName().string());
    LogError(tokenInfo, message);
    return nullptr;
}

AutoPtr<PostfixExpression> Parser::ParseFloatingPointNumber(
    /* [in] */ Type* type)
{
    TokenInfo tokenInfo = mTokenizer.GetToken();
    if (type->IsNumericType()) {
        AutoPtr<PostfixExpression> expr = new PostfixExpression();
        expr->SetType(type);
        if (type->IsFloatingPointType()) {
            expr->SetFloatingPointValue(tokenInfo.mFloatingPointValue);
            expr->SetScientificNotation(tokenInfo.mScientificNotation);
        }
        else {
            expr->SetIntegralValue(tokenInfo.mIntegralValue);
        }
        return expr;
    }

    String message = String::Format("FloatingPoint values can not be assigned to \"%s\" type.",
            type->GetName().string());
    LogError(tokenInfo, message);
    return nullptr;
}

AutoPtr<PostfixExpression> Parser::ParseStringLiteral(
    /* [in] */ Type* type)
{
    TokenInfo tokenInfo = mTokenizer.GetToken();
    if (type->IsStringType()) {
        AutoPtr<PostfixExpression> expr = new PostfixExpression();
        expr->SetType(type);
        expr->SetStringValue(tokenInfo.mStringValue);
        return expr;
    }

    String message = String::Format("\"%s\" can not be assigned to \"%s\" type.",
            tokenInfo.mStringValue.string(), type->GetName().string());
    LogError(tokenInfo, message);
    return nullptr;
}

AutoPtr<PostfixExpression> Parser::ParseIdentifier(
    /* [in] */ Type* type)
{
    TokenInfo tokenInfo = mTokenizer.GetToken();
    if (type->IsNumericType()) {
        String constStr;
        String id = tokenInfo.mStringValue;
        int idx = id.IndexOf("::");
        if (idx > 0) {
            String typeStr = id.Substring(0, idx);
            constStr = id.Substring(idx + 2);
        }
    }
    else if (type->IsEnumerationType()) {

    }

    String message = String::Format("\"%s\" can not be assigned to \"%s\" type.",
            tokenInfo.mStringValue.string(), type->GetName().string());
    LogError(tokenInfo, message);
    return nullptr;
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
        tokenInfo = mTokenizer.PeekToken();
        if (tokenInfo.mToken == Token::COMMA) {
            mTokenizer.GetToken();
            tokenInfo = mTokenizer.PeekToken();
        }
        else if (tokenInfo.mToken != Token::PARENTHESES_CLOSE) {
            LogError(tokenInfo, "\",\" or \")\" is expected.");
            result = false;
        }
        if (!result) {
            // jump to ',' or ')'
            while (tokenInfo.mToken != Token::COMMA &&
                    tokenInfo.mToken != Token::PARENTHESES_CLOSE &&
                    tokenInfo.mToken != Token::END_OF_FILE) {
                mTokenizer.GetToken();
                tokenInfo = mTokenizer.PeekToken();
            }
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
            result = false;
        }
        if (!result) {
            // jump to ',' or ']'
            while (tokenInfo.mToken != Token::COMMA &&
                    tokenInfo.mToken != Token::BRACKETS_CLOSE &&
                    tokenInfo.mToken != Token::END_OF_FILE) {
                mTokenizer.GetToken();
                tokenInfo = mTokenizer.PeekToken();
            }
        }
    }
    if (tokenInfo.mToken == Token::END_OF_FILE) {
        LogError(tokenInfo, "\"]\" is expected.");
        mTokenizer.GetToken();
        return false;
    }
    // read ']'
    mTokenizer.GetToken();

    ParseType();

    tokenInfo = mTokenizer.PeekToken();
    if (tokenInfo.mToken != Token::IDENTIFIER) {
        LogError(tokenInfo, "Parameter name is expected.");
        // jump to ',' or ';'
        while (tokenInfo.mToken != Token::COMMA &&
                tokenInfo.mToken != Token::PARENTHESES_CLOSE &&
                tokenInfo.mToken != Token::END_OF_FILE) {
            mTokenizer.GetToken();
            tokenInfo = mTokenizer.PeekToken();
        }
        return false;
    }
    mTokenizer.GetToken();

    String parameterName = tokenInfo.mStringValue;

    if (mTokenizer.PeekToken().mToken == Token::ASSIGNMENT) {
        mTokenizer.GetToken();

        ParseExpression(nullptr);
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
    mTokenizer.GetToken(Token::ANGLE_BRACKETS_CLOSE);
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
        mTokenizer.GetToken();
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
        tokenInfo = mTokenizer.PeekToken();
        if (tokenInfo.mToken == Token::COMMA) {
            mTokenizer.GetToken();
            tokenInfo = mTokenizer.PeekToken();
        }
        else if (tokenInfo.mToken != Token::PARENTHESES_CLOSE) {
            LogError(tokenInfo, "\",\" or \")\" is expected.");
            result = false;
        }
        if (!result) {
            // jump to ',' or ')'
            while (tokenInfo.mToken != Token::COMMA &&
                    tokenInfo.mToken != Token::PARENTHESES_CLOSE &&
                    tokenInfo.mToken != Token::END_OF_FILE) {
                mTokenizer.GetToken();
                tokenInfo = mTokenizer.PeekToken();
            }
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

bool Parser::ParseEnumeration()
{
    bool result = true;
    String enumName;

    // read "enum"
    mTokenizer.GetToken();
    TokenInfo tokenInfo = mTokenizer.PeekToken();
    if (tokenInfo.mToken == Token::IDENTIFIER) {
        mTokenizer.GetToken();
        enumName = tokenInfo.mStringValue;

    }
    else {
        LogError(tokenInfo, "An enumeration name is expected.");
        result = false;
    }

    if (mTokenizer.PeekToken().mToken == Token::SEMICOLON) {
        mTokenizer.GetToken();

        return result;
    }

    result = ParseEnumerationBody() && result;

    return result;
}

bool Parser::ParseEnumerationBody()
{
    bool result = true;

    TokenInfo tokenInfo = mTokenizer.PeekToken();
    if (tokenInfo.mToken != Token::BRACES_OPEN) {
        LogError(tokenInfo, "\" { \" is expected.");
        result = false;
    }
    mTokenizer.GetToken();

    tokenInfo = mTokenizer.PeekToken();
    while (tokenInfo.mToken != Token::BRACES_CLOSE &&
            tokenInfo.mToken != Token::END_OF_FILE) {
        String enumeratorName;
        if (tokenInfo.mToken == Token::IDENTIFIER) {
            mTokenizer.GetToken();
            enumeratorName = tokenInfo.mStringValue;
        }
        else {
            LogError(tokenInfo, "An enumerator name is expected.");
            result = false;
        }

        tokenInfo = mTokenizer.PeekToken();
        if (tokenInfo.mToken == Token::ASSIGNMENT) {
            mTokenizer.GetToken();
            ParseExpression(nullptr);
            tokenInfo = mTokenizer.PeekToken();
        }
        if (tokenInfo.mToken == Token::COMMA) {
            mTokenizer.GetToken();
            tokenInfo = mTokenizer.PeekToken();
        }
        else if (tokenInfo.mToken != Token::BRACES_CLOSE) {
            LogError(tokenInfo, "\"}\" is expected.");
            result = false;
        }
        if (!result) {
            // jump to ',' or '}'
            while (tokenInfo.mToken != Token::COMMA &&
                    tokenInfo.mToken != Token::BRACES_CLOSE &&
                    tokenInfo.mToken != Token::END_OF_FILE) {
                mTokenizer.GetToken();
                tokenInfo = mTokenizer.PeekToken();
            }
        }
    }
    if (tokenInfo.mToken == Token::END_OF_FILE) {
        LogError(tokenInfo, "\"}\" is expected.");
        mTokenizer.GetToken();
        return false;
    }
    // read '}'
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

    AutoPtr<Reader> prevReader = mTokenizer.GetReader();
    bool ret = ParseFile(tokenInfo);
    mTokenizer.SetReader(prevReader);

    return ret;
}

AutoPtr<Type> Parser::ChooseType(
    /* [in] */ Type* type1,
    /* [in] */ Type* type2)
{
    if (type1->IsDoubleType()) {
        return type1;
    }
    else if (type1->IsFloatType()) {
        return type2->IsDoubleType() ? type2 : type1;
    }
    else if (type1->IsLongType()) {
        return type2->IsFloatingPointType() ? type2 : type1;
    }
    return type2;
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
