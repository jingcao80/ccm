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

#include "ast/Attributes.h"
#include "parser/Phase.h"
#include "parser/Tokenizer.h"
#include "parser/World.h"
#include "util/AutoPtr.h"
#include "util/File.h"
#include "util/LightRefBase.h"
#include "util/String.h"
#include <vector>

namespace cdlc {

class Parser
{
private:
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

    void ParseConstant();

    void ParseExpression();

    void ParseInclusiveOrExpression();

    void ParseExclusiveOrExpression();

    void ParseAndExpression();

    void ParseShiftExpression();

    void ParseAdditiveExpression();

    void ParseMultiplicativeExpression();

    void ParseUnaryExpression();

    void ParsePostfixExpression();

    void ParseBooleanLiteral();

    void ParseCharacter();

    void ParseIntegralNumber();

    void ParseFloatingPointNumber();

    void ParseStringLiteral();

    void ParseIdentifier();

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


    void LogError(
        /* [in] */ TokenInfo& tokenInfo,
        /* [in] */ const String& message);

    void ShowErrors();

private:
    static const char* TAG;

    World mWorld;

    std::vector<AutoPtr<Phase>> mBeforePhases;
    std::vector<AutoPtr<Phase>> mAfterPhases;

    Tokenizer mTokenizer;
    std::vector<Error> mErrors;
};

}

#endif // __CDLC_PARSER_H__
