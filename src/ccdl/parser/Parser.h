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

#ifndef __CCDL_PARSER_H__
#define __CCDL_PARSER_H__

#include "Environment.h"
#include "Tokenizer.h"
#include "../ast/AdditiveExpression.h"
#include "../ast/AndExpression.h"
#include "../ast/Attribute.h"
#include "../ast/Coclass.h"
#include "../ast/Enumeration.h"
#include "../ast/ExclusiveOrExpression.h"
#include "../ast/Expression.h"
#include "../ast/InclusiveOrExpression.h"
#include "../ast/Interface.h"
#include "../ast/Method.h"
#include "../ast/Module.h"
#include "../ast/MultiplicativeExpression.h"
#include "../ast/Namespace.h"
#include "../ast/Parameter.h"
#include "../ast/Pool.h"
#include "../ast/PostfixExpression.h"
#include "../ast/ShiftExpression.h"
#include "../ast/Type.h"
#include "../ast/UnaryExpression.h"
#include "../util/StringMap.h"

#include <memory>

using ccdl::ast::AdditiveExpression;
using ccdl::ast::AndExpression;
using ccdl::ast::Attribute;
using ccdl::ast::Coclass;
using ccdl::ast::Enumeration;
using ccdl::ast::ExclusiveOrExpression;
using ccdl::ast::Expression;
using ccdl::ast::InclusiveOrExpression;
using ccdl::ast::Interface;
using ccdl::ast::Method;
using ccdl::ast::Module;
using ccdl::ast::MultiplicativeExpression;
using ccdl::ast::Namespace;
using ccdl::ast::Parameter;
using ccdl::ast::Pool;
using ccdl::ast::PostfixExpression;
using ccdl::ast::ShiftExpression;
using ccdl::ast::Type;
using ccdl::ast::UnaryExpression;

namespace ccdl {

class Parser
{
private:
    struct Context
    {
        Context()
            : mPreDeclarations(50)
            , mNext(0)
        {}

        void AddPreDeclaration(
            /* [in] */ const String& typeName,
            /* [in] */ const String& typeFullName)
        {
            mPreDeclarations.Put(typeName, typeFullName);
        }

        String FindPreDeclaration(
            /* [in] */ const String& typeName);

        StringMap<String> mPreDeclarations;
        Context* mNext;
    };

    struct Error
    {
        Error()
            : mErrorToken(Tokenizer::Token::ILLEGAL_TOKEN)
            , mLineNo(0)
            , mColumnNo(0)
            , mNext(nullptr)
        {}

        Tokenizer::Token mErrorToken;
        String mFileName;
        int mLineNo;
        int mColumnNo;
        String mMessage;
        Error* mNext;
    };

public:
    Parser()
        : mMode(0)
        , mParsedFiles(100)
        , mEnvironment(nullptr)
        , mModule(nullptr)
        , mCurrNamespace(nullptr)
        , mPool(nullptr)
        , mCurrContext(nullptr)
        , mStatus(NOERROR)
        , mErrorHeader(nullptr)
        , mCurrError(nullptr)
        , mNeedDump(false)
    {}

    ~Parser();

    bool Parse(
        /* [in] */ const String& filePath,
        /* [in] */ int mode);

    inline std::shared_ptr<Module> GetModule()
    { return mModule; }

private:
    void PreParse();

    void LoadCcmrtMetadata();

    void PostParse();

    bool ParseFile();

    bool ParseDeclarationWithAttribute();

    bool ParseDeclarationWithAttributeExceptModule();

    bool ParseAttribute(
        /* [out] */ Attribute& attr);

    bool ParseUuid(
        /* [out] */ Attribute& attr);

    bool ParseVersion(
        /* [out] */ Attribute& attr);

    bool ParseDescription(
        /* [out] */ Attribute& attr);

    bool ParseUrl(
        /* [out] */ Attribute& attr);

    bool ParseInterface(
        /* [in] */ Attribute* attr);

    bool ParseInterfaceBody(
        /* [in] */ Interface* interface);

    bool ParseMethod(
        /* [in] */ Interface* interface);

    bool ParseParameter(
        /* [in] */ Method* method);

    Type* ParseType();

    Type* ParseArrayType();

    bool ParseConstDataMember(
        /* [in] */ Interface* interface);

    Expression* ParseExpression(
        /* [in] */ Type* exprType);

    InclusiveOrExpression* ParseInclusiveOrExpression(
        /* [in] */ Type* exprType);

    ExclusiveOrExpression* ParseExclusiveOrExpression(
        /* [in] */ Type* exprType);

    AndExpression* ParseAndExpression(
        /* [in] */ Type* exprType);

    ShiftExpression* ParseShiftExpression(
        /* [in] */ Type* exprType);

    AdditiveExpression* ParseAdditiveExpression(
        /* [in] */ Type* exprType);

    MultiplicativeExpression* ParseMultiplicativeExpression(
        /* [in] */ Type* exprType);

    UnaryExpression* ParseUnaryExpression(
        /* [in] */ Type* exprType);

    PostfixExpression* ParsePostfixExpression(
        /* [in] */ Type* exprType);

    PostfixExpression* ParseIntegralNumber(
        /* [in] */ Type* exprType);

    PostfixExpression* ParseFloatingPointNumber(
        /* [in] */ Type* exprType);

    PostfixExpression* ParseBooleanLiteral(
        /* [in] */ Type* exprType);

    PostfixExpression* ParseCharacter(
        /* [in] */ Type* exprType);

    PostfixExpression* ParseStringLiteral(
        /* [in] */ Type* exprType);

    PostfixExpression* ParseIdentifier(
        /* [in] */ Type* exprType);

    bool ParseCoclass(
        /* [in] */ Attribute* attr);

    bool ParseCoclassBody(
        /* [in] */ Coclass* klass);

    bool ParseCoclassConstructor(
        /* [in] */ Coclass* klass);

    bool ParseCoclassInterface(
        /* [in] */ Coclass* klass);

    bool ParseEnumeration();

    bool ParseEnumerationBody(
        /* [in] */ Enumeration* enumeration);

    bool ParseInclude();

    bool ParseModule(
        /* [in] */ Attribute* attr);

    bool ParseNamespace();

    void EnterContext();

    void LeaveContext();

    Interface* FindInterface(
        /* [in] */ const String& itfName);

    Type* FindType(
        /* [in] */ const String& typeName);

    Type* CastType(
        /* [in] */ Type* type1,
        /* [in] */ Type* type2);

    void GenerateIInterface();

    void GenerateCoclassObject(
        /* [in] */ Coclass* klass);

    void LogError(
        /* [in] */ Tokenizer::Token token,
        /* [in] */ const String& message);

    void DumpError();

    void Dump();

public:
    static constexpr int MODE_CCMRT = 1;
    static constexpr int MODE_COMPONENT = 2;

    static constexpr int NOERROR = 0x0;
    static constexpr int E_FILE_NOT_FOUND_EXCEPTION = 0x1;

private:
    static const String TAG;
    int mMode;
    Tokenizer mTokenizer;
    String mPathPrefix;
    StringMap<bool> mParsedFiles;
    Environment* mEnvironment;
    std::shared_ptr<Module> mModule;
    Namespace* mCurrNamespace;
    Pool* mPool;
    Context* mCurrContext;
    int mStatus;
    Error* mErrorHeader;
    Error* mCurrError;
    bool mNeedDump;
};

}

#endif // __CCDL_PARSER_H__
