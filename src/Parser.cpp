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
#include "Logger.h"

namespace ccm {

const String Parser::TAG("Parser");

int Parser::Parse(
    /* [in] */ const std::shared_ptr<File>& file)
{
    mFile = file;

    if (!mFile->IsValid()) {
        Logger::E(TAG, "File %s is invalid.", mFile->GetPath().string());
        return E_FILE_NOT_FOUND_EXCEPTION;
    }

    mTokenizer.SetInputFile(file);
    return ParseFile();
}

int Parser::ParseFile()
{
    Tokenizer::Token token;
    while ((token = mTokenizer.GetToken()) != Tokenizer::Token::END_OF_FILE) {
        switch (token) {
            case Tokenizer::Token::BRACKETS_OPEN:
                ParseAttribute();
                continue;
            case Tokenizer::Token::COMMENT_BLOCK:
            case Tokenizer::Token::COMMENT_LINE:
                ParseComment(token);
                continue;
            case Tokenizer::Token::ENUM:
                ParseEnum();
                continue;
            case Tokenizer::Token::INTERFACE:
                ParseInterface();
                continue;
        }
    }
    return 0;
}

int Parser::ParseAttribute()
{
    // todo:
    return 0;
}

int Parser::ParseComment(
    /* [in] */ Tokenizer::Token token)
{
    // todo:
    return 0;
}

int Parser::ParseEnum()
{
    // todo:
    return 0;
}

int Parser::ParseInterface()
{
    // todo:
    return 0;
}

}
