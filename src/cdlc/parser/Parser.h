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

#ifndef __CDLC_PARSER_H__
#define __CDLC_PARSER_H__

#include "ast/AdditiveExpression.h"
#include "ast/AndExpression.h"
#include "ast/Attributes.h"
#include "ast/Constant.h"
#include "ast/ExclusiveOrExpression.h"
#include "ast/Expression.h"
#include "ast/InclusiveOrExpression.h"
#include "ast/InterfaceType.h"
#include "ast/MultiplicativeExpression.h"
#include "ast/Namespace.h"
#include "ast/PostfixExpression.h"
#include "ast/ShiftExpression.h"
#include "ast/UnaryExpression.h"
#include "parser/Phase.h"
#include "parser/Tokenizer.h"
#include "parser/World.h"
#include "util/AutoPtr.h"
#include "util/File.h"
#include "util/LightRefBase.h"
#include "util/String.h"
#include <unordered_map>
#include <vector>

namespace cdlc {

class Parser
{
private:
    class BlockContext
        : public LightRefBase
    {
    public:
        inline void AddTypeForwardDeclaration(
            /* [in] */ const String& typeName,
            /* [in] */ const String& fullTypeName)
        {
            mTypeForwardDeclarations[typeName] = fullTypeName;
        }

        inline String FindTypeForwardDeclaration(
            /* [in] */ const String& typeName)
        {
            auto it = mTypeForwardDeclarations.find(typeName);
            if (it != mTypeForwardDeclarations.end()) {
                return it->second;
            }
            return nullptr;
        }

    public:
        std::unordered_map<String, String, StringHashFunc, StringEqualsFunc>
        mTypeForwardDeclarations;

        AutoPtr<BlockContext> mNext;
    };

    class Error
    {
    public:
        Error() {}

        Error(
            /* [in] */ Error&& other)
            : mToken(other.mToken)
            , mFile(std::move(other.mFile))
            , mLineNo(other.mLineNo)
            , mColumnNo(other.mColumnNo)
            , mMessage(std::move(other.mMessage))
        {}

    public:
        Token mToken = Token::UNKNOWN;
        String mFile;
        int mLineNo = 0;
        int mColumnNo = 0;
        String mMessage;
    };

public:
    Parser();

    void AddPhase(
        /* [in] */ Phase* phase);

    bool Parse(
        /* [in] */ const String& filePath);

private:
    void Prepare();

    bool ParseFile(
        /* [in] */ const String& filePath);

    bool ParseFile(
        /* [in] */ TokenInfo tokenInfo);

    bool ParseDeclarationWithAttributes(
        /* [in] */ bool excludeModule);

    bool ParseAttributes(
        /* [out] */ Attributes& attrs);

    bool ParseUuid(
        /* [out] */ Attributes& attrs);

    bool ParseVersion(
        /* [out] */ Attributes& attrs);

    bool ParseDescription(
        /* [out] */ Attributes& attrs);

    bool ParseUri(
        /* [out] */ Attributes& attrs);

    bool ParseModule(
        /* [in] */ Attributes& attrs);

    bool ParseNamespace();

    bool ParseInterface(
        /* [in] */ Attributes& attrs);

    bool ParseInterfaceBody();

    AutoPtr<Constant> ParseConstant();

    AutoPtr<Expression> ParseExpression(
        /* [in] */ Type* type);

    AutoPtr<InclusiveOrExpression> ParseInclusiveOrExpression(
        /* [in] */ Type* type);

    AutoPtr<ExclusiveOrExpression> ParseExclusiveOrExpression(
        /* [in] */ Type* type);

    AutoPtr<AndExpression> ParseAndExpression(
        /* [in] */ Type* type);

    AutoPtr<ShiftExpression> ParseShiftExpression(
        /* [in] */ Type* type);

    AutoPtr<AdditiveExpression> ParseAdditiveExpression(
        /* [in] */ Type* type);

    AutoPtr<MultiplicativeExpression> ParseMultiplicativeExpression(
        /* [in] */ Type* type);

    AutoPtr<UnaryExpression> ParseUnaryExpression(
        /* [in] */ Type* type);

    AutoPtr<PostfixExpression> ParsePostfixExpression(
        /* [in] */ Type* type);

    AutoPtr<PostfixExpression> ParseBooleanLiteral(
        /* [in] */ Type* type);

    AutoPtr<PostfixExpression> ParseCharacter(
        /* [in] */ Type* type);

    AutoPtr<PostfixExpression> ParseIntegralNumber(
        /* [in] */ Type* type);

    AutoPtr<PostfixExpression> ParseFloatingPointNumber(
        /* [in] */ Type* type);

    AutoPtr<PostfixExpression> ParseStringLiteral(
        /* [in] */ Type* type);

    AutoPtr<PostfixExpression> ParseIdentifier(
        /* [in] */ Type* type);

    bool ParseMethod();

    bool ParseParameter();

    void ParseType();

    void ParseArray();

    bool ParseNestedInterface();

    bool ParseCoclass(
        /* [in] */ Attributes& attrs);

    bool ParseCoclassBody();

    bool ParseConstructor();

    bool ParseInterface(
        /* [in] */ void*);

    bool ParseEnumeration();

    bool ParseEnumerationBody();

    bool ParseInclude();

    void EnterBlockContext();

    void LeaveBlockContext();

    AutoPtr<Type> FindType(
        /* [in] */ const String& typeName);

    void LogError(
        /* [in] */ TokenInfo& tokenInfo,
        /* [in] */ const String& message);

    void ShowErrors();

private:
    static const char* TAG;

    World mWorld;
    AutoPtr<Module> mModule;
    AutoPtr<Namespace> mCurrentNamespace;
    AutoPtr<BlockContext> mCurrentContext;
    AutoPtr<Type> mCurrentType;

    std::vector<AutoPtr<Phase>> mBeforePhases;
    std::vector<AutoPtr<Phase>> mAfterPhases;

    Tokenizer mTokenizer;
    std::vector<Error> mErrors;
};

}

#endif // __CDLC_PARSER_H__
