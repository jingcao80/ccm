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
#include "../ast/ArrayType.h"
#include "../ast/PointerType.h"
#include "../metadata/MetaResolver.h"
#include "../util/Logger.h"
#include "../util/MetadataUtils.h"
#include "../../runtime/metadata/Component.h"
#include "../../runtime/metadata/MetaSerializer.h"

#include <memory.h>
#include <stdlib.h>
#include <unistd.h>

using ccdl::ast::ArrayType;
using ccdl::ast::PointerType;
using ccdl::metadata::MetaResolver;

using ccm::metadata::MetaSerializer;

namespace ccdl {

template<>
String StringMap<String>::Get(
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

Parser::FileContext::FileContext()
    : mPreDeclarations(50)
    , mNext(0)
{}

void Parser::FileContext::AddPredeclaration(
    /* [in] */ const String& typeName,
    /* [in] */ const String& typeFullName)
{
    mPreDeclarations.Put(typeName, typeFullName);
}

String Parser::FileContext::FindPredeclaration(
    /* [in] */ const String& typeName)
{
    return mPreDeclarations.Get(typeName);
}

//------------------------------------------------------------------------

Parser::Error::Error()
    : mErrorToken(Tokenizer::Token::ILLEGAL_TOKEN)
    , mLineNo(0)
    , mColumnNo(0)
    , mNext(nullptr)
{}

//------------------------------------------------------------------------

const String Parser::TAG("Parser");

Parser::Parser()
    : mParsedFiles(100)
    , mMode(0)
    , mPool(nullptr)
    , mCurrNamespace(nullptr)
    , mCurrContext(nullptr)
    , mParsingType(nullptr)
    , mStatus(NOERROR)
    , mErrorHeader(nullptr)
    , mCurrError(nullptr)
    , mNeedDump(false)
{}

Parser::~Parser()
{
    mPool = nullptr;
    mCurrNamespace = nullptr;
    mCurrContext = nullptr;
    mParsingType = nullptr;

    mCurrError = mErrorHeader;
    while (mCurrError != nullptr) {
        mErrorHeader = mCurrError->mNext;
        delete mCurrError;
        mCurrError = mErrorHeader;
    }
    mErrorHeader = mCurrError = nullptr;
}

bool Parser::Parse(
    /* [in] */ const String& filePath,
    /* [in] */ const String& includeDirs,
    /* [in] */ int mode)
{
    bool ret = mTokenizer.PushInputFile(filePath);
    if (!ret) {
        Logger::E(TAG, "File \"%s\" is invalid.", filePath.string());
        return false;
    }

    mMode = mode;
    String cwd = filePath.Substring(0, filePath.LastIndexOf('/') - 1);
    if (!cwd.IsNullOrEmpty()) {
        mIncludeDirs.Add(new String(cwd));
    }
    if (!includeDirs.IsNullOrEmpty()) {
        String dirs = includeDirs;
        int index = dirs.IndexOf(":");
        while (index != -1) {
            mIncludeDirs.Add(new String(dirs.Substring(0, index - 1)));
            dirs = dirs.Substring(index + 1);
            index = dirs.IndexOf(":");
        }
        mIncludeDirs.Add(new String(dirs));
    }

    mWorld.SetRootFile(filePath);
    mCurrNamespace = mWorld.GetGlobalNamespace();
    mPool = &mWorld;

    PreParse();

    ret = ParseFile();

    PostParse();

    if (!ret || mErrorHeader != nullptr) {
        DumpError();
        return false;
    }

    if (mNeedDump) Dump();
    return true;
}

void Parser::PreParse()
{
    GenerateIInterface();
    if (mMode == MODE_COMPONENT) {
        LoadCcmrtMetadata();
    }
}

void Parser::LoadCcmrtMetadata()
{
    String rtpath(getenv("RT_PATH"));
    void* newData = MetadataUtils::ReadMetadataFromElf64(
            rtpath + "/ccmrt.so");
    if (newData == nullptr) return;
    MetaSerializer serializer;
    serializer.Deserialize(reinterpret_cast<uintptr_t>(newData));
    Module* externalModule = new Module(newData);
    MetaResolver resolver(externalModule, newData);
    resolver.InitializeModule();
    mWorld.AddExternalModule(externalModule);
}

void Parser::PostParse()
{
    std::shared_ptr<Module> module = mWorld.GetWorkingModule();
    if (module != nullptr) {
        for (int i = 0; i < module->GetInterfaceNumber(); i++) {
            CheckInterfaceIntegrity(module->GetInterface(i));
        }
        for (int i = 0; i < module->GetCoclassNumber(); i++) {
            GenerateCoclassObject(module.get(), module->GetCoclass(i));
        }
    }
}

bool Parser::ParseFile()
{
    bool parseResult = true;
    EnterFileContext();

    Tokenizer::Token token;
    while ((token = mTokenizer.PeekToken()) != Tokenizer::Token::END_OF_FILE) {
        switch (token) {
            case Tokenizer::Token::BRACKETS_OPEN:
                parseResult = ParseDeclarationWithAttribute() && parseResult;
                continue;
            case Tokenizer::Token::COCLASS: {
                parseResult = ParseCoclass(nullptr) && parseResult;
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
            case Tokenizer::Token::CONST: {
                Constant* constant = ParseConstant();
                if (constant != nullptr) {
                    mPool->AddConstant(constant);
                    constant->SetNamespace(mCurrNamespace);
                }
                else {
                    parseResult = false;
                }
                continue;
            }
            default: {
                String message = String::Format("%s is not expected.", mTokenizer.DumpToken(token));
                LogError(token, message);
                mTokenizer.GetToken();
                continue;
            }
        }
    }
    token = mTokenizer.GetToken();

    LeaveFileContext();
    return parseResult;
}

bool Parser::ParseDeclarationWithAttribute()
{
    Attribute attr;
    bool parseResult = ParseAttribute(attr);

    Tokenizer::Token token = mTokenizer.PeekToken();
    switch (token) {
        case Tokenizer::Token::COCLASS: {
            parseResult = ParseCoclass(&attr) && parseResult;
            break;
        }
        case Tokenizer::Token::INTERFACE: {
            parseResult = ParseInterface(&attr) && parseResult;
            break;
        }
        case Tokenizer::Token::MODULE: {
            if (mWorld.GetWorkingModule() != nullptr) {
                LogError(token, String("Can not declare more than one module."));
                parseResult = false;
            }
            parseResult = ParseModule(&attr) && parseResult;
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
            parseResult = ParseCoclass(&attr) && parseResult;
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
    token = mTokenizer.PeekToken();
    while (token != Tokenizer::Token::BRACKETS_CLOSE &&
            token != Tokenizer::Token::END_OF_FILE) {
        switch (token) {
            case Tokenizer::Token::UUID: {
                parseResult = ParseUuid(attr) && parseResult;
                break;
            }
            case Tokenizer::Token::VERSION: {
                parseResult = ParseVersion(attr) && parseResult;
                break;
            }
            case Tokenizer::Token::DESCRIPTION: {
                parseResult = ParseDescription(attr) && parseResult;
                break;
            }
            case Tokenizer::Token::URL: {
                parseResult = ParseUrl(attr) && parseResult;
                break;
            }
            default: {
                String message = String::Format("\"%s\" is not expected.",
                        mTokenizer.DumpToken(token));
                LogError(token, message);
                break;
            }
        }
        if (!parseResult) {
            // jump over ']'
            while (token != Tokenizer::Token::BRACKETS_CLOSE &&
                    token != Tokenizer::Token::END_OF_FILE) {
                token = mTokenizer.GetToken();
            }
            return false;
        }
        token = mTokenizer.PeekToken();
        if (token == Tokenizer::Token::COMMA) {
            mTokenizer.GetToken();
            token = mTokenizer.PeekToken();
        }
        else if (token != Tokenizer::Token::BRACKETS_CLOSE) {
            LogError(token, String("\",\" or \"]\" is expected."));
            // jump over ']'
            while (token != Tokenizer::Token::BRACKETS_CLOSE &&
                    token != Tokenizer::Token::END_OF_FILE) {
                token = mTokenizer.GetToken();
            }
            return false;
        }
    }
    // read ']'
    mTokenizer.GetToken();

    return parseResult;
}

bool Parser::ParseUuid(
    /* [out] */ Attribute& attr)
{
    Tokenizer::Token token;

    // read "uuid"
    token = mTokenizer.GetToken();
    token = mTokenizer.GetToken();
    if (token != Tokenizer::Token::PARENTHESES_OPEN) {
        LogError(token, String("\"(\" is expected."));
        return false;
    }
    token = mTokenizer.GetUuidNumberToken();
    if (token != Tokenizer::Token::UUID_NUMBER) {
        LogError(token, String("uuid number is expected."));
        return false;
    }
    else {
        attr.mUuid = mTokenizer.GetString();
    }
    token = mTokenizer.GetToken();
    if (token != Tokenizer::Token::PARENTHESES_CLOSE) {
        LogError(token, String("\")\" is expected."));
        return false;
    }
    return true;
}

bool Parser::ParseVersion(
    /* [out] */ Attribute& attr)
{
    Tokenizer::Token token;

    // read "version"
    token = mTokenizer.GetToken();
    token = mTokenizer.GetToken();
    if (token != Tokenizer::Token::PARENTHESES_OPEN) {
        LogError(token, String("\"(\" is expected."));
        return false;
    }
    token = mTokenizer.GetVersionNumberToken();
    if (token != Tokenizer::Token::VERSION_NUMBER) {
        LogError(token, String("version number is expected."));
        return false;
    }
    else {
        attr.mVersion = mTokenizer.GetString();
    }
    token = mTokenizer.GetToken();
    if (token != Tokenizer::Token::PARENTHESES_CLOSE) {
        LogError(token, String("\")\" is expected."));
        return false;
    }
    return true;
}

bool Parser::ParseDescription(
    /* [out] */ Attribute& attr)
{
    Tokenizer::Token token;

    // read "description"
    token = mTokenizer.GetToken();
    token = mTokenizer.GetToken();
    if (token != Tokenizer::Token::PARENTHESES_OPEN) {
        LogError(token, String("\"(\" is expected."));
        return false;
    }
    token = mTokenizer.GetToken();
    if (token != Tokenizer::Token::STRING_LITERAL) {
        LogError(token, String("string literal is expected."));
        return false;
    }
    else {
        attr.mDescription = mTokenizer.GetString();
    }
    token = mTokenizer.GetToken();
    if (token != Tokenizer::Token::PARENTHESES_CLOSE) {
        LogError(token, String("\")\" is expected."));
        return false;
    }
    return true;
}

bool Parser::ParseUrl(
    /* [out] */ Attribute& attr)
{
    Tokenizer::Token token;

    // read "url"
    token = mTokenizer.GetToken();
    token = mTokenizer.GetToken();
    if (token != Tokenizer::Token::PARENTHESES_OPEN) {
        LogError(token, String("\"(\" is expected."));
        return false;
    }
    token = mTokenizer.GetToken();
    if (token != Tokenizer::Token::STRING_LITERAL) {
        LogError(token, String("string literal is expected."));
        return false;
    }
    else {
        attr.mUrl = mTokenizer.GetString();
    }
    token = mTokenizer.GetToken();
    if (token != Tokenizer::Token::PARENTHESES_CLOSE) {
        LogError(token, String("\")\" is expected."));
        return false;
    }
    return true;
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
        if (!fullName.Contains("::")) {
            fullName = mCurrNamespace->ToString() + fullName;
        }
        Type* type = mPool->FindType(fullName);
        if (type != nullptr) {
            if (!type->IsInterfaceType()) {
                String message = String::Format("Interface %s is name conflict.", itfName.string());
                LogError(token, message);
                parseResult = false;
            }
            String itfName = fullName.Substring(fullName.LastIndexOf("::") + 2);
            mCurrContext->AddPredeclaration(itfName, fullName);
            return parseResult;
        }

        int index = fullName.LastIndexOf("::");
        Namespace* ns = mPool->ParseNamespace(fullName.Substring(0, index - 1));
        String itfName = fullName.Substring(index + 2);

        Interface* interface = new Interface();
        interface->SetName(itfName);
        interface->SetNamespace(ns);
        interface->SetPredecl();
        mPool->AddInterfacePredeclaration(interface);

        mCurrContext->AddPredeclaration(itfName, fullName);
        return parseResult;
    }

    if (attr == nullptr || attr->mUuid.IsNullOrEmpty() || attr->mVersion.IsNullOrEmpty()) {
        String message = String::Format("Interface %s should have attributes.", itfName.string());
        LogError(token, message);
        parseResult = false;
    }

    Interface* interface = nullptr;

    String currNsString = mCurrNamespace->ToString();
    Type* type = mPool->FindType(currNsString.IsNullOrEmpty() ?
            itfName : currNsString + itfName);
    if (type != nullptr) {
        if (type->IsInterfaceType() && ((Interface*)type)->IsPredecl()) {
            interface = (Interface*)type;
        }
        else {
            String message;
            if (type->IsInterfaceType()) {
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
        token = mTokenizer.PeekToken();
        if (token == Tokenizer::Token::IDENTIFIER) {
            mTokenizer.GetToken();
            Interface* baseItf = FindInterface(mTokenizer.GetIdentifier());
            if (baseItf != nullptr && !baseItf->IsPredecl()) {
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
            while (token != Tokenizer::Token::BRACES_OPEN &&
                    token != Tokenizer::Token::END_OF_FILE) {
                mTokenizer.GetToken();
            }
            parseResult = false;
        }
    }
    else {
        interface->SetBaseInterface(FindInterface(String("ccm::IInterface")));
    }

    mParsingType = interface;

    parseResult = ParseInterfaceBody(interface) && parseResult;

    if (parseResult) {
        interface->SetDeclared();
        interface->SetAttribute(*attr);
        mPool->AddInterface(interface);
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
            Constant* constant = ParseConstant();
            if (constant != nullptr) {
                interface->AddConstant(constant);
            }
            else {
                parseResult = false;
            }
        }
        else if (token == Tokenizer::Token::IDENTIFIER) {
            parseResult = ParseMethod(interface) && parseResult;
        }
        else {
            token = mTokenizer.GetToken();
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
    method->SetReturnType(mPool->FindType(String("ccm::ECode")));

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

    token = mTokenizer.PeekToken();
    while (token != Tokenizer::Token::PARENTHESES_CLOSE &&
            token != Tokenizer::Token::END_OF_FILE) {
        parseResult = ParseParameter(method) && parseResult;
        if (!parseResult) break;
        token = mTokenizer.PeekToken();
        if (token == Tokenizer::Token::COMMA) {
            mTokenizer.GetToken();
            token = mTokenizer.PeekToken();
        }
    }
    mTokenizer.GetToken();

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
        if (interface->FindMethod(method->GetName(), method->GetSignature()) != nullptr) {
            LogError(token, String::Format("The method \"%s\" is redeclared.",
                    method->ToString().string()));
            delete method;
            return false;
        }
        interface->AddMethod(method);
        if (interface->GetMethodNumber() >= Interface::METHOD_MAX_NUMBER) {
            LogError(token, String::Format("The Interface \"%s\" has too many methods.",
                    interface->ToString().string()));
            return false;
        }
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

    token = mTokenizer.PeekToken();
    while (token != Tokenizer::Token::BRACKETS_CLOSE &&
            token != Tokenizer::Token::END_OF_FILE) {
        switch (token) {
            case Tokenizer::Token::IN:
                mTokenizer.GetToken();
                parameter->SetAttribute(Parameter::IN);
                break;
            case Tokenizer::Token::OUT:
                mTokenizer.GetToken();
                parameter->SetAttribute(Parameter::OUT);
                break;
            case Tokenizer::Token::CALLEE:
                mTokenizer.GetToken();
                parameter->SetAttribute(Parameter::CALLEE);
                break;
        }
        token = mTokenizer.PeekToken();
        if (token == Tokenizer::Token::COMMA) {
            mTokenizer.GetToken();
            token = mTokenizer.PeekToken();
        }
        else if (token != Tokenizer::Token::BRACKETS_CLOSE) {
            LogError(token, String("\",\" or \"]\" is expected."));
            // jump over ')'
            while (token != Tokenizer::Token::PARENTHESES_CLOSE &&
                    token != Tokenizer::Token::END_OF_FILE) {
                token = mTokenizer.GetToken();
            }
            delete parameter;
            return false;
        }
    }
    // read ']'
    mTokenizer.GetToken();

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

    if (mTokenizer.PeekToken() == Tokenizer::Token::ASSIGNMENT) {
        mTokenizer.GetToken();
        Expression* expr = ParseExpression(type);
        parameter->SetDefaultValue(expr);
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
        type = mPool->FindType(String::Format("ccm::%s", mTokenizer.DumpToken(token)));
    }
    else if (token == Tokenizer::Token::IDENTIFIER) {
        type = FindType(mTokenizer.GetIdentifier());
        if (type == nullptr && mParsingType->GetName().Equals(mTokenizer.GetIdentifier())) {
            type = mParsingType;
        }
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
        PointerType* ptrType = (PointerType*)mPool->FindType(ptrTypeStr);
        if (ptrType == nullptr) {
            ptrType = new PointerType();
            ptrType->SetBaseType(type);
            ptrType->SetPointerNumber(ptrNumber);
            mPool->AddTemporaryType(ptrType);
        }
        type = ptrType;
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
    ArrayType* arrayType = (ArrayType*)mPool->FindType(arrayTypeStr);
    if (arrayType == nullptr) {
        arrayType = new ArrayType();
        arrayType->SetElementType(elemType);
        mPool->AddTemporaryType(arrayType);
    }

    return arrayType;
}

Constant* Parser::ParseConstant()
{
    bool parseResult = true;
    Tokenizer::Token token;
    Type* type = nullptr;

    Constant* constant = new Constant();

    // read "const"
    token = mTokenizer.GetToken();
    token = mTokenizer.GetToken();
    if (Tokenizer::IsPrimitiveType(token)) {
        type = mPool->FindType(String::Format("ccm::%s", mTokenizer.DumpToken(token)));
    }
    else {
        Enumeration* enumeration = nullptr;
        String typeName = mTokenizer.GetIdentifier();
        Namespace* ns = mCurrNamespace;
        while (ns != nullptr) {
            String typeFullName = ns->ToString() + typeName;
            enumeration = mPool->FindEnumeration(typeFullName);
            if (enumeration != nullptr) {
                type = enumeration;
                break;
            }
            ns = ns->GetOuterNamespace();
        }
        if (enumeration == nullptr) {
            String message = String::Format("Type \"%s\" is not declared.", typeName.string());
            LogError(token, message);
            // jump to next line
            mTokenizer.SkipCurrentLine();
            delete constant;
            return nullptr;
        }
    }
    constant->SetType(type);
    token = mTokenizer.GetToken();
    if (token == Tokenizer::Token::IDENTIFIER) {
        constant->SetName(mTokenizer.GetIdentifier());
    }
    else {
        LogError(token, String("Constant name is expected."));
        // jump to next line
        mTokenizer.SkipCurrentLine();
        delete constant;
        return nullptr;
    }

    token = mTokenizer.GetToken();
    if (token != Tokenizer::Token::ASSIGNMENT) {
        LogError(token, String("\"=\" is expected."));
        // jump to next line
        mTokenizer.SkipCurrentLine();
        delete constant;
        return nullptr;
    }

    Expression* expr = ParseExpression(type);
    if (expr != nullptr) {
        constant->SetValue(expr);
    }
    else parseResult = false;

    token = mTokenizer.GetToken();
    if (token != Tokenizer::Token::SEMICOLON) {
        LogError(token, String("\";\" is expected."));
        // jump to next line
        mTokenizer.SkipCurrentLine();
        delete constant;
        return nullptr;
    }

    if (!parseResult) {
        delete constant;
        constant = nullptr;
    }

    return constant;
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
    incOrExpr->SetType(rightExpr->GetType());
    incOrExpr->SetRadix(rightExpr->GetRadix());

    while (mTokenizer.PeekToken() == Tokenizer::Token::INCLUSIVE_OR) {
        mTokenizer.GetToken();

        rightExpr = ParseExclusiveOrExpression(exprType);
        if (rightExpr == nullptr) return nullptr;

        InclusiveOrExpression* leftExpr = incOrExpr;

        if (!leftExpr->GetType()->IsIntegralType() ||
                !rightExpr->GetType()->IsIntegralType()) {
            LogError(mTokenizer.PeekToken(), String("Inclusive or operation can not be applied to"
                    "non-integral type."));
            return nullptr;
        }

        incOrExpr = new InclusiveOrExpression();
        incOrExpr->SetLeftOperand(leftExpr);
        incOrExpr->SetRightOperand(rightExpr);
        incOrExpr->SetType(CastType(leftExpr->GetType(), rightExpr->GetType()));
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
    excOrExpr->SetType(rightExpr->GetType());
    excOrExpr->SetRadix(rightExpr->GetRadix());

    while (mTokenizer.PeekToken() == Tokenizer::Token::EXCLUSIVE_OR) {
        mTokenizer.GetToken();

        rightExpr = ParseAndExpression(exprType);
        if (rightExpr == nullptr) return nullptr;

        ExclusiveOrExpression* leftExpr = excOrExpr;

        if (!leftExpr->GetType()->IsIntegralType() ||
                !rightExpr->GetType()->IsIntegralType()) {
            LogError(mTokenizer.PeekToken(), String("Exclusive or operation can not be applied to"
                    "non-integral type."));
            return nullptr;
        }

        excOrExpr = new ExclusiveOrExpression();
        excOrExpr->SetLeftOperand(leftExpr);
        excOrExpr->SetRightOperand(rightExpr);
        excOrExpr->SetType(CastType(leftExpr->GetType(), rightExpr->GetType()));
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
    andExpr->SetType(rightExpr->GetType());
    andExpr->SetRadix(rightExpr->GetRadix());

    while (mTokenizer.PeekToken() == Tokenizer::Token::AND) {
        mTokenizer.GetToken();

        rightExpr = ParseShiftExpression(exprType);
        if (rightExpr == nullptr) return nullptr;

        AndExpression* leftExpr = andExpr;

        if (!leftExpr->GetType()->IsIntegralType() ||
                !rightExpr->GetType()->IsIntegralType()) {
            LogError(mTokenizer.PeekToken(), String("And operation can not be applied to"
                    "non-integral type."));
            return nullptr;
        }

        andExpr = new AndExpression();
        andExpr->SetLeftOperand(leftExpr);
        andExpr->SetRightOperand(rightExpr);
        andExpr->SetType(CastType(leftExpr->GetType(), rightExpr->GetType()));
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
    shiExpr->SetType(rightExpr->GetType());
    shiExpr->SetRadix(rightExpr->GetRadix());

    while (mTokenizer.PeekToken() == Tokenizer::Token::SHIFT_LEFT ||
            mTokenizer.PeekToken() == Tokenizer::Token::SHIFT_RIGHT ||
            mTokenizer.PeekToken() == Tokenizer::Token::SHIFT_RIGHT_UNSIGNED) {
        Tokenizer::Token token = mTokenizer.GetToken();

        rightExpr = ParseAdditiveExpression(exprType);
        if (rightExpr == nullptr) return nullptr;

        ShiftExpression* leftExpr = shiExpr;

        if (!leftExpr->GetType()->IsIntegralType() ||
                !rightExpr->GetType()->IsIntegralType()) {
            LogError(mTokenizer.PeekToken(), String("Shift operation can not be applied to"
                    "non-integral type."));
            return nullptr;
        }

        shiExpr = new ShiftExpression();
        shiExpr->SetLeftOperand(leftExpr);
        shiExpr->SetRightOperand(rightExpr);
        shiExpr->SetOperator(
                token == Tokenizer::Token::SHIFT_LEFT ? ShiftExpression::LEFT :
                token == Tokenizer::Token::SHIFT_RIGHT ? ShiftExpression::RIGHT :
                        ShiftExpression::RIGHT_UNSIGNED);
        shiExpr->SetType(CastType(leftExpr->GetType(), rightExpr->GetType()));
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
    addExpr->SetType(rightExpr->GetType());
    addExpr->SetRadix(rightExpr->GetRadix());

    while (mTokenizer.PeekToken() == Tokenizer::Token::PLUS ||
            mTokenizer.PeekToken() == Tokenizer::Token::MINUS) {
        Tokenizer::Token token = mTokenizer.GetToken();

        rightExpr = ParseMultiplicativeExpression(exprType);
        if (rightExpr == nullptr) return nullptr;

        AdditiveExpression* leftExpr = addExpr;

        if (!leftExpr->GetType()->IsNumericType() ||
                !rightExpr->GetType()->IsNumericType()) {
            LogError(mTokenizer.PeekToken(), String("Additive operation can not be applied to"
                    "non-numeric type."));
            return nullptr;
        }

        addExpr = new AdditiveExpression();
        addExpr->SetLeftOperand(leftExpr);
        addExpr->SetRightOperand(rightExpr);
        addExpr->SetOperator(token == Tokenizer::Token::PLUS ? AdditiveExpression::PLUS :
                AdditiveExpression::MINUS);
        addExpr->SetType(CastType(leftExpr->GetType(), rightExpr->GetType()));
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
    multiExpr->SetType(rightExpr->GetType());
    multiExpr->SetRadix(rightExpr->GetRadix());

    while (mTokenizer.PeekToken() == Tokenizer::Token::ASTERISK ||
            mTokenizer.PeekToken() == Tokenizer::Token::DIVIDE ||
            mTokenizer.PeekToken() == Tokenizer::Token::MODULO) {
        Tokenizer::Token token = mTokenizer.GetToken();

        rightExpr = ParseUnaryExpression(exprType);
        if (rightExpr == nullptr) return nullptr;

        MultiplicativeExpression* leftExpr = multiExpr;

        if (!leftExpr->GetType()->IsNumericType() ||
                !rightExpr->GetType()->IsNumericType()) {
            LogError(mTokenizer.PeekToken(), String("Multiplicative operation can not be applied to"
                    "non-numeric type."));
            return nullptr;
        }

        multiExpr = new MultiplicativeExpression();
        multiExpr->SetLeftOperand(leftExpr);
        multiExpr->SetRightOperand(rightExpr);
        multiExpr->SetOperator(
                token == Tokenizer::Token::ASTERISK ? MultiplicativeExpression::MULTIPLE :
                token == Tokenizer::Token::DIVIDE ? MultiplicativeExpression::DIVIDE :
                        MultiplicativeExpression::MODULO);
        multiExpr->SetType(CastType(leftExpr->GetType(), rightExpr->GetType()));
    }

    return multiExpr;
}

UnaryExpression* Parser::ParseUnaryExpression(
    /* [in] */ Type* exprType)
{
    Tokenizer::Token token = mTokenizer.PeekToken();

    if (token == Tokenizer::Token::PLUS || token == Tokenizer::Token::MINUS ||
            token == Tokenizer::Token::COMPLIMENT || token == Tokenizer::Token::NOT) {
        token = mTokenizer.GetToken();

        UnaryExpression* operand = ParseUnaryExpression(exprType);
        if (operand == nullptr) return nullptr;

        if ((token == Tokenizer::Token::PLUS || token == Tokenizer::Token::MINUS ||
                token == Tokenizer::Token::NOT) && (!operand->GetType()->IsNumericType())) {
            LogError(mTokenizer.PeekToken(), String("Plus, minus and not operation can not be applied to"
                    "non-numeric type."));
            return nullptr;
        }
        else if (token == Tokenizer::Token::COMPLIMENT && !operand->GetType()->IsIntegralType()) {
            LogError(mTokenizer.PeekToken(), String("Compliment operation can not be applied to"
                    "non-integral type."));
            return nullptr;
        }

        UnaryExpression* unaryExpr = new UnaryExpression();
        unaryExpr->SetRightOperand(operand);
        unaryExpr->SetOperator(
                token == Tokenizer::Token::PLUS ? UnaryExpression::POSITIVE :
                token == Tokenizer::Token::MINUS ? UnaryExpression::NEGATIVE :
                token == Tokenizer::Token::COMPLIMENT ? UnaryExpression::COMPLIMENT :
                        UnaryExpression::NOT);
        unaryExpr->SetType(operand->GetType());
        return unaryExpr;
    }
    else {
        PostfixExpression* operand = ParsePostfixExpression(exprType);
        if (operand == nullptr) return nullptr;

        UnaryExpression* unaryExpr = new UnaryExpression();
        unaryExpr->SetLeftOperand(operand);
        unaryExpr->SetType(operand->GetType());
        unaryExpr->SetRadix(operand->GetRadix());
        return unaryExpr;
    }
}

PostfixExpression* Parser::ParsePostfixExpression(
    /* [in] */ Type* exprType)
{
    Tokenizer::Token token = mTokenizer.PeekToken();
    switch (token) {
        case Tokenizer::Token::NUMBER_INTEGRAL:
            return ParseIntegralNumber(exprType);
        case Tokenizer::Token::NUMBER_FLOATINGPOINT:
            return ParseFloatingPointNumber(exprType);
        case Tokenizer::Token::TRUE:
        case Tokenizer::Token::FALSE:
            return ParseBooleanLiteral(exprType);
        case Tokenizer::Token::CHARACTER:
            return ParseCharacter(exprType);
        case Tokenizer::Token::STRING_LITERAL:
            return ParseStringLiteral(exprType);
        case Tokenizer::Token::IDENTIFIER:
            return ParseIdentifier(exprType);
        case Tokenizer::Token::PARENTHESES_OPEN: {
            Expression* expr = ParseExpression(exprType);

            if (mTokenizer.PeekToken() == Tokenizer::Token::PARENTHESES_CLOSE) {
                mTokenizer.GetToken();
            }
            else {
                LogError(token, String("\")\" is expected."));
                return nullptr;
            }

            PostfixExpression* postExpr = new PostfixExpression();
            postExpr->SetType(exprType);
            postExpr->SetExpression(expr);
            return postExpr;
        }
        case Tokenizer::Token::NULLPTR:
            mTokenizer.GetToken();
            if (exprType->IsPointerType()) {
                PostfixExpression* postExpr = new PostfixExpression();
                Type* integralType = mPool->FindType(String("ccm::Long"));
                postExpr->SetType(integralType);
                postExpr->SetIntegralValue(0);
                postExpr->SetRadix(16);
                return postExpr;
            }
            else {
                String message = String::Format("\"nullptr\" can not be assigned to \"%s\" type.",
                        exprType->GetName().string());
                LogError(token, message);
                return nullptr;
            }
        default: {
            String message = String::Format("\"%s\" is not expected.",
                    mTokenizer.DumpToken(token));
            LogError(token, message);
            return nullptr;
        }
    }
}

PostfixExpression* Parser::ParseIntegralNumber(
    /* [in] */ Type* exprType)
{
    Tokenizer::Token token = mTokenizer.GetToken();
    if (exprType->IsNumericType() || exprType->IsEnumerationType()) {
        PostfixExpression* postExpr = new PostfixExpression();
        Type* integralType = mTokenizer.Is64Bit() ?
                mPool->FindType(String("ccm::Long")) : mPool->FindType(String("ccm::Integer"));
        postExpr->SetType(integralType);
        postExpr->SetIntegralValue(mTokenizer.GetIntegralValue());
        postExpr->SetRadix(mTokenizer.GetRadix());
        return postExpr;
    }
    else if (exprType->IsHANDLEType()) {
        PostfixExpression* postExpr = new PostfixExpression();
        Type* integralType = mPool->FindType(String("ccm::Long"));
        postExpr->SetType(integralType);
        postExpr->SetIntegralValue(mTokenizer.GetIntegralValue());
        postExpr->SetRadix(mTokenizer.GetRadix());
        return postExpr;
    }
    else {
        String message = String::Format("Integral values can not be assigned to \"%s\" type.",
                exprType->GetName().string());
        LogError(token, message);
        return nullptr;
    }
}

PostfixExpression* Parser::ParseFloatingPointNumber(
    /* [in] */ Type* exprType)
{
    Tokenizer::Token token = mTokenizer.GetToken();
    if (exprType->IsNumericType()) {
        PostfixExpression* postExpr = new PostfixExpression();
        Type* fpType = mTokenizer.Is64Bit() ?
                mPool->FindType(String("ccm::Double")) : mPool->FindType(String("ccm::Float"));
        postExpr->SetType(fpType);
        postExpr->SetFloatingPointValue(mTokenizer.GetFloatingPointValue());
        return postExpr;
    }
    else {
        String message = String::Format("FloatingPoint values can not be assigned to \"%s\" type.",
                exprType->GetName().string());
        LogError(token, message);
        return nullptr;
    }
}

PostfixExpression* Parser::ParseBooleanLiteral(
    /* [in] */ Type* exprType)
{
    Tokenizer::Token token = mTokenizer.GetToken();
    if (exprType->IsBooleanType()) {
        PostfixExpression* postExpr = new PostfixExpression();
        Type* booleanType = mPool->FindType(String("ccm::Boolean"));
        postExpr->SetType(booleanType);
        postExpr->SetBooleanValue(token == Tokenizer::Token::TRUE ?
                true : false);
        return postExpr;
    }
    else {
        String message = String::Format("\"%s\" can not be assigned to \"%s\" type.",
                mTokenizer.DumpToken(token), exprType->GetName().string());
        LogError(token, message);
        return nullptr;
    }
}

PostfixExpression* Parser::ParseCharacter(
    /* [in] */ Type* exprType)
{
    Tokenizer::Token token = mTokenizer.GetToken();
    if (exprType->IsNumericType()) {
        PostfixExpression* postExpr = new PostfixExpression();
        Type* charType = mPool->FindType(String("ccm::Char"));
        postExpr->SetType(charType);
        postExpr->SetIntegralValue(mTokenizer.GetCharacter());
        return postExpr;
    }
    else {
        LogError(token, String("Character can not be assigned to non-numeric type."));
        return nullptr;
    }
}

PostfixExpression* Parser::ParseStringLiteral(
    /* [in] */ Type* exprType)
{
    Tokenizer::Token token = mTokenizer.GetToken();
    if (exprType->IsStringType()) {
        PostfixExpression* postExpr = new PostfixExpression();
        Type* stringType = mPool->FindType(String("ccm::String"));
        postExpr->SetType(stringType);
        postExpr->SetStringValue(mTokenizer.GetString());
        return postExpr;
    }
    else {
        String message = String::Format("\"%s\" can not be assigned to \"%s\" type.",
                mTokenizer.GetString().string(), exprType->GetName().string());
        LogError(token, message);
        return nullptr;
    }
}

PostfixExpression* Parser::ParseIdentifier(
    /* [in] */ Type* exprType)
{
    Tokenizer::Token token = mTokenizer.GetToken();
    if (exprType->IsEnumerationType()) {
        String enumName = mTokenizer.GetIdentifier();
        if (((Enumeration*)exprType)->Contains(enumName)) {
            PostfixExpression* postExpr = new PostfixExpression();
            postExpr->SetType(exprType);
            postExpr->SetEnumerator(enumName);
            return postExpr;
        }
        else {
            String message = String::Format("\"%s\" is not a valid enumerator of %s",
                    enumName.string(), exprType->GetName().string());
            LogError(token, message);
            return nullptr;
        }
    }
    else {
        String message = String::Format("\"%s\" can not be assigned to \"%s\" type.",
                mTokenizer.GetIdentifier().string(), exprType->GetName().string());
        LogError(token, message);
        return nullptr;
    }
}

bool Parser::ParseCoclass(
    /* [in] */ Attribute* attr)
{
    bool parseResult = true;
    Tokenizer::Token token;
    String className;

    // read "coclass"
    token = mTokenizer.GetToken();
    token = mTokenizer.GetToken();
    if (token == Tokenizer::Token::IDENTIFIER) {
        className = mTokenizer.GetIdentifier();
    }
    else {
        LogError(token, String("Identifier as coclass name is expected."));
        // jump over '}'
        while (token != Tokenizer::Token::BRACES_CLOSE &&
                token != Tokenizer::Token::END_OF_FILE) {
            token = mTokenizer.GetToken();
        }
        return false;
    }

    if (attr == nullptr || attr->mUuid.IsNullOrEmpty() || attr->mVersion.IsNullOrEmpty()) {
        String message = String::Format("Coclass %s should have attributes.", className.string());
        LogError(token, message);
        parseResult = false;
    }

    Coclass* klass = new Coclass();
    klass->SetName(className);
    klass->SetNamespace(mCurrNamespace);

    parseResult = ParseCoclassBody(klass) && parseResult;

    if (parseResult) {
        klass->SetAttribute(*attr);
        mPool->AddCoclass(klass);
    }
    else {
        delete klass;
    }

    return parseResult;
}

bool Parser::ParseCoclassBody(
    /* [in] */ Coclass* klass)
{
    bool parseResult = true;
    Tokenizer::Token token;

    token = mTokenizer.GetToken();
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
        switch (token) {
            case Tokenizer::Token::CONSTRUCTOR:
                parseResult = ParseCoclassConstructor(klass) && parseResult;
                token = mTokenizer.PeekToken();
                break;
            case Tokenizer::Token::INTERFACE:
                parseResult = ParseCoclassInterface(klass) && parseResult;
                token = mTokenizer.PeekToken();
                break;
            default:
                String message = String::Format("\"%s\" is not expected.",
                        mTokenizer.DumpToken(token));
                LogError(token, message);
                // jump over '}'
                while (token != Tokenizer::Token::BRACES_CLOSE &&
                        token != Tokenizer::Token::END_OF_FILE) {
                    token = mTokenizer.GetToken();
                }
                return false;
        }
        token = mTokenizer.PeekToken();
    }
    // read "}"
    mTokenizer.GetToken();

    return parseResult;
}

bool Parser::ParseCoclassConstructor(
    /* [in] */ Coclass* klass)
{
    bool parseResult = true;
    Tokenizer::Token token;

    // read "constructor"
    token = mTokenizer.GetToken();
    token = mTokenizer.GetToken();
    if (token != Tokenizer::Token::PARENTHESES_OPEN) {
        LogError(token, String("\"(\" is expected."));
        // jump over '}'
        while (token != Tokenizer::Token::SEMICOLON &&
                token != Tokenizer::Token::END_OF_FILE) {
            token = mTokenizer.GetToken();
        }
        return false;
    }

    Method* method = new Method();
    method->SetName(String("constructor"));

    token = mTokenizer.PeekToken();
    while (token != Tokenizer::Token::PARENTHESES_CLOSE &&
            token != Tokenizer::Token::END_OF_FILE) {
        parseResult = ParseParameter(method) && parseResult;
        if (!parseResult) break;
        token = mTokenizer.PeekToken();
        if (token == Tokenizer::Token::COMMA) {
            mTokenizer.GetToken();
            token = mTokenizer.PeekToken();
        }
    }
    mTokenizer.GetToken();

    token = mTokenizer.GetToken();
    if (token != Tokenizer::Token::SEMICOLON) {
        LogError(token, String("\";\" is expected."));
        // jump to next line
        mTokenizer.SkipCurrentLine();
        delete method;
        return false;
    }

    if (parseResult) {
        if (klass->FindConstructor(method->GetName(), method->GetSignature()) != nullptr) {
            LogError(token, String::Format("The constructor \"%s\" is redeclared.",
                    method->ToString().string()));
            delete method;
            return false;
        }
        klass->AddConstructor(method);
    }
    else {
        delete method;
    }

    return parseResult;
}

bool Parser::ParseCoclassInterface(
    /* [in] */ Coclass* klass)
{
    Tokenizer::Token token;

    // read "interface"
    token = mTokenizer.GetToken();
    token = mTokenizer.GetToken();
    if (token != Tokenizer::Token::IDENTIFIER) {
        LogError(token, String("Identifier as coclass name is expected."));
        // jump over ';'
        while (token != Tokenizer::Token::SEMICOLON &&
                token != Tokenizer::Token::END_OF_FILE) {
            token = mTokenizer.GetToken();
        }
        return false;
    }

    String itfName = mTokenizer.GetIdentifier();
    Interface* interface = FindInterface(itfName);
    if (interface == nullptr) {
        String message = String::Format("Interace \"%s\" is not declared.", itfName.string());
        LogError(token, message);
        // jump to next line
        mTokenizer.SkipCurrentLine();
        return false;
    }

    token = mTokenizer.GetToken();
    if (token != Tokenizer::Token::SEMICOLON) {
        LogError(token, String("\";\" is expected."));
        // jump to next line
        mTokenizer.SkipCurrentLine();
        return false;
    }

    klass->AddInterface(interface);
    return true;
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
    Type* type = mPool->FindType(currNsString.IsNullOrEmpty() ?
            enumName : currNsString + "::" + enumName);
    if (type != nullptr) {
        String message;
        if (type->IsEnumerationType()) {
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
        mPool->AddEnumeration(enumeration);
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

    int value = 0;
    token = mTokenizer.GetToken();
    while (token != Tokenizer::Token::BRACES_CLOSE &&
            token != Tokenizer::Token::END_OF_FILE) {
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
            Expression* expr = ParseExpression(enumeration);
            if (expr != nullptr) {
                value = expr->IntegerValue();
            }
            else parseResult = false;
            token = mTokenizer.GetToken();
        }
        if (token == Tokenizer::Token::COMMA) {
            token = mTokenizer.GetToken();
        }
        else if (token != Tokenizer::Token::BRACES_CLOSE) {
            LogError(token, String("\"}\" is expected."));
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

    String filePath;
    for (int i = 0; i < mIncludeDirs.GetSize(); i++) {
        filePath = String::Format("%s/%s", mIncludeDirs.Get(i)->string(),
                mTokenizer.GetString().string());
        if (access(filePath.string(), R_OK) == 0) {
            break;
        }
    }
    if (filePath.IsNullOrEmpty()) {
        String message = String::Format("File \"%s\" is invalid.",
                mTokenizer.GetString().string());
        LogError(token, message);
        // jump to next line
        mTokenizer.SkipCurrentLine();
        return false;
    }

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
    /* [in] */ Attribute* attr)
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

    if (attr == nullptr) {
        String message = String::Format("Module %s should have attributes.", moduleName.string());
        LogError(token, message);
        parseResult = false;
    }

    std::shared_ptr<Module> module = mWorld.CreateWorkingModule();
    module->SetName(moduleName);
    mCurrNamespace = module->GetGlobalNamespace();
    mPool = module.get();

    token = mTokenizer.PeekToken();
    while (token != Tokenizer::Token::BRACES_CLOSE &&
            token != Tokenizer::Token::END_OF_FILE) {
        switch (token) {
            case Tokenizer::Token::BRACKETS_OPEN:
                parseResult = ParseDeclarationWithAttributeExceptModule() && parseResult;
                break;
            case Tokenizer::Token::COCLASS: {
                parseResult = ParseCoclass(nullptr) && parseResult;
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
            case Tokenizer::Token::CONST: {
                Constant* constant = ParseConstant();
                if (constant != nullptr) {
                    mPool->AddConstant(constant);
                    constant->SetNamespace(mCurrNamespace);
                }
                else {
                    parseResult = false;
                }
                break;
            }
            default: {
                String message = String::Format("%s is not expected.", mTokenizer.DumpToken(token));
                LogError(token, message);
                mTokenizer.GetToken();
                break;
            }
        }
        token = mTokenizer.PeekToken();
    }
    mTokenizer.GetToken();

    if (parseResult) {
        module->SetAttribute(*attr);
    }

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

    Namespace* ns = mCurrNamespace->FindNamespace(namespaceName);
    if (ns == nullptr) {
        ns = new Namespace(namespaceName);
        mCurrNamespace->AddNamespace(ns);
        mPool->AddNamespace(ns);
    }
    mCurrNamespace = ns;

    token = mTokenizer.PeekToken();
    while (token != Tokenizer::Token::BRACES_CLOSE &&
            token != Tokenizer::Token::END_OF_FILE) {
        switch (token) {
            case Tokenizer::Token::BRACKETS_OPEN:
                parseResult = ParseDeclarationWithAttributeExceptModule() && parseResult;
                break;
            case Tokenizer::Token::COCLASS: {
                parseResult = ParseCoclass(nullptr) && parseResult;
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
            case Tokenizer::Token::CONST: {
                Constant* constant = ParseConstant();
                if (constant != nullptr) {
                    mPool->AddConstant(constant);
                    constant->SetNamespace(mCurrNamespace);
                }
                else {
                    parseResult = false;
                }
                break;
            }
            default: {
                String message = String::Format("%s is not expected.", mTokenizer.DumpToken(token));
                LogError(token, message);
                mTokenizer.GetToken();
                break;
            }
        }
        token = mTokenizer.PeekToken();
    }
    mTokenizer.GetToken();

    mCurrNamespace = mCurrNamespace->GetOuterNamespace();
    return parseResult;
}

void Parser::EnterFileContext()
{
    if (mCurrContext == nullptr) {
        mCurrContext = new FileContext();
        return;
    }

    FileContext* ctx = new FileContext();
    ctx->mNext = mCurrContext;
    mCurrContext = ctx;
}

void Parser::LeaveFileContext()
{
    FileContext* ctx = mCurrContext;
    mCurrContext = ctx->mNext;
    ctx->mNext = nullptr;
    delete ctx;
}

Interface* Parser::FindInterface(
    /* [in] */ const String& itfName)
{
    Type* type = FindType(itfName);
    return type != nullptr && type->IsInterfaceType() ? (Interface*)type : nullptr;
}

Type* Parser::FindType(
    /* [in] */ const String& typeName)
{
    String fullName = typeName;
    if (!fullName.Contains("::")) {
        fullName = mCurrContext->FindPredeclaration(typeName);
        if (fullName.IsNullOrEmpty()) {
            Namespace* ns = mCurrNamespace;
            while (ns != nullptr) {
                fullName = ns->ToString() + typeName;

                Type* type = mPool->FindType(fullName);
                if (type != nullptr) {
                    return type;
                }

                if (mPool != &mWorld) {
                    type = mWorld.FindType(fullName);
                    if (type != nullptr) {
                        type = mPool->DeepCopyType(type);
                        return type;
                    }
                }

                type = mWorld.FindTypeInExternalModules(fullName);
                if (type != nullptr) {
                    type = mPool->ShallowCopyType(type);
                    return type;
                }
                ns = ns->GetOuterNamespace();
            }
            return nullptr;
        }
    }

    Type* type = mPool->FindType(fullName);
    if (type != nullptr) {
        return type;
    }

    if (mPool != &mWorld) {
        type = mWorld.FindType(fullName);
        if (type != nullptr) {
            type = mPool->DeepCopyType(type);
            return type;
        }
    }

    type = mWorld.FindTypeInExternalModules(fullName);
    if (type != nullptr) {
        type = mPool->ShallowCopyType(type);
    }
    return type;
}

Type* Parser::CastType(
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
        return type2->IsIntegerType() ? type1 : type2;
    }
    else {
        return type2;
    }
}

void Parser::GenerateIInterface()
{
    // add IInterface
    Interface* interface = new Interface();
    interface->SetName(String("IInterface"));
    interface->SetNamespace(mPool->FindNamespace(String("ccm")));
    interface->SetExternal(true);
    Attribute attr;
    attr.mUuid = "00000000-0000-0000-0000-000000000000";
    interface->SetAttribute(attr);
    mPool->AddInterface(interface);
    // add Probe method
    Method* method = new Method();
    method->SetName(String("Probe"));
    PointerType* ptrType = new PointerType();
    ptrType->SetBaseType(mPool->FindType(String("ccm::IInterface")));
    ptrType->SetPointerNumber(1);
    mPool->AddTemporaryType(ptrType);
    method->SetReturnType(ptrType);
    Parameter* param = new Parameter();
    method->AddParameter(param);
    param->SetName(String("iid"));
    param->SetType(mPool->FindType(String("ccm::IInterface")));
    param->SetAttribute(Parameter::IN);
    interface->AddMethod(method);
    // add AddRef method
    method = new Method();
    method->SetName(String("AddRef"));
    method->SetReturnType(mPool->FindType(String("ccm::Integer")));
    param = new Parameter();
    method->AddParameter(param);
    param->SetName(String("id"));
    param->SetType(mPool->FindType(String("ccm::HANDLE")));
    PostfixExpression* expr = new PostfixExpression();
    expr->SetIntegralValue(0);
    param->SetDefaultValue(expr);
    param->SetAttribute(Parameter::IN);
    interface->AddMethod(method);
    // add Release method
    method = new Method();
    method->SetName(String("Release"));
    method->SetReturnType(mPool->FindType(String("ccm::Integer")));
    param = new Parameter();
    method->AddParameter(param);
    param->SetName(String("id"));
    param->SetType(mPool->FindType(String("ccm::HANDLE")));
    expr = new PostfixExpression();
    expr->SetIntegralValue(0);
    param->SetDefaultValue(expr);
    param->SetAttribute(Parameter::IN);
    interface->AddMethod(method);
    // add GetInterfaceID method
    method = new Method();
    method->SetName(String("GetInterfaceID"));
    method->SetReturnType(mPool->FindType(String("ccm::ECode")));
    param = new Parameter();
    param->SetName(String("object"));
    param->SetType(mPool->FindType(String("ccm::IInterface*")));
    param->SetAttribute(Parameter::IN);
    method->AddParameter(param);
    param = new Parameter();
    param->SetName(String("iid"));
    ptrType = new PointerType();
    ptrType->SetBaseType(mPool->FindType(String("ccm::InterfaceID")));
    ptrType->SetPointerNumber(1);
    mPool->AddTemporaryType(ptrType);
    param->SetType(ptrType);
    param->SetAttribute(Parameter::OUT);
    method->AddParameter(param);
    interface->AddMethod(method);
}

void Parser::GenerateCoclassObject(
    /* [in] */ Pool* pool,
    /* [in] */ Coclass* klass)
{
    bool hasConstructorWithArgu = false;
    for (int i = 0; i < klass->GetConstructorNumber(); i++) {
        Method* constructor = klass->GetConstructor(i);
        if (constructor->GetParameterNumber() > 0) {
            hasConstructorWithArgu = true;
            break;
        }
    }

    if (hasConstructorWithArgu) {
        Interface* itfco = new Interface();
        itfco->SetName(String::Format("I%sClassObject", klass->GetName().string()));
        itfco->SetNamespace(klass->GetNamespace());
        itfco->SetBaseInterface(FindInterface(String("ccm::IInterface")));
        for (int i = 0; i < klass->GetConstructorNumber(); i++) {
            Method* m = klass->GetConstructor(i);
            m->SetName(String("CreateObject"));
            Parameter* param = new Parameter();
            param->SetName(String("iid"));
            param->SetType(mPool->FindType(String("ccm::InterfaceID")));
            param->SetAttribute(Parameter::IN);
            m->AddParameter(param);
            param = new Parameter();
            param->SetName(String("object"));
            Type* t = mPool->FindType(String("ccm::IInterface**"));
            if (t == nullptr) {
                PointerType* ptrType = new PointerType();
                ptrType->SetBaseType(mPool->FindType(String("ccm::IInterface")));
                ptrType->SetPointerNumber(2);
                mPool->AddTemporaryType(ptrType);
                t = ptrType;
            }
            param->SetType(t);
            param->SetAttribute(Parameter::OUT);
            m->AddParameter(param);
            itfco->AddMethod(m);
        }
        pool->AddInterface(itfco);
        klass->AddInterface(itfco);
    }
    else {
        Interface* itfco = FindInterface(String("ccm::IClassObject"));
        itfco->Specialize();
        klass->SetConstructorDefault(klass->GetConstructorNumber() == 0);
        klass->AddInterface(itfco);
    }
}

void Parser::CheckInterfaceIntegrity(
    /* [in] */ Interface* interface)
{
    for (int i = 0; i < interface->GetMethodNumber(); i++) {
        Method* method = interface->GetMethod(i);
        for (int j = 0; j < method->GetParameterNumber(); j++) {
            Parameter* param = method->GetParameter(j);
            CheckTypeIntegrity(param->GetType());
        }
        CheckTypeIntegrity(method->GetReturnType());
    }
}

void Parser::CheckTypeIntegrity(
    /* [in] */ Type* type)
{
    while (type->IsPointerType() || type->IsArrayType()) {
        if (type->IsPointerType()) {
            type = ((PointerType*)type)->GetBaseType();
        }
        else {
            type = ((ArrayType*)type)->GetElementType();
        }
    }
    if (type->IsInterfaceType()) {
        Interface* interface = (Interface*)type;
        if (interface->IsPredecl()) {
            String message = String::Format("Interface \"%s%s\" is not declared.",
                    interface->GetNamespace()->ToString().string(),
                    interface->GetName().string());
            LogError(Tokenizer::Token::ILLEGAL_TOKEN, message);
            return;
        }
    }
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

void Parser::Dump()
{
    String dumpStr = mWorld.Dump(String(""));
    printf("%s\n", dumpStr.string());
}

}
