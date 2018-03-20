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

#ifndef __CCM_PARSER_H__
#define __CCM_PARSER_H__

#include "Tokenizer.h"
#include "ccdl/AdditiveExpression.h"
#include "ccdl/AndExpression.h"
#include "ccdl/Attribute.h"
#include "ccdl/Component.h"
#include "ccdl/Enumeration.h"
#include "ccdl/ExclusiveOrExpression.h"
#include "ccdl/Expression.h"
#include "ccdl/InclusiveOrExpression.h"
#include "ccdl/Interface.h"
#include "ccdl/Method.h"
#include "ccdl/MultiplicativeExpression.h"
#include "ccdl/Namespace.h"
#include "ccdl/Parameter.h"
#include "ccdl/PostfixExpression.h"
#include "ccdl/ShiftExpression.h"
#include "ccdl/Type.h"
#include "ccdl/UnaryExpression.h"
#include "util/HashMap.h"

using ccm::ccdl::AdditiveExpression;
using ccm::ccdl::AndExpression;
using ccm::ccdl::Attribute;
using ccm::ccdl::Component;
using ccm::ccdl::Enumeration;
using ccm::ccdl::ExclusiveOrExpression;
using ccm::ccdl::Expression;
using ccm::ccdl::InclusiveOrExpression;
using ccm::ccdl::Interface;
using ccm::ccdl::Method;
using ccm::ccdl::MultiplicativeExpression;
using ccm::ccdl::Namespace;
using ccm::ccdl::Parameter;
using ccm::ccdl::PostfixExpression;
using ccm::ccdl::ShiftExpression;
using ccm::ccdl::Type;
using ccm::ccdl::UnaryExpression;

namespace ccm {

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

        HashMap<String> mPreDeclarations;
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
        : mParsedFiles(100)
        , mComponent(nullptr)
        , mCurrNamespace(nullptr)
        , mCurrContext(nullptr)
        , mStatus(NOERROR)
        , mErrorHeader(nullptr)
        , mCurrError(nullptr)
    {}

    bool Parse(
        /* [in] */ const String& filePath);

    ~Parser();

private:
    bool ParseFile();

    bool ParseDeclarationWithAttribute();

    bool ParseAttribute(
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

    bool ParseCoclass(
        /* [in] */ Attribute& attr);

    bool ParseEnumeration();

    bool ParseEnumerationBody(
        /* [in] */ Enumeration* enumeration);

    bool ParseInclude();

    bool ParseModule(
        /* [in] */ Attribute& attr);

    bool ParseNamespace();

    void EnterContext();

    void LeaveContext();

    Interface* FindBaseInterface(
        /* [in] */ const String& itfName);

    void LogError(
        /* [in] */ Tokenizer::Token token,
        /* [in] */ const String& message);
    void DumpError();

public:
    static constexpr int NOERROR = 0x0;
    static constexpr int E_FILE_NOT_FOUND_EXCEPTION = 0x1;

private:
    static const String TAG;
    Tokenizer mTokenizer;
    String mPathPrefix;
    HashMap<bool> mParsedFiles;
    Component* mComponent;
    Namespace* mCurrNamespace;
    Context* mCurrContext;
    int mStatus;
    Error* mErrorHeader;
    Error* mCurrError;
};

}

#endif // __CCM_PARSER_H__
