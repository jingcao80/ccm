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
#include "parser/Tokenizer.h"
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
        Token mToken = Token::UNKNOW;
        String mFile;
        int mLineNo = 0;
        int mColumnNo = 0;
        String mMessage;
    };

public:
    bool Parse(
        /* [in] */ const String& filePath);

private:
    bool ParseFile(
        /* [in] */ const String& filePath);

    bool ParseDeclarationWithAttributes();

    bool ParseAttributes(
        /* [out] */ Attributes& attrs);

    bool ParseUuid(
        /* [out] */ Attributes& attrs);

    bool ParseVersion(
        /* [out] */ Attributes& attrs);



    bool ParseModule();



    void LogError(
        /* [in] */ Token token,
        /* [in] */ const String& message);

    void ShowErrors();

private:
    static const char* TAG;

    Tokenizer mTokenizer;
    std::vector<Error> mErrors;
};

}

#endif // __CDLC_PARSER_H__
