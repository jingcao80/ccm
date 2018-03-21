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
#include "../ccdl/ArrayType.h"
#include "../ccdl/ConstantDataMember.h"
#include "../ccdl/PointerType.h"
#include "../util/Logger.h"

using ccm::ccdl::ArrayType;
using ccm::ccdl::ConstantDataMember;
using ccm::ccdl::PointerType;

namespace ccm {

template<>
String HashMap<String>::Get(
    /* [in] */ const String& key)
{
    if (key.IsNull()) {
        return String();
    }

    int index = HashString(key) % mBucketSize;
    Bucket* curr = mBuckets[index];
    while (curr != nullptr) {
        if (curr->mKey.Equals(key)) return curr->mValue;
        curr = curr->mNext;
    }

    return String();
}

String Parser::Context::FindPreDeclaration(
    /* [in] */ const String& typeName)
{
    return mPreDeclarations.Get(typeName);
}


const String Parser::TAG("Parser");

bool Parser::Parse(
    /* [in] */ const String& filePath)
{
    bool ret = mTokenizer.PushInputFile(filePath);
    if (!ret) {
        Logger::E(TAG, "File \"%s\" is invalid.", filePath.string());
        return false;
    }

    mPathPrefix = filePath.Substring(0, filePath.LastIndexOf('/'));

    mPool.SetRootFile(filePath);
    mCurrNamespace = new Namespace(String("__global__"));
    mPool.AddNamespace(mCurrNamespace);

    ret = ParseFile();

    if (ret) {
        String out = mPool.Dump(String(""));
        printf("%s", out.string());
    }
    else DumpError();

    return ret;
}

Parser::~Parser()
{
    mCurrNamespace = nullptr;

    mCurrError = mErrorHeader;
    while (mCurrError != nullptr) {
        mErrorHeader = mCurrError->mNext;
        delete mCurrError;
        mCurrError = mErrorHeader;
    }
    mErrorHeader = mCurrError = nullptr;
}

bool Parser::ParseFile()
{
    bool parseResult = true;
    EnterContext();

    Tokenizer::Token token;
    while ((token = mTokenizer.PeekToken()) != Tokenizer::Token::END_OF_FILE) {
        switch (token) {
            case Tokenizer::Token::BRACKETS_OPEN:
                parseResult = ParseDeclarationWithAttribute() && parseResult;
                continue;
            case Tokenizer::Token::COCLASS: {
                LogError(token, String("coclass should have attributes"));
                Attribute attr;
                parseResult = ParseCoclass(attr) && parseResult;
                continue;
            }
            case Tokenizer::Token::ENUM:
                parseResult = ParseEnumeration() && parseResult;
                continue;
            case Tokenizer::Token::INCLUDE:
                parseResult = ParseInclude() && parseResult;
                continue;
            case Tokenizer::Token::INTERFACE:
                parseResult = ParseInterface(nullptr) && parseResult;
                continue;
            case Tokenizer::Token::NAMESPACE:
                parseResult = ParseNamespace() && parseResult;
                continue;
            default: {
                String message = String::Format("%s is not expected.", mTokenizer.DumpToken(token));
                LogError(token, message);
                continue;
            }
        }
    }
    token = mTokenizer.GetToken();

    LeaveContext();
    return parseResult;
}

bool Parser::ParseDeclarationWithAttribute()
{
    Attribute attr;
    bool parseResult = ParseAttribute(attr);

    Tokenizer::Token token = mTokenizer.PeekToken();
    switch (token) {
        case Tokenizer::Token::COCLASS: {
            parseResult = ParseCoclass(attr) && parseResult;
            break;
        }
        case Tokenizer::Token::INTERFACE: {
            parseResult = ParseInterface(&attr) && parseResult;
            break;
        }
        case Tokenizer::Token::MODULE: {
            parseResult = ParseModule(attr) && parseResult;
            break;
        }
        default: {
            String message = String::Format("%s is not expected.", mTokenizer.DumpToken(token));
            LogError(token, message);
            parseResult = false;
            break;
        }
    }

    return parseResult;
}

bool Parser::ParseDeclarationWithAttributeExceptModule()
{
    Attribute attr;
    bool parseResult = ParseAttribute(attr);

    Tokenizer::Token token = mTokenizer.PeekToken();
    switch (token) {
        case Tokenizer::Token::COCLASS: {
            parseResult = ParseCoclass(attr) && parseResult;
            break;
        }
        case Tokenizer::Token::INTERFACE: {
            parseResult = ParseInterface(&attr) && parseResult;
            break;
        }
        default: {
            String message = String::Format("%s is not expected.", mTokenizer.DumpToken(token));
            LogError(token, message);
            parseResult = false;
            break;
        }
    }

    return parseResult;
}

bool Parser::ParseAttribute(
    /* [out] */ Attribute& attr)
{
    bool parseResult = true;
    Tokenizer::Token token;

    // read '['
    token = mTokenizer.GetToken();
    token = mTokenizer.GetToken();
    while (token != Tokenizer::Token::BRACKETS_CLOSE &&
            token != Tokenizer::Token::END_OF_FILE) {
        switch (token) {
            case Tokenizer::Token::UUID: {
                token = mTokenizer.GetToken();
                if (token != Tokenizer::Token::PARENTHESES_OPEN) {
                    LogError(token, String("\"(\" is expected."));
                    // jump over ']'
                    while (token != Tokenizer::Token::BRACKETS_CLOSE &&
                            token != Tokenizer::Token::END_OF_FILE) {
                        token = mTokenizer.GetToken();
                    }
                    return false;
                }
                token = mTokenizer.GetUuidNumberToken();
                if (token != Tokenizer::Token::UUID_NUMBER) {
                    LogError(token, String("uuid number is expected."));
                    // jump over ']'
                    while (token != Tokenizer::Token::BRACKETS_CLOSE &&
                            token != Tokenizer::Token::END_OF_FILE) {
                        token = mTokenizer.GetToken();
                    }
                    return false;
                }
                else {
                    attr.mUuid = mTokenizer.GetString();
                }
                token = mTokenizer.GetToken();
                if (token != Tokenizer::Token::PARENTHESES_CLOSE) {
                    LogError(token, String("\")\" is expected."));
                    // jump over ']'
                    while (token != Tokenizer::Token::BRACKETS_CLOSE &&
                            token != Tokenizer::Token::END_OF_FILE) {
                        token = mTokenizer.GetToken();
                    }
                    return false;
                }
                break;
            }
            case Tokenizer::Token::VERSION: {
                token = mTokenizer.GetToken();
                if (token != Tokenizer::Token::PARENTHESES_OPEN) {
                    LogError(token, String("\"(\" is expected."));
                    // jump over ']'
                    while (token != Tokenizer::Token::BRACKETS_CLOSE &&
                            token != Tokenizer::Token::END_OF_FILE) {
                        token = mTokenizer.GetToken();
                    }
                    return false;
                }
                token = mTokenizer.GetVersionNumberToken();
                if (token != Tokenizer::Token::VERSION_NUMBER) {
                    LogError(token, String("version number is expected."));
                    // jump over ']'
                    while (token != Tokenizer::Token::BRACKETS_CLOSE &&
                            token != Tokenizer::Token::END_OF_FILE) {
                        token = mTokenizer.GetToken();
                    }
                    return false;
                }
                else {
                    attr.mVersion = mTokenizer.GetString();
                }
                token = mTokenizer.GetToken();
                if (token != Tokenizer::Token::PARENTHESES_CLOSE) {
                    LogError(token, String("\")\" is expected."));
                    // jump over ']'
                    while (token != Tokenizer::Token::BRACKETS_CLOSE &&
                            token != Tokenizer::Token::END_OF_FILE) {
                        token = mTokenizer.GetToken();
                    }
                    return false;
                }
                break;
            }
            case Tokenizer::Token::DESCRIPTION: {
                token = mTokenizer.GetToken();
                if (token != Tokenizer::Token::PARENTHESES_OPEN) {
                    LogError(token, String("\"(\" is expected."));
                    // jump over ']'
                    while (token != Tokenizer::Token::BRACKETS_CLOSE &&
                            token != Tokenizer::Token::END_OF_FILE) {
                        token = mTokenizer.GetToken();
                    }
                    return false;
                }
                token = mTokenizer.GetToken();
                if (token != Tokenizer::Token::STRING_LITERAL) {
                    LogError(token, String("string literal is expected."));
                    // jump over ']'
                    while (token != Tokenizer::Token::BRACKETS_CLOSE &&
                            token != Tokenizer::Token::END_OF_FILE) {
                        token = mTokenizer.GetToken();
                    }
                    return false;
                }
                else {
                    attr.mDescription = mTokenizer.GetString();
                }
                token = mTokenizer.GetToken();
                if (token != Tokenizer::Token::PARENTHESES_CLOSE) {
                    LogError(token, String("\")\" is expected."));
                    // jump over ']'
                    while (token != Tokenizer::Token::BRACKETS_CLOSE &&
                            token != Tokenizer::Token::END_OF_FILE) {
                        token = mTokenizer.GetToken();
                    }
                    return false;
                }
                break;
            }
            case Tokenizer::Token::URL: {
                token = mTokenizer.GetToken();
                if (token != Tokenizer::Token::PARENTHESES_OPEN) {
                    LogError(token, String("\"(\" is expected."));
                    // jump over ']'
                    while (token != Tokenizer::Token::BRACKETS_CLOSE &&
                            token != Tokenizer::Token::END_OF_FILE) {
                        token = mTokenizer.GetToken();
                    }
                    return false;
                }
                token = mTokenizer.GetToken();
                if (token != Tokenizer::Token::STRING_LITERAL) {
                    LogError(token, String("string literal is expected."));
                    // jump over ']'
                    while (token != Tokenizer::Token::BRACKETS_CLOSE &&
                            token != Tokenizer::Token::END_OF_FILE) {
                        token = mTokenizer.GetToken();
                    }
                    return false;
                }
                else {
                    attr.mUrl = mTokenizer.GetString();
                }
                token = mTokenizer.GetToken();
                if (token != Tokenizer::Token::PARENTHESES_CLOSE) {
                    LogError(token, String("\")\" is expected."));
                    // jump over ']'
                    while (token != Tokenizer::Token::BRACKETS_CLOSE &&
                            token != Tokenizer::Token::END_OF_FILE) {
                        token = mTokenizer.GetToken();
                    }
                    return false;
                }
                break;
            }
            default:
                break;
        }
        token = mTokenizer.GetToken();
        if (token == Tokenizer::Token::COMMA) {
            token = mTokenizer.GetToken();
        }
        else if (token != Tokenizer::Token::BRACKETS_CLOSE) {
            LogError(token, String("\"]\" is expected."));
            // jump over ']'
            while (token != Tokenizer::Token::BRACKETS_CLOSE &&
                    token != Tokenizer::Token::END_OF_FILE) {
                token = mTokenizer.GetToken();
            }
            return false;
        }
    }
    return parseResult;
}

bool Parser::ParseInterface(
    /* [in] */ Attribute* attr)
{
    bool parseResult = true;
    Tokenizer::Token token;
    String itfName;

    // read "interface"
    token = mTokenizer.GetToken();
    token = mTokenizer.GetToken();
    if (token == Tokenizer::Token::IDENTIFIER) {
        itfName = mTokenizer.GetIdentifier();
    }
    else {
        LogError(token, String("Interface name is expected."));
        // jump over ';' or '}'
        while (token != Tokenizer::Token::SEMICOLON &&
                token != Tokenizer::Token::BRACES_CLOSE &&
                token != Tokenizer::Token::END_OF_FILE) {
            token = mTokenizer.GetToken();
        }
        return false;
    }

    token = mTokenizer.PeekToken();
    if (token == Tokenizer::Token::SEMICOLON) {
        token = mTokenizer.GetToken();
        String fullName = mTokenizer.GetIdentifier();
        Type* type = mPool.FindType(fullName);
        if (type != nullptr) {
            if (!type->IsInterface()) {
                String message = String::Format("Interface %s is name conflict.", itfName.string());
                LogError(token, message);
                parseResult = false;
            }
            return parseResult;
        }

        int index = fullName.LastIndexOf("::");
        String nsString = index == -1 ? String("__global__") : fullName.Substring(0, index - 1);
        Namespace* ns = mPool.ParseNamespace(nsString);
        String itfName = index == -1 ? fullName : fullName.Substring(index + 2);
        Interface* interface = new Interface();
        interface->SetName(itfName);
        interface->SetNamespace(ns);
        mPool.AddInterface(interface);

        mCurrContext->AddPreDeclaration(itfName, fullName);
        return parseResult;
    }

    if (attr == nullptr) {
        String message = String::Format("Interface %s should have attributes.", itfName.string());
        LogError(token, message);
    }

    Interface* interface = nullptr;

    String currNsString = mCurrNamespace->ToString();
    Type* type = mPool.FindType(currNsString.IsNullOrEmpty() ?
            itfName : currNsString + itfName);
    if (type != nullptr) {
        if (type->IsInterface() && !((Interface*)type)->IsDefined()) {
            interface = (Interface*)type;
        }
        else {
            String message;
            if (type->IsInterface()) {
                message = String::Format("Interface %s has already been declared.", itfName.string());
            }
            else {
                message = String::Format("Interface %s is name conflict.", itfName.string());
            }
            LogError(token, message);
            // jump over '}'
            while (token != Tokenizer::Token::BRACES_CLOSE &&
                    token != Tokenizer::Token::END_OF_FILE) {
                token = mTokenizer.GetToken();
            }
            return false;
        }
    }

    bool newAdded = false;
    if (interface == nullptr) {
        interface = new Interface();
        interface->SetName(itfName);
        interface->SetNamespace(mCurrNamespace);
        newAdded = true;
    }

    if (mTokenizer.PeekToken() == Tokenizer::Token::COLON) {
        mTokenizer.GetToken();
        if (mTokenizer.PeekToken() == Tokenizer::Token::IDENTIFIER) {
            mTokenizer.GetToken();
            Interface* baseItf = FindBaseInterface(mTokenizer.GetIdentifier());
            if (baseItf != nullptr && baseItf->IsDefined()) {
                interface->SetBaseInterface(baseItf);
            }
            else {
                String message = String::Format("Base interface \"%s\" is not found or not declared.",
                        mTokenizer.GetIdentifier().string());
                LogError(token, message);
                parseResult = false;
            }
        }
        else {
            LogError(token, String("Base interface name is expected."));
            while (mTokenizer.PeekToken() != Tokenizer::Token::BRACES_OPEN &&
                    mTokenizer.PeekToken() != Tokenizer::Token::END_OF_FILE) {
                mTokenizer.GetToken();
            }
            parseResult = false;
        }
    }

    parseResult = ParseInterfaceBody(interface) && parseResult;

    if (parseResult) {
        interface->SetDefined(true);
        interface->SetAttribute(*attr);
        if (newAdded) {
            mPool.AddInterface(interface);
        }
    }
    else {
        if (newAdded) delete interface;
    }

    return parseResult;
}

bool Parser::ParseInterfaceBody(
    /* [in] */ Interface* interface)
{
    bool parseResult = true;

    Tokenizer::Token token = mTokenizer.GetToken();
    if (token != Tokenizer::Token::BRACES_OPEN) {
        LogError(token, String("\"{\" is expected."));
        // jump over '}'
        while (token != Tokenizer::Token::BRACES_CLOSE &&
                token != Tokenizer::Token::END_OF_FILE) {
            token = mTokenizer.GetToken();
        }
        return false;
    }

    token = mTokenizer.PeekToken();
    while (token != Tokenizer::Token::BRACES_CLOSE &&
            token != Tokenizer::Token::END_OF_FILE) {
        if (token == Tokenizer::Token::CONST) {
            parseResult = ParseConstDataMember(interface) && parseResult;
        }
        else if (token == Tokenizer::Token::IDENTIFIER) {
            parseResult = ParseMethod(interface) && parseResult;
        }
        token = mTokenizer.PeekToken();
    }
    // read '}'
    mTokenizer.GetToken();

    return parseResult;
}

bool Parser::ParseMethod(
    /* [in] */ Interface* interface)
{
    bool parseResult = true;

    Tokenizer::Token token = mTokenizer.GetToken();
    if (token != Tokenizer::Token::IDENTIFIER) {
        LogError(token, String("Method name is expected."));
        // jump over ';'
        while (token != Tokenizer::Token::SEMICOLON &&
                token != Tokenizer::Token::END_OF_FILE) {
            token = mTokenizer.GetToken();
        }
        return false;
    }

    Method* method = new Method();
    method->SetName(mTokenizer.GetIdentifier());

    token = mTokenizer.GetToken();
    if (token != Tokenizer::Token::PARENTHESES_OPEN) {
        LogError(token, String("\"(\" is expected."));
        // jump over ';'
        while (token != Tokenizer::Token::SEMICOLON &&
                token != Tokenizer::Token::END_OF_FILE) {
            token = mTokenizer.GetToken();
        }
        delete method;
        return false;
    }

    while (token != Tokenizer::Token::PARENTHESES_CLOSE &&
            token != Tokenizer::Token::END_OF_FILE) {
        parseResult = ParseParameter(method) && parseResult;
        if (!parseResult) break;
        token = mTokenizer.GetToken();
        if (token != Tokenizer::Token::PARENTHESES_CLOSE &&
                token != Tokenizer::Token::COMMA) {
            LogError(token, String("\",\" is expected."));
            // jump over ';'
            while (token != Tokenizer::Token::SEMICOLON &&
                    token != Tokenizer::Token::END_OF_FILE) {
                token = mTokenizer.GetToken();
            }
            delete method;
            return false;
        }
    }

    token = mTokenizer.GetToken();
    if (token != Tokenizer::Token::SEMICOLON) {
        LogError(token, String("\";\" is expected."));
        // jump over ';'
        while (token != Tokenizer::Token::SEMICOLON &&
                token != Tokenizer::Token::END_OF_FILE) {
            token = mTokenizer.GetToken();
        }
        delete method;
        return false;
    }

    if (parseResult) {
        interface->AddMethod(method);
    }
    else {
        delete method;
    }

    return parseResult;
}

bool Parser::ParseParameter(
    /* [in] */ Method* method)
{
    Parameter* parameter = new Parameter();

    Tokenizer::Token token = mTokenizer.GetToken();
    if (token != Tokenizer::Token::BRACKETS_OPEN) {
        LogError(token, String("\"[\" is expected."));
        delete parameter;
        return false;
    }

    token = mTokenizer.GetToken();
    while (token != Tokenizer::Token::BRACKETS_CLOSE &&
            token != Tokenizer::Token::END_OF_FILE) {
        switch (token) {
            case Tokenizer::Token::IN:
                parameter->SetAttribute(Parameter::IN);
                break;
            case Tokenizer::Token::OUT:
                parameter->SetAttribute(Parameter::OUT);
                break;
            case Tokenizer::Token::CALLEE:
                parameter->SetAttribute(Parameter::CALLEE);
                break;
        }
        token = mTokenizer.GetToken();
        if (token == Tokenizer::Token::COMMA) {
            token = mTokenizer.GetToken();
        }
        else if (token != Tokenizer::Token::BRACKETS_CLOSE) {
            LogError(token, String("\"]\" is expected."));
            // jump over ')'
            while (token != Tokenizer::Token::PARENTHESES_CLOSE &&
                    token != Tokenizer::Token::END_OF_FILE) {
                token = mTokenizer.GetToken();
            }
            delete parameter;
            return false;
        }
    }

    Type* type = ParseType();
    if (type != nullptr) {
        parameter->SetType(type);
    }
    else {
        // jump over ')'
        while (token != Tokenizer::Token::PARENTHESES_CLOSE &&
                token != Tokenizer::Token::END_OF_FILE) {
            token = mTokenizer.GetToken();
        }
        delete parameter;
        return false;
    }

    token = mTokenizer.GetToken();
    if (token != Tokenizer::Token::IDENTIFIER) {
        LogError(token, String("Parameter name is expected."));
        // jump over ')'
        while (token != Tokenizer::Token::PARENTHESES_CLOSE &&
                token != Tokenizer::Token::END_OF_FILE) {
            token = mTokenizer.GetToken();
        }
        delete parameter;
        return false;
    }

    parameter->SetName(mTokenizer.GetIdentifier());
    method->AddParameter(parameter);
    return true;
}

Type* Parser::ParseType()
{
    Type* type = nullptr;
    Tokenizer::Token token = mTokenizer.GetToken();
    if (Tokenizer::IsPrimitiveType(token)) {
        type = mPool.FindType(String(mTokenizer.DumpToken(token)));
    }
    else if (token == Tokenizer::Token::IDENTIFIER) {
        type = FindType(mTokenizer.GetIdentifier());
    }
    else if (token == Tokenizer::Token::ARRAY) {
        type = ParseArrayType();
    }

    if (type == nullptr) {
        String message = String::Format("Type \"%s\" was not declared in this scope.",
                mTokenizer.DumpToken(token));
        LogError(token, message);
        return nullptr;
    }

    int ptrNumber = 0;
    while ((token = mTokenizer.PeekToken()) == Tokenizer::Token::ASTERISK) {
        mTokenizer.GetToken();
        ptrNumber++;
    }

    if (ptrNumber != 0) {
        String ptrTypeStr = type->ToString();
        for (int i = 0; i < ptrNumber; i++) {
            ptrTypeStr = ptrTypeStr + "*";
        }
        Type* ptrType = mPool.FindType(ptrTypeStr);
        if (ptrType == nullptr) {
            PointerType* ptrType = new PointerType();
            ptrType->SetBaseType(type);
            ptrType->SetPointerNumber(ptrNumber);
            mPool.AddTemporaryType(ptrType);
        }
    }

    return type;
}

Type* Parser::ParseArrayType()
{
    Tokenizer::Token token = mTokenizer.GetToken();
    if (token != Tokenizer::Token::ANGLE_BRACKETS_OPEN) {
        LogError(token, String("\"<\" is expected."));
        return nullptr;
    }

    Type* elemType = ParseType();
    if (elemType == nullptr) return nullptr;

    token = mTokenizer.GetToken();
    if (token != Tokenizer::Token::ANGLE_BRACKETS_CLOSE) {
        LogError(token, String("\">\" is expected."));
        return nullptr;
    }

    String arrayTypeStr =
            String("Array<") + elemType->ToString() + String(">");
    Type* arrayType = mPool.FindType(arrayTypeStr);
    if (arrayType == nullptr) {
        arrayType = new ArrayType();
        ((ArrayType*)arrayType)->SetElementType(elemType);
        mPool.AddTemporaryType(arrayType);
    }

    return arrayType;
}

bool Parser::ParseConstDataMember(
    /* [in] */ Interface* interface)
{
    bool parseResult = true;
    Tokenizer::Token token;
    Type* type = nullptr;

    ConstantDataMember* dataMember = new ConstantDataMember();

    // read "const"
    token = mTokenizer.GetToken();
    token = mTokenizer.GetToken();
    if (Tokenizer::IsPrimitiveType(token)) {
        type = mPool.FindType(String(mTokenizer.DumpToken(token)));
    }
    else {
        Enumeration* enumeration = nullptr;
        String typeName = mTokenizer.GetIdentifier();
        Namespace* ns = mCurrNamespace;
        while (ns != nullptr) {
            String typeFullName = ns->ToString() + typeName;
            enumeration = mPool.FindEnumeration(typeFullName);
            if (enumeration != nullptr) {
                type = enumeration;
                break;
            }
            ns = ns->GetOuterNamespace();
        }
        if (enumeration == nullptr) {
            String message = String::Format("Type \"%s\" is not declared.", typeName.string());
            LogError(token, message);
            while (token != Tokenizer::Token::SEMICOLON &&
                    token != Tokenizer::Token::END_OF_FILE) {
                token = mTokenizer.GetToken();
            }
            delete dataMember;
            return false;
        }
    }
    dataMember->SetType(type);

    token = mTokenizer.GetToken();
    if (token == Tokenizer::Token::IDENTIFIER) {
        dataMember->SetName(mTokenizer.GetIdentifier());
    }
    else {
        LogError(token, String("Constant name is expected."));
        while (token != Tokenizer::Token::SEMICOLON &&
                token != Tokenizer::Token::END_OF_FILE) {
            token = mTokenizer.GetToken();
        }
        delete dataMember;
        return false;
    }

    token = mTokenizer.GetToken();
    if (token != Tokenizer::Token::ASSIGNMENT) {
        LogError(token, String("\"=\" is expected."));
        while (token != Tokenizer::Token::SEMICOLON &&
                token != Tokenizer::Token::END_OF_FILE) {
            token = mTokenizer.GetToken();
        }
        delete dataMember;
        return false;
    }

    parseResult = ParseExpression(type) && parseResult;

    token = mTokenizer.GetToken();
    if (token != Tokenizer::Token::SEMICOLON) {
        LogError(token, String("\";\" is expected."));
        while (token != Tokenizer::Token::SEMICOLON &&
                token != Tokenizer::Token::END_OF_FILE) {
            token = mTokenizer.GetToken();
        }
        delete dataMember;
        return false;
    }

    if (parseResult) {
        interface->AddConstantDataMember(dataMember);
    }
    else {
        delete dataMember;
    }

    return parseResult;
}

Expression* Parser::ParseExpression(
    /* [in] */ Type* exprType)
{
    return ParseInclusiveOrExpression(exprType);
}

InclusiveOrExpression* Parser::ParseInclusiveOrExpression(
    /* [in] */ Type* exprType)
{
    ExclusiveOrExpression* rightExpr = ParseExclusiveOrExpression(exprType);
    if (rightExpr == nullptr) return nullptr;

    InclusiveOrExpression* incOrExpr = new InclusiveOrExpression();
    incOrExpr->SetRightOperand(rightExpr);

    while (mTokenizer.PeekToken() == Tokenizer::Token::INCLUSIVE_OR) {
        mTokenizer.GetToken();

        rightExpr = ParseExclusiveOrExpression(exprType);
        if (rightExpr == nullptr) return nullptr;

        InclusiveOrExpression* leftExpr = incOrExpr;
        incOrExpr = new InclusiveOrExpression();
        incOrExpr->SetLeftOperand(leftExpr);
        incOrExpr->SetRightOperand(rightExpr);
    }

    return incOrExpr;
}

ExclusiveOrExpression* Parser::ParseExclusiveOrExpression(
    /* [in] */ Type* exprType)
{
    AndExpression* rightExpr = ParseAndExpression(exprType);
    if (rightExpr == nullptr) return nullptr;

    ExclusiveOrExpression* excOrExpr = new ExclusiveOrExpression();
    excOrExpr->SetRightOperand(rightExpr);

    while (mTokenizer.PeekToken() == Tokenizer::Token::EXCLUSIVE_OR) {
        mTokenizer.GetToken();

        rightExpr = ParseAndExpression(exprType);
        if (rightExpr == nullptr) return nullptr;

        ExclusiveOrExpression* leftExpr = excOrExpr;
        excOrExpr = new ExclusiveOrExpression();
        excOrExpr->SetLeftOperand(leftExpr);
        excOrExpr->SetRightOperand(rightExpr);
    }

    return excOrExpr;
}

AndExpression* Parser::ParseAndExpression(
    /* [in] */ Type* exprType)
{
    ShiftExpression* rightExpr = ParseShiftExpression(exprType);
    if (rightExpr == nullptr) return nullptr;

    AndExpression* andExpr = new AndExpression();
    andExpr->SetRightOperand(rightExpr);

    while (mTokenizer.PeekToken() == Tokenizer::Token::AND) {
        mTokenizer.GetToken();

        rightExpr = ParseShiftExpression(exprType);
        if (rightExpr == nullptr) return nullptr;

        AndExpression* leftExpr = andExpr;
        andExpr = new AndExpression();
        andExpr->SetLeftOperand(leftExpr);
        andExpr->SetRightOperand(rightExpr);
    }

    return andExpr;
}

ShiftExpression* Parser::ParseShiftExpression(
    /* [in] */ Type* exprType)
{
    AdditiveExpression* rightExpr = ParseAdditiveExpression(exprType);
    if (rightExpr == nullptr) return nullptr;

    ShiftExpression* shiExpr = new ShiftExpression();
    shiExpr->SetRightOperand(rightExpr);

    while (mTokenizer.PeekToken() == Tokenizer::Token::SHIFT_LEFT ||
            mTokenizer.PeekToken() == Tokenizer::Token::SHIFT_RIGHT ||
            mTokenizer.PeekToken() == Tokenizer::Token::SHIFT_RIGHT_UNSIGNED) {
        Tokenizer::Token token = mTokenizer.GetToken();

        rightExpr = ParseAdditiveExpression(exprType);
        if (rightExpr == nullptr) return nullptr;

        ShiftExpression* leftExpr = shiExpr;
        shiExpr = new ShiftExpression();
        shiExpr->SetLeftOperand(leftExpr);
        shiExpr->SetRightOperand(rightExpr);
        shiExpr->SetOperator(
                token == Tokenizer::Token::SHIFT_LEFT ? ShiftExpression::LEFT :
                token == Tokenizer::Token::SHIFT_RIGHT ? ShiftExpression::RIGHT :
                        ShiftExpression::RIGHT_UNSIGNED);
    }

    return shiExpr;
}

AdditiveExpression* Parser::ParseAdditiveExpression(
    /* [in] */ Type* exprType)
{
    MultiplicativeExpression* rightExpr = ParseMultiplicativeExpression(exprType);
    if (rightExpr == nullptr) return nullptr;

    AdditiveExpression* addExpr = new AdditiveExpression();
    addExpr->SetRightOperand(rightExpr);

    while (mTokenizer.PeekToken() == Tokenizer::Token::PLUS ||
            mTokenizer.PeekToken() == Tokenizer::Token::MINUS) {
        Tokenizer::Token token = mTokenizer.GetToken();

        rightExpr = ParseMultiplicativeExpression(exprType);
        if (rightExpr == nullptr) return nullptr;

        AdditiveExpression* leftExpr = addExpr;
        addExpr = new AdditiveExpression();
        addExpr->SetLeftOperand(leftExpr);
        addExpr->SetRightOperand(rightExpr);
        addExpr->SetOperator(token == Tokenizer::Token::PLUS ? AdditiveExpression::PLUS :
                AdditiveExpression::MINUS);
    }

    return addExpr;
}

MultiplicativeExpression* Parser::ParseMultiplicativeExpression(
    /* [in] */ Type* exprType)
{
    UnaryExpression* rightExpr = ParseUnaryExpression(exprType);
    if (rightExpr == nullptr) return nullptr;

    MultiplicativeExpression* multiExpr = new MultiplicativeExpression();
    multiExpr->SetRightOperand(rightExpr);

    while (mTokenizer.PeekToken() == Tokenizer::Token::ASTERISK ||
            mTokenizer.PeekToken() == Tokenizer::Token::DIVIDE ||
            mTokenizer.PeekToken() == Tokenizer::Token::MODULO) {
        Tokenizer::Token token = mTokenizer.GetToken();

        rightExpr = ParseUnaryExpression(exprType);
        if (rightExpr == nullptr) return nullptr;

        MultiplicativeExpression* leftExpr = multiExpr;
        multiExpr = new MultiplicativeExpression();
        multiExpr->SetLeftOperand(leftExpr);
        multiExpr->SetRightOperand(rightExpr);
        multiExpr->SetOperator(
                token == Tokenizer::Token::ASTERISK ? MultiplicativeExpression::MULTIPLE :
                token == Tokenizer::Token::DIVIDE ? MultiplicativeExpression::DIVIDE :
                        MultiplicativeExpression::MODULO);
    }

    return multiExpr;
}

UnaryExpression* Parser::ParseUnaryExpression(
    /* [in] */ Type* exprType)
{
    Tokenizer::Token token = mTokenizer.PeekToken();

    if (token == Tokenizer::Token::PLUS || token == Tokenizer::Token::MINUS ||
            token == Tokenizer::Token::COMPLIMENT || token == Tokenizer::Token::NOT) {
        token = mTokenizer.PeekToken();

        UnaryExpression* operand = ParseUnaryExpression(exprType);
        if (operand == nullptr) return nullptr;

        UnaryExpression* unaryExpr = new UnaryExpression();
        unaryExpr->SetRightOperand(operand);
        unaryExpr->SetOperator(
                token == Tokenizer::Token::PLUS ? UnaryExpression::POSITIVE :
                token == Tokenizer::Token::MINUS ? UnaryExpression::NEGATIVE :
                token == Tokenizer::Token::COMPLIMENT ? UnaryExpression::COMPLIMENT :
                        UnaryExpression::NOT);
        return unaryExpr;
    }
    else {
        PostfixExpression* operand = ParsePostfixExpression(exprType);
        if (operand == nullptr) return nullptr;

        UnaryExpression* unaryExpr = new UnaryExpression();
        unaryExpr->SetLeftOperand(operand);
        return unaryExpr;
    }
}

PostfixExpression* Parser::ParsePostfixExpression(
    /* [in] */ Type* exprType)
{
    PostfixExpression* postExpr = nullptr;

    Tokenizer::Token token = mTokenizer.PeekToken();

    if (token == Tokenizer::Token::NUMBER_INTEGRAL) {
        mTokenizer.GetToken();
        if (exprType->IsNumericType()) {
            postExpr = new PostfixExpression();
            postExpr->SetType(exprType);
            if (exprType->IsIntegralType()) {
                postExpr->SetIntegralValue(mTokenizer.GetIntegralValue());
            }
            else if (exprType->IsFloatingPointType()) {
                postExpr->SetFloatingPointValue(mTokenizer.GetIntegralValue());
            }
        }
        else {
            String message = String::Format("Integral values can not be assigned to \"%s\" type.",
                    exprType->GetName().string());
            LogError(token, message);
            return nullptr;
        }
    }
    else if (token == Tokenizer::Token::NUMBER_FLOATINGPOINT) {
        mTokenizer.GetToken();
        if (exprType->IsNumericType()) {
            postExpr = new PostfixExpression();
            postExpr->SetType(exprType);
            if (exprType->IsIntegralType()) {
                postExpr->SetIntegralValue(mTokenizer.GetFloatingPointValue());
            }
            else if (exprType->IsFloatingPointType()) {
                postExpr->SetFloatingPointValue(mTokenizer.GetFloatingPointValue());
            }
        }
        else {
            String message = String::Format("FloatingPoint values can not be assigned to \"%s\" type.",
                    exprType->GetName().string());
            LogError(token, message);
            return nullptr;
        }
    }
    else if (token == Tokenizer::Token::TRUE ||
             token == Tokenizer::Token::FALSE) {
        mTokenizer.GetToken();
        if (exprType->GetName().Equals("Boolean")) {
            postExpr = new PostfixExpression();
            postExpr->SetType(exprType);
            postExpr->SetBooleanValue(token == Tokenizer::Token::TRUE ?
                    true : false);
        }
        else {
            String message = String::Format("\"%s\" can not be assigned to \"%s\" type.",
                    mTokenizer.DumpToken(token), exprType->GetName().string());
            LogError(token, message);
            return nullptr;
        }
    }
    else if (token == Tokenizer::Token::CHARACTER) {
        mTokenizer.GetToken();
        if (exprType->IsNumericType()) {
            postExpr = new PostfixExpression();
            postExpr->SetType(exprType);
            if (exprType->IsIntegralType()) {
                postExpr->SetIntegralValue(mTokenizer.GetCharacter());
            }
            else if (exprType->IsFloatingPointType()) {
                postExpr->SetFloatingPointValue(mTokenizer.GetCharacter());
            }
        }
        else {
            LogError(token, String("Character can not be assigned to non-numeric type."));
            return nullptr;
        }
    }
    else if (token == Tokenizer::Token::STRING_LITERAL) {
        mTokenizer.GetToken();
        if (exprType->GetName().Equals("String")) {
            postExpr = new PostfixExpression();
            postExpr->SetType(exprType);
            postExpr->SetStringValue(mTokenizer.GetString());
        }
        else {
            String message = String::Format("\"%s\" can not be assigned to \"%s\" type.",
                    mTokenizer.GetString().string(), exprType->GetName().string());
            LogError(token, message);
            return nullptr;
        }
    }
    else if (token == Tokenizer::Token::IDENTIFIER) {
        mTokenizer.GetToken();
        if (exprType->IsEnumeration()) {
            String enumName = mTokenizer.GetIdentifier();
            if (((Enumeration*)exprType)->Contains(enumName)) {
                postExpr = new PostfixExpression();
                postExpr->SetType(exprType);
                postExpr->SetEnumerator(enumName);
            }
            else {
                String message = String::Format("\"%s\" is not a valid enumerator of %s",
                        enumName.string(), exprType->GetName().string());
                LogError(token, message);
                return nullptr;
            }
        }
    }
    else if (token == Tokenizer::Token::PARENTHESES_OPEN) {
        Expression* expr = ParseExpression(exprType);

        if (mTokenizer.PeekToken() == Tokenizer::Token::PARENTHESES_CLOSE) {
            mTokenizer.GetToken();
        }
        else {
            LogError(token, String("\")\" is expected."));
            return nullptr;
        }

        postExpr = new PostfixExpression();
        postExpr->SetType(exprType);
        postExpr->SetExpression(expr);
    }

    return postExpr;
}

bool Parser::ParseCoclass(
    /* [in] */ Attribute& attr)
{
    // todo:
    return 0;
}

bool Parser::ParseEnumeration()
{
    bool parseResult = true;
    Tokenizer::Token token;
    String enumName;

    // read "enum"
    token = mTokenizer.GetToken();
    token = mTokenizer.GetToken();
    if (token == Tokenizer::Token::IDENTIFIER) {
        enumName = mTokenizer.GetIdentifier();
    }
    else {
        LogError(token, String("Identifier as enumeration name is expected."));
        // jump over '}'
        while (token != Tokenizer::Token::BRACES_CLOSE &&
                token != Tokenizer::Token::END_OF_FILE) {
            token = mTokenizer.GetToken();
        }
        return false;
    }

    String currNsString = mCurrNamespace->ToString();
    Type* type = mPool.FindType(currNsString.IsNullOrEmpty() ?
            enumName : currNsString + "::" + enumName);
    if (type != nullptr) {
        String message;
        if (type->IsEnumeration()) {
            message = String::Format("Enumeration %s has already been declared.", enumName.string());
        }
        else {
            message = String::Format("Enumeration %s is name conflict.", enumName.string());
        }
        LogError(token, message);
        // jump over '}'
        while (token != Tokenizer::Token::BRACES_CLOSE &&
                token != Tokenizer::Token::END_OF_FILE) {
            token = mTokenizer.GetToken();
        }
        return false;
    }

    Enumeration* enumeration = new Enumeration();
    enumeration->SetName(enumName);
    enumeration->SetNamespace(mCurrNamespace);

    parseResult = ParseEnumerationBody(enumeration) && parseResult;

    if (parseResult) {
        mPool.AddEnumeration(enumeration);
    }
    else {
        delete enumeration;
    }

    return parseResult;
}

bool Parser::ParseEnumerationBody(
    /* [in] */ Enumeration* enumeration)
{
    bool parseResult = true;
    Tokenizer::Token token;

    token = mTokenizer.GetToken();
    if (token != Tokenizer::Token::BRACES_OPEN) {
        LogError(token, String("\" { \" is expected."));
        // jump over '}'
        while (token != Tokenizer::Token::BRACES_CLOSE &&
                token != Tokenizer::Token::END_OF_FILE) {
            token = mTokenizer.GetToken();
        }
        return false;
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
            LogError(token, String("Identifier as enumerator name is expected."));
            // jump over '}'
            while (token != Tokenizer::Token::BRACES_CLOSE &&
                    token != Tokenizer::Token::END_OF_FILE) {
                token = mTokenizer.GetToken();
            }
            return false;
        }
        token = mTokenizer.GetToken();
        if (token == Tokenizer::Token::ASSIGNMENT) {
            parseResult = ParseExpression(enumeration) && parseResult;
            token = mTokenizer.GetToken();
        }
        if (token == Tokenizer::Token::COMMA) {
            token = mTokenizer.GetToken();
        }
        else if (token != Tokenizer::Token::BRACES_CLOSE) {
            LogError(token, String("\")\" is expected."));
            // jump over '}'
            while (token != Tokenizer::Token::BRACES_CLOSE &&
                    token != Tokenizer::Token::END_OF_FILE) {
                token = mTokenizer.GetToken();
            }
            return false;
        }
        enumeration->AddEnumerator(name, value++);
    }
    return parseResult;
}

bool Parser::ParseInclude()
{
    Tokenizer::Token token;

    // read "include"
    token = mTokenizer.GetToken();
    token = mTokenizer.GetToken();
    if (token != Tokenizer::Token::STRING_LITERAL) {
        LogError(token, String("The path of a file is expected."));
        // jump to next line
        mTokenizer.SkipCurrentLine();
        return false;
    }

    String filePath = mPathPrefix + mTokenizer.GetString();
    if (mParsedFiles.ContainsKey(filePath)) return true;

    if (!mTokenizer.PushInputFile(filePath)) {
        String message = String::Format("File \"%s\" is invalid.",
                mTokenizer.GetString().string());
        LogError(token, message);
        // jump to next line
        mTokenizer.SkipCurrentLine();
        return false;
    }
    else {
        mParsedFiles.Put(filePath, false);
    }

    bool ret = ParseFile();

    mParsedFiles.Put(filePath, true);
    mTokenizer.PopInputFileAndRemove();

    return ret;
}

bool Parser::ParseModule(
    /* [in] */ Attribute& attr)
{
    bool parseResult = true;
    String moduleName;
    Tokenizer::Token token;

    // read "module"
    token = mTokenizer.GetToken();
    if (mTokenizer.PeekToken() == Tokenizer::Token::IDENTIFIER) {
        mTokenizer.GetToken();
        moduleName = mTokenizer.GetIdentifier();
    }
    else {
        LogError(token, String("Identifier as module name is expected."));
        parseResult = false;
    }

    token = mTokenizer.GetToken();
    if (token != Tokenizer::Token::BRACES_OPEN) {
        LogError(token, String("\"{\" is expected."));
        return false;
    }

    // todo:



    token = mTokenizer.PeekToken();
    while (token != Tokenizer::Token::BRACES_CLOSE &&
            token != Tokenizer::Token::END_OF_FILE) {
        switch (token) {
            case Tokenizer::Token::BRACKETS_OPEN:
                parseResult = ParseDeclarationWithAttributeExceptModule() && parseResult;
                break;
            case Tokenizer::Token::COCLASS: {
                LogError(token, String("coclass should have attributes"));
                Attribute attr;
                parseResult = ParseCoclass(attr) && parseResult;
                break;
            }
            case Tokenizer::Token::ENUM:
                parseResult = ParseEnumeration() && parseResult;
                break;
            case Tokenizer::Token::INCLUDE:
                parseResult = ParseInclude() && parseResult;
                break;
            case Tokenizer::Token::INTERFACE:
                parseResult = ParseInterface(nullptr) && parseResult;
                break;
            case Tokenizer::Token::NAMESPACE:
                parseResult = ParseNamespace() && parseResult;
                break;
            default: {
                String message = String::Format("%s is not expected.", mTokenizer.DumpToken(token));
                LogError(token, message);
                break;
            }
        }
        token = mTokenizer.PeekToken();
    }
    mTokenizer.GetToken();

    return parseResult;
}

bool Parser::ParseNamespace()
{
    bool parseResult = true;
    String namespaceName;
    Tokenizer::Token token;

    // read "namespace"
    token = mTokenizer.GetToken();
    if (mTokenizer.PeekToken() == Tokenizer::Token::IDENTIFIER) {
        mTokenizer.GetToken();
        namespaceName = mTokenizer.GetIdentifier();
    }
    else {
        LogError(token, String("Identifier as namespace name is expected."));
        parseResult = false;
    }

    token = mTokenizer.GetToken();
    if (token != Tokenizer::Token::BRACES_OPEN) {
        LogError(token, String("\"{\" is expected."));
        return false;
    }

    Namespace* ns = new Namespace(namespaceName);
    mCurrNamespace->AddInnerNamespace(ns);
    mCurrNamespace = ns;

    token = mTokenizer.PeekToken();
    while (token != Tokenizer::Token::BRACES_CLOSE &&
            token != Tokenizer::Token::END_OF_FILE) {
        switch (token) {
            case Tokenizer::Token::BRACKETS_OPEN:
                parseResult = ParseDeclarationWithAttribute() && parseResult;
                break;
            case Tokenizer::Token::COCLASS: {
                LogError(token, String("coclass should have attributes"));
                Attribute attr;
                parseResult = ParseCoclass(attr) && parseResult;
                break;
            }
            case Tokenizer::Token::ENUM:
                parseResult = ParseEnumeration() && parseResult;
                break;
            case Tokenizer::Token::INCLUDE:
                parseResult = ParseInclude() && parseResult;
                break;
            case Tokenizer::Token::INTERFACE:
                parseResult = ParseInterface(nullptr) && parseResult;
                break;
            case Tokenizer::Token::NAMESPACE:
                parseResult = ParseNamespace() && parseResult;
                break;
            default: {
                String message = String::Format("%s is not expected.", mTokenizer.DumpToken(token));
                LogError(token, message);
                break;
            }
        }
        token = mTokenizer.PeekToken();
    }
    mTokenizer.GetToken();

    mCurrNamespace = mCurrNamespace->GetOuterNamespace();
    return parseResult;
}

void Parser::EnterContext()
{
    if (mCurrContext == nullptr) {
        mCurrContext = new Context();
        return;
    }

    Context* ctx = new Context();
    ctx->mNext = mCurrContext;
    mCurrContext = ctx;
}

void Parser::LeaveContext()
{
    Context* ctx = mCurrContext;
    mCurrContext = ctx->mNext;
    ctx->mNext = nullptr;
    delete ctx;
}

Interface* Parser::FindBaseInterface(
    /* [in] */ const String& itfName)
{
    Type* type = FindType(itfName);
    return type != nullptr && type->IsInterface() ? (Interface*)type : nullptr;
}

Type* Parser::FindType(
    /* [in] */ const String& typeName)
{
    Type* type = nullptr;
    if (typeName.Contains("::")) {
        type = mPool.FindType(typeName);
    }
    else {
        String typeFullName = mCurrContext->FindPreDeclaration(typeName);
        if (!typeFullName.IsNullOrEmpty()) {
            type = mPool.FindType(typeFullName);
        }
        else {
            Namespace* ns = mCurrNamespace;
            while (ns != nullptr) {
                typeFullName = ns->ToString() + typeName;
                type = mPool.FindType(typeFullName);
                if (type != nullptr) break;
                ns = ns->GetOuterNamespace();
            }
        }
    }
    return type;
}

void Parser::LogError(
    /* [in] */ Tokenizer::Token token,
    /* [in] */ const String& message)
{
    String file = mTokenizer.GetCurrentFile()->GetPath();
    file = file.Substring(file.LastIndexOf('/') + 1);
    int lineNo = mTokenizer.GetTokenLineNo();
    int columeNo = mTokenizer.GetTokenColumnNo();

    if (mErrorHeader == nullptr) {
        mErrorHeader = new Error();
        mErrorHeader->mErrorToken = token;
        mErrorHeader->mFileName = file;
        mErrorHeader->mLineNo = lineNo;
        mErrorHeader->mColumnNo = columeNo;
        mErrorHeader->mMessage = message;
        mCurrError = mErrorHeader;
        return;
    }

    Error* error = new Error();
    error->mFileName = file;
    error->mErrorToken = token;
    error->mLineNo = lineNo;
    error->mColumnNo = columeNo;
    error->mMessage = message;
    mCurrError->mNext = error;
    mCurrError = error;
}

void Parser::DumpError()
{
    Error* error = mErrorHeader;
    while (error != nullptr) {
        Logger::E(TAG, "%s[Line %d, Column %d] %s", error->mFileName.string(),
                error->mLineNo, error->mColumnNo, error->mMessage.string());
        error = error->mNext;
    }
}

}
