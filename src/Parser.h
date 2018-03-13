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
#include "ccdl/Component.h"
#include "ccdl/Enumeration.h"
#include "ccdl/Namespace.h"
#include "util/File.h"

#include <memory>

using ccm::ccdl::Component;
using ccm::ccdl::Enumeration;
using ccm::ccdl::Namespace;

namespace ccm {

class Parser
{
private:
    struct Error
    {
        Error()
            : mErrorToken(Tokenizer::Token::ILLEGAL_TOKEN)
            , mLineNo(0)
            , mColumnNo(0)
            , mNext(nullptr)
        {}

        Tokenizer::Token mErrorToken;
        int mLineNo;
        int mColumnNo;
        String mMessage;
        Error* mNext;
    };

public:
    Parser()
        : mFile(nullptr)
        , mComponent(nullptr)
        , mStatus(NOERROR)
        , mErrorHeader(nullptr)
        , mCurrError(nullptr)
    {}

    int Parse(
        /* [in] */ const std::shared_ptr<File>& file);

    ~Parser();

private:
    int ParseFile();

    bool ParseAttribute(
        /* [in] */ String* uuid,
        /* [in] */ String* version,
        /* [in] */ String* description);

    int ParseCoclass();

    bool ParseDeclarationWithAttribute();

    bool ParseEnumeration();

    bool ParseEnumerationBody(
        /* [in] */ Enumeration* enumeration);

    bool ParseExpression(
        /* [out] */ int* value);

    int ParseInclude();

    int ParseInterface();

    int ParseNamespace();

    void LogError(
        /* [in] */ Tokenizer::Token token,
        /* [in] */ const String& message);

public:
    static constexpr int NOERROR = 0x0;
    static constexpr int E_FILE_NOT_FOUND_EXCEPTION = 0x1;

private:
    static const String TAG;
    std::shared_ptr<File> mFile;
    Tokenizer mTokenizer;
    Component* mComponent;
    Namespace* mCurrNamespace;
    int mStatus;
    Error* mErrorHeader;
    Error* mCurrError;
};

}

#endif // __CCM_PARSER_H__
